#include <QDebug>
#include <QtMath>
#include <QFileInfo>
#include <sft/utils/DataStream.hpp>
#include <sft/sample/Sample.hpp>
#include <sft/sample/SampleData.hpp>
#include <sft/song/ItSong.hpp>
#include <sft/song/ItSongLoader.hpp>
#include <sft/song/Song.hpp>
#include <sft/mml/MmlScript.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
bool ItSongLoader::load(const QString &filename) const {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    return false;
  }

  //XmFile xmFile;

  ImpulseTracker::Module mod(filename);
  if (!mod.load()) {
    qWarning() << "Unable to load IT file";
    return false;
  }

  qDebug() << "TITLE" << filename << mod.Title << mod.InstrumentCount << mod.SampleCount << mod.PatternCount;

  return true;
}

