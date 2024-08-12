#include <QDebug>
#include <aoba/log/Log.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <aoba/tileset/TilesetAnimationModule.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <aoba/palette/Palette.hpp>
#include <aoba/image/PaletteImageAccessor.hpp>
#include <aoba/palette/PaletteGroup.hpp>
#include <aoba/image/SnesPaletteImage.hpp>
#include <aoba/palette/SnesPalette.hpp>
#include <fma/symbol/ConstantNumber.hpp>
#include <fma/symbol/CalculatedNumber.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool TilesetAnimationModuleInstance::loadTile(TilesetLayer *, TilesetTile *, CsvReader &) {
  return true;
}

// -----------------------------------------------------------------------------
QString TilesetAnimationModuleInstance::hashTile(TilesetLayer *, const TilesetTile *) {
  return "";
}

// -----------------------------------------------------------------------------
void TilesetAnimationModuleInstance::writeCsvHeader(CsvWriter &) const {
}

// -----------------------------------------------------------------------------
void TilesetAnimationModuleInstance::writeTile(const TilesetLayer *, const TilesetTile *, CsvWriter &) const {
}

// -----------------------------------------------------------------------------
void TilesetAnimationModuleInstance::replaceOldTiles(const QMap<const Aoba::TilesetTile*, Aoba::TilesetTile*> &) {
}

// -----------------------------------------------------------------------------
bool TilesetAnimationModuleInstance::build() {
  if (mTileset->generatedAnimations().isEmpty()) {
    mTileset->headerBlock()->writeNumber(0, 3);
    return true;
  }

  QMapIterator<QString, TilesetBuilderAnimation> it(mTileset->generatedAnimations());

  QString symbolName = mTileset->binaryFileName("_ANIMATION");
  FMA::linker::LinkerBlock *animationList = mTileset->assetSet()->assetLinkerObject().createLinkerBlock(symbolName, mTileset->assetSet()->animationRomBank());

  while (it.hasNext()) {
    it.next();
    const auto &animation = it.value();

    TilesetAnimationGroup *group = animation.group;
    PaletteAssetSet *paletteSet = group->tilesConfig()->paletteGroupSet()->paletteSet();

    // Generates all images for this frame
    int bytesPerBgTile = paletteSet->numSnesColors() / 4 * 8;
    int bufferTarget = (bytesPerBgTile * animation.firstIndex) >> 1;
    int bufferSize = (animation.lastIndex - animation.firstIndex + 1) * bytesPerBgTile;

    QString animationSymbol = symbolName + "_" + it.key();
    QStringList imageSymbolNames = buildAnimationFrameImages(animationSymbol + "_IMAGE", animation);

    FMA::linker::LinkerBlock *groupAnimationScript = mTileset->assetSet()->assetLinkerObject().createLinkerBlock(animationSymbol, mTileset->assetSet()->animationRomBank());
    animationList->writeNumber(bufferTarget, 2);
    animationList->writeNumber(bufferSize, 2);
    animationList->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(animationSymbol.toStdString())), 2);

    for (int frameIndex=0; frameIndex<group->numFrames(); frameIndex++) {
      const TilesetAnimationGroupFrame &frame(group->frame(frameIndex));

      if (frame.imageIndex() >= 0) {
        FMA::symbol::SymbolReferencePtr symbol(new FMA::symbol::SymbolReference(imageSymbolNames[frame.imageIndex()].toStdString()));
        FMA::symbol::ReferencePtr shiftNumber(new FMA::symbol::ConstantNumber(16));
        FMA::symbol::ReferencePtr bank(new FMA::symbol::CalculatedNumber(symbol, FMA::symbol::CalculatedNumber::RSHIFT, shiftNumber));
        groupAnimationScript->write(bank, 1);
        groupAnimationScript->write(symbol, 2);
      }

      int delay = frame.delay();
      while (delay > 0) {
        int curDelay = delay >= 128 ? 127 : delay;
        delay -= curDelay;
        groupAnimationScript->writeNumber(curDelay, 1);
      }

      switch (frame.command()) {
        case TilesetAnimationGroupFrame::USER_COMMAND:
          groupAnimationScript->writeNumber(frame.commandParameter(), 1);
          break;

        case TilesetAnimationGroupFrame::NO_COMMAND:
          break;
      }
    }

    groupAnimationScript->writeNumber(0x80, 1);
    groupAnimationScript->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(animationSymbol.toStdString())), 2);
  }

  mTileset->headerBlock()->writeNumber(mTileset->generatedAnimations().size(), 1);
  mTileset->headerBlock()->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(symbolName.toStdString())), 2);

  return true;
}

// -----------------------------------------------------------------------------
QStringList TilesetAnimationModuleInstance::buildAnimationFrameImages(const QString &symbolPrefix, const TilesetBuilderAnimation &animation) {
  QStringList result;

  TilesetAnimationGroup *group = animation.group;
  PaletteAssetSet *paletteSet = group->tilesConfig()->paletteGroupSet()->paletteSet();
  uint8_t colorOffset = paletteSet->colorOffset();

  for (int set=0; set<group->numImages(); set++) {
    ImageList list;

    for (int index=animation.firstIndex; index<=animation.lastIndex; index++) {
      list.push_back(ImageAccessorPtr(new PaletteImageAccessor(group->image(set).tiles()[animation.indices[index]])));
    }

    QString imageSymbolName = mTileset->binaryFileName(symbolPrefix + QString::number(set));
    FMA::linker::LinkerBlock *block = mTileset->assetSet()->assetLinkerObject().createLinkerBlock(imageSymbolName);
    SnesPaletteImage frameImage(list.join()->toPaletteImage());
    QByteArray buffer(frameImage.compile(mTileset->assetSet()->bgTileWidth(), mTileset->assetSet()->bgTileHeight(), colorOffset));
    block->write(buffer.data(), buffer.size());

    result.push_back(imageSymbolName);
  }

  return result;
}

// -----------------------------------------------------------------------------
QString TilesetAnimationModuleInstance::getFmaCode() const {
  Aoba::log::error("IS THIS BEING CALLED??????????????");
  return "";
}
