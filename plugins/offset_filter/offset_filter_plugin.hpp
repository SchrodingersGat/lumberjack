#ifndef OFFSET_FILTER_PLUGIN_HPP
#define OFFSET_FILTER_PLUGIN_HPP

#include "offset_filter_global.h"
#include "offset_filter.hpp"


class CUSTOM_FILTER_EXPORT OffsetFilterPlugin : public OffsetFilter
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID FilterInterface_iid)
    Q_INTERFACES(FilterPlugin)
};

#endif // OFFSET_FILTER_PLUGIN_HPP
