#include <QDebug>
#include <sft/sample/Sample.hpp>
#include <sft/sample/SampleData.hpp>
#include <sft/sample/WavSampleWriter.hpp>
#include <audiofile/AudioFile.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
bool WavSampleWriter::save(const QString &filename) const {
  AudioFile<double> audioFile;

  const SampleData *data = mSample->inputData();
  if (!data || data->numChannels() == 0 || data->samplesPerChannel() == 0) {
    data = mSample->previewData();
  }
  if (!data || data->numChannels() == 0 || data->samplesPerChannel() == 0) {
    qWarning() << "Unable to export samples with BRR format currently";
    return false;
  }

  audioFile.setAudioBufferSize(data->numChannels(), data->samplesPerChannel());
  audioFile.setSampleRate(data->sampleRate());

  for (uint8_t c=0; c<data->numChannels(); c++) {
    for (uint32_t i=0; i<data->samplesPerChannel(); i++) {
      audioFile.samples[c][i] = data->sample(c, i);
    }
  }

  return audioFile.save(filename.toStdString());
}
