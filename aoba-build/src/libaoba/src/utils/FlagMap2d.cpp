#include <cstdlib>
#include <cstring>
#include <aoba/log/Log.hpp>
#include <QVector>
#include <aoba/utils/FlagMap2d.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
FlagMap2d::~FlagMap2d() {
  unload();
}

// -----------------------------------------------------------------------------
void FlagMap2d::unload() {
  for (uint32_t i=0; i<mNumFlagMaps; i++) {
    std::free(mFlagMap[i]);
  }
  std::free(mFlagMap);
  mFlagMap = nullptr;
  mNumFlagMaps = 0;
}

// -----------------------------------------------------------------------------
void FlagMap2d::clear() {
  for (uint32_t i=0; i<mNumFlagMaps; i++) {
    std::memset(mFlagMap[i], 0, mSize.width() * mSize.height() * sizeof(uint32_t));
  }
}

// -----------------------------------------------------------------------------
void FlagMap2d::resizeAndClear(const QSize &size) {
  unload();
  mSize = size;
}

// -----------------------------------------------------------------------------
void FlagMap2d::set(const QPoint &point, int flag) {
  int index = flagToFlagIndex(flag);
  //if (index == -1) {
  //  index = createFlagIndex(flag);
  //}
  while (flagMapIndex(index) >= mNumFlagMaps) {
    addFlagMap();
  }

  mFlagMap[flagMapIndex(index)][pointToIndex(point)] |= flagBitmask(index);
}

// -----------------------------------------------------------------------------
void FlagMap2d::setMulti(const QPoint &point, const QVector<int> &flags) {
  for (int flag : flags) {
    set(point, flag);
  }
}

// -----------------------------------------------------------------------------
int FlagMap2d::createFlagIndex(int flag) {
  int id = flag;
  //int id = mNextFlagIndex++;
  //mFlagToIndex.insert({flag, id});

  while (flagMapIndex(id) >= mNumFlagMaps) {
    addFlagMap();
  }
  return id;
}

// -----------------------------------------------------------------------------
void FlagMap2d::addFlagMap() {
  mFlagMap = static_cast<uint32_t**>(std::realloc(mFlagMap, (mNumFlagMaps + 1) * sizeof(uint32_t*)));
  mFlagMap[mNumFlagMaps] = static_cast<uint32_t*>(std::malloc(mSize.width() * mSize.height() * sizeof(uint32_t)));
  std::memset(mFlagMap[mNumFlagMaps], 0, mSize.width() * mSize.height() * sizeof(uint32_t));
  mNumFlagMaps++;
}