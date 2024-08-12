#include <aoba/log/Log.hpp>
#include <QCryptographicHash>
#include <QImage>
#include <QFile>
#include <fma/Log.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <aoba/compress/AbstractWriter.hpp>
#include <aoba/image/ImageAccessor.hpp>
#include <aoba/project/Project.hpp>
#include <aoba/palette/SnesPalette.hpp>
#include <aoba/palette/Palette.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
QString PaletteAssetSet::hash() const {
  return QString::number(mNumColors, 16) + "_" +
    QString::number(mMaxColors, 16) + "_" +
    QString::number(mRomDataBank, 16) + "_" +
    QString::number(mColorOffset, 16) + "_" +
    QString::number(mTransparent, 16);
}


// -----------------------------------------------------------------------------
Palette::Palette(PaletteAssetSet *set, const QString &id, const QString &dir, int row)
  : Asset(set, id)
  , mFile(dir)
  , mRow(row)
{
  set->add(this);
}

// -----------------------------------------------------------------------------
bool Palette::reload() {
  QImagePtr image(new QImage());
  QIODevicePtr file(new QFile(mFile));
  if (!file->open(QIODevice::ReadOnly)) {
    Aoba::log::error("Unable to open palette file: " + mFile);
    return false;
  }

  mPalette = SnesPalette::fromFile(file, "png", mRow, mRow, mAssetSet->maxNumColors());
  if (!mPalette) {
    Aoba::log::error("Unable to load palette image from: " + mFile);
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
QString Palette::hash() const {
  return QString(QCryptographicHash::hash(mPalette->hash().toLatin1(), QCryptographicHash::Md5).toHex());
}

// -----------------------------------------------------------------------------
QString Palette::assetSymbolName() {
  if (!mAssetSymbolName.isEmpty()) {
    return mAssetSymbolName;
  }

  if (!mIsLoaded) {
    mAssetSymbolName = "__palette_asset_" + mId;
  } else {
    mAssetSymbolName = "__palette_" + mAssetSet->hash() + "_" + hash();
  }

  return mAssetSymbolName;
}

// -----------------------------------------------------------------------------
bool Palette::build() {
  if (mAssetSet->palettesBuilt().contains(assetSymbolName())) {
    return true;
  }

  mAssetSet->addBuiltPalette(assetSymbolName());

  double gammaCorrection = 1.0;
  const auto &properties = mAssetSet->project()->properties();
  if (properties.contains("com.aobastudio.aobabuild.gamma_correction")) {
    gammaCorrection = properties["com.aobastudio.aobabuild.gamma_correction"].toDouble();
  }

  uint16_t numColors = mPalette->numColors();
  uint32_t bytesPerColor = 2;
  
  QByteArray buffer;
  buffer.resize(numColors * bytesPerColor);
  char *colorBufferPtr = buffer.data();
  
  for (uint16_t i=0; i<numColors; i++) {
    uint16_t color = mPalette->color(i).gammaCorrected(gammaCorrection).toSnesColor();
    colorBufferPtr[0] = color;
    colorBufferPtr[1] = color >> 8;
    colorBufferPtr += bytesPerColor;
  }

  AbstractWriter *writer = mAssetSet->project()->createCompressor(mAssetSet->compression());
  if (writer == nullptr) {
    Aoba::log::error("No compressor registered named " + mAssetSet->compression());
    return false;
  }
  if (!writer->writeArray(buffer) || !writer->finalize()) {
    Aoba::log::error("Unable to compress palette");
    delete writer;
    return false;
  }

  auto *block = mAssetSet->createLinkerBlock(assetSymbolName());
  block->write(writer->buffer().data(), writer->buffer().size());

  return true;
}

// -----------------------------------------------------------------------------
QStringList Palette::getFmaObjectFiles() const {
  return mAssetSet->assetLinkerObject().getFmaObjectFiles();
}

// -----------------------------------------------------------------------------
QString Palette::getFmaCode() const {
  return "";
}
