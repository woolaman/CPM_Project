#pragma once
#include <QWidget>

namespace Ui
{
    class ParameterForm;
}

class ParameterForm : public QWidget
{
    Q_OBJECT

public:
    explicit ParameterForm(QWidget* parent = nullptr);
    ~ParameterForm();

private slots:
    void on_lineEdit_nCM_editingFinished();
    void on_lineEdit_nBK_editingFinished();
    void on_lineEdit_nCrystal_editingFinished();
    void on_lineEdit_nPixel_editingFinished();

    void on_lineEdit_EW_width_editingFinished();

    void on_lineEdit_segMethod_editingFinished();

    void on_lineEdit_peakE_editingFinished();

    void on_lineEdit_ADC_min_editingFinished();
    void on_lineEdit_ADC_max_editingFinished();
    void on_lineEdit_ADC_nBins_editingFinished();
    void on_lineEdit_ADC_cutValue_editingFinished();

    void on_lineEdit_recE_min_editingFinished();
    void on_lineEdit_recE_max_editingFinished();
    void on_lineEdit_recE_nBins_editingFinished();
    void on_lineEdit_recE_cutValue_editingFinished();

    void on_pushButton_start_clicked();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::ParameterForm* ui;
};