#ifndef PARAMETERFORM_H
#define PARAMETERFORM_H

#include <QWidget>


namespace Ui
{
class ParameterForm;
}


class ParameterForm : public QWidget
{
    Q_OBJECT

public:
    explicit ParameterForm(QWidget *parent = nullptr);
    ~ParameterForm();


signals:
    void Start();


private slots:
    void on_lineEdit_nCM_editingFinished();
    void on_lineEdit_nBK_editingFinished();
    void on_lineEdit_nCrystal_editingFinished();
    void on_lineEdit_nPixel_editingFinished();

    void on_lineEdit_enlargeFactor_editingFinished();
    void on_lineEdit_bias_editingFinished();

    void on_lineEdit_xmin_editingFinished();
    void on_lineEdit_xmax_editingFinished();
    void on_lineEdit_ymin_editingFinished();
    void on_lineEdit_ymax_editingFinished();

    void on_lineEdit_EW_width_editingFinished();

    void on_lineEdit_ADC_min_editingFinished();
    void on_lineEdit_ADC_max_editingFinished();
    void on_lineEdit_ADC_nBins_editingFinished();
    void on_lineEdit_ADC_cutValue_editingFinished();

    void on_lineEdit_peakE_editingFinished();

    void on_lineEdit_recE_min_editingFinished();
    void on_lineEdit_recE_max_editingFinished();
    void on_lineEdit_recE_nBins_editingFinished();
    void on_lineEdit_recE_cutValue_editingFinished();

    void on_lineEdit_segMethod_editingFinished();

    void on_lineEdit_fName_LUT_P_editingFinished();
    void on_lineEdit_fName_LUT_E_editingFinished();
    void on_lineEdit_fName_LUT_U_editingFinished();

    void on_pushButton_start_clicked();


private:
    Ui::ParameterForm *ui;

};

#endif // PARAMETERFORM_H
