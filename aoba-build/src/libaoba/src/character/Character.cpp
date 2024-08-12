#include <aoba/character/Character.hpp>
#include <aoba/character/CharacterComponent.hpp>
#include <yaml-cpp/yaml.h>

using namespace Aoba;

// -----------------------------------------------------------------------------
Character::Character(CharacterAssetSet *set, const QString &id, const QDir &dir)
  : Asset(set, id)
  , mPath(dir)
{
  set->add(this);
}

// -----------------------------------------------------------------------------
Character::~Character() {
}

// -----------------------------------------------------------------------------
QString Character::symbolName() const {
  return "__asset_character_" + mId;
}

// -----------------------------------------------------------------------------
bool Character::reload() {
  YAML::Node config;
  try {
    config = YAML::LoadFile(mPath.filePath("character.yml").toStdString().c_str());
  } catch(YAML::BadFile&) {
    return false;
  }

  for (CharacterComponent *component : mAssetSet->components()) {
    if (!component->load(this, config)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool Character::build() {
  FMA::linker::LinkerBlock *block = mAssetSet->assetLinkerObject().createLinkerBlock(symbolName(), mAssetSet->headerRomBank());
  bool buildStaticContent = mAssetSet->shouldBuiltStaticContent();

  for (CharacterComponent *component : mAssetSet->components()) {
    if (buildStaticContent) {
      if (!component->staticBuild(mAssetSet)) {
        return false;
      }
    }

    if (!component->build(this, block)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
QString Character::getFmaCode() const {
  return "";
}

// -----------------------------------------------------------------------------
QStringList Character::getFmaObjectFiles() const {
  return mAssetSet->assetLinkerObject().getFmaObjectFiles();
}

// -----------------------------------------------------------------------------
QString Character::propertiesToFma(const QMap<QString, QVariant> &map) const {
  QMapIterator<QString, QVariant> it(map);

  QStringList members;

  while (it.hasNext()) {
    it.next();

    const QVariant &v(it.value());
    if (v.canConvert<int>()) {
      members << it.key() + ": " + QString::number(v.value<int>());
    } else {
      members << it.key() + ": nil";
    }
  }

  return "{" + members.join(',') + "}";
}