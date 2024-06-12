/********************************************************************************
** Form generated from reading UI file 'ParameterForm.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PARAMETERFORM_H
#define UI_PARAMETERFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ParameterForm
{
public:
    QWidget *layoutWidget;
    QFormLayout *formLayout;
    QLabel *label_nCM;
    QLineEdit *lineEdit_nCM;
    QLabel *label_nBK;
    QLineEdit *lineEdit_nBK;
    QLabel *label_nCrystal;
    QLineEdit *lineEdit_nCrystal;
    QLabel *label_nPixel;
    QLineEdit *lineEdit_nPixel;
    QLabel *label_enlarge;
    QLineEdit *lineEdit_enlarge;
    QLabel *label_bias;
    QLineEdit *lineEdit_bias;
    QLabel *label_xmin;
    QLineEdit *lineEdit_xmin;
    QLabel *label_xmax;
    QLineEdit *lineEdit_xmax;
    QLabel *label_ymin;
    QLineEdit *lineEdit_ymin;
    QLabel *label_ymax;
    QLineEdit *lineEdit_ymax;
    QLabel *label_EW_width;
    QLineEdit *lineEdit_EW_width;
    QLabel *label_ADC_min;
    QLineEdit *lineEdit_ADC_min;
    QLabel *label_ADC_max;
    QLineEdit *lineEdit_ADC_max;
    QLabel *label_ADC_nBins;
    QLineEdit *lineEdit_ADC_nBins;
    QLabel *label_ADC_cutValue;
    QLineEdit *lineEdit_ADC_cutValue;
    QLabel *label_recE_min;
    QLineEdit *lineEdit_recE_min;
    QLabel *label_recE_max;
    QLineEdit *lineEdit_recE_max;
    QLabel *label_recE_nBins;
    QLineEdit *lineEdit_recE_nBins;
    QLabel *label_recE_cutValue;
    QLineEdit *lineEdit_recE_cutValue;
    QLabel *label_segMethod;
    QLineEdit *lineEdit_segMethod;
    QLabel *label_fName_LUT_P;
    QLineEdit *lineEdit_fName_LUT_P;
    QLabel *label_fName_LUT_E;
    QLineEdit *lineEdit_fName_LUT_E;
    QLabel *label_fName_LUT_U;
    QLineEdit *lineEdit_fName_LUT_U;
    QLabel *label_peakE;
    QLineEdit *lineEdit_peakE;
    QPushButton *pushButton_start;

    void setupUi(QWidget *ParameterForm)
    {
        if (ParameterForm->objectName().isEmpty())
            ParameterForm->setObjectName(QString::fromUtf8("ParameterForm"));
        ParameterForm->resize(382, 764);
        QFont font;
        font.setPointSize(10);
        ParameterForm->setFont(font);
        layoutWidget = new QWidget(ParameterForm);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(30, 0, 321, 701));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font1.setPointSize(10);
        layoutWidget->setFont(font1);
        formLayout = new QFormLayout(layoutWidget);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setLabelAlignment(Qt::AlignCenter);
        formLayout->setFormAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        formLayout->setContentsMargins(0, 0, 0, 0);
        label_nCM = new QLabel(layoutWidget);
        label_nCM->setObjectName(QString::fromUtf8("label_nCM"));
        label_nCM->setMinimumSize(QSize(0, 0));
        label_nCM->setFont(font1);
        label_nCM->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(0, QFormLayout::LabelRole, label_nCM);

        lineEdit_nCM = new QLineEdit(layoutWidget);
        lineEdit_nCM->setObjectName(QString::fromUtf8("lineEdit_nCM"));
        lineEdit_nCM->setFont(font1);
        lineEdit_nCM->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(0, QFormLayout::FieldRole, lineEdit_nCM);

        label_nBK = new QLabel(layoutWidget);
        label_nBK->setObjectName(QString::fromUtf8("label_nBK"));
        label_nBK->setMinimumSize(QSize(0, 0));
        label_nBK->setFont(font1);
        label_nBK->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(1, QFormLayout::LabelRole, label_nBK);

        lineEdit_nBK = new QLineEdit(layoutWidget);
        lineEdit_nBK->setObjectName(QString::fromUtf8("lineEdit_nBK"));
        lineEdit_nBK->setFont(font1);
        lineEdit_nBK->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(1, QFormLayout::FieldRole, lineEdit_nBK);

        label_nCrystal = new QLabel(layoutWidget);
        label_nCrystal->setObjectName(QString::fromUtf8("label_nCrystal"));
        label_nCrystal->setFont(font1);
        label_nCrystal->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(2, QFormLayout::LabelRole, label_nCrystal);

        lineEdit_nCrystal = new QLineEdit(layoutWidget);
        lineEdit_nCrystal->setObjectName(QString::fromUtf8("lineEdit_nCrystal"));
        lineEdit_nCrystal->setFont(font1);
        lineEdit_nCrystal->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(2, QFormLayout::FieldRole, lineEdit_nCrystal);

        label_nPixel = new QLabel(layoutWidget);
        label_nPixel->setObjectName(QString::fromUtf8("label_nPixel"));
        label_nPixel->setFont(font1);
        label_nPixel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(3, QFormLayout::LabelRole, label_nPixel);

        lineEdit_nPixel = new QLineEdit(layoutWidget);
        lineEdit_nPixel->setObjectName(QString::fromUtf8("lineEdit_nPixel"));
        lineEdit_nPixel->setFont(font1);
        lineEdit_nPixel->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(3, QFormLayout::FieldRole, lineEdit_nPixel);

        label_enlarge = new QLabel(layoutWidget);
        label_enlarge->setObjectName(QString::fromUtf8("label_enlarge"));
        label_enlarge->setFont(font1);
        label_enlarge->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(4, QFormLayout::LabelRole, label_enlarge);

        lineEdit_enlarge = new QLineEdit(layoutWidget);
        lineEdit_enlarge->setObjectName(QString::fromUtf8("lineEdit_enlarge"));
        lineEdit_enlarge->setFont(font1);
        lineEdit_enlarge->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(4, QFormLayout::FieldRole, lineEdit_enlarge);

        label_bias = new QLabel(layoutWidget);
        label_bias->setObjectName(QString::fromUtf8("label_bias"));
        label_bias->setFont(font1);
        label_bias->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(5, QFormLayout::LabelRole, label_bias);

        lineEdit_bias = new QLineEdit(layoutWidget);
        lineEdit_bias->setObjectName(QString::fromUtf8("lineEdit_bias"));
        lineEdit_bias->setFont(font1);
        lineEdit_bias->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(5, QFormLayout::FieldRole, lineEdit_bias);

        label_xmin = new QLabel(layoutWidget);
        label_xmin->setObjectName(QString::fromUtf8("label_xmin"));
        label_xmin->setFont(font1);
        label_xmin->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(6, QFormLayout::LabelRole, label_xmin);

        lineEdit_xmin = new QLineEdit(layoutWidget);
        lineEdit_xmin->setObjectName(QString::fromUtf8("lineEdit_xmin"));
        lineEdit_xmin->setFont(font1);
        lineEdit_xmin->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(6, QFormLayout::FieldRole, lineEdit_xmin);

        label_xmax = new QLabel(layoutWidget);
        label_xmax->setObjectName(QString::fromUtf8("label_xmax"));
        label_xmax->setFont(font1);
        label_xmax->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(7, QFormLayout::LabelRole, label_xmax);

        lineEdit_xmax = new QLineEdit(layoutWidget);
        lineEdit_xmax->setObjectName(QString::fromUtf8("lineEdit_xmax"));
        lineEdit_xmax->setFont(font1);
        lineEdit_xmax->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(7, QFormLayout::FieldRole, lineEdit_xmax);

        label_ymin = new QLabel(layoutWidget);
        label_ymin->setObjectName(QString::fromUtf8("label_ymin"));
        label_ymin->setFont(font1);
        label_ymin->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(8, QFormLayout::LabelRole, label_ymin);

        lineEdit_ymin = new QLineEdit(layoutWidget);
        lineEdit_ymin->setObjectName(QString::fromUtf8("lineEdit_ymin"));
        lineEdit_ymin->setFont(font1);
        lineEdit_ymin->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(8, QFormLayout::FieldRole, lineEdit_ymin);

        label_ymax = new QLabel(layoutWidget);
        label_ymax->setObjectName(QString::fromUtf8("label_ymax"));
        label_ymax->setFont(font1);
        label_ymax->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(9, QFormLayout::LabelRole, label_ymax);

        lineEdit_ymax = new QLineEdit(layoutWidget);
        lineEdit_ymax->setObjectName(QString::fromUtf8("lineEdit_ymax"));
        lineEdit_ymax->setFont(font1);
        lineEdit_ymax->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(9, QFormLayout::FieldRole, lineEdit_ymax);

        label_EW_width = new QLabel(layoutWidget);
        label_EW_width->setObjectName(QString::fromUtf8("label_EW_width"));
        label_EW_width->setFont(font1);
        label_EW_width->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(10, QFormLayout::LabelRole, label_EW_width);

        lineEdit_EW_width = new QLineEdit(layoutWidget);
        lineEdit_EW_width->setObjectName(QString::fromUtf8("lineEdit_EW_width"));
        lineEdit_EW_width->setFont(font1);
        lineEdit_EW_width->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(10, QFormLayout::FieldRole, lineEdit_EW_width);

        label_ADC_min = new QLabel(layoutWidget);
        label_ADC_min->setObjectName(QString::fromUtf8("label_ADC_min"));
        label_ADC_min->setFont(font1);
        label_ADC_min->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(11, QFormLayout::LabelRole, label_ADC_min);

        lineEdit_ADC_min = new QLineEdit(layoutWidget);
        lineEdit_ADC_min->setObjectName(QString::fromUtf8("lineEdit_ADC_min"));
        lineEdit_ADC_min->setFont(font1);
        lineEdit_ADC_min->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(11, QFormLayout::FieldRole, lineEdit_ADC_min);

        label_ADC_max = new QLabel(layoutWidget);
        label_ADC_max->setObjectName(QString::fromUtf8("label_ADC_max"));
        label_ADC_max->setFont(font1);
        label_ADC_max->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(12, QFormLayout::LabelRole, label_ADC_max);

        lineEdit_ADC_max = new QLineEdit(layoutWidget);
        lineEdit_ADC_max->setObjectName(QString::fromUtf8("lineEdit_ADC_max"));
        lineEdit_ADC_max->setFont(font1);
        lineEdit_ADC_max->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(12, QFormLayout::FieldRole, lineEdit_ADC_max);

        label_ADC_nBins = new QLabel(layoutWidget);
        label_ADC_nBins->setObjectName(QString::fromUtf8("label_ADC_nBins"));
        label_ADC_nBins->setFont(font1);
        label_ADC_nBins->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(13, QFormLayout::LabelRole, label_ADC_nBins);

        lineEdit_ADC_nBins = new QLineEdit(layoutWidget);
        lineEdit_ADC_nBins->setObjectName(QString::fromUtf8("lineEdit_ADC_nBins"));
        lineEdit_ADC_nBins->setFont(font1);
        lineEdit_ADC_nBins->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(13, QFormLayout::FieldRole, lineEdit_ADC_nBins);

        label_ADC_cutValue = new QLabel(layoutWidget);
        label_ADC_cutValue->setObjectName(QString::fromUtf8("label_ADC_cutValue"));
        label_ADC_cutValue->setFont(font1);
        label_ADC_cutValue->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(14, QFormLayout::LabelRole, label_ADC_cutValue);

        lineEdit_ADC_cutValue = new QLineEdit(layoutWidget);
        lineEdit_ADC_cutValue->setObjectName(QString::fromUtf8("lineEdit_ADC_cutValue"));
        lineEdit_ADC_cutValue->setFont(font1);
        lineEdit_ADC_cutValue->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(14, QFormLayout::FieldRole, lineEdit_ADC_cutValue);

        label_recE_min = new QLabel(layoutWidget);
        label_recE_min->setObjectName(QString::fromUtf8("label_recE_min"));
        label_recE_min->setFont(font1);
        label_recE_min->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(16, QFormLayout::LabelRole, label_recE_min);

        lineEdit_recE_min = new QLineEdit(layoutWidget);
        lineEdit_recE_min->setObjectName(QString::fromUtf8("lineEdit_recE_min"));
        lineEdit_recE_min->setFont(font1);
        lineEdit_recE_min->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(16, QFormLayout::FieldRole, lineEdit_recE_min);

        label_recE_max = new QLabel(layoutWidget);
        label_recE_max->setObjectName(QString::fromUtf8("label_recE_max"));
        label_recE_max->setFont(font1);
        label_recE_max->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(17, QFormLayout::LabelRole, label_recE_max);

        lineEdit_recE_max = new QLineEdit(layoutWidget);
        lineEdit_recE_max->setObjectName(QString::fromUtf8("lineEdit_recE_max"));
        lineEdit_recE_max->setFont(font1);
        lineEdit_recE_max->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(17, QFormLayout::FieldRole, lineEdit_recE_max);

        label_recE_nBins = new QLabel(layoutWidget);
        label_recE_nBins->setObjectName(QString::fromUtf8("label_recE_nBins"));
        label_recE_nBins->setFont(font1);
        label_recE_nBins->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(18, QFormLayout::LabelRole, label_recE_nBins);

        lineEdit_recE_nBins = new QLineEdit(layoutWidget);
        lineEdit_recE_nBins->setObjectName(QString::fromUtf8("lineEdit_recE_nBins"));
        lineEdit_recE_nBins->setFont(font1);
        lineEdit_recE_nBins->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(18, QFormLayout::FieldRole, lineEdit_recE_nBins);

        label_recE_cutValue = new QLabel(layoutWidget);
        label_recE_cutValue->setObjectName(QString::fromUtf8("label_recE_cutValue"));
        label_recE_cutValue->setFont(font1);
        label_recE_cutValue->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(19, QFormLayout::LabelRole, label_recE_cutValue);

        lineEdit_recE_cutValue = new QLineEdit(layoutWidget);
        lineEdit_recE_cutValue->setObjectName(QString::fromUtf8("lineEdit_recE_cutValue"));
        lineEdit_recE_cutValue->setFont(font1);
        lineEdit_recE_cutValue->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(19, QFormLayout::FieldRole, lineEdit_recE_cutValue);

        label_segMethod = new QLabel(layoutWidget);
        label_segMethod->setObjectName(QString::fromUtf8("label_segMethod"));
        label_segMethod->setFont(font1);
        label_segMethod->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(20, QFormLayout::LabelRole, label_segMethod);

        lineEdit_segMethod = new QLineEdit(layoutWidget);
        lineEdit_segMethod->setObjectName(QString::fromUtf8("lineEdit_segMethod"));
        lineEdit_segMethod->setFont(font1);
        lineEdit_segMethod->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(20, QFormLayout::FieldRole, lineEdit_segMethod);

        label_fName_LUT_P = new QLabel(layoutWidget);
        label_fName_LUT_P->setObjectName(QString::fromUtf8("label_fName_LUT_P"));
        label_fName_LUT_P->setFont(font1);
        label_fName_LUT_P->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(21, QFormLayout::LabelRole, label_fName_LUT_P);

        lineEdit_fName_LUT_P = new QLineEdit(layoutWidget);
        lineEdit_fName_LUT_P->setObjectName(QString::fromUtf8("lineEdit_fName_LUT_P"));
        lineEdit_fName_LUT_P->setFont(font1);
        lineEdit_fName_LUT_P->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(21, QFormLayout::FieldRole, lineEdit_fName_LUT_P);

        label_fName_LUT_E = new QLabel(layoutWidget);
        label_fName_LUT_E->setObjectName(QString::fromUtf8("label_fName_LUT_E"));
        label_fName_LUT_E->setFont(font1);
        label_fName_LUT_E->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(22, QFormLayout::LabelRole, label_fName_LUT_E);

        lineEdit_fName_LUT_E = new QLineEdit(layoutWidget);
        lineEdit_fName_LUT_E->setObjectName(QString::fromUtf8("lineEdit_fName_LUT_E"));
        lineEdit_fName_LUT_E->setFont(font1);
        lineEdit_fName_LUT_E->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(22, QFormLayout::FieldRole, lineEdit_fName_LUT_E);

        label_fName_LUT_U = new QLabel(layoutWidget);
        label_fName_LUT_U->setObjectName(QString::fromUtf8("label_fName_LUT_U"));
        label_fName_LUT_U->setFont(font1);
        label_fName_LUT_U->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(23, QFormLayout::LabelRole, label_fName_LUT_U);

        lineEdit_fName_LUT_U = new QLineEdit(layoutWidget);
        lineEdit_fName_LUT_U->setObjectName(QString::fromUtf8("lineEdit_fName_LUT_U"));
        lineEdit_fName_LUT_U->setFont(font1);
        lineEdit_fName_LUT_U->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(23, QFormLayout::FieldRole, lineEdit_fName_LUT_U);

        label_peakE = new QLabel(layoutWidget);
        label_peakE->setObjectName(QString::fromUtf8("label_peakE"));
        label_peakE->setFont(font1);
        label_peakE->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(15, QFormLayout::LabelRole, label_peakE);

        lineEdit_peakE = new QLineEdit(layoutWidget);
        lineEdit_peakE->setObjectName(QString::fromUtf8("lineEdit_peakE"));
        lineEdit_peakE->setFont(font1);
        lineEdit_peakE->setCursorPosition(3);
        lineEdit_peakE->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(15, QFormLayout::FieldRole, lineEdit_peakE);

        pushButton_start = new QPushButton(ParameterForm);
        pushButton_start->setObjectName(QString::fromUtf8("pushButton_start"));
        pushButton_start->setGeometry(QRect(29, 710, 321, 48));
        pushButton_start->setFont(font1);

        retranslateUi(ParameterForm);

        QMetaObject::connectSlotsByName(ParameterForm);
    } // setupUi

    void retranslateUi(QWidget *ParameterForm)
    {
        ParameterForm->setWindowTitle(QCoreApplication::translate("ParameterForm", "Form", nullptr));
        label_nCM->setText(QCoreApplication::translate("ParameterForm", "CM \346\225\260\351\207\217", nullptr));
        lineEdit_nCM->setText(QCoreApplication::translate("ParameterForm", "20", nullptr));
        label_nBK->setText(QCoreApplication::translate("ParameterForm", "BK \346\225\260\351\207\217", nullptr));
        lineEdit_nBK->setText(QCoreApplication::translate("ParameterForm", "5", nullptr));
        label_nCrystal->setText(QCoreApplication::translate("ParameterForm", "\346\231\266\344\275\223\346\225\260\351\207\217", nullptr));
        lineEdit_nCrystal->setText(QCoreApplication::translate("ParameterForm", "28", nullptr));
        label_nPixel->setText(QCoreApplication::translate("ParameterForm", "\345\233\276\345\203\217\345\244\247\345\260\217", nullptr));
        lineEdit_nPixel->setText(QCoreApplication::translate("ParameterForm", "512", nullptr));
        label_enlarge->setText(QCoreApplication::translate("ParameterForm", "\345\233\276\345\203\217\346\224\276\345\244\247\345\233\240\345\255\220", nullptr));
        lineEdit_enlarge->setText(QCoreApplication::translate("ParameterForm", "1.06", nullptr));
        label_bias->setText(QCoreApplication::translate("ParameterForm", "\345\233\276\345\203\217\345\201\217\347\247\273\345\244\247\345\260\217", nullptr));
        lineEdit_bias->setText(QCoreApplication::translate("ParameterForm", "58", nullptr));
        label_xmin->setText(QCoreApplication::translate("ParameterForm", "x \346\234\200\345\260\217\345\200\274", nullptr));
        lineEdit_xmin->setText(QCoreApplication::translate("ParameterForm", "0", nullptr));
        label_xmax->setText(QCoreApplication::translate("ParameterForm", "x \346\234\200\345\244\247\345\200\274", nullptr));
        lineEdit_xmax->setText(QCoreApplication::translate("ParameterForm", "512", nullptr));
        label_ymin->setText(QCoreApplication::translate("ParameterForm", "y \346\234\200\345\260\217\345\200\274", nullptr));
        lineEdit_ymin->setText(QCoreApplication::translate("ParameterForm", "0", nullptr));
        label_ymax->setText(QCoreApplication::translate("ParameterForm", "y \346\234\200\345\244\247\345\200\274", nullptr));
        lineEdit_ymax->setText(QCoreApplication::translate("ParameterForm", "512", nullptr));
        label_EW_width->setText(QCoreApplication::translate("ParameterForm", "\350\203\275\347\252\227\345\256\275\345\272\246", nullptr));
        lineEdit_EW_width->setText(QCoreApplication::translate("ParameterForm", "0.25", nullptr));
        label_ADC_min->setText(QCoreApplication::translate("ParameterForm", "ADC \346\234\200\345\260\217\345\200\274", nullptr));
        lineEdit_ADC_min->setText(QCoreApplication::translate("ParameterForm", "0", nullptr));
        label_ADC_max->setText(QCoreApplication::translate("ParameterForm", "ADC \346\234\200\345\244\247\345\200\274", nullptr));
        lineEdit_ADC_max->setText(QCoreApplication::translate("ParameterForm", "60000", nullptr));
        label_ADC_nBins->setText(QCoreApplication::translate("ParameterForm", "ADC \350\203\275\350\260\261 bin \346\225\260\351\207\217", nullptr));
        lineEdit_ADC_nBins->setText(QCoreApplication::translate("ParameterForm", "600", nullptr));
        label_ADC_cutValue->setText(QCoreApplication::translate("ParameterForm", "ADC \346\210\252\346\226\255\345\200\274", nullptr));
        lineEdit_ADC_cutValue->setText(QCoreApplication::translate("ParameterForm", "14000", nullptr));
        label_recE_min->setText(QCoreApplication::translate("ParameterForm", "\350\203\275\351\207\217\346\234\200\345\260\217\345\200\274 (keV) ", nullptr));
        lineEdit_recE_min->setText(QCoreApplication::translate("ParameterForm", "0", nullptr));
        label_recE_max->setText(QCoreApplication::translate("ParameterForm", "\350\203\275\351\207\217\346\234\200\345\244\247\345\200\274 (keV) ", nullptr));
        lineEdit_recE_max->setText(QCoreApplication::translate("ParameterForm", "1600", nullptr));
        label_recE_nBins->setText(QCoreApplication::translate("ParameterForm", "\350\203\275\350\260\261 bin \346\225\260\351\207\217", nullptr));
        lineEdit_recE_nBins->setText(QCoreApplication::translate("ParameterForm", "800", nullptr));
        label_recE_cutValue->setText(QCoreApplication::translate("ParameterForm", "\350\203\275\350\260\261\346\210\252\346\226\255\345\200\274 (keV) ", nullptr));
        lineEdit_recE_cutValue->setText(QCoreApplication::translate("ParameterForm", "200", nullptr));
        label_segMethod->setText(QCoreApplication::translate("ParameterForm", "\345\210\206\345\211\262\346\226\271\346\263\225\357\274\2321 \346\210\226 2", nullptr));
        lineEdit_segMethod->setText(QCoreApplication::translate("ParameterForm", "1", nullptr));
        label_fName_LUT_P->setText(QCoreApplication::translate("ParameterForm", "\344\275\215\347\275\256\346\237\245\346\211\276\350\241\250", nullptr));
        lineEdit_fName_LUT_P->setText(QCoreApplication::translate("ParameterForm", "LUT_Position.bin", nullptr));
        label_fName_LUT_E->setText(QCoreApplication::translate("ParameterForm", "\350\203\275\351\207\217\346\240\241\346\255\243\350\241\250", nullptr));
        lineEdit_fName_LUT_E->setText(QCoreApplication::translate("ParameterForm", "LUT_Energy.bin", nullptr));
        label_fName_LUT_U->setText(QCoreApplication::translate("ParameterForm", "\345\235\207\345\214\200\346\200\247\346\240\241\346\255\243\350\241\250", nullptr));
        lineEdit_fName_LUT_U->setText(QCoreApplication::translate("ParameterForm", "LUT_Uniformity.bin", nullptr));
        label_peakE->setText(QCoreApplication::translate("ParameterForm", "\345\263\260\344\275\215\350\203\275\351\207\217 (keV)", nullptr));
        lineEdit_peakE->setText(QCoreApplication::translate("ParameterForm", "511", nullptr));
        pushButton_start->setText(QCoreApplication::translate("ParameterForm", "\345\220\257\345\212\250", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ParameterForm: public Ui_ParameterForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PARAMETERFORM_H
