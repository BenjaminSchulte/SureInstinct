#pragma once 

#include "MapSceneLayer.hpp"

class MapSceneObjectLayer : public MapSceneLayer {
  Q_OBJECT

public:
  //! Constructor
  MapSceneObjectLayer(const QString &name)
    : MapSceneLayer(name)
  {
  }

  //! Returns the name of the object
  virtual QString objectType() const = 0;

  //! Returns whether this is an NPC layer
  bool isObjectLayer() const override { return true; }

  //! Whether the layer supports a tool
  bool supportsTool(MapEditTool tool) const override {
    switch (tool) {
      case MapEditTool::OBJECT: return true;
      default: return false;
    }
  }

  //! Returns the default tool
  MapEditTool fallbackTool() const override { return MapEditTool::OBJECT; }
};
