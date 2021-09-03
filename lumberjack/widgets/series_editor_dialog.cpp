#include <qcolordialog.h>

#include "series_editor_dialog.hpp"
#include "datatable_widget.hpp"


SeriesEditorDialog::SeriesEditorDialog(QSharedPointer<DataSeries> s, QWidget *parent) : QDialog(parent), series(s)
{
    ui.setupUi(this);

    setWindowModality(Qt::WindowModality::ApplicationModal);
    setWindowTitle(tr("Edit Series"));

    ui.label_text->setText(series->getLabel());
    ui.label_text->setPlaceholderText(tr("Set series label"));

    ui.units_text->setText(series->getUnits());
    ui.units_text->setPlaceholderText(tr("Set series units"));

    connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &SeriesEditorDialog::save);
    connect(ui.buttonBox, &QDialogButtonBox::rejected, this, &SeriesEditorDialog::reject);

    connect(ui.inspectData, &QPushButton::released, this, &SeriesEditorDialog::inspectData);
    connect(ui.line_color, &QPushButton::released, this, &SeriesEditorDialog::setColor);

    color = series->getColor();

    updateSeriesStats();
    updateColorButton();
}


SeriesEditorDialog::~SeriesEditorDialog()
{
    // TODO
}


void SeriesEditorDialog::setColor()
{
    auto *dlg = new QColorDialog(color);

    int result = dlg->exec();

    if (result == QDialog::Accepted)
    {
        color = dlg->selectedColor();
    }

    dlg->deleteLater();

    updateColorButton();
}


void SeriesEditorDialog::updateColorButton()
{
    // Sets the background color of the "set color" button

    QString ss = "background-color: " + color.name();

    ui.line_color->setStyleSheet(ss);

    ui.line_color->update();
}


void SeriesEditorDialog::updateSeriesStats()
{
    auto n = series->size();

    ui.nPoints->setText(QString::number(n));

    ui.minValue->setText(n > 0 ? QString::number(series->getMinimumValue()) : "---");
    ui.maxValue->setText(n > 0 ? QString::number(series->getMaximumValue()) : "---");

    ui.minTime->setText(n > 0 ? QString::number(series->getOldestTimestamp()) : "---");
    ui.maxTime->setText(n > 0 ? QString::number(series->getNewestTimestamp()) : "---");
}


void SeriesEditorDialog::inspectData()
{
    DataSeriesTableView* table = new DataSeriesTableView(series);

    table->show();
}


void SeriesEditorDialog::save()
{
    // TODO - Show user that the "label" needs to be non empty
    if (ui.label_text->text().isEmpty()) return;

    series->setLabel(ui.label_text->text());
    series->setUnits(ui.units_text->text());

    series->setColor(color);

    series->update();
    series->updateStyle();

    accept();
}



