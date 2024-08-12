#pragma once

#include "SongLoader.hpp"
#include "../channel/Channel.hpp"
#include "../Configuration.hpp"

namespace Sft {
class Channel;
class Instrument;

class MmlSongLoader : public SongLoader {
public:
  //! Constructor
  MmlSongLoader(Song *song) : SongLoader(song) {}

  //! Loads the Song
  bool load(const QString &filename) const override;
};

}