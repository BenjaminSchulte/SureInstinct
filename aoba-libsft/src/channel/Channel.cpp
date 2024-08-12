#include <QDebug>
#include <sft/mml/MmlScript.hpp>
#include <sft/channel/Channel.hpp>

using namespace Sft;

// ----------------------------------------------------------------------------
Channel::Channel(Song *song, const QString &name, int index)
  : mSong(song)
  , mScript(new MmlScript(song))
  , mName(name)
  , mIndex(index)  
{
}

// ----------------------------------------------------------------------------
Channel::~Channel() {
  delete mScript;
}
