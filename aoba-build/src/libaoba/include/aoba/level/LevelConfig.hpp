#pragma once

#include <QVector>
#include "../asset/Asset.hpp"
#include "../asset/LinkedAssetSet.hpp"

namespace Aoba {
class Level;
class LevelComponent;

class LevelAssetSet : public LinkedAssetSet<Level> {
public:
  //! Constructor
  LevelAssetSet(Project *project, const QString &name, uint8_t headerRomBank)
    : LinkedAssetSet<Level>(name, project)
    , mName(name)
    , mHeaderRomBank(headerRomBank)
  {}

  //! Deconstructor
  ~LevelAssetSet();

  //! Returns the asset type name
  inline const QString &assetName() const { return mName; }

  //! Adds a component
  void addComponent(LevelComponent*);

  //! Returns a single component
  LevelComponent *getComponent(const QString &type) const;

  //! Returns a single component
  template<typename T>
  T *getComponent() const { return dynamic_cast<T*>(getComponent(T::TypeId())); }

  //! Returns the header ROM bank
  inline uint8_t headerRomBank() const { return mHeaderRomBank; }

  //! Returns all components
  inline const QVector<LevelComponent*> components() const { return mComponents; }

private:
  //! The name
  QString mName;

  //! The header ROM bank
  uint8_t mHeaderRomBank;

  //! List of all components
  QVector<LevelComponent*> mComponents;
};

}