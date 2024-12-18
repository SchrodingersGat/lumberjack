#include "plugin_filter.hpp"


/**
 * @brief FilterPlugin::setFilterInputs - Default implementation to set filter inputs
 * @param inputs
 * @param errors
 * @return
 */
bool FilterPlugin::setFilterInputs(QList<DataSeriesPointer> inputs, QStringList &errors)
{
    if (inputs.count() < getMinInputCount())
    {
        errors.append(tr("Not enough inputs provided"));
        return false;
    }

    if (inputs.count() > getMaxInputCount())
    {
        errors.append(tr("Too many inputs provided"));
        return false;
    }

    m_inputs = inputs;
    return true;
}
