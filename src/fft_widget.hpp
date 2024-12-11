#ifndef FFT_WIDGET_HPP
#define FFT_WIDGET_HPP

#include "plot_widget.hpp"
#include "data_series.hpp"

class FFTWidget : public PlotWidget
{
    Q_OBJECT

public:
    FFTWidget();
    virtual ~FFTWidget();

    void updateInterval(const QwtInterval &interval);

protected:
    virtual bool isCurveTrackingEnabled(void) const override { return false; }
    virtual PlotCurveUpdater* generateNewWorker(DataSeriesPointer series) override;

    virtual void resampleCurves(int axis_id = yBoth) override;

    // Internally keep track of timestamp limits
    double timestamp_min = 0;
    double timestamp_max = 0;

    void initAxes();
};

#endif // FFT_WIDGET_HPP
