#pragma once

#include <QVector>
#include "Sprite.hpp"

namespace FMA {
namespace output {
class DynamicBuffer;
}
}

namespace Aoba {
class Project;
class SpriteAnimationIndexAssetSet;
enum class SpriteAnimationCommand;

struct SpriteAnimationBuilderFrame {
  SpriteAnimationBuilderFrame() {}
  SpriteAnimationBuilderFrame(int frame, bool flipX, bool flipY, int x, int y)
    : frame(frame)
    , flipX(flipX)
    , flipY(flipY)
    , x(x)
    , y(y)
  {}

  bool operator==(const SpriteAnimationBuilderFrame &other) const {
    return frame == other.frame && flipX == other.flipX && flipY == other.flipY && x == other.x && y == other.y;
  }

  int frame;
  bool flipX;
  bool flipY;
  int x;
  int y;
};

struct SpriteAnimationBuilderReference {
  SpriteAnimationBuilderReference() {}
  SpriteAnimationBuilderReference(const QString &label, int offset, int size)
    : label(label), offset(offset), size(size)
  {}

  QString label;
  int offset;
  int size;
};

class SpriteAnimationBuilder {
public:
  //! Constructor
  SpriteAnimationBuilder(Project *project, int maxNumSlots);

  //! Deconstructor
  ~SpriteAnimationBuilder();

  //! Adds a frame
  void add(const SpriteAnimation &anim, int frame);

  //! Finishes the animation
  bool finish();

  //! Returns the symbol name
  QString animationSymbolName(SpriteAnimationIndexAssetSet *) const;

  //! Returns the symbol name
  QString frameSymbolName(SpriteAnimationIndexAssetSet *) const;

private:
  //! Adds a frame
  void addCommand(SpriteAnimationCommand command, int parameter, const QString &stringParameter, int frame);

  //! Adds a frame
  void addFrame(const SpriteAnimation &, int frame);

  //! Adds frame skip
  void addSkip(int count);

  //! Adds frame skip
  void addDelay(int count);

  //! Adds a frame
  unsigned int addFrame(const SpriteAnimation &, int frame) const;

  //! Converts the frame to the value in the reference index
  int referenceFrame(int frame, bool flipX, bool flipY, int x, int y);

  //! Writes a reference
  void writeReference(const QString &target, int size);

  //! Returns a hash from the animation
  QString animationHash() const;

  //! Returns a hash from the frame reference index
  QString frameHash() const;

  //! Adds a frame and delays
  void addFrameAndDelay(int newFrame, int delay);

  //! The project
  Project *mProject;

  //! The frame reference index
  QVector<SpriteAnimationBuilderFrame> mFrameReferenceIndex;

  //! Offsets of all labels
  QMap<QString, int> mLabelOffsets;

  //! Offsets of all references
  QVector<SpriteAnimationBuilderReference> mReferences;

  //! Writing buffer
  FMA::output::DynamicBuffer *mAnimationBuffer;

  //! Writing buffer
  FMA::output::DynamicBuffer *mFrameBuffer;

  //! Whether the end has been reached
  bool mIsEnd = false;

  //! The previous index
  int mPreviousFrame = -1;
};

}