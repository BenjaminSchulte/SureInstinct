#include <QDebug>
#include <sft/mml/MmlWritableCompiledScript.hpp>
#include <sft/mml/MmlModifyOctaveCommand.hpp>
#include <sft/mml/MmlSetOctaveCommand.hpp>
#include <sft/mml/MmlSetAdsrPropertyCommand.hpp>
#include <sft/mml/MmlSetInstrumentCommand.hpp>
#include <sft/mml/MmlSetTempoCommand.hpp>
#include <sft/mml/MmlSetVolumeCommand.hpp>
#include <sft/mml/MmlSetPanningCommand.hpp>
#include <sft/mml/MmlSetEchoEnabledCommand.hpp>
#include <sft/mml/MmlSetPitchChannelEnabledCommand.hpp>
#include <sft/mml/MmlSetNoiseEnabledCommand.hpp>
#include <sft/mml/MmlSetTrackRepeatOffsetCommand.hpp>
#include <sft/mml/MmlPauseNoteCommand.hpp>
#include <sft/mml/MmlPlayNoteCommand.hpp>
#include <sft/mml/MmlResumeNoteCommand.hpp>
#include <sft/mml/MmlBeginLoopCommand.hpp>
#include <sft/mml/MmlRepeatLoopCommand.hpp>
#include <sft/mml/MmlVibratePitchCommand.hpp>
#include <sft/mml/MmlPitchNoteCommand.hpp>
#include <sft/mml/MmlSetPitchOffsetCommand.hpp>
#include <sft/Configuration.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::error(int, int, const QString &) {
  // no error debugging yet
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::setAttribute(const QString &, const QString &) {
  // not necessary
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::beginTrack(int) {
  // not necessary
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::note(QChar note, bool half, int duration) {
  int noteValue = -1;

  switch (note.unicode()) {
    case 'c': noteValue = (!half) ? 0 : 1; break;
    case 'd': noteValue = (!half) ? 2 : 3; break;
    case 'e': noteValue = (!half) ? 4 : 4; break;
    case 'f': noteValue = (!half) ? 5 : 6; break;
    case 'g': noteValue = (!half) ? 7 : 8; break;
    case 'a': noteValue = (!half) ? 9 : 10; break;
    case 'b': noteValue = (!half) ? 11 : 11; break;
    case 'h': noteValue = (!half) ? 11 : 11; break;
  }

  add(new MmlPlayNoteCommand(info(), noteValue, duration));
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::decrementOctave() {
  add(new MmlModifyOctaveCommand(info(), -1));
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::incrementOctave() {
  add(new MmlModifyOctaveCommand(info(), 1));
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::setTrackRepeatOffset() {
  add(new MmlSetTrackRepeatOffsetCommand(info()));
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::startLoop() {
  add(new MmlBeginLoopCommand(info()));
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::endLoop(int repeatTimes) {
  add(new MmlRepeatLoopCommand(info(), repeatTimes));
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::resumeNote(int duration) {
  add(new MmlResumeNoteCommand(info(), duration));
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::pause(int duration) {
  add(new MmlPauseNoteCommand(info(), duration));
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::setInstrument(const QString &instrument) {
  add(new MmlSetInstrumentCommand(info(), instrument));
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::setOctave(int octave) {
  add(new MmlSetOctaveCommand(info(), octave));
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::setTempo(int tempo) {
  add(new MmlSetTempoCommand(info(), tempo));
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::setVolume(int volume) {
  add(new MmlSetVolumeCommand(info(), volume));
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::setPanning(int panning) {
  add(new MmlSetPanningCommand(info(), panning));
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::setEchoEnabled(bool enabled) {
  add(new MmlSetEchoEnabledCommand(info(), enabled));
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::setPitchChannelEnabled(bool enabled) {
  add(new MmlSetPitchChannelEnabledCommand(info(), enabled));
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::setNoiseEnabled(bool enabled, int power) {
  add(new MmlSetNoiseEnabledCommand(info(), enabled, power));
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::easePitch(uint8_t duration, int notes) {
  add(new MmlPitchNoteCommand(info(), duration, notes));
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::vibratePitch(uint8_t duration, float notes) {
  add(new MmlVibratePitchCommand(info(), duration, notes));
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::setPitchOffset(float notes, bool relative) {
  add(new MmlSetPitchOffsetCommand(info(), notes, relative));
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::setAdsrA(int a) {
  add(new MmlSetAdsrPropertyCommand(info(), MmlSetAdsrPropertyCommand::A, a));
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::setAdsrD(int d) {
  add(new MmlSetAdsrPropertyCommand(info(), MmlSetAdsrPropertyCommand::D, d));
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::setAdsrS(int s) {
  add(new MmlSetAdsrPropertyCommand(info(), MmlSetAdsrPropertyCommand::S, s));
}

// -----------------------------------------------------------------------------
void MmlWritableCompiledScript::setAdsrR(int r) {
  add(new MmlSetAdsrPropertyCommand(info(), MmlSetAdsrPropertyCommand::R, r));
}

// -----------------------------------------------------------------------------
