#include <maker/game/GameProject.hpp>
#include <maker/game/GameModuleRegistry.hpp>
#include <aoba/log/Log.hpp>
#include <aoba/log/ConsoleLog.hpp>

int main(int argc, char **argv) {
  Aoba::ConsoleLog consoleLog;
  Aoba::Log::instance()->addAdapter(&consoleLog);

  if (argc < 3) {
    Aoba::log::error("Missing project path and build target as argument");
    return 1;
  }

  Maker::GameModuleRegistry modules;
  modules.findModulesFromEnvironmentVariables();

  Maker::GameProject project((QString(argv[1])), &modules);
  project.buildTarget().addTarget(QString(argv[2]));
  project.assetTypes().registerDefaultAssetTypes();

  if (!project.initialize() || !project.load()) {
    return 1;
  }

  if (!project.build() || !project.link()) {
    return 1;
  }

  return 0;
}
