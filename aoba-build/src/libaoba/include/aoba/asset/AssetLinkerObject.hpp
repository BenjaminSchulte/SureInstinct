#pragma once

#include <QString>
#include <QVector>
#include <QStringList>
#include <fma/Log.hpp>
#include <fma/linker/LinkerObject.hpp>

namespace Aoba {
class Project;

class AssetLinkerObject {
public:
  //! Constructor
  AssetLinkerObject(const QString &id, Project *project);

  //! Returns the linker object
  inline FMA::linker::LinkerObject &linkerObject() {
    return mLinkerObject;
  }

  //! Creates a new linker block
  FMA::linker::LinkerBlock *createLinkerBlock(const QString &id, int bank=-1);

  //! Returns all linker object files
  QStringList getFmaObjectFiles();

  //! Returns whether the object has been linked
  inline bool hasBeenLinked() {
    return !mNeedsObjectLinking;
  }

  //! Returns the file postfix
  QString postfix();

  //! Returns the filename
  QString fileName();

protected:
  //! The asset type ID
  QString mAssetTypeId;

  //! The asset number
  int mFilePostfix = -1;
  
  //! The project
  Project *mProject;

  //! The shared linker object
  FMA::linker::LinkerObject mLinkerObject;

  //! True until needsObjectLinking() is called
  bool mNeedsObjectLinking = true;
  
  //! List of all blocks
  QVector<FMA::linker::LinkerBlock*> mBlocks;
};

}