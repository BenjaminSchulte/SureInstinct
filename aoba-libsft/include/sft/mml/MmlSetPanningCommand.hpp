#pragma once

#include "MmlCommand.hpp"

namespace Sft {

class MmlSetPanningCommand : public MmlCommand {
public:
  //! Constructor
  MmlSetPanningCommand(const MmlParserInfo &info, int panning) : MmlCommand(info), mPanning(panning) {}

  //! Returns the type
  MmlCommandType type() const override { return MmlCommandType::SET_PANNING; }

  //! Returns the panning
  inline int panning() const { return mPanning; }

private:
  //! The panning size
  int mPanning;
};

}