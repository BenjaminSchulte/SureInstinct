#pragma once

#include <QSharedPointer>
#include <sft/Configuration.hpp>

namespace Sft {
class Song;
struct MmlExecutedCommand;
struct MmlExecutedBasicState;
class MmlPlayNoteCommand;
class MmlSetAdsrPropertyCommand;
typedef QSharedPointer<class Instrument> InstrumentPtr;
}

namespace Editor {

class AudioManager;

struct AudioSongProcessorChannel {
  //! Index within the executed script
  unsigned int mNextCommandIndex = 0;

  //! This is only approximated, but should be used for testing
  int mNextCommandDelay = 0;

  //! The current tick
  unsigned int mTick = 0;

  //! The repeat tick
  int mRepeatTick = -1;

  //! Current pitch duration
  int mPitchDuration = 0;

  //! The note pitch
  int mNote = 0;

  //! The note pitch
  double mInstrumentPitch = 0;

  //! Current pitch offset
  double mPitchOffset = 0;

  //! Current pitch adjust
  double mEachPitchAdjust = 0;

  //! The current pitch adjust
  double mEasePitch = 0;

  //! Current pitch adjust for vibrato
  double mVibratoPitchAdjust = 0;

  //! Current pitch adjust for vibrato
  double mVibratoPitch = 0;

  //! Current direction
  bool mVibratoPositive = true;

  //! Vibrato speed
  int mVibratoSpeed = 0;

  //! Vibrato counter
  int mVibratoCounter = 0;

  //! Whether the command is active
  bool mActive = false;

  //! Whether a pitch update is required
  bool mPitchInvalid = true;
};

class AudioSongProcessor : public QObject {
  Q_OBJECT

public:
  //! Constructor
  AudioSongProcessor(AudioManager *manager) : mManager(manager) {}

  //! Sets the current song
  void setSong(Sft::Song *song);

  //! Unloads the current song
  void unloadSong();

  //! Resets everything
  void reset();

  //! Resets everything
  void rewind();

  //! Jumps to a tick
  void jumpToTick(unsigned int tick);

  //! Starts playback
  void play();

  //! Stops playback
  void stop();

  //! Pauses playback
  void pause();

  //! Sub tick
  void onTimer();

  //! Whether the song is playing
  inline bool isPlaying() const { return mIsPlaying; }

  //! Returns the tick of a channel
  inline int channelTick(int channel) const { return mChannel[channel].mTick; }

  //! Returns the repeat tick
  inline int repeatTick() const { return mRepeatTick; }

  //! Sets the repeat tick
  void setRepeatTick(int tick) { mRepeatTick = tick; }

signals:
  //! The song has been finished playing
  void songFinished();

private:
  //! Sets the song speed
  void setSpeed(int speed);

  //! Sets the song speed
  void setEchoEnabled(int channel, bool enabled);

  //! Sets the song speed
  void setPitchModulationEnabled(int channel, bool enabled);

  //! Sets the song speed
  void setNoiseEnabled(int channel, bool enabled, int power);

  //! Sets the instrument
  void setInstrument(int channel, const QString &instrument);

  //! Sets the adsr property
  void setAdsrProperty(int channel, const Sft::MmlExecutedBasicState &state, const QSharedPointer<Sft::MmlSetAdsrPropertyCommand> &command);

  //! Sets the instrument
  void setPitchOffset(int channel, float notes, bool relative);

  //! Sub tick
  void processAudioTick(int channel);

  //! Processes the next command
  void processNextCommand(AudioSongProcessorChannel &channel, int index);

  //! Processes the next command
  void processCommand(const Sft::MmlExecutedCommand &command, AudioSongProcessorChannel &channel, int index);

  //! Processes the next command
  void playNote(const Sft::MmlExecutedBasicState &state, const QSharedPointer<Sft::MmlPlayNoteCommand> &play, AudioSongProcessorChannel &channel, int index);

  //! Updates the pitch
  void updatePitch(AudioSongProcessorChannel &channel, int index);

  //! Updates the pitch
  void updateAdsr(int index, const Sft::MmlExecutedBasicState &state);

  //! Processes the next command
  void onEndOfChannel(AudioSongProcessorChannel &channel, int index);

  //! The audio manager
  AudioManager *mManager;

  //! The current song
  Sft::Song *mSong = nullptr;

  //! Whether the current song is being played
  bool mIsPlaying = false;

  //! Repeat tick
  int mRepeatTick = 0;

  //! All channels
  AudioSongProcessorChannel mChannel[Sft::MAX_NUM_CHANNEL];
};

}