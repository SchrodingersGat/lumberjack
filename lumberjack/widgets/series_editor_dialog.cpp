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

    ui.scaling->setValue(series->getScaler());
    ui.offset->setValue(series->getOffset());

    connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &SeriesEditorDialog::save);
    connect(ui.buttonBox, &QDialogButtonBox::rejected, this, &SeriesEditorDialog::reject);

    connect(ui.inspectData, &QPushButton::released, this, &SeriesEditorDialog::inspectData);
    connect(ui.line_color, &QPushButton::released, this, &SeriesEditorDialog::setColor);

    color = series->getColor();

    ui.line_width->setMinimum(DataSeries::LINE_WIDTH_MIN * 2);
    ui.line_width->setMaximum(DataSeries::LINE_WIDTH_MAX * 2);

    ui.line_width->setValue(series->getLineWidth() * 2);

    // Line style options
    ui.line_style->addItem(tr("No Line"), (int) Qt::NoPen);
    ui.line_style->addItem(tr("Solid"), (int) Qt::SolidLine);
    ui.line_style->addItem(tr("Dashed"), (int) Qt::DashLine);
    ui.line_style->addItem(tr("Dotted"), (int) Qt::DotLine);

    for (int idx = 0; idx < ui.line_style->count(); idx++)
    {
        if (series->getLineStyle() == ui.line_style->itemData(idx))
        {
            ui.line_style->setCurrentIndex(idx);
            break;
        }
    }

    // Symbol style options
    ui.symbol_style->addItem(tr("No Symbol"), (int) QwtSymbol::NoSymbol);
    ui.symbol_style->addItem(tr("Circle"), (int) QwtSymbol::Ellipse);
    ui.symbol_style->addItem(tr("Square"), (int) QwtSymbol::Rect);
    ui.symbol_style->addItem(tr("Triangle"), (int) QwtSymbol::Triangle);
    ui.symbol_style->addItem(tr("Diamond"), (int) QwtSymbol::Diamond);
    ui.symbol_style->addItem(tr("Cross"), (int) QwtSymbol::Cross);
    ui.symbol_style->addItem(tr("Star"), (int) QwtSymbol::Star2);

    for (int idx = 0; idx < ui.symbol_style->count(); idx++)
    {
        if (series->getSymbolStyle() == ui.symbol_style->itemData(idx))
        {
            ui.symbol_style->setCurrentIndex(idx);
            break;
        }
    }

    ui.symbol_size->setMinimum(DataSeries::SYMBOL_SIZE_MIN);
    ui.symbol_size->setMaximum(DataSeries::SYMBOL_SIZE_MAX);
    ui.symbol_size->setValue(series->getSymbolSize());

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

    series->setScaler(ui.scaling->value(), false);
    series->setOffset(ui.offset->value(), false);

    series->setColor(color);

    series->setLineWidth((float) ui.line_width->value() / 2);

    series->setLineStyle(ui.line_style->currentData().toInt());
    series->setSymbolStyle(ui.symbol_style->currentData().toInt());
    series->setSymbolSize(ui.symbol_size->value());

    series->update();
    series->updateStyle();

    accept();
}



