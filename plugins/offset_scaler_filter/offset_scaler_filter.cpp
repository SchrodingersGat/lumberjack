#include <qrandom.h>


#include "offset_scaler_filter.hpp"


bool OffsetScalerFilter::beforeProcessStep()
{
    // TODO - set filter options manually
    auto gen = QRandomGenerator();

    m_scaler = (double) gen.bounded(-10, 10) / 10;
    m_offset = (double) gen.bounded(-100, 100) / 10;

    return true;
}


void OffsetScalerFilter::afterProcessStep()
{
    // TODO
}


void OffsetScalerFilter::cancelProcessing()
{
    // TODO
    m_processing = false;
}


uint8_t OffsetScalerFilter::getProgress() const
{
    if (m_input.isNull() || m_output.isNull()) return 0;
    if (m_input->size() == 0) return 0;

    float progress = (float) m_output->size() / (float) m_input->size();

    return (uint8_t) (progress * 100);
}


bool OffsetScalerFilter::setFilterInputs(QList<DataSeriesPointer> inputs, QStringList &errors)
{
    if (!FilterPlugin::setFilterInputs(inputs, errors))
    {
        return false;
    }

    m_input = inputs.first();

    if (m_input.isNull())
    {
        errors.append(tr("Null data series provided"));
        return false;
    }

    return true;
}


bool OffsetScalerFilter::filterData(QStringList &errors)
{
    QString label = m_input->getLabel() + " - Scale + Offset";
    m_output = DataSeriesPointer(new DataSeries(label));

    uint64_t idx = 0;

    m_processing = true;

    while (m_processing && idx < m_input->size())
    {
        DataPoint point = m_input->getDataPoint(idx);

        point.value *= m_scaler;
        point.value += m_offset;

        m_output->addData(point);
    }

    m_processing = false;
    return true;

}


QList<DataSeriesPointer> OffsetScalerFilter::getFilterOutputs(void)
{
    QList<DataSeriesPointer> outputs;

    outputs.append(m_output);
    return outputs;
}
