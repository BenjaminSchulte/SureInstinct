#include <QDebug>
#include <sft/song/Song.hpp>
#include <sft/Configuration.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
Song::Song()
  : mNumChannels(MAX_NUM_CHANNEL)
{
  initializeChannels();
}

// -----------------------------------------------------------------------------
Song::~Song() {
  for (uint8_t i=0; i<mNumChannels; i++) {
    delete mChannels[i];
  }
  delete[] mChannels;
}

// -----------------------------------------------------------------------------
void Song::initializeChannels() {
  mChannels = new Channel*[MAX_NUM_CHANNEL];
  for (uint8_t i=0; i<mNumChannels; i++) {
    Channel *channel = new Channel(this, QString("Channel ") + QString::number(i + 1, 10), i);
    mChannels[i] = channel;
  }
}
