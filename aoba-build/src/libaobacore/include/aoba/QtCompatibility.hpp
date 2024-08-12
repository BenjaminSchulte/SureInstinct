#pragma once

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
#  define QT_SKIP_EMPTY_PARTS Qt::SkipEmptyParts
#else
#  define QT_SKIP_EMPTY_PARTS QString::SkipEmptyParts
#endif