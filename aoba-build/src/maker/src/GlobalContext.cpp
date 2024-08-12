#include <maker/GlobalContext.hpp>
#include <maker/tileset/TileRenderCache.hpp>

GlobalContext *GlobalContext::mGlobalInstance = nullptr;

// -----------------------------------------------------------------------------
GlobalContext &context() {
  return *GlobalContext::get();
}

// -----------------------------------------------------------------------------
GlobalContext::GlobalContext()
  : mTileRenderCache(new TileRenderCache())
  , mProjectLocalConfiguration(MakerProjectPtr())
{
  mModuleRegistry.findModulesFromEnvironmentVariables();
}

// -----------------------------------------------------------------------------
void GlobalContext::setProject(const MakerProjectPtr &project) {
  mProject = project;
  mProjectLocalConfiguration = ProjectLocalConfiguration(project);
  mProjectLocalConfiguration.load();
}

// -----------------------------------------------------------------------------
GlobalContext *GlobalContext::get() {
  if (mGlobalInstance == nullptr) {
    mGlobalInstance = new GlobalContext();
  }

  return mGlobalInstance;
}
