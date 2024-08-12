#pragma once

#include <QUndoCommand>

enum class AobaUndoCommandType {
  SET_TILE,
  SET_TERRAIN,
  UNKNOWN = -1
};

class AobaUndoCommand : public QUndoCommand {
public:
};

template<typename T> class AobaUndoCommandTemplate : public AobaUndoCommand {
public:
  //! Tries to merge this record
  bool mergeWith(const QUndoCommand *other) override {
    if (other->id() != id()) {
      return false;
    }

    return mergeWithCommand(dynamic_cast<const T*>(other));
  }

  //! Tries to merge
  virtual bool mergeWithCommand(const T *) {
    return false;
  }
};
