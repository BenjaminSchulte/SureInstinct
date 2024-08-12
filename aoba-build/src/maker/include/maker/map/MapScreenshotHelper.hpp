#pragma once

#include <QImage>

namespace Maker {
class GameLevelAsset;
}

namespace Aoba {
class Level;
class Tilemap;
class TilemapAssetSetLayer;
}

struct MapScreenshotConfiguration {
  //! Configuration
  MapScreenshotConfiguration() {}

  //! Constructor
  MapScreenshotConfiguration(bool front, bool back, bool main, bool sub)
    : mFront(front), mBack(back), mMain(main), mSub(sub) {}

  //! Returns whether front rendering is active (priority)
  inline bool front() const { return mFront; }

  //! Returns whether back rendering is active (non-priority)
  inline bool back() const { return mBack; }

  //! Returns whether this layer in rendered on main screen
  inline bool main() const { return mMain; }

  //! Returns whether this layer in rendered on sub screen
  inline bool sub() const { return mSub; }

private:
  //! Render priority?
  bool mFront = true;

  //! Render non-priority?
  bool mBack = true;

  //! Render main screen?
  bool mMain = true;

  //! Render back screen?
  bool mSub = true;
};

class MapScreenshotHelper {
public:
  //! Takes a screenshot
  QImage screenshot(Maker::GameLevelAsset *level) const;

  //! Renders a layer
  void renderLayer(Aoba::Tilemap *tilemap, Aoba::TilemapAssetSetLayer *layer, const QString &switchId, const QRect &tiles, QPixmap &main, QPixmap &sub, const MapScreenshotConfiguration &config) const;
};