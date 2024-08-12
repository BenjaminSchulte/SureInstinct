#include <QDebug>
#include <aoba/log/Log.hpp>
#include <aoba/sprite/Sprite.hpp>
#include <aoba/sprite/SpriteLoader.hpp>
#include <aoba/csv/CsvReader.hpp>
#include <aoba/image/QImageAccessor.hpp>
#include <aoba/palette/SnesPalette.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool SpriteLoader::load(const YAML::Node &config) {
  if (!loadImages()
    || !loadImageGroups(config)
    || !loadFrames(config)
    || !loadFrameGroups(config)
    || !loadAnimations(config)
  ) {
    return false;
  }

  mSprite->setCalculatedViewbox(mViewbox);

  return true;
}

// -----------------------------------------------------------------------------
bool SpriteLoader::loadImages() {
  //Aoba::log::debug(".. Loading frames");

  QFile csvFile(mSprite->path().filePath("images.csv"));
  if (!csvFile.open(QIODevice::ReadOnly)) {
    Aoba::log::error("Could not open frames CSV file");
    return false;
  }

  QTextStream in(&csvFile);
  CsvReader csv(in, true);
  while (csv.nextRow()) {
    if (!loadImage(csv)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool SpriteLoader::loadImageGroups(const YAML::Node &config) {
  if (!config["image_groups"] || !Yaml::isObject(config["image_groups"])) {
    return true;
  }

  for (const auto &pair : config["image_groups"]) {
    if (!loadImageGroup(pair.first.as<std::string>().c_str(), pair.second)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------  
bool SpriteLoader::loadImageGroup(const QString &id, const YAML::Node &config) {
  SpriteImageGroup group;

  for (const auto &item : config) {
    group.push_back(item["image"].as<int>());
  }

  if (group.isEmpty()) {
    Aoba::log::error("Image groups may not be empty");
    return false;
  }

  mSprite->createImageGroup(id, group);

  return true;
}

// -----------------------------------------------------------------------------
bool SpriteLoader::loadFrameGroups(const YAML::Node &config) {
  if (!config["frame_groups"] || !Yaml::isObject(config["frame_groups"])) {
    return true;
  }

  for (const auto &pair : config["frame_groups"]) {
    if (!loadFrameGroup(pair.first.as<std::string>().c_str(), pair.second)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------  
bool SpriteLoader::loadFrameGroup(const QString &id, const YAML::Node &config) {
  SpriteFrameGroup group;
  for (const auto &item : config) {
    SpriteFrameGroupFrame frame;
    frame.frameId = item["frame"].as<std::string>().c_str();
    group.push_back(frame);
  }

  mSprite->createFrameGroup(id, group);

  return true;
}

// -----------------------------------------------------------------------------
bool SpriteLoader::loadAnimations(const YAML::Node &config) {
  for (const auto &pair : config["animations"]) {
    QString id(pair.first.as<std::string>().c_str());
    for (const auto &item : pair.second) {
      if (!loadAnimationFrame(id, item)) {
        return false;
      }
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool SpriteLoader::loadFrames(const YAML::Node &config) {
  for (const auto &pair : config["frames"]) {
    QString id(pair.first.as<std::string>().c_str());
    
    if (!loadFrame(id, pair.second)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool SpriteLoader::loadFrame(const QString &id, const YAML::Node &config) {
  SpriteFrame frame;

  if (config["shared_image_group"]) {
    frame.sharedImage = Yaml::asString(config["shared_image_group"]);
  }

  if (config["static_image_group"]) {
    frame.staticImage = Yaml::asString(config["static_image_group"]);
  }

  for (const auto &imageConfig : config["images"]) {
    SpriteFrameImage image;
    image.image = Yaml::asInt(imageConfig["image"]);
    image.priority = Yaml::asInt(imageConfig["priority"]);
    image.x = Yaml::asInt(imageConfig["x"]);
    image.y = Yaml::asInt(imageConfig["y"]);
    frame.images.push_back(image);
  }

  mSprite->addFrame(id, frame);

  return true;
}

// -----------------------------------------------------------------------------
bool SpriteLoader::loadImage(CsvReader &csv) {
  SpriteImage frame;
  int x = csv.get("x").toInt();
  int y = csv.get("y").toInt();
  int width = csv.get("w").toInt();
  int height = csv.get("h").toInt();
  frame.pivotX = csv.get("px").toInt();
  frame.pivotY = csv.get("py").toInt();

  QString paletteFile = csv.get("palette");
  QImagePtr image = loadImage(csv.get("image"));
  SnesPalettePtr palette = loadPalette(paletteFile);
  if (!image || !palette) {
    return false;
  }

  QImageAccessor img(image);
  ImageAccessorPtr frameImage(img.slice(QRect(x, y, width, height)));
  QVector<int> paletteIndices = frameImage->findPalettes(palette, 16, true);
  if (!paletteIndices.length()) {
    Aoba::log::warn("Could not find any matching palette for frame at " + QString::number(x) + ", " + QString::number(y) + " in image " + csv.get("image"));
    return false;
  }

  frame.paletteIndex = mSprite->addPalette(paletteFile);
  if (frame.paletteIndex == -1) {
    return false;
  }

  frame.subPaletteIndex = paletteIndices.first();
  SnesPaletteView paletteView = palette->subPalette(paletteIndices.first(), 16, true);
  frame.image = frameImage->convertToPaletteImage(paletteView);

  mSprite->addImage(frame);

  return true;
}

// -----------------------------------------------------------------------------
bool SpriteLoader::loadAnimationFrame(const QString &id, const YAML::Node &config) {
  QString command = Yaml::asString(config["action"]);

  if (command == "move") {
    mSprite->animation(id).push_back(SpriteAnimation(SpriteAnimationCommand::MOVE, Yaml::asInt(config["speed"])));
    return true;
  } else if (command == "label" || (command.isEmpty() && Yaml::isString(config["label"]))) {
    mSprite->animation(id).push_back(SpriteAnimation(SpriteAnimationCommand::LABEL, Yaml::asString(config["label"])));
    return true;
  } else if (command == "goto") {
    mSprite->animation(id).push_back(SpriteAnimation(SpriteAnimationCommand::GOTO, Yaml::asString(config["label"])));
    return true;
  } else if (command == "user_command") {
    mSprite->animation(id).push_back(SpriteAnimation(SpriteAnimationCommand::USER_COMMAND, Yaml::asInt(config["command"])));
    return true;
  } else if (command == "begin_loop") {
    mSprite->animation(id).push_back(SpriteAnimation(SpriteAnimationCommand::BEGIN_LOOP));
    return true;
  } else if (command == "loop") {
    mSprite->animation(id).push_back(SpriteAnimation(SpriteAnimationCommand::LOOP));
    return true;
  } else if (command == "destroy_self") {
    mSprite->animation(id).push_back(SpriteAnimation(SpriteAnimationCommand::DESTROY_SELF));
    return true;
  } else if (!command.isEmpty()) {
    Aoba::log::warn("Unsupported animaton command " + command);
    return false;
  }

  SpriteAnimation frame;
  frame.frame = Yaml::asString(config["frame"]);
  frame.delay = Yaml::asInt(config["delay"]);
  frame.x = Yaml::asInt(config["x"]);
  frame.y = Yaml::asInt(config["y"]);
  frame.flipX = Yaml::asBool(config["flipX"]);
  frame.flipY = Yaml::asBool(config["flipY"]);
  if (frame.delay <= 0) {
    Aoba::log::warn("Invalid speed for delay for animation " + id);
    return false;
  }


  mSprite->animation(id).push_back(frame);

  if (frame.x < -8 || frame.x >= 8 || frame.y < -8 || frame.y >= 8) {
    Aoba::log::warn("Animation frame x/y must be between -8 and 7");
    return false;
  }

  if (!mSprite->hasFrame(frame.frame)) {
    Aoba::log::warn("Sprite has no frame " + frame.frame);
    return false;
  }

  const auto &frameInfo = mSprite->frames()[frame.frame];
  QRect frameViewbox;
  for (const auto &frameImageInfo : frameInfo.images) {
    const auto &frameImage = mSprite->images()[frameImageInfo.image];

    frameViewbox = frameViewbox.united(QRect(
      frameImageInfo.x - frameImage.pivotX,
      frameImageInfo.y - frameImage.pivotY,
      frameImage.image->width(),
      frameImage.image->height()
    ));
  }
  
  mViewbox = mViewbox.united(frameViewbox);

  return true;
}

// -----------------------------------------------------------------------------
QImagePtr SpriteLoader::loadImage(const QString &id) {
  if (mImages.contains(id)) {
    return mImages[id];
  }

  //Aoba::log::debug(".. Loading sprite sheet " + id);
  QImagePtr image(QImageAccessor::loadImage(mSprite->path().filePath(id)));
  if (!image) {
    Aoba::log::warn("Failed to load image file " + id);
    return QImagePtr();
  }

  mImages.insert(id, image);
  return image;
}

// -----------------------------------------------------------------------------
SnesPalettePtr SpriteLoader::loadPalette(const QString &id) {
  if (mPalettes.contains(id)) {
    return mPalettes[id];
  }

  //Aoba::log::debug(".. Loading sprite palette " + id);

  QImagePtr paletteImage(QImageAccessor::loadImage(mSprite->path().filePath(id)));
  if (!paletteImage) {
    Aoba::log::warn("Failed to load palette file " + id);
    return SnesPalettePtr();
  }

  SnesPalettePtr palette(SnesPalette::fromImage(paletteImage.get()));
  if (!palette) {
    Aoba::log::warn("Failed to generate palette from image file " + id);
    return SnesPalettePtr();
  }

  mPalettes.insert(id, palette);
  return palette;
}