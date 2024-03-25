#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <QString>

extern int nCM;
extern int nBK;
extern int nPixel;
extern int nCrystal;
extern int crystalNum;

extern qreal factor; // 调整点阵大小
extern int bias;  // 调整点阵位置

// choose central area to analysis
extern int xmin; //int xmin = 0;
extern int xmax; //int xmax = 512;
extern int ymin; //int ymin = 0;
extern int ymax; //int ymax = 512;

extern QString segmentMethod;

extern qreal EW_width;
extern int ADC_min;
extern int ADC_max; // ADC channel value
extern int ADC_nBins;
extern int ADC_binWidth;
extern int ADC_cutValue; // 将小于该值的bin置为0，防止寻峰错误

extern int recE_min;
extern int recE_max; // keV
extern int recE_nBins;
extern int recE_binWidth;
extern int recE_cutValue; // 将小于该值的bin置为0，防止寻峰错误

extern QString currentPath;
extern QString fName_LUT_P;
extern QString fName_LUT_E;
extern QString fName_LUT_U;

extern int peakE;

#endif // PARAMETERS_H
