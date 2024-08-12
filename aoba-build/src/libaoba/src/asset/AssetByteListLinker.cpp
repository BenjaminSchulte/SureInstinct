#include <aoba/log/Log.hpp>
#include <aoba/asset/AssetByteListLinker.hpp>
#include <aoba/asset/AssetLinkerObject.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <fma/symbol/ConstantNumber.hpp>
#include <fma/symbol/CalculatedNumber.hpp>
#include <fma/linker/LinkerBlock.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
void AssetByteList::add(int id, const FMA::symbol::ReferencePtr record) {
  mRecords.insert(id, record);

  if (id > mMaxRecordId) {
    mMaxRecordId = id;
  }
}

// -----------------------------------------------------------------------------
void AssetByteList::add(int id, int number) {
  add(id, FMA::symbol::ReferencePtr(new FMA::symbol::ConstantNumber(number)));
}

// -----------------------------------------------------------------------------
void AssetByteList::addSymbol(int id, const QString &name, int shift) {
  FMA::symbol::ReferencePtr symbol(new FMA::symbol::SymbolReference(name.toStdString()));

  if (shift > 0) {
    FMA::symbol::ReferencePtr shiftNumber(new FMA::symbol::ConstantNumber(shift));
    FMA::symbol::ReferencePtr shiftedSymbol(new FMA::symbol::CalculatedNumber(
      symbol,
      FMA::symbol::CalculatedNumber::RSHIFT,
      shiftNumber
    ));
    add(id, shiftedSymbol);
  } else {
    add(id, symbol);
  }
}

// -----------------------------------------------------------------------------
AssetByteListLinker::~AssetByteListLinker() {
  for (AssetByteList *list : mLists) {
    delete list;
  }
}

// -----------------------------------------------------------------------------
AssetByteList *AssetByteListLinker::createList(const QString &name) {
  AssetByteList *list = new AssetByteList(name);
  mLists.push_back(list);
  return list;
}

// -----------------------------------------------------------------------------
QStringList AssetByteListLinker::link(const QString &id, Project *project) {
  AssetLinkerObject obj(id, project);
  
  if (mBytesPerRecord != mAlignToBytes) {
    Aoba::log::todo("AssetByteListLinker should merge lists better");
  }

  for (AssetByteList *list : mLists) {
    FMA::linker::LinkerBlock *block = obj.createLinkerBlock(list->name());

    const auto &map(list->records());
    int size = list->size();

    for (int i=0; i<size; i++) {
      if (map.contains(i)) {
        block->write(map[i], mAlignToBytes);
      } else {
        block->writeNumber(0, mAlignToBytes);
      }
    }
  }

  return obj.getFmaObjectFiles();
}