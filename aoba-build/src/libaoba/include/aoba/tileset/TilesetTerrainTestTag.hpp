#pragma once

#include <QVector>
#include "TilesetTerrainTest.hpp"

namespace Aoba {

class TilesetTerrainTestTag : public TilesetTerrainTest {
public:
  //! Constructor
  TilesetTerrainTestTag(TilesetTerrainRule *rule) : TilesetTerrainTest(rule) {}

  //! Returns the type
  static QString TypeId() { return "tag"; }

  //! Returns the type
  QString typeId() const override { return TypeId(); }

  //! Adds a tag
  inline void setTags(const QVector<int> &tags) { mTags = tags; }

  //! Adds a tag
  inline void addTag(int tag) { mTags.push_back(tag); }

  //! Returns all tags
  inline const QVector<int> &tags() const { return mTags; }
  
  //! Applies the terrain
  bool test(TilemapTerrainLayer*, TilemapTerrainBuildContext &context, const QPoint &pos) const override;

  //! Loads the terrain
  bool load(const YAML::Node &) override;

  //! Saves the terrain
  bool save(YAML::Emitter &) const override;

private:
  //! List of all tags to test
  QVector<int> mTags;

  //! List of all tags to test
  QVector<int> mNotTags;
};

}