#include <editor/application/DisplayConfiguration.hpp>
#include <editor/render/RenderTheme.hpp>

using namespace Editor;

// -----------------------------------------------------------------------------
DisplayConfiguration::DisplayConfiguration()
  : mTheme(new RenderTheme)
{
}

// -----------------------------------------------------------------------------
DisplayConfiguration::~DisplayConfiguration() {
  delete mTheme;
}
