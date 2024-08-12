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
class RamVariable;

class ScriptVariableClass : public FMA::types::Class {
public:
  static FMA::types::ClassPtr create(const FMA::types::RootModulePtr &root, const FMA::types::ClassPtr &Class);

  // STATIC

  // PROTOTYPE
  static FMA::interpret::ResultPtr initialize(const FMA::interpret::ContextPtr &context, const FMA::interpret::GroupedParameterList &parameter);
  static FMA::interpret::ResultPtr id(const FMA::interpret::ContextPtr &context, const FMA::interpret::GroupedParameterList &parameter);
  static FMA::interpret::ResultPtr to_s(const FMA::interpret::ContextPtr &context, const FMA::interpret::GroupedParameterList &parameter);

  static RamVariable *asVariable(const FMA::interpret::ContextPtr &context);
  static RamVariable *asVariable(FMA::Project *project, const FMA::types::TypePtr &);
};

}
