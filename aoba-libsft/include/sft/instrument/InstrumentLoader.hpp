#pragma once

#include <QDir>
#include "Instrument.hpp"

namespace Sft {
class Song;

class InstrumentLoader {
public:  
  //! Constructor
  InstrumentLoader(Song *song) : mSong(song) {}

  //! Deconstructor
  virtual ~InstrumentLoader() = default;

  //! Loads an external instrument
  static InstrumentPtr loadExternalInstrument(Song *song, const QString &id, const QDir &path);

protected:
  //! The song
  Song *mSong;
};

}