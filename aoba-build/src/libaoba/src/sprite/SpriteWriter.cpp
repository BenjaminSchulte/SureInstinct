#include <aoba/log/Log.hpp>
#include <aoba/sprite/Sprite.hpp>
#include <aoba/sprite/SpriteWriter.hpp>
#include <aoba/csv/CsvReader.hpp>
#include <yaml-cpp/yaml.h>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool SpriteWriter::save(YAML::Emitter &root) {
  if (!writeImageGroups(root)
    || !writeFrames(root)
    || !writeFrameGroups(root)
    || !writeAnimations(root)
  ) {
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
bool SpriteWriter::writeFrameGroups(YAML::Emitter &root) {
  if (mSprite->frameGroups().isEmpty()) {
    return true;
  }

  root << YAML::Key << "frame_groups" << YAML::Value << YAML::BeginMap;
  
  QMapIterator<QString, SpriteFrameGroup> it(mSprite->frameGroups());
  while (it.hasNext()) {
    it.next();

    root << YAML::Key << it.key().toStdString() << YAML::Value << YAML::BeginSeq;
    for (const SpriteFrameGroupFrame &frame : it.value()) {
      root << frame.frameId.toStdString();
    }
    root << YAML::EndSeq;
  }

  root << YAML::EndMap;

  return true;
}

// -----------------------------------------------------------------------------
bool SpriteWriter::writeImageGroups(YAML::Emitter &root) {
  if (mSprite->imageGroups().isEmpty()) {
    return true;
  }

  root << YAML::Key << "image_groups" << YAML::Value << YAML::BeginMap;
  QMapIterator<QString, SpriteImageGroup> it(mSprite->imageGroups());
  while (it.hasNext()) {
    it.next();

    root << YAML::Key << it.key().toStdString() << YAML::Value << YAML::BeginSeq;
    for (int image : it.value()) {
      root << YAML::BeginMap << YAML::Key << "image" << YAML::Value << image << YAML::EndMap;
    }
    root << YAML::EndSeq;
  }
  root << YAML::EndMap;
  
  return true;
}

// -----------------------------------------------------------------------------
bool SpriteWriter::writeFrames(YAML::Emitter &root) {
  if (mSprite->frames().isEmpty()) {
    return true;
  }

  root << YAML::Key << "frames" << YAML::Value << YAML::BeginMap;
  QMapIterator<QString, SpriteFrame> it(mSprite->frames());
  while (it.hasNext()) {
    it.next();

    root << YAML::Key << it.key().toStdString() << YAML::Value << YAML::BeginMap;
    if (!writeFrame(root, it.value())) {
      return false;
    }
    root << YAML::EndMap;
  }
  root << YAML::EndMap;
  
  return true;
}

// -----------------------------------------------------------------------------
bool SpriteWriter::writeFrame(YAML::Emitter &root, const SpriteFrame &frame) {
  if (!frame.sharedImage.isEmpty()) {
    root << YAML::Key << "shared_image_group" << YAML::Value << frame.sharedImage.toStdString();
  }
  if (!frame.staticImage.isEmpty()) {
    root << YAML::Key << "static_image_group" << YAML::Value << frame.staticImage.toStdString();
  }

  root << YAML::Key << "images" << YAML::Value << YAML::Flow << YAML::BeginSeq;
  for (const auto &image : frame.images) {
    root << YAML::Flow << YAML::BeginMap;
    root << YAML::Key << "image" << YAML::Value << image.image;
    if (image.x != 0) {
      root << YAML::Key << "x" << YAML::Value << image.x;
    }
    if (image.y != 0) {
      root << YAML::Key << "y" << YAML::Value << image.y;
    }
    if (image.priority != 0) {
      root << YAML::Key << "priority" << YAML::Value << image.priority;
    }
    root << YAML::EndMap;
  }
  root << YAML::EndSeq;

  return true;
}

// -----------------------------------------------------------------------------
bool SpriteWriter::writeAnimations(YAML::Emitter &root) {
  if (mSprite->animations().isEmpty()) {
    return true;
  }

  root << YAML::Key << "animations" << YAML::Value << YAML::BeginMap;
  QMapIterator<QString, SpriteAnimationFrames> it(mSprite->animations());
  while (it.hasNext()) {
    it.next();
    
    root << YAML::Key << it.key().toStdString() << YAML::Value << YAML::BeginSeq;
    for (const auto &item : it.value()) {
      root << YAML::BeginMap;
      if (!writeAnimation(root, item)) {
        return false;
      }
      root << YAML::EndMap;
    }
    root << YAML::EndSeq;
  }
  root << YAML::EndMap;
  
  return true;
}

// -----------------------------------------------------------------------------
bool SpriteWriter::writeAnimation(YAML::Emitter &root, const SpriteAnimation &anim) {
  switch (anim.command) {
    case SpriteAnimationCommand::NONE:
      root << YAML::Key << "frame" << YAML::Value << anim.frame.toStdString();
      root << YAML::Key << "delay" << YAML::Value << anim.delay;
      if (anim.x != 0) { root << YAML::Key << "x" << YAML::Value << anim.x; }
      if (anim.y != 0) { root << YAML::Key << "y" << YAML::Value << anim.y; }
      if (anim.flipX) { root << YAML::Key << "flipX" << YAML::Value << true; }
      if (anim.flipY) { root << YAML::Key << "flipY" << YAML::Value << true; }
      break;
    case SpriteAnimationCommand::BEGIN_LOOP:
      root << YAML::Key << "action" << YAML::Value << "begin_loop";
      break;
    case SpriteAnimationCommand::LOOP:
      root << YAML::Key << "action" << YAML::Value << "loop";
      break;
    case SpriteAnimationCommand::MOVE:
      root << YAML::Key << "action" << YAML::Value << "move";
      root << YAML::Key << "speed" << YAML::Value << anim.commandParameter;
      break;
    case SpriteAnimationCommand::DESTROY_SELF:
      root << YAML::Key << "action" << YAML::Value << "destroy_self";
      break;
    case SpriteAnimationCommand::USER_COMMAND:
      root << YAML::Key << "action" << YAML::Value << "user_command";
      root << YAML::Key << "command" << YAML::Value << anim.commandParameter;
      break;

    case SpriteAnimationCommand::LABEL:
    case SpriteAnimationCommand::GOTO:
    case SpriteAnimationCommand::END:
    default:
      Aoba::log::warn("Unsupported animation command");
      return false;
  }

  return true;
}