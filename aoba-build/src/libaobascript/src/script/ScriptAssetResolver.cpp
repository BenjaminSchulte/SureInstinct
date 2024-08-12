#include <aoba/script/ScriptAssetResolver.hpp>
#include <fma/core/Number.hpp>
#include <fma/core/SymbolReference.hpp>
#include <fma/symbol/ConstantNumber.hpp>
#include <fma/symbol/SymbolReference.hpp>

using namespace Aoba;
using namespace FMA::interpret;
using namespace FMA::core;
using namespace FMA::symbol;

// ----------------------------------------------------------------------------
ResultPtr ScriptAssetResolver::scriptResolve(const ContextPtr &context, const QString &id, const QString &type) {
  QVariant result = resolve(id, type);
  if (!result.isValid() || result.isNull()) {
    context->log().error() << "Invalid result for " << id.toStdString() << "/" << type.toStdString();
    return NumberClass::createInstance(context, 0);
  }

  if (result.type() == QVariant::String) {
    return SymbolReferenceClass::createInstance(context, SymbolReferencePtr(new SymbolReference(result.toString().toStdString())));
  } else if (result.type() == QVariant::Int) {
    return NumberClass::createInstance(context, result.toInt());
  } else {
    context->log().error() << "Invalid result TYPE for " << id.toStdString() << "/" << type.toStdString();
    return NumberClass::createInstance(context, 0);
  }
}
