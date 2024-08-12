#include <aoba/log/Log.hpp>
#include <QFile>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <fma/output/DynamicBuffer.hpp>
#include <aoba/project/Project.hpp>
#include <aoba/song/SongBuilder.hpp>
#include <aoba/sfx/SfxLibrary.hpp>
#include <aoba/sfx/Sfx.hpp>
#include <aoba/yaml/YamlTools.hpp>
#include <fma/output/DynamicBuffer.hpp>
#include <yaml-cpp/yaml.h>

using namespace Aoba;

// -----------------------------------------------------------------------------
SfxLibraryAssetSet::SfxLibraryAssetSet(Project *project, SfxAssetSet *sfx)
  : LinkedAssetSet<SfxLibrary>("sfx_library", project)
  , mSfxDatabase(sfx)
{
}

// -----------------------------------------------------------------------------
SfxLibraryAssetSet::~SfxLibraryAssetSet() {
}


// -----------------------------------------------------------------------------
SfxLibrary::SfxLibrary(SfxLibraryAssetSet *set, const QString &id, const QString &file)
  : Asset(set, id)
  , mFile(file)
{
  set->add(this);
}

// -----------------------------------------------------------------------------
SfxLibrary::~SfxLibrary() {
}

// -----------------------------------------------------------------------------
QString SfxLibrary::symbolName() const {
  return "__asset_sfxlib_" + mId;
}

// -----------------------------------------------------------------------------
bool SfxLibrary::reload() {
  YAML::Node config;
  try {
    config = YAML::LoadFile(mFile.toStdString().c_str());
  } catch(YAML::BadFile&) {
    Aoba::log::error("Unable to parse YML file: " + mFile);
    return false;
  }

  mParentSfxListId = Yaml::asString(config["extends"], "");

  for (const auto &node : config["sfx"]) {
    mSfxList.push_back(node.as<std::string>().c_str());
  }

  return true;
}

// -----------------------------------------------------------------------------
SfxLibrary *SfxLibrary::getParentSfxLibrary() const {
  if (mParentSfxListId.isEmpty()) {
    return nullptr;
  }

  return mAssetSet->get(mParentSfxListId);
}

// -----------------------------------------------------------------------------
int SfxLibrary::sfxCount() const {
  SfxLibrary *parent = getParentSfxLibrary();
  
  return mSfxList.count() + (parent ? parent->sfxCount() : 0);
}

// -----------------------------------------------------------------------------
int SfxLibrary::sfxId(const QString &sfx) const {
  SfxLibrary *parent = getParentSfxLibrary();
  int base = 0;

  if (parent) {
    int record = parent->sfxId(sfx);
    if (record >= 0) {
      return record;
    }
    base = parent->sfxCount();
  }

  int record = mSfxList.indexOf(sfx);
  if (record == -1) {
    return -1;
  }

  return record + base;
}

// -----------------------------------------------------------------------------
QVector<QString> SfxLibrary::sfxList() const {
  SfxLibrary *parent = getParentSfxLibrary();
  QVector<QString> list = parent ? parent->sfxList() : QVector<QString>();
  list.append(mSfxList);
  return list;
}

// -----------------------------------------------------------------------------
QVector<Sfx*> SfxLibrary::sfxAssetList(bool &success) const {
  QVector<Sfx*> list;
  success = true;

  for (const QString &item : sfxList()) {
    Sfx *sfx = mAssetSet->sfxDatabase()->get(item);
    if (!sfx) {
      Aoba::log::error("Unable to find SFX " + item);
      success = false;
      return list;
    }

    list.push_back(sfx);
  }

  return list;
}

// -----------------------------------------------------------------------------
QVector<int> SfxLibrary::sfxInstrumentList(const QVector<Sfx*> &sfxList) {
  QVector<int> instruments;
  QMap<int, bool> instrumentMap;

  for (Sfx *sfx : sfxList) {
    for (int id : sfx->songBuilder()->instruments()) {
      if (!instrumentMap[id]) {
        instrumentMap.insert(id, true);
        instruments.push_back(id);
      }
    }
  }

  return instruments;
}

// -----------------------------------------------------------------------------
bool SfxLibrary::build() {
  bool ok;

  QVector<Sfx*> allSfx = sfxAssetList(ok);
  if (!ok) {
    return false;
  }

  FMA::output::DynamicBuffer buffer;
  SongBuilder::writeInstrumentList(&buffer, sfxInstrumentList(allSfx));

  FMA::linker::LinkerBlock *block = mAssetSet->assetLinkerObject().createLinkerBlock(symbolName());
  block->write(buffer.getData(), buffer.getSize());

  FMA::output::DynamicBuffer header;
  FMA::output::DynamicBuffer content;

  // 2 channels with uint16 address pointers for content starting at 0x4000
  const int numChannels = 2;
  int sfxContentStart = 0x4000 + allSfx.length() * numChannels * 2;
  for (Sfx *sfx : allSfx) {
    for (int channel=0; channel<numChannels; channel++) {
      if (sfx->songBuilder()->isChannelEmpty(channel)) {
        header.writeNumber(0, 2);
      } else {
        header.writeNumber(sfxContentStart + content.getSize(), 2);

        sfx->songBuilder()->writeChannelTo(channel, &content);
      }
    }
  }

  block->writeNumber((header.getSize() + content.getSize() + 2) / 3, 2);

  block->write(header.getData(), header.getSize());
  block->write(content.getData(), content.getSize());

  return true;
}

// -----------------------------------------------------------------------------
QStringList SfxLibrary::getFmaObjectFiles() const {
  return mAssetSet->assetLinkerObject().getFmaObjectFiles();
}

// -----------------------------------------------------------------------------
QString SfxLibrary::getFmaCode() const {
  return "";
}
