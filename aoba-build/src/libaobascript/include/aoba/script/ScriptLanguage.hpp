#pragma once

#include "./ScriptProject.hpp"
#include <fma/plugin/Plugin.hpp>
#include <fma/interpret/BaseContext.hpp>
#include <fma/interpret/ParameterList.hpp>

namespace Aoba {

// -----------------------------------------------------------------------------
class ScriptLanguagePlugin : public FMA::plugin::LanguagePlugin {
public:
  //! Constructor
  ScriptLanguagePlugin(ScriptProject *project)
    : FMA::plugin::LanguagePlugin(&project->fmaProject())
    , mProject(project)
  {}

  //! Returns the plugin type
  FMA::plugin::PluginType getPluginType() const override {
    return FMA::plugin::TYPE_LANGUAGE;
  }

  //! Returns the name
  const char *getName() const override {
    return "AobaScript Language";
  }

  //! Return the description
  const char *getDescription() const override {
    return "";
  }

  //! Initialize
  bool initialize() override;

  //! Release
  void release() override {}

protected:
  //! Requires an asset
  static FMA::interpret::ResultPtr __engine_variable(const FMA::interpret::ContextPtr &context, const FMA::interpret::GroupedParameterList &parameter);

  //! Requires an asset
  static FMA::interpret::ResultPtr __asset(const FMA::interpret::ContextPtr &context, const FMA::interpret::GroupedParameterList &parameter);

  //! Adds a translation
  static FMA::interpret::ResultPtr __translate(const FMA::interpret::ContextPtr &context, const FMA::interpret::GroupedParameterList &parameter);

  //! Adds a translation
  static FMA::interpret::ResultPtr __translate_comment(const FMA::interpret::ContextPtr &context, const FMA::interpret::GroupedParameterList &parameter);

  //! Creates a parameter
  static FMA::interpret::ResultPtr __param(const FMA::interpret::ContextPtr &context, const FMA::interpret::GroupedParameterList &parameter);

  //! Creates a parameter
  static FMA::interpret::ResultPtr is_param(const FMA::interpret::ContextPtr &context, const FMA::interpret::GroupedParameterList &parameter);

  //! Creates a scope
  static FMA::interpret::ResultPtr __scope(const FMA::interpret::ContextPtr &context, const FMA::interpret::GroupedParameterList &parameter);

  //! Creates a variable
  static FMA::interpret::ResultPtr __declare(const FMA::interpret::ContextPtr &context, const FMA::interpret::GroupedParameterList &parameter);

  //! Adds the return automatically
  static FMA::interpret::ResultPtr auto_insert_rts(const FMA::interpret::ContextPtr &context, const FMA::interpret::GroupedParameterList &parameter);

  //! The script project
  ScriptProject *mProject;
};

}
