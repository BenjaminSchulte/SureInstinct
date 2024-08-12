#pragma once

#include "MmlCommand.hpp"

namespace Sft {

class MmlSetInstrumentCommand : public MmlCommand {
public:
  //! Constructor
  MmlSetInstrumentCommand(const MmlParserInfo &info, const QString &instrument) : MmlCommand(info), mInstrument(instrument) {}

  //! Returns the type
  MmlCommandType type() const override { return MmlCommandType::SET_INSTRUMENT; }

  //! Returns the instrument
  inline const QString &instrument() const { return mInstrument; }

  //! Updates the execute state
  void updateExecuteState(Song *, MmlExecutedState &) const override;

private:
  //! The instrument size
  QString mInstrument;
};

}