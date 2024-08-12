#pragma once

#include <QString>

namespace Aoba {
class RamVariableScope;

class RamVariable {
public:
  //! Constructor
  RamVariable(RamVariableScope *parent, const QString &id)
    : mParent(parent)
    , mId(id)
  {}

  //! Deconstructor
  virtual ~RamVariable() = default;

  //! Returns the parent
  inline RamVariableScope *scope() const {
    return mParent;
  }

  //! Returns the ID
  inline const QString &id() const {
    return mId;
  }

  //! Returns an unique identifier for this variable type
  virtual QString uniqueTypeIdentifier() const = 0;

  //! Returns an unique identifier for this variable
  virtual QString uniqueIdentifier() const;

  //! Sets the variable compressed
  void setShouldDecompress(bool decompressed) {
    mDecompressed = decompressed;
  }

  //! Returns whether the variable is compressed
  inline bool shouldDecompress() const {
    return mDecompressed;
  }

  //! Sets the symbol name
  void setSymbolName(const QString &name);

  //! Returns the symbol name
  inline const QString &symbolName() const;

  //! Returns whether this has a default value
  virtual bool hasDefaultValue() const = 0;

  //! Returns the default value
  virtual void *defaultValue(uint16_t &sizeInBits) {
    sizeInBits = 0;
    return nullptr;
  }

  //! Returns the size in bits
  virtual int sizeInBits() const = 0;

protected:
  //! The parent scope
  RamVariableScope *mParent;

  //! The ID
  QString mId;

  //! Whether this should be decompressed
  bool mDecompressed = false;
};

}