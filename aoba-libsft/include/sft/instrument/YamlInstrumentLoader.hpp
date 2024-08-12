#pragma once

#include "InstrumentLoader.hpp"

namespace Sft {

class YamlInstrumentLoader : public InstrumentLoader {
public:  
  //! Constructor
  YamlInstrumentLoader(Song *song) : InstrumentLoader(song) {}

  //! Loads an instrument
  InstrumentPtr load(const QString &fileName);
};

}