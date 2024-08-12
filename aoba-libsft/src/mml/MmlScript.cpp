#include <QDebug>
#include <sft/mml/MmlScript.hpp>
#include <sft/mml/MmlParser.hpp>
#include <sft/mml/MmlWritableExecutedScript.hpp>
#include <sft/mml/MmlWritableCompiledScript.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
MmlScript::~MmlScript() {
  delete mCompiled;
  delete mExecuted;
}

// -----------------------------------------------------------------------------
void MmlScript::clear() {
  mCode = "";
  mInvalid = true;
}

// -----------------------------------------------------------------------------
void MmlScript::set(const QString &code) {
  clear();
  append(code);
}

// -----------------------------------------------------------------------------
void MmlScript::append(const QString &code) {
  insert(mCode.length(), code);
}

// -----------------------------------------------------------------------------
void MmlScript::insert(int position, const QString &code) {
  mCode = mCode.left(position) + code + mCode.mid(position);
  mInvalid = true;
}

// -----------------------------------------------------------------------------
void MmlScript::compile() {
  delete mCompiled;
  delete mExecuted;
  
  mCompiled = new MmlWritableCompiledScript(mSong);

  MmlParser parser(mCompiled);
  parser.loadFromString(mCode);

  mExecuted = mCompiled->execute();
  mInvalid = false;
}