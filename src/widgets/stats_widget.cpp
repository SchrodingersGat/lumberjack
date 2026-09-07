#include <QHeaderView>

#include "stats_widget.hpp"


StatsWidget::StatsWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);

    setWindowTitle("Statistics");

    model = new QStandardItemModel(this);
    proxyModel = new StatsFilterProxyModel(this);

    proxyModel->setSourceModel(model);
    proxyModel->setSortRole(StatsRoles::ValueRole);

    ui.statsTable->setModel(proxyModel);
    ui.statsTable->horizontalHeader()->setSectionsClickable(true);
    ui.statsTable->setSortingEnabled(true);

    initTable();

    ui.valueColumnCombo->addItem(tr("Min"), ColumnMin);
    ui.valueColumnCombo->addItem(tr("Max"), ColumnMax);
    ui.valueColumnCombo->addItem(tr("Mean"), ColumnMean);

    ui.valueOpCombo->addItem(">", StatsFilterProxyModel::GreaterThan);
    ui.valueOpCombo->addItem(QString::fromUtf8("\xE2\x89\xA5"), StatsFilterProxyModel::GreaterOrEqual);
    ui.valueOpCombo->addItem("<", StatsFilterProxyModel::LessThan);
    ui.valueOpCombo->addItem(QString::fromUtf8("\xE2\x89\xA4"), StatsFilterProxyModel::LessOrEqual);
    ui.valueOpCombo->addItem("=", StatsFilterProxyModel::Equal);

    connect(ui.nameFilterEdit, &QLineEdit::textChanged, this, &StatsWidget::onNameFilterChanged);

    connect(ui.valueFilterCheck, &QCheckBox::toggled, ui.valueColumnCombo, &QWidget::setEnabled);
    connect(ui.valueFilterCheck, &QCheckBox::toggled, ui.valueOpCombo, &QWidget::setEnabled);
    connect(ui.valueFilterCheck, &QCheckBox::toggled, ui.valueThresholdSpin, &QWidget::setEnabled);

    connect(ui.valueFilterCheck, &QCheckBox::toggled, this, &StatsWidget::onValueFilterChanged);
    connect(ui.valueColumnCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &StatsWidget::onValueFilterChanged);
    connect(ui.valueOpCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &StatsWidget::onValueFilterChanged);
    connect(ui.valueThresholdSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &StatsWidget::onValueFilterChanged);

    connect(ui.clearFiltersButton, &QPushButton::clicked, this, &StatsWidget::onClearFilters);
}


void StatsWidget::initTable()
{
    QStringList headers;

    headers << tr("Series");
    headers << tr("Min");
    headers << tr("Max");
    headers << tr("Mean");

    model->setColumnCount(headers.length());
    model->setHorizontalHeaderLabels(headers);
}


void StatsWidget::updateStats(const QList<DataSeriesPointer> &seriesList, const QwtInterval &interval)
{
    double tMin = interval.minValue();
    double tMax = interval.maxValue();

    model->setRowCount(seriesList.count());

    // TODO: Make this a threaded function, could take a long time to calculate

    for (int idx = 0; idx < seriesList.count(); idx++)
    {
        auto series = seriesList.at(idx);

        if (series.isNull()) continue;

        // A series only has data "in range" if at least one sample falls between
        // tMin and tMax - matches the indexing DataSeries::getMinimumValue/getMaximumValue
        // use internally, since those return meaningless sentinel values otherwise
        bool hasData = false;

        if (series->size() > 0)
        {
            auto idxMin = series->getIndexForTimestamp(tMin, DataSeries::SEARCH_RIGHT_TO_LEFT);
            auto idxMax = series->getIndexForTimestamp(tMax, DataSeries::SEARCH_RIGHT_TO_LEFT);

            hasData = (idxMin + 1) <= idxMax;
        }

        auto nameItem = new QStandardItem(series->getLabel());
        nameItem->setEditable(false);
        nameItem->setData(series->getLabel(), StatsRoles::ValueRole);
        model->setItem(idx, ColumnSeries, nameItem);

        if (hasData)
        {
            setColumnValue(idx, ColumnMin, series->getMinimumValue(tMin, tMax), true);
            setColumnValue(idx, ColumnMax, series->getMaximumValue(tMin, tMax), true);
            setColumnValue(idx, ColumnMean, series->getMeanValue(tMin, tMax), true);
        }
        else
        {
            setColumnValue(idx, ColumnMin, 0.0, false);
            setColumnValue(idx, ColumnMax, 0.0, false);
            setColumnValue(idx, ColumnMean, 0.0, false);
        }
    }
}


void StatsWidget::setColumnValue(int row, int column, double value, bool hasData)
{
    auto item = new QStandardItem(hasData ? QString::number(value) : QString::fromUtf8("\xE2\x80\x93"));
    item->setEditable(false);

    if (hasData)
    {
        item->setData(value, StatsRoles::ValueRole);
    }

    model->setItem(row, column, item);
}


void StatsWidget::onNameFilterChanged(const QString &text)
{
    proxyModel->setNameFilter(text);
}


void StatsWidget::onValueFilterChanged()
{
    if (!ui.valueFilterCheck->isChecked())
    {
        proxyModel->clearValueFilter();
        return;
    }

    int column = ui.valueColumnCombo->currentData().toInt();
    auto op = static_cast<StatsFilterProxyModel::CompareOp>(ui.valueOpCombo->currentData().toInt());
    double threshold = ui.valueThresholdSpin->value();

    proxyModel->setValueFilter(column, op, threshold);
}


void StatsWidget::onClearFilters()
{
    ui.nameFilterEdit->clear();
    ui.valueFilterCheck->setChecked(false);
    ui.valueColumnCombo->setCurrentIndex(0);
    ui.valueOpCombo->setCurrentIndex(0);
    ui.valueThresholdSpin->setValue(0.0);
}
