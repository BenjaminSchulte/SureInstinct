#pragma once

#include <QString>
#include <QDir>

namespace Sft {
class Song;
class Instrument;

class SongWriter {
public:
  //! Constructor
  SongWriter(Song *song) : mSong(song) {}

  //! Deconstructor
  virtual ~SongWriter() = default;

  //! Returns the Song
  inline Song *song() const { return mSong; }

  //! Loads the Song
  virtual bool save(const QString &filename) const = 0;

protected:
  //! The Song to save
  Song *mSong;
};

}