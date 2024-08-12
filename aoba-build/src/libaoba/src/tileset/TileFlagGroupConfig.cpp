#include <QDebug>
#include <aoba/tileset/TileFlagGroupConfig.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
TileFlagGroupAssetSet::~TileFlagGroupAssetSet() {
  removeAllGenerators();
  removeAllParameters();
}

// -----------------------------------------------------------------------------
void TileFlagGroupAssetSet::removeAllGenerators() {
  for (TileFlagGroupGenerator *generator : mGenerators) {
    delete generator;
  }
  mGenerators.clear();
}

// -----------------------------------------------------------------------------
void TileFlagGroupAssetSet::removeAllParameters() {
  for (TileFlagGroupParameter *parameter : mParameters) {
    delete parameter;
  }
  mParameters.clear();
}

// -----------------------------------------------------------------------------
bool TileFlagGroupAssetSet::loadTile(TilesetLayer*, TilesetTile *, TileFlagGroupTile &ourTile, CsvReader &csv) const {
  QMapIterator<QString, TileFlagGroupParameter*> it(mParameters);
  while (it.hasNext()) {
    it.next();

    bool ok = true;
    ourTile.values[it.key()] = it.value()->loadTile(it.key(), csv, ok);
    if (!ok) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
QString TileFlagGroupAssetSet::hashTile(TilesetLayer*, const TilesetTile*, const TileFlagGroupTile &ourTile) const {
  QStringList hash;
  QMapIterator<QString, TileFlagGroupParameter*> it(mParameters);
  while (it.hasNext()) {
    it.next();

    hash << it.value()->hashTile(ourTile.values[it.key()]);
  }

  return hash.join('\t');
}

// -----------------------------------------------------------------------------
void TileFlagGroupAssetSet::writeCsvHeader(CsvWriter &csv) const {
  QMapIterator<QString, TileFlagGroupParameter*> it(mParameters);
  while (it.hasNext()) {
    it.next();
    it.value()->writeCsvHeader(it.key(), csv);
  }
}

// -----------------------------------------------------------------------------
void TileFlagGroupAssetSet::writeTile(const TilesetLayer*, const TilesetTile*, const TileFlagGroupTile &ourTile, CsvWriter &csv) const {
  QMapIterator<QString, TileFlagGroupParameter*> it(mParameters);
  while (it.hasNext()) {
    it.next();
    it.value()->writeTile(it.key(), ourTile.values[it.key()], csv);
  }
}

// -----------------------------------------------------------------------------
uint64_t TileFlagGroupAssetSet::generate(const TileFlagGroupTile &tile, bool &ok) const {
  uint64_t value = 0;
  for (TileFlagGroupGenerator *gen : mGenerators) {
    if (!gen->generate(tile, value)) {
      ok = false;
      break;
    }
  }
  return value;
}
