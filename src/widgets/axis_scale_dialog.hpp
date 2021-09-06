#ifndef AXIS_SCALE_DIALOG_HPP
#define AXIS_SCALE_DIALOG_HPP

#include <QDialog>

#include "ui_axis_scale_dialog.h"


class AxisScaleDialog : public QDialog
{
    Q_OBJECT

public:
    AxisScaleDialog(double min, double max, QWidget *parent = nullptr);
    virtual ~AxisScaleDialog();

    double getMinValue(void);
    double getMaxValue(void);

protected:
    Ui::axis_scale_form ui;

    double min_value = 0;
    double max_value = 0;
};

#endif // AXIS_SCALE_DIALOG_HPP
