#pragma once

#include <QMap>
#include <QVariant>
#include <QSharedPointer>
#include "../asset/Asset.hpp"
#include "../asset/AssetSet.hpp"

namespace Aoba {
class TileFlagGroupAssetSet;
class TilesetTile;
class TilesetLayer;
class CsvReader;
class CsvWriter;

class TileFlagGroup : public Asset<TileFlagGroupAssetSet> {
public:
  //! Constructor
  TileFlagGroup(TileFlagGroupAssetSet *set, const QString &id) : Asset<TileFlagGroupAssetSet>(set, id) {}

  //! Deconstructor
  ~TileFlagGroup() {}
};


struct TileFlagGroupTile {
  QMap<QString, QVariant> values;
};

class TileFlagGroupGenerator {
public:
  //! Constructor
  TileFlagGroupGenerator() = default;

  //! Deconstructor
  virtual ~TileFlagGroupGenerator() = default;

  //! Sets the owner
  inline void setOwner(TileFlagGroupAssetSet *set) { mGroup = set; }

  //! Returns the owner
  inline TileFlagGroupAssetSet *owner() const { return mGroup; }

  //! Generates data
  virtual bool generate(const TileFlagGroupTile &, uint64_t &value) const = 0;

protected:
  //! The owner
  TileFlagGroupAssetSet *mGroup = nullptr;
};

class TileFlagGroupParameter {
public:
  //! Constructor
  TileFlagGroupParameter(const QString &name) : mName(name) {}

  //! Deconstructor
  virtual ~TileFlagGroupParameter() = default;

  //! Returns the name
  inline const QString &name() const { return mName; }

  //! Sets the owner
  inline void setOwner(TileFlagGroupAssetSet *set) { mGroup = set; }

  //! Returns the owner
  inline TileFlagGroupAssetSet *owner() const { return mGroup; }

  //! Writes the CSV header
  virtual void writeCsvHeader(const QString &id, CsvWriter &csv) const = 0;

  //! Writes the CSV data
  virtual void writeTile(const QString &id, const QVariant &, CsvWriter &csv) const = 0;

  //! Loads a tile
  virtual QVariant loadTile(const QString &id, CsvReader &csv, bool &ok) const = 0;

  //! Loads a tile
  virtual QString hashTile(const QVariant &) const = 0;

  //! Loads a tile
  virtual uint64_t numericValue(const QVariant &) const { return 0; }

protected:
  //! The name
  QString mName;

  //! The owner
  TileFlagGroupAssetSet *mGroup = nullptr;
};


class TileFlagGroupAssetSet : public AssetSet<TileFlagGroup> {
public:
  //! Constructor
  TileFlagGroupAssetSet(Project *project, const QString &id, uint8_t dataSize)
    : AssetSet<TileFlagGroup>(id, project)
    , mDataSize(dataSize)
  {}

  //! Deconstructor
  ~TileFlagGroupAssetSet();

  //! Removes all generators
  void removeAllGenerators();

  //! Removes all parameters
  void removeAllParameters();

  //! Adds a managed generator
  inline void addManagedGenerator(TileFlagGroupGenerator *generator) {
    mGenerators.push_back(generator);
    generator->setOwner(this);
  }

  //! Adds a managed generator
  inline void addManagedParameter(const QString &id, TileFlagGroupParameter *parameter) {
    mParameters.insert(id, parameter);
    parameter->setOwner(this);
  }

  //! Returns the data size
  inline uint8_t dataSize() const { return mDataSize; }

  //! Loads a tile
  bool loadTile(TilesetLayer*, TilesetTile*, TileFlagGroupTile &, CsvReader &) const;

  //! Hashs a tile
  QString hashTile(TilesetLayer*, const TilesetTile*, const TileFlagGroupTile &) const;

  //! Generates csv header
  void writeCsvHeader(CsvWriter &) const;

  //! Generates csv header
  void writeTile(const TilesetLayer*, const TilesetTile*, const TileFlagGroupTile &, CsvWriter &) const;

  //! Generates a tile
  uint64_t generate(const TileFlagGroupTile &, bool &ok) const;

  //! Returns a parameter
  inline TileFlagGroupParameter *parameter(const QString &id) const { return mParameters[id]; }

private:
  //! Data size in bits;
  uint8_t mDataSize;

  //! List of all generators
  QVector<TileFlagGroupGenerator*> mGenerators;

  //! List of all parameters
  QMap<QString, TileFlagGroupParameter*> mParameters;
};

}
