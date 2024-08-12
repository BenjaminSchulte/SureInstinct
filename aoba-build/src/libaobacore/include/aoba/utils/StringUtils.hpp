#pragma once

#include <QString>

namespace Aoba {

class StringUtils {
public:
  //! Escapes a string
  static QString escape(const QString &text);

  //! To int
  static inline int toInt(const QString &text, int fallback=-1) {
    bool ok;
    int value = text.toInt(&ok);
    if (!ok) { value = fallback; }
    return value;
  }
};

}