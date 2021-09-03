#ifndef SERIES_EDITOR_DIALOG_HPP
#define SERIES_EDITOR_DIALOG_HPP

#include <QDialog>

#include "ui_curve_editor_dialog.h"

#include "data_series.hpp"


class SeriesEditorDialog : public QDialog
{
    Q_OBJECT

public:
    SeriesEditorDialog(QSharedPointer<DataSeries> series, QWidget *parent = nullptr);
    virtual ~SeriesEditorDialog();

public slots:
    void save();

    void inspectData();
    void setColor();
    void updateColorButton();

protected:
    Ui::series_editor_form ui;

    QSharedPointer<DataSeries> series;

    QColor color;

    void updateSeriesStats();
};

#endif // SERIES_EDITOR_DIALOG_HPP
