// @class Block
// @brief Block class, include own data, map, histogram, etc.
// @details Block类是探测器质控标定软件的核心类，该类既包含数据，又包含各种具体实现方法。
// 其核心成员变量有电子学输出事件的位置信息和能量信息，晶体类对象，能谱，floodmap，分割结果，图像质量等等。
// 其核心成员函数有 Fill(), CalMap(), Segment(), CalRecEHist(), 等等。
#include "Block.h"
#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <QtCore>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "gsl/gsl_linalg.h"
#include <algorithm>
#include <stdexcept>

// @brief 默认构造函数
Block::Block()
{
	m_ID = 0;

	m_I0 = cv::Mat::zeros(nPixel, nPixel, CV_64FC1);

	m_pt = cv::Mat_<cv::Vec2w>(nCrystal, nCrystal);

	m_segr = cv::Mat::zeros(nPixel, nPixel, CV_16UC1);
	m_segr_plus = cv::Mat::zeros(nPixel, nPixel, CV_16UC1);
	m_edge = cv::Mat::zeros(nPixel, nPixel, CV_8UC1);
	m_segMap = cv::Mat::zeros(nPixel, nPixel, CV_64FC1);

	m_ADCHist = new Histogram(ADC_min, ADC_max, ADC_nBins);
	m_recEHist = new Histogram(recE_min, recE_max, recE_nBins);

	for (int i = 0; i < crystalNum; ++i)
	{
		Crystal* aCrystal = new Crystal(i);
		m_crystals.append(aCrystal);
	}

	m_num1 = 2;
	m_num2 = 13;

	m_PVR = 0;
	m_FWHM = 0;

	m_uMax = 0;
	m_uMin = 0;
	m_uMean = 0;
	m_uSD = 0;
}

/**
 * @brief 带有参数 ID 的构造函数
 */
Block::Block(int ID)
{
	m_ID = ID;

	m_I0 = cv::Mat::zeros(nPixel, nPixel, CV_64FC1);

	m_pt = cv::Mat_<cv::Vec2w>(nCrystal, nCrystal);

	m_segr = cv::Mat::zeros(nPixel, nPixel, CV_16UC1);
	m_segr_plus = cv::Mat::zeros(nPixel, nPixel, CV_16UC1);
	m_edge = cv::Mat::zeros(nPixel, nPixel, CV_8UC1);
	m_segMap = cv::Mat::zeros(nPixel, nPixel, CV_64FC1);

	m_ADCHist = new Histogram(ADC_min, ADC_max, ADC_nBins);
	m_recEHist = new Histogram(recE_min, recE_max, recE_nBins);

	for (int i = 0; i < crystalNum; ++i)
	{
		Crystal* aCrystal = new Crystal(i);
		m_crystals.append(aCrystal);
	}

	m_num1 = 2;
	m_num2 = 13;

	m_PVR = 0;
	m_FWHM = 0;

	m_uMax = 0;
	m_uMin = 0;
	m_uMean = 0;
	m_uSD = 0;
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

int Block::GetID() const
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
	qDebug() << "start to calculate floodmap in energy window.";
	m_I0.setTo(0);
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
 * 平均值移动过程分为 2 步，第一步将所有的peak分组，按组进行平均值移动迭代；
 * 第二步再对所有peak做小范围平均值移动迭代。
 * 参考文献：Q. Wei, T. Dai, T. Ma, Y. Liu, and Y. Gu, Crystal Identification in
 * Dual-Layer-Offset DOI-PET Detectors Using Stratified Peak Tracking Based on SVD and Mean-Shift Algorithm,
 * IEEE Transactions on Nuclear Science 63, 2502 (2016).
 */
void Block::Segment1()
{
	// 自动寻峰, 奇异值分解, 寻找规则峰位作为平均值移动算法的迭代初始位置
	// 平均值移动算法，寻找peaks
	cv::Mat_<qreal> I0 = m_I0.clone();

	cv::Mat temp;
	cv::GaussianBlur(I0, temp, cv::Size(5, 5), 1, 1);
	cv::Laplacian(temp, I0, CV_64F, 5);

	I0 = I0 * -100;
	I0.setTo(1.e-5, I0 < 2);

	m_I0 = I0;

	qDebug() << "SVD start.";
	gsl_matrix* A = gsl_matrix_alloc(nPixel, nPixel);
	gsl_matrix* V = gsl_matrix_alloc(nPixel, nPixel);
	gsl_vector* S = gsl_vector_alloc(nPixel);
	gsl_vector* workspace = gsl_vector_alloc(nPixel);

	for (int i = 0; i < nPixel; ++i)
	{
		for (int j = 0; j < nPixel; ++j)
		{
			gsl_matrix_set(A, i, j, I0(i, j));
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

	cv::Mat I1 = UMat * SMat * VMat.t();

	gsl_matrix_free(A);
	gsl_matrix_free(V);
	gsl_vector_free(S);
	gsl_vector_free(workspace);
	qDebug() << "SVD done. ";

	cv::Mat col_sum, row_sum;
	cv::reduce(I1, col_sum, 0, cv::REDUCE_SUM);
	cv::reduce(I1, row_sum, 1, cv::REDUCE_SUM);

	// smooth
	cv::Mat colSum;
	cv::Mat rowSum;
	cv::GaussianBlur(col_sum, colSum, cv::Size(5, 5), 3); // 应用高斯模糊
	cv::GaussianBlur(row_sum, rowSum, cv::Size(5, 5), 3);

	QVector<qreal> colSumVec;
	QVector<qreal> rowSumVec;
	for (size_t i = 0; i < nPixel; i++)
	{
		colSumVec.append(colSum.at<qreal>(i));
		rowSumVec.append(rowSum.at<qreal>(i));
	}

	QVector<int> peak_x_index = FindPeaks(colSumVec, nCrystal);
	QVector<int> peak_y_index = FindPeaks(rowSumVec, nCrystal);

	for (int i = 0; i < nCrystal; ++i)
	{
		for (int j = 0; j < nCrystal; ++j)
		{
			m_pt(i, j)[0] = peak_x_index[j];
			m_pt(i, j)[1] = peak_y_index[i];
		}
	}
	qDebug() << QString::fromLocal8Bit("所有晶体初始位置设置完毕。");

	//**********************************************************
	// step1，每组整体做平均值移动
	for (int iRow = 0; iRow < m_num1; ++iRow)
	{
		for (int iCol = 0; iCol < m_num1; ++iCol)
		{
			// 求 4 个点的重心的初始位置
			cv::Point2f c(0, 0);
			for (int i = 0; i < m_num2; ++i)
			{
				for (int j = 0; j < m_num2; ++j)
				{
					int idx_row = iRow * m_num2 + i;
					int idx_col = iCol * m_num2 + j;
					int x = m_pt(idx_row, idx_col)[0];
					int y = m_pt(idx_row, idx_col)[1];
					c += cv::Point2f(x, y);
				}
			}

			c = c / (m_num2 * m_num2); // 初始位置
			cv::Point2f pre_c(-1, -1);

			while (cv::norm(pre_c - c) > 1)  // 迭代移动重心
			{
				pre_c = c;

				// 计算重心
				c = cv::Point2f(0, 0);
				for (int i = 0; i < m_num2; ++i)
				{
					for (int j = 0; j < m_num2; ++j)
					{
						int idx_row = iRow * m_num2 + i;
						int idx_col = iCol * m_num2 + j;
						int x = m_pt(idx_row, idx_col)[0];
						int y = m_pt(idx_row, idx_col)[1];
						cv::Point2f p(x, y);

						// 累加项 // 附近的点进行累加
						int r = 8; // 累加范围
						cv::Point2f u(0, 0);
						qreal d = 0;
						for (int n = std::max(y - r, 1); n <= std::min(y + r, 511); ++n)
						{
							for (int m = std::max(x - r, 1); m <= std::min(x + r, 511); ++m)
							{
								if (cv::norm(p - cv::Point2f(m, n)) <= r)
								{
									u += I0(n, m) * cv::Point2f(m, n);
									d += I0(n, m);
								}
							}
						}
						c = c + u / d;
					}
				}
				c = c / (m_num2 * m_num2);

				for (int i = 0; i < m_num2; ++i)
				{
					for (int j = 0; j < m_num2; ++j)
					{
						int idx_row = iRow * m_num2 + i;
						int idx_col = iCol * m_num2 + j;
						m_pt(idx_row, idx_col)[0] += qRound(c.x - pre_c.x);
						m_pt(idx_row, idx_col)[1] += qRound(c.y - pre_c.y);
					}
				}
			}
		}
	}
	qDebug() << QString::fromLocal8Bit("第一次平均值移动迭代，每组峰位移动完成。");
	//***********************************************************************/

	//*****************************************************
	// step2, all peak 做平均值移动, 使得每个峰做小范围移动
	for (int iRow = 0; iRow < nCrystal; ++iRow)
	{
		for (int iCol = 0; iCol < nCrystal; ++iCol)
		{
			cv::Point2f x(m_pt(iRow, iCol)[0], m_pt(iRow, iCol)[1]);
			cv::Point2f pre_x(0, 0);
			while (cv::norm(pre_x - x) > 0.1)
			{
				pre_x = x;
				cv::Point2f u(0, 0);
				qreal d = 0;

				int r = 2;
				for (int n = std::max(static_cast<int>(x.y - r), 1);
					n <= std::min(static_cast<int>(x.y + r), 511); ++n)
				{
					for (int m = std::max(static_cast<int>(x.x - r), 1);
						m <= std::min(static_cast<int>(x.x + r), 511); ++m)
					{
						if (cv::norm(x - cv::Point2f(m, n)) <= r)
						{
							//qreal k = cv::norm(x - cv::Point2f(m, n)) / pow((2 * r + 1), 2);
							//qreal g = (1 / sqrt(2 * M_PI)) * exp(-0.5 * k);
							qreal g = 1;
							u += I0(n, m) * g * cv::Point2f(m, n);
							d += I0(n, m) * g;
						}
					}
				}
				x = u / d;
			}
			m_pt(iRow, iCol)[0] = qRound(x.x);
			m_pt(iRow, iCol)[1] = qRound(x.y);
		}
	}
	qDebug() << QString::fromLocal8Bit("第二次平均值移动迭代。所有峰位寻找完成。");
	//*****************************************************/
}


void Block::Segment2()
{
	cv::Mat_<qreal> I0 = m_I0.clone();

	cv::Mat temp;
	cv::GaussianBlur(I0, temp, cv::Size(5, 5), 1, 1);
	cv::Laplacian(temp, I0, CV_64F, 5);

	I0 = I0 * -100;
	I0.setTo(1.e-5, I0 < 2);

	m_I0 = I0;

	cv::Mat col_sum, row_sum;
	cv::reduce(I0, col_sum, 0, cv::REDUCE_SUM);
	cv::reduce(I0, row_sum, 1, cv::REDUCE_SUM);

	// smooth
	cv::Mat colSum;
	cv::Mat rowSum;
	cv::GaussianBlur(col_sum, colSum, cv::Size(5, 5), 3); // 应用高斯模糊
	cv::GaussianBlur(row_sum, rowSum, cv::Size(5, 5), 3);

	// scale
	qreal minVal, maxVal;
	cv::Point minLoc, maxLoc;
	cv::minMaxLoc(colSum, &minVal, &maxVal, &minLoc, &maxLoc);
	colSum = colSum / maxVal * 100;
	colSum.setTo(0, colSum < 5);

	cv::minMaxLoc(rowSum, &minVal, &maxVal, &minLoc, &maxLoc);
	rowSum = rowSum / maxVal * 100;
	rowSum.setTo(0, rowSum < 5);

	QVector<int> x_locs;
	for (size_t i = 1; i < nPixel - 1; i++)
	{
		auto mid = colSum.at<qreal>(i);
		auto left = colSum.at<qreal>(i - 1);
		auto right = colSum.at<qreal>(i + 1);
		if (mid > left && mid > right && mid > 10)
		{
			x_locs.append(i);
		}
	}

	QVector<int> y_locs;
	for (size_t i = 1; i < nPixel - 1; i++)
	{
		auto mid = rowSum.at<qreal>(i);
		auto left = rowSum.at<qreal>(i - 1);
		auto right = rowSum.at<qreal>(i + 1);
		if (mid > left && mid > right && mid > 10)
		{
			y_locs.append(i);
		}
	}

	auto xmin = x_locs.first();
	auto xmax = x_locs.last();

	auto ymin = y_locs.first();
	auto ymax = y_locs.last();

	float x_width = 1.0 * (xmax - xmin) / 24;
	float y_width = 1.0 * (ymax - ymin) / 24;

	QVector<int> peak_x_index(nCrystal, 0);
	QVector<int> peak_y_index(nCrystal, 0);

	for (size_t i = 0; i < nCrystal; i++)
	{
		if (i == 0)
		{
			peak_x_index[i] = xmin;
			peak_y_index[i] = ymin;
			continue;
		}

		if (i == 1)
		{
			peak_x_index[i] = round(xmin + x_width / 2);
			peak_y_index[i] = round(ymin + y_width / 2);
			continue;
		}

		if (i == (nCrystal - 1))
		{
			peak_x_index[i] = xmax;
			peak_y_index[i] = ymax;
			continue;
		}

		if (i == (nCrystal - 2))
		{
			peak_x_index[i] = round(xmax - x_width / 2);
			peak_y_index[i] = round(ymax - y_width / 2);
			continue;
		}

		peak_x_index[i] = round(xmin + i * x_width - x_width / 2);
		peak_y_index[i] = round(ymin + i * y_width - y_width / 2);
	}

	for (int i = 0; i < nCrystal; ++i)
	{
		for (int j = 0; j < nCrystal; ++j)
		{
			m_pt(i, j)[0] = peak_x_index[j];
			m_pt(i, j)[1] = peak_y_index[i];
		}
	}
	qDebug() << QString::fromLocal8Bit("所有晶体初始位置设置完毕。");

	// 按行排序
	for (size_t iRow = 0; iRow < nCrystal; iRow++)
	{
		auto y0 = m_pt(iRow, 0)[1];

		int r = 15;
		cv::Mat subImage = I0(cv::Rect(0, y0 - r, I0.cols, 2 * r));

		//cv::namedWindow("Image Window", cv::WINDOW_AUTOSIZE); // 窗口名称和模式
		//cv::imshow("Image Window", subImage);
		//cv::waitKey(0);

		cv::Mat col_sum;
		cv::reduce(subImage, col_sum, 0, cv::REDUCE_SUM);

		QVector<qreal> colSumVec;
		for (size_t i = 0; i < nPixel; i++)
		{
			colSumVec.append(col_sum.at<qreal>(i));
		}

		QVector<int> peak_x_index = FindPeaks(colSumVec, nCrystal);

		for (size_t iCol = 0; iCol < nCrystal; iCol++)
		{
			m_pt(iRow, iCol)[0] = peak_x_index[iCol];
		}
	}

	// 按列排序
	for (size_t iCol = 0; iCol < nCrystal; iCol++)
	{
		cv::Mat_<qreal> LineMat(1, nCrystal); // line
		for (int iRow = 0; iRow < nCrystal; ++iRow)
		{
			LineMat(0, iRow) = m_pt(iRow, iCol)[0];
		}

		cv::Mat_<qreal> resizedLineMat;
		cv::resize(LineMat, resizedLineMat, cv::Size(nPixel, 1), 0, 0, cv::INTER_LINEAR);

		int r = 15;
		QVector<qreal> rowSumVec;
		for (size_t iRow = 0; iRow < nPixel; iRow++)
		{
			int colID = round(resizedLineMat(0, iRow));
			double sum = 0;
			for (size_t i = colID - r; i < colID + r; i++)
			{
				sum += I0(iRow, i);
			}
			rowSumVec.append(sum);
		}

		QVector<int> peak_y_index = FindPeaks(rowSumVec, nCrystal);
		for (size_t iRow = 0; iRow < nCrystal; iRow++)
		{
			m_pt(iRow, iCol)[1] = peak_y_index[iRow];
		}
	}
}

/**
 * @brief 自动寻峰可能存在错误，此时需要进行手动调整。对于输入的位置信息，
 * 计算该位置与所有peak的距离，将该位置信息赋予离它最近的peak。
 */
void Block::ManualAdjust(QPoint pos)
{
	cv::Mat_<qreal> dis(nCrystal, nCrystal, 0.0);
	for (int i = 0; i < nCrystal; ++i)
	{
		for (int j = 0; j < nCrystal; ++j)
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
	int Radius = 25; // 判断像素点属于哪个peak的依据是距离，但是距离过远的点不进行分割，直接设定为0xFFFF

	// m_edgeFlag: 
	// 0, 边缘晶体不强行分割成2根晶体;
	// 1, random method; 
	// 2, line-cut method
	if (m_edgeFlag == 0)
	{
		// 边缘晶体不强行分割成 2 根晶体
		for (int i = 0; i < nPixel; ++i)
		{
			for (int j = 0; j < nPixel; ++j)
			{
				cv::Mat_<qreal> dis(nCrystal, nCrystal);

				for (int iRow = 0; iRow < nCrystal; ++iRow)
				{
					for (int iCol = 0; iCol < nCrystal; ++iCol)
					{
						cv::Point2f p(m_pt(iRow, iCol)[0], m_pt(iRow, iCol)[1]);
						dis(iRow, iCol) = cv::norm(cv::Point2f(j, i) - p);
					}
				}

				cv::Point min_loc;
				cv::Point max_loc;
				double minVal;
				double maxVal;
				cv::minMaxLoc(dis, &minVal, &maxVal, &min_loc, &max_loc);

				if (minVal > Radius)
				{
					m_segr(i, j) = 0xFFFF; // 距离过远的点不进行分割，直接设定为0xFFFF
				}
				else
				{
					int rowID = min_loc.y;
					int colID = min_loc.x;
					rowID = (nCrystal - 1) - rowID; // 反转 y 轴，为了匹配电子学坐标
					m_segr(i, j) = rowID * nCrystal + colID;
				}
			}
		}
	}

	if (m_edgeFlag == 1)
	{
		// 边缘晶体分割成 2 根晶体，random method;
		srand(time(0));
		for (int i = 0; i < nPixel; ++i)
		{
			for (int j = 0; j < nPixel; ++j)
			{
				cv::Mat_<qreal> dis(nCrystal, nCrystal);

				for (int iRow = 0; iRow < nCrystal; ++iRow)
				{
					for (int iCol = 0; iCol < nCrystal; ++iCol)
					{
						cv::Point2f p(m_pt(iRow, iCol)[0], m_pt(iRow, iCol)[1]);
						dis(iRow, iCol) = cv::norm(cv::Point2f(j, i) - p);
					}
				}

				cv::Point min_loc;
				cv::Point max_loc;
				double minVal;
				double maxVal;
				cv::minMaxLoc(dis, &minVal, &maxVal, &min_loc, &max_loc);

				if (minVal > Radius)
				{
					m_segr(i, j) = 0xFFFF; // 距离过远的点不进行分割，直接设定为0xFFFF
					m_segr_plus(i, j) = 0xFFFF;
				}
				else
				{
					int rowID = min_loc.y;
					int colID = min_loc.x;
					rowID = (nCrystal - 1) - rowID; // 反转 y 轴，为了匹配电子学坐标
					m_segr(i, j) = rowID * nCrystal + colID;

					// 修正26=>28
					if (rowID == 0)
					{
						// 0 ==> 50% is 0, 50% is 1
						int aN = rand() % 100;
						if (aN < 50)
						{
							rowID++;
						}
					}
					else if (rowID == (nCrystal - 1))
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

					if (colID == 0)
					{
						// 0 ==> 50% is 0, 50% is 1
						int aN = rand() % 100;
						if (aN < 50)
						{
							colID++;
						}
					}
					else if (colID == (nCrystal - 1))
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

					m_segr_plus(i, j) = rowID * (nCrystal + 2) + colID;
				}
			}
		}
	}

	if (m_edgeFlag == 2)
	{
		// 边缘晶体分割成 2 根晶体，line-cut method;
		std::vector<double> x0;
		std::vector<double> y0;
		std::vector<double> xq;
		for (int i = 0; i < nPixel; ++i)
		{
			xq.push_back(i);
		}

		// up line
		x0.clear();
		y0.clear();
		for (size_t i = 0; i < nCrystal; i++)
		{
			x0.push_back(m_pt(0, i)[0]);
			y0.push_back(m_pt(0, i)[1]);
		}
		auto uLine = interp1(x0, y0, xq);

		// bottom line
		x0.clear();
		y0.clear();
		for (size_t i = 0; i < nCrystal; i++)
		{
			x0.push_back(m_pt(nCrystal - 1, i)[0]);
			y0.push_back(m_pt(nCrystal - 1, i)[1]);
		}
		auto bLine = interp1(x0, y0, xq);

		// left line
		x0.clear();
		y0.clear();
		for (size_t i = 0; i < nCrystal; i++)
		{
			y0.push_back(m_pt(i, 0)[0]);
			x0.push_back(m_pt(i, 0)[1]);
		}
		auto lLine = interp1(x0, y0, xq);

		//right line
		x0.clear();
		y0.clear();
		for (size_t i = 0; i < nCrystal; i++)
		{
			y0.push_back(m_pt(i, nCrystal - 1)[0]);
			x0.push_back(m_pt(i, nCrystal - 1)[1]);
		}
		auto rLine = interp1(x0, y0, xq);

		for (int i = 0; i < nPixel; ++i)
		{
			for (int j = 0; j < nPixel; ++j)
			{
				cv::Mat_<qreal> dis(nCrystal, nCrystal);

				for (int iRow = 0; iRow < nCrystal; ++iRow)
				{
					for (int iCol = 0; iCol < nCrystal; ++iCol)
					{
						cv::Point2f p(m_pt(iRow, iCol)[0], m_pt(iRow, iCol)[1]);
						dis(iRow, iCol) = cv::norm(cv::Point2f(j, i) - p);
					}
				}

				cv::Point min_loc;
				cv::Point max_loc;
				double minVal;
				double maxVal;
				cv::minMaxLoc(dis, &minVal, &maxVal, &min_loc, &max_loc);

				if (minVal > Radius)
				{
					m_segr(i, j) = 0xFFFF; // 距离过远的点不进行分割，直接设定为0xFFFF
					m_segr_plus(i, j) = 0xFFFF;
				}
				else
				{
					int rowID = min_loc.y;
					int colID = min_loc.x;

					int aRowID = (nCrystal - 1) - rowID; //为了匹配电子学坐标，反转 y 轴
					m_segr(i, j) = aRowID * nCrystal + colID;

					rowID++; // 1==>2, ..., 24==>25
					colID++; // 1==>2, ..., 24==>25

					int uBorder = qRound(uLine[j]);
					int bBorder = qRound(bLine[j]);
					int lBorder = qRound(lLine[i]);
					int rBorder = qRound(rLine[i]);

					if (i < uBorder)
					{
						rowID = 0;
					}
					else if (i > bBorder)
					{
						rowID = nCrystal + 1; // 27
					}

					if (j < lBorder)
					{
						colID = 0;
					}
					else if (j > rBorder)
					{
						colID = nCrystal + 1; // 27
					}

					rowID = (nCrystal + 1) - rowID; // 反转
					m_segr_plus(i, j) = rowID * (nCrystal + 2) + colID;
				}
			}
		}
	}

	m_edge = cv::Mat::zeros(nPixel, nPixel, CV_8UC1);
	m_segMap = m_I0.clone();

	qreal max_val;
	cv::minMaxLoc(m_I0, NULL, &max_val, NULL, NULL);

	for (int i = 1; i < nPixel - 1; ++i)
	{
		for (int j = 1; j < nPixel - 1; ++j)
		{
			int id0 = 0;
			int id1 = 0;
			int id2 = 0;

			if (m_edgeFlag == 0 || m_edgeFlag == 1)
			{
				id0 = m_segr(i, j);
				id1 = m_segr(i + 1, j);
				id2 = m_segr(i, j + 1);
			}

			if (m_edgeFlag == 2)
			{
				id0 = m_segr_plus(i, j);
				id1 = m_segr_plus(i + 1, j);
				id2 = m_segr_plus(i, j + 1);
			}

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
	GenPositionLUT();
	CalRecEHist();
	GenEnergyLUT();
	GenUniformityLUT();
}

/**
 * @details 位置查找表生成后，就可以对单根晶体性能进行计算，
 * 此时将BK中保存的原始事例信息按映射表分配给对应的晶体。
 * 同时让所有晶体进行寻峰、计算 ADC 到 keV 的转换系数，计算每个晶体的能谱。
 */
void Block::CalRecEHist()
{
	for (int i = 0; i < m_eList.size(); ++i)
	{
		quint16 x = m_xList[i];
		quint16 y = m_yList[i];
		quint16 e = m_eList[i];

		if (0 < x && x < nPixel && 0 < y && y < nPixel)
		{
			quint16 ID = m_segr_plus(y, x);
			if (ID != 0xFFFF)
			{
				m_crystals[ID]->Fill(e);
			}
		}
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
	// step 5, 求分割质量参数

	// calculate peaks mean-height
	int nPeaks = nCrystal * nCrystal;
	qreal peak_total_evts = 0;
	qreal totalFWHM = 0;
	for (int i = 0; i < nCrystal; ++i)
	{
		for (int j = 0; j < nCrystal; ++j)
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
	qreal edgeEvts = 0;
	quint64 edgeN = 0;
	for (int i = 0; i < nPixel; ++i)
	{
		for (int j = 0; j < nPixel; ++j)
		{
			if (1 == m_edge(i, j))
			{
				edgeN++;
				edgeEvts += m_I0(i, j);
			}

		}
	}

	qreal valley_mean_height = edgeEvts / edgeN;
	m_PVR = peak_mean_height / valley_mean_height;
	m_FWHM = totalFWHM / nPeaks;
}

void Block::GenPositionLUT()
{
	//根据分割结果，生成位置查找表
	int CMID = m_ID / nBK;
	int BKID = m_ID % nBK;
	QString CMIDStr = QString::number(CMID).rightJustified(2, '0');
	QString BKIDStr = QString::number(BKID).rightJustified(2, '0');

	QString fName = currentPath + "Data/PLUT_" + CMIDStr + "_" + BKIDStr;
	QFile file(fName);
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
			quint16 crystalID = m_segr_plus(i, j);
			out.writeRawData(reinterpret_cast<char*>(&crystalID), sizeof(quint16));
		}
	}

	file.close();
	qDebug() << QString::fromLocal8Bit("位置查找表已生成: ") + fName;
}

void Block::GenEnergyLUT()
{
	int CMID = m_ID / nBK;
	int BKID = m_ID % nBK;
	QString CMIDStr = QString::number(CMID).rightJustified(2, '0');
	QString BKIDStr = QString::number(BKID).rightJustified(2, '0');

	// 保存能量转换系数
	QString fName = currentPath + "Data/ELUT_" + CMIDStr + "_" + BKIDStr;
	QFile file1(fName);
	if (!file1.open(QIODevice::WriteOnly))
	{
		qDebug() << QString::fromLocal8Bit("打开文件失败，无法生成能量查找表。");
		return;
	}

	QDataStream outStream1(&file1);
	for (int i = 0; i < 1024; ++i)
	{
		if (i < crystalNum)
		{
			qreal slope = m_crystals[i]->GetSlope();
			slope *= pow(2, 16);
			quint16 uSlope = static_cast<quint16>(slope);
			outStream1.writeRawData(reinterpret_cast<char*>(&uSlope), sizeof(quint16));
		}
		else
		{
			quint16 temp = 0;
			outStream1.writeRawData(reinterpret_cast<char*>(&temp), sizeof(quint16));
		}
	}

	file1.close();
	qDebug() << QString::fromLocal8Bit("能量查找表已生成: ") + fName;

	// 保存能谱
	fName = currentPath + "Data/EHist_" + CMIDStr + "_" + BKIDStr;
	QFile file2(fName);
	if (!file2.open(QIODevice::WriteOnly))
	{
		qDebug() << QString::fromLocal8Bit("打开文件失败，无法生成能谱。");
		return;
	}

	QDataStream outStream2(&file2);
	for (int i = 0; i < crystalNum; ++i)
	{
		QVector<qreal> aHist = m_crystals[i]->GetADCHist()->GetBinContents();
		for (const auto& var : aHist)
		{
			quint16 uVar = static_cast<quint16>(var);
			outStream2.writeRawData(reinterpret_cast<char*>(&uVar), sizeof(quint16));
		}
	}

	file2.close();
	qDebug() << QString::fromLocal8Bit("能谱已保存: ") + fName;
}

void Block::ReGenELUT()
{
	int CMID = m_ID / nBK;
	int BKID = m_ID % nBK;
	QString CMIDStr = QString::number(CMID).rightJustified(2, '0');
	QString BKIDStr = QString::number(BKID).rightJustified(2, '0');

	// 保存能量转换系数
	QString fName = currentPath + "Data/ELUT_" + CMIDStr + "_" + BKIDStr;
	QFile file(fName);
	if (!file.open(QIODevice::WriteOnly))
	{
		qDebug() << QString::fromLocal8Bit("打开文件失败，无法生成能量查找表。");
		return;
	}

	QDataStream outStream(&file);
	for (int i = 0; i < 1024; ++i)
	{
		if (i < crystalNum)
		{
			qreal slope = m_crystals[i]->GetSlope();
			slope *= pow(2, 16);
			quint16 uSlope = static_cast<quint16>(slope);
			outStream.writeRawData(reinterpret_cast<char*>(&uSlope), sizeof(quint16));
		}
		else
		{
			quint16 temp = 0;
			outStream.writeRawData(reinterpret_cast<char*>(&temp), sizeof(quint16));
		}
	}

	file.close();
	qDebug() << QString::fromLocal8Bit("能量查找表已生成: ") + fName;
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

	// 获取中心的 22x22 区域
	cv::Mat_<qreal> nEvts_center = nEvts(cv::Rect(2, 2, nCrystal - 4, nCrystal - 4));

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

cv::Mat_<qreal> Block::GetSegMap()
{
	return m_segMap;
}

qreal Block::GetER()
{
	auto h = m_recEHist->Clone();
	h->SetCutValue(recE_cutValue);
	//h->Smooth();
	return h->GetResolution();
}

qreal Block::GetPVR() const
{
	return m_PVR;
}

qreal Block::GetFWHM() const
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
	m_segr.setTo(0);
	m_segr_plus.setTo(0);
	m_edge.setTo(0);
	m_segMap.setTo(0);
}

QVector<int> Block::FindPeaks(QVector<qreal> v, int nPeaks)
{
	//**************************************************
	QVector<int> peaks;
	for (int iPeak = 0; iPeak < nPeaks; ++iPeak)
	{
		int maxIdx = std::max_element(v.begin(), v.end()) - v.begin();
		peaks.append(maxIdx);

		int r = 8;
		int start = std::max(maxIdx - r, 0);
		int stop = std::min(maxIdx + r, nPixel - 1);
		for (int i = start; i <= stop; ++i)
		{
			v[i] = 0;
		}
	}

	std::sort(peaks.begin(), peaks.end());
	return peaks;

	/*********************************************
	// 初始化
	QVector<int> adjustedValues = peaks; // 保留原始序列
	int n = peaks.size();

	for (int iter = 0; iter < 1; ++iter)
	{
		// 计算相邻差值
		QVector<double> diff(n - 1);
		for (int i = 0; i < n - 1; ++i)
		{
			diff[i] = adjustedValues[i + 1] - adjustedValues[i];
		}

		// 计算平均差值
		double meanDiff = std::accumulate(diff.begin(), diff.end(), 0.0) / diff.size();

		// 调整差值
		for (int i = 0; i < diff.size(); ++i)
		{
			diff[i] += 0.999 * (meanDiff - diff[i]);
		}

		// 根据调整后的差值重建序列，保持首尾元素不变
		for (int i = 1; i < n; ++i)
		{
			adjustedValues[i] = round(adjustedValues[i - 1] + diff[i - 1]);
		}
	}

	return adjustedValues;
	//return peaks;
	**********************************************/

	/*****************************************
	std::vector<int> locs;
	std::vector<qreal> prominences;

	int n = v.size();

	for (int i = 1; i < n - 1; ++i)
	{
		// 检查是否为局部最大值
		if (v[i] > v[i - 1] && v[i] > v[i + 1] && v[i] > 3)
		{
			double peakValue = v[i];

			// 从峰向左扫描
			int leftIndex = i;
			while (leftIndex > 0 && v[leftIndex - 1] <= peakValue)
			{
				--leftIndex;
			}
			double leftMin = *std::min_element(v.begin() + leftIndex, v.begin() + i + 1);

			// 从峰向右扫描
			int rightIndex = i;
			while (rightIndex < n - 1 && v[rightIndex + 1] <= peakValue)
			{
				++rightIndex;
			}
			double rightMin = *std::min_element(v.begin() + i, v.begin() + rightIndex + 1);

			// 计算显著性
			double referenceLevel = std::max(leftMin, rightMin);
			double prominence = peakValue - referenceLevel;

			// 保存结果
			locs.emplace_back(i);
			prominences.emplace_back(prominence);
		}
	}

	auto nLocs = locs.size();
	if (nLocs > nPeaks)
	{
		// 创建索引数组
		std::vector<size_t> indices(nLocs);
		for (size_t i = 0; i < indices.size(); ++i)
		{
			indices[i] = i;
		}

		// 根据 prominences 的值从大到小排序索引数组
		std::sort(indices.begin(), indices.end(), [&prominences](size_t i1, size_t i2) {
			return prominences[i1] > prominences[i2]; // 从大到小排序
		});

		// 使用排序后的索引数组重新排列 locs
		QVector<int> sortedLocs(nPeaks);
		for (size_t i = 0; i < nPeaks; ++i)
		{
			sortedLocs[i] = locs[indices[i]];
		}

		//排序 locs，从小到大
		std::sort(sortedLocs.begin(), sortedLocs.end());

		return sortedLocs;
	}
	else
	{
		QVector<int> peaks;
		for (int iPeak = 0; iPeak < nPeaks; ++iPeak)
		{
			int maxIdx = std::max_element(v.begin(), v.end()) - v.begin();
			peaks.append(maxIdx);

			int r = 3;
			int start = std::max(maxIdx - r, 0);
			int stop = std::min(maxIdx + r, nPixel - 1);
			for (int i = start; i <= stop; ++i)
			{
				v[i] = 0;
			}
			std::sort(peaks.begin(), peaks.end());
			return peaks;
		}
	}
	******************************************/
}

cv::Mat_<qreal> Block::GetUniformity() const
{
	return m_uniformity;
}

QVector<qreal> Block::GetUStatistics() const
{
	QVector<qreal> answer;
	answer.push_back(m_uMax);
	answer.push_back(m_uMin);
	answer.push_back(m_uMean);
	answer.push_back(m_uSD);
	return answer;
}

// 线性插值函数
double Block::interp1Linear(const std::vector<double>& x, const std::vector<double>& y, double xq)
{
	if (x.size() != y.size() || x.empty())
	{
		throw std::invalid_argument("Input vectors x and y must be of the same size and non-empty.");
	}

	// 确保 x 是升序
	if (!std::is_sorted(x.begin(), x.end()))
	{
		throw std::invalid_argument("Input vector x must be sorted in ascending order.");
	}

	// 如果 xq 在范围之外，直接返回边界值（可以根据需要调整处理方式）
	if (xq <= x.front()) return y.front();
	if (xq >= x.back()) return y.back();

	// 查找 xq 所在区间
	auto it = std::upper_bound(x.begin(), x.end(), xq);
	size_t idx = std::distance(x.begin(), it) - 1;

	// 线性插值公式
	double x1 = x[idx], x2 = x[idx + 1];
	double y1 = y[idx], y2 = y[idx + 1];
	return y1 + (y2 - y1) * (xq - x1) / (x2 - x1);
}

std::vector<double> Block::interp1(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& xq)
{
	std::vector<double> yq;
	for (double xqi : xq)
	{
		yq.push_back(interp1Linear(x, y, xqi));
	}
	return yq;
}