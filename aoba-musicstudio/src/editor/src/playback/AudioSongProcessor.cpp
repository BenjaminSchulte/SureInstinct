#include <QDebug>
#include <editor/playback/AudioSampleManager.hpp>
#include <editor/playback/AudioSongProcessor.hpp>
#include <editor/playback/AudioManager.hpp>
#include <editor/playback/AudioPlayer.hpp>
#include <sft/song/Song.hpp>
#include <sft/instrument/Instrument.hpp>
#include <sft/mml/MmlScript.hpp>
#include <sft/mml/MmlExecutedScript.hpp>
#include <sft/mml/MmlPlayNoteCommand.hpp>
#include <sft/mml/MmlSetEchoEnabledCommand.hpp>
#include <sft/mml/MmlSetNoiseEnabledCommand.hpp>
#include <sft/mml/MmlSetNoiseEnabledCommand.hpp>
#include <sft/mml/MmlSetPitchChannelEnabledCommand.hpp>
#include <sft/mml/MmlSetTempoCommand.hpp>
#include <sft/mml/MmlSetInstrumentCommand.hpp>
#include <sft/mml/MmlSetAdsrPropertyCommand.hpp>
#include <sft/mml/MmlSetPitchOffsetCommand.hpp>

using namespace Editor;

// ----------------------------------------------------------------------------
void AudioSongProcessor::reset() {
  for (int i=0; i<Sft::MAX_NUM_CHANNEL; i++) {
    mChannel[i] = AudioSongProcessorChannel();
    mManager->channelOff(i);
    mManager->echoOff(i);
    mManager->noiseOff(i);
    mManager->pitchModulationOff(i);
  }

  mManager->samples()->clear();

  setSpeed(Sft::INITIAL_SPEED);
}

// ----------------------------------------------------------------------------
void AudioSongProcessor::rewind() {
  if (mRepeatTick == 0) {
    reset();
  }

  jumpToTick(mRepeatTick);
}

// ----------------------------------------------------------------------------
void AudioSongProcessor::jumpToTick(unsigned int tick) {
  if (!mSong) {
    return;
  }

  for (int i=0; i<mSong->numChannels(); i++) {
    mSong->channel(i)->script()->requireCompiled();
  
    auto *executed = mSong->channel(i)->script()->executed();
    unsigned command = executed->commandIndexAtTick(tick);
    while (command < executed->numCommands() && executed->command(command).state().tick < tick) {
      command++;
    }

    mChannel[i].mNextCommandIndex = command;
    mChannel[i].mNextCommandDelay = 0;
    
    if (command < executed->numCommands()) {
      mChannel[i].mNextCommandDelay = executed->command(mChannel[i].mNextCommandIndex).state().tick - tick;
    }

    mChannel[i].mTick = tick;
    mChannel[i].mActive = true;

    if (command < executed->numCommands()) {
      const auto &state = executed->command(command).state();
      setNoiseEnabled(i, state.noiseEnabled, state.noiseClock);
      setPitchModulationEnabled(i, state.pitchTrackEnabled);
      if (state.instrument) {
        setInstrument(i, state.instrument->id());
      }
      updateAdsr(i, state);
    }
  }
}

// ----------------------------------------------------------------------------
void AudioSongProcessor::updateAdsr(int index, const Sft::MmlExecutedBasicState &state) {
  mManager->channel(index)->setAdsr(true, state.adsrA, state.adsrD, state.adsrS, state.adsrR);
}

// ----------------------------------------------------------------------------
void AudioSongProcessor::setSong(Sft::Song *song) {
  unloadSong();

  mSong = song;
}

// ----------------------------------------------------------------------------
void AudioSongProcessor::unloadSong() {
  stop();
  reset();

  mSong = nullptr;  
}

// ----------------------------------------------------------------------------
void AudioSongProcessor::play() {
  if (mIsPlaying || !mSong) {
    return;
  }

  mIsPlaying = true;
  for (int i=0; i<mSong->numChannels(); i++) {
    mChannel[i].mActive = true;
  }
}

// ----------------------------------------------------------------------------
void AudioSongProcessor::stop() {
  pause();
  reset();
}

// ----------------------------------------------------------------------------
void AudioSongProcessor::pause() {
  if (!mIsPlaying) {
    return;
  }

  mIsPlaying = false;

  int numChannel = (mSong ? mSong->numChannels() : Sft::MAX_NUM_CHANNEL);
  for (int i=0; i<numChannel; i++) {
    mManager->channelOff(i);
  }
}

// ----------------------------------------------------------------------------
void AudioSongProcessor::onTimer() {
  if (!mIsPlaying || !mSong) {
    return;
  }

  for (int i=0; i<mSong->numChannels(); i++) {
    processAudioTick(i);
  }
}

// ----------------------------------------------------------------------------
void AudioSongProcessor::processAudioTick(int channelIndex) {
  auto &channel = mChannel[channelIndex];

  if (channel.mPitchDuration > 0) {
    channel.mPitchDuration--;
    channel.mEasePitch += channel.mEachPitchAdjust;
    channel.mPitchInvalid = true;
  }

  if (channel.mVibratoSpeed > 0) {
    if (channel.mVibratoPositive) {
      channel.mVibratoPitch += channel.mVibratoPitchAdjust;
    } else {
      channel.mVibratoPitch -= channel.mVibratoPitchAdjust;
    }

    if (--channel.mVibratoCounter == 0) {
      channel.mVibratoCounter = channel.mVibratoSpeed * 2;
      channel.mVibratoPositive = !channel.mVibratoPositive;
    }

    channel.mPitchInvalid = true;
  }
  
  while (channel.mActive && channel.mNextCommandDelay <= 0) {
    processNextCommand(channel, channelIndex);
  }

  if (channel.mActive) {
    channel.mNextCommandDelay--;
    channel.mTick++;

    if (channel.mPitchInvalid) {
      updatePitch(channel, channelIndex);
    }
  }
}

// ----------------------------------------------------------------------------
void AudioSongProcessor::processNextCommand(AudioSongProcessorChannel &channel, int channelIndex) {
  auto *script = mSong->channel(channelIndex)->script();
  script->requireCompiled();
  if (channel.mNextCommandIndex >= script->executed()->numCommands()) {
    onEndOfChannel(channel, channelIndex);
    return;
  }

  const auto &command = script->executed()->command(channel.mNextCommandIndex);
  if (command.state().tick > channel.mTick) {
    qDebug() << channelIndex << "- Wait for command tick" << command.state().tick << channel.mTick;
    channel.mNextCommandDelay = command.state().tick - channel.mTick;
    return;
  }

  channel.mNextCommandIndex++;
  channel.mNextCommandDelay = command.command()->duration();

  processCommand(command, channel, channelIndex);
}

// ----------------------------------------------------------------------------
void AudioSongProcessor::processCommand(const Sft::MmlExecutedCommand &command, AudioSongProcessorChannel &channel, int channelIndex) {
  switch (command.command()->type()) {
    // Play a note
    case Sft::MmlCommandType::PLAY_NOTE:
      playNote(command.state(), command.command().dynamicCast<Sft::MmlPlayNoteCommand>(), channel, channelIndex);
      break;

    // Pause - simple doing :D
    case Sft::MmlCommandType::PAUSE:
      mManager->channelOff(channelIndex);
      break;

    // Sets the tempo
    case Sft::MmlCommandType::SET_TEMPO:
      setSpeed(command.command().dynamicCast<Sft::MmlSetTempoCommand>()->tempo());
      break;

    // Sets the echo
    case Sft::MmlCommandType::SET_ECHO_ENABLED:
      setEchoEnabled(channelIndex, command.command().dynamicCast<Sft::MmlSetEchoEnabledCommand>()->enabled());
      break;

    // Sets the pitch
    case Sft::MmlCommandType::SET_PITCH_CHANNEL_ENABLED:
      setPitchModulationEnabled(channelIndex, command.command().dynamicCast<Sft::MmlSetPitchChannelEnabledCommand>()->enabled());
      break;

    // Sets the noise
    case Sft::MmlCommandType::SET_NOISE_ENABLED: {
      const auto &noise = command.command().dynamicCast<Sft::MmlSetNoiseEnabledCommand>();
      setNoiseEnabled(channelIndex, noise->enabled(), noise->power());
      break;
    }
    
    // Sets the repeat offset
    case Sft::MmlCommandType::SET_TRACK_REPEAT_OFFSET:
      channel.mRepeatTick = channel.mTick;
      break;

    // Sets the instrument
    case Sft::MmlCommandType::SET_INSTRUMENT:
      setInstrument(channelIndex, command.command().dynamicCast<Sft::MmlSetInstrumentCommand>()->instrument());
      break;

    // Sets the instrument
    case Sft::MmlCommandType::SET_ADSR_PROPERTY:
      setAdsrProperty(channelIndex, command.state(), command.command().dynamicCast<Sft::MmlSetAdsrPropertyCommand>());
      break;

    case Sft::MmlCommandType::SET_PITCH_OFFSET: {
      const auto &pitch = command.command().dynamicCast<Sft::MmlSetPitchOffsetCommand>();
      setPitchOffset(channelIndex, pitch->notes(), pitch->isRelative());
      break;
    }

    // Unnecessary commands (already executed while compiling)
    case Sft::MmlCommandType::RESUME_NOTE:
    case Sft::MmlCommandType::SET_PANNING:
    case Sft::MmlCommandType::SET_VOLUME:
    case Sft::MmlCommandType::BEGIN_LOOP:
    case Sft::MmlCommandType::REPEAT_LOOP:
    case Sft::MmlCommandType::PITCH_NOTE:
    case Sft::MmlCommandType::SET_OCTAVE:
    case Sft::MmlCommandType::MODIFY_OCTAVE:
    case Sft::MmlCommandType::VIBRATE_PITCH:
      break;

    // Unknown command
    default:
      qDebug() << channelIndex << "Tick" << command.state().tick << "- Unable to process command" << (int)command.command()->type();
      break;
  }
}

// ----------------------------------------------------------------------------
void AudioSongProcessor::setInstrument(int channel, const QString &instrumentId) {
  const auto &instrument = mSong->instruments().findById(instrumentId);
  AudioPlayer::instance()->assignInstrument(instrument, channel);

  if (instrument) {
    mChannel[channel].mInstrumentPitch = instrument->playbackNoteAdjust();
  }
}

// ----------------------------------------------------------------------------
void AudioSongProcessor::setAdsrProperty(int channel, const Sft::MmlExecutedBasicState &oldState, const QSharedPointer<Sft::MmlSetAdsrPropertyCommand> &command) {
  Sft::MmlExecutedBasicState state(oldState);

  switch (command->property()) {
    case Sft::MmlSetAdsrPropertyCommand::A: state.adsrA = command->value(); break;
    case Sft::MmlSetAdsrPropertyCommand::D: state.adsrD = command->value(); break;
    case Sft::MmlSetAdsrPropertyCommand::S: state.adsrS = command->value(); break;
    case Sft::MmlSetAdsrPropertyCommand::R: state.adsrR = command->value(); break;
  }

  updateAdsr(channel, state);
}

// ----------------------------------------------------------------------------
void AudioSongProcessor::playNote(const Sft::MmlExecutedBasicState &state, const QSharedPointer<Sft::MmlPlayNoteCommand> &play, AudioSongProcessorChannel &channel, int index) {
  AudioManager::Channel *managerChannel = mManager->channel(index);

  int note = state.octave * Sft::NUM_NOTES_PER_OCTAVE + play->note();
  double instrumentVolume = 1;

  if (state.instrument) {
    instrumentVolume = state.instrument->volume();
  } else if (state.noiseEnabled) {
    managerChannel->setAdsr(true, 1, 1, 1, 1);
  } else {
    qWarning() << "Tried to play invalid instrument";
    return;
  }

  int dspVolume = ((state.volume / 127.0) * instrumentVolume) * 127.0 + 0.5;
  
  channel.mPitchDuration = state.pitch.duration;
  if (state.pitch.duration > 1) {
    channel.mEachPitchAdjust = state.pitch.notes / ((double)state.pitch.duration - 1);
    channel.mEasePitch = 0;
  }
  if (state.vibrate.duration > 0) {
    channel.mVibratoSpeed = state.vibrate.duration;
    channel.mVibratoCounter = state.vibrate.duration;
    channel.mVibratoPitch = 0;
    channel.mVibratoPitchAdjust = state.vibrate.notes / state.vibrate.duration;
    channel.mVibratoPositive = true;
  } else {
    channel.mVibratoSpeed = 0;
    channel.mVibratoCounter = 0;
    channel.mVibratoPitchAdjust = 0;
  }

  channel.mNote = note;
  managerChannel->setVolume(dspVolume, dspVolume);
  updatePitch(channel, index);
  channel.mPitchInvalid = false;
  managerChannel->play();
}

// ----------------------------------------------------------------------------
void AudioSongProcessor::updatePitch(AudioSongProcessorChannel &channel, int index) {
  AudioManager::Channel *managerChannel = mManager->channel(index);
  
  uint16_t pitch = AudioPlayer::calculatePitch(channel.mNote, channel.mInstrumentPitch, channel.mPitchOffset + channel.mEasePitch + channel.mVibratoPitch);
  managerChannel->setPitch(pitch);
}

// ----------------------------------------------------------------------------
void AudioSongProcessor::onEndOfChannel(AudioSongProcessorChannel &channel, int index) {
  channel.mActive = false;
  mManager->channelOff(index);

  bool hasActive = false;
  for (int i=0; i<mSong->numChannels(); i++) {
    if (mChannel[i].mActive) {
      hasActive = true;
      break;  
    }
  }

  if (!hasActive) {
    emit songFinished();
  }
}

// ----------------------------------------------------------------------------
void AudioSongProcessor::setSpeed(int speed) {
  mManager->setSongPlaybackTimer(speed);
}

// ----------------------------------------------------------------------------
void AudioSongProcessor::setPitchOffset(int channel, float notes, bool relative) {
  double newOffset = notes;

  if (relative) {
    mChannel[channel].mPitchOffset += newOffset;
  } else {
    mChannel[channel].mPitchOffset = newOffset;
  }

  mChannel[channel].mPitchInvalid = true;
}

// ----------------------------------------------------------------------------
void AudioSongProcessor::setEchoEnabled(int channel, bool enabled) {
  if (enabled) {
    mManager->echoOn(channel);
  } else {
    mManager->echoOff(channel);
  }
}

// ----------------------------------------------------------------------------
void AudioSongProcessor::setPitchModulationEnabled(int channel, bool enabled) {
  if (enabled) {
    mManager->pitchModulationOn(channel);
  } else {
    mManager->pitchModulationOff(channel);
  }
}

// ----------------------------------------------------------------------------
void AudioSongProcessor::setNoiseEnabled(int channel, bool enabled, int power) {
  if (power >= 0) {
    mManager->setNoiseClock(power);
  }

  if (enabled) {
    mManager->noiseOn(channel);
  } else {
    mManager->noiseOff(channel);
  }
}
