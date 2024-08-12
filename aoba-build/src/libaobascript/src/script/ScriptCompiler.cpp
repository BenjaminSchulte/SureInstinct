#include <aoba/log/Log.hpp>
#include <fma/Parser.hpp>
#include <fma/interpret/Interpreter.hpp>
#include <fma/plugin/Plugin.hpp>
#include <fma/core/Plugin.hpp>
#include <aoba/script/ScriptProject.hpp>
#include <aoba/script/ScriptCompiler.hpp>
#include <aoba/script/ScriptLanguage.hpp>
#include <aoba/script/ScriptMemoryAdapter.hpp>

using namespace Aoba;

// ----------------------------------------------------------------------------
ScriptCompiler::ScriptCompiler(ScriptProject *project)
  : mProject(project)
  , mInterpreter(&project->fmaProject())
{
  mMemoryPlugin = std::shared_ptr<ScriptMemoryPlugin>(new ScriptMemoryPlugin(project, &project->fmaProject()));
  mLanguagePlugin = std::shared_ptr<ScriptLanguagePlugin>(new ScriptLanguagePlugin(project));
}

// ----------------------------------------------------------------------------
ScriptCompiler::~ScriptCompiler() {
}

// ----------------------------------------------------------------------------
bool ScriptCompiler::initialize() {
  if (!loadPlugin(FMA::PluginPtr(new FMA::core::CorePlugin(&mProject->fmaProject())))) {
    return false;
  }

  if (!loadPlugin(mMemoryPlugin)) {
    return false;
  }

  if (!loadPlugin(mLanguagePlugin)) {
    return false;
  }

  mProject->fmaProject().setMemoryAdapter(mMemoryPlugin->createAdapter());

  return true;
}

// ----------------------------------------------------------------------------
bool ScriptCompiler::loadPlugin(const FMA::PluginPtr &plugin) {
  Aoba::log::debug("[FMA] Loading plugin:  " + QString(plugin->getName()));

  if (!plugin->initialize()) {
    Aoba::log::warn("[FMA] Could not initialize plugin:  " + QString(plugin->getName()));
    return false;
  }

  Aoba::log::debug("[FMA] Plugin has been loaded:  " + QString(plugin->getName()));
  return true;
}

// ----------------------------------------------------------------------------
bool ScriptCompiler::parseAndInterpretFile(const QString &fileName) {
  FMA::Parser parser(&mProject->fmaProject());

  Aoba::log::debug("Parsing input file:  " + fileName);
  if (!parser.parseFile(fileName.toStdString().c_str())) {
    Aoba::log::warn("Failed to parse input file:  " + fileName);
    for(auto const& error: parser.getErrors()) {
      Aoba::log::warn(QString::number(error.file.line) + ":" + QString::number(error.file.col) + ": " + QString(error.message.c_str()));
    }

    return false;
  }

  mInterpreter.execute(parser.getResult());
  if (mProject->fmaProject().log().hasErrors()) {
    return false;
  }

  return true;
}

// ----------------------------------------------------------------------------