#pragma once

#include "MmlWritableCompiledScript.hpp"

namespace Sft {
class Song;
class MmlExecutedScript;

class MmlScript {
public:
  //! Constructor
  MmlScript(Song *song) : mSong(song) {}

  //! Deconstructor
  ~MmlScript();
  
  //! Clears the code
  void clear();

  //! Adds code to the end
  void set(const QString &);

  //! Adds code to the end
  void append(const QString &);

  //! Inserts code at a specific position
  void insert(int position, const QString &);

  //! Returns the whole code
  inline const QString &code() const { return mCode; }

  //! Returns the compiled code
  inline MmlCompiledScript *compiled() const { return mCompiled; }

  //! Returns the compiled code
  inline MmlExecutedScript *executed() const { return mExecuted; }

  //! Requires the code to be compiled
  inline void requireCompiled() {
    if (mInvalid) {
      compile();
    }
  }

  //! Compiles the code
  void compile();

private:
  //! The song
  Song *mSong;

  //! Whether the cache is invalid
  bool mInvalid = true;

  //! The code
  QString mCode;

  //! The compiled script
  MmlWritableCompiledScript *mCompiled = nullptr;

  //! The executed script
  MmlExecutedScript *mExecuted = nullptr;
};
 
}
