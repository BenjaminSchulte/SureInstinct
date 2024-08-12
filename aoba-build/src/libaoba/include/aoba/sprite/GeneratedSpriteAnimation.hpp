#pragma once

#include <QVector>

namespace Aoba {

class GeneratedSpriteAnimation {
public:
  //! Constructor
  GeneratedSpriteAnimation(int id, const QString &name, int animationId, const QString &anim, const QString &dir)
    : mId(id), mName(name), mAnimationId(animationId), mAnimation(anim), mDirection(dir) {}

  //! Returns the ID
  inline int id() const { return mId; }

  //! Returns the name
  inline const QString &name() const { return mName; }

  //! Returne the animation ID
  inline int animationId() const { return mAnimationId; }

  //! Returns the name
  inline const QString &animation() const { return mAnimation; }

  //! Returns the name
  inline const QString &direction() const { return mDirection; }

private:
  //! The ID
  int mId;

  //! The name
  QString mName;

  //! The ID
  int mAnimationId;

  //! The name
  QString mAnimation;

  //! The name
  QString mDirection;
};

}