#include <aoba/script/ScriptVariableClass.hpp>
#include <aoba/script/ScriptProject.hpp>
#include <aoba/variable/RamVariableRegistry.hpp>
#include <aoba/variable/RamVariableSchema.hpp>
#include <aoba/variable/RamVariable.hpp>
#include <fma/interpret/BaseContext.hpp>
#include <fma/interpret/ParameterList.hpp>
#include <fma/types/ClassPrototype.hpp>
#include <fma/types/Object.hpp>
#include <fma/types/InternalValue.hpp>
#include <fma/core/SymbolReference.hpp>
#include <fma/core/String.hpp>
#include <fma/core/Number.hpp>
#include <fma/Project.hpp>

using namespace Aoba;
using namespace FMA;
using namespace FMA::types;
using namespace FMA::core;
using namespace FMA::interpret;

typedef InternalObject<RamVariable*> InternalVariableValue;


// ----------------------------------------------------------------------------
ClassPtr ScriptVariableClass::create(const RootModulePtr &root, const ClassPtr &ClassObject) {
  ClassPtr klass = ClassPtr(new Class("RamVariable", "RamVariable"));
  klass->extends(ClassObject);

  ClassPrototypePtr proto(klass->getPrototype());
  proto->setMember("initialize", TypePtr(new InternalFunctionValue("initialize", ScriptVariableClass::initialize)));
  proto->setMember("to_s", TypePtr(new InternalFunctionValue("to_s", ScriptVariableClass::to_s)));
  proto->setMember("id", TypePtr(new InternalFunctionValue("to_sym", ScriptVariableClass::id)));

  root->setMember("RamVariable", klass);
  return klass;
}

// ----------------------------------------------------------------------------
ResultPtr ScriptVariableClass::initialize(const ContextPtr &context, const GroupedParameterList &parameter) {
  const auto &args = parameter.only_args();
  if (!args.size()) {
    context->log().error() << "Missing argument for RamVariable";
    return ResultPtr();
  }

  context->self()->setMember("__memory_scope", args.front());

  return Result::executed(context, context->self());
}

// ----------------------------------------------------------------------------
ResultPtr ScriptVariableClass::to_s(const ContextPtr &context, const GroupedParameterList &) {
  RamVariable *v = asVariable(context);
  if (v == nullptr) {
    return ResultPtr();
  }

  ResultPtr scriptProjectResult(context->getRootLevelContext()->getMember("@scriptProject"));
  if (!scriptProjectResult->get()->isInternalObjectOfType("AobaScriptProject")) {
    return ResultPtr();
  }

  ScriptProject *scriptProject = dynamic_cast<InternalObject<ScriptProject*>*>(scriptProjectResult->get().get())->getValue();
  return StringClass::createInstance(context, (QString("<v,") + QString::number(scriptProject->variables()->schema()->idOfVariable(v->uniqueIdentifier())) + ">").toStdString());
}

// ----------------------------------------------------------------------------
ResultPtr ScriptVariableClass::id(const ContextPtr &context, const GroupedParameterList &) {
  RamVariable *v = asVariable(context);
  if (v == nullptr) {
    return ResultPtr();
  }

  ResultPtr scriptProjectResult(context->getRootLevelContext()->getMember("@scriptProject"));
  if (!scriptProjectResult->get()->isInternalObjectOfType("AobaScriptProject")) {
    return ResultPtr();
  }

  ScriptProject *scriptProject = dynamic_cast<InternalObject<ScriptProject*>*>(scriptProjectResult->get().get())->getValue();

  return NumberClass::createInstance(context, scriptProject->variables()->schema()->idOfVariable(v->uniqueIdentifier()));
}

// ----------------------------------------------------------------------------
RamVariable *ScriptVariableClass::asVariable(const interpret::ContextPtr &context) {
  return asVariable(context->getProject(), context->self());
}

// ----------------------------------------------------------------------------
RamVariable *ScriptVariableClass::asVariable(FMA::Project *project, const types::TypePtr &value) {
  const TypePtr &obj = value->getMember("__memory_scope");
  if (!obj || !obj->isInternalObjectOfType("AobaVariable")) {
    project->log().error() << "Unable to access internal memory variable";
    return nullptr;
  }

  return std::dynamic_pointer_cast<InternalVariableValue>(obj)->getValue();
}