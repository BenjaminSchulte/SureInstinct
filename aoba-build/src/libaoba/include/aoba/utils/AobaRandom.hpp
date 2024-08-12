#pragma once

#include <random>

namespace Aoba {

class AobaRandom {
public:
  //! Constructor
  AobaRandom(uint_fast32_t seed) {
    mEngine.seed(seed);
  }

  //! Returns a random value
  inline double random() {
    std::uniform_real_distribution<> dist(0, 1);
    return dist(mEngine);
  }

  //! Returns a random value
  inline int random(int max) { return randomRange(0, max - 1); }

  //! Returns a random value
  int randomRange(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(mEngine);
  }

private:
  //! Random engine
  std::ranlux24_base mEngine;
};

}