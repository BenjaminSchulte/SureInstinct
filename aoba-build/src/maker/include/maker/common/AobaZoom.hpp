#pragma once

class AobaZoom {
public:
  //! Constructor
  AobaZoom(float zoom=2.0f) : mZoom(zoom) {}

  //! Returns the zoom
  inline float get() const { return mZoom; }

  //! Sets the zoom
  inline void set(float v) { mZoom = v; }

  //! Zooms in
  void zoomIn() {
    if (mZoom < 0.9f) {
      mZoom *= 2.0f;
    } else if (mZoom < 2.9f) {
      mZoom += 0.5f;
    } else if (mZoom < 5.0f) {
      mZoom += 1.0f;
    }
  }

  //! Zooms out
  void zoomOut() {
    if (mZoom >= 3.1f) {
      mZoom -= 1.0f;
    } else if (mZoom >= 1.1f) {
      mZoom -= 0.5f;
    } else if (mZoom > 0.2f) {
      mZoom /= 2.0f;
    }
  }

private:
  //! Current zoom
  float mZoom;
};