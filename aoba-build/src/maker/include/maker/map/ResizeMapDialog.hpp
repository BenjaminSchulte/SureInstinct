#pragma once

#include <QDialog>
#include <QImage>

class QGraphicsScene;

namespace Ui {
class ResizeMapDialog;
}

namespace Maker {
class GameLevelAsset;
class GameTilemapAsset;
}

class ResizeMapDialog : public QDialog {
	Q_OBJECT
	
public:
  //! Constructor
	explicit ResizeMapDialog(Maker::GameLevelAsset *level, QWidget *parent = nullptr);

  //! Deconstructor
	~ResizeMapDialog();

  //! Returns the offset
  QPoint mapOffset() const;

  //! Returns the offset
  QSize newMapSize() const;

private slots:
  void on_alignTopLeft_clicked() { setAlign(-1, -1); }
  void on_alignTopCenter_clicked() { setAlign(0, -1); }
  void on_alignTopRight_clicked() { setAlign(1, -1); }
  void on_alignCenterLeft_clicked() { setAlign(-1, 0); }
  void on_alignCenter_clicked() { setAlign(0, 0); }
  void on_alignCenterRight_clicked() { setAlign(1, 0); }
  void on_alignBottomLeft_clicked() { setAlign(-1, 1); }
  void on_alignBottomCenter_clicked() { setAlign(0, 1); }
  void on_alignBottomRight_clicked() { setAlign(1, 1); }

  void on_widthEdit_valueChanged(int) { sizeChanged(); }
  void on_heightEdit_valueChanged(int) { sizeChanged(); }

  void on_offsetXEdit_valueChanged(int) { offsetChanged(); }
  void on_offsetYEdit_valueChanged(int) { offsetChanged(); }

private:
  //! Updates the align
  void setAlign(int x, int y);

  //! Size changed
  void sizeChanged();

  //! Size changed
  void offsetChanged();

  //! Updates the align button
  void updateAlignButton();

  //! Realigns the map
  void realignMap();

  //! Updates the preview scene
  void updatePreviewScene();

  //! The scene
  QGraphicsScene *mScene;

  //! The level
  Maker::GameLevelAsset *mLevel;

  //! The level
  Maker::GameTilemapAsset *mTilemap;

  //! The Ui
  Ui::ResizeMapDialog *mUi;

  //! The current align
  int mAlignX = 0;

  //! The current align
  int mAlignY = 0;

  //! Whether a custom align is enabled
  bool mHasCustomAlign = false;

  //! Whether the align should be protected
  bool mProtectAlign = true;

  //! Graphics frame
  QGraphicsRectItem *mNewSizeFrame;

  //! Graphics frame
  QGraphicsPixmapItem *mPixmap;
};
