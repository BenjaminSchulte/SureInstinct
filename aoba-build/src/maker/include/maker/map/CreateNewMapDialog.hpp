#pragma once

#include <QVector>
#include <QDialog>
#include <QSharedPointer>

namespace Ui {
class CreateNewMapDialog;
}

namespace Aoba {
class LevelAssetSet;
class TilemapAssetSet;
}

namespace Maker {
class GameLevelAssetType;
class GameLevelAsset;
}

typedef QSharedPointer<class MakerProject> MakerProjectPtr;

class CreateNewMapDialog : public QDialog {
	Q_OBJECT
	
public:
  //! Constructor
	explicit CreateNewMapDialog(const MakerProjectPtr &project, QWidget *parent = nullptr);

  //! Deconstructor
	~CreateNewMapDialog();

  //! Returns the id
  QString id() const;

  //! Returns the name
  QString name() const;

  //! Creates the level
  Maker::GameLevelAsset *level();

public slots:
  //! Validates the input
  void revalidate();

private slots:
  //! Sets the ID from the given name
  void setIdFromName();

  //! Level type changed
  void on_typeSelect_currentIndexChanged(int);

private:
  //! Updates the UI
  void updateUi();

  //! Loads all tilesets
  void loadTilesets();

  //! Returns the id
  QString safeTilemapId(const QString &id) const;

  //! The project
  MakerProjectPtr mProject;

  //! The UI
	Ui::CreateNewMapDialog *mUi;

  //! The last automated ID given
  QString mAutomatedId;

  //! Valid
  bool mValid = false;

  //! The generated map
  Maker::GameLevelAsset *mResult = nullptr;

  //! The tilemap type
  Aoba::LevelAssetSet *mLevel = nullptr;

  //! The tilemap type
  const Aoba::TilemapAssetSet *mTilemap = nullptr;

  //! List of all types
  QVector<Maker::GameLevelAssetType*> mLevelTypes;
};


