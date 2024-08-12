#pragma once

#include <QString>

namespace Maker {
class GameLog;

class GameLogTask {
public:
  //! Constructor
  GameLogTask(const GameLog *log) : mLog(log) {}

  //! Deconstructor
  virtual ~GameLogTask() = default;

  //! Writes the message
  virtual void start() = 0;

  //! Failed with a reason
  virtual bool failed(const QString &reason);

  //! Succeeded with a reason
  virtual void succeeded();

protected:
  //! The log
  const GameLog *mLog;
};

class GameLogAssetTask : public GameLogTask {
public:
  //! Constructor
  GameLogAssetTask(const GameLog *log, const QString &action, const QString &type, const QString &id)
    : GameLogTask(log)
    , mAction(action)
    , mType(type)
    , mId(id)
  {}

  // Writes the message to the log
  void start() override;

protected:
  //! The action
  QString mAction;
  
  //! The type
  QString mType;

  //! The id
  QString mId;
};

class GameLog {
public:
  //! Creates a task
  GameLogAssetTask assetTask(const QString &action, const QString &type, const QString &id) const;

  //! Sets the project percent
  void percent(double percent);

  //! Writes direct
  void debug(const QString &text) const;

  //! Writes direct
  void note(const QString &text) const;

  //! Writes direct
  void warning(const QString &text) const;

  //! Writes direct
  void error(const QString &text) const;
};

}