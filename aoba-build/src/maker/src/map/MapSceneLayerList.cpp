#include <aoba/log/Log.hpp>

#include <aoba/level/LevelTilemapComponent.hpp>
#include <aoba/level/LevelNpcComponent.hpp>
#include <aoba/tilemap/TilemapTerrain.hpp>
#include <aoba/tilemap/Tilemap.hpp>
#include <aoba/tileset/TilesetTerrainLayer.hpp>
#include <aoba/tileset/TilesetTerrain.hpp>
#include <aoba/tileset/Tileset.hpp>

#include <rpg/tileset/TilesetCollisionModule.hpp>

#include <maker/game/GameTilesetAssetType.hpp>
#include <maker/game/GameTilemapAssetType.hpp>
#include <maker/game/GameLevelAssetType.hpp>

#include <maker/map/MapScene.hpp>
#include <maker/map/MapSceneLayer.hpp>
#include <maker/map/MapSceneLayerList.hpp>
#include <maker/map/TilemapMapSceneLayer.hpp>
#include <maker/map/ExitMapSceneLayer.hpp>
#include <maker/map/NpcMapSceneLayer.hpp>
#include <maker/map/CollisionMapSceneLayer.hpp>
#include <maker/map/MoveEventMapSceneLayer.hpp>
#include <maker/map/TerrainMapSceneLayer.hpp>
#include <maker/map/EnterEventMapSceneLayer.hpp>

// -----------------------------------------------------------------------------
MapSceneLayerList::~MapSceneLayerList() {
  clear();
}

// -----------------------------------------------------------------------------
void MapSceneLayerList::clear() {
  QVector<MapSceneLayer*> copy = mLayers;
  mLayers.clear();

  for (MapSceneLayer *layer : copy) {
    delete layer;
  }
}

// -----------------------------------------------------------------------------
void MapSceneLayerList::addManaged(MapSceneLayer *layer) {
  connect(layer, SIGNAL(visibleChanged()), this, SIGNAL(visiblityChanged()));
  mLayers.push_back(layer);
}

// -----------------------------------------------------------------------------
void MapSceneLayerList::createLayersFromMap(MapScene *scene) {
  clear();

  createTerrainLayersFromMap(scene, "");
  createTileLayersFromMap(scene, "");
  for (const QString &id : scene->tilemap()->asset()->tilemapSwitches().keys()) {
    createTerrainLayersFromMap(scene, id);
    createTileLayersFromMap(scene, id);
  }
  createCollisionLayersFromMap(scene);
  createComponentLayersFromMap(scene);
}

// -----------------------------------------------------------------------------
void MapSceneLayerList::createTerrainLayersFromMap(MapScene *scene, const QString &switchId) {
  Aoba::TilesetTerrain *terrain = scene->tileset()->asset()->terrain();
  if (!terrain) {
    return;
  }

  Aoba::TilemapTerrain *tilemapTerrain = scene->tilemap()->asset()->tilemapSwitch(switchId)->requireTerrain();
  QString prefix;
  if (!switchId.isEmpty()) {
    prefix = "[" + switchId + "] ";
  }

  QMapIterator<QString, Aoba::TilesetTerrainLayer*> it(terrain->layers());
  while (it.hasNext()) {
    it.next();

    tilemapTerrain->getOrCreateLayer(it.key());
    addManaged(new TerrainMapSceneLayer(prefix + tr("Terrain: %1").arg(it.value()->name()), it.key(), switchId));
  }
}

// -----------------------------------------------------------------------------
void MapSceneLayerList::createTileLayersFromMap(MapScene *scene, const QString &switchId) {
  QString prefix;
  if (!switchId.isEmpty()) {
    prefix = "[" + switchId + "] ";
  }

  for (const Aoba::LevelTilemapRenderOrder &layer : scene->level()->tilemapRenderOrder()) {
    addManaged(new TilemapMapSceneLayer(prefix + layer.name, layer.layer, switchId, layer.renderPriority, layer.renderNonPriority, layer.main, layer.sub));
  }
}

// -----------------------------------------------------------------------------
void MapSceneLayerList::createCollisionLayersFromMap(MapScene *scene) {
  for (Aoba::TilesetAssetSetModule *mod : scene->tileset()->asset()->assetSet()->modules()) {
    Aoba::TilesetAssetSetModuleInstance *inst = mod->instanceFor(scene->tileset()->asset());

    if (inst->typeId() == Rpg::TilesetCollisionModuleInstance::TypeId()) {
      addManaged(new ExitMapSceneLayer(tr("Map Exits"), "", dynamic_cast<Rpg::TilesetCollisionModuleInstance*>(inst)));

      for (auto *layer : scene->tilemap()->asset()->assetSet()->layers()) {
        addManaged(new CollisionMapSceneLayer(tr("%1: Collision").arg(layer->name()), layer, "", dynamic_cast<Rpg::TilesetCollisionModuleInstance*>(inst)));
      }
      for (auto *layer : scene->tilemap()->asset()->assetSet()->layers()) {
        addManaged(new MoveEventMapSceneLayer(tr("%1: Move events").arg(layer->name()), layer, "", dynamic_cast<Rpg::TilesetCollisionModuleInstance*>(inst)));
      }
      for (auto *layer : scene->tilemap()->asset()->assetSet()->layers()) {
        addManaged(new EnterEventMapSceneLayer(tr("%1: Enter events").arg(layer->name()), layer, "", dynamic_cast<Rpg::TilesetCollisionModuleInstance*>(inst)));
      }
    } else {
      Aoba::log::warn("Skipping unsupported tileset module: " + inst->typeId());
    }
  }
}

// -----------------------------------------------------------------------------
void MapSceneLayerList::createComponentLayersFromMap(MapScene *scene) {
  for (Aoba::LevelComponent *comp : scene->level()->asset()->assetSet()->components()) {
    if (comp->typeId() == Aoba::LevelNpcComponent::TypeId()) {
      addManaged(new NpcMapSceneLayer(tr("Entities"), dynamic_cast<Aoba::LevelNpcComponent*>(comp)));
    }
  }
}
