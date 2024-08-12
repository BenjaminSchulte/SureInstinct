#pragma once

#include <QString>
#include <QVector>
#include <QSharedPointer>
#include "../asset/Asset.hpp"
#include "../asset/LinkedAssetSet.hpp"

namespace Sft {
class Sample;
}

namespace Aoba {
class Sample;

class SampleAssetSet : public LinkedAssetSet<Sample> {
public:
  //! Constructor
  SampleAssetSet(Project *project)
    : LinkedAssetSet<Sample>("sample", project)
  {}

  //! Adds a bytes per frame option
  void addStreamBytesPerFrameOption(uint32_t bpf) { mStreamBytesPerFrame.push_back(bpf & 0xFFFFFFFC); }

  //! Returns all possible bytes per frame
  inline const QVector<uint32_t> &streamBytesPerFrame() const { return mStreamBytesPerFrame; }

protected:
  //! List of all possible bytes per frame
  QVector<uint32_t> mStreamBytesPerFrame;
};

class Sample : public Asset<SampleAssetSet> {
public:
  //! Constructor
  Sample(SampleAssetSet *set, const QString &id, const QString &file);

  //! Deconstructor
  ~Sample();

  //! Returns the symbol name for this sample
  QString symbolName() const;

  //! Loads the asset
  bool reload() override;

  //! Builds the asset
  bool build() override;

  //! Returns the necessary FMA code
  QString getFmaCode() const override;

  //! REturns the FMA objects
  QStringList getFmaObjectFiles() const override;

  //! Returns the SFT sample
  inline Sft::Sample *sample() const { return mSample; }

  //! Returns the size in bytes
  uint16_t sizeInBytes() const;

  //! Returns the size in bytes
  uint16_t numBrrChunks() const;
  
  //! Returns whether this sample is streamable
  inline bool streamable() const { return mStreamable; }

  //! Returns the bytes per frame
  inline uint32_t bytesPerFrame() const { return mBytesPerFrame; } 

  //! Returns the file
  inline const QString &file() const { return mFile; }

protected:
  //! Finds the closest bytes per frame
  int findClosestBytesPerFrame(int bpf) const;

  //! Loads the asset
  bool reloadFromYml();

  //! The containing file
  QString mFile;

  //! The BRR sample
  Sft::Sample *mSample = nullptr;

  //! Whether this sample is streamable
  bool mStreamable = false;

  //! Bytes per frame (only for streamable samples)
  uint32_t mBytesPerFrame = 0;
};

}