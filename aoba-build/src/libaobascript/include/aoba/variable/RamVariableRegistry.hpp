#pragma once

namespace Aoba {
class RamVariableScope;
class RamVariableSchema;

class RamVariableRegistry {
public:
  //! Constructor
  RamVariableRegistry();

  //! Deconstructor
  ~RamVariableRegistry();

  //! Returns the root scope
  inline RamVariableScope *root() { return mRoot; }

  //! Returns the schema
  inline RamVariableSchema *schema() { return mSchema; }

protected:
  //! Global scope
  RamVariableScope *mRoot;

  //! The schema
  RamVariableSchema *mSchema;

};

}