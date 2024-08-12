#pragma once

#include <QString>
#include <QObject>

namespace Sft {
class Song;
}

namespace Editor {

class Project : public QObject {
  Q_OBJECT

public:
  //! Constructor
  Project();

  //! Deconstructor
  ~Project();

  //! Returns the song
  inline Sft::Song *song() const { return mSong; }

  //! Returns the working directory
  inline QString songPath() const { return ""; }

protected:
  //! The song
  Sft::Song *mSong;
};

}