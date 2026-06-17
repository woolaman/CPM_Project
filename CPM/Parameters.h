#pragma once
#include <QString>

extern int nCM;
extern int nBK;
extern int nPixel;
extern int nCrystal;
extern int crystalNum;

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
extern int ADC_cutValue; // value < cutValue is set to 0, for finding the true peak

extern qreal recE_min;
extern qreal recE_max; // keV
extern int recE_nBins;
extern qreal recE_binWidth;
extern qreal recE_cutValue;

extern QString currentPath;