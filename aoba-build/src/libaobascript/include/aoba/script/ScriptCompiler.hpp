#pragma once

#include <QString>
#include <memory>
#include <fma/interpret/Interpreter.hpp>

namespace FMA {
namespace plugin {
class Plugin;
}
typedef std::shared_ptr<FMA::plugin::Plugin> PluginPtr;
}

namespace Aoba {
class ScriptProject;
class ScriptMemoryPlugin;
class ScriptLanguagePlugin;

class ScriptCompiler {
public:
  //! Constructor
  ScriptCompiler(ScriptProject *project);

  //! Deconstructor
  ~ScriptCompiler();

  //! Load
  bool initialize();

  //! Loads a plugin
  bool loadPlugin(const FMA::PluginPtr &);

  //! Parses a file
  bool parseAndInterpretFile(const QString &fileName);

protected:
  //! The project
  ScriptProject *mProject;

  //! The memory manager
  std::shared_ptr<ScriptMemoryPlugin> mMemoryPlugin;

  //! The language plugin
  std::shared_ptr<ScriptLanguagePlugin> mLanguagePlugin;

  //! The interpreter
  FMA::interpret::Interpreter mInterpreter;
};

}