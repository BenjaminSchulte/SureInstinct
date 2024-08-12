#pragma once

#include <fma/plugin/Plugin.hpp>
#include <fma/plugin/MemoryPluginAdapter.hpp>

namespace Aoba {

class ScriptMemorySymbolMap : public FMA::plugin::MemorySymbolMap { 
public:
  FMA::symbol::SymbolReferencePtr createReference() override;
  FMA::symbol::SymbolReferencePtr createReference(const std::string &hint) override;

  FMA::symbol::ReferencePtr createCommand(const std::string &command) override;
  void addEmulatorBreakpoint(const FMA::symbol::SymbolReferencePtr &, bool notifyOnly, const std::string &comment) override;

  bool hasResolved(const std::string &) const override;
  uint64_t getResolved(const std::string &) const override;

  std::vector<std::string> getSymbolNames() const override;
  const std::vector<FMA::plugin::MemorySymbolMapCommand> &getCommands() const override;
  const std::vector<FMA::plugin::MemorySymbolMapBreakpoint> &getBreakpoints() const override;

private:
  //! The next reference counter
  int mReferenceCounter = 0;
};

}