#pragma once

#include "MmlCommand.hpp"

namespace Sft {
class MmlExecutedScript;

class MmlCompiledScript {
public:
  //! Constructor
  MmlCompiledScript(Song *song) : mSong(song) {}

  //! Deconstructor
  virtual ~MmlCompiledScript() = default;

  //! Flattens the script into a sequence
  MmlExecutedScript *execute() const;

  //! Returns a list of all instruments
  QStringList instruments() const;

  //! Returns a command
  MmlCommandPtr commandAtOffset(int offset) const;

protected:
  //! The song
  Song *mSong;

  //! Lists of all commands
  QVector<MmlCommandPtr> mCommands;
};

}