#pragma once

#include <QDir>
#include "../asset/Asset.hpp"
#include "../asset/LinkedAssetSet.hpp"

namespace Aoba {
class Character;
class CharacterComponent;

class CharacterAssetSet : public LinkedAssetSet<Character> {
public:
  //! Constructor
  CharacterAssetSet(Project *project, const QString &name, uint8_t headerRomBank);

  //! Deconstructor
  ~CharacterAssetSet();

  //! Returns the asset name
  inline const QString &assetName() const { return mName; }

  //! Adds a component
  void addComponent(CharacterComponent*);

  //! Returns a single component
  CharacterComponent *getComponent(const QString &type) const;

  //! Returns a single component
  template<typename T>
  T *getComponent() const { return dynamic_cast<T*>(getComponent(T::TypeId())); }

  //! Returns all components
  inline const QVector<CharacterComponent*> components() const { return mComponents; }

  //! Returns the header ROM bank
  inline uint8_t headerRomBank() const { return mHeaderRomBank; }

  //! Returns whether static content should now be built
  inline bool shouldBuiltStaticContent() {
    if (!mMustBuiltStaticContent) { return false; }
    mMustBuiltStaticContent = false;
    return true;
  }

  //! Returns an hitbox ID
  int getHitBoxId(const QRect &);

private:
  //! The name
  QString mName;

  //! List of all components
  QVector<CharacterComponent*> mComponents;

  //! The header ROM bank
  uint8_t mHeaderRomBank;

  //! List of all hitboxes
  FMA::linker::LinkerBlock *mHitBoxBlock[4];

  //! Map containing all hitboxes
  QMap<QString, int> mHitBoxIndex;

  //! True if static content still needs to be built
  bool mMustBuiltStaticContent = true;
};

}