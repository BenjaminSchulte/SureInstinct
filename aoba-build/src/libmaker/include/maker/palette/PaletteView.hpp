#pragma once

#include <QGraphicsView>
#include <QVector>
#include <aoba/palette/SnesPalette.hpp>

namespace Maker {

class PaletteView : public QGraphicsView {
  Q_OBJECT

public:
  //! Constructor
  PaletteView(QWidget *parent = nullptr);
  
  //! Deconstructor
  ~PaletteView();

public slots:
  //! Sets the palette
  void setPalette(const Aoba::SnesPalettePtr &palette);

  //! The palette has been updated
  void updatePalette();

private:
  //! Recreates the scene
  void createSceneMembers();

  //! The scene
  QGraphicsScene *mScene;

  //! List of all color elements in the scene
  QVector<QGraphicsRectItem*> mColors;

  //! The current palette
  Aoba::SnesPalettePtr mPalette;
};

}