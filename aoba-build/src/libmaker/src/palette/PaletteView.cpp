#include <QGraphicsRectItem>
#include <maker/palette/PaletteView.hpp>

using namespace Aoba;
using namespace Maker;

// -----------------------------------------------------------------------------
PaletteView::PaletteView(QWidget *parent)
  : QGraphicsView(parent)
  , mScene(new QGraphicsScene)
{
  setScene(mScene);
  createSceneMembers();
}
  
// -----------------------------------------------------------------------------
PaletteView::~PaletteView() {
  delete mScene;
}

// -----------------------------------------------------------------------------
void PaletteView::createSceneMembers() {
  mScene->clear();
  mColors.clear();
  
  int numRows = mPalette ? mPalette->numColors() / 16 : 1;

  for (int i=0, y=0; y<numRows; y++) {
    for (int x=0; x<16; x++, i++) {
      mColors.push_back(mScene->addRect(x * 16, y *16, 16, 16));
    }
  }
  updatePalette();
}

// -----------------------------------------------------------------------------
void PaletteView::setPalette(const Aoba::SnesPalettePtr &palette) {
  mPalette = palette;

  createSceneMembers();
  updatePalette();
}

// -----------------------------------------------------------------------------
void PaletteView::updatePalette() {
  if (!mPalette) {
    return;
  }

  uint16_t numColors = qMin<uint16_t>(256, mPalette->numColors());

  for (uint16_t i=0; i<numColors; i++) {
    mColors[i]->setBrush(QBrush(mPalette->color(i).toQColor()));
  }
}

// -----------------------------------------------------------------------------
