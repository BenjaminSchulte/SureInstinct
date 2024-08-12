#include <QDebug>
#include <QFileInfo>
#include <sft/instrument/InstrumentLoader.hpp>
#include <sft/mml/MmlScript.hpp>
#include <sft/mml/MmlParser.hpp>
#include <sft/song/MmlSongLoader.hpp>
#include <sft/song/Song.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
bool MmlSongLoader::load(const QString &filename) const {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << "File not found:" << filename;
    return false;
  }

  QTextStream in(&file);
  int track = -1;
  while (!in.atEnd()) {
    QString row = in.readLine();

    if (row.left(7).toLower() == ".track ") {
      track = row.mid(7).trimmed().toInt();
      if (track >= mSong->numChannels()) {
        track = -1;
      }
    } else if (row.isEmpty() || row[0] != '.') {
      if (track >= 0) {
        mSong->channel(track)->script()->append(row + "\n");
      }
    }
  }

  for (int i=0; i<mSong->numChannels(); i++) {
    mSong->channel(i)->script()->compile();

    for (const QString &instrument : mSong->channel(i)->script()->compiled()->instruments()) {
      if (!mSong->instruments().findById(instrument)) {
        InstrumentLoader::loadExternalInstrument(mSong, instrument, QFileInfo(filename).absoluteDir());
      }
    }
    
    mSong->channel(i)->script()->compile();
  }

  return true;
}

