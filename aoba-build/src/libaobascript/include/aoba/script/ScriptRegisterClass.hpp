#pragma once

#include <fma/types/Class.hpp>
#include <fma/types/RootModule.hpp>

namespace Aoba {

class ScriptRegisterClass : public FMA::types::Class {
public:
  static FMA::types::ClassPtr create(const FMA::types::RootModulePtr &root, const FMA::types::ClassPtr &Class);
  static FMA::interpret::ResultPtr createInstance(const FMA::interpret::ContextPtr &context, const std::string &value);

  static FMA::interpret::ResultPtr initialize(const FMA::interpret::ContextPtr &context, const FMA::interpret::GroupedParameterList &parameter);
};

}
