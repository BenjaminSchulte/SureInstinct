#include <aoba/log/Log.hpp>
#include <aoba/variable/RamVariableScope.hpp>
#include <aoba/variable/NumberRamVariable.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
RamVariableScope::RamVariableScope(RamVariableScope *parent, const QString &id, bool shared, bool global, bool persistent)
  : mParent(parent)
  , mId(id)
  , mShared(shared)
  , mGlobal(global)
  , mPersistent(persistent)
{}

// -----------------------------------------------------------------------------
RamVariableScope::~RamVariableScope() {
  for (RamVariableScope *scope : mChildScopes) {
    delete scope;
  }
  for (RamVariable *var : mVariables) {
    delete var;
  }
}

// -----------------------------------------------------------------------------
QString RamVariableScope::uniqueIdentifier() const {
  QString flags = 
    QString(mShared ? "S" : "") +
    QString(mGlobal ? "G" : "") + 
    QString(mPersistent ? "P" : "");

  return
    (mParent ? mParent->uniqueIdentifier() + "." : "") +
    mId +
    (flags.length() ? "[" + flags + "]" : "");
}

// -----------------------------------------------------------------------------
bool RamVariableScope::hasSharedParent() const {
  if (!mParent) {
    return false;
  }

  if (mParent->isShared()) {
    return true;
  }

  return mParent->hasSharedParent();
}

// -----------------------------------------------------------------------------
RamVariableScope *RamVariableScope::createScope(const QString &id, bool shared, bool global, bool persistent) {
  if (mChildScopes.contains(id)) {
    return mChildScopes[id];
  }

  global = mGlobal || global;
  persistent = mPersistent || persistent;

  RamVariableScope *scope = new RamVariableScope(this, id, shared, global, persistent);
  mChildScopes.insert(id, scope);
  return scope;
}

// -----------------------------------------------------------------------------
NumberRamVariable *RamVariableScope::createNumberVariable(const QString &_id, int min, int max) {
  QString id = _id;
  int append = 0;
  while (mVariables.contains(id)) {
    id = _id + QString::number(++append);
  }

  NumberRamVariable *var = new NumberRamVariable(this, id, min, max);
  mVariables.insert(id, var);
  return var;
}

// -----------------------------------------------------------------------------
bool RamVariableScope::isValid() const {
  if (mShared && !hasSameVariableContentType(this)) {
    Aoba::log::warn("Shared variable scopes can not have child scopes with different persistent/global flags");
    return false;
  }

  if (hasSharedParent() && hasVariablesWithDefaultValue()) {
    Aoba::log::warn("RamVariables in shared scopes can not have default values");
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
bool RamVariableScope::hasSameVariableContentType(const RamVariableScope *other) const {
  if (other->isGlobal() != mGlobal || other->isPersistent() != mPersistent) {
    return false;
  }

  for (RamVariableScope *child : mChildScopes) {
    if (!child->hasSameVariableContentType(other)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool RamVariableScope::hasVariablesWithDefaultValue() const {
  for (RamVariable *v : mVariables) {
    if (v->hasDefaultValue()) {
      return true;
    }
  }

  return false;
}

// -----------------------------------------------------------------------------
int RamVariableScope::sharedTotalSizeInBits() const {
  int size = 0;

  for (RamVariable *v : mVariables) {
    size = qMax(size, v->sizeInBits());
  }

  for (RamVariableScope *s : mChildScopes) {
    size = qMax(size, s->totalSizeInBits());
  }

  return size;
}

// -----------------------------------------------------------------------------
int RamVariableScope::totalSizeInBits() const {
  if (mShared) {
    return sharedTotalSizeInBits();
  }

  int size = 0;
  for (RamVariable *v : mVariables) {
    size += v->sizeInBits();
  }

  for (RamVariableScope *s : mChildScopes) {
    size += s->totalSizeInBits();
  }

  return size;
}
