#include <aoba/log/Log.hpp>
#include <QFile>
#include <aoba/asset/AssetLinkerObject.hpp>
#include <aoba/project/Project.hpp>
#include <fma/memory/MemoryLocationList.hpp>
#include <fma/linker/LinkerObjectSerializer.hpp>
#include <fma/linker/LinkerBlock.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
AssetLinkerObject::AssetLinkerObject(const QString &id, Project *project)
  : mAssetTypeId(id)
  , mProject(project)
{}

// -----------------------------------------------------------------------------
QString AssetLinkerObject::postfix() {
  if (mFilePostfix == -1) {
    mFilePostfix = mProject->uniqueNumber();
  }

  return "_" + QString::number(mFilePostfix);
}

// -----------------------------------------------------------------------------
QString AssetLinkerObject::fileName() {
  return mProject->objFile(mAssetTypeId + postfix() + ".fmaobj");
}

// -----------------------------------------------------------------------------
FMA::linker::LinkerBlock *AssetLinkerObject::createLinkerBlock(const QString &id, int bank) {
  FMA::linker::LinkerBlock *block = mLinkerObject.createBlock(); 
  
  if (bank != -1) {
    FMA::memory::MemoryLocationList *frameConfigLocationList = new FMA::memory::MemoryLocationList();
    FMA::memory::MemoryLocationConstraint frameConfigLocation(true);
    frameConfigLocation.bank(bank);
    frameConfigLocationList->add(frameConfigLocation);
    block->setLocation(FMA::plugin::MemoryLocationPtr(frameConfigLocationList));
  }

  block->setNameHint(id.toStdString());
  block->symbol(id.toStdString());

  mBlocks.push_back(block);

  return block;
}

// -----------------------------------------------------------------------------
QStringList AssetLinkerObject::getFmaObjectFiles() {
  if (!mNeedsObjectLinking) {
    return {};
  }

  mNeedsObjectLinking = false;

  for (FMA::linker::LinkerBlock *block : mBlocks) {
    if (block->getSize() == 0) {
      // Aoba::log::debug("Adding data to empty block for " + fileName() + " (" + block->getNameHint().c_str() + ")");
      block->writeNumber(0, 1);
    }
  }

  FMA::linker::LinkerObjectSerializer serializer(&linkerObject());
  if (!serializer.serialize()) {
    Aoba::log::error("Failed to serialize FMA LinkerObject");
    return {};
  }

  QFile objectFile(fileName());
  if (!objectFile.open(QIODevice::WriteOnly)) {
    Aoba::log::error("Failed to open output file");
    return {};
  }

  objectFile.write((const char*)serializer.buffer().getData(), serializer.buffer().getSize());
  return {fileName()};
}