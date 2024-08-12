#pragma once

#include <fma/plugin/Plugin.hpp>
#include <fma/plugin/MemoryPluginAdapter.hpp>

namespace Aoba {
class ScriptProject;

class ScriptMemoryAdapter : public FMA::plugin::MemoryPluginAdapter {
public:
  //! Constructor
  ScriptMemoryAdapter(ScriptProject *scriptProject, FMA::Project *project)
    : FMA::plugin::MemoryPluginAdapter(project)
    , mScriptProject(scriptProject)
  {}

  //! Deconstructor
  ~ScriptMemoryAdapter();

  //! Returns the symbol map
  FMA::plugin::MemorySymbolMap *getSymbolMap() const override;

  //! Allocated a new block
  FMA::plugin::MemoryBlock *allocateBlock(FMA::plugin::MemoryBlock::Usage usage, const std::string &name) override;

  //! Returns a list of all blocks
  const FMA::plugin::MemoryBlockList &getBlocks() override;

  //! Creates a reference
  FMA::symbol::SymbolReferencePtr createReference(const std::string &name) override;

  //! Places all dynamic blocks
  bool placeDynamicBlocks() override;

  //! Places all static blocks
  bool placeStaticBlocks() override;

  //! Registers symbols from the block
  bool registerLinkerBlockSymbols(FMA::linker::LinkerBlock *block) override;

  //! Returns placement
  const FMA::plugin::MemoryBlockPlacement *requireStaticBlockPlacement(FMA::linker::LinkerBlock *block) override;

protected:
  //! The script project
  ScriptProject *mScriptProject;

  //! List of all blocks
  FMA::plugin::MemoryBlockList mBlocks;

  //! The next safe label id
  int mNextSafeLabelId = 1;
};

// -----------------------------------------------------------------------------
class ScriptMemoryPlugin : public FMA::plugin::MemoryManagerPlugin {
public:
  //! Constructor
  ScriptMemoryPlugin(ScriptProject *scriptProject, FMA::Project *project)
    : FMA::plugin::MemoryManagerPlugin(project)
    , mScriptProject(scriptProject)
  {}

  //! Returns the plugin type
  FMA::plugin::PluginType getPluginType() const override {
    return FMA::plugin::TYPE_MEMORY_MANAGER;
  }

  //! Returns the name
  const char *getName() const override {
    return "AobaScript Memory Manager";
  }

  //! Return the description
  const char *getDescription() const override {
    return "";
  }

  //! Initialize
  bool initialize() override {
    return true;
  }

  //! Release
  void release() override {}

  //! Creates the adapter
  FMA::plugin::MemoryPluginAdapter *createAdapter() override;

  //! Releases the adapter
  void releaseAdapter(FMA::plugin::MemoryPluginAdapter *adapter) override;

protected:
  //! The script project
  ScriptProject *mScriptProject;

};

}
