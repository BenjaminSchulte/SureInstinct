#include <QDebug>
#include <aoba/log/Log.hpp>
#include <fma/Log.hpp>
#include <fma/memory/MemoryLocationList.hpp>
#include <fma/linker/LinkerObject.hpp>
#include <fma/linker/LinkerObjectSerializer.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <fma/output/DynamicBuffer.hpp>
#include <aoba/asset/AssetLinkerObject.hpp>
#include <aoba/asset/AssetByteListLinker.hpp>
#include <aoba/sprite/Sprite.hpp>
#include <aoba/sprite/SpriteType.hpp>
#include <aoba/sprite/SpriteAnimationIndex.hpp>
#include <aoba/sprite/SpriteBuilder.hpp>
#include <aoba/sprite/GeneratedSpriteAnimation.hpp>
#include <aoba/image/ImageMapSplitter.hpp>
#include <aoba/image/SnesPaletteImage.hpp>
#include <aoba/image/PaletteImageAccessor.hpp>
#include <aoba/image/ImageAccessor.hpp>
#include <aoba/sprite/SpriteAnimationBuilder.hpp>

using namespace Aoba;

namespace {
  int spriteBuilderNextSymbolId = 0;
}


// -----------------------------------------------------------------------------
SpriteBuilderImage SpriteBuilderImage::append(const SpriteBuilderImage &other) const {
  SpriteBuilderImage result(*this);

  result.imageSymbolName = "";
  result.slotCount += other.slotCount;

  for (SpriteBuilderImageItem newItem : other.items) {
    for (auto &part : newItem.parts) {
      part.index += slotCount * 2;
    }
    result.items.push_back(newItem);
  }

  return result;
}


// -----------------------------------------------------------------------------
SpriteBuilder::SpriteBuilder(Sprite *sprite)
 : mSprite(sprite)
{
}

// -----------------------------------------------------------------------------
SpriteBuilder::~SpriteBuilder() {
  delete mAnimationConfigBlock;
  delete mFrameConfigBlock;
  delete mFrameGroupBlock;
}

// -----------------------------------------------------------------------------
bool SpriteBuilder::build() {
  mSpriteSymbolName = "__asset_sprite_" + mSprite->id() + "_" + QString::number(spriteBuilderNextSymbolId++, 10);
  mSpriteFrameIndexBlock = ".__assetdata_sprite_frame_index_" + mSprite->id() + "_" + QString::number(spriteBuilderNextSymbolId++, 10);
  mAnimationIndexSymbol = ".__assetdata_sprite_animation_index_" + mSprite->id() + "_"  + QString::number(spriteBuilderNextSymbolId++, 10);
  mFrameGroupIndexSymbol = ".__assetdata_sprite_frame_group_" + mSprite->id() + "_"  + QString::number(spriteBuilderNextSymbolId++, 10);

  mAnimationConfigBlock = new FMA::linker::LinkerBlock();
  mFrameConfigBlock = new FMA::linker::LinkerBlock();
  mFrameGroupBlock = new FMA::linker::LinkerBlock();

  if (!buildAnimations()
    || !buildFrameGroups()
    || !buildSpriteConfig()
    || !buildAnimationIndex()
    || !buildFrameGroupIndex()
  ) {
    return false;
  }

  mSpriteConfigBlock = mSprite->assetSet()->assetLinkerObject().createLinkerBlock(mSpriteSymbolName);
  mSpriteConfigBlock->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(mSpriteFrameIndexBlock.toStdString())), 2);
  mSpriteConfigBlock->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(mAnimationIndexSymbol.toStdString())), 2);
  mSpriteConfigBlock->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(mFrameGroupIndexSymbol.toStdString())), 2);
  mSpriteConfigBlock->writeNumber(0, 1); // default palette index
  mSpriteConfigBlock->writeNumber(mMaxSlotsPerFrame, 1); // Maximum slots used

  mSpriteConfigBlock->write(mAnimationConfigBlock);
  mSpriteConfigBlock->write(mFrameConfigBlock);
  mSpriteConfigBlock->write(mFrameGroupBlock);

  delete mAnimationConfigBlock;
  delete mFrameConfigBlock;
  delete mFrameGroupBlock;
  mAnimationConfigBlock = nullptr;
  mFrameConfigBlock = nullptr;
  mFrameGroupBlock = nullptr;

  int assetId = mSprite->assetSet()->assetId(mSprite->id());
  mSprite->assetSet()->recordAddresses()->addSymbol(assetId, mSpriteSymbolName);
  mSprite->assetSet()->recordBanks()->addSymbol(assetId, mSpriteSymbolName, 16);

  return mSuccess;
}

// -----------------------------------------------------------------------------
bool SpriteBuilder::buildFrameGroups() {
  QMapIterator<QString, SpriteFrameGroup> it(mSprite->frameGroups());
  while (it.hasNext()) {
    it.next();

    if (!buildFrameGroup(it.key(), it.value())) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool SpriteBuilder::buildFrameGroupIndex() {
  mFrameGroupBlock->symbol(mFrameGroupIndexSymbol.toStdString());

  QMapIterator<QString, int> it(mSprite->spriteType()->animationIndex()->frameGroups());
  while (it.hasNext()) {
    it.next();

    if (!mFrameGroupSymbolNames.contains(it.key())) {
      mFrameGroupBlock->writeNumber(0, 2);
    } else {
      mFrameGroupBlock->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(mFrameGroupSymbolNames[it.key()].toStdString())), 2);
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool SpriteBuilder::buildFrameGroup(const QString &id, const SpriteFrameGroup &frames) {
  QString groupId = mFrameGroupIndexSymbol + "_group_" + id;
  mFrameGroupBlock->symbol(groupId.toStdString());
  mFrameGroupSymbolNames.insert(id, groupId);
  for (const auto &frame : frames) {
    int index = buildFrame(frame.frameId);
    if (index < 0) {
      return false;
    }

    mFrameGroupBlock->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(mGeneratedFrames[index].frameConfigSymbolName.toStdString())), 2);
  }

  return true;
}

// -----------------------------------------------------------------------------
bool SpriteBuilder::buildAnimations() {
  QMapIterator<QString, GeneratedSpriteAnimation*> it(mSprite->generatedAnimations());
  while (it.hasNext()) {
    it.next();

    if (!buildAnimation(it.value())) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool SpriteBuilder::buildAnimationIndex() {
  mAnimationConfigBlock->symbol(mAnimationIndexSymbol.toStdString());

  QMap<int, int> generatedIds;
  QStringList allIndexAnimations;
  int maxId = -1;
  for (const QString &animation : mSprite->spriteType()->animationIndex()->animations().keys()) {
    for (const QString &direction : mSprite->spriteType()->animationIndex()->directions().keys()) {
      QString id = mSprite->findBestAnimationId(animation, direction);

      GeneratedSpriteAnimation *anim = mSprite->generatedAnimations()[id];
      int expectedId = mSprite->spriteType()->animationIndex()->animationId(direction, animation);

      generatedIds.insert(expectedId, anim->id());
      allIndexAnimations.push_back(anim->name());
      maxId = qMax(maxId, expectedId);
    }
  }

  QMapIterator<QString, GeneratedSpriteAnimation*> it(mSprite->generatedAnimations());
  while (it.hasNext()) {
    it.next();
    
    if (allIndexAnimations.contains(it.key())) {
      continue;
    }

    if (generatedIds.contains(it.value()->id())) {
      Aoba::log::warn("Animation ID is already in use: " + it.key());
      return false;
    }

    generatedIds.insert(it.value()->id(), it.value()->id());
    maxId = qMax(maxId, it.value()->id());
  }

  for (int i=0; i<=maxId; i++) {
    int index = mAnimationToGeneratedAnimation[generatedIds[i]];
    const auto &anim = mGeneratedAnimations[index];
    mAnimationConfigBlock->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(anim.animationScriptSymbolName.toStdString())), 2);
    mAnimationConfigBlock->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(anim.frameReferenceSymbolName.toStdString())), 2);
  }

  return true;
}

// -----------------------------------------------------------------------------
bool SpriteBuilder::buildSpriteConfig() {
  mFrameConfigBlock->symbol(mSpriteFrameIndexBlock.toStdString());

  //int blockSize = mSprite->assetSet()->largeSize();
  //mFrameConfigBlock->writeNumber(blockSize / 8, 1);
  //mFrameConfigBlock->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(mAnimationIndexSymbol.toStdString())), 2);

  for (const auto &frame : mGeneratedFrames) {
    mFrameConfigBlock->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(frame.frameConfigSymbolName.toStdString())), 2);
  }

  return true;
}

// -----------------------------------------------------------------------------
bool SpriteBuilder::buildAnimation(GeneratedSpriteAnimation *target) {
  if (!mSprite->animations().contains(target->name()) || mSprite->animation(target->name()).isEmpty()) {
    Aoba::log::warn("Animation " + target->name() + " does not exist on sprite " + mSprite->id() + " or is empty");
    return false;
  }

  const SpriteAnimationFrames &frames = mSprite->animation(target->name());
  for (const SpriteAnimation &frame : frames) {
    if (!buildAnimationFrame(frame)) {
      return false;
    }
  }
  
  mAnimationToGeneratedAnimation.insert(target->id(), mGeneratedAnimations.count());
  int maxNumSlots = 0;
  for (const SpriteAnimation &frame : frames) {
    if (frame.isCommand()) {
      continue;
    }

    int index = getFrameIndex(frame.frame);
    if (index < 0) {
      Aoba::log::warn("Animation " + target->name() + " could not fetch frame " + frame.frame);
      return false;
    }
    maxNumSlots = qMax(maxNumSlots, mGeneratedFrames[index].parts.count());
  }

  SpriteAnimationBuilder builder(mSprite->assetSet()->project(), maxNumSlots);
  for (const SpriteAnimation &frame : frames) {
    builder.add(frame, getFrameIndex(frame.frame));
  }
  if (!builder.finish()) {
    return false;
  }

  SpriteBuilderAnimation newAnimation;
  newAnimation.animationScriptSymbolName = builder.animationSymbolName(mSprite->spriteType()->animationIndex());
  newAnimation.frameReferenceSymbolName = builder.frameSymbolName(mSprite->spriteType()->animationIndex());
  mGeneratedAnimations.push_back(newAnimation);

  return true;
}

// -----------------------------------------------------------------------------
QString SpriteBuilder::getFrameIndexHash(const QString &index) const {
  return index;
}

// -----------------------------------------------------------------------------
int SpriteBuilder::getFrameIndex(const QString &index) const {
  QString hash = getFrameIndexHash(index);
  if (!mFrameToGeneratedFrame.contains(hash)) {
    return -1;
  }

  return mFrameToGeneratedFrame[hash];
}

// -----------------------------------------------------------------------------
bool SpriteBuilder::buildAnimationFrame(const SpriteAnimation &animFrame) {
  if (animFrame.delay == 0) {
    return true;
  }

  int generatedFrameId = buildFrame(animFrame.frame);
  if (generatedFrameId < 0) {
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
int SpriteBuilder::buildFrame(const QString &frameIndex) {
  int index = getFrameIndex(frameIndex);
  if (index >= 0) {
    return index;
  }

  if (!mSprite->hasFrame(frameIndex)) {
    Aoba::log::warn("Frame with index " + frameIndex + " does not exist");
    return -1;
  }

  const auto &frame = mSprite->frames()[frameIndex];
  int newFrameId = mGeneratedFrames.count();
  mFrameToGeneratedFrame.insert(getFrameIndexHash(frameIndex), newFrameId);

  SpriteBuilderFrame newFrame;
  newFrame.frameConfigSymbolName = ".__assetdata_sprite_" + mSprite->id() + "_" + frameIndex + "_config" + QString::number(spriteBuilderNextSymbolId++, 10);
  if (!buildFrameImages(frame, newFrame)) {
    Aoba::log::warn("Failed for frame " + frameIndex);
    return false;
  }

  mFrameConfigBlock->symbol(newFrame.frameConfigSymbolName.toStdString());
  mFrameConfigBlock->writeNumber(newFrame.parts.size() | 0x80, 1);

  uint8_t staticImageSizePerRow = (newFrame.staticImageSlotCount * newFrame.slotSize / 8 * 2);
  uint8_t sizePerRow = (newFrame.imageSlotCount * newFrame.slotSize / 8 * 2);
  if (sizePerRow >= 128 || staticImageSizePerRow >= 128) {
    Aoba::log::warn("Too many slot used in sprite");
    return false;
  }

  if (newFrame.hasStaticImage()) {
    mFrameConfigBlock->writeNumber(staticImageSizePerRow | 0x80, 1);
    mFrameConfigBlock->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(newFrame.frameStaticImageSymbolName.toStdString())), 3);
  }

  mFrameConfigBlock->writeNumber(sizePerRow, 1);
  mFrameConfigBlock->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(newFrame.frameImageSymbolName.toStdString())), 3);

  int count = newFrame.parts.size();
  int numFlagsLeft = 0;
  for (int i=0; i<count; i++) {
    const SpriteBuilderFrameConfig &slot = newFrame.parts[i];

    if (numFlagsLeft-- == 0) {
      uint8_t flagByte = 0;
      for (int j=0; j<8 && i+j<count; j++) {
        flagByte |= (newFrame.parts[i+j].priority ? 1 : 0) << j;
      }
      mFrameConfigBlock->writeNumber(flagByte, 1);
      numFlagsLeft = 7;
    }

    mFrameConfigBlock->writeNumber(slot.x, 1);
    mFrameConfigBlock->writeNumber(slot.y, 1);
    mFrameConfigBlock->writeNumber(slot.index, 1);
  }

  mGeneratedFrames.push_back(newFrame);
  return newFrameId;
}

// -----------------------------------------------------------------------------
QVector<int> SpriteBuilder::gatherFrameImageIds(const SpriteFrame &frame) {
  QVector<int> imageIds;

  if (!frame.sharedImage.isEmpty()) {
    if (!mSprite->imageGroups().contains(frame.sharedImage)) {
      Aoba::log::warn("Image group " + frame.sharedImage + " is undefined");
      mSuccess = false;
      return imageIds;
    }
    imageIds = mSprite->imageGroups()[frame.sharedImage];
  } else {
    for (const auto &image : frame.images) {
      imageIds.push_back(image.image);
    }
  }

  return imageIds;
}

// -----------------------------------------------------------------------------
QVector<int> SpriteBuilder::dynamicImageIds(const QVector<int> &imageIds, const QVector<int> &staticImageIds) const {
  QVector<int> result;

  for (int id : imageIds) {
    if (!staticImageIds.contains(id)) {
      result.push_back(id);
    }
  }

  return result;
}

// -----------------------------------------------------------------------------
bool SpriteBuilder::buildFrameImages(const SpriteFrame &frame, SpriteBuilderFrame &newFrame) {
  if (frame.staticImage.isEmpty()) {
    return buildFrameImagesNoStatic(frame, newFrame);
  } else {
    return buildFrameImagesWithStatic(frame, newFrame);
  }
}

// -----------------------------------------------------------------------------
bool SpriteBuilder::buildFrameImageParts(SpriteBuilderFrame &newFrame, const QVector<SpriteFrameImage> &images, const SpriteBuilderImage &image) {
  QMap<int, int> imageIdToImageIndex;
  int imageIndex = 0;
  for (const auto &imageItemInfo : image.items) {
    imageIdToImageIndex.insert(imageItemInfo.image, imageIndex++);
  }

  for (const auto &frameImage : images) {
    if (!imageIdToImageIndex.contains(frameImage.image)) {
      Aoba::log::warn("Unable to locate image " + QString::number(frameImage.image));
      return false;
    }

    const auto &imageItemInfo = image.items[imageIdToImageIndex[frameImage.image]];
    for (const auto &part : imageItemInfo.parts) {
      newFrame.parts.push_back(SpriteBuilderFrameConfig(
        part.index,
        part.x + frameImage.x,
        part.y + frameImage.y,
        frameImage.priority
      ));
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool SpriteBuilder::buildFrameImagesWithStatic(const SpriteFrame &frame, SpriteBuilderFrame &newFrame) {
  if (!mSprite->imageGroups().contains(frame.staticImage)) {
    Aoba::log::warn("Image group " + frame.staticImage + " is undefined");
    return false;
  }

  QVector<int> staticImageIds = mSprite->imageGroups()[frame.staticImage];
  QVector<int> allImageIds = gatherFrameImageIds(frame);
  QVector<int> imageIds = dynamicImageIds(allImageIds, staticImageIds);

  const SpriteBuilderImage &staticImage(generateImage(staticImageIds));
  const SpriteBuilderImage &dynamicImage(generateImage(imageIds));
  newFrame.frameStaticImageSymbolName = staticImage.imageSymbolName;
  newFrame.frameImageSymbolName = dynamicImage.imageSymbolName;
  newFrame.slotSize = dynamicImage.slotSize;
  newFrame.imageSlotCount = dynamicImage.slotCount;
  newFrame.staticImageSlotCount = staticImage.slotCount;
  mMaxSlotsPerFrame = qMax<uint8_t>(mMaxSlotsPerFrame, newFrame.slotCount());

  SpriteBuilderImage image(staticImage.append(dynamicImage));

  return buildFrameImageParts(newFrame, frame.images, image);
}

// -----------------------------------------------------------------------------
bool SpriteBuilder::buildFrameImagesNoStatic(const SpriteFrame &frame, SpriteBuilderFrame &newFrame) {
  QVector<int> imageIds = gatherFrameImageIds(frame);
  if (imageIds.isEmpty()) {
    mSuccess = false;
    return false;
  }

  const SpriteBuilderImage &image(generateImage(imageIds));
  newFrame.frameImageSymbolName = image.imageSymbolName;
  newFrame.slotSize = image.slotSize;
  newFrame.imageSlotCount = image.slotCount;
  mMaxSlotsPerFrame = qMax<uint8_t>(mMaxSlotsPerFrame, image.slotCount);

  return buildFrameImageParts(newFrame, frame.images, image);
}

// -----------------------------------------------------------------------------
const SpriteBuilderImage &SpriteBuilder::generateImage(const QVector<int> &ids) {
  QStringList hashImages;
  for (const int id : ids) {
    hashImages.push_back(QString::number(id));
  }
  QString hash = hashImages.join("_");

  if (mGeneratedImages.contains(hash)) {
    return mGeneratedImages[hash];
  }
  
  SpriteBuilderImage imageInfo;
  imageInfo.imageSymbolName = ".__assetdata_sprite_" + mSprite->id() + "_" + hash;
  imageInfo.slotCount = 0;
  imageInfo.slotSize = mSprite->assetSet()->largeSize();

  int frameIndex = 0;
  for (const int id : ids) {
    const auto &image = mSprite->images()[id];
    ImageMapSplitter splitter(image.image, imageInfo.slotSize, imageInfo.slotSize);

    SpriteBuilderImageItem imageItemInfo;
    imageItemInfo.image = id;
    imageItemInfo.sections = splitter.split();

    for (const ImageMapSplitterSection &section : imageItemInfo.sections) {
      imageItemInfo.parts.push_back(SpriteBuilderFrameConfig(
        frameIndex,
        section.x - image.pivotX,
        section.y - image.pivotY
      ));
      frameIndex += imageInfo.slotSize / 8;
    }

    imageInfo.slotCount += imageItemInfo.sections.count();
    imageInfo.items.push_back(imageItemInfo);
  }

  FMA::linker::LinkerBlock *imageRowBlock = mSprite->assetSet()->assetLinkerObject().createLinkerBlock(imageInfo.imageSymbolName);
  imageRowBlock->setNameHint(imageInfo.imageSymbolName.toStdString());
  imageRowBlock->symbol(imageInfo.imageSymbolName.toStdString());

  for (int y=0; y<imageInfo.slotSize; y+=8) {
    ImageList spriteFrameRow;
  
    for (const auto &imageItemInfo : imageInfo.items) {
      for (const ImageMapSplitterSection &section : imageItemInfo.sections) {
        for (int x=0; x<imageInfo.slotSize; x+=8) {
          spriteFrameRow.push_back(section.image->resizeTo(imageInfo.slotSize, imageInfo.slotSize)->slice(QRect(x, y, 8, 8)));
        }
      }
    }

    PaletteImagePtr resultImage(spriteFrameRow.join()->toPaletteImage());
    if (!resultImage) {
      Aoba::log::warn("Failed to generate PaletteImage from " + QString::number(spriteFrameRow.count()) + " images. Requested to generate sprite for " + QString::number(ids.count()) + " IDs");
      mSuccess = false;
      break;
    }

    SnesPaletteImage snesImage(resultImage);
    QByteArray compiledSnesImage = snesImage.compile(8, 8, 0);
    imageRowBlock->write(compiledSnesImage.data(), compiledSnesImage.size());
  }

  mGeneratedImages.insert(hash, imageInfo);
  return mGeneratedImages[hash];
}
