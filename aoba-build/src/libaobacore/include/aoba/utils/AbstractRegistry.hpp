#pragma once

#include <QMap>

namespace Aoba {

template <typename R, typename T>
class AbstractRegistry {
public:
  //! Returns the instance
  static R* instance() {
    if (!R::mInstance) {
      R::mInstance = new R();
      R::mInstance->registerDefaultTypes();
    }

    return R::mInstance;
  }

  //! Destroys the instance
  static void destroyInstance() {
    delete R::mInstance;
    R::mInstance = nullptr;
  }

  //! Registers the default types
  virtual void registerDefaultTypes() {}

  //! Returns whether a record exists
  inline bool has(const QString &id) const { return mRecords.contains(id); }

  //! Returns whether a record exists
  inline const T &get(const QString &id) const { return mRecords[id]; }

  //! Returns whether a record exists
  inline void add(const QString &id, T &item) { mRecords.insert(id, item); }

protected:
  //! Constructor
  AbstractRegistry() {}

  //! Deconstructor
  virtual ~AbstractRegistry() {}

  //! The content map
  QMap<QString, T> mRecords;
};

}