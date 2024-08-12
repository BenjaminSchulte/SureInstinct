#pragma once

#include <QMap>
#include <aoba/image/ImageAccessor.hpp>
#include <aoba/palette/SnesPalette.hpp>

namespace FMA {
namespace linker {
class LinkerObject;
class LinkerBlock;
}
}

namespace Aoba {
class Sprite;
class GeneratedSpriteAnimation;
struct SpriteAnimation;
struct ImageMapSplitterSection;

struct SpriteBuilderFrameConfig {
  SpriteBuilderFrameConfig() {}
  SpriteBuilderFrameConfig(int index, int x, int y, int8_t priority=0) : index(index), x(x), y(y), priority(priority) {}

  int index;
  int x;
  int y;
  int priority;
};

struct SpriteBuilderAnimation {
  QString animationScriptSymbolName;
  QString frameReferenceSymbolName;
};

struct SpriteBuilderFrame {
  QString frameConfigSymbolName;
  QString frameImageSymbolName;
  QString frameStaticImageSymbolName;
  int slotSize;

  int staticImageSlotCount = 0;
  int imageSlotCount = 0;

  inline bool hasStaticImage() const { return staticImageSlotCount > 0; }
  inline int slotCount() const { return imageSlotCount + staticImageSlotCount; }

  QVector<SpriteBuilderFrameConfig> parts;
};

struct SpriteBuilderImageItem {
  int image;
  QVector<SpriteBuilderFrameConfig> parts;
  QVector<ImageMapSplitterSection> sections;
};

struct SpriteBuilderImage {
  QString imageSymbolName;
  int slotSize;
  int slotCount;
  QVector<SpriteBuilderImageItem> items;

  SpriteBuilderImage append(const SpriteBuilderImage &other) const;
};

class SpriteBuilder {
public:
  //! Constructor
  SpriteBuilder(Sprite *sprite);

  //! Deconstructor
  ~SpriteBuilder();

  //! Loads the Sprite
  bool build();

private:
  //! Builds the frame index
  bool buildAnimationIndex();

  //! Builds the frame index
  bool buildFrameGroupIndex();

  //! Builds the frame index
  bool buildSpriteConfig();

  //! Builds an animation
  bool buildFrameGroups();

  //! Builds an animation
  bool buildAnimations();

  //! Builds an animation
  bool buildAnimation(GeneratedSpriteAnimation *);

  //! Builds a single frame
  bool buildAnimationFrame(const SpriteAnimation &frame);

  //! Builds an animation
  bool buildFrameGroup(const QString &, const SpriteFrameGroup &frame);

  //! Builds a single frame
  int buildFrame(const QString &index);

  //! Builds a single frame
  bool buildFrameImages(const SpriteFrame &frame, SpriteBuilderFrame &newFrame);

  //! Builds a single frame
  bool buildFrameImagesNoStatic(const SpriteFrame &frame, SpriteBuilderFrame &newFrame);

  //! Builds a single frame
  bool buildFrameImagesWithStatic(const SpriteFrame &frame, SpriteBuilderFrame &newFrame);

  //! Returns image IDs which are not part of the static images
  bool buildFrameImageParts(SpriteBuilderFrame &newFrame, const QVector<SpriteFrameImage> &images, const SpriteBuilderImage &index);

  //! Returns image IDs which are not part of the static images
  QVector<int> dynamicImageIds(const QVector<int> &imageIds, const QVector<int> &staticImageIds) const;

  //! Collects all image IDs of the frames
  QVector<int> gatherFrameImageIds(const SpriteFrame &frame);

  //! Generates images
  const SpriteBuilderImage &generateImage(const QVector<int> &);

  //! Returns the viewbox
  int findViewboxIndex();

  //! Returns the frame index or -1
  int getFrameIndex(const QString &index) const;

  //! Returns the index hash
  QString getFrameIndexHash(const QString &index) const;

  // The frame config block
  FMA::linker::LinkerBlock *mSpriteConfigBlock;

  // The frame config block
  FMA::linker::LinkerBlock *mAnimationConfigBlock = nullptr;

  // The frame config block
  FMA::linker::LinkerBlock *mFrameConfigBlock = nullptr;

  // The frame config block
  FMA::linker::LinkerBlock *mFrameGroupBlock = nullptr;

  // The name of the sprite index symbol
  QString mSpriteSymbolName;

  // The name of the sprite index symbol
  QString mSpriteFrameIndexBlock;

  // The name of the animation index block
  QString mAnimationIndexSymbol;

  // The frame group index
  QString mFrameGroupIndexSymbol;

  // The Sprite
  Sprite *mSprite;

  // List of all frames
  QMap<int, int> mAnimationToGeneratedAnimation;

  // List of all frames
  QMap<QString, int> mFrameToGeneratedFrame;

  //! List of frame group items
  QMap<QString, QString> mFrameGroupSymbolNames;

  //! List of all images
  QMap<QString, SpriteBuilderImage> mGeneratedImages;

  // List of all frames
  QVector<SpriteBuilderAnimation> mGeneratedAnimations;

  // List of all frames
  QVector<SpriteBuilderFrame> mGeneratedFrames;

  //! Maximum number of slots used by one frame
  uint8_t mMaxSlotsPerFrame = 0;

  //! Whether the builder succeeded
  bool mSuccess = true;
};

}

