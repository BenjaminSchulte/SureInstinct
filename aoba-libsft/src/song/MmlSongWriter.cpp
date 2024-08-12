#include <QDebug>
#include <QFileInfo>
#include <sft/mml/MmlScript.hpp>
#include <sft/mml/MmlParser.hpp>
#include <sft/song/MmlSongWriter.hpp>
#include <sft/channel/Channel.hpp>
#include <sft/mml/MmlScript.hpp>
#include <sft/song/Song.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
bool MmlSongWriter::save(const QString &filename) const {
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) {
    return false;
  }

  QTextStream out(&file);
  for (int i=0; i<mSong->numChannels(); i++) {
    out << ".TRACK " << i << "\n";
    out << mSong->channel(i)->script()->code() << "\n";
  }

  return true;
}

