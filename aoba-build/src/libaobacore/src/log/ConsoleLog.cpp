#include <aoba/log/ConsoleLog.hpp>
#include <iostream>

using namespace Aoba;

// ----------------------------------------------------------------------------
void ConsoleLog::write(LogLevel level, const QString &msg) {
  std::string prefix;

  switch (level) {
    case LOG_TODO: prefix = "Todo: "; break;
    case LOG_WARN: prefix = "Warning: "; break;
    case LOG_ERROR: prefix = "Error: "; break;
    case LOG_FATAL: prefix = "FATAL: "; break;

    default: break;
  }

  if (level >= LOG_WARN) {
    std::cerr << prefix << msg.toStdString() << std::endl;
  } else {
    std::cout << prefix << msg.toStdString() << std::endl;
  }
}
