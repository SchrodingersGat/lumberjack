#include "axis_edit_dialog.hpp"


AxisEditorDialog::AxisEditorDialog(int axisId, QString title, double min, double max, QWidget *parent) : QDialog(parent)
{
    Q_UNUSED(axisId);

    ui.setupUi(this);

    setWindowModality(Qt::ApplicationModal);

    setWindowTitle(tr("Set Axis Scale"));

    min_value = min;
    max_value = max;

    ui.axisTitle->setText(title);

    ui.minValue->setText(QString::number(min, 'f', 4));
    ui.minValue->setPlaceholderText(tr("Minimum value"));

    ui.maxValue->setText(QString::number(max, 'f', 4));
    ui.maxValue->setPlaceholderText(tr("Maximum value"));

    connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui.buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}


AxisEditorDialog::~AxisEditorDialog()
{
    // TODO
}


/**
 * @brief AxisEditorDialog::getTitle returns the "title" for the axis
 * @return
 */
QString AxisEditorDialog::getTitle()
{
    return ui.axisTitle->text().trimmed();
}


/**
 * @brief AxisScaleDialog::getMinValue returns the "minimum" value
 * @return
 */
double AxisEditorDialog::getMinValue()
{
    bool ok = false;

    double val = ui.minValue->text().toDouble(&ok);

    return ok ? val : min_value;
}


double AxisEditorDialog::getMaxValue()
{
    bool ok = false;

    double val = ui.maxValue->text().toDouble(&ok);

    return ok ? val : max_value;
}
