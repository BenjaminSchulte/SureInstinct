#include <aoba/log/Log.hpp>
#include <aoba/level/LevelScriptAssetResolver.hpp>
#include <aoba/level/LevelSwitchComponent.hpp>
#include <aoba/level/LevelNpcComponent.hpp>
#include <aoba/level/Level.hpp>
#include <fma/core/Number.hpp>
#include <fma/core/SymbolReference.hpp>
#include <fma/symbol/ConstantNumber.hpp>
#include <fma/symbol/SymbolReference.hpp>

using namespace Aoba;
using namespace FMA::symbol;
using namespace FMA::core;
using namespace FMA::interpret;

// ----------------------------------------------------------------------------
QVariant LevelScriptAssetResolver::resolve(const QString &id, const QString &type) {
  QStringList parts = id.split('/');

  Level *level = mType->get(parts[0]);
  if (!level) {
    Aoba::log::error("Unable to find level: " + parts[0]);
    return QVariant();
  }

  if (type == "address") {
    return level->symbolName();
  } else if (type == "switch_address" && parts.length() == 2) {
    LevelSwitchComponent *component = level->assetSet()->getComponent<LevelSwitchComponent>();
    if (!component) {
      Aoba::log::error("No switch component configured for level");
      return QVariant();
    }
    return component->symbolName(level, parts[1]);
  } else if (type == "npc_id" && parts.length() == 2) {
    LevelNpcComponent *component = level->assetSet()->getComponent<LevelNpcComponent>();
    if (!component) {
      Aoba::log::error("No NPC component configured for level");
      return QVariant();
    }
    int npcId = component->indexOfNpc(level, parts[1]);
    if (npcId < 0) {
      Aoba::log::error("No NPC with ID" + parts[1] + "found in level" + parts[0]);
      return QVariant();
    }
    return npcId;
  } else {
    Aoba::log::error("Unsupported level type: " + type);
    return QVariant();
  }
}
