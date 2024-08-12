#include <aoba/log/Log.hpp>
#include <aoba/script/ScriptMemoryBlock.hpp>
#include <aoba/script/ScriptMemoryAdapter.hpp>
#include <aoba/script/ScriptProject.hpp>
#include <fma/symbol/SymbolReference.hpp>

using namespace Aoba;
using namespace FMA::plugin;


// ----------------------------------------------------------------------------
ScriptMemoryAdapter::~ScriptMemoryAdapter() {
  for (MemoryBlock *block : mBlocks) {
    delete block;
  }
}

// ----------------------------------------------------------------------------
MemorySymbolMap *ScriptMemoryAdapter::getSymbolMap() const {
  return mScriptProject->symbolMap();
}

// ----------------------------------------------------------------------------
MemoryBlock *ScriptMemoryAdapter::allocateBlock(MemoryBlock::Usage usage, const std::string &name) {
  ScriptMemoryBlock *block = new ScriptMemoryBlock(mScriptProject, usage);
  block->setNameHint(name);
  mBlocks.push_back(block);
  return block;
}

// ----------------------------------------------------------------------------
const MemoryBlockList &ScriptMemoryAdapter::getBlocks() {
  Aoba::log::debug("ScriptMemoryAdapter::getBlocks()");
  return mBlocks;
}

// ----------------------------------------------------------------------------
FMA::symbol::SymbolReferencePtr ScriptMemoryAdapter::createReference(const std::string &name) {
  return FMA::symbol::SymbolReferencePtr(
    new FMA::symbol::SymbolReference("__label." + name + "." + std::to_string(mNextSafeLabelId++))
  );
}

// ----------------------------------------------------------------------------
bool ScriptMemoryAdapter::placeDynamicBlocks() {
  Aoba::log::debug("ScriptMemoryAdapter::placeDynamicBlocks()");
  return false;
}

// ----------------------------------------------------------------------------
bool ScriptMemoryAdapter::placeStaticBlocks() {
  Aoba::log::debug("ScriptMemoryAdapter::placeStaticBlocks()");
  return false;
}

// ----------------------------------------------------------------------------
bool ScriptMemoryAdapter::registerLinkerBlockSymbols(FMA::linker::LinkerBlock *block) {
  (void)block;
  Aoba::log::debug("ScriptMemoryAdapter::registerLinkerBlockSymbols()");
  return false;
}

// ----------------------------------------------------------------------------
const MemoryBlockPlacement *ScriptMemoryAdapter::requireStaticBlockPlacement(FMA::linker::LinkerBlock *block) {
  (void)block;
  Aoba::log::debug("ScriptMemoryAdapter::requireStaticBlockPlacement()");
  return nullptr;
}



// ----------------------------------------------------------------------------
MemoryPluginAdapter *ScriptMemoryPlugin::createAdapter() {
  return new ScriptMemoryAdapter(mScriptProject, project);
}

// ----------------------------------------------------------------------------
void ScriptMemoryPlugin::releaseAdapter(MemoryPluginAdapter *adapter) {
  delete adapter;
}

// ----------------------------------------------------------------------------