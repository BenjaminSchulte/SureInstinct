#pragma once

#include <QPalette>

namespace Editor {

class RenderTheme {
public:
  //! Font IDs
  enum FontId {
    FontDefault,
    FontBold
  };

  //! Icon ID
  enum IconId {
    IconAdd,
    IconSpeaker
  };

  //! Constructor
  RenderTheme();

  //! Returns the palette
  inline QPalette &palette() { return mPalette; }

  //! Returns a font
  QFont font(FontId id) const;

  //! Returns the default font size
  inline int defaultFontSize() const { return 12; }

  //! Returns an icon
  QImage icon(IconId id, int size=16) const;

protected:
  //! The palette
  QPalette mPalette;
};

}