#include <QDebug>
#include <QDataStream>
#include <QByteArray>
#include <QFile>
#include <sft/sample/Sample.hpp>
#include <sft/sample/SampleData.hpp>
#include <sft/sample/BrrSampleLoader.hpp>
#include <sft/brr/BrrSample.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
bool BrrSampleLoader::load(const QString &fileName) const {
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly)) {
    return false;
  }

  BrrSample *sample = mSample->createRawBrrSample();

  QDataStream in(&file);
  QByteArray chunkArray(9, 0);
  while (!in.atEnd()) {
    int readData = in.readRawData(chunkArray.data(), 9);
    if (readData != 9) {
      break;
    }

    const uint8_t *data = reinterpret_cast<const uint8_t*>(chunkArray.constData());

    BrrSampleChunk *chunk = sample->appendChunk();
    
    uint8_t header = data[0];
    chunk->setShift(header >> 4);
    chunk->setFilter((BrrSampleChunk::Filter)((header >> 2) & 0x03));

    for (uint8_t s=0; s<16; s+=2) {
      uint8_t samples = data[1 + (s >> 1)];
      chunk->wave()[s  ] = (samples >> 4) & 0x0F;
      chunk->wave()[s+1] = samples & 0x0F;
    }
  }

  return true;
}
