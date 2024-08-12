#include <aoba/project/Project.hpp>
#include <aoba/asset/Asset.hpp>
#include <aoba/utils/StringUtils.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
QString AbstractAsset::binaryFileName(const QString &postfix) const {
  return project()->objFile(QString(typeId()) + "_" + StringUtils::escape(id()) + postfix);
}