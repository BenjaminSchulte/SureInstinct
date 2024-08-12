#pragma once

#include "SongWriter.hpp"
#include "../channel/Channel.hpp"
#include "../Configuration.hpp"

namespace Sft {
class Channel;
class Instrument;

class MmlSongWriter : public SongWriter {
public:
  //! Constructor
  MmlSongWriter(Song *song) : SongWriter(song) {}

  //! Loads the Song
  bool save(const QString &filename) const override;
};

}