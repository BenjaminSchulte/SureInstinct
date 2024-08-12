#pragma once

#include <QVector>
#include <QString>
#include "LogLevel.hpp"

namespace Aoba {
class LogAdapter;

class Log {
public:
  //! Adds an adapter
  void addAdapter(LogAdapter *adapter);

  //! Removes an adapter
  bool removeAdapter(LogAdapter *adapter);

  //! Removes all adapters
  inline void removeAllAdapters() { mAdapters.clear(); }

  //! Logs a message
  void log(LogLevel level, const QString &msg);

  //! Returns the log level
  inline LogLevel logLevel() const { return mLogLevel; }

  //! Sets the log level
  inline void setLogLevel(LogLevel logLevel) { mLogLevel = logLevel; }

  //! Returns the global instance
  static Log *instance();

  //! Unloads the global instance
  static void unloadInstance();

private:
  //! Log level
  LogLevel mLogLevel = LOG_TRACE;

  //! List of attached adapters
  QVector<LogAdapter*> mAdapters;

  //! Log instance
  static Log *mLog;
};

namespace log {
  inline void trace(const QString &msg) { Log::instance()->log(LOG_TRACE, msg); }
  inline void debug(const QString &msg) { Log::instance()->log(LOG_DEBUG, msg); }
  inline void info(const QString &msg) { Log::instance()->log(LOG_INFO, msg); }
  inline void note(const QString &msg) { Log::instance()->log(LOG_NOTE, msg); }
  inline void todo(const QString &msg) { Log::instance()->log(LOG_TODO, msg); }
  inline void warn(const QString &msg) { Log::instance()->log(LOG_WARN, msg); }
  inline void error(const QString &msg) { Log::instance()->log(LOG_ERROR, msg); }
  inline void fatal(const QString &msg) { Log::instance()->log(LOG_FATAL, msg); }
}

}