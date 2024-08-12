#include <aoba/log/Log.hpp>
#include <QStringList>
#include <aoba/script/ScriptMemoryBlock.hpp>
#include <aoba/script/ScriptMemoryAdapter.hpp>
#include <aoba/script/ScriptProject.hpp>
#include <fma/memory/MemoryLocationList.hpp>
#include <fma/symbol/Reference.hpp>
#include <fma/symbol/SymbolReference.hpp>

using namespace Aoba;
using namespace FMA::assem;
using namespace FMA::symbol;
using namespace FMA::plugin;

namespace {
  uint32_t nextScriptId = 0;
}

// ----------------------------------------------------------------------------
ScriptMemoryBlock::ScriptMemoryBlock(ScriptProject *project, Usage usage)
  : FMA::plugin::MemoryBlock(usage)
  , mProject(project)
  , mLinkerBlock(mProject->fmaLinkerObject().createBlock())
{
  mScriptId = "__custom_script" + QString::number(nextScriptId);
  nextScriptId++;

  mLinkerBlock->symbol(mScriptId.toStdString());
  
  FMA::memory::MemoryLocationList *locationList = new FMA::memory::MemoryLocationList();
  FMA::memory::MemoryLocationConstraint location(true);
  location.banks(0xC0, 0xCF);
  locationList->add(location);
  mLinkerBlock->setLocation(FMA::plugin::MemoryLocationPtr(locationList));
}

// ----------------------------------------------------------------------------
SymbolReferencePtr ScriptMemoryBlock::createReference() {
  return SymbolReferencePtr(new SymbolReference(mScriptId.toStdString()));
}

// ----------------------------------------------------------------------------
void ScriptMemoryBlock::markIsUsed(bool used) {
  mHasBeenUsed = mHasBeenUsed || used;
}

// ----------------------------------------------------------------------------
void ScriptMemoryBlock::write(void *data, uint32_t size) {
  mLinkerBlock->write(data, size);
}

// ----------------------------------------------------------------------------
void ScriptMemoryBlock::write(const ReferencePtr &reference, uint32_t size) {
  mLinkerBlock->write(reference, size);
}

// ----------------------------------------------------------------------------
void ScriptMemoryBlock::write(Instruction *instruct) {
  (void)instruct;
  Aoba::log::debug("ScriptMemoryBlock::write(instruct)");
}

// ----------------------------------------------------------------------------
ReferencePtr ScriptMemoryBlock::reference() {
  Aoba::log::debug("ScriptMemoryBlock::reference()");
  return ReferencePtr();
}

// ----------------------------------------------------------------------------
ReferencePtr ScriptMemoryBlock::reference(const SymbolReferencePtr &reference) {
  mLinkerBlock->symbol(reference->getName());
  return reference;
}

// ----------------------------------------------------------------------------
bool ScriptMemoryBlock::buildByteCode(BinaryCodeGeneratorScope *scope) {
  (void)scope;
  Aoba::log::debug("ScriptMemoryBlock::buildByteCode()");
  return false;
}

// ----------------------------------------------------------------------------
MemoryLocationPtr ScriptMemoryBlock::location() {
  Aoba::log::debug("ScriptMemoryBlock::location()");
  return MemoryLocationPtr();
}

// ----------------------------------------------------------------------------