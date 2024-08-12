#pragma once

#include "../script/ScriptAssetResolver.hpp"

namespace Aoba {

class ProjectScriptAssetResolver : public ScriptAssetResolver {
public:
  //! Constructor
  ProjectScriptAssetResolver() {}

  //! Returns a reference
  FMA::interpret::ResultPtr scriptResolve(const FMA::interpret::ContextPtr &context, const QString &id, const QString &type) override;

  //! Returns an information
  QVariant resolve(const QString &, const QString &) override {
    return QVariant();
  }

protected:
};

}