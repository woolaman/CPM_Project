#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <QString>

extern int nCM;
extern int nBK;
extern int nPixel;
extern int nCrystal;
extern int crystalNum;

extern qreal enlarge; // 调整点阵大小
extern int bias; // 调整点阵位置

// choose central area to analysis
extern int xmin;
extern int xmax;
extern int ymin;
extern int ymax;

extern qreal EW_width;

enum class SegmentMethod
{
    SVD = 1,
    FindMaximum = 2,
};

extern SegmentMethod segMethod;

extern qreal peakE;

extern int ADC_min;
extern int ADC_max; // ADC channel value
extern int ADC_nBins;
extern qreal ADC_binWidth;
extern int ADC_cutValue; // 将小于该值的bin置为0，防止寻峰错误

extern qreal recE_min;
extern qreal recE_max; // keV
extern int recE_nBins;
extern qreal recE_binWidth;
extern qreal recE_cutValue; // 将小于该值的bin置为0，防止寻峰错误

extern QString currentPath;
extern QString fName_LUT_P;
extern QString fName_LUT_E;
extern QString fName_LUT_U;

#endif // PARAMETERS_H
