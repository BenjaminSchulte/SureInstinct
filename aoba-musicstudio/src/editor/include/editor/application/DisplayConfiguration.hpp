#pragma once

#include "../render/RenderTheme.hpp"

namespace Editor {

class DisplayConfiguration {
public:
  //! Constructor
  DisplayConfiguration();

  //! Deconstructor
  ~DisplayConfiguration();

  //! Returns the theme
  inline RenderTheme *theme() const { return mTheme; }

  //! defaultTextMargin
  inline int defaultTextMargin() const { return 6; }

  //! listItemHeight
  inline int listItemHeight() const { return mTheme->defaultFontSize() + defaultTextMargin() * 2; }

  //! The piano key height
  inline int pianoKeyHeight() const { return 16; }

  //! The piano width
  inline int pianoWidth() const { return 100; }

  //! The piano header height
  inline int pianoHeaderHeight() const { return pianoKeyHeight(); }

  //! The width of a measure tick
  inline int measureTickWidth() const { return 16; }

protected:
  //! The theme
  RenderTheme *mTheme;
};

}