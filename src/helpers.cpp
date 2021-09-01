#include "helpers.hpp"

#include <math.h>

/**
 * @brief fixedWidthNumber formats an arbitrary number into a fixed-width string
 * @param value
 * @return
 */
QString fixedWidthNumber(double value)
{
    QString text;

    text = QString::number(value, 'g', 10);

    const int W = 16;

    if (text.length() > W)
    {
        text = text.left(W);
    }

    while (text.length() < W)
    {
        text += " ";
    }

    return text;
}
