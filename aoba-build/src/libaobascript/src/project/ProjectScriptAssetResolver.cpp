#include <aoba/log/Log.hpp>
#include <aoba/project/ProjectScriptAssetResolver.hpp>
#include <aoba/script/ScriptProject.hpp>
#include <aoba/script/ScriptProjectInterface.hpp>
#include <fma/interpret/BaseContext.hpp>
#include <fma/interpret/Result.hpp>
#include <fma/types/InternalValue.hpp>
#include <fma/core/Number.hpp>
#include <fma/core/SymbolReference.hpp>
#include <fma/symbol/ConstantNumber.hpp>
#include <fma/symbol/SymbolReference.hpp>

using namespace Aoba;
using namespace FMA::symbol;
using namespace FMA::core;
using namespace FMA::interpret;
using namespace FMA::types;

// ----------------------------------------------------------------------------
ResultPtr ProjectScriptAssetResolver::scriptResolve(const ContextPtr &context, const QString &id, const QString &type) {
  ResultPtr scriptProjectResult(context->getRootLevelContext()->getMember("@scriptProject"));
  if (!scriptProjectResult->get()->isInternalObjectOfType("AobaScriptProject")) {
    context->log().error() << "Failed to fetch script project on root level context";
    return NumberClass::createInstance(context, 0);
  }

  ScriptProject *project = dynamic_cast<InternalObject<ScriptProject*>*>(scriptProjectResult->get().get())->getValue();
  ScriptProjectInterface *projectInterface = project->projectInterface();
  if (!projectInterface) {
    context->log().error() << "No script project interface has been declared";
    return NumberClass::createInstance(context, 0);
  }

  QStringList typeParts = type.split('/');

  if (typeParts[0] == "language_id") {
    return NumberClass::createInstance(context, projectInterface->getLocaleId(id));
  } else if (typeParts[0] == "table" && typeParts.size() >= 3) {
    QString key = id;

    if (typeParts.size() == 4) {
      key = typeParts[3];
    }

    return NumberClass::createInstance(context, projectInterface->lookupTable(typeParts[1], typeParts[2], key, id.toInt()));
  } else {
    context->log().error() << "Unsupported project configuration " << type.toStdString();
    return NumberClass::createInstance(context, 0);
  }
}
