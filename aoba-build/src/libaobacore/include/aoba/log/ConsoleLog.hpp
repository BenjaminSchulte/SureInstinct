#pragma once

#include "LogAdapter.hpp"

namespace Aoba {

//! Constructor
class ConsoleLog : public LogAdapter {
public:
  //! Logs a message
  void write(LogLevel level, const QString &) override;
};

}