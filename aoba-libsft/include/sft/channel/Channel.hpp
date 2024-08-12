#pragma once

#include <QString>
#include <QVector>

namespace Sft {
class Song;
class MmlScript;

class Channel {
public:
  //! Constructor
  Channel(Song *song, const QString &name, int index);

  //! Deconstructor
  ~Channel();

  //! Returns the song
  inline Song *song() const { return mSong; }

  //! Returns the name
  inline const QString &name() const { return mName; }

  //! Returns the script
  inline MmlScript *script() const { return mScript; }

  //! Returns the index
  inline int index() const { return mIndex; }

protected:
  //! The owner song
  Song *mSong;

  //! The code of this channel
  MmlScript *mScript;

  //! The channel name
  QString mName;

  //! The index
  int mIndex;
};

}