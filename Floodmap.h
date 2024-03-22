#ifndef FLOODMAP_H
#define FLOODMAP_H

#include <opencv2/core.hpp>
#include <QtCore/QtCore>

class Floodmap
{
public:
    Floodmap();
    ~Floodmap();


    cv::Mat_<quint16> GetSegResult();


    void SetCrysatlN(int n);
    void SetMap(cv::Mat_<qreal> I);

    void Fill(int x, int y);


private:
    int nPixel = 512;
    int m_nCrystal;
    cv::Mat_<qreal> I0;
    cv::Mat_<quint16> m_segr;

};

#endif // FLOODMAP_H
