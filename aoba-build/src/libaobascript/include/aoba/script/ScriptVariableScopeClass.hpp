#pragma once

#include <fma/types/Class.hpp>
#include <fma/types/RootModule.hpp>
#include <fma/interpret/Result.hpp>

namespace FMA {
class Project;
namespace interpret {
class BaseContext;
typedef std::shared_ptr<BaseContext> ContextPtr;
}
}
namespace Aoba {
class RamVariableScope;

class ScriptVariableScopeClass : public FMA::types::Class {
public:
  static FMA::types::ClassPtr create(const FMA::types::RootModulePtr &root, const FMA::types::ClassPtr &Class);

  // STATIC

  // PROTOTYPE
  static FMA::interpret::ResultPtr initialize(const FMA::interpret::ContextPtr &context, const FMA::interpret::GroupedParameterList &parameter);

  static RamVariableScope *asVariableScope(const FMA::interpret::ContextPtr &context);
  static RamVariableScope *asVariableScope(FMA::Project *project, const FMA::types::TypePtr &);
};

}
