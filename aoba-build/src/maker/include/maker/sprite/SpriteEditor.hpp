#pragma once

#include <QMainWindow>
#include <QMap>
#include <aoba/sprite/Sprite.hpp>

typedef QSharedPointer<class MakerProject> MakerProjectPtr;
typedef QSharedPointer<QImage> QImagePtr;

class QComboBox;
class QFrame;
class QListWidgetItem;
class SpriteEditorScene;
class SpriteAnimationScene;
class AobaTimelineTrack;
class AobaTimelineItem;

namespace Maker {
class GameSpriteAssetType;
class GameSpriteAsset;
}

namespace Ui {
class SpriteEditor;
}

struct SpriteEditorSprite {
  SpriteEditorSprite() {}
  SpriteEditorSprite(Aoba::Sprite *sprite) : mSprite(sprite) {
    mImages = sprite->images();
    mFrames = sprite->frames();
    mAnimations = sprite->animations();
    mImageGroups = sprite->imageGroups();
  }

  //! Returns an image of an frame
  QImagePtr frameImage(int frame);

  Aoba::Sprite *mSprite = nullptr;
  bool mModified = false;
  QVector<Aoba::SpriteImage> mImages;
  QMap<QString, Aoba::SpriteAnimationFrames> mAnimations;
  QMap<QString, Aoba::SpriteFrame> mFrames;
  QStringList mGeneratedFrames;
  QMap<int, QImagePtr> mCachedImages;
  QMap<QString, Aoba::SpriteImageGroup> mImageGroups;
};

class SpriteEditor : public QMainWindow {
  Q_OBJECT

public:
  //! Constructor
  SpriteEditor(const MakerProjectPtr &project, QWidget *parent);

  //! Deconstructor
  ~SpriteEditor();

  //! Saves the changes
  bool save();

private slots:
  void updateUi();

  void onSpriteListCurrentIndexChanged(int index);

  void onAnimationListCurrentIndexChanged(int index);

  void on_timeline_currentItemChanged(AobaTimelineItem *item);

  void on_imageList_itemDoubleClicked(QListWidgetItem *item);

  void on_frameTypeCombo_currentIndexChanged(int index);

  void on_animationPlayButton_clicked();
  void on_animationRestartButton_clicked();

  void on_actionCreateCustomAnimation_triggered();
  void on_actionPriorityImage_triggered();
  void on_actionSharedImage_triggered();
  void on_actionStaticImage_triggered();

  void on_actionFlipX_triggered();
  void on_actionFlipY_triggered();

  void updateFrameFromScene();

  void addSpriteFrame();
  void removeSpriteFrame();

private:
  //! Saves a sprite
  bool saveSprite(SpriteEditorSprite *);

  //! Ensures that the current frame can be edited
  bool hasSafeEditFrame(int index);

  //! Returns the safe edit frame
  Aoba::SpriteAnimation &editAnimation(int index);

  //! Create the image frame group config
  void createImageFrameGroup(const Aoba::SpriteAnimation &);

  //! Create the image frame group config
  void createMoveFrameGroup(const Aoba::SpriteAnimation &);

  //! Updates the preview
  void updateItemPreview(AobaTimelineItem *item, const Aoba::SpriteAnimation &frame);

  //! Looks up for a noncommand frame
  int findNonCommandFrame(int index);

  //! Unloads the current sprite
  void unloadCurrentSprite();

  //! Updates the sprite list
  void updateSpriteList();

  //! Updates the animation list
  void updateAnimationList();

  //! Updates the frame list
  void updateFrameList();

  //! Updates the list of all images
  void updateImageList();

  //! Creates a custom animation
  void createCustomAnimation(const QString &name, const QString &dir);

  //! Sets a frame
  void setFrame(int);

  //! The UI
  Ui::SpriteEditor *mUi;

  //! The project
  MakerProjectPtr mProject;

  //! Sprite list
  Maker::GameSpriteAssetType *mSprites = nullptr;

  //! The current animation ID
  QString mAnimation;

  //! The current editor scene
  SpriteEditorScene *mScene;

  //! The preview
  SpriteAnimationScene *mAnimationScene;

  //! The images track
  AobaTimelineTrack *mImageTrack;

  //! Sprite select combo box
  QComboBox *mSpriteList;

  //! Sprite select combo box
  QComboBox *mAnimationList;

  //! Current group box with properties
  QFrame *mFrameGroup = nullptr;

  //! List of sprite copies
  QMap<QString, SpriteEditorSprite*> mLoadedSprites;

  //! The current sprite
  SpriteEditorSprite *mSprite = nullptr;

  //! Whether the current frame is locked
  int mCurrentFrame = -1;

  //! Whether the current frame is locked
  int mCurrentVisibleFrame = -1;

  //! Whether the frame is flipped
  bool mFrameFlippedX;

  //! Whether the frame is flipped
  bool mFrameFlippedY;

  //! Temporary pointer for invalid cases
  Aoba::SpriteAnimation mInvalidAnimation;
};