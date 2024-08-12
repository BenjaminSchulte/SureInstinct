#pragma once

#include <QString>
#include "LogLevel.hpp"

namespace Aoba {

class LogAdapter {
public:
  //! Constructor
  LogAdapter() = default;

  //! Deconstructor
  virtual ~LogAdapter() = default;

  //! Logs a message
  virtual void write(LogLevel level, const QString &) = 0;
};

}