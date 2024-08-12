#pragma once

#include "./RamVariable.hpp"

namespace Aoba {

class NumberRamVariable : public RamVariable {
public:
  //! Constructor
  NumberRamVariable(RamVariableScope *parent, const QString &id, int min, int max)
    : RamVariable(parent, id)
    , mMinimum(min)
    , mMaximum(max)
  {}

  //! Sets a default variable
  inline void setDefaultValue(int value) {
    mHasDefaultValue = true;
    mDefaultValue = value;
  }

  //! Returns whether this has a default value
  bool hasDefaultValue() const override {
    return mHasDefaultValue;
  }

  //! Returns the size in bits
  int sizeInBits() const override;

  //! Returns the default value
  void *defaultValue(uint16_t &size) override {
    size = sizeInBits();
    return &mDefaultValue;
  }

  //! Returns an unique identifier for this variable type
  QString uniqueTypeIdentifier() const override;


protected:
  //! Whether a default value has been set
  bool mHasDefaultValue = false;

  //! The minimum value
  int mMinimum;

  //! The maximum
  int mMaximum;

  //! The default value
  int mDefaultValue;
};

}