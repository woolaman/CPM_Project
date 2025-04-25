/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include "LabelFloodmap.h"
#include "LabelHistogram.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLineEdit *lineEdit_dataPath;
    QPushButton *pushButton_readinData;
    QPushButton *pushButton_setEW;
    QLabel *label_chooseBK;
    QPushButton *pushButton_segment;
    QPushButton *pushButton_genPositionLUT;
    QPushButton *pushButton_genEnergyLUT;
    QPushButton *pushButton_calPeaks;
    QPushButton *pushButton_calOnePeak;
    QWidget *layoutWidget1;
    QFormLayout *formLayout_crystalIDEdit;
    QLabel *label_rowID;
    QLabel *label_colID;
    QLineEdit *lineEdit_rowID;
    QLineEdit *lineEdit_colID;
    QWidget *layoutWidget2;
    QFormLayout *formLayout_BKIDEdit;
    QLabel *label_CMID;
    QLabel *label_BKID;
    QLineEdit *lineEdit_CMID;
    QLineEdit *lineEdit_BKID;
    QWidget *layoutWidget3;
    QFormLayout *formLayout_EWEdit;
    QLabel *label_minEW;
    QLabel *label_maxEW;
    QLineEdit *lineEdit_minEW;
    QLineEdit *lineEdit_maxEW;
    QProgressBar *progressBar_readinData;
    LabelFloodmap *label_floodmap;
    LabelHistogram *label_eHist;
    QPushButton *pushButton_calSegFOM;
    QWidget *layoutWidget1_2;
    QFormLayout *formLayout_segFOM;
    QLabel *label_PVR;
    QLabel *label_FWHM;
    QLineEdit *lineEdit_PVR;
    QLineEdit *lineEdit_FWHM;
    QPushButton *pushButton_writePeaks;
    QPushButton *pushButton_calEnergyResolution;
    QPushButton *pushButton_calUniformity;
    QPushButton *pushButton_crystalES;
    QPushButton *pushButton_report;
    QLineEdit *lineEdit_stdDev;
    QLabel *label_stdDev;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1000, 888);
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(10);
        MainWindow->setFont(font);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        lineEdit_dataPath = new QLineEdit(centralwidget);
        lineEdit_dataPath->setObjectName(QString::fromUtf8("lineEdit_dataPath"));
        lineEdit_dataPath->setGeometry(QRect(130, 30, 300, 60));
        lineEdit_dataPath->setFont(font);
        lineEdit_dataPath->setAlignment(Qt::AlignCenter);
        pushButton_readinData = new QPushButton(centralwidget);
        pushButton_readinData->setObjectName(QString::fromUtf8("pushButton_readinData"));
        pushButton_readinData->setGeometry(QRect(30, 30, 100, 60));
        pushButton_readinData->setFont(font);
        pushButton_setEW = new QPushButton(centralwidget);
        pushButton_setEW->setObjectName(QString::fromUtf8("pushButton_setEW"));
        pushButton_setEW->setGeometry(QRect(30, 208, 120, 60));
        pushButton_setEW->setFont(font);
        label_chooseBK = new QLabel(centralwidget);
        label_chooseBK->setObjectName(QString::fromUtf8("label_chooseBK"));
        label_chooseBK->setGeometry(QRect(50, 160, 80, 36));
        label_chooseBK->setFont(font);
        label_chooseBK->setAlignment(Qt::AlignCenter);
        pushButton_segment = new QPushButton(centralwidget);
        pushButton_segment->setObjectName(QString::fromUtf8("pushButton_segment"));
        pushButton_segment->setGeometry(QRect(30, 290, 400, 48));
        pushButton_segment->setFont(font);
        pushButton_genPositionLUT = new QPushButton(centralwidget);
        pushButton_genPositionLUT->setObjectName(QString::fromUtf8("pushButton_genPositionLUT"));
        pushButton_genPositionLUT->setGeometry(QRect(30, 358, 400, 48));
        pushButton_genPositionLUT->setFont(font);
        pushButton_genEnergyLUT = new QPushButton(centralwidget);
        pushButton_genEnergyLUT->setObjectName(QString::fromUtf8("pushButton_genEnergyLUT"));
        pushButton_genEnergyLUT->setGeometry(QRect(30, 426, 400, 48));
        pushButton_genEnergyLUT->setFont(font);
        pushButton_calPeaks = new QPushButton(centralwidget);
        pushButton_calPeaks->setObjectName(QString::fromUtf8("pushButton_calPeaks"));
        pushButton_calPeaks->setGeometry(QRect(30, 490, 132, 40));
        pushButton_calPeaks->setFont(font);
        pushButton_calOnePeak = new QPushButton(centralwidget);
        pushButton_calOnePeak->setObjectName(QString::fromUtf8("pushButton_calOnePeak"));
        pushButton_calOnePeak->setGeometry(QRect(30, 548, 132, 48));
        pushButton_calOnePeak->setFont(font);
        layoutWidget1 = new QWidget(centralwidget);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(180, 550, 251, 48));
        layoutWidget1->setFont(font);
        formLayout_crystalIDEdit = new QFormLayout(layoutWidget1);
        formLayout_crystalIDEdit->setObjectName(QString::fromUtf8("formLayout_crystalIDEdit"));
        formLayout_crystalIDEdit->setLabelAlignment(Qt::AlignCenter);
        formLayout_crystalIDEdit->setFormAlignment(Qt::AlignHCenter|Qt::AlignTop);
        formLayout_crystalIDEdit->setContentsMargins(0, 0, 0, 0);
        label_rowID = new QLabel(layoutWidget1);
        label_rowID->setObjectName(QString::fromUtf8("label_rowID"));
        label_rowID->setFont(font);
        label_rowID->setAlignment(Qt::AlignCenter);

        formLayout_crystalIDEdit->setWidget(0, QFormLayout::LabelRole, label_rowID);

        label_colID = new QLabel(layoutWidget1);
        label_colID->setObjectName(QString::fromUtf8("label_colID"));
        label_colID->setFont(font);
        label_colID->setAlignment(Qt::AlignCenter);

        formLayout_crystalIDEdit->setWidget(0, QFormLayout::FieldRole, label_colID);

        lineEdit_rowID = new QLineEdit(layoutWidget1);
        lineEdit_rowID->setObjectName(QString::fromUtf8("lineEdit_rowID"));
        lineEdit_rowID->setMaximumSize(QSize(120, 16777215));
        lineEdit_rowID->setFont(font);
        lineEdit_rowID->setAlignment(Qt::AlignCenter);

        formLayout_crystalIDEdit->setWidget(1, QFormLayout::LabelRole, lineEdit_rowID);

        lineEdit_colID = new QLineEdit(layoutWidget1);
        lineEdit_colID->setObjectName(QString::fromUtf8("lineEdit_colID"));
        lineEdit_colID->setMaximumSize(QSize(120, 16777215));
        lineEdit_colID->setFont(font);
        lineEdit_colID->setAlignment(Qt::AlignCenter);

        formLayout_crystalIDEdit->setWidget(1, QFormLayout::FieldRole, lineEdit_colID);

        layoutWidget2 = new QWidget(centralwidget);
        layoutWidget2->setObjectName(QString::fromUtf8("layoutWidget2"));
        layoutWidget2->setGeometry(QRect(160, 130, 271, 71));
        layoutWidget2->setFont(font);
        formLayout_BKIDEdit = new QFormLayout(layoutWidget2);
        formLayout_BKIDEdit->setObjectName(QString::fromUtf8("formLayout_BKIDEdit"));
        formLayout_BKIDEdit->setLabelAlignment(Qt::AlignCenter);
        formLayout_BKIDEdit->setFormAlignment(Qt::AlignCenter);
        formLayout_BKIDEdit->setContentsMargins(0, 0, 0, 0);
        label_CMID = new QLabel(layoutWidget2);
        label_CMID->setObjectName(QString::fromUtf8("label_CMID"));
        label_CMID->setFont(font);
        label_CMID->setAlignment(Qt::AlignCenter);

        formLayout_BKIDEdit->setWidget(0, QFormLayout::LabelRole, label_CMID);

        label_BKID = new QLabel(layoutWidget2);
        label_BKID->setObjectName(QString::fromUtf8("label_BKID"));
        label_BKID->setFont(font);
        label_BKID->setAlignment(Qt::AlignCenter);

        formLayout_BKIDEdit->setWidget(0, QFormLayout::FieldRole, label_BKID);

        lineEdit_CMID = new QLineEdit(layoutWidget2);
        lineEdit_CMID->setObjectName(QString::fromUtf8("lineEdit_CMID"));
        lineEdit_CMID->setMaximumSize(QSize(125, 16777215));
        lineEdit_CMID->setFont(font);
        lineEdit_CMID->setAlignment(Qt::AlignCenter);

        formLayout_BKIDEdit->setWidget(1, QFormLayout::LabelRole, lineEdit_CMID);

        lineEdit_BKID = new QLineEdit(layoutWidget2);
        lineEdit_BKID->setObjectName(QString::fromUtf8("lineEdit_BKID"));
        lineEdit_BKID->setMaximumSize(QSize(125, 16777215));
        lineEdit_BKID->setFont(font);
        lineEdit_BKID->setAlignment(Qt::AlignCenter);

        formLayout_BKIDEdit->setWidget(1, QFormLayout::FieldRole, lineEdit_BKID);

        layoutWidget3 = new QWidget(centralwidget);
        layoutWidget3->setObjectName(QString::fromUtf8("layoutWidget3"));
        layoutWidget3->setGeometry(QRect(160, 220, 271, 48));
        layoutWidget3->setFont(font);
        formLayout_EWEdit = new QFormLayout(layoutWidget3);
        formLayout_EWEdit->setObjectName(QString::fromUtf8("formLayout_EWEdit"));
        formLayout_EWEdit->setLabelAlignment(Qt::AlignCenter);
        formLayout_EWEdit->setFormAlignment(Qt::AlignCenter);
        formLayout_EWEdit->setContentsMargins(0, 0, 0, 0);
        label_minEW = new QLabel(layoutWidget3);
        label_minEW->setObjectName(QString::fromUtf8("label_minEW"));
        label_minEW->setFont(font);

        formLayout_EWEdit->setWidget(0, QFormLayout::LabelRole, label_minEW);

        label_maxEW = new QLabel(layoutWidget3);
        label_maxEW->setObjectName(QString::fromUtf8("label_maxEW"));
        label_maxEW->setFont(font);
        label_maxEW->setAlignment(Qt::AlignCenter);

        formLayout_EWEdit->setWidget(0, QFormLayout::FieldRole, label_maxEW);

        lineEdit_minEW = new QLineEdit(layoutWidget3);
        lineEdit_minEW->setObjectName(QString::fromUtf8("lineEdit_minEW"));
        lineEdit_minEW->setMaximumSize(QSize(125, 16777215));
        lineEdit_minEW->setFont(font);
        lineEdit_minEW->setAlignment(Qt::AlignCenter);

        formLayout_EWEdit->setWidget(1, QFormLayout::LabelRole, lineEdit_minEW);

        lineEdit_maxEW = new QLineEdit(layoutWidget3);
        lineEdit_maxEW->setObjectName(QString::fromUtf8("lineEdit_maxEW"));
        lineEdit_maxEW->setMaximumSize(QSize(125, 16777215));
        lineEdit_maxEW->setFont(font);
        lineEdit_maxEW->setAlignment(Qt::AlignCenter);

        formLayout_EWEdit->setWidget(1, QFormLayout::FieldRole, lineEdit_maxEW);

        progressBar_readinData = new QProgressBar(centralwidget);
        progressBar_readinData->setObjectName(QString::fromUtf8("progressBar_readinData"));
        progressBar_readinData->setGeometry(QRect(30, 90, 400, 20));
        progressBar_readinData->setFont(font);
        progressBar_readinData->setValue(0);
        progressBar_readinData->setAlignment(Qt::AlignCenter);
        progressBar_readinData->setTextVisible(true);
        label_floodmap = new LabelFloodmap(centralwidget);
        label_floodmap->setObjectName(QString::fromUtf8("label_floodmap"));
        label_floodmap->setGeometry(QRect(460, 300, 512, 512));
        label_floodmap->setFont(font);
        label_floodmap->setAlignment(Qt::AlignCenter);
        label_eHist = new LabelHistogram(centralwidget);
        label_eHist->setObjectName(QString::fromUtf8("label_eHist"));
        label_eHist->setGeometry(QRect(460, 0, 512, 300));
        label_eHist->setFont(font);
        label_eHist->setAlignment(Qt::AlignCenter);
        pushButton_calSegFOM = new QPushButton(centralwidget);
        pushButton_calSegFOM->setObjectName(QString::fromUtf8("pushButton_calSegFOM"));
        pushButton_calSegFOM->setGeometry(QRect(30, 620, 132, 48));
        pushButton_calSegFOM->setFont(font);
        layoutWidget1_2 = new QWidget(centralwidget);
        layoutWidget1_2->setObjectName(QString::fromUtf8("layoutWidget1_2"));
        layoutWidget1_2->setGeometry(QRect(180, 620, 251, 48));
        layoutWidget1_2->setFont(font);
        formLayout_segFOM = new QFormLayout(layoutWidget1_2);
        formLayout_segFOM->setObjectName(QString::fromUtf8("formLayout_segFOM"));
        formLayout_segFOM->setLabelAlignment(Qt::AlignCenter);
        formLayout_segFOM->setFormAlignment(Qt::AlignHCenter|Qt::AlignTop);
        formLayout_segFOM->setContentsMargins(0, 0, 0, 0);
        label_PVR = new QLabel(layoutWidget1_2);
        label_PVR->setObjectName(QString::fromUtf8("label_PVR"));
        label_PVR->setFont(font);
        label_PVR->setAlignment(Qt::AlignCenter);

        formLayout_segFOM->setWidget(0, QFormLayout::LabelRole, label_PVR);

        label_FWHM = new QLabel(layoutWidget1_2);
        label_FWHM->setObjectName(QString::fromUtf8("label_FWHM"));
        label_FWHM->setFont(font);
        label_FWHM->setAlignment(Qt::AlignCenter);

        formLayout_segFOM->setWidget(0, QFormLayout::FieldRole, label_FWHM);

        lineEdit_PVR = new QLineEdit(layoutWidget1_2);
        lineEdit_PVR->setObjectName(QString::fromUtf8("lineEdit_PVR"));
        lineEdit_PVR->setMaximumSize(QSize(120, 16777215));
        lineEdit_PVR->setFont(font);
        lineEdit_PVR->setAlignment(Qt::AlignCenter);

        formLayout_segFOM->setWidget(1, QFormLayout::LabelRole, lineEdit_PVR);

        lineEdit_FWHM = new QLineEdit(layoutWidget1_2);
        lineEdit_FWHM->setObjectName(QString::fromUtf8("lineEdit_FWHM"));
        lineEdit_FWHM->setMinimumSize(QSize(80, 0));
        lineEdit_FWHM->setMaximumSize(QSize(120, 16777215));
        lineEdit_FWHM->setFont(font);
        lineEdit_FWHM->setAlignment(Qt::AlignCenter);

        formLayout_segFOM->setWidget(1, QFormLayout::FieldRole, lineEdit_FWHM);

        pushButton_writePeaks = new QPushButton(centralwidget);
        pushButton_writePeaks->setObjectName(QString::fromUtf8("pushButton_writePeaks"));
        pushButton_writePeaks->setGeometry(QRect(179, 490, 250, 40));
        pushButton_writePeaks->setFont(font);
        pushButton_calEnergyResolution = new QPushButton(centralwidget);
        pushButton_calEnergyResolution->setObjectName(QString::fromUtf8("pushButton_calEnergyResolution"));
        pushButton_calEnergyResolution->setGeometry(QRect(30, 680, 132, 48));
        pushButton_calEnergyResolution->setFont(font);
        pushButton_calUniformity = new QPushButton(centralwidget);
        pushButton_calUniformity->setObjectName(QString::fromUtf8("pushButton_calUniformity"));
        pushButton_calUniformity->setGeometry(QRect(30, 740, 148, 48));
        pushButton_calUniformity->setFont(font);
        pushButton_crystalES = new QPushButton(centralwidget);
        pushButton_crystalES->setObjectName(QString::fromUtf8("pushButton_crystalES"));
        pushButton_crystalES->setGeometry(QRect(180, 680, 251, 48));
        pushButton_crystalES->setFont(font);
        pushButton_report = new QPushButton(centralwidget);
        pushButton_report->setObjectName(QString::fromUtf8("pushButton_report"));
        pushButton_report->setGeometry(QRect(30, 800, 401, 48));
        pushButton_report->setFont(font);
        lineEdit_stdDev = new QLineEdit(centralwidget);
        lineEdit_stdDev->setObjectName(QString::fromUtf8("lineEdit_stdDev"));
        lineEdit_stdDev->setGeometry(QRect(290, 740, 131, 48));
        lineEdit_stdDev->setAlignment(Qt::AlignCenter);
        label_stdDev = new QLabel(centralwidget);
        label_stdDev->setObjectName(QString::fromUtf8("label_stdDev"));
        label_stdDev->setGeometry(QRect(220, 740, 53, 48));
        label_stdDev->setFont(font);
        label_stdDev->setAlignment(Qt::AlignCenter);
        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        lineEdit_dataPath->setText(QCoreApplication::translate("MainWindow", "\350\276\223\345\205\245\346\226\207\344\273\266", nullptr));
        pushButton_readinData->setText(QCoreApplication::translate("MainWindow", "\350\257\273\345\205\245\346\225\260\346\215\256", nullptr));
        pushButton_setEW->setText(QCoreApplication::translate("MainWindow", "\350\203\275\350\260\261\346\237\245\347\234\213", nullptr));
        label_chooseBK->setText(QCoreApplication::translate("MainWindow", "\346\250\241\345\235\227\351\200\211\346\213\251", nullptr));
        pushButton_segment->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213\345\210\206\345\211\262", nullptr));
        pushButton_genPositionLUT->setText(QCoreApplication::translate("MainWindow", "\347\224\237\346\210\220\344\275\215\347\275\256\346\237\245\346\211\276\350\241\250", nullptr));
        pushButton_genEnergyLUT->setText(QCoreApplication::translate("MainWindow", "\347\224\237\346\210\220\350\203\275\351\207\217\346\240\241\346\255\243\350\241\250", nullptr));
        pushButton_calPeaks->setText(QCoreApplication::translate("MainWindow", "\346\237\245\347\234\213\343\200\201\344\277\256\346\224\271\345\263\260\344\275\215", nullptr));
        pushButton_calOnePeak->setText(QCoreApplication::translate("MainWindow", "\345\215\225\346\240\271\345\263\260\344\275\215", nullptr));
        label_rowID->setText(QCoreApplication::translate("MainWindow", "\350\241\214\345\217\267", nullptr));
        label_colID->setText(QCoreApplication::translate("MainWindow", "\345\210\227\345\217\267", nullptr));
        lineEdit_rowID->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        lineEdit_colID->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        label_CMID->setText(QCoreApplication::translate("MainWindow", "CM_ID", nullptr));
        label_BKID->setText(QCoreApplication::translate("MainWindow", "BK_ID", nullptr));
        lineEdit_CMID->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        lineEdit_BKID->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        label_minEW->setText(QCoreApplication::translate("MainWindow", "EW_min", nullptr));
        label_maxEW->setText(QCoreApplication::translate("MainWindow", "EW_max", nullptr));
        lineEdit_minEW->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        lineEdit_maxEW->setText(QCoreApplication::translate("MainWindow", "65535", nullptr));
        label_floodmap->setText(QCoreApplication::translate("MainWindow", "image area", nullptr));
        label_eHist->setText(QCoreApplication::translate("MainWindow", "eHist area", nullptr));
        pushButton_calSegFOM->setText(QCoreApplication::translate("MainWindow", "\350\256\241\347\256\227\345\210\206\345\211\262\350\264\250\351\207\217", nullptr));
        label_PVR->setText(QCoreApplication::translate("MainWindow", "PVR", nullptr));
        label_FWHM->setText(QCoreApplication::translate("MainWindow", "FWHM", nullptr));
        lineEdit_PVR->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        lineEdit_FWHM->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        pushButton_writePeaks->setText(QCoreApplication::translate("MainWindow", "\351\207\215\346\226\260\347\224\237\346\210\220\350\203\275\351\207\217\346\240\241\346\255\243\350\241\250", nullptr));
        pushButton_calEnergyResolution->setText(QCoreApplication::translate("MainWindow", "\350\256\241\347\256\227\350\203\275\351\207\217\345\210\206\350\276\250\347\216\207", nullptr));
        pushButton_calUniformity->setText(QCoreApplication::translate("MainWindow", "\350\256\241\347\256\227\345\235\207\345\214\200\346\200\247\346\240\241\346\255\243\350\241\250", nullptr));
        pushButton_crystalES->setText(QCoreApplication::translate("MainWindow", "\345\215\225\346\240\271\346\231\266\344\275\223\350\203\275\350\260\261", nullptr));
        pushButton_report->setText(QCoreApplication::translate("MainWindow", "\347\224\237\346\210\220\350\264\250\346\216\247\346\212\245\345\221\212", nullptr));
        lineEdit_stdDev->setText(QString());
        label_stdDev->setText(QCoreApplication::translate("MainWindow", "\346\240\207\345\207\206\345\267\256", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
