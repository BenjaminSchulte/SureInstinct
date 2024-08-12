#include <QDebug>
#include <aoba/log/Log.hpp>
#include <aoba/tileset/TilesetTerrain.hpp>
#include <aoba/tileset/TilesetTerrainLayer.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
TilesetTerrainLayer::TilesetTerrainLayer(TilesetTerrain *terrain, const QString &id, const QString &name)
  : mTerrain(terrain)
  , mId(id)
  , mName(name)
{
}

// -----------------------------------------------------------------------------
bool TilesetTerrainLayer::load(const YAML::Node &node) {
  mName = Yaml::asString(node["name"], mId);

  QString layerId(Yaml::asString(node["default_tileset_layer"]));
  TilesetAssetSetLayer *layerConfig = mTerrain->tileset()->assetSet()->getLayer(layerId);
  if (!layerConfig) {
    Aoba::log::warn("Unable to get layer " + layerId + " on tileset " + mTerrain->tileset()->id());
    return false;
  }

  mEditorMirrorX = Yaml::asBool(node["editor_mirror_x"]);
  mEditorMirrorY = Yaml::asBool(node["editor_mirror_y"]);

  mDefaultLayer = mTerrain->tileset()->layer(layerConfig);

  return true;
}

// -----------------------------------------------------------------------------
bool TilesetTerrainLayer::save(YAML::Emitter &) const {
  Aoba::log::debug("TODO: SAVE TilesetTerrainLayer");
  return false;
}
