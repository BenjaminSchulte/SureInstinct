#include <aoba/log/Log.hpp>
#include <aoba/tileset/TilesetTerrain.hpp>
#include <aoba/tileset/TilesetTerrainType.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
TilesetTerrainType::TilesetTerrainType(TilesetTerrain *terrain, const QString &id, const QString &name, int iconTile, int index)
  : mTerrain(terrain)
  , mId(id)
  , mName(name)
  , mIconTile(iconTile)
  , mIndex(index)
{
}

// -----------------------------------------------------------------------------
void TilesetTerrainType::addTag(int tag) {
  if (mTags.contains(tag)) {
    return;
  }

  mTags.push_back(tag);
}

// -----------------------------------------------------------------------------
void TilesetTerrainType::addLayer(TilesetTerrainLayer *layer) {
  if (mLayers.contains(layer)) {
    return;
  }

  mLayers.push_back(layer);
}

// -----------------------------------------------------------------------------
bool TilesetTerrainType::load(const YAML::Node &node) {
  mName = Yaml::asString(node["name"], mId);
  mIconTile = Yaml::asInt(node["icon_tile"], 0);

  mLayers.clear();
  if (Yaml::isArray(node["layers"])) {
    for (const auto &layerNode : node["layers"]) {
      TilesetTerrainLayer *layer = mTerrain->layer(Yaml::asString(layerNode));
      if (!layer) {
        Aoba::log::warn("Unable to get layer " + Yaml::asString(layerNode) + " on tileset " + mTerrain->tileset()->id());
        return false;
      }

      mLayers.push_back(layer);
    }
  }

  mTags.clear();
  if (Yaml::isArray(node["tags"])) {
    for (const auto &tag : node["tags"]) {
      addTag(mTerrain->getTag(Yaml::asString(tag)));
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool TilesetTerrainType::save(YAML::Emitter &) const {
  Aoba::log::debug("TODO: SAVE TilesetTerrainType");
  return false;
}
