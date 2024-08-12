#include <editor/project/Project.hpp>
#include <sft/song/Song.hpp>

using namespace Editor;

// ----------------------------------------------------------------------------
Project::Project()
  : mSong(new Sft::Song)
{
}

// ----------------------------------------------------------------------------
Project::~Project() {
  delete mSong;
}
