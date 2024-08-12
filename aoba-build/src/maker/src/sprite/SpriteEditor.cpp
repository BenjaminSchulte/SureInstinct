#include <QIcon>
#include <QInputDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QFrame>
#include <maker/sprite/SpriteEditor.hpp>
#include <maker/sprite/SpriteEditorScene.hpp>
#include <maker/sprite/SpriteEditorWriter.hpp>
#include <maker/sprite/SpriteAnimationScene.hpp>
#include <maker/common/AobaTimelineTrack.hpp>
#include <maker/common/AobaTimelineItem.hpp>
#include <maker/common/AobaTimelineHeader.hpp>
#include <maker/project/MakerProject.hpp>
#include <maker/game/GameAsset.hpp>
#include <maker/game/GameSpriteAssetType.hpp>
#include <aoba/sprite/Sprite.hpp>
#include <aoba/sprite/SpriteType.hpp>
#include <aoba/sprite/SpriteAnimationIndex.hpp>
#include <aoba/palette/PaletteGroup.hpp>
#include <aoba/palette/Palette.hpp>
#include <aoba/palette/SnesPalette.hpp>
#include <aoba/image/PaletteImageAccessor.hpp>
#include <maker/game/GameProject.hpp>
#include "ui_SpriteEditor.h"

// -----------------------------------------------------------------------------
QImagePtr SpriteEditorSprite::frameImage(int frame) {
  if (mCachedImages.contains(frame)) {
    return mCachedImages[frame];
  }

  const Aoba::SpriteImage &image = mImages[frame];
  Aoba::PaletteImageAccessor paletteImage(image.image->toPaletteImage());
  Aoba::Palette *palette = mSprite->mainPaletteGroup()->get(image.paletteIndex);
  if (!palette) {
    return QImagePtr();
  }

  QImagePtr imageData = paletteImage.assignPalette(palette->palette()->view());
  mCachedImages.insert(frame, imageData);

  return mCachedImages[frame];
}



// -----------------------------------------------------------------------------
SpriteEditor::SpriteEditor(const MakerProjectPtr &project, QWidget *parent)
  : QMainWindow(parent)
  , mUi(new Ui::SpriteEditor)
  , mProject(project)
  , mScene(new SpriteEditorScene(this))
  , mAnimationScene(new SpriteAnimationScene(this))
{
  mUi->setupUi(this);

  mSpriteList = new QComboBox();
  mAnimationList = new QComboBox();

  mUi->spriteToolBar->addWidget(mSpriteList);
  mUi->animationToolBar->insertWidget(mUi->actionCreateCustomAnimation, mAnimationList);

  mUi->frameTypeCombo->addItem(tr("Image"), (int)Aoba::SpriteAnimationCommand::NONE);
  mUi->frameTypeCombo->addItem(tr("Loop Start"), (int)Aoba::SpriteAnimationCommand::BEGIN_LOOP);
  mUi->frameTypeCombo->addItem(tr("Loop"), (int)Aoba::SpriteAnimationCommand::LOOP);
  mUi->frameTypeCombo->addItem(tr("Move"), (int)Aoba::SpriteAnimationCommand::MOVE);
  mUi->frameTypeCombo->addItem(tr("Destroy Self"), (int)Aoba::SpriteAnimationCommand::DESTROY_SELF);

  connect(mSpriteList, SIGNAL(currentIndexChanged(int)), this, SLOT(onSpriteListCurrentIndexChanged(int)));
  connect(mAnimationList, SIGNAL(currentIndexChanged(int)), this, SLOT(onAnimationListCurrentIndexChanged(int)));
  connect(mScene, SIGNAL(imagesChanged()), this, SLOT(updateFrameFromScene()));
  connect(mScene, SIGNAL(selectionChanged()), this, SLOT(updateUi()));

  connect(mUi->timeline->header(), &AobaTimelineHeader::addClicked, this, &SpriteEditor::addSpriteFrame);
  connect(mUi->timeline->header(), &AobaTimelineHeader::removeClicked, this, &SpriteEditor::removeSpriteFrame);

  mImageTrack = mUi->timeline->createTrack(tr("Images"));
  
  mUi->frameEditor->setScene(mScene);
  mUi->frameEditor->scale(4, 4);
  mUi->preview->setScene(mAnimationScene);
  mUi->preview->scale(2, 2);
  
  setWindowFlags(Qt::Widget);
  setCorner(Qt::Corner::BottomLeftCorner, Qt::DockWidgetArea::LeftDockWidgetArea);
  setCorner(Qt::Corner::BottomRightCorner, Qt::DockWidgetArea::RightDockWidgetArea);
  setCorner(Qt::Corner::TopLeftCorner, Qt::DockWidgetArea::LeftDockWidgetArea);
  setCorner(Qt::Corner::TopRightCorner, Qt::DockWidgetArea::RightDockWidgetArea);

  updateSpriteList();
}

// -----------------------------------------------------------------------------
SpriteEditor::~SpriteEditor() {
  for (SpriteEditorSprite *sprite : mLoadedSprites) {
    delete sprite;
  }

  delete mAnimationList;
  delete mSpriteList;
  delete mUi;
}

// -----------------------------------------------------------------------------
void SpriteEditor::unloadCurrentSprite() {
  if (mSprite && !mSprite->mModified) {
    mLoadedSprites.remove(mSprite->mSprite->id());
    delete mSprite;
  }

  mSprite = nullptr;
}

// -----------------------------------------------------------------------------
void SpriteEditor::on_animationPlayButton_clicked() {
  mAnimationScene->setPaused(!mUi->animationPlayButton->isChecked());
}

// -----------------------------------------------------------------------------
void SpriteEditor::on_animationRestartButton_clicked() {
  mAnimationScene->restart();
}

// -----------------------------------------------------------------------------
void SpriteEditor::onSpriteListCurrentIndexChanged(int index) {
  mAnimation = "";
  unloadCurrentSprite();

  if (index == -1 || !mSprites) {
    updateUi();
    return;
  }

  QString id = mSpriteList->itemData(index).toString();
  Maker::GameSpriteAsset *gameSprite = dynamic_cast<Maker::GameSpriteAsset*>(mProject->gameProject()->assets().getAsset("MapSprite", id));
  if (!gameSprite) {
    updateUi();
    updateImageList();
    return;
  }

  Aoba::Sprite *sprite = gameSprite->asset();
  if (!mLoadedSprites.contains(sprite->id())) {
    mLoadedSprites.insert(sprite->id(), new SpriteEditorSprite(sprite));
  }

  mSprite = mLoadedSprites[sprite->id()];

  updateImageList();
  updateAnimationList();
  updateUi();
}

// -----------------------------------------------------------------------------
void SpriteEditor::onAnimationListCurrentIndexChanged(int index) {
  mAnimation = "";
  mAnimationScene->clearAnimation();
  mUi->timeline->clear();

  if (index == -1 || !mSprite) {
    updateUi();
    return;
  }

  mAnimation = mAnimationList->itemData(index).toString();
  QStringList parts = mAnimation.split('_');
  if (parts.size() > 1) {
    if (parts[1] == "up") {
      mAnimationScene->setMoveVector(QPointF(0, -1));
    } else if (parts[1] == "left") {
      mAnimationScene->setMoveVector(QPointF(-1, 0));
    } else if (parts[1] == "right") {
      mAnimationScene->setMoveVector(QPointF(1, 0));
    } else {
      mAnimationScene->setMoveVector(QPointF(0, 1));
    }
  } else {
    mAnimationScene->setMoveVector(QPointF(0, 1));
  }

  mAnimationScene->setAnimation(mSprite, mAnimation);
  updateFrameList();
  updateUi();
}

// -----------------------------------------------------------------------------
void SpriteEditor::on_actionFlipX_triggered() {
  if (mCurrentVisibleFrame == -1) {
    return;
  }

  mFrameFlippedX = mUi->actionFlipX->isChecked();
  mScene->setFlip(mFrameFlippedX, mFrameFlippedY);
  updateFrameFromScene();
}

// -----------------------------------------------------------------------------
void SpriteEditor::on_actionFlipY_triggered() {
  if (mCurrentVisibleFrame == -1) {
    return;
  }

  mFrameFlippedY = mUi->actionFlipY->isChecked();
  mScene->setFlip(mFrameFlippedX, mFrameFlippedY);
  updateFrameFromScene();
}

// -----------------------------------------------------------------------------
void SpriteEditor::on_timeline_currentItemChanged(AobaTimelineItem *item) {
  if (!item) {
    mCurrentFrame = -1;
    mCurrentVisibleFrame = -1;
    mScene->removeAllImages();
    return;
  }

  setFrame((int)item->start());
}

// -----------------------------------------------------------------------------
void SpriteEditor::updateSpriteList() {
  mSpriteList->clear();

  unloadCurrentSprite();
  mAnimation = "";
  updateImageList();


  mSprites = mProject->gameProject()->assetTypes().get<Maker::GameSpriteAssetType>("MapSprite");
  if (!mSprites) {
    updateUi();
    return;
  }

  for (const Aoba::Sprite *sprite : mSprites->assetSet()->all()) {
    mSpriteList->addItem(sprite->name(), sprite->id());
  }

  mSpriteList->setCurrentIndex(0);

  updateUi();
}

// -----------------------------------------------------------------------------
void SpriteEditor::updateAnimationList() {
  mAnimationList->clear();
  mAnimationScene->clearAnimation();
  mAnimation = "";

  if (!mSprite) {
    return;
  }

  auto *index = mSprite->mSprite->spriteType()->animationIndex();
  QVector<QString> generatedAnimations;
  for (const QString &anim : index->animations().keys()) {
    for (const QString &dir : index->directions().keys()) {
      QString key = anim + "_" + dir;
      mAnimationList->addItem(anim + ": " + dir, key);
      generatedAnimations.push_back(key);
    }
  }

  QMapIterator<QString, Aoba::SpriteAnimationFrames> it(mSprite->mAnimations);
  while (it.hasNext()) {
    it.next();

    if (generatedAnimations.contains(it.key())) {
      continue;
    }

    mAnimationList->addItem(it.key(), it.key());
  }

  int newIndex = qMax(0, mAnimationList->findData("stand_down"));
  mAnimationList->setCurrentIndex(newIndex);

  updateUi();
}

// -----------------------------------------------------------------------------
void SpriteEditor::updateItemPreview(AobaTimelineItem *item, const Aoba::SpriteAnimation &frame) {
  if (item == nullptr) {
    return;
  }

  if (frame.isCommand()) {
    switch (frame.command) {
      default: item->setText("FX"); break;
    }
    item->setIcon(QIcon());
  } else { 
    const QVector<Aoba::SpriteFrameImage> &images = mSprite->mFrames[frame.frame].images;
    if (!images.isEmpty()) {
      QPixmap pixmap;
      QImagePtr image(mSprite->frameImage(images.front().image));
      pixmap.convertFromImage(*image.get());
      item->setIcon(QIcon(pixmap));
    } else {
      item->setIcon(QIcon());
      item->setText("-");
    }
  }

}

// -----------------------------------------------------------------------------
void SpriteEditor::updateFrameList() {
  mUi->timeline->clear();
  mScene->removeAllImages();

  if (!mSprite || mAnimation.isEmpty()) {
    return;
  }

  const Aoba::SpriteAnimationFrames &frames = mSprite->mAnimations[mAnimation];
  for (int i=0; i<frames.count(); i++)   {
    AobaTimelineItem *item = new AobaTimelineItem(i, 1, mImageTrack);
    updateItemPreview(item, frames[i]);
    mImageTrack->addItem(item);
  }

  on_timeline_currentItemChanged(mUi->timeline->currentItem());
}

// -----------------------------------------------------------------------------
void SpriteEditor::addSpriteFrame() {
  if (!mSprite || mAnimation.isEmpty()) {
    return;
  }

  Aoba::SpriteAnimation animation;
  QString newFrameName = ".temp" + QString::number(mSprite->mGeneratedFrames.length());
  mSprite->mGeneratedFrames.push_back(newFrameName);
  mSprite->mFrames[newFrameName] = mSprite->mFrames[animation.frame];
  animation.frame = newFrameName;
  mSprite->mAnimations[mAnimation].insert(mCurrentFrame + 1, animation);

  AobaTimelineItem *item = new AobaTimelineItem(mCurrentFrame + 1, 1, mImageTrack);
  updateItemPreview(item, animation);
  mImageTrack->insertItem(mCurrentFrame + 1, item);
  mUi->timeline->setCurrentFrameIndex(mCurrentFrame + 1);
}

// -----------------------------------------------------------------------------
void SpriteEditor::removeSpriteFrame() {
  if (!mSprite || mAnimation.isEmpty() || mCurrentFrame == -1) {
    return;
  }

  mSprite->mAnimations[mAnimation].remove(mCurrentFrame);
  mImageTrack->removeItem(mImageTrack->currentItem());
}

// -----------------------------------------------------------------------------
void SpriteEditor::setFrame(int index) {
  mCurrentFrame = -1;
  mCurrentVisibleFrame = -1;
  mScene->removeAllImages();
  
  delete mFrameGroup;
  mFrameGroup = nullptr;

  if (!mSprite || mAnimation.isEmpty()) {
    updateUi();
    return;
  }

  int visibleIndex = findNonCommandFrame(index);
  if (visibleIndex == -1) {
    updateUi();
    return;
  }

  Aoba::SpriteAnimation commandFrame = mSprite->mAnimations[mAnimation][index];
  Aoba::SpriteAnimation frame = mSprite->mAnimations[mAnimation][visibleIndex];
  if (frame.isCommand()) {
    updateUi();
    return;
  }

  const Aoba::SpriteFrame &images = mSprite->mFrames[frame.frame];

  mFrameFlippedX = frame.flipX;
  mFrameFlippedY = frame.flipY;
  mScene->setFlip(frame.flipX, frame.flipY);
  mUi->actionFlipX->setChecked(mFrameFlippedX);
  mUi->actionFlipY->setChecked(mFrameFlippedY);

  for (const Aoba::SpriteFrameImage &image : images.images) {
    const Aoba::SpriteImage &imageInfo = mSprite->mImages[image.image];

    QPoint pos(image.x + frame.x - imageInfo.pivotX, image.y + frame.y - imageInfo.pivotY);

    bool isStatic = false;
    bool isShared = false;
    if (!images.sharedImage.isEmpty() && mSprite->mImageGroups.contains(images.sharedImage)) {
      isShared = mSprite->mImageGroups[images.sharedImage].contains(image.image);
    }
    if (!images.staticImage.isEmpty() && mSprite->mImageGroups.contains(images.staticImage)) {
      isStatic = mSprite->mImageGroups[images.staticImage].contains(image.image);
    }

    mScene->addImage(image.image, mSprite->frameImage(image.image), pos, isStatic, isShared, image.priority);
  }

  mUi->frameTypeCombo->setCurrentIndex(mUi->frameTypeCombo->findData((int)commandFrame.command));

  mCurrentVisibleFrame = visibleIndex;
  mCurrentFrame = index;
  on_frameTypeCombo_currentIndexChanged(mUi->frameTypeCombo->currentIndex());
  updateUi();
}

// -----------------------------------------------------------------------------
void SpriteEditor::on_frameTypeCombo_currentIndexChanged(int index) {
  delete mFrameGroup;
  mFrameGroup = nullptr;

  if (index == -1 || !mSprite || mCurrentFrame == -1) {
    return;
  }

  Aoba::SpriteAnimation &animation = mSprite->mAnimations[mAnimation][mCurrentFrame];
  Aoba::SpriteAnimationCommand newCommand = (Aoba::SpriteAnimationCommand)mUi->frameTypeCombo->itemData(index).toInt();
  
  if (animation.command != newCommand) {
    animation.command = newCommand;
    animation.commandParameter = 0;
    animation.delay = 0;
    switch (animation.command) {
      case Aoba::SpriteAnimationCommand::NONE: animation.delay = 5; break;
      case Aoba::SpriteAnimationCommand::MOVE: animation.commandParameter = 128; break;

      default:
        break;
    }

    mSprite->mModified = true;
  }

  switch (animation.command) {
    case Aoba::SpriteAnimationCommand::NONE: createImageFrameGroup(animation); break;
    case Aoba::SpriteAnimationCommand::MOVE: createMoveFrameGroup(animation); break;

    default:
      break;
  }
}

// -----------------------------------------------------------------------------
Aoba::SpriteAnimation &SpriteEditor::editAnimation(int index) {
  mSprite->mModified = true;
  return mSprite->mAnimations[mAnimation][index];
}

// -----------------------------------------------------------------------------
void SpriteEditor::createImageFrameGroup(const Aoba::SpriteAnimation &animation) {
  mFrameGroup = new QFrame(mUi->scrollAreaWidgetContents);
  mFrameGroup->setFrameShape(QFrame::StyledPanel);
  mFrameGroup->setFrameShadow(QFrame::Raised);
  dynamic_cast<QVBoxLayout*>(mUi->scrollAreaWidgetContents->layout())->insertWidget(1, mFrameGroup);

  QFormLayout *layout = new QFormLayout(mFrameGroup);
  mFrameGroup->setLayout(layout);
  
  QSpinBox *duration = new QSpinBox(mFrameGroup);
  duration->setMinimum(1);
  duration->setMaximum(255);
  duration->setValue(animation.delay);
  connect(duration, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
    editAnimation(mCurrentFrame).delay = value;
  });
  layout->addRow(tr("Duration"), duration);
}

// -----------------------------------------------------------------------------
void SpriteEditor::createMoveFrameGroup(const Aoba::SpriteAnimation &animation) {
  mFrameGroup = new QFrame(mUi->scrollAreaWidgetContents);
  mFrameGroup->setFrameShape(QFrame::StyledPanel);
  mFrameGroup->setFrameShadow(QFrame::Raised);
  dynamic_cast<QVBoxLayout*>(mUi->scrollAreaWidgetContents->layout())->insertWidget(1, mFrameGroup);

  QFormLayout *layout = new QFormLayout(mFrameGroup);
  mFrameGroup->setLayout(layout);
  
  QDoubleSpinBox *duration = new QDoubleSpinBox(mFrameGroup);
  duration->setMinimum(-16);
  duration->setMaximum(16);
  duration->setSingleStep(0.025);
  duration->setValue(animation.commandParameter / 256.0);
  connect(duration, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
    editAnimation(mCurrentFrame).commandParameter = value * 256.0;
  });
  layout->addRow(tr("Speed"), duration);
}

// -----------------------------------------------------------------------------
bool SpriteEditor::hasSafeEditFrame(int frameIndex) {
  if (frameIndex == -1 || !mSprite || mAnimation.isEmpty()) {
    return false;
  }

  Aoba::SpriteAnimation &animation = mSprite->mAnimations[mAnimation][frameIndex];
  if (animation.isCommand()) {
    return false;
  }

  if (!mSprite->mGeneratedFrames.contains(animation.frame)) {
    QString newFrameName = ".temp" + QString::number(mSprite->mGeneratedFrames.length());
    mSprite->mGeneratedFrames.push_back(newFrameName);
    mSprite->mFrames[newFrameName] = mSprite->mFrames[animation.frame];
    animation.frame = newFrameName;
  }

  return true;
}

// -----------------------------------------------------------------------------
void SpriteEditor::updateFrameFromScene() {
  if (!hasSafeEditFrame(mCurrentVisibleFrame)) {
    return;
  }

  Aoba::SpriteAnimation &animation = mSprite->mAnimations[mAnimation][mCurrentVisibleFrame];
  Aoba::SpriteFrame &frame = mSprite->mFrames[animation.frame];
  frame.images.clear();
  animation.flipX = mScene->isFlipX();
  animation.flipY = mScene->isFlipY();

  QStringList tempSharedGroupId;
  QVector<int> tempSharedGroup;
  QStringList tempStaticGroupId;
  QVector<int> tempStaticGroup;
  for (SpriteEditorSceneItem *item : mScene->images()) {
    const auto &imageInfo = mSprite->mImages[item->imageIndex()];

    QPointF originPos(item->flippedPos(mScene->isFlipX(), mScene->isFlipY()));
    QPoint scenePos(originPos.x(), originPos.y());
    QPoint pos(
      scenePos.x() + imageInfo.pivotX - animation.x,
      scenePos.y() + imageInfo.pivotY - animation.y
    );

    Aoba::SpriteFrameImage img;
    img.image = item->imageIndex();
    img.x = pos.x();
    img.y = pos.y();
    img.priority = item->priority();
    frame.images.push_back(img);

    if (item->isShared()) {
      tempSharedGroupId.push_back(QString::number(item->imageIndex()));
      tempSharedGroup.push_back(item->imageIndex());
    }

    if (item->isStatic()) {
      tempStaticGroupId.push_back(QString::number(item->imageIndex()));
      tempStaticGroup.push_back(item->imageIndex());
    }
  }

  if (!tempSharedGroup.isEmpty()) {
    frame.sharedImage = tempSharedGroupId.join('.');
    mSprite->mImageGroups.insert(frame.sharedImage, tempSharedGroup);
  } else {
    frame.sharedImage = "";
  }

  if (!tempStaticGroup.isEmpty()) {
    frame.staticImage = tempStaticGroupId.join('.');
    mSprite->mImageGroups.insert(frame.staticImage, tempStaticGroup);
  } else {
    frame.staticImage = "";
  }

  mSprite->mModified = true;

  updateItemPreview(mImageTrack->itemAtFrameIndex(mCurrentVisibleFrame), animation);
}

// -----------------------------------------------------------------------------
int SpriteEditor::findNonCommandFrame(int index) {
  const QVector<Aoba::SpriteAnimation> &frames = mSprite->mAnimations[mAnimation];
  if (frames.isEmpty()) {
    return -1;
  }

  if (index >= frames.count()) {
    index = frames.count() - 1;
  }

  int startIndex = index;
  for (; index < frames.count(); index++) {
    if (!frames[index].isCommand()) {
      return index;
    }
  }

  for (index = startIndex; index >= 0; index--) {
    if (!frames[index].isCommand()) {
      return index;
    }
  }

  return -1;
}

// -----------------------------------------------------------------------------
void SpriteEditor::updateImageList() {
  mUi->imageList->clear();

  if (!mSprite) {
    return;
  }

  int index = 0;
  for (const Aoba::SpriteImage &image : mSprite->mImages) {
    (void)image;

    QListWidgetItem *item = new QListWidgetItem(tr("Image %1").arg(index), mUi->imageList);
    item->setData(Qt::UserRole, index);

    QImagePtr iconData(mSprite->frameImage(index));
    if (!iconData) {
      index++;
      continue;
    }

    QPixmap icon;
    icon.convertFromImage(*iconData.get());
    item->setIcon(QIcon(icon.scaled(icon.size() * 2)));

    index++;
  }
}

// -----------------------------------------------------------------------------
void SpriteEditor::on_imageList_itemDoubleClicked(QListWidgetItem *item) {
  if (!mSprite || mAnimation.isEmpty() || mCurrentVisibleFrame == -1) {
    return;
  }

  int index = item->data(Qt::UserRole).toInt();
  if (index < 0 || index >= mSprite->mImages.count()) {
    return;
  }

  mScene->addImage(index, mSprite->frameImage(index), QPoint(0, 0), false, false, false);
  updateFrameFromScene();
}

// -----------------------------------------------------------------------------
void SpriteEditor::on_actionCreateCustomAnimation_triggered() {
  if (!mSprite) {
    return;
  }

  bool ok;
  QString text;
  QRegularExpression re("^[a-zA-Z0-9_]+$");

  do {
    text = QInputDialog::getText(this, tr("Create custom animation"), tr("Animation ID:"), QLineEdit::Normal, text, &ok).trimmed();
    if (!ok) {
      return;
    }

    if (mSprite->mAnimations.contains(text) || mSprite->mAnimations.contains(text + "_down")) {
      QMessageBox::warning(this, tr("Create custom animaiton"), tr("An animation with the ID '%1' already exists").arg(text));
      continue;
    }

    if (!re.match(text).hasMatch()) {
      QMessageBox::warning(this, tr("Create custom animation"), tr("The animation ID '%1' contains invalid characters").arg(text));
      continue;
    }

    break;
  } while(true);

  const QMap<QString, int> &dirs = mSprite->mSprite->spriteType()->animationIndex()->directions();
  if (dirs.isEmpty()) {
    createCustomAnimation(text, "down");
  } else {
    QMapIterator<QString, int> it(dirs);
    while (it.hasNext()) {
      it.next();
      createCustomAnimation(text, it.key());
    }
  }

  updateAnimationList();
}

// -----------------------------------------------------------------------------
void SpriteEditor::createCustomAnimation(const QString &name, const QString &direction) {
  mSprite->mAnimations.insert(name + "_" + direction, {});
}

// -----------------------------------------------------------------------------
void SpriteEditor::on_actionPriorityImage_triggered() {
  SpriteEditorSceneItem *item = dynamic_cast<SpriteEditorSceneItem*>(mScene->selectedItems().first());
  if (item) {
    item->setPriority(mUi->actionPriorityImage->isChecked());
    updateFrameFromScene();
  }
}

// -----------------------------------------------------------------------------
void SpriteEditor::on_actionSharedImage_triggered() {
  SpriteEditorSceneItem *item = dynamic_cast<SpriteEditorSceneItem*>(mScene->selectedItems().first());
  if (item) {
    item->setIsShared(mUi->actionSharedImage->isChecked());
    updateFrameFromScene();
  }
}

// -----------------------------------------------------------------------------
void SpriteEditor::on_actionStaticImage_triggered() {
  SpriteEditorSceneItem *item = dynamic_cast<SpriteEditorSceneItem*>(mScene->selectedItems().first());
  if (item) {
    item->setIsStatic(mUi->actionStaticImage->isChecked());
    updateFrameFromScene();
  }
}

// -----------------------------------------------------------------------------
void SpriteEditor::updateUi() {
  bool hasSprite = mSprite != nullptr;
  bool hasFrame = mCurrentFrame != -1;
  bool hasImage = mScene->selectedItems().count() == 1;
  bool hasImages = mScene->selectedItems().count() > 0;

  mAnimationList->setEnabled(hasSprite);
  mUi->actionCreateCustomAnimation->setEnabled(hasSprite);
  
  mUi->actionFlipX->setEnabled(hasFrame);
  mUi->actionFlipY->setEnabled(hasFrame);

  mUi->frameTypeCombo->setEnabled(hasFrame);

  mUi->actionPriorityImage->setEnabled(hasImage);
  mUi->actionImageToBackground->setEnabled(hasImages);
  mUi->actionImageToForeground->setEnabled(hasImages);
  mUi->actionStaticImage->setEnabled(hasImage);
  mUi->actionSharedImage->setEnabled(hasImage);

  if (hasImage) {
    SpriteEditorSceneItem *item = dynamic_cast<SpriteEditorSceneItem*>(mScene->selectedItems().first());

    mUi->actionPriorityImage->setChecked(item->priority());
    mUi->actionStaticImage->setChecked(item->isStatic());
    mUi->actionSharedImage->setChecked(item->isShared());
  } else {
    mUi->actionPriorityImage->setChecked(false);
    mUi->actionStaticImage->setChecked(false);
    mUi->actionSharedImage->setChecked(false);
  }
}

// -----------------------------------------------------------------------------
bool SpriteEditor::save() {
  bool success = true;

  for (SpriteEditorSprite *sprite : mLoadedSprites) {
    if (!sprite->mModified) {
      continue;
    }

    if (!saveSprite(sprite)) {
      success = false;
    }
  }

  return success;
}

// -----------------------------------------------------------------------------
bool SpriteEditor::saveSprite(SpriteEditorSprite *sprite) {
  SpriteEditorWriter writer(sprite);
  return writer.save() && sprite->mSprite->save();
}
