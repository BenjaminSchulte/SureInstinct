#include <QDebug>
#include <QFile>
#include <aoba/log/Log.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GameLinkResult.hpp>
#include <aoba/asset/AssetByteListLinker.hpp>

using namespace Maker;
using namespace Aoba;

// -----------------------------------------------------------------------------
GameProject::GameProject(const QDir &path, GameModuleRegistry *moduleRegistry)
  : mPath(path)
  , mModuleRegistry(GameModuleRegistry(moduleRegistry))
  , mLoadedModules(&mModuleRegistry)
  , mBuildTarget(this)
  , mScripts(this)
  , mAssets(this)
  , mAssetTypes(this)
  , mMessages(this)
{
  mModuleRegistry.findModules(mPath.absoluteFilePath("modules"));
}

// -----------------------------------------------------------------------------
GameProject::~GameProject() {
}

// -----------------------------------------------------------------------------
bool GameProject::initialize() {
  return true;
}

// -----------------------------------------------------------------------------
void GameProject::loadModuleProperties() {
  for (const GameModule *mod : mModule->allModules()) {
    QMapIterator<QString, GameModulePropertyDefinition> it(mod->properties().properties());
    while (it.hasNext()) {
      it.next();
      mProperties.insert(it.key(), it.value().defaultValue());
    }
  }
}

// -----------------------------------------------------------------------------
bool GameProject::load() {
  mModule = mLoadedModules.load(mPath);
  if (!mModule) {
    mLog.error("Could not load project module");
    return false;
  }

  loadModuleProperties();

  if (!mBuildTarget.initialize()) {
    mLog.error("Could not initialize build target configuration");
    return false;
  }

  if (!mScripts.initialize()) {
    mLog.error("Could not initialize script compiler");
    return false;
  }

  if (!mScripts.load()) {
    mLog.error("Could not initialize script compiler");
    return false;
  }

  if (!mAssetTypes.load()) {
    mLog.error("Could not load config/asset_types.yml");
    return false;
  }

  if (!mAssets.load()) {
    mLog.error("Could not load config/assets.yml");
    return false;
  }

  if (!mMessages.configure()) {
    mLog.error("Could not load config/locales.yml");
    return false;
  }

  if (!mAssets.loadAssets()) {
    mLog.error("Could not load assets");
    return false;
  }
  
  if (!mMessages.load()) {
    mLog.error("Could not load config/global_messages.yml");
    return false;
  }

  if (!mScripts.loadQueued()) {
    mLog.error("Failed to load all scripts");
    return false;
  }

  if (!mMessages.loadTranslations()) {
    mLog.warning("Unable to load translations from input files");
  }

  return true;
}

// -----------------------------------------------------------------------------
bool GameProject::build() {
  if (!mAssets.prepareGlobalAssets()) {
    return false;
  }

  if (!mMessages.build()) {
    mLog.error("Could not build messages");
    return false;
  }

  if (!mAssets.buildAssets()) {
    mLog.error("Could not build assets");
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
QString GameProject::gitCommit() const {
  std::array<char, 128> buffer;
  std::string response;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("git rev-parse HEAD", "r"), pclose);
  if (!pipe) {
    Aoba::log::debug("Could not fetch project version");
    return "00000000000000000000000000000000";
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    response += buffer.data();
  }

  QString message(response.c_str());
  return message.left(32);
}

// -----------------------------------------------------------------------------
void GameProject::lookupTable(const QString &type, const QString &id) {
  mScripts.lookupTable(type, id);
}

// -----------------------------------------------------------------------------
int GameProject::lookupTable(const QString &type, const QString &id, const QString &key, int value) {
  return mScripts.lookupTable(type, id, key, value);
}

// -----------------------------------------------------------------------------
void GameProject::linkProjectVersion(GameLinkResult &result) {
  result.addCode("GIT_COMMIT_HASH=\"" + gitCommit() + "\"");
}

// -----------------------------------------------------------------------------
void GameProject::linkAssetLists(const QString &id, GameLinkResult &result, AssetByteListLinker *lists) {
  for (const QString &obj : lists->link(id, this)) {
    result.addObject(obj);
  }
}

// -----------------------------------------------------------------------------
QString GameProject::getScriptSymbolName(const QString &func) const {
  return mScripts.getScriptSymbolName(func);
}

// -----------------------------------------------------------------------------
bool GameProject::link() {
  QStringList buildConfig;
  buildConfig.push_back("module Aoba");
  buildConfig.push_back("end");
  buildConfig.push_back("Aoba::OBJECT_PATH=\"" + mPath.absoluteFilePath("obj") + "\"");

  QStringList includeList;
  for (GameModule *mod : mModule->dependencies()) {
    includeList += mod->header().asmIncludePath();
    QString content(mod->buildFmaCode(this));
    if (!content.isEmpty()) {
      buildConfig.push_back(content);
    }
  }
  includeList += mModule->header().asmIncludePath();
  
  GameLinkResult result;
  if (!mAssets.linkAssets(result)) {
    mLog.error("Failed to link assets");
    return false;
  }

  if (!mAssetTypes.linkTypes(result)) {
    mLog.error("Failed to link asset types");
    return false;
  }

  if (!mScripts.linkScripts(result)) {
    mLog.error("Failed to link scripts");
    return false;
  }
  
  if (!mMessages.linkMessages(result)) {
    mLog.error("Failed to link messages");
    return false;
  }

  linkAssetLists("assetByteListLinker8", result, mAssetByteListLinker8);
  linkAssetLists("assetByteListLinker16", result, mAssetByteListLinker16);

  linkProjectVersion(result);

  QFile objectsFile(objFile("compile-flags.txt"));
  if (!objectsFile.open(QIODevice::WriteOnly)) {
    std::cerr << "Could not open compile-flags.txt for writing" << std::endl;
    return 1;
  }
  if (!includeList.isEmpty()) {
    objectsFile.write("-I" + includeList.join(" -I").toLatin1() + " ");
  }
  if (result.objects().size()) {
    objectsFile.write("-S" + result.objects().join(" -S").toLatin1());
  }

  buildConfig.push_back(result.code());

  buildConfig.push_back(mModule->buildFmaCode(this));

  QFile buildConfigFile(objFile("build.fma"));
  if (!buildConfigFile.open(QIODevice::WriteOnly)) {
    mLog.error("Could not open build.fma for writing");
    return false;
  }
  buildConfigFile.write(buildConfig.join('\n').toLatin1());

  if (!mMessages.saveTranslations()) {
    mLog.warning("Unable to generate translation files");
  }
  return true;
}

// -----------------------------------------------------------------------------
QString GameProject::relativeAssetFile(const QString &fileName) const {
  // /media/sym/aaaa/blah
  // /home/aaaa/blah

  // Tries to find a valid path
  QString directPath = QFileInfo(fileName).canonicalFilePath();
  if (!directPath.isEmpty()) {
    return QDir(assetPath().canonicalPath()).relativeFilePath(directPath);
  }

  if (fileName.startsWith(assetPath().absolutePath())) {
    return fileName.mid(assetPath().absolutePath().length() + 1);
  }
  if (fileName.startsWith(assetPath().canonicalPath())) {
    return fileName.mid(assetPath().canonicalPath().length() + 1);
  }

  // Try to find the asset folder
  QStringList postfix({QFileInfo(fileName).fileName()});
  QDir dir(QFileInfo(fileName).dir());
  while (!dir.isRoot() && dir.dirName() != "assets") {
    postfix.push_front(dir.dirName());
    dir.cdUp();
  }

  // Could not find any directory
  if (dir.isRoot()) {
    return fileName;
  }

  return postfix.join(QDir::separator());
}