#include <aoba/log/Log.hpp>
#include <aoba/utils/AobaPath.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool Aoba::createWritablePath(const QDir &p) {
  return p.mkpath(".");
}
