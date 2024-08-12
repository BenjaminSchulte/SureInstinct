#include <QDebug>
#include <sft/sample/Sample.hpp>
#include <sft/sample/SampleData.hpp>
#include <sft/sample/WavSampleLoader.hpp>
#include <audiofile/AudioFile.hpp>
#include <yaml-cpp/yaml.h>
#include <aoba/yaml/YamlTools.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
bool WavSampleLoader::load(const QString &filename) const {
  AudioFile<double> audioFile;

  if (!audioFile.load(filename.toStdString())) {
    qWarning() << "Unable to load sample:" << filename;
    return false;
  }

  SampleData *data = mSample->inputData();
  data->allocate(audioFile.getNumChannels(), audioFile.getSampleRate(), audioFile.getNumSamplesPerChannel());

  for (uint8_t c=0; c<data->numChannels(); c++) {
    for (uint32_t i=0; i<data->samplesPerChannel(); i++) {
      data->sample(c, i) = audioFile.samples[c][i];
    }
  }


  QFileInfo wavFileInfo(filename);
  YAML::Node root;
  try {
    root = YAML::LoadFile(wavFileInfo.dir().absoluteFilePath(wavFileInfo.baseName() + ".yml").toStdString());
  } catch(YAML::BadFile &f) {
    qDebug() << "Sample doesn't provide a valid YML configuration file:" << filename;
  }

  mSample->setCompressionFactor(Aoba::Yaml::asDouble(root["quality"], 1.0));

  return true;
}
