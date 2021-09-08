#include "axis_scale_dialog.hpp"


AxisScaleDialog::AxisScaleDialog(double min, double max, QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);

    setWindowModality(Qt::ApplicationModal);

    setWindowTitle(tr("Set Axis Scale"));

    min_value = min;
    max_value = max;

    ui.minValue->setText(QString::number(min, 'f', 4));
    ui.minValue->setPlaceholderText(tr("Minimum value"));

    ui.maxValue->setText(QString::number(max, 'f', 4));
    ui.maxValue->setPlaceholderText(tr("Maximum value"));

    connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui.buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}


AxisScaleDialog::~AxisScaleDialog()
{
    // TODO
}


/**
 * @brief AxisScaleDialog::getMinValue returns the "minimum" value
 * @return
 */
double AxisScaleDialog::getMinValue()
{
    bool ok = false;

    double val = ui.minValue->text().toDouble(&ok);

    return ok ? val : min_value;
}


double AxisScaleDialog::getMaxValue()
{
    bool ok = false;

    double val = ui.maxValue->text().toDouble(&ok);

    return ok ? val : max_value;
}
