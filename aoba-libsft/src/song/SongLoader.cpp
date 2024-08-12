#include <QDebug>
#include <sft/song/MmlSongLoader.hpp>
#include <sft/song/XmSongLoader.hpp>
#include <sft/song/ItSongLoader.hpp>
#include <sft/song/Song.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
bool SongLoader::loadFromFile(Song *song, const QString &filename) {
  QFileInfo info(filename);
  QString ext(info.completeSuffix().toLower());
  
  if (ext == "mml") {
    MmlSongLoader loader(song);
    return loader.load(filename);
  } else if (ext == "xm") {
    XmSongLoader loader(song);
    return loader.load(filename);
  } else if (ext == "it") {
    ItSongLoader loader(song);
    return loader.load(filename);
  }

  return false;
}