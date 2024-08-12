#include <maker/game/GameLog.hpp>
#include <aoba/log/Log.hpp>
#include <iostream>

using namespace Maker;

// -----------------------------------------------------------------------------
bool GameLogTask::failed(const QString &reason) {
  mLog->error(reason);
  return false;
}

// -----------------------------------------------------------------------------
void GameLogTask::succeeded() {
}

// -----------------------------------------------------------------------------
void GameLogAssetTask::start() {
  Aoba::log::info(QString("\x1b[32;1m") + mAction.leftJustified(6) + "\x1b[35;1m" + mType.leftJustified(20) + "\x1b[34;1m" + mId + "\x1b[m");
}

// -----------------------------------------------------------------------------
GameLogAssetTask GameLog::assetTask(const QString &action, const QString &type, const QString &id) const {
  GameLogAssetTask task(this, action, type, id);
  task.start();
  return task;
}

// -----------------------------------------------------------------------------
void GameLog::percent(double percent) {
  std::cout << "PERCENT" << percent;
}

// -----------------------------------------------------------------------------
void GameLog::debug(const QString &text) const {
  Aoba::log::debug(text);
}

// -----------------------------------------------------------------------------
void GameLog::note(const QString &text) const {
  Aoba::log::note(text);
}

// -----------------------------------------------------------------------------
void GameLog::warning(const QString &text) const {
  Aoba::log::warn(text);
}

// -----------------------------------------------------------------------------
void GameLog::error(const QString &text) const {
  Aoba::log::error(text);
}
