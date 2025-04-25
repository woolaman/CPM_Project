/**
 * @class Block
 *
 * block class
 *
*/

#ifndef BLOCK_H
#define BLOCK_H

#include "Crystal.h"
#include "Parameters.h"

#include <opencv2/core.hpp>

class Block
{
public:
    Block();
    Block(int ID);
    ~Block();

    int GetID();
    void SetID(int ID);

    void Fill(quint16 x, quint16 y, quint16 e);

    void CalMap(int EW_min, int EW_max);

    void Segment(SegmentMethod method);
    void Segment1(); // SVD method
    void Segment2(); // find maximum value method
	void Segment3(); 

    void ManualAdjust(QPoint pos);

    void CalSegResult();
    void CalRecEHist();
    void CalSegFOM();

    void GenPositionLUT();
    void GenEnergyLUT();
    void GenUniformityLUT();

    Crystal* GetCrystal(int ID);
    Histogram* GetADCHist();
    Histogram* GetRecEHist();

    cv::Mat_<qreal> GetMap();
    cv::Mat_<cv::Vec2w> GetPeakTable();
    cv::Mat_<quint16> GetSegResult(int n);
    cv::Mat_<qreal> GetSegMap();

    qreal GetER();
    qreal GetPVR();
    qreal GetFWHM();

    qreal CalFWHM(cv::Mat I);

    void Clear();

    cv::Mat_<qreal> GetUniformity();
    QVector<qreal> GetUStatistics();

	void SetMap(cv::Mat_<qreal> I) { m_I0 = I; }
	void SetPeakTable(cv::Mat_<cv::Vec2w> pt) { m_pt = pt; }

private: // private members
    int m_ID;

    QVector<quint16> m_xList;
    QVector<quint16> m_yList;
    QVector<quint16> m_eList;

    QVector<Crystal*> m_crystals;

    Histogram* m_ADCHist;
    Histogram* m_recEHist;

    cv::Mat_<qreal> m_I0;
	cv::Mat_<cv::Vec2w> m_pt;

    cv::Mat_<quint16> seg1;
    cv::Mat_<quint16> seg2;
    cv::Mat_<quint16> seg1_plus;
    cv::Mat_<quint16> seg2_plus;

    cv::Mat_<quint16> m_segr0;
    cv::Mat_<quint16> m_segr1;

    cv::Mat_<quint8> m_edge;
    cv::Mat_<qreal> m_segMap;

    int m_num1;
    int m_num2;

    qreal m_PVR;
    qreal m_FWHM;

    cv::Mat_<qreal> m_uniformity;
    qreal m_uMin;
    qreal m_uMax;
    qreal m_uMean;
    qreal m_uSD; // Standard deviation

    int m_edgeFlag = 0; // 0, 边缘晶体不强行分割成2根晶体，1, random method; 2, line-cut method

private: // private functions
    void CalGroupPar(int N);
    QVector<int> FindPeaks(QVector<qreal> v, int nPeaks);

	// 线性插值函数
	double interp1Linear(const std::vector<double>& x, 
		const std::vector<double>& y, 
		double xq);

	// interp1 函数实现
	std::vector<double> interp1(const std::vector<double>& x,
		const std::vector<double>& y,
		const std::vector<double>& xq);
};

#endif // BLOCK_H