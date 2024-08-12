#include <QDebug>
#include <sft/mml/MmlCommand.hpp>
#include <sft/mml/MmlSetInstrumentCommand.hpp>
#include <sft/mml/MmlCompiledScript.hpp>
#include <sft/mml/MmlWritableExecutedScript.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
MmlCommandPtr MmlCompiledScript::commandAtOffset(int offset) const {
  MmlCommandPtr lastCommand;
  for (const auto &command : mCommands) {
    if (command->info().start <= offset && (command->info().end + 1) >= offset) {
      return command;
    }

    lastCommand = command;
  }

  return lastCommand;
}
// -----------------------------------------------------------------------------
QStringList MmlCompiledScript::instruments() const {
  QStringList all;

  for (const auto &command : mCommands) {
    if (command->type() != MmlCommandType::SET_INSTRUMENT) {
      continue;
    }

    QString instrument = command.dynamicCast<MmlSetInstrumentCommand>()->instrument();
    if (!all.contains(instrument)) {
      all.push_back(instrument);
    }
  }

  return all;
}

// -----------------------------------------------------------------------------
MmlExecutedScript *MmlCompiledScript::execute() const {
  MmlWritableExecutedScript *instance = new MmlWritableExecutedScript();
  MmlExecutedState state;

  while ((int)state.commandIndex < mCommands.size()) {
    const MmlCommandPtr &command = mCommands[state.commandIndex];
    state.commandIndex++;

    instance->add(MmlExecutedCommand(command, state));
    state.tick += command->duration();

    command->updateExecuteState(mSong, state);
  }

  return instance;
}
