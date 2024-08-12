#pragma once

#include "./LevelComponent.hpp"

namespace Aoba {
class Character;
class CharacterAssetSet;

enum class LevelNpcConditionOperator {
  NO_CONDITION
};

struct LevelNpcCondition {
  //! The condition type
  LevelNpcConditionOperator mOperator;

  //! The variable
  QString mVariable;

  //! The compare value
  int mCompare;
};

struct LevelNpcInstance {
  LevelNpcInstance() : mGlobalIdentifier(mNextGlobalIdentifier++) {}

  //! X position (8 pixel steps)
  int mX;

  //! Y position (8 pixel steps)
  int mY;

  //! NPC index
  uint8_t mIndex;
  
  //! Global identifier
  uint32_t mGlobalIdentifier;

  //! Reference ID
  QString mIdentifier;

  //! View direction as string
  QString mDirection;

  //! The target layer
  bool mFrontLayer;

  //! The condition
  LevelNpcCondition mCondition;

  //! SA1 only
  bool mSa1Only;

  static uint32_t mNextGlobalIdentifier;
};

struct LevelNpcScriptList {
  bool hasAnyScript() const {
    return !mInitialize.isEmpty() || !mMovement.isEmpty() || !mAction.isEmpty();
  }

  //! The initialize script
  QString mInitialize;

  //! The movement script
  QString mMovement;

  //! The action script
  QString mAction;
};

struct LevelNpc {
  //! Clones the type
  LevelNpc cloneType() const {
    LevelNpc copy;
    copy.mCharacter = mCharacter;
    copy.mCrowdId = mCrowdId;
    copy.mScripts.mAction = mScripts.mAction;
    copy.mScripts.mInitialize = mScripts.mInitialize;
    copy.mScripts.mMovement = mScripts.mMovement;
    return copy;
  }

  //! Returns a hash for the NPC without its instances
  QString typeHash() const {
    QStringList hashItems;
    hashItems << mCharacter;
    hashItems << mCrowdId;
    hashItems << mScripts.mAction;
    hashItems << mScripts.mInitialize;
    hashItems << mScripts.mMovement;
    return hashItems.join('#');
  }

  //! The character
  QString mCharacter;

  //! Whether the character is a crowded NPC
  QString mCrowdId;

  //! Scripts
  LevelNpcScriptList mScripts;

  //! List of all instances
  QVector<LevelNpcInstance> mInstances;
};

struct LevelNpcList {
  //! All NPCs
  QVector<LevelNpc> mNpcList;

  //! NPC index
  QMap<QString, int> mNpcIndex;
};

class LevelNpcComponent : public LevelComponent {
public:
  //! Constructor
  LevelNpcComponent(const QString &id, CharacterAssetSet *characters)
    : LevelComponent(id)
    , mCharacters(characters)
  {}

  //! The type id
  static QString TypeId() { return "LevelNpcComponent"; }

  //! Returns the type ID
  QString typeId() const override { return TypeId(); }

  //! Loads the component
  bool load(Level *level, YAML::Node &config) const override;

  //! Saves the component
  bool save(Level *Level, YAML::Emitter &root) const override;
  
  //! Builds the component
  bool build(Level *level, FMA::linker::LinkerBlock *block) const override;

  //! Index of a NPC
  int indexOfNpc(Level *level, const QString &name) const;

  //! Finds an NPC
  bool findNpc(Level *level, uint32_t id, const LevelNpc **npc, const LevelNpcInstance **instance) const;

  //! Sets an NPC instance
  bool setNpcInstance(Level *level, uint32_t id, const LevelNpcInstance &value) const;

  //! Modify an NPC
  bool modifyNpc(Level *level, uint32_t id, const LevelNpc &npc, const LevelNpcInstance &instance) const;

  //! Resizes content
  bool resize(Level *level, int left, int top, int right, int bottom, const QSize &newSize) const override;

  //! Returns the list
  LevelNpcList listFor(Level *level);

  //! Returns the characters
  inline CharacterAssetSet *characters() const { return mCharacters; }

protected:
  //! Builds the component
  bool buildNpc(Level *level, FMA::linker::LinkerBlock *block, const LevelNpc &npc) const;

  //! Builds the component
  bool buildNpcInstance(Character *character, FMA::linker::LinkerBlock *block, const LevelNpcInstance &npc) const;

  //! Writes a script pointer
  bool writeScript(Level *level, FMA::linker::LinkerBlock *block, const QString &script) const;

  //! Characters type
  CharacterAssetSet *mCharacters;
};
  
}

Q_DECLARE_METATYPE(Aoba::LevelNpcList);