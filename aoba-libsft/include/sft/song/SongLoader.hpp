#pragma once

#include <QString>
#include <QDir>

namespace Sft {
class Song;
class Instrument;

class SongLoader {
public:
  //! Constructor
  SongLoader(Song *song) : mSong(song) {}

  //! Deconstructor
  virtual ~SongLoader() = default;

  //! Returns the Song
  inline Song *song() const { return mSong; }

  //! Loads the Song
  virtual bool load(const QString &filename) const = 0;

  //! Loads a song from a file
  static bool loadFromFile(Song *song, const QString &filename);

protected:
  //! The Song to load
  Song *mSong;
};

}