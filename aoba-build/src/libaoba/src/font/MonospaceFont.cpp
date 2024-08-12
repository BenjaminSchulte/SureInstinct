#include <QString>
#include <aoba/font/MonospaceFont.hpp>
#include <aoba/font/FontRegistry.hpp>
#include <aoba/image/QImageAccessor.hpp>
#include <aoba/image/SnesPaletteImage.hpp>
#include <aoba/palette/SnesPalette.hpp>
#include <aoba/project/Project.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <fma/symbol/ConstantNumber.hpp>
#include <fma/symbol/CalculatedNumber.hpp>
#include <yaml-cpp/yaml.h>

using namespace Aoba;

// -----------------------------------------------------------------------------
QVector<FontInterface*> MonospaceFontAssetSet::allFonts() const {
  QVector<FontInterface*> result;
  for (auto *item : mItems) {
    result.push_back(item);
  }
  return result;
}


// -----------------------------------------------------------------------------
MonospaceFont::MonospaceFont(MonospaceFontAssetSet *set, const QString &id, const QDir &dir)
  : Asset<MonospaceFontAssetSet>(set, id)
  , mPath(dir)
{
  set->add(this);
}

// -----------------------------------------------------------------------------
bool MonospaceFont::reload() {
  YAML::Node config;
  try {
    config = YAML::LoadFile(mPath.absoluteFilePath("font.yml").toStdString().c_str());
  } catch(YAML::BadFile&) {
    Aoba::log::error("Failed to load font YML");
    return false;
  }

  QString allLetters = config["letters"].as<std::string>().c_str();

  QImagePtr paletteImage(QImageAccessor::loadImage(mPath.filePath("palette.png")));
  QImagePtr tilesImage(QImageAccessor::loadImage(mPath.filePath("font.png")));
  if (!tilesImage || !paletteImage) {
    return false;
  }

  SnesPalettePtr palette(SnesPalette::fromImage(paletteImage.get()));
  if (!palette) {
    return false;
  }
  
  ImageAccessorPtr tiles(QImageAccessor(tilesImage).convertToPaletteImage(palette->subPalette(0, 4, true)));
  ImageList letters(tiles->split(QSize(mAssetSet->letterWidth(), mAssetSet->letterHeight())));

  int numLetters = allLetters.size();
  if (numLetters > letters.length()) {
    Aoba::log::warn("Font.yml defines more letters than provided in image. " + QString::number(numLetters) + " expected, image provides " + QString::number(letters.length()));
    return false;
  }

  for (int imageIndex=0; imageIndex<numLetters; imageIndex++) {
    uint c = allLetters[imageIndex].unicode();
    mLetterImages.insert(c, letters.at(imageIndex));
    //addSupportedLetter(c);
  }

  return true;
}

// -----------------------------------------------------------------------------
QString MonospaceFont::fontIndexSymbolName() const {
  return "__asset_monospace_font_" + mId;
}

// -----------------------------------------------------------------------------
bool MonospaceFont::build() {
  ImageList generatedLetters;
  QVector<int> letterImageIndex;

  for (uint32_t letter : mLetters.keys()) {
    if (!mLetterImages.contains(letter)) {
      Aoba::log::warn("Skipping unsupported letter " + QString(QChar(letter)) + " in font " + mId);
      continue;
    }

    int imageIndex = generatedLetters.length();
    generatedLetters.push_back(mLetterImages[letter]);
    int index = assetSet()->project()->fonts()->letters().indexOf(QChar(letter));
    if (index == -1) {
      Aoba::log::warn("Requested to generate letter " + QString(QChar(letter)) + " which is not present in font letter registry");
      return false;
    }

    while (letterImageIndex.length() <= index) {
      letterImageIndex.push_back(-1);
    }

    letterImageIndex[index] = imageIndex * 8 * 2;
  }


  QString imageDataSymbol = fontIndexSymbolName() + "_image";
  FMA::linker::LinkerBlock *fontData = assetSet()->assetLinkerObject().createLinkerBlock(fontIndexSymbolName());
  for (int letter : letterImageIndex) {
    FMA::symbol::ReferencePtr baseImage(new FMA::symbol::SymbolReference(imageDataSymbol.toStdString()));
    FMA::symbol::ReferencePtr offset(new FMA::symbol::ConstantNumber(letter));
    FMA::symbol::ReferencePtr ref(new FMA::symbol::CalculatedNumber(baseImage, FMA::symbol::CalculatedNumber::ADD, offset));
    fontData->write(ref, 2);
  }

  SnesPaletteImage image(generatedLetters.join()->toPaletteImage());
  QByteArray imageData(image.compile(mAssetSet->letterWidth(), mAssetSet->letterHeight(), 0));
  fontData->symbol(imageDataSymbol.toStdString());
  fontData->write(imageData.data(), imageData.size());

  return true;
}

// -----------------------------------------------------------------------------
QStringList MonospaceFont::getFmaObjectFiles() const {
  return mAssetSet->assetLinkerObject().getFmaObjectFiles();
}

// -----------------------------------------------------------------------------
QString MonospaceFont::getFmaCode() const {
  QStringList fma;
  fma << "module MonospaceFont";
  fma << "extern " + fontIndexSymbolName();
  fma << mId + "=" + fontIndexSymbolName();
  fma << "end";
  return fma.join('\n');
}
