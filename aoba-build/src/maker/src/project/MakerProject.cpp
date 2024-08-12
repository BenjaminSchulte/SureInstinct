#include <aoba/log/Log.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GameLevelAssetType.hpp>
#include <maker/project/MakerProject.hpp>
#include <maker/GlobalContext.hpp>

// -----------------------------------------------------------------------------
MakerProject::~MakerProject() {
  delete mProject;
}

// -----------------------------------------------------------------------------
MakerProjectPtr MakerProject::load(const QString &folderName) {
  Maker::GameProject *project = new Maker::GameProject(folderName, context().moduleRegistry());
  project->buildTarget().addTarget("ntsc");
  project->assetTypes().registerDefaultAssetTypes();

  if (!project->initialize() || !project->load()) {
    delete project;
    return MakerProjectPtr();
  }

  MakerProjectPtr makerProject(new MakerProject(project));
  makerProject->assetTree().reloadFromProject(project);

  return makerProject;
}

// -----------------------------------------------------------------------------
Maker::GameLevelAsset *MakerProject::getLevel(const QString &id) const {
  QStringList parts = id.split('/');
  if (parts.size() == 1) {
    Aoba::log::warn("Invalid ID for getLevel(): " + id);
    return nullptr;
  }

  Maker::AbstractGameAsset *asset = mProject->assets().getAsset(parts[0], parts[1]);
  if (!asset) {
    Aoba::log::warn("Unable to fetch asset: " + parts[0] + " " + parts[1]);
    return nullptr;
  }

  Maker::GameLevelAsset *gameAsset = dynamic_cast<Maker::GameLevelAsset*>(asset);
  if (!gameAsset) {
    Aoba::log::warn("Unable to convert asset to level asset");
    return nullptr;
  }

  return gameAsset;
}

// -----------------------------------------------------------------------------
QString MakerProject::title() const {
  return mProject->gameModule()->header().name();
}