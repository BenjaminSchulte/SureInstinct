#include <iostream>
#include <aoba/log/Log.hpp>
#include <aoba/log/LogAdapter.hpp>

using namespace Aoba;

Log *Log::mLog = nullptr;

// ----------------------------------------------------------------------------
void Log::addAdapter(LogAdapter *adapter) {
  removeAdapter(adapter);
  mAdapters.push_back(adapter);
}

// ----------------------------------------------------------------------------
bool Log::removeAdapter(LogAdapter *adapter) {
  if (!mAdapters.contains(adapter)) {
    return false;
  }

  mAdapters.removeAll(adapter);
  return true;
}

// ----------------------------------------------------------------------------
void Log::log(LogLevel level, const QString &msg) {
  if (level < mLogLevel) {
    return;
  }

  if (mAdapters.isEmpty()) {
    std::cout << "[MISSING LOG ADAPTER] " << msg.toStdString() << std::endl;
  }

  for (LogAdapter *adapter : mAdapters) {
    adapter->write(level, msg);
  }
}

// ----------------------------------------------------------------------------
void Log::unloadInstance() {
  delete mLog;
  mLog = nullptr;
}

// ----------------------------------------------------------------------------
Log *Log::instance() {
  if (!mLog) {
    mLog = new Log();
  }

  return mLog;
}