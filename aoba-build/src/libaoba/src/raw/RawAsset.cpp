#include <QDebug>
#include <aoba/log/Log.hpp>
#include <QFile>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/CalculatedNumber.hpp>
#include <fma/symbol/ConstantNumber.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <aoba/project/Project.hpp>
#include <aoba/raw/RawAsset.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
RawAssetSet::RawAssetSet(Project *project, int bank)
  : LinkedAssetSet<RawAsset>("raw", project)
  , mBank(bank)
{
}


// -----------------------------------------------------------------------------
RawAsset::RawAsset(RawAssetSet *set, const QString &id, const QString &dir)
  : Asset(set, id)
  , mFile(dir)
{
  set->add(this);
}

// -----------------------------------------------------------------------------
QString RawAsset::symbolName() const {
  return "__asset_binary_raw_" + mId;
}

// -----------------------------------------------------------------------------
bool RawAsset::build() {
  QFile file(mFile);
  if (!file.open(QIODevice::ReadOnly)) {
    Aoba::log::error("Unable to open file " + mFile);
    return false;
  }

  QByteArray buffer = file.readAll();

  FMA::linker::LinkerBlock *block = mAssetSet->assetLinkerObject().createLinkerBlock(symbolName(), mAssetSet->bank());
  block->write(buffer.data(), buffer.size());

  return true;
}

// -----------------------------------------------------------------------------
QStringList RawAsset::getFmaObjectFiles() const {
  return mAssetSet->assetLinkerObject().getFmaObjectFiles();
}

