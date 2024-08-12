#include <aoba/log/Log.hpp>
#include <aoba/variable/RamVariableScope.hpp>
#include <aoba/variable/RamVariable.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
QString RamVariable::uniqueIdentifier() const {
  return mParent->uniqueIdentifier() + "." + mId + "[" + uniqueTypeIdentifier() + "]";
}

// -----------------------------------------------------------------------------
