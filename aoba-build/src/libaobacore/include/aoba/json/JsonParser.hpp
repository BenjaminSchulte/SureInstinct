#pragma once

#include <QString>

namespace Aoba {

class JsonParser {
public:
  static QString parseString(const QString &, bool *ok=nullptr);
};

}