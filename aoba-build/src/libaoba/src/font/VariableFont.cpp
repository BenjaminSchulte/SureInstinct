#include <QString>
#include <aoba/font/VariableFont.hpp>
#include <aoba/font/VariableFontLoader.hpp>
#include <aoba/font/VariableFontBuilder.hpp>
#include <aoba/font/FontRegistry.hpp>
#include <aoba/project/Project.hpp>
#include <yaml-cpp/yaml.h>

using namespace Aoba;

// -----------------------------------------------------------------------------
QVector<FontInterface*> VariableFontAssetSet::allFonts() const {
  QVector<FontInterface*> result;
  for (auto *item : mItems) {
    result.push_back(item);
  }
  return result;
}


// -----------------------------------------------------------------------------
VariableFont::VariableFont(VariableFontAssetSet *set, const QString &id, const QDir &dir)
  : Asset<VariableFontAssetSet>(set, id)
  , mPath(dir)
{
  set->add(this);
}

// -----------------------------------------------------------------------------
bool VariableFont::reload() {
  VariableFontLoader loader(this);
  return loader.load(mPath);
}

// -----------------------------------------------------------------------------
QString VariableFont::fontIndexSymbolName() const {
  return "__asset_variablespace_font_" + mId;
}

// -----------------------------------------------------------------------------
bool VariableFont::build() {
  VariableFontBuilder builder(this);

  for (uint32_t letter : mLetters.keys()) {
    if (!mGlyphs.contains(letter)) {
      //Aoba::log::warn("Skipping unsupported letter " + QString(QChar(letter)) + " in font " + mId);
      continue;
    }
    
    //Aoba::log::info("Generate " + QString(QChar(letter)) + " in font " + mId);
    builder.add(mGlyphs[letter]);
  }

  FMA::linker::LinkerBlock *block = mAssetSet->assetLinkerObject().createLinkerBlock(fontIndexSymbolName());
  builder.build(block);

  return true;
}

// -----------------------------------------------------------------------------
QStringList VariableFont::getFmaObjectFiles() const {
  return mAssetSet->assetLinkerObject().getFmaObjectFiles();
}

// -----------------------------------------------------------------------------
QString VariableFont::getFmaCode() const {
  QStringList fma;
  fma << "module VariableFont";
  fma << "extern " + fontIndexSymbolName();
  fma << mId + "=" + fontIndexSymbolName();
  fma << "end";
  return fma.join('\n');
}
