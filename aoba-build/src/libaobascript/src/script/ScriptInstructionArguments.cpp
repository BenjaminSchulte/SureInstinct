#include <aoba/log/Log.hpp>
#include <aoba/script/ScriptInstructionArguments.hpp>
#include <aoba/script/ScriptVariableClass.hpp>
#include <aoba/script/ScriptProject.hpp>
#include <aoba/variable/RamVariable.hpp>
#include <aoba/variable/RamVariableRegistry.hpp>
#include <aoba/variable/RamVariableSchema.hpp>
#include <fma/interpret/ProjectContext.hpp>
#include <fma/interpret/Result.hpp>
#include <fma/assem/ConstantNumberOperand.hpp>
#include <fma/assem/SymbolReferenceOperand.hpp>
#include <fma/assem/MemoryAddressOperand.hpp>
#include <fma/assem/DirectPageOperand.hpp>
#include <fma/assem/RelativeAddressOperand.hpp>
#include <fma/assem/LocalAddressOperand.hpp>
#include <fma/assem/StackAddressOperand.hpp>
#include <fma/assem/RegisterOperand.hpp>
#include <fma/types/Object.hpp>
#include <fma/types/RootModule.hpp>
#include <fma/types/InternalValue.hpp>
#include <fma/types/Class.hpp>

#include <iostream>

using namespace Aoba;
using namespace FMA;
using namespace FMA::types;
using namespace FMA::assem;
using namespace FMA::interpret;


// ----------------------------------------------------------------------------
ScriptInstructionArguments::ScriptInstructionArguments(const ContextPtr &context, Project *project, const GroupedParameterList &_params)
  : left(nullptr)
  , mid(nullptr)
  , right(nullptr)
  , context(context)
  , project(project)
  , valid(true)
{
  const TypeList &params = _params.only_args();
  numArgs = params.size();

  if (numArgs == 1) {
    left = analyzeArgs(params.front());
  } else if (numArgs == 2) {
    left = analyzeArgs(params.front());
    right = analyzeArgs(params.back());
  } else if (numArgs == 3) {
    left = analyzeArgs(params.front());
    mid = analyzeArgs(params.at(1));
    right = analyzeArgs(params.back());
  }
}

// ----------------------------------------------------------------------------
ScriptInstructionArguments::~ScriptInstructionArguments() {
  delete left;
  delete mid;
  delete right;
}

// ----------------------------------------------------------------------------
ScriptInstructionArgument *ScriptInstructionArguments::analyzeArgs(const TypePtr &param) {
  const ObjectPtr &value = param->asObject();

  if (!value) {
    valid = false;
    return nullptr;
  }

  ClassPtr ImmediateNumber = project->root()->getMember("ImmediateNumber")->asClass();
  ClassPtr TypedNumber = project->root()->getMember("TypedNumber")->asClass();
  ClassPtr Number = project->root()->getMember("Number")->asClass();
  ClassPtr Register = project->root()->getMember("Register")->asClass();
  ClassPtr SymbolReference = project->root()->getMember("SymbolReference")->asClass();
  ClassPtr RamVariable = project->root()->getMember("RamVariable")->asClass();

  const ClassPtr &self = value->getClass();

  if (self->isInstanceOf(ImmediateNumber)) {
    return analyzeImmediate(value);
  } else if (self->isInstanceOf(Number)) {
    return analyzeNumber(value);
  } else if (self->isInstanceOf(SymbolReference)) {
    return analyzeSymbol(value);
  } else if (self->isInstanceOf(TypedNumber)) {
    return analyzeTypedNumber(value);
  } else if (self->isInstanceOf(Register)) {
    return analyzeRegister(value);
  } else if (self->isInstanceOf(RamVariable)) {
    return analyzeVariable(value);
  } else if (value->hasMember("to_sym")) {
    GroupedParameterList empty;
    return analyzeArgs(value->callDirect("to_sym", context, empty)->get());
  } else {
    std::cout << "UNKNOWN ARG " << value->asString() << std::endl;
    valid = false;
  }

  return nullptr;
}

// ----------------------------------------------------------------------------
ScriptInstructionArgument *ScriptInstructionArguments::analyzeImmediate(const ObjectPtr &value) {
  ScriptInstructionArgument *arg = analyzeArgs(value->getMember("number"));
  if (arg == nullptr || arg->type != ScriptInstructionArgument::ADDRESS) {
    delete arg;
    valid = false;
    return arg;
  }

  arg->type = ScriptInstructionArgument::IMMEDIATE;
  return arg;
}

// ----------------------------------------------------------------------------
ScriptInstructionArgument *ScriptInstructionArguments::analyzeNumber(const ObjectPtr &value) {
  TypePtr valueObject = value->getMember("__value");
  if (!valueObject->isInternalObjectOfType("Number")) {
    valid = false;
    return nullptr;
  }

  InternalNumberValue *number = dynamic_cast<InternalNumberValue*>(valueObject.get());
  return new ScriptInstructionArgument(
    ScriptInstructionArgument::ADDRESS,
    number->getValue()
  );
}

// ----------------------------------------------------------------------------
ScriptInstructionArgument *ScriptInstructionArguments::analyzeVariable(const ObjectPtr &value) {
  RamVariable *v = ScriptVariableClass::asVariable(project, value);
  if (!v) {
    valid = false;
    return nullptr;
  }

  ResultPtr scriptProjectResult(context->getRootLevelContext()->getMember("@scriptProject"));
  if (!scriptProjectResult->get()->isInternalObjectOfType("AobaScriptProject")) {
    valid = false;
    return nullptr;
  }

  ScriptProject *scriptProject = dynamic_cast<InternalObject<ScriptProject*>*>(scriptProjectResult->get().get())->getValue();
  return new ScriptInstructionArgument(
    ScriptInstructionArgument::VARIABLE,
    scriptProject->variables()->schema()->idOfVariable(v->uniqueIdentifier())
  );
}

// ----------------------------------------------------------------------------
ScriptInstructionArgument *ScriptInstructionArguments::analyzeTypedNumber(const ObjectPtr &value) {
  ScriptInstructionArgument *inner = analyzeArgs(value->getMember("number"));
  TypePtr valueObject = value->getMember("type")->getMember("__value");
  if (!valueObject->isInternalObjectOfType("String")) {
    valid = false;
    return nullptr;
  }

  std::string typeName = dynamic_cast<InternalStringValue*>(valueObject.get())->getValue();
  if (typeName == "dp") {
    if (inner->type == ScriptInstructionArgument::ADDRESS) {
      inner->type = ScriptInstructionArgument::DIRECT_PAGE;
      return inner;
    }
  } else if (typeName == "param") {
    if (inner->type == ScriptInstructionArgument::ADDRESS) {
      inner->type = ScriptInstructionArgument::PARAM;
      return inner;
    }
  } else if (typeName == "long_address") {
    if (inner->type == ScriptInstructionArgument::ADDRESS) {
      inner->type = ScriptInstructionArgument::LONG_ADDRESS;
      return inner;
    } else if (inner->type == ScriptInstructionArgument::INDIRECT) {
      inner->type = ScriptInstructionArgument::LONG_INDIRECT;
      return inner;
    }
  } else if (typeName == "indirect") {
    TypePtr relativeTo = value->getMember("relative_to");
    if (relativeTo->isUndefined()) {
      if (inner->type == ScriptInstructionArgument::ADDRESS) {
        inner->type = ScriptInstructionArgument::INDIRECT;
        return inner;
      } else if (inner->type == ScriptInstructionArgument::LONG_ADDRESS) {
        inner->type = ScriptInstructionArgument::LONG_INDIRECT;
        return inner;
      }
    } else {
      ScriptInstructionArgument *outer = analyzeArgs(relativeTo);
      if (outer != nullptr && outer->type == ScriptInstructionArgument::REG_X) {
        right = outer;
        inner->type = ScriptInstructionArgument::INDIRECT;
        return inner;
      }
    }
  } else if (typeName == "register_member_X") {
    numArgs = 2;
    right = new ScriptInstructionArgument(ScriptInstructionArgument::REG_X);
    return inner;
  } else if (typeName == "register_member_C0") {
    numArgs = 2;
    right = new ScriptInstructionArgument(ScriptInstructionArgument::REG_C0);
    return inner;
  } else if (typeName == "register_member_U") {
    numArgs = 2;
    right = new ScriptInstructionArgument(ScriptInstructionArgument::REG_U);
    return inner;
  } else if (typeName == "register_member_P") {
    numArgs = 2;
    right = new ScriptInstructionArgument(ScriptInstructionArgument::REG_P);
    return inner;
  }

  valid = false;
  delete inner;
  return nullptr;
}

// ----------------------------------------------------------------------------
ScriptInstructionArgument *ScriptInstructionArguments::analyzeRegister(const ObjectPtr &value) {
  TypePtr valueObject = value->getMember("name")->getMember("__value");
  if (!valueObject->isInternalObjectOfType("String")) {
    valid = false;
    return nullptr;
  }

  std::string typeName = dynamic_cast<InternalStringValue*>(valueObject.get())->getValue();
  if (typeName == "A") {
    return new ScriptInstructionArgument(ScriptInstructionArgument::REG_A);
  } else if (typeName == "X") {
    return new ScriptInstructionArgument(ScriptInstructionArgument::REG_X);
  } else if (typeName == "C0") {
    return new ScriptInstructionArgument(ScriptInstructionArgument::REG_C0);
  } else if (typeName == "U") {
    return new ScriptInstructionArgument(ScriptInstructionArgument::REG_U);
  } else if (typeName == "P") {
    return new ScriptInstructionArgument(ScriptInstructionArgument::REG_P);
  } else if (typeName == "S") {
    return new ScriptInstructionArgument(ScriptInstructionArgument::REG_S);
  }

  valid = false;
  return nullptr;
}

// ----------------------------------------------------------------------------
ScriptInstructionArgument *ScriptInstructionArguments::analyzeSymbol(const ObjectPtr &value) {
  TypePtr valueObject = value->getMember("__value");
  if (!valueObject->isInternalObjectOfType("SymbolReference")) {
    valid = false;
    return nullptr;
  }

  InternalReferenceValue *reference = dynamic_cast<InternalReferenceValue*>(valueObject.get());
  return new ScriptInstructionArgument(
    ScriptInstructionArgument::ADDRESS,
    reference->getValue()
  );
}

// ----------------------------------------------------------------------------