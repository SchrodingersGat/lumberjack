#ifndef AXIS_EDIT_DIALOG_H
#define AXIS_EDIT_DIALOG_H

#include <QDialog>

#include "ui_axis_edit_dialog.h"


class AxisEditorDialog : public QDialog
{
    Q_OBJECT

public:
    AxisEditorDialog(int axisId, QString label, double min, double max, QWidget *parent = nullptr);
    virtual ~AxisEditorDialog();

    QString getTitle(void);
    double getMinValue(void);
    double getMaxValue(void);

protected:
    Ui::axis_scale_form ui;

    double min_value = 0;
    double max_value = 0;
};

#endif // AXIS_EDIT_DIALOG_H
