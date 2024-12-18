#include "offset_scaler_filter.hpp"


bool OffsetScalerFilter::beforeProcessStep()
{
    // TODO - set filter options
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
    // TODO
    return 0;
}


bool OffsetScalerFilter::setFilterInputs(QList<DataSeriesPointer> inputs, QStringList &errors)
{
    bool result = FilterPlugin::setFilterInputs(inputs, errors);

    // TODO: Custom checks here?
    return result;
}


bool OffsetScalerFilter::filterData(QStringList &errors)
{
    // TODO - Filter the data!
    return true;
}


QList<DataSeriesPointer> OffsetScalerFilter::getFilterOutputs(void)
{
    return m_outputs;
}
