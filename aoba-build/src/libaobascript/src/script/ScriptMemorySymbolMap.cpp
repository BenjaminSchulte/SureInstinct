#include <aoba/log/Log.hpp>
#include <aoba/script/ScriptMemorySymbolMap.hpp>
#include <fma/symbol/SymbolReference.hpp>

using namespace Aoba;
using namespace FMA::plugin;
using namespace FMA::symbol;

namespace {
  std::vector<MemorySymbolMapCommand> emptyCommandMap;
  std::vector<MemorySymbolMapBreakpoint> emptyBreakpointMap;
}

// ----------------------------------------------------------------------------
SymbolReferencePtr ScriptMemorySymbolMap::createReference() {
  QString refName = ".custom_script_reference_" + QString::number(mReferenceCounter++);
  return SymbolReferencePtr(new SymbolReference(refName.toStdString()));
}

// ----------------------------------------------------------------------------
SymbolReferencePtr ScriptMemorySymbolMap::createReference(const std::string &hint) {
  (void)hint;
  return createReference();
}

// ----------------------------------------------------------------------------
ReferencePtr ScriptMemorySymbolMap::createCommand(const std::string &command) {
  (void)command;
  Aoba::log::debug("ScriptMemoryAdapter::createCommand()");
  return ReferencePtr();
}

// ----------------------------------------------------------------------------
void ScriptMemorySymbolMap::addEmulatorBreakpoint(const SymbolReferencePtr &, bool, const std::string &) {
}

// ----------------------------------------------------------------------------
bool ScriptMemorySymbolMap::hasResolved(const std::string &) const {
  Aoba::log::debug("ScriptMemoryAdapterhasResolved:hasResolved()");
  return false;
}

// ----------------------------------------------------------------------------
uint64_t ScriptMemorySymbolMap::getResolved(const std::string &) const {
  Aoba::log::debug("ScriptMemoryAdapter::getResolved()");
  return 0;
}

// ----------------------------------------------------------------------------
std::vector<std::string> ScriptMemorySymbolMap::getSymbolNames() const {
  Aoba::log::debug("ScriptMemoryAdapter::getSymbolNames()");
  return {};
}

// ----------------------------------------------------------------------------
const std::vector<MemorySymbolMapCommand> &ScriptMemorySymbolMap::getCommands() const {
  Aoba::log::debug("ScriptMemoryAdapter::getCommands()");
  return emptyCommandMap;
}

// ----------------------------------------------------------------------------
const std::vector<MemorySymbolMapBreakpoint> &ScriptMemorySymbolMap::getBreakpoints() const {
  Aoba::log::debug("ScriptMemoryAdapter::getBreakpoints()");
  return emptyBreakpointMap;
}

