#include "dataview_widget.hpp"


DataviewWidget::DataviewWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);

    auto *manager = DataSourceManager::getInstance();

    // Trigger update whenever the data sources are updated!
    connect(manager, &DataSourceManager::sourcesChanged, this, &DataviewWidget::refresh);

    // Update whenever filter text is changed
    connect(ui.filterText, &QLineEdit::textEdited, this, &DataviewWidget::filterTextUpdated);

    connect(ui.clearButton, &QPushButton::released, this, &DataviewWidget::clearFilter);

    refresh();
}


/**
 * @brief DataviewWidget::clearFilter callback when the "clear filter" button is pressed
 */
void DataviewWidget::clearFilter()
{
    ui.filterText->clear();
}


void DataviewWidget::filterTextUpdated(QString text)
{
    Q_UNUSED(text);

    blockSignals(true);

    refresh();

    blockSignals(false);
}


void DataviewWidget::refresh()
{
    int series_count = ui.treeWidget->refresh(ui.filterText->text());

    ui.resultsLabel->setText(QString::number(series_count) + tr(" series available"));
}
