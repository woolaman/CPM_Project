#include "Block.h"

#include <QDebug>
#include <QFile>

#include "opencv2/imgproc.hpp"

#include "Parameters.h"


Block::Block()
{
    m_I0 = cv::Mat::zeros(nPixel, nPixel, CV_64FC1);
    m_pt = cv::Mat_<cv::Vec2w>(nCrystal, nCrystal);
    m_segr = cv::Mat::zeros(nPixel, nPixel, CV_16UC1);

    m_ADCHist = new Histogram(ADC_min, ADC_max, ADC_nBins);
    m_recEHist = new Histogram(recE_min, recE_max, recE_nBins);

    for (int i = 0; i < crystalNum; ++i)
    {
        Crystal* aCrystal = new Crystal(i);
        m_crystals.append(aCrystal);
    }

    CalGroupPar();
}


Block::Block(int ID)
{
    m_ID = ID;

    m_I0 = cv::Mat::zeros(nPixel, nPixel, CV_64FC1);
    m_pt = cv::Mat_<cv::Vec2w>(nCrystal, nCrystal);
    m_segr = cv::Mat::zeros(nPixel, nPixel, CV_16UC1);

    m_ADCHist = new Histogram(ADC_min, ADC_max, ADC_nBins);
    m_recEHist = new Histogram(recE_min, recE_max, recE_nBins);

    for (int i = 0; i < crystalNum; ++i)
    {
        Crystal* aCrystal = new Crystal(i);
        m_crystals.append(aCrystal);
    }

    CalGroupPar();
}


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


void Block::Fill(quint16 x, quint16 y, quint16 e)
{
    m_xList.append(x);
    m_yList.append(y);
    m_eList.append(e);
    m_ADCHist->Fill(e);
}


void Block::SetEW(int EW_min, int EW_max)
{
    m_EW_min = EW_min;
    m_EW_max = EW_max;
}


void Block::CalMap()
{
    //qDebug() << "start to calculate floodmap beteen energy window.";
    for (int i = 0; i < m_eList.size(); ++i)
    {
        quint16 x = m_xList[i];
        quint16 y = m_yList[i];
        quint16 e = m_eList[i];

        if(m_EW_min<e && e<m_EW_max)
        {
            m_I0(y, x) += 1;
        }
    }
}


void Block::Segment(SegmentMethod method)
{
    if(method==SegmentMethod::SVD)
    {
        Segment1();
    }

    if(method==SegmentMethod::FindMaximum)
    {
        Segment2();
    }
}


void Block::Segment1()
{
    // 自动寻峰, 奇异值分解, 寻找规则峰位作为平均值移动算法的迭代初始位置
    // 平均值移动算法，寻找peaks
    qDebug() << "SVD start.";
    cv::Mat U, S, Vt;
    cv::SVD::compute(m_I0, S, U, Vt); // 进行奇异值分解

    cv::Mat S1 = cv::Mat::zeros(m_I0.size(), m_I0.type());
    S1.at<qreal>(0, 0) = S.at<qreal>(0, 0);
    cv::Mat I1 = U*S1*Vt;
    qDebug() << "SVD done. ";

    cv::Mat col_sum, row_sum;
    cv::reduce(I1, col_sum, 0, cv::REDUCE_SUM);
    cv::reduce(I1, row_sum, 1, cv::REDUCE_SUM);

    QVector<qreal> colSumVec = {col_sum.begin<qreal>(), col_sum.end<qreal>()};
    QVector<qreal> rowSumVec = {row_sum.begin<qreal>(), row_sum.end<qreal>()};
    QVector<int> peak_x_index = FindPeaks(colSumVec, nCrystal);
    QVector<int> peak_y_index = FindPeaks(rowSumVec, nCrystal);

    for (int i = 0; i < nCrystal; ++i)
    {
        for (int  j= 0; j < nCrystal; ++j)
        {
            m_pt(i, j)[0] = peak_x_index[j];
            m_pt(i, j)[1] = peak_y_index[i];
        }
    }
    qDebug() << "所有晶体初始位置设置完毕。";

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
                    c0 += cv::Point2f(m_pt(iRow*m_num2+i, iCol*m_num2+j)[0],
                                      m_pt(iRow*m_num2+i, iCol*m_num2+j)[1]);
                }
            }

            cv::Point2f c = c0/(m_num2*m_num2); // 初始位置
            cv::Point2f pre_c(0, 0);

            while (cv::norm(pre_c-c) > 1)  // 迭代移动重心
            {
                pre_c = c;
                c = cv::Point2f(0, 0);

                // 计算重心
                for (int i = 0; i < m_num2; ++i)
                {
                    for (int j = 0; j < m_num2; ++j)
                    {
                        cv::Point2f x(m_pt(iRow*m_num2+i, iCol*m_num2+j)[0],
                                      m_pt(iRow*m_num2+i, iCol*m_num2+j)[1]);

                        // 累加项
                        cv::Point2f u(0, 0);
                        qreal d = 0;
                        int r = 6;
                        for (int n = x.y-r; n <= x.y+r; ++n)
                        {
                            for (int m = x.x-r; m <= x.x+r; ++m)
                            {
                                if (cv::norm(x-cv::Point2f(m, n)) <= r)
                                {
                                    u += m_I0(n, m)*cv::Point2f(m, n);
                                    d += m_I0(n, m);
                                }
                            }
                        }

                        if(d==0)
                        {
                            qDebug() <<  "迭代过程中，分母 d = 0 !!! Bug !!!";
                        }
                        else
                        {
                            c += u/d;
                        }
                    }
                }

                c = c/(m_num2*m_num2);

                for (int i = 0; i < m_num2; ++i)
                {
                    for (int j = 0; j < m_num2; ++j)
                    {
                        m_pt(iRow*m_num2+i, iCol*m_num2+j)[0] += qRound(c.x - pre_c.x);
                        m_pt(iRow*m_num2+i, iCol*m_num2+j)[1] += qRound(c.y - pre_c.y);
                    }
                }
            }
        }
    }
    qDebug() << "第一次平均值移动迭代，每组峰位移动完成。";

    // step2, all peak 做平均值移动, 使得每个峰做小范围移动
    for (int iRow = 0; iRow < nCrystal; ++iRow)
    {
        for (int iCol = 0; iCol < nCrystal; ++iCol)
        {
            cv::Point2f x(m_pt(iRow, iCol)[0], m_pt(iRow, iCol)[1]);
            cv::Point2f pre_x(0, 0);
            while(cv::norm(pre_x-x)>0.1)
            {
                pre_x = x;
                cv::Point2f u(0, 0);
                qreal d = 0;

                int r = 6;
                for (int n = x.y-r; n <= x.y+r; ++n)
                {
                    for (int m = x.x-r; m <= x.x+r; ++m)
                    {
                        if (cv::norm(x-cv::Point2f(m, n)) <= r)
                        {
                            //qreal k = cv::norm(x-cv::Point2f(m, n))/pow((2*r+1), 2);
                            //qreal g = (1/sqrt(2*M_PI))*exp(-0.5*k);
                            qreal g = 1;
                            u += m_I0(n, m)*g*cv::Point2f(m, n);
                            d += m_I0(n, m)*g;
                        }
                    }
                }

                if (d==0)
                {
                    qDebug() << "迭代过程中，分母 d = 0 !!! Bug !!!";
                }
                else
                {
                    x = u/d;
                }
            }
            m_pt(iRow, iCol)[0] = qRound(x.x);
            m_pt(iRow, iCol)[1] = qRound(x.y);
        }
    }
    qDebug() << "第二次平均值移动迭代。所有峰位寻找完成。";
}


void Block::Segment2() // maximum method to find peaks
{
    // 使用 OpenCV 的平滑函数对数据进行平滑处理
    cv::Mat_<qreal> I1;
    cv::blur(m_I0, I1, cv::Size(5, 5));

    //cv::Mat_<qreal> I2;
    //cv::GaussianBlur(I1, I2, cv::Size(3, 3), 1, 1);
    //I1 = I2;

    QVector<cv::Point> peaks;
    for (int i = 0; i < crystalNum; ++i)
    {
        qreal minVal, maxVal;
        cv::Point minLoc, maxLoc;
        cv::minMaxLoc(I1, &minVal, &maxVal, &minLoc, &maxLoc);

        peaks.append(maxLoc);

        // 将maxLoc附近的元素置为0
        int m0 = maxLoc.x;
        int n0 = maxLoc.y;
        int r = 5;

        for (int n = n0-r; n <= n0+r; ++n)
        {
            for (int m = m0-r; m <=m0+r ; ++m)
            {
                if(cv::norm(maxLoc-cv::Point(m, n))<=r)
                {
                    I1(n, m) = 0;
                }
            }
        }
    }

    // sort peaks
    for (int iRow = 0; iRow < nCrystal; ++iRow)
    {
        // 选出 y 值较小的 m_nCrystal 个点作为一行， y值升序排列
        std::sort(peaks.begin(), peaks.end(),
                  [](const cv::Point& p1, const cv::Point& p2){return p1.y < p2.y;});

        QVector<cv::Point> rowPeaks(peaks.begin(), peaks.begin()+nCrystal);
        // 将他们按 x 值升序排列
        std::sort(rowPeaks.begin(), rowPeaks.end(),
                  [](const cv::Point& p1, const cv::Point& p2){return p1.x < p2.x;});

        for (int iCol = 0; iCol < nCrystal; ++iCol)
        {
            peaks.pop_front();
            m_pt(iRow, iCol)[0] = rowPeaks[iCol].x;
            m_pt(iRow, iCol)[1] = rowPeaks[iCol].y;
        }
    }
}


void Block::ManualAdjust(QPoint pos)
{
    cv::Mat_<qreal> dis(nCrystal, nCrystal, 0.0);
    for (int i = 0; i < nCrystal; ++i)
    {
        for (int j = 0; j < nCrystal; ++j)
        {
            cv::Point2f p1(m_pt(i, j)[0], m_pt(i, j)[1]);
            cv::Point2f p2(pos.x(), pos.y());
            dis(i, j) = cv::norm(p1-p2);
        }
    }

    cv::Point min_loc;
    cv::minMaxLoc(dis, NULL, NULL, &min_loc, NULL);

    m_pt(min_loc)[0] = pos.x();
    m_pt(min_loc)[1] = pos.y();
}


void Block::CalSegResult()
{
    //step 4, 开始分割, //计算像素点属于哪个峰
    cv::Mat_<quint16> seg1(nPixel, nPixel); // 行信息
    cv::Mat_<quint16> seg2(nPixel, nPixel); // 列信息

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
                    dis(iRow, iCol) = cv::norm(cv::Point2f(j, i)-p);
                }
            }

            cv::Point min_loc;
            cv::minMaxLoc(dis, NULL, NULL, &min_loc, NULL);
            seg1(i, j) = min_loc.y;
            seg2(i, j) = min_loc.x;
        }
    }

    m_segr = seg1*nCrystal + seg2;
    m_edge = cv::Mat::zeros(nPixel, nPixel, CV_8UC1);
    m_segMap = m_I0.clone();

    qreal max_val;
    cv::minMaxLoc(m_I0, NULL, &max_val, NULL, NULL);

    for (int i = 1; i < nPixel-1; ++i)
    {
        for (int j = 1; j < nPixel-1; ++j)
        {
            int id0 = m_segr(i, j);
            int id1 = m_segr(i+1, j);
            int id2 = m_segr(i, j+1);
            if(id0!=id1 || id0!=id2)
            {
                m_edge(i, j) = 1;
                m_segMap(i, j) = max_val*1.1;
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
    fs.write("floodmap", m_I0); // 将I0 也写入
    fs.write("edge", m_edge);   // 写入边界结果
    fs.write("segr", m_segr);   // 写入矩阵数据
    fs.release();  // 关闭文件
}


void Block::CalRecEHist()
{
    for (int i = 0; i < m_eList.size(); ++i)
    {
        quint16 x = m_xList[i];
        quint16 y = m_yList[i];
        quint16 e = m_eList[i];

        quint16 ID = m_segr(y, x);
        m_crystals[ID]->Fill(e);
        //if(m_EW_min<e && e<m_EW_max)
        //{
        //    quint16 ID = m_segr(y, x);
        //    m_crystals[ID]->Fill(e);
        //}
    }

    for (auto aCrystal : m_crystals)
    {
        aCrystal->CalRecEHist();
        m_recEHist->Add(aCrystal->GetRecEHist());
    }
}


void Block::CalSegFOM()
{
    // step 5, 求分割质量参数
    // IR 是图像均值比去分割线上的计数，约大质量越好
    // rms 是各个分割区域的均方差，越小越好
    quint64 totalEvts = 0;
    quint64 edgeEvts = 0;
    quint64 edgeN = 0;
    QVector< QVector<int> > nEvts(crystalNum);
    int pixelCounter = 0;
    for (int i = 0; i < nPixel; ++i)
    {
        for (int j = 0; j < nPixel; ++j)
        {
            if ( ymin<i && i<ymax && xmin<j && j<xmax )
            {
                qreal aN = m_I0(i, j);
                pixelCounter++;
                totalEvts += aN;        

                if ( 1==m_edge(i, j) )
                {
                    edgeN++;
                    edgeEvts += aN;
                }

                int crystalID = m_segr(i, j);
                nEvts[crystalID].append(aN);
            }
        }
    }

    m_IR = 1.0*totalEvts/pixelCounter/(1.0*edgeEvts/edgeN);

    QVector<qreal> RMS;
    for (int iCrystal=0; iCrystal<crystalNum; ++iCrystal)
    {
        // 求每个晶体的rms
        QVector<int> data = nEvts[iCrystal];
        int sum = std::accumulate(data.begin(), data.end(), 0);
        qreal mean = 1.0*sum/data.size();

        // 计算每个数据点与平均值的差值的平方和
        qreal sumSquaredDiff = 0.0;
        for (auto value : data)
        {
            qreal diff = value - mean;
            sumSquaredDiff += diff * diff;
        }

        // 求平方和的平均值
        qreal meanSquaredDiff = sumSquaredDiff / data.size();

        // 取平方根，即均方差
        qreal aRMS = sqrt(meanSquaredDiff);
        RMS.append(aRMS);
    }

    // 求所有晶体的平均RMS
    m_RMS = std::accumulate(RMS.begin(), RMS.end(), 0.0)/RMS.size();
}


void Block::GenPositionLUT()
{
    //根据分割结果，生成位置查找表
    QFile file(fName_LUT_P);
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "打开文件失败，无法生成位置查找表。";
        return;
    }

    QDataStream out(&file);

    // 写入矩阵数据
    for (int iRow = 0; iRow < nPixel; ++iRow)
    {
        for (int iCol = 0; iCol < nPixel; ++iCol)
        {
            out << m_segr(iRow, iCol);
        }
    }

    file.close();
    qDebug() << "位置查找表已生成: " + fName_LUT_P;
}


void Block::GenEnergyLUT()
{
    QFile outFile(fName_LUT_E);
    if(!outFile.open(QIODevice::WriteOnly))
    {
        qDebug() << "打开文件失败，无法生成能量查找表。";
        return;
    }
    QDataStream outStream(&outFile);

    for (int i = 0; i < crystalNum; ++i)
    {
        outStream << m_crystals[i]->GetSlope();
    }

    outFile.close();
    qDebug() << "能量查找表已生成: " + fName_LUT_E;
}


void Block::GenUniformityLUT()
{


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


cv::Mat_<quint16> Block::GetSegResult()
{
    return m_segr;
}


cv::Mat_<qreal> Block::GetSegMap()
{
    return m_segMap;
}


qreal Block::GetER()
{
    m_recEHist->Smooth();
    return m_recEHist->GetResolution();
}


qreal Block::GetIR()
{
    return m_IR;
}


qreal Block::GetRMS()
{
    return m_RMS;
}


void Block::Clear()
{
    m_xList.clear();
    m_yList.clear();
    m_eList.clear();

    for (auto aCrystal : m_crystals)
    {
        aCrystal->Clear();
    }

    m_ADCHist->Clear();
    m_recEHist->Clear();
}


void Block::CalGroupPar()
{
    int n1 = 0;
    int n2 = 0;
    int minDiff = nCrystal; // 初始最小差值为 n

    // 在 2 到 √n 之间搜索因子
    for (int i = 2; i <= qSqrt(nCrystal); ++i)
    {
        if (nCrystal % i == 0)
        {
            int factor1 = i;
            int factor2 = nCrystal / i;
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
        int maxIdx = std::max_element( v.begin(), v.end() ) - v.begin();
        peaks.append(maxIdx);

        int r = 6;
        int start = std::max(maxIdx-r, 0);
        int stop = std::min(maxIdx+r, nPixel-1);
        for (int i=start; i<=stop; ++i)
        {
            v[i] = 0;
        }
    }

    std::sort(peaks.begin(), peaks.end());
    return peaks;
}

