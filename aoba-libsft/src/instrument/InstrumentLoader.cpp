#include <QDebug>
#include <sft/song/Song.hpp>
#include <sft/instrument/Instrument.hpp>
#include <sft/instrument/InstrumentLoader.hpp>
#include <sft/instrument/YamlInstrumentLoader.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
InstrumentPtr InstrumentLoader::loadExternalInstrument(Song *song, const QString &id, const QDir &path) {
  InstrumentPtr result = song->instruments().findById(id);
  if (result) {
    return result;
  }

  QDir testPath(path);
  testPath.cd("../instruments");
  if (QFileInfo(testPath.absoluteFilePath(id + ".yml")).exists()) {
    YamlInstrumentLoader loader(song);
    result = loader.load(testPath.absoluteFilePath(id + ".yml"));
  }

  if (result) {
    result->setId(id);
  }
  
  return result;
}