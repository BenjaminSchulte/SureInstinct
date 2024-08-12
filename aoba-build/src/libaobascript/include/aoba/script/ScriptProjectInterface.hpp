#pragma once

#include <QString>

namespace Aoba {
class RamVariableRegistry;

class ScriptProjectInterface {
public:
  //! Deconstructor
  virtual ~ScriptProjectInterface() = default;

  //! Adds a translation
  virtual void addTranslationHeader(const QString &text, const QString &context) = 0;

  //! Adds a translation
  virtual void addTranslationComment(const QString &text, const QString &context) = 0;

  //! Adds a translation
  virtual QString addTranslation(const QString &text, const QString &font, const QString &context) = 0;

  //! Looks up a generated table
  virtual void lookupTable(const QString &type, const QString &id) = 0;

  //! Looks up a generated table
  virtual int lookupTable(const QString &type, const QString &id, const QString &key, int value) = 0;

  //! Returns the variable registry
  virtual RamVariableRegistry *getVariables() = 0;

  //! Returns the language ID
  virtual int getLocaleId(const QString &locale) = 0;
};

}