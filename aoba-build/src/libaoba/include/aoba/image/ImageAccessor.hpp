#pragma once

#include <QRect>
#include <QSharedPointer>
#include <QVector>

class QImage;
typedef QSharedPointer<class QImage> QImagePtr;

namespace Aoba {

struct SnesPaletteView;
typedef QSharedPointer<class SnesPalette> SnesPalettePtr;
typedef QSharedPointer<class ImageAccessor> ImageAccessorPtr;
typedef QSharedPointer<const class ImageAccessor> ConstImageAccessorPtr;
typedef QSharedPointer<class RgbaImage> RgbaImagePtr;
typedef QSharedPointer<class PaletteImage> PaletteImagePtr;
typedef uint32_t ImageColor;

struct ImageList : public QVector<ImageAccessorPtr> {

  int minWidth() const; 
  int maxWidth() const;
  int minHeight() const;
  int maxHeight() const;
  int totalWidth() const;
  int totalHeight() const;

  ImageAccessorPtr join() const;
};

class ImageAccessor : public QEnableSharedFromThis<ImageAccessor> {
public:
  ImageAccessor() {}
  virtual ~ImageAccessor() {}

  virtual ImageAccessorPtr createNewImage(int width, int height) const = 0;

  virtual ImageAccessorPtr clone() const;
  virtual int width() const = 0;
  virtual int height() const = 0;
  virtual uint8_t bytesPerPixel() const = 0;
  virtual uint8_t *data() = 0;
  inline ImageColor *data(int x, int y) { return reinterpret_cast<ImageColor*>(data() + (y * width() + x) * bytesPerPixel()); }
  virtual const uint8_t *constData() const = 0;
  inline const ImageColor *constData(int x, int y) const { return reinterpret_cast<const ImageColor*>(constData() + (y * width() + x) * bytesPerPixel()); }
  virtual QColor pixelColor(int x, int y) const = 0;
  virtual bool pixelIsTransparent(int x, int y) const;
  virtual QVector<QColor> colors() const;
  virtual void clearPixel(const QRect &) = 0;

  virtual QRect validRect(const QRect &) const;
  virtual ImageAccessorPtr slice(const QRect &rect) const;

  virtual ImageColor getRaw(int x, int y) const;
  virtual void setRaw(int x, int y, ImageColor color);

  virtual void draw(const ConstImageAccessorPtr &, const QPoint &);
  virtual void draw(const ConstImageAccessorPtr &, const QRect &);
  virtual void draw(const ConstImageAccessorPtr &, const QRect &, const QPoint &);

  virtual ImageList split(const QSize &) const;

  virtual bool isPaletteImage() const { return false; }
  virtual PaletteImagePtr toPaletteImage() const { return PaletteImagePtr(); }
  virtual bool isRgbaImage() const { return false; }
  virtual RgbaImagePtr toRgbaImage() const { return RgbaImagePtr(); }
  virtual bool isQImage() const { return false; }
  virtual QImagePtr toQImage() const { return QImagePtr(); }

  virtual QVector<int> findPalettes(const SnesPalettePtr &palettes, uint16_t numColorsPerPalette, bool transparent);
  virtual bool matchesPalette(const SnesPaletteView &view);
  virtual ImageAccessorPtr convertToPaletteImage(const SnesPaletteView &view) const;
  virtual ImageAccessorPtr convertToRgbaImage(const SnesPaletteView &view) const;

  virtual ImageAccessorPtr flip(bool x, bool y) const;
  virtual ImageAccessorPtr resizeTo(int w, int h) const;

  virtual QString hash() const;

  virtual int countVisiblePixel() const;
  virtual int countVisiblePixel(const QRect &) const;

  virtual void dump() const;
};

QStringList colorsAsString(const QVector<QColor> &);
QStringList colorsAsString(const QVector<struct SnesColor> &);

}