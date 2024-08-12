#pragma once

#include <QString>

namespace FMA {
namespace linker {
class LinkerBlock;
}
}

namespace Aoba {
class RamVariableScope;
class RamVariable;
class UsageMap;
class ScriptProject;

struct RamVariableSchemaRecord {
  enum RamVariableSchemaGroup {
    // All variables which are not limited to a savegame
    GLOBAL_PERSISTENT_WITH_DEFAULT,

    // All variables which are not limited to a savegame
    GLOBAL_PERSISTENT,

    // Saved to the SRAM and initialized at first game start
    PERSISTENT_WITH_DEFAULT,

    // Saved to the SRAM filled with random content
    PERSISTENT,

    // Only available during gameplay
    TEMPORARY_WITH_DEFAULT,

    // Only available during gameplay
    TEMPORARY,

    // Number of schema groups
    NUM_SCHEMA_GROUPS,

    // Invalid group
    INVALID
  };

  //! Constructor
  RamVariableSchemaRecord() {}

  //! Copy Constructor
  RamVariableSchemaRecord(const RamVariableSchemaRecord &);

  //! Constructor
  RamVariableSchemaRecord(const QString &id, RamVariableSchemaGroup group, uint32_t offset, uint32_t size)
    : mId(id)
    , mGroup(group)
    , mOffset(offset)
    , mSize(size)
  {}

  //! Deconstructor
  ~RamVariableSchemaRecord();

  //! Copy
  RamVariableSchemaRecord &operator=(const RamVariableSchemaRecord &) = default;

  //! Returns the ID of this record
  inline const QString &id() const { return mId; }

  //! Returns the group of this record
  inline RamVariableSchemaGroup group() const { return mGroup; }

  //! Returns the offset in bits of this record
  inline uint32_t offsetInBits() const { return mOffset; }

  //! Returns the size in bits of this record
  inline uint32_t sizeInBits() const { return mSize; }

  //! Returns whether this variable is actually required or if it's a leftover from an old version
  inline bool hasActualUsage() const { return mHasUsage; }

  //! Whether this record has a default value
  inline bool hasDefaultValue() const {
    return mDefaultData != nullptr;
  }

  //! Sets the default value
  void setDefaultValue(void *data, uint32_t size);

  //! Returns the default data
  inline void *defaultValue() const { return mDefaultData; }

  //! Returns the default value size in bits
  inline uint32_t defaultValueSize() const { return mDefaultDataSize; }

  //! Marks this record as used
  inline void markUsed() { mHasUsage = true; }

private:
  //! The ID
  QString mId;

  //! The group
  RamVariableSchemaGroup mGroup;

  //! The offset
  uint32_t mOffset;

  //! The size
  uint32_t mSize;
  
  //! Whether this record has usage
  bool mHasUsage = false;  

  //! Default data
  void *mDefaultData = nullptr;

  //! Default data size
  uint32_t mDefaultDataSize = 0;
};

class RamVariableSchema {
public:
  struct RamVariableCreateDescriptor {
    bool mIsShared = false;
    int mOffset = 0;
    int mSizeLeft = 0;
  };

  //! Constructor
  RamVariableSchema(uint16_t maxSizeInBytes, uint16_t numSaveSlots, uint8_t identifierSize);

  //~ Deconstructor
  ~RamVariableSchema();

  //! Restores the schema from a file
  bool restoreFromFile(const QString &fileName);

  //! Stores the schema into a file
  bool storeToFile(const QString &fileName) const;

  //! Adds all records from a scope tree
  bool addFromScope(RamVariableScope *scope);

  //! Adds all records from a scope tree
  int addVariable(RamVariable *v, const RamVariableCreateDescriptor &desc, bool &valid);

  //! Returns whether the size is valid
  bool hasValidSize() const;

  //! Returns the total data size
  uint16_t persistentDataSize() const;

  //! Returns the size of the global data in bytes
  uint16_t globalDataSize() const;

  //! Returns the size of the global data in bytes
  uint16_t saveSlotDataSize() const;

  //! Returns the size of the global data in bytes
  uint16_t extendedSaveSlotDataSize() const;

  //! Returns the size of the global data in bytes
  uint16_t temporaryDataSize() const;

  //! Builds this schema
  bool build(ScriptProject *project) const;

  //! Returns FMA code
  QString getFmaCode() const;

  //! Returns the ID of an record
  int idOfVariable(const QString &id);

protected:
  //! Returns the correct linker block
  FMA::linker::LinkerBlock *defaultLinkerBlock(RamVariableSchemaRecord::RamVariableSchemaGroup group, FMA::linker::LinkerBlock *global, FMA::linker::LinkerBlock *persistent, FMA::linker::LinkerBlock *temporary) const;

  //! Sets a default global
  void setVariableDefaultValue(const RamVariableSchemaRecord &record, FMA::linker::LinkerBlock *target) const;

  //! Adds all records from a scope tree
  int addFromScopeWithDescriptor(RamVariableScope *scope, RamVariableCreateDescriptor &desc, bool &valid);

  //! Adds all records from a scope tree
  int addFromSharedScopeWithDescriptor(RamVariableScope *scope, RamVariableCreateDescriptor &desc, bool &valid);

  //! Adds all records from a scope tree
  int addFromNestedSharedScopeWithDescriptor(RamVariableScope *scope, RamVariableCreateDescriptor &desc, bool &valid);

  //! Returns the offset within the requested ram block
  RamVariableSchemaRecord::RamVariableSchemaGroup groupForScope(RamVariableScope *scope, RamVariable *v) const;

  //! Returns the offset within the requested ram block
  int allocateRam(RamVariableSchemaRecord::RamVariableSchemaGroup group, int sizeInBits);

  //! Looks up for the maximum position within the data buffer
  uint32_t getMaxOffset(RamVariableSchemaRecord::RamVariableSchemaGroup group) const;

  //! Maximum size in bytes
  uint16_t mMaxSizeInBytes;

  //! Maximum number of save slots
  uint16_t mNumSaveSlots;
  
  //! Usage maps for all
  UsageMap *mUsageMap[RamVariableSchemaRecord::NUM_SCHEMA_GROUPS];

  //! RamVariable schema record for restoring schemas from JSON
  QMap<QString, RamVariableSchemaRecord> mRecords;

  //! RamVariable IDs
  QStringList mVariableIds;

  //! The identifier
  uint8_t mIdentifierSize;
};

}