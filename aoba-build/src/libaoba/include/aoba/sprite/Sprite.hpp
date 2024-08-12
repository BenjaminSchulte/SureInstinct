#pragma once

#include <QRect>
#include <QMap>
#include "SpriteConfig.hpp"
#include <aoba/image/ImageAccessor.hpp>

namespace Aoba {

class PaletteGroup;
class GeneratedSpriteAnimation;
class SpriteTypeAssetSet;

struct SpriteImage {
  ImageAccessorPtr image;
  int pivotX;
  int pivotY;
  int paletteIndex; // The palette in the mPaletteToId map
  int subPaletteIndex; // The palette row within the palette file
};

struct SpriteFrameImage {
  int image;
  int8_t priority=0;
  int x=0;
  int y=0;
};

struct SpriteFrame {
  QString sharedImage;
  QString staticImage;
  QVector<SpriteFrameImage> images;
};

struct SpriteFrameGroupFrame {
  QString frameId;
};

enum class SpriteAnimationCommand {
  NONE,
  LABEL,
  GOTO,
  MOVE,
  LOOP,
  BEGIN_LOOP,
  END,
  DESTROY_SELF,
  USER_COMMAND
};

typedef QVector<int> SpriteImageGroup;

typedef QVector<SpriteFrameGroupFrame> SpriteFrameGroup;

struct SpriteAnimation {
  SpriteAnimation() {}

  SpriteAnimation(SpriteAnimationCommand command, int parameter=0)
    : command(command), commandParameter(parameter), frame(-1), delay(0) {}

  SpriteAnimation(SpriteAnimationCommand command, const QString &parameter)
    : command(command), commandStringParameter(parameter), frame(-1), delay(0) {}

  SpriteAnimation(const QString &frame, int x, int y, bool flipX, bool flipY, int delay)
    : frame(frame), x(x), y(y), flipX(flipX), flipY(flipY), delay(delay) {}

  inline bool isCommand() const { return command != SpriteAnimationCommand::NONE; }
  inline bool isLoopCommand() const { return command == SpriteAnimationCommand::LOOP; }

  SpriteAnimationCommand command = SpriteAnimationCommand::NONE;
  int commandParameter;
  QString commandStringParameter;
  QString frame;
  int x = 0;
  int y = 0;
  bool flipX = false;
  bool flipY = false;
  int delay = 0;
};

typedef QVector<SpriteAnimation> SpriteAnimationFrames;

class Sprite : public Asset<SpriteAssetSet> {
public:
  //! Constructor
  Sprite(SpriteAssetSet *set, const QString &id, const QDir &path, bool includeDefaultPalette);

  //! Deconstructor
  ~Sprite();

  //! Saves the asset
  bool save() override;

  //! Loads the asset
  bool reload() override;

  //! Builds the asset
  bool build() override;

  //! Returns the necessary FMA code
  QString getFmaCode() const override;

  //! Returns the path
  inline const QDir &path() const { return mPath; }

  //! Returns a frame
  inline bool hasFrame(const QString &id) const { return mFrames.contains(id); }

  //! Returns a frame
  inline const QMap<QString, SpriteFrame> &frames() const { return mFrames; }

  //! Adds a frame
  inline void addFrame(const QString &id, const SpriteFrame &frame) { mFrames.insert(id, frame); }

  //! Returns all frames
  inline const QVector<SpriteImage> &images() const { return mImages; }

  //! Adds a new image
  inline void addImage(const SpriteImage &image) { mImages.push_back(image); }

  //! Creates an image group
  inline void createImageGroup(const QString &id, const SpriteImageGroup &group) { mImageGroups.insert(id, group); }

  //! Creates an image group
  inline void createFrameGroup(const QString &id, const SpriteFrameGroup &group) { mFrameGroups.insert(id, group); }

  //! Returns all image groups
  inline const QMap<QString, SpriteImageGroup> &imageGroups() const { return mImageGroups; }

  //! Returns all image groups
  inline const QMap<QString, SpriteFrameGroup> &frameGroups() const { return mFrameGroups; }

  //! Overrides data
  void overrideData(const QVector<SpriteImage> &images, const QMap<QString, SpriteImageGroup> &groups, const QMap<QString, SpriteFrame> &frames, const QMap<QString, SpriteAnimationFrames> &animations);

  //! Adds a new palette
  int addPalette(const QString &id);

  //! Returns an animation
  SpriteAnimationFrames &animation(const QString &id);

  //! Returns all animations
  inline const QMap<QString, SpriteAnimationFrames> &animations() const { return mAnimations; }

  //! Returns a generated animation
  GeneratedSpriteAnimation *generatedAnimation(const QString &id);

  //! Returns all generated animations
  inline const QMap<QString, GeneratedSpriteAnimation*> &generatedAnimations() const { return mGeneratedAnimations; }
  
  //! Returns the best animation ID
  QString findBestAnimationId(const QString &animation, const QString &direction) const;

  //! Returns additional object files to include
  QStringList getFmaObjectFiles() const override;

  //! Returns a new palette group
  PaletteGroup *createPaletteGroup(const QString &id);

  //! Returns the main palette group
  PaletteGroup *mainPaletteGroup();

  //! Returns whether the default palette should be included
  inline bool includesDefaultPalette() const { return mIncludeDefaultPalette; }

  //! Returns the sprite type
  inline SpriteTypeAssetSet *spriteType() const { return mType; }

  //! Sets the viewbox
  inline void setCalculatedViewbox(const QRect &r) { mViewbox = r; }

  //! Returns the calculated viewbox
  inline const QRect &calculatedViewbox() const { return mViewbox; }

  //! Returns a palette ID
  inline bool hasPalette(const QString &id) const { return mPaletteToId.contains(id); }

  //! Returns a palette ID
  inline int paletteId(const QString &id) const { return mPaletteToId[id]; }

private:
  //! The containing path
  QDir mPath;

  //! Whether we should include the default palettes
  bool mIncludeDefaultPalette;
  
  //! All frames
  QVector<SpriteImage> mImages;

  //! Map of palettes to indices
  QMap<QString, int> mPaletteToId;

  //! Map of all animations
  QMap<QString, SpriteAnimationFrames> mAnimations;

  //! Map of all animations
  QMap<QString, SpriteFrame> mFrames;

  //! Map of all animations generated
  QMap<QString, GeneratedSpriteAnimation*> mGeneratedAnimations;

  //! List of all image groups
  QMap<QString, SpriteImageGroup> mImageGroups;

  //! List of all image groups
  QMap<QString, SpriteFrameGroup> mFrameGroups;

  //! List of all palette groups
  QVector<PaletteGroup*> mPaletteGroups;

  //! The main palette group (only if includes default palette)
  PaletteGroup *mMainPaletteGroup = nullptr;

  //! The type
  SpriteTypeAssetSet *mType = nullptr;

  //! List of all custom animations
  QVector<QString> mCustomAnimations;
 
  //! Calculates viewbox (maximum bounding box for animation)
  QRect mViewbox;
};

}