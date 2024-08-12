#pragma once

#include <QString>
#include <QVariant>
#include <QMap>

namespace Aoba {
class FontRegistry;
class TextRegistry;
class AssetByteListLinker;
class AbstractAssetSet;
class Project;
class AbstractWriter;
class RamVariableRegistry;

using CompressorClassConstructor = AbstractWriter*(*)(Project*);

enum class Arch {
  UNKNOWN = 0,
  NTSC,
  PAL
};

class Project {
public:
  //! Constructor
  Project();

  //! Deconstructor
  virtual ~Project();

  //! Sets the arch
  void setArch(Arch arch) { mArch = arch; }

  //! Returns the architecture
  inline Arch arch() const { return mArch; }
  
  //! Returns whether SA1 is enabled
  bool sa1enabled() const;

  //! Returns a property
  inline const QMap<QString, QVariant> &properties() const { return mProperties; }

  //! Returns a property
  inline QMap<QString, QVariant> &properties() { return mProperties; }

  //! Returns the frames per second
  int framesPerSecond() const;

  //! Returns the object file path
  virtual QString objFile(const QString &type) const = 0;

  //! Returns an asset type set
  virtual AbstractAssetSet *assetSet(const QString &id) const = 0;

  //! Returns the internal script symbol for a script
  virtual QString getScriptSymbolName(const QString &func) const = 0;

  //! Looks up a generated table
  virtual void lookupTable(const QString &type, const QString &id) = 0;

  //! Looks up a generated table
  virtual int lookupTable(const QString &type, const QString &id, const QString &key, int value) = 0;

  //! Returns the font registry
  inline FontRegistry *fonts() const { return mFonts; }

  //! Returns the font registry
  inline TextRegistry *texts() const { return mTexts; }

  //! Returns a helper to create asset byte lists
  inline AssetByteListLinker *assetByteListLinker8() const { return mAssetByteListLinker8; }

  //! Returns a helper to create asset byte lists
  inline AssetByteListLinker *assetByteListLinker16() const { return mAssetByteListLinker16; }

  //! Returns a unique number
  inline uint32_t uniqueNumber() { return mNextUniqueNumber++; }

  //! Registers a compressor
  inline void registerCompressor(const QString &name, const CompressorClassConstructor &constr) {
    mCompressors.insert(name, constr);
  }

  //! Creates a new compressor
  inline AbstractWriter *createCompressor(const QString &compressor) {
    if (!mCompressors.contains(compressor)) {
      return nullptr;
    }
    return mCompressors[compressor](this);
  }

protected:
  //! All known compressors
  QMap<QString, CompressorClassConstructor> mCompressors;

  //! Map containing all properties
  QMap<QString, QVariant> mProperties;

  //! Font registry
  FontRegistry *mFonts;

  //! Font registry
  TextRegistry *mTexts;

  //! Asset byte list linker
  AssetByteListLinker *mAssetByteListLinker8;

  //! Asset byte list linker
  AssetByteListLinker *mAssetByteListLinker16;

  //! Unique number
  uint32_t mNextUniqueNumber = 1;

  //! Arch
  Arch mArch = Arch::UNKNOWN;

  //! The game code
  QString mGameCode = "AOBA";

  //! The game title
  QString mGameTitle = "AOBA game";

  //! The game destination
  QString mGameDestination = "japan";

  //! Whether the SA1 is enabled
  bool mSa1Enabled = false;
};

}
