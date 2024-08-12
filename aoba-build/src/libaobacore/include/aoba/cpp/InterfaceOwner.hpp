#pragma once

#if defined(QT_NO_DEBUG) && !defined(QT_FORCE_ASSERTS)
#else
#  include <aoba/log/Log.hpp>
#endif

#include <QMap>

#include "Interface.hpp"

namespace Aoba {

template <typename TBase>
class InterfaceOwner {
public:
  //! The interface type
  typedef TBase InterfaceType;

  //! Constructor
  InterfaceOwner() = default;

  //! Deconstructor
  virtual ~InterfaceOwner() = default;

  //! Returns whether an interface exists
  virtual bool hasInterface(const char *id) const { return mInterfaces.contains(id); }

  //! Returns an interface exists
  virtual TBase *interface(const char *id) const { return mInterfaces[id]; }

  //! Returns a list of all interfaces
  virtual const QMap<QString, TBase*> &interfaces() const { return mInterfaces; }

  //! Returns whether an interface exists
  template<typename T>
  inline bool hasInterface() const {
    return mInterfaces.contains(T::TypeId());
  }

  //! Returns an interface
  template<typename T>
  inline T* interface() const {
#   if defined(QT_NO_DEBUG) && !defined(QT_FORCE_ASSERTS)
#   else
      if (!hasInterface(T::TypeId())) {
        Aoba::log::warn("[ERROR] Missing interface:" << T::TypeId();
      }
      Q_ASSERT(hasInterface(T::TypeId()));
#   endif

    return dynamic_cast<T*>(interface(T::TypeId()));
  }

protected:
  //! Adds an interface
  template <typename T>
  void addInterface(T *interface) {
    Q_ASSERT(!mInterfaces.contains(T::TypeId()));
    mInterfaces.insert(T::TypeId(), interface);
  }

  //! List of all interfaces
  QMap<QString, TBase*> mInterfaces;
};

}