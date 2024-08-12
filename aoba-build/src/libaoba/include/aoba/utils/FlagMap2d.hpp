#pragma once

#include <map>
#include <QSize>
#include <QPoint>

namespace Aoba {

class FlagMap2d {
public:
  //! Constructor
  FlagMap2d(const QSize &size=QSize()) {
    resizeAndClear(size);
  }

  //! Deconstructor
  ~FlagMap2d();

  //! Unloads the map
  void unload();

  //! Clears the map
  void clear();

  //! Resize to a size
  void resizeAndClear(const QSize &);

  //! Sets a flag
  void set(const QPoint &point, int flag);

  //! Sets many items
  void setMulti(const QPoint &point, const QVector<int> &flags);

  //! Tests for a flag
  inline bool test(const QPoint &point, int flag) const {
    int index = flagToFlagIndex(flag);
    uint32_t mapIndex = flagMapIndex(index);
    return (mapIndex >= mNumFlagMaps) ? false : (mFlagMap[mapIndex][pointToIndex(point)] & flagBitmask(index));
    //return index == -1 ? false : (mFlagMap[flagMapIndex(index)][pointToIndex(point)] & flagBitmask(index));
  }

private:
  //! Adds a flag index
  int createFlagIndex(int flag);

  //! Adds a new flag map
  void addFlagMap();

  //! Requires a flag map to exist
  inline int flagToFlagIndex(int flag) const {
    return flag;
    //std::map<int, int>::const_iterator it(mFlagToIndex.find(flag));
    //return it == mFlagToIndex.end() ? -1 : it->second;
  }

  //! Returns the flag map bitmask
  inline uint32_t flagBitmask(int flagIndex) const { return 1 << (flagIndex & 0x1F); }

  //! Returns the flag map index
  inline uint32_t flagMapIndex(int flagIndex) const { return flagIndex >> 5; }

  //! Converts the point to an index
  inline int pointToIndex(const QPoint &point) const { return point.x() + point.y() * mSize.width(); }

  //! The size
  QSize mSize;

  //! The maps
  //std::map<int, int> mFlagToIndex;

  //! The next flag index;
  //int mNextFlagIndex = 0;

  //! List of flags
  uint32_t **mFlagMap = nullptr;

  //! Size of the flag list
  uint32_t mNumFlagMaps = 0;
};

}