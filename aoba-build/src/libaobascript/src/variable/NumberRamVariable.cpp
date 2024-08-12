#include <aoba/log/Log.hpp>
#include <aoba/variable/NumberRamVariable.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
QString NumberRamVariable::uniqueTypeIdentifier() const {
  return "uint" + QString::number(sizeInBits());
}

// -----------------------------------------------------------------------------
int NumberRamVariable::sizeInBits() const {
  int n = mMaximum - mMinimum;

  unsigned int count = 0; 
  do {
    count++; 
    n >>= 1; 
  } while(n);

  return count; 
}
