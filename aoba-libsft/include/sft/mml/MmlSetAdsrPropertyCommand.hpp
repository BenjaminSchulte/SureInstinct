#pragma once

#include "MmlCommand.hpp"

namespace Sft {

class MmlSetAdsrPropertyCommand : public MmlCommand {
public:
  enum Property {
    A,
    D,
    S,
    R
  };

  //! Constructor
  MmlSetAdsrPropertyCommand(const MmlParserInfo &info, Property property, int8_t value) : MmlCommand(info), mProperty(property), mValue(value) {}

  //! Returns the type
  MmlCommandType type() const override { return MmlCommandType::SET_ADSR_PROPERTY; }

  //! Returns the property
  inline Property property() const { return mProperty; }

  //! Returns the value
  inline int8_t value() const { return mValue; }

  //! Updates the execute state
  void updateExecuteState(Song *song, MmlExecutedState &state) const override;

private:
  //! The property
  Property mProperty;

  //! The value
  int8_t mValue;
};

}