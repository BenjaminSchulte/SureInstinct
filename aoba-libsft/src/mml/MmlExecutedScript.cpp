#include <QDebug>
#include <sft/mml/MmlExecutedScript.hpp>
#include <sft/mml/MmlCommand.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
int MmlExecutedScript::commandIndexForCodeOffset(int codeOffset) const {
  if (codeOffset <= 0) {
    return 0;
  }

  int index = std::lower_bound(mCommands.begin(), mCommands.end(), codeOffset, [](const MmlExecutedCommand &command, int tick) -> bool {
    return command.command()->info().start < tick;
  }) - mCommands.begin();

  if (index == mCommands.size()) {
    return index - 1;
  } else if (index == 0) {
    return 0;
  } else if (mCommands[index].command()->info().start == codeOffset) {
    return index;
  }

  int previousTick = mCommands[--index].command()->info().start;
  while (mCommands[index].command()->info().start == previousTick) {
    if (--index < 0) {
      return 0;
    }
  }

  return index + 1;
}

// -----------------------------------------------------------------------------
int MmlExecutedScript::commandIndexAtTick(unsigned tick) const {
  if (tick == 0) {
    return 0;
  }

  int index = std::lower_bound(mCommands.begin(), mCommands.end(), tick, [](const MmlExecutedCommand &command, unsigned tick) -> bool {
    return command.state().tick < tick;
  }) - mCommands.begin();

  if (index == mCommands.size()) {
    return -1;
  } else if (index == 0) {
    return 0;
  } else if (mCommands[index].state().tick == tick) {
    return index;
  }

  unsigned previousTick = mCommands[--index].state().tick;
  while (mCommands[index].state().tick == previousTick) {
    if (--index < 0) {
      return 0;
    }
  }

  return index + 1;
}