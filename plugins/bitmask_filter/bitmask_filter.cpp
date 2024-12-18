#include <qrandom.h>


#include "bitmask_filter.hpp"


bool BitmaskFilter::beforeProcessStep()
{
    // TODO - set filter options manually
    auto gen = QRandomGenerator::securelySeeded();

    // Generate a random mask
    m_mask = gen.bounded(0xFFFFFFFF);

    return true;
}


void BitmaskFilter::afterProcessStep()
{
    // TODO
}


void BitmaskFilter::cancelProcessing()
{
    // TODO
    m_processing = false;
}


uint8_t BitmaskFilter::getProgress() const
{
    if (m_input.isNull() || m_output.isNull()) return 0;
    if (m_input->size() == 0) return 0;

    float progress = (float) m_output->size() / (float) m_input->size();

    return (uint8_t) (progress * 100);
}


bool BitmaskFilter::setFilterInputs(QList<DataSeriesPointer> inputs, QStringList &errors)
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


bool BitmaskFilter::processData()
{
    QString label = m_input->getLabel() + " - Bitmask";
    m_output = DataSeriesPointer(new DataSeries(label));

    uint64_t idx = 0;

    m_processing = true;

    while (m_processing && idx < m_input->size())
    {
        DataPoint point = m_input->getDataPoint(idx);

        // TODO: Support negative values?
        uint32_t value = point.value > 0 ? (uint32_t) point.value : 0;

        // Hack for now, apply a "random" AND mask
        value &= m_mask;

        point.value = (double) value;

        m_output->addData(point);

        idx++;
    }

    m_processing = false;
    return true;

}


QList<DataSeriesPointer> BitmaskFilter::getFilterOutputs(void)
{
    QList<DataSeriesPointer> outputs;

    outputs.append(m_output);
    return outputs;
}
