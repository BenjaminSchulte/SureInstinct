#pragma once

#include "SongLoader.hpp"
#include "../channel/Channel.hpp"
#include "../Configuration.hpp"

namespace Sft {
class Channel;
class Instrument;
class DataStream;

class XmSongLoader : public SongLoader {
public:
  //! Constructor
  XmSongLoader(Song *song) : SongLoader(song) {}

  //! Loads the Song
  bool load(const QString &filename) const override;
};

}