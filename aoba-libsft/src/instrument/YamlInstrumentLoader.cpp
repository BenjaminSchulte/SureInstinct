#include <QDebug>
#include <sft/song/Song.hpp>
#include <sft/sample/Sample.hpp>
#include <sft/brr/BrrSample.hpp>
#include <sft/sample/SampleLoader.hpp>
#include <sft/instrument/Instrument.hpp>
#include <sft/instrument/YamlInstrumentLoader.hpp>
#include <yaml-cpp/yaml.h>
#include <aoba/yaml/YamlTools.hpp>
#include <sft/Configuration.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
InstrumentPtr YamlInstrumentLoader::load(const QString &fileName) {
  YAML::Node root;
  try {
    root = YAML::LoadFile(fileName.toStdString());
  } catch(YAML::BadFile &f) {
    return nullptr;
  }

  QString name = Aoba::Yaml::asString(root["name"], QFileInfo(fileName).fileName());
  QString sampleId = Aoba::Yaml::asString(root["sample"]);

  Sample *sample = SampleLoader::loadExternalSample(mSong, sampleId, QFileInfo(fileName).absoluteDir());
  if (!sample) {
    return nullptr;
  }

  InstrumentPtr instrument = mSong->instruments().create(sample);
  instrument->setName(name);

  if (Aoba::Yaml::asBool(root["adsr"]["enabled"])) {
    instrument->setAdsr(InstrumentAdsr(
      Aoba::Yaml::asInt(root["adsr"]["ar"]),
      Aoba::Yaml::asInt(root["adsr"]["dr"]),
      Aoba::Yaml::asInt(root["adsr"]["sl"]),
      Aoba::Yaml::asInt(root["adsr"]["sr"])
    ));
  }

  double adjust = (double)Aoba::Yaml::asInt(root["pitch"], 0) / 0x100;
  instrument->setNoteAdjust(adjust);

  double loop;
  try {
    if (!root["loop"].as<bool>()) {
      loop = -1;
    } else {
      loop = root["loop"].as<bool>() ? 0 : -1;
    }
  } catch(YAML::TypedBadConversion<bool> &e) {
    try {
      loop = root["loop"].as<double>() / 1000.0;
    } catch(YAML::TypedBadConversion<double> &e) {
      loop = -1;
    }
  }
  
  sample->setLoopStart(loop < 0 ? 0 : loop);
qDebug() << "LOOP" << loop;
  instrument->setLooped(loop >= 0);
  sample->setLooped(loop >= 0);
  instrument->setVolume(Aoba::Yaml::asInt(root["volume"], 127) / 127.0);

  return instrument;
}
