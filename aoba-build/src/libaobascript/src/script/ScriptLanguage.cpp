#include <aoba/log/Log.hpp>
#include <aoba/script/ScriptLanguage.hpp>
#include <aoba/script/ScriptInstructionArguments.hpp>
#include <aoba/script/ScriptCommand.hpp>
#include <aoba/script/ScriptAssetResolver.hpp>
#include <aoba/script/ScriptProjectInterface.hpp>
#include <aoba/script/ScriptRegisterClass.hpp>
#include <aoba/script/ScriptVariableScopeClass.hpp>
#include <aoba/script/ScriptVariableClass.hpp>
#include <aoba/variable/RamVariableRegistry.hpp>
#include <aoba/variable/RamVariableScope.hpp>
#include <aoba/variable/NumberRamVariable.hpp>
#include <fma/Project.hpp>
#include <fma/interpret/ProjectContext.hpp>
#include <fma/types/Base.hpp>
#include <fma/types/Decorator.hpp>
#include <fma/types/DecoratorContainer.hpp>
#include <fma/types/ClassPrototype.hpp>
#include <fma/types/RootModule.hpp>
#include <fma/types/InternalValue.hpp>
#include <fma/types/Object.hpp>
#include <fma/core/Boolean.hpp>
#include <fma/core/Number.hpp>
#include <fma/core/TypedNumber.hpp>
#include <fma/core/DataBlock.hpp>
#include <fma/core/SymbolReference.hpp>
#include <fma/plugin/MemoryPluginAdapter.hpp>
#include <fma/interpret/Interpreter.hpp>
#include <fma/interpret/BaseContext.hpp>
#include <fma/interpret/Result.hpp>
#include <fma/symbol/ConstantNumber.hpp>
#include <fma/symbol/CalculatedNumber.hpp>
#include <fma/symbol/SymbolReference.hpp>

using namespace Aoba;
using namespace FMA::plugin;
using namespace FMA::interpret;
using namespace FMA::types;
using namespace FMA::plugin;
using namespace FMA::core;
using namespace FMA::symbol;

template<>
char const* FMA::types::getInternalObjectName<Aoba::ScriptProject*>() {
  return "AobaScriptProject";
}

template<>
char const* FMA::types::getInternalObjectName<Aoba::RamVariableScope*>() {
  return "AobaVariableScope";
}

template<>
char const* FMA::types::getInternalObjectName<Aoba::RamVariable*>() {
  return "AobaVariable";
}

#define INSTRUCTION_START(name) \
  proto->setMember(name, TypePtr(new InternalFunctionValue(name, [](const ContextPtr &context, const GroupedParameterList &params) -> ResultPtr { \
    MemoryBlock *block = DataBlockClass::memoryBlock(context); \
    if (block == NULL) { \
      context->log().error() << "Could not access memory block"; \
      return ResultPtr(new Result()); \
    }

#define RETURN_INSTRUCTION(name) \
    INSTRUCTION_START(name) \
    block->setLastIsReturn(true); \
    INSTRUCTION_START_VARIANTS()

#define INSTRUCTION_START_VARIANTS() \
    ScriptInstructionArguments args(context, context->getProject(), params); \
    if (!args.isValid()) {

#define INSTRUCTION(name) \
    INSTRUCTION_START(name) \
    block->setLastIsReturn(false); \
    INSTRUCTION_START_VARIANTS()

#define VARIANT0() \
    } else if (args.getNumArgs() == 0) {

#define VARIANT1(leftType) \
    } else if (args.getNumArgs() == 1 && args.getLeft()->type == ScriptInstructionArgument::leftType) {

#define VARIANT2(leftType, rightType) \
    } else if (args.getNumArgs() == 2 && args.getLeft()->type == ScriptInstructionArgument::leftType && args.getRight()->type == ScriptInstructionArgument::rightType) {

#define VARIANT3(leftType, midType, rightType) \
    } else if (args.getNumArgs() == 3 && args.getLeft()->type == ScriptInstructionArgument::leftType && args.getMid()->type == ScriptInstructionArgument::midType && args.getRight()->type == ScriptInstructionArgument::rightType) {

#define END_VARIANT() \
    return ResultPtr(new Result());

#define END_INSTRUCTION(name) \
    } \
    context->log().error() << "Invalid parameters for " << name << " instruction"; \
    if (args.getNumArgs() >= 1)context->log().error() << "  - " << (int)args.getLeft()->type; \
    if (args.getNumArgs() >= 2)context->log().error() << "  - " << (int)args.getRight()->type; \
    return ResultPtr(new Result()); \
  }))); \
  proto->setMember("::" name, proto->getMember(name))

namespace {
  inline void writeNumberToBlock(MemoryBlock *block, uint64_t number, uint8_t bytes) {
    block->write(&number, bytes);
  }

  inline void writeArg(MemoryBlock *block, const ScriptInstructionArgument *reference, uint8_t bytes) {
    if (reference->isConstNumber()) {
      writeNumberToBlock(block, reference->number, bytes);
    } else {
      block->write(reference->reference, bytes);
    }
  }

  int sizeOfConst(ScriptInstructionArgument *arg, int max) {
    if (!arg->isConstNumber()) {
      return max - 1;
    }

    uint32_t value = arg->number;
    if (max == 1 || value < 0x100) { return 0; }
    else if (max == 2 || value < 0x10000) { return 1; }
    else if (max == 3 || value < 0x1000000) { return 2; }
    return max;
  }

  void writeLongAddressOp(MemoryBlock *block, ScriptCommand command, const ReferencePtr &referencePlusOne, int minus) {
    ReferencePtr one(new ConstantNumber(minus));
    ReferencePtr reference(new CalculatedNumber(referencePlusOne, CalculatedNumber::SUB, one));
    ReferencePtr commandValue(new ConstantNumber((int)command));
    ReferencePtr bankShift(new ConstantNumber(16));
    ReferencePtr bankValue(new CalculatedNumber(reference, CalculatedNumber::RSHIFT, bankShift));
    ReferencePtr bankMask(new ConstantNumber(0x0F));
    ReferencePtr maskedBank(new CalculatedNumber(bankValue, CalculatedNumber::AND, bankMask));
    ReferencePtr shiftedCommandValue(new CalculatedNumber(commandValue, CalculatedNumber::OR, maskedBank));
    block->write(shiftedCommandValue, 1);
    block->write(reference, 2);
  }

  void writeRamOp(MemoryBlock *block, ScriptCommand command, ScriptInstructionArgument *reference) {
    ReferencePtr address;

    if (reference->isConstNumber()) {
      address = ReferencePtr(new ConstantNumber(reference->number));
    } else {
      address = reference->reference;
    }

    ReferencePtr commandValue(new ConstantNumber((int)command));
    ReferencePtr bankShift(new ConstantNumber(16));
    ReferencePtr bankValue(new CalculatedNumber(address, CalculatedNumber::RSHIFT, bankShift));
    ReferencePtr bankMask(new ConstantNumber(0x01));
    ReferencePtr maskedBank(new CalculatedNumber(bankValue, CalculatedNumber::AND, bankMask));
    ReferencePtr shiftedCommandValue(new CalculatedNumber(commandValue, CalculatedNumber::OR, maskedBank));
    block->write(shiftedCommandValue, 1);
    block->write(address, 2);
  }
}

#define WRITE8(value) writeNumberToBlock(block, (uint64_t)value, 1)
#define WRITE16(value) writeNumberToBlock(block, (uint64_t)value, 2)
#define WRITE24(value) writeNumberToBlock(block, (uint64_t)value, 3)
#define WRITE32(value) writeNumberToBlock(block, (uint64_t)value, 4)

// ----------------------------------------------------------------------------
bool ScriptLanguagePlugin::initialize() {
  const RootModulePtr &root = project->root();
  const ClassPtr &object = root->getMember("Object")->asClass();
  const ClassPtr &dataBlock = root->getMember("DataBlock")->asClass();
  const ClassPtr &klass = root->getMember("Class")->asClass();
  ClassPrototypePtr objectProto(object->getPrototype());
  ClassPrototypePtr proto(dataBlock->getPrototype());

  ScriptVariableScopeClass::create(root, klass);
  ScriptVariableClass::create(root, klass);

  INSTRUCTION("ADS8");
    VARIANT0() WRITE8(ScriptCommand::ADD8_STACK_SIGNED); END_VARIANT()
    VARIANT1(IMMEDIATE) WRITE8(ScriptCommand::ADD8_IMMEDIATE); writeArg(block, args.getLeft(), 1); END_VARIANT()
  END_INSTRUCTION("ADS8");
  INSTRUCTION("ADS16");
    VARIANT0() WRITE8(ScriptCommand::ADD16_STACK_SIGNED); END_VARIANT()
    VARIANT1(IMMEDIATE) WRITE8(ScriptCommand::ADD16_IMMEDIATE); writeArg(block, args.getLeft(), 2); END_VARIANT()
  END_INSTRUCTION("ADS16");
  INSTRUCTION("ADS24");
    VARIANT0() WRITE8(ScriptCommand::ADD24_STACK_SIGNED); END_VARIANT()
  END_INSTRUCTION("ADS24");
  INSTRUCTION("ADS32");
    VARIANT0() WRITE8(ScriptCommand::ADD32_STACK_SIGNED); END_VARIANT()
  END_INSTRUCTION("ADS32");
  INSTRUCTION("SBS8");
    VARIANT0() WRITE8(ScriptCommand::SUB8_STACK_SIGNED); END_VARIANT()
    VARIANT1(IMMEDIATE) WRITE8(ScriptCommand::SUB8_IMMEDIATE); writeArg(block, args.getLeft(), 1); END_VARIANT()
  END_INSTRUCTION("SBS8");
  INSTRUCTION("SBS16");
    VARIANT0() WRITE8(ScriptCommand::SUB16_STACK_SIGNED); END_VARIANT()
    VARIANT1(IMMEDIATE) WRITE8(ScriptCommand::SUB16_IMMEDIATE); writeArg(block, args.getLeft(), 2); END_VARIANT()
  END_INSTRUCTION("SBS16");
  INSTRUCTION("SBS24");
    VARIANT0() WRITE8(ScriptCommand::SUB24_STACK_SIGNED); END_VARIANT()
  END_INSTRUCTION("SBS24");
  INSTRUCTION("SBS32");
    VARIANT0() WRITE8(ScriptCommand::SUB32_STACK_SIGNED); END_VARIANT()
  END_INSTRUCTION("SBS32");
  INSTRUCTION("AND8");
    VARIANT1(IMMEDIATE) WRITE8(ScriptCommand::AND8_IMMEDIATE); writeArg(block, args.getLeft(), 1); END_VARIANT()
    VARIANT1(ADDRESS) writeRamOp(block, ScriptCommand::AND8_RAM, args.getLeft()); END_VARIANT()
    VARIANT2(ADDRESS, REG_C0) writeRamOp(block, ScriptCommand::AND8_RAM_C0, args.getLeft()); END_VARIANT()
  END_INSTRUCTION("AND8");
  INSTRUCTION("AND16");
    VARIANT1(IMMEDIATE) WRITE8(ScriptCommand::AND16_IMMEDIATE); writeArg(block, args.getLeft(), 1); END_VARIANT()
    VARIANT1(ADDRESS) writeRamOp(block, ScriptCommand::AND16_RAM, args.getLeft()); END_VARIANT()
    VARIANT2(ADDRESS, REG_C0) writeRamOp(block, ScriptCommand::AND16_RAM_C0, args.getLeft()); END_VARIANT()
  END_INSTRUCTION("AND16");
  INSTRUCTION("ASL");
    VARIANT1(IMMEDIATE) WRITE8(ScriptCommand::ASL_IMMEDIATE); writeArg(block, args.getLeft(), 1); END_VARIANT()
  END_INSTRUCTION("ASL");
  INSTRUCTION("BBC");
    VARIANT2(ADDRESS, ADDRESS) WRITE8(ScriptCommand::BBC); writeArg(block, args.getLeft(), 1); writeArg(block, args.getRight(), 2); END_VARIANT()
  END_INSTRUCTION("BBC");
  INSTRUCTION("BBS");
    VARIANT2(ADDRESS, ADDRESS) WRITE8(ScriptCommand::BBS); writeArg(block, args.getLeft(), 1); writeArg(block, args.getRight(), 2); END_VARIANT()
  END_INSTRUCTION("BBS");
  INSTRUCTION("BGE");
    VARIANT1(ADDRESS) WRITE8(ScriptCommand::BGE); writeArg(block, args.getLeft(), 2); END_VARIANT()
    VARIANT2(IMMEDIATE, ADDRESS) WRITE8(ScriptCommand::BGE_IMMEDIATE); writeArg(block, args.getLeft(), 2); writeArg(block, args.getRight(), 2); END_VARIANT()
  END_INSTRUCTION("BGE");
  INSTRUCTION("BLT");
    VARIANT1(ADDRESS) WRITE8(ScriptCommand::BLT); writeArg(block, args.getLeft(), 2); END_VARIANT()
    VARIANT2(IMMEDIATE, ADDRESS) WRITE8(ScriptCommand::BLT_IMMEDIATE); writeArg(block, args.getLeft(), 2); writeArg(block, args.getRight(), 2); END_VARIANT()
  END_INSTRUCTION("BLT");
  INSTRUCTION("BNZ");
    VARIANT1(ADDRESS) WRITE8(ScriptCommand::BNZ); writeArg(block, args.getLeft(), 2); END_VARIANT()
  END_INSTRUCTION("BNZ");
  INSTRUCTION("BVC");
    VARIANT2(VARIABLE, ADDRESS) WRITE8(ScriptCommand::BVC); writeArg(block, args.getLeft(), 2); writeArg(block, args.getRight(), 2); END_VARIANT()
  END_INSTRUCTION("BVC");
  INSTRUCTION("BVS");
    VARIANT2(VARIABLE, ADDRESS) WRITE8(ScriptCommand::BVS); writeArg(block, args.getLeft(), 2); writeArg(block, args.getRight(), 2); END_VARIANT()
  END_INSTRUCTION("BVS");
  INSTRUCTION("BZE");
    VARIANT1(ADDRESS) WRITE8(ScriptCommand::BZE); writeArg(block, args.getLeft(), 2); END_VARIANT()
  END_INSTRUCTION("BZE");
  INSTRUCTION("CLR");
    VARIANT3(ADDRESS, ADDRESS, REG_C0) WRITE8(ScriptCommand::CLR_BIT + args.getMid()->number); writeArg(block, args.getLeft(), 2); END_VARIANT()
    VARIANT1(VARIABLE) WRITE8(ScriptCommand::CLR_VARIABLE); writeArg(block, args.getLeft(), 2); END_VARIANT()
  END_INSTRUCTION("CLR");
  INSTRUCTION("CNZ");
    VARIANT1(ADDRESS) WRITE8(ScriptCommand::CNZ); writeArg(block, args.getLeft(), 2); END_VARIANT()
  END_INSTRUCTION("CNZ");
  INSTRUCTION("CZE");
    VARIANT1(ADDRESS) WRITE8(ScriptCommand::CZE); writeArg(block, args.getLeft(), 2); END_VARIANT()
  END_INSTRUCTION("CZE");
  INSTRUCTION("EXIT");
    VARIANT0() WRITE8(ScriptCommand::EXIT); END_VARIANT()
  END_INSTRUCTION("EXIT");
  INSTRUCTION("INVOKE");
    VARIANT1(ADDRESS) writeLongAddressOp(block, ScriptCommand::INVOKE, args.getLeft()->reference, 1); END_VARIANT()
  END_INSTRUCTION("INVOKE");
  INSTRUCTION("JMP");
    VARIANT1(ADDRESS) writeLongAddressOp(block, ScriptCommand::JMP, args.getLeft()->reference, 0); END_VARIANT()
  END_INSTRUCTION("JMP");
  INSTRUCTION("JSR");
    VARIANT1(ADDRESS) writeLongAddressOp(block, ScriptCommand::JSR, args.getLeft()->reference, 0); END_VARIANT()
  END_INSTRUCTION("JSR");
  INSTRUCTION("LDA");
    VARIANT1(IMMEDIATE) 
      if (args.getLeft()->isConstNumber()) {
        uint64_t number = args.getLeft()->number;
        if (number < 0x100) { WRITE8(ScriptCommand::LDA8_IMMEDIATE); WRITE8(number); }
        else if (number < 0x10000) { WRITE8(ScriptCommand::LDA16_IMMEDIATE); WRITE16(number); }
        else if (number < 0x1000000) { WRITE8(ScriptCommand::LDA24_IMMEDIATE); WRITE24(number); }
        else { WRITE8(ScriptCommand::LDA32_IMMEDIATE); WRITE32(number); }
        END_VARIANT()
      } else {
      }
    VARIANT1(VARIABLE) WRITE8(ScriptCommand::LDA_VARIABLE); writeArg(block, args.getLeft(), 2); END_VARIANT()
    VARIANT3(ADDRESS, ADDRESS, REG_C0) WRITE8(ScriptCommand::LDA_BIT + args.getMid()->number); writeArg(block, args.getLeft(), 2); END_VARIANT()
  END_INSTRUCTION("LDA");
  INSTRUCTION("LDA8");
    VARIANT1(ADDRESS) writeRamOp(block, ScriptCommand::LDA8_RAM, args.getLeft()); END_VARIANT()
    VARIANT1(PARAM) WRITE8(ScriptCommand::LDA8_PARAM); writeArg(block, args.getLeft(), 1); END_VARIANT()
    VARIANT2(ADDRESS, REG_P) WRITE8(ScriptCommand::LDA8_PARAM); writeArg(block, args.getLeft(), 1); END_VARIANT()
    VARIANT1(IMMEDIATE) WRITE8(ScriptCommand::LDA8_IMMEDIATE); writeArg(block, args.getLeft(), 1); END_VARIANT()
    VARIANT2(ADDRESS, REG_C0) writeRamOp(block, ScriptCommand::LDA8_RAM_C0, args.getLeft()); END_VARIANT()
    VARIANT2(ADDRESS, REG_U) WRITE8(ScriptCommand::LDA8_RAM_U); writeArg(block, args.getLeft(), 1); END_VARIANT()
  END_INSTRUCTION("LDA8");
  INSTRUCTION("LDA16");
    VARIANT1(ADDRESS) writeRamOp(block, ScriptCommand::LDA16_RAM, args.getLeft()); END_VARIANT()
    VARIANT1(PARAM) WRITE8(ScriptCommand::LDA16_PARAM); writeArg(block, args.getLeft(), 1); END_VARIANT()
    VARIANT2(ADDRESS, REG_P) WRITE8(ScriptCommand::LDA16_PARAM); writeArg(block, args.getLeft(), 1); END_VARIANT()
    VARIANT1(IMMEDIATE) WRITE8(ScriptCommand::LDA16_IMMEDIATE); writeArg(block, args.getLeft(), 2); END_VARIANT()
    VARIANT2(ADDRESS, REG_C0) writeRamOp(block, ScriptCommand::LDA16_RAM_C0, args.getLeft()); END_VARIANT()
    VARIANT2(ADDRESS, REG_U) WRITE8(ScriptCommand::LDA16_RAM_U); writeArg(block, args.getLeft(), 1); END_VARIANT()
  END_INSTRUCTION("LDA16");
  INSTRUCTION("LDA24");
    VARIANT1(PARAM) WRITE8(ScriptCommand::LDA24_PARAM); writeArg(block, args.getLeft(), 1); END_VARIANT()
    VARIANT2(ADDRESS, REG_P) WRITE8(ScriptCommand::LDA24_PARAM); writeArg(block, args.getLeft(), 1); END_VARIANT()
    VARIANT1(IMMEDIATE) WRITE8(ScriptCommand::LDA24_IMMEDIATE); writeArg(block, args.getLeft(), 3); END_VARIANT()
  END_INSTRUCTION("LDA24");
  INSTRUCTION("LDA32");
    VARIANT1(PARAM) WRITE8(ScriptCommand::LDA32_PARAM); writeArg(block, args.getLeft(), 1); END_VARIANT()
    VARIANT2(ADDRESS, REG_P) WRITE8(ScriptCommand::LDA32_PARAM); writeArg(block, args.getLeft(), 1); END_VARIANT()
    VARIANT1(IMMEDIATE) WRITE8(ScriptCommand::LDA32_IMMEDIATE); writeArg(block, args.getLeft(), 4); END_VARIANT()
  END_INSTRUCTION("LDA32");
  INSTRUCTION("LDI");
    VARIANT1(VARIABLE) WRITE8(ScriptCommand::LDI_VARIABLE); writeArg(block, args.getLeft(), 2); END_VARIANT()
  END_INSTRUCTION("LDI");
  INSTRUCTION("LDX");
    VARIANT1(IMMEDIATE) 
      if (args.getLeft()->isConstNumber()) {
        uint64_t number = args.getLeft()->number;
        if (number < 0x100) { WRITE8(ScriptCommand::LDX8_IMMEDIATE); WRITE8(number); }
        else { WRITE8(ScriptCommand::LDX16_IMMEDIATE); WRITE16(number); }
        END_VARIANT()
      } else {
      }
    //VARIANT1(VARIABLE) WRITE8(ScriptCommand::LDX_VARIABLE); writeArg(block, args.getLeft(), 2); END_VARIANT()
  END_INSTRUCTION("LDX");
  INSTRUCTION("LDX8");
    //VARIANT1(ADDRESS) writeRamOp(block, ScriptCommand::LDX8_RAM, args.getLeft()->reference); END_VARIANT()
    //VARIANT1(PARAM) WRITE8(ScriptCommand::LDX8_PARAM); writeArg(block, args.getLeft(), 1); END_VARIANT()
    VARIANT1(IMMEDIATE) WRITE8(ScriptCommand::LDX8_IMMEDIATE); writeArg(block, args.getLeft(), 1); END_VARIANT()
  END_INSTRUCTION("LDX8");
  INSTRUCTION("LDX16");
    //VARIANT1(ADDRESS) writeRamOp(block, ScriptCommand::LDX16_RAM, args.getLeft()->reference); END_VARIANT()
    //VARIANT1(PARAM) WRITE8(ScriptCommand::LDX16_PARAM); writeArg(block, args.getLeft(), 1); END_VARIANT()
    VARIANT1(IMMEDIATE) WRITE8(ScriptCommand::LDX16_IMMEDIATE); writeArg(block, args.getLeft(), 2); END_VARIANT()
  END_INSTRUCTION("LDX16");
  INSTRUCTION("LSR");
    VARIANT1(IMMEDIATE) WRITE8(ScriptCommand::LSR_IMMEDIATE); writeArg(block, args.getLeft(), 1); END_VARIANT()
  END_INSTRUCTION("LSR");
  INSTRUCTION("ORA8");
    VARIANT1(ADDRESS) writeRamOp(block, ScriptCommand::ORA8_RAM, args.getLeft()); END_VARIANT()
    VARIANT2(ADDRESS, REG_C0) writeRamOp(block, ScriptCommand::ORA8_RAM_C0, args.getLeft()); END_VARIANT()
  END_INSTRUCTION("ORA8");
  INSTRUCTION("ORA16");
    VARIANT1(ADDRESS) writeRamOp(block, ScriptCommand::ORA16_RAM, args.getLeft()); END_VARIANT()
    VARIANT2(ADDRESS, REG_C0) writeRamOp(block, ScriptCommand::ORA16_RAM_C0, args.getLeft()); END_VARIANT()
  END_INSTRUCTION("ORA16");
  INSTRUCTION("FUNC_END");
    VARIANT0() WRITE8(ScriptCommand::FUNC_END); WRITE8(0); END_VARIANT()
    VARIANT1(ADDRESS) WRITE8(ScriptCommand::FUNC_END); writeArg(block, args.getLeft(), 1); END_VARIANT()
  END_INSTRUCTION("FUNC_END");
  INSTRUCTION("FUNC_START");
    VARIANT0() WRITE8(ScriptCommand::FUNC_START); WRITE8(0); END_VARIANT()
    VARIANT1(ADDRESS) WRITE8(ScriptCommand::FUNC_START); writeArg(block, args.getLeft(), 1); END_VARIANT()
  END_INSTRUCTION("FUNC_START");
  INSTRUCTION("DEC");
    VARIANT0() WRITE8(ScriptCommand::DEC); END_VARIANT()
  END_INSTRUCTION("DEC");
  INSTRUCTION("INC");
    VARIANT0() WRITE8(ScriptCommand::INC); END_VARIANT()
  END_INSTRUCTION("INC");
  /*INSTRUCTION("XBA");
    VARIANT0() WRITE8(ScriptCommand::XBA); END_VARIANT()
  END_INSTRUCTION("XBA");*/
  INSTRUCTION("PHA8");
    VARIANT0() WRITE8(ScriptCommand::PHA8); END_VARIANT()
  END_INSTRUCTION("PHA8");
  INSTRUCTION("PHA16");
    VARIANT0() WRITE8(ScriptCommand::PHA16); END_VARIANT()
  END_INSTRUCTION("PHA16");
  INSTRUCTION("PHA24");
    VARIANT0() WRITE8(ScriptCommand::PHA24); END_VARIANT()
  END_INSTRUCTION("PHA24");
  INSTRUCTION("PHA32");
    VARIANT0() WRITE8(ScriptCommand::PHA32); END_VARIANT()
  END_INSTRUCTION("PHA32");
  INSTRUCTION("PHC0");
    VARIANT0() WRITE8(ScriptCommand::PHC0); END_VARIANT()
  END_INSTRUCTION("PHC0");
  INSTRUCTION("PLA8");
    VARIANT0() WRITE8(ScriptCommand::PLA8); END_VARIANT()
  END_INSTRUCTION("PLA8");
  INSTRUCTION("PLA16");
    VARIANT0() WRITE8(ScriptCommand::PLA16); END_VARIANT()
  END_INSTRUCTION("PLA16");
  INSTRUCTION("PLA24");
    VARIANT0() WRITE8(ScriptCommand::PLA24); END_VARIANT()
  END_INSTRUCTION("PLA24");
  INSTRUCTION("PLA32");
    VARIANT0() WRITE8(ScriptCommand::PLA32); END_VARIANT()
  END_INSTRUCTION("PLA32");
  INSTRUCTION("PLC0");
    VARIANT0() WRITE8(ScriptCommand::PLC0); END_VARIANT()
  END_INSTRUCTION("PLC0");
  INSTRUCTION("PUSH8");
    VARIANT1(IMMEDIATE) WRITE8(ScriptCommand::PUSH8_IMMEDIATE8); writeArg(block, args.getLeft(), 1); END_VARIANT()
  END_INSTRUCTION("PUSH8");
  INSTRUCTION("PUSH16");
    VARIANT1(IMMEDIATE) WRITE8(ScriptCommand::PUSH16_IMMEDIATE8 + sizeOfConst(args.getLeft(), 2)); writeArg(block, args.getLeft(), 1+sizeOfConst(args.getLeft(), 2)); END_VARIANT()
  END_INSTRUCTION("PUSH16");
  INSTRUCTION("PUSH24");
    VARIANT1(IMMEDIATE) WRITE8(ScriptCommand::PUSH24_IMMEDIATE8 + sizeOfConst(args.getLeft(), 3)); writeArg(block, args.getLeft(), 1+sizeOfConst(args.getLeft(), 3)); END_VARIANT()
  END_INSTRUCTION("PUSH24");
  INSTRUCTION("PUSH32");
    VARIANT1(IMMEDIATE) WRITE8(ScriptCommand::PUSH32_IMMEDIATE32); writeArg(block, args.getLeft(), 4); END_VARIANT()
  END_INSTRUCTION("PUSH32");
  RETURN_INSTRUCTION("RTS");
    VARIANT0()
      WRITE8(ScriptCommand::RTS); END_VARIANT()
  END_INSTRUCTION("RTS");
  INSTRUCTION("SET");
    VARIANT3(ADDRESS, ADDRESS, REG_C0) WRITE8(ScriptCommand::SET_BIT + args.getMid()->number); writeArg(block, args.getLeft(), 2); END_VARIANT()
    VARIANT1(VARIABLE) WRITE8(ScriptCommand::SET_VARIABLE); writeArg(block, args.getLeft(), 2); END_VARIANT()
  END_INSTRUCTION("SET");
  INSTRUCTION("STA");
    VARIANT1(VARIABLE) WRITE8(ScriptCommand::STA_VARIABLE); writeArg(block, args.getLeft(), 2); END_VARIANT()
  END_INSTRUCTION("STA");
  INSTRUCTION("STA8");
    VARIANT1(PARAM) WRITE8(ScriptCommand::STA8_PARAM); writeArg(block, args.getLeft(), 1); END_VARIANT()
    VARIANT2(ADDRESS, REG_P) WRITE8(ScriptCommand::STA8_PARAM); writeArg(block, args.getLeft(), 1); END_VARIANT()
    VARIANT1(ADDRESS) writeRamOp(block, ScriptCommand::STA8_RAM, args.getLeft()); END_VARIANT()
    VARIANT2(ADDRESS, REG_C0) writeRamOp(block, ScriptCommand::STA8_RAM_C0, args.getLeft()); END_VARIANT()
    VARIANT2(ADDRESS, REG_U) WRITE8(ScriptCommand::STA8_RAM_U); writeArg(block, args.getLeft(), 1); END_VARIANT()
  END_INSTRUCTION("STA8");
  INSTRUCTION("STA16");
    VARIANT1(PARAM) WRITE8(ScriptCommand::STA16_PARAM); writeArg(block, args.getLeft(), 1); END_VARIANT()
    VARIANT2(ADDRESS, REG_P) WRITE8(ScriptCommand::STA16_PARAM); writeArg(block, args.getLeft(), 1); END_VARIANT()
    VARIANT1(ADDRESS) writeRamOp(block, ScriptCommand::STA16_RAM, args.getLeft()); END_VARIANT()
    VARIANT2(ADDRESS, REG_C0) writeRamOp(block, ScriptCommand::STA16_RAM_C0, args.getLeft()); END_VARIANT()
    VARIANT2(ADDRESS, REG_U) WRITE8(ScriptCommand::STA16_RAM_U); writeArg(block, args.getLeft(), 1); END_VARIANT()
  END_INSTRUCTION("STA16");
  INSTRUCTION("STA24");
    VARIANT1(PARAM) WRITE8(ScriptCommand::STA24_PARAM); writeArg(block, args.getLeft(), 1); END_VARIANT()
    VARIANT2(ADDRESS, REG_P) WRITE8(ScriptCommand::STA24_PARAM); writeArg(block, args.getLeft(), 1); END_VARIANT()
  END_INSTRUCTION("STA24");
  INSTRUCTION("TAX");
    VARIANT0() WRITE8(ScriptCommand::TAX); END_VARIANT()
  END_INSTRUCTION("TAX");
  INSTRUCTION("TXA");
    VARIANT0() WRITE8(ScriptCommand::TXA); END_VARIANT()
  END_INSTRUCTION("TXA");
  INSTRUCTION("TAC0");
    VARIANT0() WRITE8(ScriptCommand::TAC0); END_VARIANT()
  END_INSTRUCTION("TAC0");
  INSTRUCTION("TC0A");
    VARIANT0() WRITE8(ScriptCommand::TC0A); END_VARIANT()
  END_INSTRUCTION("TC0A");

  objectProto->setMember("is_param?", TypePtr(new InternalFunctionValue("is_param?", ScriptLanguagePlugin::is_param)));

  proto->setMember("::__asset", TypePtr(new InternalFunctionValue("__asset", ScriptLanguagePlugin::__asset)));
  proto->setMember("::__param", TypePtr(new InternalFunctionValue("__param", ScriptLanguagePlugin::__param)));
  root->setMember("__engine_variable", TypePtr(new InternalFunctionValue("__engine_variable", ScriptLanguagePlugin::__engine_variable)));
  root->setMember("__asset", TypePtr(new InternalFunctionValue("__asset", ScriptLanguagePlugin::__asset)));
  proto->setMember("::__translate", TypePtr(new InternalFunctionValue("__translate", ScriptLanguagePlugin::__translate)));
  proto->setMember("::__translate_comment", TypePtr(new InternalFunctionValue("__translate_comment", ScriptLanguagePlugin::__translate_comment)));
  //root->setMember("__translate", TypePtr(new InternalFunctionValue("__translate", ScriptLanguagePlugin::__translate)));
  
  Interpreter interpreter(project);
  ContextPtr context(new ProjectContext(&interpreter));
  ClassPtr Register = ScriptRegisterClass::create(root, klass);
  root->setMember("C0", ScriptRegisterClass::createInstance(context, "C0")->get());
  root->setMember("U", ScriptRegisterClass::createInstance(context, "U")->get());
  root->setMember("P", ScriptRegisterClass::createInstance(context, "P")->get());
  root->setMember("scope", TypePtr(new InternalFunctionValue("__scope", ScriptLanguagePlugin::__scope)));
  root->setMember("declare", TypePtr(new InternalFunctionValue("__declare", ScriptLanguagePlugin::__declare)));

  root->setMember("@scriptProject", TypePtr(new InternalObject<ScriptProject*>(mProject)));

  TypePtr container = root->getMember(FMA_TYPE_DECORATORCONTAINER_MEMBER);
  if (container->isDecoratorContainer()) {
    DecoratorContainerPtr containerPtr(std::dynamic_pointer_cast<DecoratorContainer>(container));
    GroupedParameterList params;
    DecoratorPtr decorator(new Decorator(TypePtr(new InternalFunctionValue("auto_insert_rts", ScriptLanguagePlugin::auto_insert_rts)), params));
    containerPtr->registerCallback(decorator);
  }

  return true;
}

// ----------------------------------------------------------------------------
ResultPtr ScriptLanguagePlugin::__asset(const ContextPtr &context, const GroupedParameterList &parameter) {
  const auto &args = parameter.only_args();
  if (args.size() != 3) {
    context->log().error() << "Could not find data block";
    return NumberClass::createInstance(context, 0);
  }
  
  auto &typeValue = args.front();
  auto &idValue = args.at(1);
  auto &subTypeValue = args.back();
  QString type(typeValue->hasMember("to_s") ? typeValue->convertToString(context).c_str() : typeValue->asString().c_str());
  QString id(idValue->hasMember("to_s") ? idValue->convertToString(context).c_str() : idValue->asString().c_str());
  QString subType(subTypeValue->hasMember("to_s") ? subTypeValue->convertToString(context).c_str() : subTypeValue->asString().c_str());

  ResultPtr scriptProjectResult(context->getRootLevelContext()->getMember("@scriptProject"));
  if (!scriptProjectResult->get()->isInternalObjectOfType("AobaScriptProject")) {
    context->log().error() << "Failed to fetch script project on root level context";
    return NumberClass::createInstance(context, 0);
  }

  ScriptProject *project = dynamic_cast<InternalObject<ScriptProject*>*>(scriptProjectResult->get().get())->getValue();
  if (!project->hasAssetResolver(type)) {
    context->log().error() << "No asset resolver for type " << type.toStdString();
    return NumberClass::createInstance(context, 0);
  }

  return project->assetResolver(type)->scriptResolve(context, id, subType);
}

// ----------------------------------------------------------------------------
ResultPtr ScriptLanguagePlugin::is_param(const ContextPtr &context, const GroupedParameterList &) {
  if (!context->self()->isObjectOfType("TypedNumber")) {
    return BooleanClass::createInstance(context, false);
  }

  if (context->self()->getDirectMember("type")->convertToString(context) != "param") {
    return BooleanClass::createInstance(context, false);
  }

  return BooleanClass::createInstance(context, true);
}

// ----------------------------------------------------------------------------
ResultPtr ScriptLanguagePlugin::__scope(const ContextPtr &context, const GroupedParameterList &parameter) {
  ResultPtr scriptProjectResult(context->getRootLevelContext()->getMember("@scriptProject"));
  if (!scriptProjectResult->get()->isInternalObjectOfType("AobaScriptProject")) {
    context->log().error() << "Failed to fetch script project on root level context";
    return NumberClass::createInstance(context, 0);
  }

  ScriptProject *project = dynamic_cast<InternalObject<ScriptProject*>*>(scriptProjectResult->get().get())->getValue();
  ScriptProjectInterface *projectInterface = project->projectInterface();

  const auto &args = parameter.only_args();
  const auto &kwargs = parameter.only_kwargs();
  if (!args.size()) {
    context->log().error() << "Missing name for scope";
    return NumberClass::createInstance(context, 0);
  }

  bool root = false;
  bool global = false;
  bool shared = false;
  bool persistent = false;

  QString name(args.front()->convertToString(context).c_str());

  MemberMapConstIterator it;
  if ((it = kwargs.find("root")) != kwargs.end()) {
    root = it->second->convertToBoolean(context);
  }
  if ((it = kwargs.find("shared")) != kwargs.end()) {
    shared = it->second->convertToBoolean(context);
  }
  if ((it = kwargs.find("persistent")) != kwargs.end()) {
    persistent = it->second->convertToBoolean(context);
  }
  if ((it = kwargs.find("global")) != kwargs.end()) {
    global = it->second->convertToBoolean(context);
  }

  RamVariableScope *scope;
  if (root) {
    scope = projectInterface->getVariables()->root()->createScope(name, shared, global, persistent);
  } else {
    if ((it = kwargs.find("in")) == kwargs.end()) {
      context->log().error() << "Missing 'in' parameter for scope declaration";
      return NumberClass::createInstance(context, 0);
    }

    scope = ScriptVariableScopeClass::asVariableScope(context->getInterpreter()->getProject(), it->second);
    if (!scope) {
      context->log().error() << "Invalid scope for 'in' parameter at scope declaration";
      return NumberClass::createInstance(context, 0);
    }

    scope = scope->createScope(name, shared, global, persistent);
  }
  
  ClassPtr scopeClass = context->getRootLevelContext()->resolve("RamVariableScope")->asClass();
  if (!scopeClass) {
    context->log().error() << "Failed to resolve RamVariableScope class";
    return ResultPtr(new Result());
  }

  GroupedParameterList createParameter;
  createParameter.push_back(TypePtr(new InternalObject<RamVariableScope*>(scope)));
  TypePtr type = scopeClass->createInstance(context, createParameter);

  context->getCallee()->setMember(name.toStdString(), type);
  return ResultPtr(new Result(context, type));
}

// ----------------------------------------------------------------------------
ResultPtr ScriptLanguagePlugin::__declare(const ContextPtr &context, const GroupedParameterList &parameter) {
  const auto &args = parameter.only_args();
  const auto &kwargs = parameter.only_kwargs();
  if (!args.size()) {
    context->log().error() << "Missing name for variable";
    return NumberClass::createInstance(context, 0);
  }

  RamVariableScope *scope;
  MemberMapConstIterator it;
  if ((it = kwargs.find("in")) == kwargs.end()) {
    context->log().error() << "Missing argument 'in' for variable declaration";
    return NumberClass::createInstance(context, 0);
  }

  scope = ScriptVariableScopeClass::asVariableScope(context->getInterpreter()->getProject(), it->second);
  if (!scope) {
    context->log().error() << "Invalid scope for 'in' parameter at variable declaration";
    return NumberClass::createInstance(context, 0);
  }

  if ((it = kwargs.find("as")) == kwargs.end()) {
    context->log().error() << "Missing argument 'as' for variable declaration";
    return NumberClass::createInstance(context, 0);
  }
  QString type(it->second->convertToString(context).c_str());

  bool decompressed = false;
  if ((it = kwargs.find("decompressed_access")) != kwargs.end()) {
    decompressed = it->second->convertToBoolean(context);
  }

  QString name(args.front()->convertToString(context).c_str());
  QString fullName(context->getCallee()->getParentNameHint().c_str());

  if (context->getCallee()->isModule()) {
    fullName += QString(".") + context->getCallee()->asModule()->getName().c_str();
  }

  if (fullName.isEmpty()) {
    fullName = name;
  } else {
    fullName += QString(".") + name;
  }

  RamVariable *v;
  if (type == "number") {
    int min = 0;
    int max = 127;
    if ((it = kwargs.find("min")) != kwargs.end()) {
      min = it->second->convertToNumber(context);
    }
    if ((it = kwargs.find("max")) == kwargs.end()) {
      context->log().error() << "Please provide a 'max' argument for number variables";
      return NumberClass::createInstance(context, 0);
    }
    max = it->second->convertToNumber(context);

    NumberRamVariable *nv = scope->createNumberVariable(fullName, min, max);
    if ((it = kwargs.find("default")) != kwargs.end()) {
      nv->setDefaultValue(it->second->convertToNumber(context));
    }

    v = nv;
  } else if (type == "boolean" || type == "bool" || type == "flag") {
    NumberRamVariable *nv = scope->createBoolean(fullName);
    if ((it = kwargs.find("default")) != kwargs.end()) {
      nv->setDefaultValue(it->second->convertToBoolean(context));
    }
    v = nv;
  } else {
    context->log().error() << "Unsupported variable type " << type.toStdString() << " for variable " << name.toStdString();
    return ResultPtr(new Result());
  }

  v->setShouldDecompress(decompressed);

  ClassPtr variableClass = context->getRootLevelContext()->resolve("RamVariable")->asClass();
  if (!variableClass) {
    context->log().error() << "Failed to resolve RamVariable class";
    return ResultPtr(new Result());
  }


  GroupedParameterList createParameter;
  createParameter.push_back(TypePtr(new InternalObject<RamVariable*>(v)));
  TypePtr variableClassType = variableClass->createInstance(context, createParameter);
  context->getCallee()->setMember(name.toStdString(), variableClassType);

  return ResultPtr(new Result(context, variableClassType));
}

// ----------------------------------------------------------------------------
ResultPtr ScriptLanguagePlugin::__translate(const ContextPtr &context, const GroupedParameterList &parameter) {
  MemoryBlock *block = DataBlockClass::memoryBlock(context);
  const auto &args = parameter.only_args();
  const auto &kwargs = parameter.only_kwargs();
  if (!block || !args.size()) {
    context->log().error() << "Could not find data block";
    return NumberClass::createInstance(context, 0);
  }
  
  auto &value = args.front();
  QString font("default");
  QString scope(block->getNameHint().c_str());
  MemberMapConstIterator it;
  if ((it = kwargs.find("font")) != kwargs.end()) {
    font = it->second->convertToString(context).c_str();
  }
  if ((it = kwargs.find("context")) != kwargs.end()) {
    if (!it->second->isObjectOfType("Nil")) {
      scope = it->second->convertToString(context).c_str();
    }
  }
  
  QString text(value->hasMember("to_s") ? value->convertToString(context).c_str() : value->asString().c_str());
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

  QString symbol = projectInterface->addTranslation(text, font, scope);
  return SymbolReferenceClass::createInstance(context, SymbolReferencePtr(new SymbolReference(symbol.toStdString())));
}

// ----------------------------------------------------------------------------
ResultPtr ScriptLanguagePlugin::__translate_comment(const ContextPtr &context, const GroupedParameterList &parameter) {
  MemoryBlock *block = DataBlockClass::memoryBlock(context);
  const auto &args = parameter.only_args();
  const auto &kwargs = parameter.only_kwargs();
  if (!block || !args.size()) {
    context->log().error() << "Could not find data block";
    return NumberClass::createInstance(context, 0);
  }
  
  auto &value = args.front();
  QString scope(block->getNameHint().c_str());
  MemberMapConstIterator it;
  bool isHeader = false;
  if ((it = kwargs.find("header")) != kwargs.end()) {
    isHeader = it->second->convertToBoolean(context);
  }
  if ((it = kwargs.find("context")) != kwargs.end()) {
    if (!it->second->isObjectOfType("Nil")) {
      scope = it->second->convertToString(context).c_str();
    }
  }
  QString text(value->hasMember("to_s") ? value->convertToString(context).c_str() : value->asString().c_str());
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
  
  if (isHeader) {
    projectInterface->addTranslationHeader(text, scope);
  } else {
    projectInterface->addTranslationComment(text, scope);
  }

  return NumberClass::createInstance(context, 0);
}

// ----------------------------------------------------------------------------
ResultPtr ScriptLanguagePlugin::__param(const ContextPtr &context, const GroupedParameterList &parameter) {
  MemoryBlock *block = DataBlockClass::memoryBlock(context);

  const auto &args = parameter.only_args();
  if (!block || !args.size()) {
    context->log().error() << "Could not find data block";
    return NumberClass::createInstance(context, 0);
  }
  
  int size = args.front()->convertToNumber(context);
  if (size < 1) {
    context->log().error() << "Parameter size must be at least 1";
    return NumberClass::createInstance(context, 0);
  }

  if (!context->self()->hasMember("__param_counter")) {
    context->self()->setMember("__param_counter", NumberClass::createInstance(context, size)->get());
    return TypedNumberClass::createInstance(context, NumberClass::createInstance(context, 0)->get(), "param");
  } else {
    int oldSize = context->self()->getMember("__param_counter")->convertToNumber(context);
    context->self()->setMember("__param_counter", NumberClass::createInstance(context, size + oldSize)->get());
    return TypedNumberClass::createInstance(context, NumberClass::createInstance(context, oldSize)->get(), "param");
  }
}

// ----------------------------------------------------------------------------
ResultPtr ScriptLanguagePlugin::__engine_variable(const ContextPtr &context, const GroupedParameterList &parameter) {
  const auto &args = parameter.only_args();
  if (!args.size()) {
    context->log().error() << "Missing variable name for engine variable";
    return NumberClass::createInstance(context, 0);
  }
  
  std::string name = args.front()->convertToString(context);
  return FMA::core::SymbolReferenceClass::createInstance(context, FMA::symbol::ReferencePtr(new FMA::symbol::SymbolReference(name)));
}

// ----------------------------------------------------------------------------
ResultPtr ScriptLanguagePlugin::auto_insert_rts(const ContextPtr &context, const GroupedParameterList &params) {
  GroupedParameterList empty;
  params.only_blocks().front()->call(context, empty);

  TypePtr type = context->getInterpreter()->getGlobalContext()->resolve("::__current_block")->get();
  MemoryBlock *block = DataBlockClass::memoryBlock(context);
  if (type->isObjectOfType("Function") && !block->isReturned()) {
    context->getInterpreter()->getGlobalContext()->resolve("::RTS")->get();
  }

  return ResultPtr(new Result());
}

// ----------------------------------------------------------------------------
