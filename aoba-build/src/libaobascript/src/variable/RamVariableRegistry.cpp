#include <aoba/log/Log.hpp>
#include <aoba/variable/RamVariableRegistry.hpp>
#include <aoba/variable/RamVariableScope.hpp>
#include <aoba/variable/RamVariableSchema.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
RamVariableRegistry::RamVariableRegistry()
  : mRoot(new RamVariableScope(nullptr, "__aobascriptvariable_", false, false, false))
  , mSchema(new RamVariableSchema(0x2000, 3, 5))
{}

// -----------------------------------------------------------------------------
RamVariableRegistry::~RamVariableRegistry() {
  delete mSchema;
  delete mRoot;
}

