#include "ParameterForm.h"
#include "ui_ParameterForm.h"
#include "Parameters.h"
#include "MainWindow.h"

#include <QFile>
#include <QDebug>


ParameterForm::ParameterForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ParameterForm)
{
    ui->setupUi(this);

    ui->lineEdit_nCM->setText(QString::number(nCM));
    ui->lineEdit_nBK->setText(QString::number(nBK));
    ui->lineEdit_nCrystal->setText(QString::number(nCrystal));
    ui->lineEdit_nPixel->setText(QString::number(nPixel));

    ui->lineEdit_enlarge->setText(QString::number(enlarge, 'f', 2));
    ui->lineEdit_bias->setText(QString::number(bias));

    ui->lineEdit_xmin->setText(QString::number(xmin));
    ui->lineEdit_xmax->setText(QString::number(xmax));
    ui->lineEdit_ymin->setText(QString::number(ymin));
    ui->lineEdit_ymax->setText(QString::number(ymax));

    ui->lineEdit_EW_width->setText(QString::number(EW_width, 'f', 2));

    ui->lineEdit_segMethod->setText(
        QString::number(static_cast<int>(segMethod)));

    ui->lineEdit_peakE->setText(QString::number(peakE, 'f', 2));

    ui->lineEdit_ADC_min->setText(QString::number(ADC_min));
    ui->lineEdit_ADC_max->setText(QString::number(ADC_max));
    ui->lineEdit_ADC_nBins->setText(QString::number(ADC_nBins));
    ui->lineEdit_ADC_cutValue->setText(QString::number(ADC_cutValue));

    ui->lineEdit_recE_min->setText(QString::number(recE_min, 'f', 1));
    ui->lineEdit_recE_max->setText(QString::number(recE_max));
    ui->lineEdit_recE_nBins->setText(QString::number(recE_nBins));
    ui->lineEdit_recE_cutValue->setText(QString::number(recE_cutValue));

    ui->lineEdit_fName_LUT_P->setText(fName_LUT_P);
    ui->lineEdit_fName_LUT_E->setText(fName_LUT_E);
    ui->lineEdit_fName_LUT_U->setText(fName_LUT_U);
}


ParameterForm::~ParameterForm()
{
    delete ui;
}


void ParameterForm::on_lineEdit_nCM_editingFinished()
{
    nCM = ui->lineEdit_nCM->text().toInt();
}


void ParameterForm::on_lineEdit_nBK_editingFinished()
{
    nBK = ui->lineEdit_nBK->text().toInt();
}


void ParameterForm::on_lineEdit_nCrystal_editingFinished()
{
    nCrystal = ui->lineEdit_nCrystal->text().toInt();
}


void ParameterForm::on_lineEdit_nPixel_editingFinished()
{
    nPixel = ui->lineEdit_nPixel->text().toInt();
}


void ParameterForm::on_lineEdit_enlarge_editingFinished()
{
    enlarge = ui->lineEdit_enlarge->text().toDouble();
}


void ParameterForm::on_lineEdit_bias_editingFinished()
{
    bias = ui->lineEdit_bias->text().toInt();
}


void ParameterForm::on_lineEdit_xmin_editingFinished()
{
    xmin = ui->lineEdit_xmin->text().toInt();
}


void ParameterForm::on_lineEdit_xmax_editingFinished()
{
    xmax = ui->lineEdit_xmax->text().toInt();
}


void ParameterForm::on_lineEdit_ymin_editingFinished()
{
    ymin = ui->lineEdit_ymin->text().toInt();
}


void ParameterForm::on_lineEdit_ymax_editingFinished()
{
    ymax = ui->lineEdit_ymax->text().toInt();
}


void ParameterForm::on_lineEdit_EW_width_editingFinished()
{
    EW_width = ui->lineEdit_EW_width->text().toDouble();
}


void ParameterForm::on_lineEdit_ADC_min_editingFinished()
{
    ADC_min = ui->lineEdit_ADC_min->text().toInt();
}


void ParameterForm::on_lineEdit_ADC_max_editingFinished()
{
    ADC_max = ui->lineEdit_ADC_max->text().toInt();
}


void ParameterForm::on_lineEdit_ADC_nBins_editingFinished()
{
    ADC_nBins = ui->lineEdit_ADC_nBins->text().toInt();
}


void ParameterForm::on_lineEdit_ADC_cutValue_editingFinished()
{
    ADC_cutValue = ui->lineEdit_ADC_cutValue->text().toInt();
}


void ParameterForm::on_lineEdit_peakE_editingFinished()
{
    peakE = ui->lineEdit_peakE->text().toDouble();
}


void ParameterForm::on_lineEdit_recE_min_editingFinished()
{
    recE_min = ui->lineEdit_recE_min->text().toDouble();
}


void ParameterForm::on_lineEdit_recE_max_editingFinished()
{
    recE_max = ui->lineEdit_recE_max->text().toDouble();
}


void ParameterForm::on_lineEdit_recE_nBins_editingFinished()
{
    recE_nBins = ui->lineEdit_recE_nBins->text().toInt();
}


void ParameterForm::on_lineEdit_recE_cutValue_editingFinished()
{
    recE_cutValue = ui->lineEdit_recE_cutValue->text().toDouble();
}


void ParameterForm::on_lineEdit_segMethod_editingFinished()
{
    int ID = ui->lineEdit_segMethod->text().toInt();

    switch (ID)
    {
    case 1:
        segMethod = SegmentMethod::SVD;
        break;
    case 2:
        segMethod = SegmentMethod::FindMaximum;
        break;
    default:
        qCritical() << "Segment method value is illegal";
    }
}


void ParameterForm::on_lineEdit_fName_LUT_P_editingFinished()
{
    fName_LUT_P = ui->lineEdit_fName_LUT_P->text();
}


void ParameterForm::on_lineEdit_fName_LUT_E_editingFinished()
{
    fName_LUT_E = ui->lineEdit_fName_LUT_E->text();
}


void ParameterForm::on_lineEdit_fName_LUT_U_editingFinished()
{
    fName_LUT_U = ui->lineEdit_fName_LUT_U->text();
}


void ParameterForm::on_pushButton_start_clicked()
{
    crystalNum = nCrystal * nCrystal;
    ADC_binWidth = (ADC_max-ADC_min)/ADC_nBins;
    recE_binWidth = (recE_max-recE_min)/recE_nBins;

    MainWindow* mainWindow = new MainWindow(this);
    mainWindow->setWindowTitle("CPM: crystal position map program");
    int x = this->pos().x();
    int y = this->pos().y();
    int width = this->geometry().width();
    // int height = this->geometry().height();
    mainWindow->move(x+width+20, y);
    mainWindow->show();
}



void ParameterForm::closeEvent(QCloseEvent *event)
{
    // 执行一些操作
    qDebug() << "Window is closing. Performing some operations...";

    qDebug() << "Save parameters to file.";
    QString fName = currentPath + ".config.par";
    QFile file(fName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);

        out << "# Parameter setup file \n" << Qt::endl;

        out << "nCM = " + QString::number(nCM) + "\n";
        out << "nBK = " + QString::number(nBK) + "\n";
        out << "nCrystal = " + QString::number(nCrystal) + "\n";
        out << "nPixel = " + QString::number(nPixel) + "\n" << Qt::endl;

        out << "enlarge = " + QString::number(enlarge, 'f', 2) + "\n";
        out << "bias = " + QString::number(bias) + "\n" << Qt::endl;

        out << "xmin = " + QString::number(xmin) + "\n";
        out << "xmax = " + QString::number(xmax) + "\n";
        out << "ymin = " + QString::number(ymin) + "\n";
        out << "ymax = " + QString::number(ymax) + "\n" << Qt::endl;

        out << "EW_width = " + QString::number(EW_width, 'f', 2)
                   + "\n" << Qt::endl;

        out << "segmentMethod = " +
                   QString::number(static_cast<int>(segMethod))
                   + "\n" << Qt::endl;

        out << "peakE = " + QString::number(peakE, 'f', 2)  + "\n" << Qt::endl;

        out << "ADC_min = " + QString::number(ADC_min)  + "\n";
        out << "ADC_max = " + QString::number(ADC_max)  + "\n";
        out << "ADC_nBins = " + QString::number(ADC_nBins)  + "\n";
        out << "ADC_cutValue = " + QString::number(ADC_cutValue)
                   + "\n" << Qt::endl;

        out << "recE_min = " + QString::number(recE_min)  + "\n";
        out << "recE_max = " + QString::number(recE_max)  + "\n";
        out << "recE_nBins = " + QString::number(recE_nBins)  + "\n";
        out << "recE_cutValue = " + QString::number(recE_cutValue)
                   + "\n" << Qt::endl;

        out << "Position LUT = " + fName_LUT_P + "\n";
        out << "Energy LUT = " + fName_LUT_E + "\n";
        out << "Uniformity LUT = " + fName_LUT_U + "\n" << Qt::endl;
    }
    else
    {
        qDebug() << "Fail to save parameters.";
    }

    file.close();

    // 调用父类的 closeEvent 方法继续执行默认的关闭操作
    QWidget::closeEvent(event);
}

