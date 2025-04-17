#ifndef SCALER_FILTER_PLUGIN_HPP
#define SCALER_FILTER_PLUGIN_HPP

#include "scaler_filter_global.h"
#include "scaler_filter.hpp"

class CUSTOM_FILTER_EXPORT ScalerFilterPlugin : public ScalerFilter
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID FilterInterface_iid)
    Q_INTERFACES(FilterPlugin)
};

#endif // SCALER_FILTER_PLUGIN_HPP
