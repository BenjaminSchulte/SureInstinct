#include <iostream>
#include <aoba/log/Log.hpp>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <fma/linker/LinkerBlock.hpp>
#include <aoba/asset/AssetLinkerObject.hpp>
#include <aoba/script/ScriptProject.hpp>
#include <aoba/variable/RamVariableSchema.hpp>
#include <aoba/variable/RamVariableScope.hpp>
#include <aoba/variable/RamVariable.hpp>
#include <aoba/variable/UsageMap.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
RamVariableSchemaRecord::RamVariableSchemaRecord(const RamVariableSchemaRecord &old) {
  *this = old;

  mDefaultData = nullptr;
  setDefaultValue(old.mDefaultData, old.mDefaultDataSize);
}

// -----------------------------------------------------------------------------
RamVariableSchemaRecord::~RamVariableSchemaRecord() {
  if (mDefaultData) {
    free(mDefaultData);
  }
}

// -----------------------------------------------------------------------------
void RamVariableSchemaRecord::setDefaultValue(void *data, uint32_t size) {
  uint32_t numBytes = qMax<uint32_t>(1, (size + 7) / 8);
  if (mDefaultData) {
    free(mDefaultData);
  }

  if (size == 0) {
    mDefaultData = nullptr;
    mDefaultDataSize = 0;
    return;
  }
  
  mDefaultData = malloc(numBytes);
  memcpy(mDefaultData, data, numBytes);
  mDefaultDataSize = size;
}

// -----------------------------------------------------------------------------
RamVariableSchema::RamVariableSchema(uint16_t maxSizeInBytes, uint16_t numSaveSlots, uint8_t identifierSize)
  : mMaxSizeInBytes(maxSizeInBytes)
  , mNumSaveSlots(numSaveSlots)
  , mIdentifierSize(identifierSize)
{
  for (int i=0; i<RamVariableSchemaRecord::NUM_SCHEMA_GROUPS; i++) {
    /*int sizeInBits;

    switch (i) {
      case RamVariableSchemaRecord::GLOBAL_PERSISTENT_WITH_DEFAULT:
      case RamVariableSchemaRecord::GLOBAL_PERSISTENT:
        sizeInBits = maxSizeInBytes * 8;
        break;

      case RamVariableSchemaRecord::PERSISTENT_WITH_DEFAULT:
      case RamVariableSchemaRecord::PERSISTENT:
        sizeInBits = (maxSizeInBytes / numSaveSlots) * 8;
        break;

      case RamVariableSchemaRecord::TEMPORARY_WITH_DEFAULT:
      case RamVariableSchemaRecord::TEMPORARY:
        sizeInBits = maxSizeInBytes * 8;
        break;
    }*/

    mUsageMap[i] = new UsageMap(0, maxSizeInBytes * 8);
  }
}

// -----------------------------------------------------------------------------
RamVariableSchema::~RamVariableSchema() {
  for (int i=0; i<RamVariableSchemaRecord::NUM_SCHEMA_GROUPS; i++) {
    delete mUsageMap[i];
  }
}

// -----------------------------------------------------------------------------
bool RamVariableSchema::addFromScope(RamVariableScope *scope) {
  if (!scope->isValid()) {
    Aoba::log::warn("Tried to add variables from an invalid scope.");
    return false;
  }

  RamVariableCreateDescriptor desc;
  bool valid = true;
  addFromScopeWithDescriptor(scope, desc, valid);
  if (!valid) {
    return false;
  }

  if (!hasValidSize()) {
    Aoba::log::warn("Maximum SRAM size has been exceeded. Try to remove the variableschema.json file for a rebuild or minimize variable usage.");
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
int RamVariableSchema::addFromNestedSharedScopeWithDescriptor(RamVariableScope *scope, RamVariableCreateDescriptor &desc, bool &valid) {
  RamVariableCreateDescriptor thisDesc(desc);

  uint32_t size = scope->totalSizeInBits();
  thisDesc.mSizeLeft = size;

  for (RamVariableScope *child : scope->scopes()) {
    RamVariableCreateDescriptor copy(thisDesc);
    addFromScopeWithDescriptor(child, copy, valid);
  }

  for (RamVariable *var : scope->variables()) {
    RamVariableCreateDescriptor copy(thisDesc);
    addVariable(var, copy, valid);
  }
  
  return size;
}

// -----------------------------------------------------------------------------
int RamVariableSchema::addFromSharedScopeWithDescriptor(RamVariableScope *scope, RamVariableCreateDescriptor &desc, bool &valid) {
  if (desc.mIsShared) {
    return addFromNestedSharedScopeWithDescriptor(scope, desc, valid);
  }

  uint32_t size = scope->totalSizeInBits();
  if (size == 0) {
    return 0;
  }

  QString id = scope->uniqueIdentifier();

  desc.mIsShared = true;

  if (mRecords.contains(id) && mRecords[id].sizeInBits() >= size) {
    desc.mSizeLeft = mRecords[id].sizeInBits();
    desc.mOffset = mRecords[id].offsetInBits();
    mRecords[id].markUsed();
  } else {
    desc.mSizeLeft = size;
    desc.mOffset = allocateRam(groupForScope(scope, nullptr), size);

    RamVariableSchemaRecord record(id, groupForScope(scope, nullptr), desc.mOffset, size);
    record.markUsed();
    mRecords[id] = record;
  }

  if (desc.mOffset < 0) {
    valid = false;
    return 0;
  }

  for (RamVariableScope *child : scope->scopes()) {
    RamVariableCreateDescriptor copy(desc);
    addFromScopeWithDescriptor(child, copy, valid);
  }

  for (RamVariable *var : scope->variables()) {
    RamVariableCreateDescriptor copy(desc);
    addVariable(var, copy, valid);
  }
  
  return size;
}

// -----------------------------------------------------------------------------
int RamVariableSchema::addFromScopeWithDescriptor(RamVariableScope *scope, RamVariableCreateDescriptor &desc, bool &valid) {
  if (scope->isShared()) {
    return addFromSharedScopeWithDescriptor(scope, desc, valid);
  }

  int size = 0;

  for (RamVariableScope *child : scope->scopes()) {
    int itemSize = addFromScopeWithDescriptor(child, desc, valid);
    desc.mOffset += itemSize;
    desc.mSizeLeft -= itemSize;
    size += itemSize; 
  }

  for (RamVariable *var : scope->variables()) {
    int itemSize = addVariable(var, desc, valid);
    desc.mOffset += itemSize;
    desc.mSizeLeft -= itemSize;
    size += itemSize; 
  }

  return size;
}

// -----------------------------------------------------------------------------
int RamVariableSchema::addVariable(RamVariable *v, const RamVariableCreateDescriptor &desc, bool &valid) {
  int offset;
  RamVariableSchemaRecord::RamVariableSchemaGroup group = groupForScope(v->scope(), v);
  QString id = v->uniqueIdentifier();

  if (desc.mIsShared) {
    offset = desc.mOffset;

    if (mRecords.contains(id) && mRecords[id].offsetInBits() == (uint32_t)desc.mOffset) {
      mRecords[id].markUsed();
    } else {
      RamVariableSchemaRecord record(id, group, desc.mOffset, v->sizeInBits());
      if (v->hasDefaultValue()) {
        Aoba::log::warn("Shared variables can not have a default value");
      }
      record.markUsed();
      mRecords[id] = record;
    }
  } else {
    if (mRecords.contains(id)) {
      mRecords[id].markUsed();
      offset = mRecords[id].offsetInBits();
    } else {
      offset = allocateRam(group, v->sizeInBits());
      
      RamVariableSchemaRecord record(id, group, offset, v->sizeInBits());
      record.markUsed();
      mRecords[id] = record;
    }

    if (v->hasDefaultValue()) {
      uint16_t size = 0;
      void *data = v->defaultValue(size);
      if (size) {
        mRecords[id].setDefaultValue(data, size);
      }
    }
  }

  if (offset == -1) {
    valid = false;
    return 0;
  }

  return v->sizeInBits();
}

// -----------------------------------------------------------------------------
RamVariableSchemaRecord::RamVariableSchemaGroup RamVariableSchema::groupForScope(RamVariableScope *scope, RamVariable *v) const {
  if (!scope->isPersistent()) {
    if (v && v->hasDefaultValue()) {
      return RamVariableSchemaRecord::TEMPORARY_WITH_DEFAULT;
    } else {
      return RamVariableSchemaRecord::TEMPORARY;
    }
  }

  if (scope->isGlobal()) {
    if (v && v->hasDefaultValue()) {
      return RamVariableSchemaRecord::GLOBAL_PERSISTENT_WITH_DEFAULT;
    } else {
      return RamVariableSchemaRecord::GLOBAL_PERSISTENT;
    }
  } else {
    if (v && v->hasDefaultValue()) {
      return RamVariableSchemaRecord::PERSISTENT_WITH_DEFAULT;
    } else {
      return RamVariableSchemaRecord::PERSISTENT;
    }
  }
}

// -----------------------------------------------------------------------------
int RamVariableSchema::allocateRam(RamVariableSchemaRecord::RamVariableSchemaGroup group, int sizeInBits) {
  UsageMapAddressRanges ranges = mUsageMap[group]->findFreeAddresses(mUsageMap[group]->getOffset(), mUsageMap[group]->getSize(), sizeInBits);
  if (ranges.isEmpty()) {
    Aoba::log::warn("No free address range for type " + QString::number((int)group) + " with requested size " + QString::number(sizeInBits));
    return -1;
  }

  uint64_t leftMost = 0xFFFFFFFFFFFFFFFF;
  for (const UsageMapAddressRange &range : ranges) {
    leftMost = qMin<uint64_t>(leftMost, range.from);
  }

  mUsageMap[group]->block(leftMost, sizeInBits);
  return leftMost;
}

// -----------------------------------------------------------------------------
bool RamVariableSchema::restoreFromFile(const QString &fileName) {
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return false;
  }

  QByteArray bytes = file.readAll();
  file.close();

  QJsonParseError jsonError;
  QJsonDocument document = QJsonDocument::fromJson(bytes, &jsonError);
  if (jsonError.error != QJsonParseError::NoError) {
    Aoba::log::warn("Error in JSON file " + fileName + ": " + jsonError.errorString());
    return false;
  }

  QJsonObject obj(document.object());
  if (obj["version"].toInt(0) != 1) {
    Aoba::log::warn("Invalid record schema version");
    return false;
  }

  for (const auto &record : obj["records"].toArray()) {
    QJsonObject obj = record.toObject();

    QString id = obj["id"].toString();
    int offset = obj["offset"].toInt();
    int size = obj["size"].toInt();
    RamVariableSchemaRecord::RamVariableSchemaGroup group = (RamVariableSchemaRecord::RamVariableSchemaGroup)obj["group"].toInt();

    if (group < RamVariableSchemaRecord::NUM_SCHEMA_GROUPS) {
      mUsageMap[group]->block(offset, size);
    }

    mRecords.insert(id, RamVariableSchemaRecord(id, group, offset, size));
  }

  return true;
}

// -----------------------------------------------------------------------------
bool RamVariableSchema::storeToFile(const QString &fileName) const {
  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    Aoba::log::warn("Unable to open file " + fileName + " for writing");
    return false;
  }

  QJsonObject root;
  root.insert("version", QJsonValue::fromVariant(1));

  QJsonArray records;
  for (const auto &input : mRecords) {
    QJsonObject record;
    record.insert("id", QJsonValue::fromVariant(input.id()));
    record.insert("offset", QJsonValue::fromVariant(input.offsetInBits()));
    record.insert("size", QJsonValue::fromVariant(input.sizeInBits()));
    record.insert("group", QJsonValue::fromVariant((int)input.group()));
    records.push_back(record);
  }
  root.insert("records", records);

  QJsonDocument document(root);
  file.write(document.toJson());

  return true;
}

// -----------------------------------------------------------------------------
int RamVariableSchema::idOfVariable(const QString &id) {
  if (!mVariableIds.contains(id)) {
    mVariableIds.push_back(id);
  }

  return mVariableIds.indexOf(id);
}

// -----------------------------------------------------------------------------
bool RamVariableSchema::build(ScriptProject *project) const {
  if (!hasValidSize()) {
    return false;
  }

  uint32_t maxGlobalDefault = getMaxOffset(RamVariableSchemaRecord::GLOBAL_PERSISTENT_WITH_DEFAULT);
  uint32_t maxPersistentDefault = getMaxOffset(RamVariableSchemaRecord::PERSISTENT_WITH_DEFAULT);
  uint32_t maxTemporaryDefault = getMaxOffset(RamVariableSchemaRecord::TEMPORARY_WITH_DEFAULT);

  FMA::linker::LinkerBlock *blockAddress = project->fmaLinkerObject().createBlock();
  FMA::linker::LinkerBlock *blockConfig = project->fmaLinkerObject().createBlock();
  FMA::linker::LinkerBlock *blockGlobalDefault = project->fmaLinkerObject().createBlock();
  FMA::linker::LinkerBlock *blockPersistentDefault = project->fmaLinkerObject().createBlock();
  FMA::linker::LinkerBlock *blockTemporaryDefault = project->fmaLinkerObject().createBlock();
  
  blockAddress->symbol("__asset_variableschema_index_address");
  blockConfig->symbol("__asset_variableschema_index_config");
  blockGlobalDefault->symbol("__asset_variableschema_default_global");
  blockTemporaryDefault->symbol("__asset_variableschema_default_temporary");
  blockPersistentDefault->symbol("__asset_variableschema_default_persistent");

  if (mVariableIds.isEmpty()) {
    blockAddress->writeNumber(0, 2);
    blockConfig->writeNumber(0, 2);
    blockGlobalDefault->writeNumber(0, 1);
    blockPersistentDefault->writeNumber(0, 1);
    blockTemporaryDefault->writeNumber(0, 1);
    return true;
  }

  uint16_t nextVariableId = 0;
  for (const QString &id : mVariableIds) {
    const auto &record = mRecords[id];

    uint32_t baseInBits = 0;
    uint16_t bitmask = 0;
    switch (record.group()) {
      case RamVariableSchemaRecord::GLOBAL_PERSISTENT:
        baseInBits = maxGlobalDefault;
        bitmask |= 0xC000;
        break;
      case RamVariableSchemaRecord::GLOBAL_PERSISTENT_WITH_DEFAULT:
        bitmask |= 0xC000;
        break;

      case RamVariableSchemaRecord::PERSISTENT:
        baseInBits = maxPersistentDefault;
        break;
      case RamVariableSchemaRecord::PERSISTENT_WITH_DEFAULT:
        break;

      case RamVariableSchemaRecord::TEMPORARY:
        baseInBits = maxTemporaryDefault;
        bitmask = 0x8000;
        break;
      case RamVariableSchemaRecord::TEMPORARY_WITH_DEFAULT:
        bitmask = 0x8000;
        break;

      default:
        break;
    }

    if (record.hasDefaultValue()) {
      setVariableDefaultValue(record, defaultLinkerBlock(record.group(), blockGlobalDefault, blockPersistentDefault, blockTemporaryDefault));
    }

    uint32_t offset = record.offsetInBits() + baseInBits;
    blockAddress->writeNumber((offset >> 3) | bitmask, 2);
    blockConfig->writeNumber(offset & 0x07, 1);
    blockConfig->writeNumber(record.sizeInBits() - 1, 1);

    nextVariableId++;
  }

  if (blockGlobalDefault->getSize() == 0) {
    blockGlobalDefault->writeNumber(0, 1);
  }
  if (blockPersistentDefault->getSize() == 0) {
    blockPersistentDefault->writeNumber(0, 1);
  }
  if (blockTemporaryDefault->getSize() == 0) {
    blockTemporaryDefault->writeNumber(0, 1);
  }

  return true;
}

// -----------------------------------------------------------------------------
void RamVariableSchema::setVariableDefaultValue(const RamVariableSchemaRecord &record, FMA::linker::LinkerBlock *target) const {
  uint32_t bitsToWrite = qMin<uint32_t>(record.defaultValueSize(), record.sizeInBits());
  uint32_t bitOffset = record.offsetInBits();
  char *inputData = reinterpret_cast<char*>(record.defaultValue());

  // TODO: improve performance here~
  for (uint32_t i=0; i<bitsToWrite; i++) {
    bool bit = (inputData[i / 8] >> (i % 8)) & 1;
    uint32_t writeBit = bitOffset + i;
    uint8_t writeBitmask = 1 << (writeBit % 8);
    uint32_t byte = writeBit / 8;

    while (target->getSize() <= byte) {
      target->writeNumber(0, 1);
    }

    char oldValue = reinterpret_cast<char*>(target->getData())[byte];
    if (bit) {
      oldValue = oldValue | writeBitmask;
    } else {
      oldValue = oldValue & ~writeBitmask;
    }

    target->replace(&oldValue, byte, 1);
  }
}

// -----------------------------------------------------------------------------
FMA::linker::LinkerBlock *RamVariableSchema::defaultLinkerBlock(RamVariableSchemaRecord::RamVariableSchemaGroup group, FMA::linker::LinkerBlock *global, FMA::linker::LinkerBlock *persistent, FMA::linker::LinkerBlock *temporary) const {
  switch (group) {
    case RamVariableSchemaRecord::GLOBAL_PERSISTENT:
    case RamVariableSchemaRecord::GLOBAL_PERSISTENT_WITH_DEFAULT:
      return global;

    case RamVariableSchemaRecord::PERSISTENT:
    case RamVariableSchemaRecord::PERSISTENT_WITH_DEFAULT:
      return persistent;

    case RamVariableSchemaRecord::TEMPORARY:
    case RamVariableSchemaRecord::TEMPORARY_WITH_DEFAULT:
    default:
      return temporary;
  }
}

// -----------------------------------------------------------------------------
QString RamVariableSchema::getFmaCode() const {
  QStringList result;
  result << "SRAM_GLOBAL_DATA_SIZE=" + QString::number(globalDataSize());
  result << "SRAM_SAVESLOT_DATA_SIZE=" + QString::number(saveSlotDataSize());
  result << "SRAM_EXTENDED_SAVESLOT_DATA_SIZE=" + QString::number(extendedSaveSlotDataSize());
  result << "SRAM_TEMPORARY_DATA_SIZE=" + QString::number(temporaryDataSize());
  result << "SRAM_HEADER_SIZE=" + QString::number(mIdentifierSize);
  result << "NUM_SRAM_SAVESLOTS=" + QString::number(mNumSaveSlots);
  result << "Aoba::GameVariable.setup";

  Aoba::log::debug(
    "SRAM: Global " + QString::number(globalDataSize())
     + " SaveSlot " + QString::number(saveSlotDataSize())
     + " FullSaveSlot " + QString::number(extendedSaveSlotDataSize())
     + " Temporary " + QString::number(temporaryDataSize())
     + " Header " + QString::number(mIdentifierSize)
  );

  return result.join('\n');
}

// -----------------------------------------------------------------------------
bool RamVariableSchema::hasValidSize() const {
  return persistentDataSize() <= mMaxSizeInBytes;
}

// -----------------------------------------------------------------------------
uint16_t RamVariableSchema::persistentDataSize() const {
  return globalDataSize() + saveSlotDataSize() * mNumSaveSlots;
}

// -----------------------------------------------------------------------------
uint32_t RamVariableSchema::getMaxOffset(RamVariableSchemaRecord::RamVariableSchemaGroup group) const {
  uint32_t max = 0;

  for (const auto &record : mRecords) {
    if (record.group() != group) {
      continue;
    }

    max = qMax(max, record.sizeInBits() + record.offsetInBits());
  }

  return max;
}

// -----------------------------------------------------------------------------
uint16_t RamVariableSchema::globalDataSize() const {
  uint32_t sizeInBits =
    getMaxOffset(RamVariableSchemaRecord::GLOBAL_PERSISTENT_WITH_DEFAULT)
    + getMaxOffset(RamVariableSchemaRecord::GLOBAL_PERSISTENT)
    + mIdentifierSize
    + 2; //checksum

  return (sizeInBits + 7) / 8;
}

// -----------------------------------------------------------------------------
uint16_t RamVariableSchema::saveSlotDataSize() const {
  uint32_t sizeInBits = 
    getMaxOffset(RamVariableSchemaRecord::PERSISTENT_WITH_DEFAULT)
    + getMaxOffset(RamVariableSchemaRecord::PERSISTENT)
    + 2; //checksum

  return (sizeInBits + 7) / 8;
}

// -----------------------------------------------------------------------------
uint16_t RamVariableSchema::extendedSaveSlotDataSize() const {
  return (mMaxSizeInBytes - globalDataSize()) / mNumSaveSlots;
}

// -----------------------------------------------------------------------------
uint16_t RamVariableSchema::temporaryDataSize() const {
  uint32_t sizeInBits = 
    getMaxOffset(RamVariableSchemaRecord::TEMPORARY_WITH_DEFAULT)
    + getMaxOffset(RamVariableSchemaRecord::TEMPORARY);

  return (sizeInBits + 7) / 8;
}