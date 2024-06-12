/**
 * @class Block
 * @brief Block class, include own data, map, histogram, etc.
 *
 * @details Block类是探测器质控标定软件的核心类，该类既包含数据，又包含各种具体实现方法。
 * 其核心成员变量有电子学输出事件的位置信息和能量信息，晶体类对象，能谱，floodmap，分割结果，图像质量等等。
 * 其核心成员函数有 Fill(), CalMap(), Segment(), CalRecEHist(), 等等。
*/

#include "Block.h"

#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <QtCore>

#include "opencv2/imgproc.hpp"
#include "gsl/gsl_linalg.h"

/**
 * @brief 默认构造函数
 */
Block::Block()
{
	m_ID = 0;

	m_I0 = cv::Mat::zeros(nPixel, nPixel, CV_64FC1);
	// m_segr = cv::Mat::zeros(nPixel, nPixel, CV_16UC1);
	m_edge = cv::Mat::zeros(nPixel, nPixel, CV_8UC1);
	m_segMap = cv::Mat::zeros(nPixel, nPixel, CV_64FC1);

	m_pt = cv::Mat_<cv::Vec2w>(nCrystal - 2, nCrystal - 2);

	m_ADCHist = new Histogram(ADC_min, ADC_max, ADC_nBins);
	m_recEHist = new Histogram(recE_min, recE_max, recE_nBins);

	for (int i = 0; i < crystalNum; ++i)
	{
		Crystal* aCrystal = new Crystal(i);
		m_crystals.append(aCrystal);
	}

	CalGroupPar(nCrystal - 2);

	m_PVR = 0;
	m_FWHM = 0;
}

/**
 * @brief 带有参数 ID 的构造函数
 */
Block::Block(int ID)
{
	m_ID = ID;

	m_I0 = cv::Mat::zeros(nPixel, nPixel, CV_64FC1);
	// m_segr = cv::Mat::zeros(nPixel, nPixel, CV_16UC1);
	m_edge = cv::Mat::zeros(nPixel, nPixel, CV_8UC1);
	m_segMap = cv::Mat::zeros(nPixel, nPixel, CV_64FC1);

	m_pt = cv::Mat_<cv::Vec2w>(nCrystal - 2, nCrystal - 2);

	m_ADCHist = new Histogram(ADC_min, ADC_max, ADC_nBins);
	m_recEHist = new Histogram(recE_min, recE_max, recE_nBins);

	for (int i = 0; i < crystalNum; ++i)
	{
		Crystal* aCrystal = new Crystal(i);
		m_crystals.append(aCrystal);
	}

	CalGroupPar(nCrystal - 2);

	m_PVR = 0;
	m_FWHM = 0;
}

/**
 * @brief 析构函数
 */
Block::~Block()
{
	for (int i = 0; i < crystalNum; ++i)
	{
		delete m_crystals[i];
	}

	delete m_ADCHist;
	delete m_recEHist;
}

int Block::GetID()
{
	return m_ID;
}

void Block::SetID(int ID)
{
	m_ID = ID;
}

/**
 * @brief 调用此函数将属于该BK的数据填入成员变量中供后面调用，同时获得BK的ADC谱
 */
void Block::Fill(quint16 x, quint16 y, quint16 e)
{
	m_xList.append(x);
	m_yList.append(y);
	m_eList.append(e);
	m_ADCHist->Fill(e);
}

/**
 * @brief 根据能窗参数生成floodmap用于分割。
 * @details Floodmap的图像质量受到能窗的影响，不同能量的事例即使击中同一根晶体，
 * 对应在floomap中的位置也会有轻微变化，即“拖影”现象。所以为了得到清晰的floodmap，
 * 顺利地分割，应该是基于一定能窗范围去产生图像。
 */
void Block::CalMap(int EW_min, int EW_max)
{
	qDebug() << "start to calculate floodmap beteen energy window.";
	for (int i = 0; i < m_eList.size(); ++i)
	{
		quint16 x = m_xList[i];
		quint16 y = m_yList[i];
		quint16 e = m_eList[i];

		if (EW_min < e && e < EW_max)
		{
			m_I0(y, x) += 1;
		}
	}
}

/**
 * @brief 根据分割方案参数选择具体的分割函数。 方法1：SVD；方法2：寻找极大值。
 */
void Block::Segment(SegmentMethod method)
{
	switch (method)
	{
	case SegmentMethod::SVD:
		Segment1();
		break;
	case SegmentMethod::FindMaximum:
		Segment2();
		break;
	default:
		qDebug() << "Segment Method parameter must be 1 or 2.";
		break;
	}
}

/**
 * @details SVD分割方法。该方法具体步骤有2部分，第一部分为矩阵的奇异值分解用于生产所有peak的初始位置；
 * 第二部分为平均值移动算法，将初始位置迭代到正确位置。
 * 奇异值分解过程调用 GSL 库。首先将原始floodmap I0进行SVD分解，I0 = USVt。
 * 然后提取最大奇异值，S'(0, 0) = S(0, 0), S'其他元素为0，计算 I1  = US'Vt。
 * I1 图像能够保留所有peak的信息，同时滤掉背景，且所有peak呈规则排布，便于确定peak的位置。
 * 将 I1 分别在行和列方向求和，利用求和的结果寻峰，获取行/列峰位的初始位置。
 * 平均值移动过程分为 2 步，第一步将所有的peak分组，按组进行平均值移动迭代；第二步再对所有peak做小范围平均值移动迭代。
 * 参考文献：Q. Wei, T. Dai, T. Ma, Y. Liu, and Y. Gu, Crystal Identification in 
 * Dual-Layer-Offset DOI-PET Detectors Using Stratified Peak Tracking Based on SVD and Mean-Shift Algorithm, 
 * IEEE Transactions on Nuclear Science 63, 2502 (2016).
 */
void Block::Segment1()
{
	// 自动寻峰, 奇异值分解, 寻找规则峰位作为平均值移动算法的迭代初始位置
	// 平均值移动算法，寻找peaks

	qDebug() << "SVD start.";
	gsl_matrix* A = gsl_matrix_alloc(nPixel, nPixel);
	gsl_matrix* V = gsl_matrix_alloc(nPixel, nPixel);
	gsl_vector* S = gsl_vector_alloc(nPixel);
	gsl_vector* workspace = gsl_vector_alloc(nPixel);

	for (int i = 0; i < nPixel; ++i)
	{
		for (int j = 0; j < nPixel; ++j)
		{
			gsl_matrix_set(A, i, j, m_I0(i, j));
		}
	}

	gsl_linalg_SV_decomp(A, V, S, workspace);

	cv::Mat_<qreal> UMat = cv::Mat::zeros(nPixel, nPixel, CV_64FC1);
	cv::Mat_<qreal> SMat = cv::Mat::zeros(nPixel, nPixel, CV_64FC1);
	cv::Mat_<qreal> VMat = cv::Mat::zeros(nPixel, nPixel, CV_64FC1);

	for (int i = 0; i < nPixel; ++i)
	{
		for (int j = 0; j < nPixel; ++j)
		{
			UMat(i, j) = gsl_matrix_get(A, i, j);
			VMat(i, j) = gsl_matrix_get(V, i, j);
		}
	}

	SMat(0, 0) = gsl_vector_get(S, 0);

	cv::Mat I1 = UMat * SMat*VMat.t();

	gsl_matrix_free(A);
	gsl_matrix_free(V);
	gsl_vector_free(S);
	gsl_vector_free(workspace);
	qDebug() << "SVD done. ";

	cv::Mat col_sum, row_sum;
	cv::reduce(I1, col_sum, 0, cv::REDUCE_SUM);
	cv::reduce(I1, row_sum, 1, cv::REDUCE_SUM);

	QVector<qreal> colSumVec;
	QVector<qreal> rowSumVec;
	for (size_t i = 0; i < nPixel; i++)
	{
		colSumVec.append(col_sum.at<qreal>(i));
		rowSumVec.append(row_sum.at<qreal>(i));
	}

	QVector<int> peak_x_index = FindPeaks(colSumVec, nCrystal - 2);
	QVector<int> peak_y_index = FindPeaks(rowSumVec, nCrystal - 2);

	for (int i = 0; i < nCrystal - 2; ++i)
	{
		for (int j = 0; j < nCrystal - 2; ++j)
		{
			m_pt(i, j)[0] = peak_x_index[j];
			m_pt(i, j)[1] = peak_y_index[i];
		}
	}
	qDebug() << QString::fromLocal8Bit("所有晶体初始位置设置完毕。");

	// step1，每组整体做平均值移动
	for (int iRow = 0; iRow < m_num1; ++iRow)
	{
		for (int iCol = 0; iCol < m_num1; ++iCol)
		{
			// 求 4 个点的重心的初始位置
			cv::Point2f c0(0, 0);
			for (int i = 0; i < m_num2; ++i)
			{
				for (int j = 0; j < m_num2; ++j)
				{
					c0 += cv::Point2f(m_pt(iRow*m_num2 + i, iCol*m_num2 + j)[0],
						m_pt(iRow*m_num2 + i, iCol*m_num2 + j)[1]);
				}
			}

			cv::Point2f c = c0 / (m_num2*m_num2); // 初始位置
			cv::Point2f pre_c(0, 0);

			while (cv::norm(pre_c - c) > 1)  // 迭代移动重心
			{
				pre_c = c;
				c = cv::Point2f(0, 0);

				// 计算重心
				for (int i = 0; i < m_num2; ++i)
				{
					for (int j = 0; j < m_num2; ++j)
					{
						cv::Point2f x(m_pt(iRow*m_num2 + i, iCol*m_num2 + j)[0],
							m_pt(iRow*m_num2 + i, iCol*m_num2 + j)[1]);

						// 累加项 // 附近的点进行累加
						cv::Point2f u(0, 0);
						qreal d = 0;
						int r = 6; // 累加范围
						for (int n = x.y - r; n <= x.y + r; ++n)
						{
							for (int m = x.x - r; m <= x.x + r; ++m)
							{
								if (cv::norm(x - cv::Point2f(m, n)) <= r)
								{
									u += m_I0(n, m)*cv::Point2f(m, n);
									d += m_I0(n, m);
								}
							}
						}

						if (d == 0)
						{
							qDebug() << "迭代过程中，分母 d = 0 !!! Bug !!!";
						}
						else
						{
							c += u / d;
						}
					}
				}

				c = c / (m_num2*m_num2);

				for (int i = 0; i < m_num2; ++i)
				{
					for (int j = 0; j < m_num2; ++j)
					{
						m_pt(iRow*m_num2 + i, iCol*m_num2 + j)[0] += qRound(c.x - pre_c.x);
						m_pt(iRow*m_num2 + i, iCol*m_num2 + j)[1] += qRound(c.y - pre_c.y);
					}
				}
			}
		}
	}
	qDebug() << QString::fromLocal8Bit("第一次平均值移动迭代，每组峰位移动完成。");

	// step2, all peak 做平均值移动, 使得每个峰做小范围移动
	for (int iRow = 0; iRow < nCrystal - 2; ++iRow)
	{
		for (int iCol = 0; iCol < nCrystal - 2; ++iCol)
		{
			cv::Point2f x(m_pt(iRow, iCol)[0], m_pt(iRow, iCol)[1]);
			cv::Point2f pre_x(0, 0);
			while (cv::norm(pre_x - x) > 0.1)
			{
				pre_x = x;
				cv::Point2f u(0, 0);
				qreal d = 0;

				int r = 3;
				for (int n = x.y - r; n <= x.y + r; ++n)
				{
					for (int m = x.x - r; m <= x.x + r; ++m)
					{
						if (cv::norm(x - cv::Point2f(m, n)) <= r)
						{
							//qreal k = cv::norm(x-cv::Point2f(m, n))/pow((2*r+1), 2);
							//qreal g = (1/sqrt(2*M_PI))*exp(-0.5*k);
							qreal g = 1;
							u += m_I0(n, m)*g*cv::Point2f(m, n);
							d += m_I0(n, m)*g;
						}
					}
				}

				if (d == 0)
				{
					qDebug() << "迭代过程中，分母 d = 0 !!! Bug !!!";
				}
				else
				{
					x = u / d;
				}
			}
			m_pt(iRow, iCol)[0] = qRound(x.x);
			m_pt(iRow, iCol)[1] = qRound(x.y);
		}
	}
	qDebug() << QString::fromLocal8Bit("第二次平均值移动迭代。所有峰位寻找完成。");
}


/**
 * @details 该方法直接调用opencv库函数寻找图像中的极大值点，
 * 然后将该点附近像素置为0，继续寻找下一个极大值点，直至找到所有峰位。
 * 找到所有峰位后，他们的顺序是混乱的，需要对他们进行排序。排序方法为先按行排序，然后按列排序。
 * 具体为将所有的点按 y 值排序，每 nCrystal 个点为一组，第0组为第0行，第1组为第1行，...
 * 每组内的点再按 x 值 排序，对应所有列。
 */
void Block::Segment2() // maximum method to find peaks
{
	// 使用 OpenCV 的平滑函数对数据进行平滑处理
	cv::Mat_<qreal> I1;
	cv::blur(m_I0, I1, cv::Size(5, 5));

	//cv::Mat_<qreal> I2;
	//cv::GaussianBlur(I1, I2, cv::Size(3, 3), 1, 1);
	//I1 = I2;

	QVector<cv::Point> peaks;
	int nPeaks = (nCrystal - 2) * (nCrystal - 2);
	for (int i = 0; i < nPeaks; ++i)
	{
		qreal minVal, maxVal;
		cv::Point minLoc, maxLoc;
		cv::minMaxLoc(I1, &minVal, &maxVal, &minLoc, &maxLoc);

		peaks.append(maxLoc);

		// 将maxLoc附近的元素置为0
		int m0 = maxLoc.x;
		int n0 = maxLoc.y;
		int r = 6;

		for (int n = n0 - r; n <= n0 + r; ++n)
		{
			for (int m = m0 - r; m <= m0 + r; ++m)
			{
				if (cv::norm(maxLoc - cv::Point(m, n)) <= r)
				{
					I1(n, m) = 0;
				}
			}
		}
	}

	// sort peaks
	for (int iRow = 0; iRow < nCrystal - 2; ++iRow)
	{
		// 选出 y 值较小的 m_nCrystal 个点作为一行， y值升序排列
		std::sort(peaks.begin(), peaks.end(),
			[](const cv::Point& p1, const cv::Point& p2) {return p1.y < p2.y; });

		QVector<cv::Point> rowPeaks;
		for (size_t i = 0; i < nCrystal - 2; i++)
		{
			rowPeaks.append(peaks[i]);
		}

		// 将他们按 x 值升序排列
		std::sort(rowPeaks.begin(), rowPeaks.end(),
			[](const cv::Point& p1, const cv::Point& p2) {return p1.x < p2.x; });

		for (int iCol = 0; iCol < nCrystal - 2; ++iCol)
		{
			peaks.pop_front();
			m_pt(iRow, iCol)[0] = rowPeaks[iCol].x;
			m_pt(iRow, iCol)[1] = rowPeaks[iCol].y;
		}
	}
}

/**
 * @brief 自动寻峰可能存在错误，此时需要进行手动调整。对于输入的位置信息，
 * 计算该位置与所有peak的距离，将该位置信息赋予离它最近的peak。
 */
void Block::ManualAdjust(QPoint pos)
{
	cv::Mat_<qreal> dis(nCrystal - 2, nCrystal - 2, 0.0);
	for (int i = 0; i < nCrystal - 2; ++i)
	{
		for (int j = 0; j < nCrystal - 2; ++j)
		{
			cv::Point2f p1(m_pt(i, j)[0], m_pt(i, j)[1]);
			cv::Point2f p2(pos.x(), pos.y());
			dis(i, j) = cv::norm(p1 - p2);
		}
	}

	cv::Point min_loc;
	cv::minMaxLoc(dis, NULL, NULL, &min_loc, NULL);

	m_pt(min_loc)[0] = pos.x();
	m_pt(min_loc)[1] = pos.y();
}

/**
 * @brief 计算分割结果。
 * @details 在获取了所有peak的信息后，我们需要计算floodmap中像素对晶体编号的映射。
 * 判断的依据是距离，该像素离哪个peak最近，则属于哪个peak。
 * B4 探测器的晶体阵列为26*26，边缘为长条状2-to-1，角落为4-to-1，为了便于后面图像重建算法调用，
 * 我将生产28*28晶体的位置映射表。边缘晶体的事例会有 50% 随机映射到新的晶体编号中去。
 */
void Block::CalSegResult()
{
	//step 4, 开始分割, //计算像素点属于哪个峰
	seg1 = cv::Mat_<quint16>(nPixel, nPixel); // 行信息
	seg2 = cv::Mat_<quint16>(nPixel, nPixel); // 列信息

	seg1_plus = cv::Mat_<quint16>(nPixel, nPixel); // 行信息
	seg2_plus = cv::Mat_<quint16>(nPixel, nPixel); // 列信息

	int flag = 2; // 1, random method; 2, line-cut method

	if (1 == flag)
	{
		srand(time(0));
		for (int i = 0; i < nPixel; ++i)
		{
			for (int j = 0; j < nPixel; ++j)
			{
				cv::Mat_<qreal> dis(nCrystal - 2, nCrystal - 2);

				for (int iRow = 0; iRow < nCrystal - 2; ++iRow)
				{
					for (int iCol = 0; iCol < nCrystal - 2; ++iCol)
					{
						cv::Point2f p(m_pt(iRow, iCol)[0], m_pt(iRow, iCol)[1]);
						dis(iRow, iCol) = cv::norm(cv::Point2f(j, i) - p);
					}
				}

				cv::Point min_loc;
				cv::minMaxLoc(dis, NULL, NULL, &min_loc, NULL);

				int rowID = min_loc.y;
				int colID = min_loc.x;

				seg1(i, j) = rowID;
				seg2(i, j) = colID;


				// 修正26=>28
				if (0 == rowID)
				{
					// 0 ==> 50% is 0, 50% is 1
					int aN = rand() % 100;
					if (aN < 50)
					{
						rowID++;
					}
				}
				else if ((nCrystal - 3) == rowID)
				{
					// 25 ==> 50% is 26, 50% is 27
					int aN = rand() % 100;
					if (aN < 50)
					{
						rowID += 1;
					}
					else
					{
						rowID += 2;
					}
				}
				else
				{
					rowID++; // 1==>2, ..., 24==>25
				}

				if (0 == colID)
				{
					// 0 ==> 50% is 0, 50% is 1
					int aN = rand() % 100;
					if (aN < 50)
					{
						colID++;
					}
				}
				else if ((nCrystal - 3) == colID)
				{
					// 25 ==> 50% is 26, 50% is 27
					int aN = rand() % 100;
					if (aN < 50)
					{
						colID += 1;
					}
					else
					{
						colID += 2;
					}
				}
				else
				{
					colID++; // 1==>2, ..., 24==>25
				}

				seg1_plus(i, j) = rowID;
				seg2_plus(i, j) = colID;
			}
		}

		m_segr0 = seg1 * (nCrystal - 2) + seg2;
		m_segr1 = seg1_plus * nCrystal + seg2_plus;
	}

	if (2 == flag)
	{
		cv::Mat_<qreal> uLineMat(1, nCrystal - 2); // up line
		cv::Mat_<qreal> bLineMat(1, nCrystal - 2); // bottom line
		cv::Mat_<qreal> lLineMat(1, nCrystal - 2); // left line
		cv::Mat_<qreal> rLineMat(1, nCrystal - 2); // right line

		for (int i = 0; i < nCrystal - 2; ++i)
		{
			uLineMat(0, i) = m_pt(0, i)[1]; // row index, y
			bLineMat(0, i) = m_pt(nCrystal - 3, i)[1]; // row index, y
			lLineMat(0, i) = m_pt(i, 0)[0]; // col index, x
			rLineMat(0, i) = m_pt(i, nCrystal - 3)[0]; // col index, x
		}

		cv::Mat_<qreal> resizedUMat;
		cv::Mat_<qreal> resizedBMat;
		cv::Mat_<qreal> resizedLMat;
		cv::Mat_<qreal> resizedRMat;

		cv::resize(uLineMat, resizedUMat, cv::Size(nPixel, 1), 0, 0, cv::INTER_LINEAR);
		cv::resize(bLineMat, resizedBMat, cv::Size(nPixel, 1), 0, 0, cv::INTER_LINEAR);
		cv::resize(lLineMat, resizedLMat, cv::Size(nPixel, 1), 0, 0, cv::INTER_LINEAR);
		cv::resize(rLineMat, resizedRMat, cv::Size(nPixel, 1), 0, 0, cv::INTER_LINEAR);

		for (int i = 0; i < nPixel; ++i)
		{
			for (int j = 0; j < nPixel; ++j)
			{
				cv::Mat_<qreal> dis(nCrystal - 2, nCrystal - 2);

				for (int iRow = 0; iRow < nCrystal - 2; ++iRow)
				{
					for (int iCol = 0; iCol < nCrystal - 2; ++iCol)
					{
						cv::Point2f p(m_pt(iRow, iCol)[0], m_pt(iRow, iCol)[1]);
						dis(iRow, iCol) = cv::norm(cv::Point2f(j, i) - p);
					}
				}

				cv::Point min_loc;
				cv::minMaxLoc(dis, NULL, NULL, &min_loc, NULL);

				int rowID = min_loc.y + 1;
				int colID = min_loc.x + 1;

				int uBorder = qRound(resizedUMat(0, j));
				int bBorder = qRound(resizedBMat(0, j));
				int lBorder = qRound(resizedLMat(0, i));
				int rBorder = qRound(resizedRMat(0, i));

				if (i < uBorder)
				{
					rowID = 0;
				}
				else if (i > bBorder)
				{
					rowID = nCrystal - 1;
				}

				if (j < lBorder)
				{
					colID = 0;
				}
				else if (j > rBorder)
				{
					colID = nCrystal - 1;
				}

				seg1(i, j) = rowID;
				seg2(i, j) = colID;
			}
		}

		m_segr0 = seg1 * nCrystal + seg2;
		m_segr1 = m_segr0;
	}

	m_edge = cv::Mat::zeros(nPixel, nPixel, CV_8UC1);
	m_segMap = m_I0.clone();

	qreal max_val;
	cv::minMaxLoc(m_I0, NULL, &max_val, NULL, NULL);

	for (int i = 1; i < nPixel - 1; ++i)
	{
		for (int j = 1; j < nPixel - 1; ++j)
		{
			int id0 = m_segr0(i, j);
			int id1 = m_segr0(i + 1, j);
			int id2 = m_segr0(i, j + 1);
			if (id0 != id1 || id0 != id2)
			{
				m_edge(i, j) = 1;
				m_segMap(i, j) = max_val;
			}
		}
	}

	// 将每个BK 的分割结果保存成文件，以便后面调用
	// 这样做的必要性是因为所有BK分割完才能生成整台机器的位置查找表
	// 这样做还有一个优点，如果某个BK分割结果不理想，
	// 后期还可以单独分割这个BK，然后生成位置查找表
	QString fileName = "./Data/segResult.xml";

	// 创建文件存储对象
	cv::FileStorage fs(fileName.toStdString(), cv::FileStorage::WRITE);
	fs.write("floodmap", m_I0); // 将 I0 写入
	fs.write("edge", m_edge);   // 写入边界结果
	fs.write("segr", m_segr1);   // 写入分割结果
	fs.release();  // 关闭文件
}

/**
 * @details 位置查找表生成后，就可以对单根晶体性能进行计算，此时将BK中保存的原始事例信息按映射表分配给对应的晶体。
 * 同时让所有晶体进行寻峰、计算 ADC 到 keV 的转换系数，计算每个晶体的能谱。
 */
void Block::CalRecEHist()
{
	for (int i = 0; i < m_eList.size(); ++i)
	{
		quint16 x = m_xList[i];
		quint16 y = m_yList[i];
		quint16 e = m_eList[i];

		quint16 ID = m_segr1(y, x);
		m_crystals[ID]->Fill(e);
	}

	for (int i = 0; i < m_crystals.size(); ++i)
	{
		auto aCrystal = m_crystals[i];
		aCrystal->CalRecEHist();
		auto iRecEHist = aCrystal->GetRecEHist();
		iRecEHist->SetCutValue(recE_cutValue);
		m_recEHist->Add(iRecEHist);
	}
}

/**
 * @brief 计算图像质量参数。
 * @details 图像质量参数为PVR和FWHM。PVR为峰谷比，即计算所有peak的平均高度 hp, 
 * 和所有晶体交界处的高度 hv，PVR = hp/hv。
 * FWHM为半高宽，所有peak的平均半高宽。每个peak的半高宽计算过程中分别计算了x 方向和 y 方向两个方向的半高宽，
 * 然后进行了平均。每次计算半高宽时又进行了平滑和插值。具体过程见 CalFWHM() 函数。
 */
void Block::CalSegFOM()
{
	int nPeaks = (nCrystal - 4) * (nCrystal - 4);
	// step 5, 求分割质量参数
	// calculate peaks mean-height

	//cv::Mat I1;
	//cv::blur(m_I0, I1, cv::Size(3, 3));

	qreal peak_total_evts = 0;
	qreal totalFWHM = 0;
	for (int i = 1; i < nCrystal - 3; ++i) // 0--25, 1--24
	{
		for (int j = 1; j < nCrystal - 3; ++j)
		{
			int x = m_pt(i, j)[0];
			int y = m_pt(i, j)[1];
			peak_total_evts += m_I0(y, x);

			// reduce 3*15 matrix to line, then find FWHM
			// x direction
			cv::Mat M1 = m_I0(cv::Rect(x - 7, y - 1, 15, 3));
			qreal FWHM_x = CalFWHM(M1);

			// y direction
			cv::Mat M2 = m_I0(cv::Rect(x - 1, y - 7, 3, 15));
			qreal FWHM_y = CalFWHM(M2.t());

			qreal FWHM_xy = (FWHM_x + FWHM_y) / 2;
			totalFWHM += FWHM_xy;
		}
	}
	qreal peak_mean_height = peak_total_evts / nPeaks;

	// valley mean-height
	quint64 edgeEvts = 0;
	quint64 edgeN = 0;
	for (int i = 0; i < nPixel; ++i)
	{
		for (int j = 0; j < nPixel; ++j)
		{
			if (ymin < i && i < ymax && xmin < j && j < xmax)
			{
				int aN = static_cast<int>(m_I0(i, j));
				if (1 == m_edge(i, j))
				{
					edgeN++;
					edgeEvts += aN;
				}
			}
		}
	}

	//
	// magic here !!!
	//
	cv::Mat_<qreal> uLineMat(1, nCrystal - 2); // up line
	cv::Mat_<qreal> bLineMat(1, nCrystal - 2); // bottom line
	cv::Mat_<qreal> lLineMat(1, nCrystal - 2); // left line
	cv::Mat_<qreal> rLineMat(1, nCrystal - 2); // right line

	for (int i = 0; i < nCrystal - 2; ++i)
	{
		uLineMat(0, i) = m_pt(1, i)[1]; // row index, y
		bLineMat(0, i) = m_pt(nCrystal - 4, i)[1]; // row index, y
		lLineMat(0, i) = m_pt(i, 1)[0]; // col index, x
		rLineMat(0, i) = m_pt(i, nCrystal - 4)[0]; // col index, x
	}

	cv::Mat_<qreal> resizedUMat;
	cv::Mat_<qreal> resizedBMat;
	cv::Mat_<qreal> resizedLMat;
	cv::Mat_<qreal> resizedRMat;

	cv::resize(uLineMat, resizedUMat, cv::Size(nPixel, 1), 0, 0, cv::INTER_LINEAR);
	cv::resize(bLineMat, resizedBMat, cv::Size(nPixel, 1), 0, 0, cv::INTER_LINEAR);
	cv::resize(lLineMat, resizedLMat, cv::Size(nPixel, 1), 0, 0, cv::INTER_LINEAR);
	cv::resize(rLineMat, resizedRMat, cv::Size(nPixel, 1), 0, 0, cv::INTER_LINEAR);

	for (int i = 0; i < nPixel; ++i)
	{
		for (int j = 0; j < nPixel; ++j)
		{
			cv::Mat_<qreal> dis(nCrystal - 2, nCrystal - 2);

			for (int iRow = 0; iRow < nCrystal - 2; ++iRow)
			{
				for (int iCol = 0; iCol < nCrystal - 2; ++iCol)
				{
					cv::Point2f p(m_pt(iRow, iCol)[0], m_pt(iRow, iCol)[1]);
					dis(iRow, iCol) = cv::norm(cv::Point2f(j, i) - p);
				}
			}

			cv::Point min_loc;
			cv::minMaxLoc(dis, NULL, NULL, &min_loc, NULL);

			int rowID = min_loc.y + 1;
			int colID = min_loc.x + 1;

			int uBorder = qRound(resizedUMat(0, j));
			int bBorder = qRound(resizedBMat(0, j));
			int lBorder = qRound(resizedLMat(0, i));
			int rBorder = qRound(resizedRMat(0, i));

			qreal valley_mean_height = 1.0*edgeEvts / edgeN;

			m_PVR = peak_mean_height / valley_mean_height;
			m_FWHM = totalFWHM / nPeaks;
		}
	}
}

void Block::GenPositionLUT()
{
	//根据分割结果，生成位置查找表
	QFile file(currentPath + fName_LUT_P);
	if (!file.open(QIODevice::WriteOnly))
	{
		qDebug() << "打开文件失败，无法生成位置查找表。";
		return;
	}

	QDataStream out(&file);

	// 写入矩阵数据
	for (int i = 0; i < nPixel; ++i)
	{
		for (int j = 0; j < nPixel; ++j)
		{
			quint16 rowID = seg1_plus(i, j);
			quint16 colID = seg2_plus(i, j);
			out << colID << rowID;
		}
	}

	file.close();
	qDebug() << QString::fromLocal8Bit("位置查找表已生成: ") + fName_LUT_P;
}


void Block::GenEnergyLUT()
{
	QFile file(currentPath + fName_LUT_E);
	if (!file.open(QIODevice::WriteOnly))
	{
		qDebug() << QString::fromLocal8Bit("打开文件失败，无法生成能量查找表。");
		return;
	}

	QDataStream outStream(&file);

	//qreal sumSlope = 0;
	for (int i = 0; i < crystalNum; ++i)
	{
		qreal slope = m_crystals[i]->GetSlope();
		outStream << slope;
		//sumSlope += slope;
		//qDebug() << "crystal " << i << " , slope = " << slope;
	}
	//qDebug() << "mean of slope = " << sumSlope/crystalNum;

	file.close();
	qDebug() << QString::fromLocal8Bit("能量查找表已生成: ") + fName_LUT_E;
}

void Block::GenUniformityLUT()
{
	cv::Mat_<qreal> nEvts = cv::Mat::zeros(nCrystal, nCrystal, CV_64FC1);

	int totalEvts = 0;
	for (int iRow = 0; iRow < nCrystal; ++iRow)
	{
		for (int iCol = 0; iCol < nCrystal; ++iCol)
		{
			int id = iRow * nCrystal + iCol;
			int N = m_crystals[id]->GetEntries(350, 650);
			nEvts(iRow, iCol) = N;
			totalEvts += N;
		}
	}

	// 获取中心的 24x24 区域
	cv::Mat_<qreal> nEvts_center = nEvts(cv::Rect(2, 2, 24, 24));

	// 计算最大值和最小值
	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	cv::minMaxLoc(nEvts_center, &minVal, &maxVal, &minLoc, &maxLoc);

	// 计算平均值和方差
	cv::Scalar mean, stdDev;
	cv::meanStdDev(nEvts_center, mean, stdDev);
	qreal meanVal = mean.val[0];
	qreal stdDevVal = stdDev.val[0];

	m_uniformity = nEvts / meanVal;
	m_uMax = maxVal / meanVal;
	m_uMin = minVal / meanVal;
	m_uMean = meanVal;
	m_uSD = stdDevVal / meanVal;

	// 输出统计信息
	qDebug() << "max value: " << m_uMax;
	qDebug() << "min value: " << m_uMin;
	qDebug() << "mean value: " << m_uMean;
	qDebug() << "standard deviation: " << m_uSD;
}

cv::Mat_<qreal> Block::GetMap()
{
	return m_I0;
}

Crystal* Block::GetCrystal(int ID)
{
	return m_crystals[ID];
}

Histogram* Block::GetADCHist()
{
	return m_ADCHist;
}

Histogram* Block::GetRecEHist()
{
	return m_recEHist;
}

cv::Mat_<cv::Vec2w> Block::GetPeakTable()
{
	return m_pt;
}

cv::Mat_<quint16> Block::GetSegResult(int n)
{
	if (0 == n)
	{
		return m_segr0;
	}
	else if (1 == n)
	{
		return m_segr1;
	}
	else
	{
		qDebug() << "input parameter is wrong.";
		return m_segr0;
	}
}

cv::Mat_<qreal> Block::GetSegMap()
{
	return m_segMap;
}

qreal Block::GetER()
{
	auto h = m_recEHist->Clone();
	h->SetCutValue(recE_cutValue);
	h->Smooth();
	return h->GetResolution();
}

qreal Block::GetPVR()
{
	return m_PVR;
}

qreal Block::GetFWHM()
{
	return m_FWHM;
}

qreal Block::CalFWHM(cv::Mat I)
{
	cv::Mat col_sum;
	cv::reduce(I, col_sum, 0, cv::REDUCE_SUM);

	cv::Mat resizedMat;
	cv::resize(col_sum, resizedMat,
		cv::Size(I.cols * 10, 1), 0, 0, cv::INTER_LINEAR);

	cv::Mat blurredMat;
	cv::blur(resizedMat, blurredMat, cv::Size(10, 1));

	QVector<qreal> v;			
	for (size_t i = 0; i < blurredMat.elemSize(); i++)
	{
		v.append(blurredMat.at<qreal>(i));
	}

	int peakIdx = std::max_element(v.begin(), v.end()) - v.begin();
	qreal peakValue = v[peakIdx];

	qreal halfMax = peakValue / 2;
	int leftIdx = peakIdx;
	int rightIdx = peakIdx;

	while (leftIdx > 0 && v[leftIdx] > halfMax)
	{
		leftIdx--;
	}

	while (rightIdx<v.size() - 1 && v[rightIdx]>halfMax)
	{
		rightIdx++;
	}

	qreal FWHM = (rightIdx - leftIdx) / 10.0; // 半高宽
	return FWHM;
}

void Block::Clear()
{
	m_xList.clear();
	m_yList.clear();
	m_eList.clear();

	for (int i = 0; i < m_crystals.size(); ++i)
	{
		m_crystals[i]->Clear();
	}

	m_ADCHist->Clear();
	m_recEHist->Clear();

	m_I0.setTo(0);
	m_pt.setTo(cv::Vec2w(0, 0));
	m_segr0.setTo(0);
	m_segr1.setTo(0);
	m_edge.setTo(0);
	m_segMap.setTo(0);
}

void Block::CalGroupPar(int N)
{
	int n1 = 0;
	int n2 = 0;
	int minDiff = N; // 初始最小差值为 n

	// 在 2 到 sqrt(N) 之间搜索因子
	for (int i = 2; i <= qSqrt(N); ++i)
	{
		if (N % i == 0)
		{
			int factor1 = i;
			int factor2 = N / i;
			int diff = qAbs(factor2 - factor1); // 计算差值

			// 如果当前差值更小，则更新最小差值和对应的因子
			if (diff < minDiff)
			{
				minDiff = diff;
				n1 = factor1;
				n2 = factor2;
			}
		}
	}

	m_num1 = n1;
	m_num2 = n2;

	qDebug() << "SVD method need two parameters to group peaks, n1 = "
		<< n1 << ", n2 = " << n2;
}

QVector<int> Block::FindPeaks(QVector<qreal> v, int nPeaks)
{
	QVector<int> peaks;
	for (int iPeak = 0; iPeak < nPeaks; ++iPeak)
	{
		int maxIdx = std::max_element(v.begin(), v.end()) - v.begin();
		peaks.append(maxIdx);

		int r = 5;
		int start = std::max(maxIdx - r, 0);
		int stop = std::min(maxIdx + r, nPixel - 1);
		for (int i = start; i <= stop; ++i)
		{
			v[i] = 0;
		}
	}

	std::sort(peaks.begin(), peaks.end());
	return peaks;
}

cv::Mat_<qreal> Block::GetUniformity()
{
	return m_uniformity;
}

QVector<qreal> Block::GetUStatistics()
{
	QVector<qreal> answer;
	answer.push_back(m_uMax);
	answer.push_back(m_uMin);
	answer.push_back(m_uMean);
	answer.push_back(m_uSD);
	return answer;
}