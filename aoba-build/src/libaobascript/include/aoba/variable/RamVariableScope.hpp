#pragma once

#include <QString>
#include <QMap>

namespace Aoba {
class NumberRamVariable;
class TextRamVariable;
class RamVariable;

class RamVariableScope {
public:
  //! Constructor
  RamVariableScope(RamVariableScope *parent, const QString &id, bool shared, bool global, bool persistent);

  //! Deconstructor
  ~RamVariableScope();

  //! Returns the ID
  inline RamVariableScope *parent() const { return mParent; }

  //! Returns whether any parent is shared
  bool hasSharedParent() const;

  //! Returns an unique identifier
  QString uniqueIdentifier() const;

  //! Returns the total size in bits
  int totalSizeInBits() const;

  //! Returns the ID
  inline const QString &id() const { return mId; }

  //! Returns whether this is shared or not
  inline bool isShared() const { return mShared; }

  //! Returns whether this is shared or not
  inline bool isPersistent() const { return mPersistent; }

  //! Returns whether this is shared or not
  inline bool isGlobal() const { return mGlobal; }

  //! Creates a child scope
  RamVariableScope *createScope(const QString &, bool shared, bool global, bool persistent);

  //! Creates a variable
  NumberRamVariable *createNumberVariable(const QString &id, int min, int max);

  //! Creates a text variable
  //TextRamVariable *createTextVariable(const QString &id, int maxLength);

  //! Returns all scopes
  inline QList<RamVariableScope*> scopes() const { return mChildScopes.values(); }

  //! Returns all variables
  inline QList<RamVariable*> variables() const { return mVariables.values(); }

  //! Tests whether this scope is valid
  bool isValid() const;

  //! Creates a boolean
  inline NumberRamVariable *createBoolean(const QString &id) {
    return createNumberVariable(id, 0, 1);
  } 

  //! Returns whether any variable in this scope has a default value
  bool hasVariablesWithDefaultValue() const;

protected:
  //! Tests whether the variable type of this scope matches all other
  bool hasSameVariableContentType(const RamVariableScope *scope) const;

  //! Returns the total size in bits
  int sharedTotalSizeInBits() const;

  //! The parent
  RamVariableScope *mParent;

  //! The ID
  QString mId;

  //! List of all child scopes
  QMap<QString, RamVariableScope*> mChildScopes;

  //! List of all child scopes
  QMap<QString, RamVariable*> mVariables;

  //! Whether this scope is shared
  bool mShared;

  //! Whether this scope is global
  bool mGlobal;

  //! Whether this scope is persistent
  bool mPersistent;
};

}