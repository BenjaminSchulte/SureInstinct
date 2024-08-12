#pragma once

#include <QSharedPointer>
#include "MmlParserInterface.hpp"
#include "../Configuration.hpp"

namespace Sft {
class Song;
struct MmlExecutedState;

typedef QSharedPointer<class MmlCommand> MmlCommandPtr;
typedef QWeakPointer<class MmlCommand> WeakMmlCommandPtr;

enum class MmlCommandType {
  PLAY_NOTE,
  RESUME_NOTE,
  PAUSE,
  SET_INSTRUMENT,
  SET_PANNING,
  SET_TEMPO,
  SET_TRACK_REPEAT_OFFSET,
  SET_VOLUME,
  BEGIN_LOOP,
  REPEAT_LOOP,
  MODIFY_OCTAVE,
  SET_OCTAVE,
  SET_ECHO_ENABLED,
  SET_NOISE_ENABLED,
  SET_PITCH_CHANNEL_ENABLED,
  PITCH_NOTE,
  SET_ADSR_PROPERTY,
  SET_PITCH_OFFSET,
  VIBRATE_PITCH,
  CUSTOM = 0x100
};

struct MmlPitchInfo {
  uint8_t duration = 0;
  int8_t notes = 0;
};

struct MmlVibrateInfo {
  uint8_t duration = 0;
  float notes = 0;
};

class MmlCommand {
public:
  //! Constructor
  MmlCommand(const MmlParserInfo &info) : mInfo(info) {}

  //! Deconstructor
  virtual ~MmlCommand() = default;

  //! Returns the type
  virtual MmlCommandType type() const = 0;

  //! Returns the duration
  virtual unsigned duration() const { return 0; }

  //! Updates the execute state
  virtual void updateExecuteState(Song *, MmlExecutedState &) const {}

  //! Returns the info
  inline const MmlParserInfo &info() const { return mInfo; }

protected:
  //! The info
  MmlParserInfo mInfo;
};

}