#include <QDebug>
#include <aoba/log/Log.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <aoba/project/Project.hpp>
#include <aoba/sprite/Sprite.hpp>
#include <aoba/sprite/SpriteType.hpp>
#include <aoba/sprite/SpriteAnimationIndex.hpp>
#include <aoba/character/Character.hpp>
#include <aoba/character/CharacterSpriteComponent.hpp>
#include <aoba/level/Level.hpp>
#include <aoba/level/LevelNpcComponent.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

uint32_t LevelNpcInstance::mNextGlobalIdentifier = 0;

// -----------------------------------------------------------------------------
bool LevelNpcComponent::resize(Level *level, int left, int top, int, int, const QSize &) const {
  LevelNpcList list = level->properties()[mId].value<LevelNpcList>();

  for (LevelNpc &npc : list.mNpcList) {
    LevelNpc newNpc = npc;

    for (LevelNpcInstance &instance : npc.mInstances) {
      instance.mX += left * 2;
      instance.mY += top * 2;
    }
  }

  QVariant value;
  value.setValue(list);
  level->properties().insert(mId, value);
  return true;
}

// -----------------------------------------------------------------------------
bool LevelNpcComponent::save(Level *level, YAML::Emitter &root) const {
  const LevelNpcList &instance = level->properties()[mId].value<LevelNpcList>();

  root << YAML::Key << "npc" << YAML::Value << YAML::BeginSeq;
  for (const auto &group : instance.mNpcList) {
    if (group.mInstances.isEmpty()) {
      continue;
    }

    root << YAML::BeginMap;
    root << YAML::Key << "character" << YAML::Value << group.mCharacter.toStdString();
    if (!group.mCrowdId.isEmpty()) { root << YAML::Key << "crowd" << YAML::Value << group.mCrowdId.toStdString(); }
    
    if (group.mScripts.hasAnyScript()) {
      root << YAML::Key << "scripts" << YAML::Value << YAML::BeginMap;
      if (!group.mScripts.mInitialize.isEmpty()) { root << YAML::Key << "initialize" << YAML::Value << group.mScripts.mInitialize.toStdString(); }
      if (!group.mScripts.mMovement.isEmpty()) { root << YAML::Key << "movement" << YAML::Value << group.mScripts.mMovement.toStdString(); }
      if (!group.mScripts.mAction.isEmpty()) { root << YAML::Key << "action" << YAML::Value << group.mScripts.mAction.toStdString(); }
      root << YAML::EndMap;
    }

    root << YAML::Key << "instances" << YAML::Value << YAML::BeginSeq;
    for (const auto &inst : group.mInstances) {
      root << YAML::BeginMap;

      if (!inst.mIdentifier.isEmpty()) { root << YAML::Key << "id" << YAML::Value << inst.mIdentifier.toStdString(); }
      if (inst.mDirection != "down") { root << YAML::Key << "direction" << YAML::Value << inst.mDirection.toStdString(); }
      if (inst.mFrontLayer) { root << YAML::Key << "layer" << YAML::Value << "front"; }
      
      root << YAML::Key << "position" << YAML::Value << YAML::Flow << YAML::BeginMap;
      root << YAML::Key << "x" << YAML::Value << (inst.mX / 2.0);
      root << YAML::Key << "y" << YAML::Value << (inst.mY / 2.0);
      root << YAML::EndMap;

      root << YAML::EndMap;
    }
    root << YAML::EndSeq << YAML::EndMap;
  }
  root << YAML::EndSeq;

  return true;
}

// -----------------------------------------------------------------------------
bool LevelNpcComponent::load(Level *level, YAML::Node &config) const {
  LevelNpcList npcList;
  int index = 0;
  
  for (const auto &node : config["npc"]) {
    LevelNpc npc;
    npc.mCharacter = Yaml::asString(node["character"]);
    npc.mCrowdId = Yaml::asString(node["crowd"]);
    if (node["scripts"].IsDefined()) {
      npc.mScripts.mInitialize = level->levelScriptName(Yaml::asString(node["scripts"]["initialize"]));
      npc.mScripts.mMovement = level->levelScriptName(Yaml::asString(node["scripts"]["movement"]));
      npc.mScripts.mAction = level->levelScriptName(Yaml::asString(node["scripts"]["action"]));
    }

    for (const auto &inst : node["instances"]) {
      if (Yaml::asBool(inst["sa1_only"]) && !level->assetSet()->project()->sa1enabled()) {
        continue;
      }

      LevelNpcInstance instance;
      instance.mIndex = index++;
      instance.mIdentifier = Yaml::asString(inst["id"]);
      instance.mDirection = Yaml::asString(inst["direction"], "down");
      instance.mFrontLayer = Yaml::asString(inst["layer"]) == "front";
      instance.mX = Yaml::asDouble(inst["position"]["x"]) * 2;
      instance.mY = Yaml::asDouble(inst["position"]["y"]) * 2;

      if (inst["condition"].IsDefined()) {
        Aoba::log::warn("TODO: NPC condition not supported yet");
      }

      npc.mInstances.push_back(instance);
      npcList.mNpcIndex.insert(instance.mIdentifier, instance.mIndex);
    }

    if (!npc.mInstances.isEmpty()) {
      npcList.mNpcList.push_back(npc);
    }
  }

  QVariant value;
  value.setValue(npcList);
  level->properties().insert(mId, value);

  return true;
}

// -----------------------------------------------------------------------------
bool LevelNpcComponent::build(Level *level, FMA::linker::LinkerBlock *block) const {
  const LevelNpcList &instance = level->properties()[mId].value<LevelNpcList>();

  block->writeNumber(instance.mNpcList.size(), 1);
  for (const auto &npc : instance.mNpcList) {
    if (!buildNpc(level, block, npc)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
int LevelNpcComponent::indexOfNpc(Level *level, const QString &name) const {
  const LevelNpcList &instance = level->properties()[mId].value<LevelNpcList>();

  if (!instance.mNpcIndex.contains(name)) {
    return -1;
  }
  
  return instance.mNpcIndex[name];
}

// -----------------------------------------------------------------------------
bool LevelNpcComponent::findNpc(Level *level, uint32_t id, const LevelNpc **npc, const LevelNpcInstance **instance) const {
  const LevelNpcList &config = level->properties()[mId].value<LevelNpcList>();

  for (const auto &levelNpc : config.mNpcList) {
    for (const auto &levelNpcInstance : levelNpc.mInstances) {
      if (levelNpcInstance.mGlobalIdentifier == id) {
        *npc = &levelNpc;
        *instance = &levelNpcInstance;
        return true;
      }
    }
  }

  return false;
}

// -----------------------------------------------------------------------------
bool LevelNpcComponent::setNpcInstance(Level *level, uint32_t id, const LevelNpcInstance &value) const {
  LevelNpcList config = level->properties()[mId].value<LevelNpcList>();

  bool found = false;
  for (auto &levelNpc : config.mNpcList) {
    for (auto &levelNpcInstance : levelNpc.mInstances) {
      if (levelNpcInstance.mGlobalIdentifier == id) {
        levelNpcInstance = value;
        found = true;
      }
    }

    if (found) { break; }
  }

  QVariant result;
  result.setValue(config);
  level->properties().insert(mId, result);

  return found;
}

// -----------------------------------------------------------------------------
bool LevelNpcComponent::modifyNpc(Level *level, uint32_t id, const LevelNpc &npc, const LevelNpcInstance &instance) const {
  const LevelNpcList &config = level->properties()[mId].value<LevelNpcList>();

  QMap<QString, LevelNpc> npcByHash;
  for (const LevelNpc &levelNpc : config.mNpcList) {
    QString hash = levelNpc.typeHash();

    if (!npcByHash.contains(hash)) {
      npcByHash.insert(hash, levelNpc.cloneType());
    }

    for (const LevelNpcInstance &instance : levelNpc.mInstances) {
      if (instance.mGlobalIdentifier == id) {
        continue;
      }

      npcByHash[hash].mInstances.push_back(instance);
    }
  }

  QString newItemHash(npc.typeHash());
  if (!npcByHash.contains(newItemHash)) {
    npcByHash[newItemHash] = npc.cloneType();
  }
  npcByHash[newItemHash].mInstances.push_back(instance);


  int index = 0;
  LevelNpcList newConfig;
  QMapIterator<QString, LevelNpc> it(npcByHash);
  while (it.hasNext()) {
    it.next();

    const LevelNpc &npcConfig = it.value();
    if (npcConfig.mInstances.isEmpty()) {
      continue;
    }

    int numRecord = npcConfig.mInstances.count();

    for (int block=0; block<numRecord; block += 7) {
      int maxRecord = qMin(numRecord, block + 7);
      LevelNpc newNpc(npcConfig.cloneType());
      for (int i=block; i<maxRecord; i++) {
        LevelNpcInstance inst = npcConfig.mInstances[i];
        inst.mIndex = index++;
        newNpc.mInstances.push_back(inst);
        newConfig.mNpcIndex.insert(inst.mIdentifier, inst.mIndex);
      }
      newConfig.mNpcList.push_back(newNpc);
    }
  }

  QVariant result;
  result.setValue(newConfig);
  level->properties().insert(mId, result);
  return true;
}

// -----------------------------------------------------------------------------
LevelNpcList LevelNpcComponent::listFor(Level *level) {
  return level->properties()[mId].value<LevelNpcList>();
}

// -----------------------------------------------------------------------------
bool LevelNpcComponent::buildNpc(Level *level, FMA::linker::LinkerBlock *block, const LevelNpc &npc) const {
  uint8_t numInstances = npc.mInstances.size();
  if (numInstances >= 8) {
    Aoba::log::warn("Too many instances of NPC: " + npc.mCharacter);
    return false;
  }
  if (numInstances == 0) {
    Aoba::log::warn("NPC must have at least one instance");
    return false;
  }

  bool hasInitializeScript = !npc.mScripts.mInitialize.isEmpty();
  bool hasMovementScript = !npc.mScripts.mMovement.isEmpty();
  bool hasActionScript = !npc.mScripts.mAction.isEmpty();

  uint8_t flags = 0
    | (numInstances - 1) // 0x07
    | (hasActionScript ? 0x80 : 0)
    | (hasInitializeScript ? 0x40 : 0)
    | (hasMovementScript ? 0x20 : 0)
    | (npc.mCrowdId.isEmpty() ? 0 : 0x10)
  ;

  Character *character = mCharacters->get(npc.mCharacter);
  if (!character) {
    Aoba::log::error("Could not find character: " + npc.mCharacter);
    return false;
  }
  
  block->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(character->symbolName().toStdString())), 2);

  block->writeNumber(flags, 1);
  if (!writeScript(level, block, npc.mScripts.mAction)
    || !writeScript(level, block, npc.mScripts.mInitialize)
    || !writeScript(level, block, npc.mScripts.mMovement)
  ) {
    return false;
  }

  for (const auto &instance : npc.mInstances) {
    if (!buildNpcInstance(character, block, instance)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool LevelNpcComponent::buildNpcInstance(Character *character, FMA::linker::LinkerBlock *block, const LevelNpcInstance &npc) const {
  CharacterSpriteComponent *spriteComponent = character->assetSet()->getComponent<CharacterSpriteComponent>();
  if (!spriteComponent) {
    Aoba::log::error("Failed to fetch sprite component for character: " + character->id());
    return false;
  }

  Sprite *sprite = spriteComponent->getSpriteFor(character);
  int direction = 0;
  if (sprite != nullptr) {
    direction = sprite->spriteType()->animationIndex()->directionId(npc.mDirection);
  }
  if (direction == -1) {
    Aoba::log::warn("Invalid NPC direction: " + npc.mDirection);
    direction = 0;
  }

  uint8_t flags = 0
    | direction
    | (npc.mFrontLayer ? 0x80 : 0)
    ;

  block->writeNumber(flags, 1);
  block->writeNumber(npc.mX, 1);
  block->writeNumber(npc.mY, 1);
  return true;
}

// -----------------------------------------------------------------------------
bool LevelNpcComponent::writeScript(Level *level, FMA::linker::LinkerBlock *block, const QString &script) const {
  if (script.isEmpty()) {
    return true;
  }

  QString scriptName = level->assetSet()->project()->getScriptSymbolName(script);
  if (scriptName.isEmpty()) {
    return false;
  }

  block->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(scriptName.toStdString())), 3);
  return true;
}
