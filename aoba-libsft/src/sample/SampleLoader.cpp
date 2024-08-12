#include <QDebug>
#include <sft/song/Song.hpp>
#include <sft/sample/Sample.hpp>
#include <sft/sample/SampleLoader.hpp>
#include <sft/sample/WavSampleLoader.hpp>
#include <sft/sample/BrrSampleLoader.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
Sample *SampleLoader::loadExternalSample(Song *song, const QString &id, const QDir &path) {
  Sample *result = song->samples().findById(id);
  if (result) {
    return result;
  }

  QDir testPath(path);
  testPath.cd("../samples");
  result = new Sample(id, id);
  bool success = false;
  if (QFileInfo(testPath.absoluteFilePath(id + ".wav")).exists()) {
    success = WavSampleLoader(result).load(testPath.absoluteFilePath(id + ".wav"));
  } else if (QFileInfo(testPath.absoluteFilePath(id + ".brr")).exists()) {
    success = BrrSampleLoader(result).load(testPath.absoluteFilePath(id + ".brr"));
  }

  if (!success) {
    delete result;
    result = nullptr;
  } else {
    result->setId(id);
    song->samples().add(result);
  }

  return result;
}