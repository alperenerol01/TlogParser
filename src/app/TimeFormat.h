#pragma once

#include <QString>

/**
 * @brief Formats a duration as minutes and seconds, e.g. "01:32".
 * @param milliseconds The duration in milliseconds.
 * @return The text in "mm:ss" form.
 */
QString formatTime(qint64 milliseconds);
