#pragma once

#include <QDir>
#include "../image/ImageAccessor.hpp"
#include "../asset/Asset.hpp"
#include "../asset/LinkedAssetSet.hpp"
#include "./Font.hpp"

namespace Aoba {
class MonospaceFont;

class MonospaceFontAssetSet : public LinkedAssetSet<MonospaceFont>, public FontSetInterface {
public:
  //! Constructor
  MonospaceFontAssetSet(Project *project, const QString &id, uint8_t letterWidth, uint8_t letterHeight)
    : LinkedAssetSet<MonospaceFont>("monospace_font", project)
    , mId(id)
    , mLetterWidth(letterWidth)
    , mLetterHeight(letterHeight)
  {}

  //! Returns the set ID
  QString fontSetId() const override { return mId; }

  //! Returns the letter width
  inline uint8_t letterWidth() const { return mLetterWidth; }

  //! Returns the letter height
  inline uint8_t letterHeight() const { return mLetterHeight; }

  //! Returns all fonts
  QVector<FontInterface*> allFonts() const override;

protected:
  //! The ID
  QString mId;

  //! The letter width
  uint8_t mLetterWidth;

  //! The letter height
  uint8_t mLetterHeight;
};

class MonospaceFont : public Asset<MonospaceFontAssetSet>, public FontInterface {
public:
  //! Constructor
  MonospaceFont(MonospaceFontAssetSet *set, const QString &id, const QDir &dir);

  //! Returns the path
  inline const QDir &path() const { return mPath; }

  //! Returns the set ID
  QString fontId() const override { return mId; }

  //! Loads the asset
  bool reload() override;

  //! Builds the asset
  bool build() override;

  //! Returns the necessary FMA code
  QString getFmaCode() const override;

  //! Returns additional object files to include
  QStringList getFmaObjectFiles() const override;

  //! Returns the font index symbol name
  QString fontIndexSymbolName() const;

  //! Returns the font set
  FontSetInterface *fontSet() const override {
    return mAssetSet;
  }

protected:
  //! The containing file
  QDir mPath;

  //! A map containing all letters and their image
  QMap<uint, ImageAccessorPtr> mLetterImages;
};

}