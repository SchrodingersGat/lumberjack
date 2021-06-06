#include <qelapsedtimer.h>

#include "plot_curve.hpp"

PlotCurveUpdater::PlotCurveUpdater(DataSeries &data_series) : QObject(), series(data_series)
{
    // TODO
}


/**
 * Re-sample the data for the provided data series, between the specified timestamps
 *
 * TODO: Description of how the algorithm works!
 *
 * TODO: This algorithm constructs two arrays, and then the QwtPlotCurve->setSamples() function
 *       *COPIES* the data across to the curve.
 *       Instead, perhaps we could use setRawSamples() function to prevent an unnecessary copy operation.
 */
void PlotCurveUpdater::updateCurveSamples(double t_min, double t_max, unsigned int n_pixels)
{
    // If the arguments are the same as last time, ignore
    if (t_min == t_min_latest && t_max == t_max_latest && n_pixels == n_pixels_latest)
    {
        return;
    }

    t_min_latest = t_min;
    t_max_latest = t_max;
    n_pixels_latest = n_pixels;

    // Profiling timer
    QElapsedTimer elapsed;
    elapsed.restart();

    // Initialize empty arrays
    QVector<double> t_data;
    QVector<double> y_data;

    // Quick check for an empty series
    if (series.size() == 0 || n_pixels == 0)
    {
        emit sampleComplete(t_data, y_data);
        return;
    }

    // Ensure that the timestamp values are ordered correctly
    if (t_min > t_max)
    {
        double swap = t_min;

        t_min = t_max;
        t_max = swap;
    }

    // Extract data access indices
    auto idx_min = series.getIndexForTimestamp(t_min);
    auto idx_max = series.getIndexForTimestamp(t_max);

    auto n_samples = idx_max - idx_min;

    /* If there is at least one sample available "before" the minimum timestamp,
     * we *always* draw that sample first.
     * This wil ensure that a line gets drawn off the left of the screen!
     */

    bool sample_left = idx_min > 0;
    bool sample_right = idx_max < (series.size() - 1);

    // If the number of available points is *not greater* than the number of pixels,
    // simple return *all* samples within the specified timespan
    if (n_samples <= n_pixels)
    {
        // We know how many samples are going to be inserted
        t_data.reserve(n_samples + 2);
        y_data.reserve(n_samples + 2);

        if (sample_left)
        {
            t_data.push_back(series.getTimestamp(idx_min - 1));
            y_data.push_back(series.getValue(idx_min - 1));
        }

        for (auto idx = idx_min; (idx <= idx_max) && (idx < series.size()); idx++)
        {
            auto point = series.getDataPoint(idx);

            t_data.push_back(point.timestamp);
            y_data.push_back(point.value);
        }

        if (sample_right)
        {
            t_data.push_back(series.getTimestamp(idx_max + 1));
            y_data.push_back(series.getValue(idx_max + 1));
        }

        emit sampleComplete(t_data, y_data);

        return;
    }

    // The "worst case" down sampling requires 4 data points per pixel
    // So, pre-allocate that amount of memory
    t_data.reserve(4 * n_pixels + 2);
    y_data.reserve(4 * n_pixels + 2);

    if (sample_left)
    {
        t_data.push_back(series.getTimestamp(idx_min - 1));
        y_data.push_back(series.getValue(idx_min - 1));
    }

    // Time delta per pixel
    double dt = (t_max - t_min) / n_pixels;

    double t = series.getTimestamp(idx_min);

    // Pre-calculate the time of the "next" pixel
    double t_next = t + dt;

    // Construct pointers to raw samples to be added
    DataPoint pt_first = series.getDataPoint(idx_min);
    DataPoint pt_min = pt_first;
    DataPoint pt_max = pt_first;
    DataPoint pt_last = pt_first;

    int pt_counter = 1;

    if (idx_max >= series.size())
    {
        idx_max = series.size() - 1;
    }

    bool min_value_found = false;
    bool max_value_found = false;

    for (uint64_t idx = idx_min; idx <= idx_max; idx++)
    {
        auto point = series.getDataPoint(idx);

        if ((idx < idx_max) && (point.timestamp < t_next))
        {
            // Increment sample counter within this pixel window
            pt_counter++;

            // Update min / max values
            if (point.value < pt_min.value)
            {
                pt_min = point;
            }
            if (point.value > pt_max.value)
            {
                pt_max = point;
            }

            // Record this as the "most recent" point
            pt_last = point;
        }
        else
        {
            /* We have moved to the next "pixel":
             * - Work out how many points need to be added to the previous pixel
             */

            // Record the current time as the start of the next "pixel"
            t = t_next;
            t_next = t + dt;

            // Add in the data points as required
            if (pt_counter > 0)
            {
                t_data.push_back(pt_first.timestamp);
                y_data.push_back(pt_first.value);
            }

            if (pt_counter > 2)
            {
                // If the "minimum" value was lower than the first and last points
                if ((pt_min.value < pt_first.value) && (pt_min.value < pt_last.value))
                {
                    min_value_found = true;
                }
                else
                {
                    min_value_found = false;
                }

                // If the "maximum" value was greater than the first and last points
                if ((pt_max.value > pt_first.value) && (pt_max.value > pt_last.value))
                {
                    max_value_found = true;
                }
                else
                {
                    max_value_found = false;
                }

                // Now work out the timestamp order in which to add the point(s)
                if (min_value_found)
                {
                    // Min *and* max value found, determine which one is first
                    if (max_value_found)
                    {
                        if (pt_min.timestamp <= pt_max.timestamp)
                        {
                            t_data.push_back(pt_min.timestamp);
                            y_data.push_back(pt_min.value);

                            t_data.push_back(pt_max.timestamp);
                            y_data.push_back(pt_max.value);
                        }
                        else
                        {
                            t_data.push_back(pt_max.timestamp);
                            y_data.push_back(pt_max.value);

                            t_data.push_back(pt_min.timestamp);
                            y_data.push_back(pt_min.value);
                        }
                    }
                    else
                    {
                        // Just the minimum value
                        t_data.push_back(pt_min.timestamp);
                        y_data.push_back(pt_min.value);
                    }
                }
                else if (max_value_found)
                {
                    t_data.push_back(pt_max.timestamp);
                    y_data.push_back(pt_max.value);
                }
            }

            // Always add the "last" value
            if (pt_counter >= 1)
            {
                t_data.push_back(pt_last.timestamp);
                y_data.push_back(pt_last.value);
            }

            // Reset point data
            pt_first = point;
            pt_min = point;
            pt_max = point;
            pt_last = point;

            // Reset point counter
            pt_counter = 1;
        }
    }

    // If there is a point "off screen" to the right, add it
    if (sample_right)
    {
        t_data.push_back(series.getTimestamp(idx_max + 1));
        y_data.push_back(series.getValue(idx_max + 1));
    }

    // Reduce the allocated memory to fit
    t_data.shrink_to_fit();
    y_data.shrink_to_fit();

    // Signal that the downsampling process is now complete
    emit sampleComplete(t_data, y_data);

    qInfo() << "updateCurveSamples reduced" << n_samples << "samples to" << t_data.size() << "points in" << elapsed.elapsed() << "ms";
}


PlotCurve::PlotCurve(QSharedPointer<DataSeries> s) :
    QwtPlotCurve(),
    series(s),
    worker(*s)
{
    if (!series.isNull())
    {
        QwtPlotCurve::setTitle((*series).getLabel());
    }

    setPen(QColor(50, 150, 150));

    worker.moveToThread(&workerThread);

    connect(&worker, &PlotCurveUpdater::sampleComplete, this, &PlotCurve::onDataResampled);
}


PlotCurve::~PlotCurve()
{
    qDebug() << "~PlotCurve()";
    // Wait for the resampling thread to complete
    workerThread.quit();
    workerThread.wait();

    qDebug() << "~PlotCurve() complete";
}


void PlotCurve::onDataResampled(const QVector<double> &t_data, const QVector<double> &y_data)
{
    // Re-draw the curve
    setSamples(t_data, y_data);
}


void PlotCurve::resampleData(double t_min, double t_max, unsigned int n_pixels)
{
    if (!series.isNull())
    {
        worker.updateCurveSamples(t_min, t_max, n_pixels);
    }
}



