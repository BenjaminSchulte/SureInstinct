#pragma once

#include <sft/mml/MmlParserInterface.hpp>
#include "SongBuilderAdapter.hpp"

namespace FMA {
namespace output {
class DynamicBuffer;
}
}

namespace Aoba {

class SongBuilderChannel {
public:
  //! Constructor
  SongBuilderChannel();

  //! Deconstructor
  ~SongBuilderChannel();

  //! Writes a byte to the channel
  void write(unsigned char byte);

  //! Writes a byte to the channel
  void override(unsigned char byte, int offset);

  //! Reserves a byte
  int reserve();

  //! Returns the size
  int getSize() const;

  //! Returns the data
  void *getData() const;

private:
  //! The target buffer
  FMA::output::DynamicBuffer *mBuffer;
};

class SongBuilder : public Sft::MmlParserInterface {
public:
  //! Constructor
  SongBuilder(const SongBuilderAdapter *adapter) : mAdapter(adapter) {}

  //! Displays an error
  void error(int row, int col, const QString &message) override;

  //! Sets the title
  void setAttribute(const QString &id, const QString &value) override;

  //! Starts a track
  void beginTrack(int index) override;

  //! Adds a note
  void note(QChar note, bool half, int duration) override;

  //! Decrements the octave
  void decrementOctave() override;

  //! Increments the octave
  void incrementOctave() override;

  //! Starts a loop
  void setTrackRepeatOffset() override;

  //! Starts a loop
  void startLoop() override;

  //! Ends a loop
  void endLoop(int repeatTimes) override;

  //! Resumes the note
  void resumeNote(int duration) override;

  //! Pauses
  void pause(int duration) override;

  //! Sets the instrument
  void setInstrument(const QString &instrument) override;

  //! Sets the octave
  void setOctave(int octave) override;

  //! Sets the tempo
  void setTempo(int tempo) override;

  //! Sets the volume
  void setVolume(int volume) override;

  //! Sets the panning
  void setPanning(int panning) override;

  //! Sets the echo enabled
  void setEchoEnabled(bool enabled) override;

  //! Sets the echo enabled
  void setNoiseEnabled(bool enabled, int power) override;

  //! Sets the echo enabled
  void setPitchChannelEnabled(bool enabled) override;

  //! Sets the ADSR value
  void setAdsrA(int a) override;

  //! Sets the ADSR value
  void setAdsrD(int d) override;

  //! Sets the ADSR value
  void setAdsrS(int s) override;

  //! Sets the ADSR value
  void setAdsrR(int r) override;
  
  //! Sets the pitch offset
  void setPitchOffset(float notes, bool relative) override;

  //! Adds a ease pitch
  void easePitch(uint8_t duration, int notes) override;

  //! Adds a ease pitch
  void vibratePitch(uint8_t duration, float notes) override;

  //! Returns whether there was an error
  inline bool didFail() const { return mFailed; }

  //! Writes the song into the linked buffer
  void writeTo(FMA::output::DynamicBuffer *block) const;

  //! Writes the song into the linked buffer
  void writeChannelTo(int i, FMA::output::DynamicBuffer *block) const;

  //! Writes the song buffer without the instrument list
  void writeSongDataTo(FMA::output::DynamicBuffer *block) const;

  //! Returns all used instruments
  inline const QVector<int> &instruments() const { return mInstrumentList; }

  //! Writes the instrument list to the buffer
  static void writeInstrumentList(FMA::output::DynamicBuffer *block, const QVector<int> &instrumentList);

  //! Returns whether a channel is empty
  inline bool isChannelEmpty(int i) const { return mChannels[i].getSize() == 0; }

protected:
  //! Writes a note value
  void writeNoteValue(int noteValue, int duration);

  //! Adapter
  const SongBuilderAdapter *mAdapter;

  //! Whether the builder failed
  bool mFailed = false;

  //! Current channel index
  int mCurrentChannel = 0;

  //! List of all channels
  SongBuilderChannel mChannels[8];

  //! List of all instrument IDS
  QVector<int> mInstrumentList;

  //! The loop address stack
  QVector<int> mLoopStack;
};

}