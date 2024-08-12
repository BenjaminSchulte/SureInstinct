#pragma once

#include <QString>
#include <QVariant>
#include <fma/interpret/BaseContext.hpp>
#include <fma/symbol/Reference.hpp>

namespace Aoba {

class ScriptAssetResolver {
public:
  //! Constructor
  ScriptAssetResolver() = default;
  
  //! Deconstructor
  virtual ~ScriptAssetResolver() = default;

  //! Returns a reference
  virtual FMA::interpret::ResultPtr scriptResolve(const FMA::interpret::ContextPtr &context, const QString &id, const QString &type);

  //! Returns an information
  virtual QVariant resolve(const QString &id, const QString &type) = 0;

  //! Prepares the resolve (only called for global assets)
  virtual bool prepare(const QString &, const QString &) { return true; }
};

}