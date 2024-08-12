#pragma once

#include <QStringList>

namespace Maker {

class GameLinkResult {
public:
  //! Adds code to the result
  inline void addCode(const QString &code) { mCode.push_back(code); }

  //! Adds an object
  inline void addObject(const QString &obj) { mObjects.push_back(obj); }

  //! Returns the code
  inline QString code() const { return mCode.join('\n'); }
  
  //! Returns all objects
  inline QStringList objects() const { return mObjects; }

protected:
  //! The code
  QStringList mCode;

  //! List of all objects
  QStringList mObjects;
};

}