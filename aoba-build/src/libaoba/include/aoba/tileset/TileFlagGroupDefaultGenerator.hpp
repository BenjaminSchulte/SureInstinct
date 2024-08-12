#pragma once

#include <QString>
#include "TileFlagGroupConfig.hpp"

namespace Aoba {

struct TileFlagGroupDefaultGeneratorConfig {
  //! Returns an numeric value of an parameter
  QString numericParameter;
};

class TileFlagGroupDefaultGenerator : public TileFlagGroupGenerator {
public:
  //! Constructor
  TileFlagGroupDefaultGenerator(const TileFlagGroupDefaultGeneratorConfig &config)
    : mConfig(config) {}

  //! Returns the configuration
  inline TileFlagGroupDefaultGeneratorConfig &config() { return mConfig; }

  //! Generates data
  bool generate(const TileFlagGroupTile &, uint64_t &value) const override;

protected:
  //! The configuration
  TileFlagGroupDefaultGeneratorConfig mConfig;
};

}
