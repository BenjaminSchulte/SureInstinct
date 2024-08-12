#include <aoba/script/ScriptVariableScopeClass.hpp>
#include <fma/interpret/BaseContext.hpp>
#include <fma/interpret/ParameterList.hpp>
#include <fma/types/ClassPrototype.hpp>
#include <fma/types/Object.hpp>
#include <fma/types/InternalValue.hpp>
#include <fma/core/SymbolReference.hpp>
#include <fma/Project.hpp>

using namespace Aoba;
using namespace FMA;
using namespace FMA::types;
using namespace FMA::core;
using namespace FMA::interpret;

typedef InternalObject<RamVariableScope*> InternalVariableScopeValue;


// ----------------------------------------------------------------------------
ClassPtr ScriptVariableScopeClass::create(const RootModulePtr &root, const ClassPtr &ClassObject) {
  ClassPtr klass = ClassPtr(new Class("RamVariableScope", "RamVariableScope"));
  klass->extends(ClassObject);

  ClassPrototypePtr proto(klass->getPrototype());
  proto->setMember("initialize", TypePtr(new InternalFunctionValue("initialize", ScriptVariableScopeClass::initialize)));

  root->setMember("RamVariableScope", klass);
  return klass;
}

// ----------------------------------------------------------------------------
ResultPtr ScriptVariableScopeClass::initialize(const ContextPtr &context, const GroupedParameterList &parameter) {
  const auto &args = parameter.only_args();
  if (!args.size()) {
    context->log().error() << "Missing argument for RamVariableScope";
    return ResultPtr();
  }

  context->self()->setMember("__memory_scope", args.front());

  return Result::executed(context, context->self());
}

// ----------------------------------------------------------------------------
RamVariableScope *ScriptVariableScopeClass::asVariableScope(const interpret::ContextPtr &context) {
  return asVariableScope(context->getProject(), context->self());
}

// ----------------------------------------------------------------------------
RamVariableScope *ScriptVariableScopeClass::asVariableScope(FMA::Project *project, const types::TypePtr &value) {
  const TypePtr &obj = value->getMember("__memory_scope");
  if (!obj || !obj->isInternalObjectOfType("AobaVariableScope")) {
    project->log().error() << "Unable to access internal memory scope";
    return nullptr;
  }

  return std::dynamic_pointer_cast<InternalVariableScopeValue>(obj)->getValue();
}