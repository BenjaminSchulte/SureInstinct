#include <QDebug>
#include <QFile>
#include <aoba/palette/Palette.hpp>
#include <aoba/palette/PaletteGroup.hpp>
#include <aoba/sprite/Sprite.hpp>
#include <aoba/sprite/SpriteLoader.hpp>
#include <aoba/sprite/SpriteAnimationIndex.hpp>
#include <aoba/sprite/SpriteBuilder.hpp>
#include <aoba/sprite/SpriteType.hpp>
#include <aoba/sprite/SpriteWriter.hpp>
#include <aoba/sprite/GeneratedSpriteAnimation.hpp>
#include <aoba/project/Project.hpp>
#include <yaml-cpp/yaml.h>

using namespace Aoba;

// -----------------------------------------------------------------------------
Sprite::Sprite(SpriteAssetSet *set, const QString &id, const QDir &dir, bool includeDefaultPalette)
  : Asset(set, id)
  , mPath(dir)
  , mIncludeDefaultPalette(includeDefaultPalette)
{
  set->add(this);
}

// -----------------------------------------------------------------------------
Sprite::~Sprite() {
  for (GeneratedSpriteAnimation *anim : mGeneratedAnimations) {
    delete anim;
  }
  for (PaletteGroup *group : mPaletteGroups) {
    delete group;
  }
}

// -----------------------------------------------------------------------------
bool Sprite::reload() {
  YAML::Node config;
  try {
    config = YAML::LoadFile(mPath.filePath("sprite.yml").toStdString().c_str());
  } catch(YAML::BadFile&) {
    return false;
  }

  QString typeId = config["type"].as<std::string>().c_str();
  mType = dynamic_cast<SpriteTypeAssetSet*>(mAssetSet->project()->assetSet(typeId));
  if (mType == nullptr) {
    std::cerr << "Invalid sprite type";
    return false;
  }

  SpriteLoader loader(this);
  if (!loader.load(config)) {
    return false;
  }

  for (const QString &direction : mType->animationIndex()->directions().keys()) {
    for (const QString &animation : mType->animationIndex()->animations().keys()) {
      QString id = animation + "_" + direction;
      if (mAnimations.contains(id)) {
        generatedAnimation(id);
      }
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool Sprite::save() {
  YAML::Emitter root;
  root << YAML::BeginMap;
  root << YAML::Key << "type" << YAML::Value << mType->typeId().toStdString();

  SpriteWriter writer(this);
  if (!writer.save(root)) {
    return false;
  }

  root << YAML::EndMap;

  QFile outFile(path().filePath("sprite.yml"));
  if (!outFile.open(QIODevice::WriteOnly)) {
    return false;
  }

  outFile.write(root.c_str(), root.size());

  return true;
}

// -----------------------------------------------------------------------------
void Sprite::overrideData(const QVector<SpriteImage> &images, const QMap<QString, SpriteImageGroup> &groups, const QMap<QString, SpriteFrame> &frames, const QMap<QString, SpriteAnimationFrames> &animations) {
  mImages = images;
  mImageGroups = groups;
  mFrames = frames;
  mAnimations = animations;

  mGeneratedAnimations.clear();

  for (const QString &direction : mType->animationIndex()->directions().keys()) {
    for (const QString &animation : mType->animationIndex()->animations().keys()) {
      QString id = animation + "_" + direction;
      if (mAnimations.contains(id)) {
        generatedAnimation(id);
      }
    }
  }
}

// -----------------------------------------------------------------------------
bool Sprite::build() {
  SpriteBuilder builder(this);
  if (!builder.build()) {
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
QString Sprite::findBestAnimationId(const QString &animation, const QString &direction) const {
  QStringList animations = {animation, "stand"};
  QStringList directions = spriteType()->animationIndex()->directions().keys();
  directions.push_front(direction);

  for (const QString &testAnim : animations) {
    for (const QString &testDirection: directions) {
      QString id = testAnim + "_" + testDirection;

      if (generatedAnimations().contains(id)) {
        return id;
      }
    }
  }

  return generatedAnimations().keys().front();
}

// -----------------------------------------------------------------------------
QStringList Sprite::getFmaObjectFiles() const {
  return mAssetSet->assetLinkerObject().getFmaObjectFiles();
}

// -----------------------------------------------------------------------------
QString Sprite::getFmaCode() const {
  return mType->animationIndex()->buildFmaCodeOnce();
}

// -----------------------------------------------------------------------------
PaletteGroup *Sprite::mainPaletteGroup() {
  if (!mMainPaletteGroup) {
    mMainPaletteGroup = createPaletteGroup("main");
  }

  return mMainPaletteGroup;
}

// -----------------------------------------------------------------------------
int Sprite::addPalette(const QString &id) {
  if (mPaletteToId.contains(id)) {
    return mPaletteToId[id];
  }

  int newId = mPaletteToId.count();
  mPaletteToId.insert(id, newId);

  if (mIncludeDefaultPalette) {
    if (!mainPaletteGroup()->add(newId, mPath.filePath(id), 0)->load()) {
      return -1;
    }
  }

  return mPaletteToId[id];
}

// -----------------------------------------------------------------------------
SpriteAnimationFrames &Sprite::animation(const QString &id) {
  if (!mAnimations.contains(id)) {
    mAnimations.insert(id, SpriteAnimationFrames());
  }
  
  return mAnimations[id];
}

// -----------------------------------------------------------------------------
GeneratedSpriteAnimation *Sprite::generatedAnimation(const QString &fullId) {
  QStringList parts = fullId.split('_');
  if (parts.size() < 2) {
    QString anyValidDirection;
    for (const auto &direction : mType->animationIndex()->directions().keys()) {
      QString key = parts[0] + "_" + direction;
      if (!mAnimations.contains(key)) {
        continue;
      }

      if (direction == "down" || anyValidDirection.isEmpty()) {
        anyValidDirection = direction;
      }
      generatedAnimation(key);
    }

    if (anyValidDirection.isEmpty()) {
      anyValidDirection = "down";
    }

    return generatedAnimation(parts[0] + "_" + anyValidDirection);
  }

  QString animationId = parts[0];
  QString directionId = parts[1];

  if (!mGeneratedAnimations.contains(fullId)) {
    int animationIndex = mType->animationIndex()->animationId(animationId);
    int directionIndex = mType->animationIndex()->directionId(directionId);
    int index = mType->animationIndex()->animationId(directionId, animationId);
    
    if (index == -1) {
      if (!mCustomAnimations.contains(animationId)) {
        mCustomAnimations.push_back(animationId);
      }

      if (directionIndex == -1) {
        directionIndex = 0;
      }

      animationIndex = mCustomAnimations.indexOf(animationId) + mType->animationIndex()->numAnimations();
      index = animationIndex * mType->animationIndex()->numDirections() + directionIndex;
    }
    mGeneratedAnimations.insert(fullId, new GeneratedSpriteAnimation(index, fullId, animationIndex, animationId, directionId));
  }

qDebug() << fullId << mGeneratedAnimations[fullId]->id() << mGeneratedAnimations[fullId]->animationId();
  return mGeneratedAnimations[fullId];
}

// -----------------------------------------------------------------------------
PaletteGroup *Sprite::createPaletteGroup(const QString &id) {
  PaletteGroup *group = new PaletteGroup(mAssetSet->palettes(), id);
  mPaletteGroups.push_back(group);
  return group;
}
