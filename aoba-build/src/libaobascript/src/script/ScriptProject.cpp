#include <aoba/log/Log.hpp>
#include <aoba/variable/RamVariableRegistry.hpp>
#include <aoba/script/ScriptProject.hpp>
#include <aoba/script/ScriptMemoryBlock.hpp>
#include <fma/types/RootModule.hpp>
#include <fma/plugin/MemoryPluginAdapter.hpp>
#include <iostream>

using namespace Aoba;

// ----------------------------------------------------------------------------
ScriptProject::ScriptProject()
  : mVariables(new RamVariableRegistry())
{}

// ----------------------------------------------------------------------------
ScriptProject::~ScriptProject() {
  delete mVariables;
}

// ----------------------------------------------------------------------------
QString ScriptProject::getScriptSymbolName(const QString &funcName) const {
  QString parent = "root";
  FMA::types::TypePtr func = mProject.root();

  QStringList outerParts(funcName.split("::"));
  for (int i=0; i<outerParts.size(); i++) {
    QStringList parts(outerParts[i].split('.'));
    for (int j=0; j<parts.size(); j++) {

      if (!func->hasMember(parts[j].toStdString())) {
        mProject.log().error() << "Could not find member: " << parts[j].toStdString() << " in " << parent.toStdString();
        return "";
      }

      func = func->getMember(parts[j].toStdString());
      parent = parent + "::" + parts[j];
    }
  }

  if (!func->isObjectOfType("Function")) {
    mProject.log().error() << "Found symbol " << funcName.toStdString() << " but it is no 'Function' class";
    return "";
  }

  //FMA::types::ObjectPtr funcObj = func->asObject();
  FMA::plugin::MemoryBlock *block = FMA::plugin::MemoryBlock::getFromType(func);
  if (!block) {
    mProject.log().error() << "Found symbol " << funcName.toStdString() << " but could not find memory block";
    return "";
  }

  ScriptMemoryBlock *scriptBlock = dynamic_cast<ScriptMemoryBlock*>(block);
  return scriptBlock->scriptId();
}
