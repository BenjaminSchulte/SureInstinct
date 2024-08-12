#pragma once

#include <fma/plugin/Plugin.hpp>
#include <fma/plugin/MemoryPluginAdapter.hpp>
#include <fma/linker/LinkerBlock.hpp>

namespace Aoba {
class ScriptProject;

class ScriptMemoryBlock : public FMA::plugin::MemoryBlock {
public:
  //! Constructor
  ScriptMemoryBlock(ScriptProject *project, Usage usage);

  //! Sets the name hint
  void setNameHint(const std::string &hint) override { mNameHint = hint; }

  //! Returns the name hint
  std::string getNameHint() const override { return mNameHint; }

  //! Returns a reference
  FMA::symbol::SymbolReferencePtr createReference() override;

  //! Marks this block to be used
  void markIsUsed(bool used=true) override;

  //! Writes data
  void write(void *data, uint32_t size) override;

  //! Writes a reference
  void write(const FMA::symbol::ReferencePtr &reference, uint32_t size) override;

  //! Writes an instruction
  void write(FMA::assem::Instruction *instruct) override;

  //! Returns a reference
  FMA::symbol::ReferencePtr reference() override;

  //! Returns a reference
  FMA::symbol::ReferencePtr reference(const FMA::symbol::SymbolReferencePtr &reference) override;

  //! Builds the byte code to the binary code generator scope
  bool buildByteCode(FMA::assem::BinaryCodeGeneratorScope *scope) override;

  //! Returns the location
  FMA::plugin::MemoryLocationPtr location() override;

  //! Returns the script ID
  inline const QString &scriptId() const { return mScriptId; }

protected:
  //! The internal script ID
  QString mScriptId;

  //! The name hint
  std::string mNameHint;

  //! The project
  ScriptProject *mProject;

  //! The linker block
  FMA::linker::LinkerBlock *mLinkerBlock;

  //! Whether the block has been used
  bool mHasBeenUsed = false;
};

}
