#ifndef __FMA65816_LANG_INSTRUCTIONARGUMENTS_H__
#define __FMA65816_LANG_INSTRUCTIONARGUMENTS_H__

#include <fma/interpret/ParameterList.hpp>
#include <fma/types/Base.hpp>
#include <fma/symbol/Reference.hpp>
#include <fma/assem/Operand.hpp>
#include <fma/Project.hpp>

namespace Aoba {

struct ScriptInstructionArgument {
  enum Type {
    INVALID,
    INDIRECT,
    INDIRECT_X,
    IMMEDIATE,
    DIRECT_PAGE,
    LONG_ADDRESS,
    LONG_INDIRECT,
    REG_A,
    REG_X,
    REG_C0,
    REG_U,
    REG_P,
    REG_S,
    ADDRESS,
    PARAM,
    VARIABLE
  };

  enum ValueType {
    VALUE_NONE,
    VALUE_NUMBER,
    VALUE_SYMBOL
  };

  ScriptInstructionArgument()
    : type(INVALID), valueType(VALUE_NONE) {}
  ScriptInstructionArgument(Type type)
    : type(type), valueType(VALUE_NONE) {}
  ScriptInstructionArgument(Type type, const uint64_t &number)
    : type(type), valueType(VALUE_NUMBER), number(number) {}
  ScriptInstructionArgument(Type type, const FMA::symbol::ReferencePtr &reference)
    : type(type), valueType(VALUE_SYMBOL), reference(reference) {}
  
  ~ScriptInstructionArgument() {
  }

  inline bool isConstNumber() const { return valueType == VALUE_NUMBER; }

  Type type;
  ValueType valueType;
  uint64_t number;
  FMA::symbol::ReferencePtr reference;
};

class ScriptInstructionArguments {
public:
  ScriptInstructionArguments(const FMA::interpret::ContextPtr &context, FMA::Project *project, const FMA::interpret::GroupedParameterList &params);
  ~ScriptInstructionArguments();

  inline bool isValid() const { return valid; }

  inline ScriptInstructionArgument *getLeft() const { return left; }
  inline ScriptInstructionArgument *getMid() const { return mid; }
  inline ScriptInstructionArgument *getRight() const { return right; }

  inline uint8_t getNumArgs() const { return numArgs; }
 
protected:
  ScriptInstructionArgument *analyzeArgs(const FMA::types::TypePtr &param);

  ScriptInstructionArgument *analyzeImmediate(const FMA::types::ObjectPtr &value);
  ScriptInstructionArgument *analyzeNumber(const FMA::types::ObjectPtr &value);
  ScriptInstructionArgument *analyzeTypedNumber(const FMA::types::ObjectPtr &value);
  ScriptInstructionArgument *analyzeSymbol(const FMA::types::ObjectPtr &value);
  ScriptInstructionArgument *analyzeRegister(const FMA::types::ObjectPtr &value);
  ScriptInstructionArgument *analyzeVariable(const FMA::types::ObjectPtr &value);

  ScriptInstructionArgument *left;
  ScriptInstructionArgument *mid;
  ScriptInstructionArgument *right;

  FMA::interpret::ContextPtr context;
  FMA::Project *project;
  bool valid;
  uint8_t numArgs;
};

}

#endif
