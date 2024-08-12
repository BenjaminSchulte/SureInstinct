#pragma once

#include <QString>
#include <QVector>
#include <QMap>
#include <fma/symbol/Reference.hpp>

namespace Aoba {
class Project;

class AssetByteList {
public:
  //! Constructor
  AssetByteList(const QString &name) : mName(name) {}

  //! Returns the name
  inline const QString &name() const { return mName; }

  //! Returns the size
  inline int size() const { return mMaxRecordId + 1; }

  //! Adds a record
  void add(int id, const FMA::symbol::ReferencePtr record);

  //! Adds a record
  void add(int id, int number);

  //! Adds a record
  void addSymbol(int id, const QString &symbol, int shift=0);

  //! Returns all records
  inline const QMap<int, FMA::symbol::ReferencePtr> &records() const {
    return mRecords;
  }

private:
  //! The name
  QString mName;

  //! The maximum record ID
  int mMaxRecordId = -1;

  //! List of all references
  QMap<int, FMA::symbol::ReferencePtr> mRecords;
};

class AssetByteListLinker {
public:
  //! Constructor
  AssetByteListLinker(int bytesPerRecord, int alignToBytes) : mBytesPerRecord(bytesPerRecord), mAlignToBytes(alignToBytes) {}

  //! Deconstructor
  ~AssetByteListLinker();

  //! Constructor
  AssetByteList *createList(const QString &name);

  //! Links the lists
  QStringList link(const QString &id, Project *project);

private:
  //! Number of bytes per record
  int mBytesPerRecord;

  //! Aligns to bytes
  int mAlignToBytes;

  //! List of all lists
  QVector<AssetByteList*> mLists;
};

}