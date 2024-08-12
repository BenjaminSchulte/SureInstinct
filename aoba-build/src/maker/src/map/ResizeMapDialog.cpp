#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QPushButton>
#include <QMessageBox>
#include <maker/game/GameProject.hpp>
#include <maker/game/GameLevelAssetType.hpp>
#include <maker/game/GameTilemapAssetType.hpp>
#include <maker/project/MakerProject.hpp>
#include <maker/map/ResizeMapDialog.hpp>
#include <maker/map/MapScreenshotHelper.hpp>
#include <maker/GlobalContext.hpp>
#include "ui_ResizeMapDialog.h"

// -----------------------------------------------------------------------------
ResizeMapDialog::ResizeMapDialog(Maker::GameLevelAsset *level, QWidget *parent)
	: QDialog(parent)
  , mScene(new QGraphicsScene(this))
  , mLevel(level)
  , mTilemap(level->tilemap())
	, mUi(new Ui::ResizeMapDialog)
{
  mUi->setupUi(this);

  QPixmap screenshot;
  screenshot.convertFromImage(MapScreenshotHelper().screenshot(level));
  
  mPixmap = mScene->addPixmap(screenshot);
  mPixmap->setScale(1.0 / 16.0);
  mNewSizeFrame = mScene->addRect(QRectF(), QPen(QColor(0, 0, 0, 255)), Qt::NoBrush);

  mUi->mapOffsetView->setStyleSheet("background: transparent");
  mUi->mapOffsetView->setScene(mScene);

  mUi->widthEdit->setMaximum(mTilemap->asset()->assetSet()->maxWidth());
  mUi->heightEdit->setMaximum(mTilemap->asset()->assetSet()->maxHeight());

  mUi->widthEdit->setValue(mTilemap->asset()->width());
  mUi->heightEdit->setValue(mTilemap->asset()->height());

  setAlign(-1, -1);
}

// -----------------------------------------------------------------------------
ResizeMapDialog::~ResizeMapDialog() {
	delete mUi;
}

// -----------------------------------------------------------------------------
void ResizeMapDialog::sizeChanged() {
  int width = mUi->widthEdit->value();
  int height = mUi->heightEdit->value();

  int diffX = width - mTilemap->asset()->width();
  int diffY = height - mTilemap->asset()->height();

  mProtectAlign = true;
  mUi->offsetXEdit->setMinimum(qMin(0, diffX));
  mUi->offsetXEdit->setMaximum(qMax(0, diffX));
  mUi->offsetYEdit->setMinimum(qMin(0, diffY));
  mUi->offsetYEdit->setMaximum(qMax(0, diffY));
  mProtectAlign = false;

  realignMap();
}

// -----------------------------------------------------------------------------
void ResizeMapDialog::setAlign(int x, int y) {
  mHasCustomAlign = false;
  mAlignX = x;
  mAlignY = y;

  updateAlignButton();
  realignMap();
}

// -----------------------------------------------------------------------------
void ResizeMapDialog::updateAlignButton() {
  int x = mHasCustomAlign ? -2 : mAlignX;
  int y = mHasCustomAlign ? -2 : mAlignY;

  mUi->alignTopLeft->setChecked(x == -1 && y == -1);
  mUi->alignTopCenter->setChecked(x == 0 && y == -1);
  mUi->alignTopRight->setChecked(x == 1 && y == -1);
  mUi->alignCenterLeft->setChecked(x == -1 && y == 0);
  mUi->alignCenter->setChecked(x == 0 && y == 0);
  mUi->alignCenterRight->setChecked(x == 1 && y == 0);
  mUi->alignBottomLeft->setChecked(x == -1 && y == 1);
  mUi->alignBottomCenter->setChecked(x == 0 && y == 1);
  mUi->alignBottomRight->setChecked(x == 1 && y == 1);
}

// -----------------------------------------------------------------------------
void ResizeMapDialog::offsetChanged() {
  if (mProtectAlign) {
    return;
  }

  mHasCustomAlign = true;
  updateAlignButton();
  updatePreviewScene();
}
  
// -----------------------------------------------------------------------------
void ResizeMapDialog::realignMap() {
  if (mHasCustomAlign) {
    return;
  }

  mProtectAlign = true;

  if (mAlignX == -1) {
    mUi->offsetXEdit->setValue(0);
  } else if (mAlignX == 0) {
    mUi->offsetXEdit->setValue(((mUi->offsetXEdit->maximum() - mUi->offsetXEdit->minimum()) >> 1) + mUi->offsetXEdit->minimum());
  } else if (mUi->offsetXEdit->minimum() < 0) {
    mUi->offsetXEdit->setValue(mUi->offsetXEdit->minimum());
  } else {
    mUi->offsetXEdit->setValue(mUi->offsetXEdit->maximum());
  }

  if (mAlignY == -1) {
    mUi->offsetYEdit->setValue(0);
  } else if (mAlignY == 0) {
    mUi->offsetYEdit->setValue(((mUi->offsetYEdit->maximum() - mUi->offsetYEdit->minimum()) >> 1) + mUi->offsetYEdit->minimum());
  } else if (mUi->offsetYEdit->minimum() < 0) {
    mUi->offsetYEdit->setValue(mUi->offsetYEdit->minimum());
  } else {
    mUi->offsetYEdit->setValue(mUi->offsetYEdit->maximum());
  }

  mProtectAlign = false;
  updatePreviewScene();
}

// -----------------------------------------------------------------------------
QPoint ResizeMapDialog::mapOffset() const {
  return QPoint(
    mUi->offsetXEdit->value(),
    mUi->offsetYEdit->value()
  );
}

// -----------------------------------------------------------------------------
QSize ResizeMapDialog::newMapSize() const {
  return QSize(
    mUi->widthEdit->value(),
    mUi->heightEdit->value()
  );
}

// -----------------------------------------------------------------------------
void ResizeMapDialog::updatePreviewScene() {
  QRect innerSize(
    QPoint(
      mUi->offsetXEdit->minimum(),
      mUi->offsetYEdit->minimum()
    ),
    QPoint(
      mUi->offsetXEdit->maximum() + mTilemap->asset()->width(),
      mUi->offsetYEdit->maximum() + mTilemap->asset()->height()
    )
  );
  
  mScene->setSceneRect(innerSize);
  mPixmap->setPos(mapOffset());
  mNewSizeFrame->setRect(QRect(QPoint(0, 0), newMapSize()));

  mUi->mapOffsetView->fitInView(innerSize, Qt::KeepAspectRatio);
}
