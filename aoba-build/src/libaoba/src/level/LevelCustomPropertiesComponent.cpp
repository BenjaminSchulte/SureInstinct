#include <QSize>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <aoba/project/Project.hpp>
#include <aoba/log/Log.hpp>
#include <aoba/font/FontRegistry.hpp>
#include <aoba/text/TextRegistry.hpp>
#include <aoba/text/TextTranslatable.hpp>
#include <aoba/text/TextTranslatableIndex.hpp>
#include <aoba/level/Level.hpp>
#include <aoba/level/LevelConfig.hpp>
#include <aoba/level/LevelCustomPropertiesComponent.hpp>
#include <aoba/property/TextPropertyType.hpp>
#include <aoba/property/PositionPropertyType.hpp>
#include <aoba/property/NumberPropertyType.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool LevelCustomPropertiesComponent::resize(Level *level, int left, int top, int, int, const QSize &newSize) const {
  (void)level;
  (void)top;
  (void)left;
  (void)newSize;
  // Todo check properties
  Aoba::log::todo("LevelCustomPropertiesComponent::resize");
  return false;
}

// -----------------------------------------------------------------------------
bool LevelCustomPropertiesComponent::save(Level *level, YAML::Emitter &root) const {
  root << YAML::Key << mId.toStdString() << YAML::Value;
  mProperties->save(root, level->properties()[mId]);
  return true;
}

// -----------------------------------------------------------------------------
bool LevelCustomPropertiesComponent::load(Level *level, YAML::Node &config) const {
  level->properties()[mId] = mProperties->load(config[mId.toStdString()]);

  return preloadProperty("", mProperties, level->properties()[mId], level);
}

// -----------------------------------------------------------------------------
bool LevelCustomPropertiesComponent::build(Level *level, FMA::linker::LinkerBlock *block) const {
  return buildProperty("", mProperties, level->properties()[mId], level, block);
}

// -----------------------------------------------------------------------------
bool LevelCustomPropertiesComponent::preloadProperty(const QString &id, PropertyType *type, const Property &value, Level *level) const {
  QString typeId = type->typeId();

  // TODO: use global property registry to support custom properties
  if (typeId == "Aoba::GroupPropertyType") {
    return preloadGroupProperty(id, dynamic_cast<GroupPropertyType*>(type), value, level);
  } else if (typeId == "Aoba::TextPropertyType") {
    return preloadTextProperty(id, dynamic_cast<TextPropertyType*>(type), value, level);
  }
  return true;
}

// -----------------------------------------------------------------------------
bool LevelCustomPropertiesComponent::preloadGroupProperty(const QString &, GroupPropertyType *type, const Property &value, Level *level) const {
  const QMap<QString, QVariant> &map = value.toMap();

  for (const QString &key : type->keysInOrder()) {
    if (!preloadProperty(key, type->get(key), map[key], level)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool LevelCustomPropertiesComponent::preloadTextProperty(const QString &id, TextPropertyType *type, const Property &value, Level *level) const {
  TextTranslatable *translatable = level->project()->texts()->translations()->section("Level_" + level->id())->text(id);
  translatable->addTranslation(level->project()->texts()->translations()->inputLanguage(), value.toString());
  
  for (const QString &fontId : type->fonts()) {
    FontInterface *font = level->project()->fonts()->font(fontId);
    if (!font) {
      Aoba::log::error("Unable to find font: " + fontId);
      return false;
    }

    translatable->addFont(font);
  }

  level->properties().insert(mId + "." + id, translatable->symbolName());
  return true;
}

// -----------------------------------------------------------------------------
bool LevelCustomPropertiesComponent::buildProperty(const QString &id, PropertyType *type, const Property &value, Level *level, FMA::linker::LinkerBlock *block) const {
  QString typeId = type->typeId();

  // TODO: use global property registry to support custom properties
  if (typeId == "Aoba::GroupPropertyType") {
    return buildGroupProperty(id, dynamic_cast<GroupPropertyType*>(type), value, level, block);
  } else if (typeId == "Aoba::TextPropertyType") {
    return buildTextProperty(id, dynamic_cast<TextPropertyType*>(type), value, level, block);
  } else if (typeId == "Aoba::Vector2dPropertyType") {
    return buildVector2dProperty(id, dynamic_cast<Vector2dPropertyType*>(type), value, level, block);
  } else if (typeId == "Aoba::PositionPropertyType") {
    return buildVector2dProperty(id, dynamic_cast<Vector2dPropertyType*>(type), value, level, block);
  } else if (typeId == "Aoba::NumberPropertyType") {
    return buildNumberProperty(id, dynamic_cast<NumberPropertyType*>(type), value, level, block);
  } else {
    Aoba::log::error("Can not build unsupported property type: " + type->typeId());
    return false;
  }
}

// -----------------------------------------------------------------------------
bool LevelCustomPropertiesComponent::buildGroupProperty(const QString &, GroupPropertyType *type, const Property &value, Level *level, FMA::linker::LinkerBlock *block) const {
  const QMap<QString, QVariant> &map = value.toMap();

  for (const QString &key : type->keysInOrder()) {
    if (!buildProperty(key, type->get(key), map[key], level, block)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool LevelCustomPropertiesComponent::buildTextProperty(const QString &id, TextPropertyType*, const Property &, Level *level, FMA::linker::LinkerBlock *block) const {
  QString symbolName = level->properties()[mId + ":" + id].toString();
  if (symbolName.isEmpty()) {
    block->writeNumber(0, 3);
    return true;
  }

  block->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(symbolName.toStdString())), 3);
  return true;
}

// -----------------------------------------------------------------------------
bool LevelCustomPropertiesComponent::buildVector2dProperty(const QString &, Vector2dPropertyType *type, const Property &value, Level *level, FMA::linker::LinkerBlock *block) const {
  (void)type;
  (void)level;

  QPointF pos = value.toPointF();
  block->writeNumber(pos.x() * 16, 2);
  block->writeNumber(pos.y() * 16, 2);
  return true;
}

// -----------------------------------------------------------------------------
bool LevelCustomPropertiesComponent::buildNumberProperty(const QString &, NumberPropertyType *type, const Property &value, Level *level, FMA::linker::LinkerBlock *block) const {
  (void)type;
  (void)level;

  block->writeNumber(value.toInt(), 2);
  return true;
}
