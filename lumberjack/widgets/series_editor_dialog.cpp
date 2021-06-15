#include "series_editor_dialog.hpp"


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
}


SeriesEditorDialog::~SeriesEditorDialog()
{
    // TODO
}


void SeriesEditorDialog::save()
{
    // TODO - Show user that the "label" needs to be non empty
    if (ui.label_text->text().isEmpty()) return;

    series->setLabel(ui.label_text->text());
    series->setUnits(ui.units_text->text());

    series->update();

    accept();
}



