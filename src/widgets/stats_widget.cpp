#include <QHeaderView>

#include "flow_layout.hpp"
#include "stats_widget.hpp"


StatsWidget::StatsWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);

    setWindowTitle("Statistics");

    // The filter row's controls are laid out in code (rather than in the .ui)
    // so they wrap onto additional rows instead of forcing the panel wide
    // when the Stats View is narrowed
    auto filterFlowLayout = new FlowLayout(ui.filterContainer, 0, 6, 6);
    filterFlowLayout->addWidget(ui.rangeModeCombo);
    filterFlowLayout->addWidget(ui.nameFilterEdit);
    filterFlowLayout->addWidget(ui.valueFilterCheck);
    filterFlowLayout->addWidget(ui.valueColumnCombo);
    filterFlowLayout->addWidget(ui.valueOpCombo);
    filterFlowLayout->addWidget(ui.valueThresholdSpin);
    filterFlowLayout->addWidget(ui.clearFiltersButton);

    model = new QStandardItemModel(this);
    proxyModel = new StatsFilterProxyModel(this);

    proxyModel->setSourceModel(model);
    proxyModel->setSortRole(StatsRoles::ValueRole);

    ui.statsTable->setModel(proxyModel);
    ui.statsTable->horizontalHeader()->setSectionsClickable(true);
    ui.statsTable->setSortingEnabled(true);

    initTable();

    ui.rangeModeCombo->addItem(tr("Visible view"), RangeVisible);
    ui.rangeModeCombo->addItem(tr("Whole trace"), RangeWholeTrace);

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

    connect(ui.rangeModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &StatsWidget::onRangeModeChanged);
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
    lastSeriesList = seriesList;
    lastViewInterval = interval;

    recomputeStats();
}


void StatsWidget::recomputeStats()
{
    auto rangeMode = static_cast<RangeMode>(ui.rangeModeCombo->currentData().toInt());

    double tMin = lastViewInterval.minValue();
    double tMax = lastViewInterval.maxValue();

    model->setRowCount(lastSeriesList.count());

    // TODO: Make this a threaded function, could take a long time to calculate

    for (int idx = 0; idx < lastSeriesList.count(); idx++)
    {
        auto series = lastSeriesList.at(idx);

        if (series.isNull()) continue;

        bool hasData;

        if (rangeMode == RangeWholeTrace)
        {
            // The whole trace "has data" as long as the series has any
            // samples at all - DataSeries::getMinimumValue/getMaximumValue/
            // getMeanValue() (no arguments) already cover the series' full
            // timestamp range internally
            hasData = series->size() > 0;
        }
        else
        {
            // A series only has data "in range" if at least one sample falls between
            // tMin and tMax - matches the indexing DataSeries::getMinimumValue/getMaximumValue
            // use internally, since those return meaningless sentinel values otherwise
            hasData = false;

            if (series->size() > 0)
            {
                auto idxMin = series->getIndexForTimestamp(tMin, DataSeries::SEARCH_RIGHT_TO_LEFT);
                auto idxMax = series->getIndexForTimestamp(tMax, DataSeries::SEARCH_RIGHT_TO_LEFT);

                hasData = (idxMin + 1) <= idxMax;
            }
        }

        auto nameItem = new QStandardItem(series->getLabel());
        nameItem->setEditable(false);
        nameItem->setData(series->getLabel(), StatsRoles::ValueRole);
        model->setItem(idx, ColumnSeries, nameItem);

        if (hasData)
        {
            if (rangeMode == RangeWholeTrace)
            {
                setColumnValue(idx, ColumnMin, series->getMinimumValue(), true);
                setColumnValue(idx, ColumnMax, series->getMaximumValue(), true);
                setColumnValue(idx, ColumnMean, series->getMeanValue(), true);
            }
            else
            {
                setColumnValue(idx, ColumnMin, series->getMinimumValue(tMin, tMax), true);
                setColumnValue(idx, ColumnMax, series->getMaximumValue(tMin, tMax), true);
                setColumnValue(idx, ColumnMean, series->getMeanValue(tMin, tMax), true);
            }
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


void StatsWidget::onRangeModeChanged()
{
    recomputeStats();
}
