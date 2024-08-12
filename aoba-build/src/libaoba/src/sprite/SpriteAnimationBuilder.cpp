#include <aoba/log/Log.hpp>
#include <QCryptographicHash>
#include <fma/output/DynamicBuffer.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <aoba/project/Project.hpp>
#include <aoba/asset/AssetLinkerObject.hpp>
#include <aoba/asset/AssetByteListLinker.hpp>
#include <aoba/sprite/SpriteAnimationBuilder.hpp>
#include <aoba/sprite/SpriteAnimationIndex.hpp>
#include <aoba/sprite/Sprite.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
SpriteAnimationBuilder::SpriteAnimationBuilder(Project *project, int maxNumSlots)
  : mProject(project)
  , mAnimationBuffer(new FMA::output::DynamicBuffer())
  , mFrameBuffer(new FMA::output::DynamicBuffer())
{
  mFrameBuffer->writeNumber(maxNumSlots, 1);
}

// -----------------------------------------------------------------------------
SpriteAnimationBuilder::~SpriteAnimationBuilder() {
  delete mAnimationBuffer;
  delete mFrameBuffer;
}

// -----------------------------------------------------------------------------
void SpriteAnimationBuilder::add(const SpriteAnimation &anim, int frame) {
  addCommand(anim.command, anim.commandParameter, anim.commandStringParameter, frame);

  if (!anim.isCommand()) {
    addFrame(anim, frame);
  }
}

// -----------------------------------------------------------------------------
void SpriteAnimationBuilder::addFrame(const SpriteAnimation &animation, int frame) {
  addFrameAndDelay(referenceFrame(frame, animation.flipX, animation.flipY, animation.x, animation.y), animation.delay);
}

// -----------------------------------------------------------------------------
void SpriteAnimationBuilder::addFrameAndDelay(int newFrame, int delay) {
  if (mIsEnd || delay == 0) {
    return;
  }

  int skipCount = (newFrame - 1) - mPreviousFrame;
  addSkip(skipCount);

  int thisDelay = qMin(delay - 1, 0xF);
  mAnimationBuffer->writeNumber(thisDelay, 1);
  mPreviousFrame = newFrame;

  addDelay(delay - thisDelay - 1);
}

// -----------------------------------------------------------------------------
void SpriteAnimationBuilder::addDelay(int count) {
  while (count > 0) {
    int abs = qMin(count, 10);
    addFrameAndDelay(mPreviousFrame, abs);
    count -= abs;
  }
}

// -----------------------------------------------------------------------------
void SpriteAnimationBuilder::addSkip(int count) {
  mPreviousFrame += count;

  while (count < 0) {
    int abs = qMin(0 - count, 8);
    mAnimationBuffer->writeNumber(0x20 - abs, 1);
    count += abs;
  }

  while (count > 0) {
    int abs = qMin(count - 1, 7);
    mAnimationBuffer->writeNumber(0x10 + abs, 1);
    count -= abs + 1;
  }
}

// -----------------------------------------------------------------------------
void SpriteAnimationBuilder::writeReference(const QString &target, int size) {
  mReferences.push_back(SpriteAnimationBuilderReference(target, mAnimationBuffer->getSize(), size));
  mAnimationBuffer->writeNumber(0, size);
}

// -----------------------------------------------------------------------------
void SpriteAnimationBuilder::addCommand(SpriteAnimationCommand command, int parameter, const QString &stringParameter, int) {
  if (mIsEnd) {
    return;
  }

  uint8_t tmp;

  switch (command) {
    case SpriteAnimationCommand::LABEL: mLabelOffsets.insert(stringParameter, mAnimationBuffer->getSize()); break;
    case SpriteAnimationCommand::USER_COMMAND: mAnimationBuffer->writeNumber(parameter, 1); break;
    //case SpriteAnimationCommand::GOTO: mAnimationBuffer->writeNumber(0x81, 1); writeReference(stringParameter, 1); mIsEnd=true; break;
    case SpriteAnimationCommand::BEGIN_LOOP: mAnimationBuffer->writeNumber(0x80, 1); break;
    case SpriteAnimationCommand::LOOP: mAnimationBuffer->writeNumber(0x81, 1); mIsEnd=true; break;
    case SpriteAnimationCommand::END: mAnimationBuffer->writeNumber(0x82, 1); mIsEnd=true; break;
    case SpriteAnimationCommand::DESTROY_SELF: mAnimationBuffer->writeNumber(0x83, 1); mIsEnd=true; break;

    case SpriteAnimationCommand::MOVE:
      tmp = mProject->lookupTable("uint16", "character_speed", QString::number(parameter), parameter);
      if (tmp >= 0x20) {
        Aoba::log::error("Move table reference out of range. Try removing speed variants in project.");
      } else {
        mAnimationBuffer->writeNumber(0x20 | tmp, 1);
      }
      break;

    case SpriteAnimationCommand::NONE: break;

    default:
      Aoba::log::warn("Unsupported sprite animation command " + QString::number((int)command));
      break;
  }
}

// -----------------------------------------------------------------------------
bool SpriteAnimationBuilder::finish() {
  addCommand(SpriteAnimationCommand::END, 0, "", 0);

  for (const auto &reference : mReferences) {
    if (!mLabelOffsets.contains(reference.label)) {
      Aoba::log::warn("Animation has no label " + reference.label + " defined");
      return false;
    }

    mAnimationBuffer->seek(reference.offset);
    mAnimationBuffer->writeNumber(mLabelOffsets[reference.label], reference.size);
  }

  return true;
}

// -----------------------------------------------------------------------------
int SpriteAnimationBuilder::referenceFrame(int frame, bool flipX, bool flipY, int x, int y) {
  SpriteAnimationBuilderFrame ref(frame, flipX, flipY, x, y);

  int index = mFrameReferenceIndex.indexOf(ref);
  if (index != -1) {
    return index;
  }

  index = mFrameReferenceIndex.size();
  mFrameReferenceIndex.push_back(ref);

  mFrameBuffer->writeNumber(frame, 1);
  mFrameBuffer->writeNumber((((uint8_t)x & 0xF) << 4) | ((uint8_t)y & 0x0F), 1);
  mFrameBuffer->writeNumber((flipX ? 0x40 : 0) | (flipY ? 0x80 : 0), 1);

  return index;
}

// -----------------------------------------------------------------------------
QString SpriteAnimationBuilder::animationHash() const {
  QCryptographicHash hash(QCryptographicHash::Md5);
  hash.addData(static_cast<const char*>(mAnimationBuffer->getData()), mAnimationBuffer->getSize());
  return hash.result().toHex();
}

// -----------------------------------------------------------------------------
QString SpriteAnimationBuilder::frameHash() const {
  QCryptographicHash hash(QCryptographicHash::Md5);
  hash.addData(static_cast<const char*>(mFrameBuffer->getData()), mFrameBuffer->getSize());
  return hash.result().toHex();
}

// -----------------------------------------------------------------------------
QString SpriteAnimationBuilder::animationSymbolName(SpriteAnimationIndexAssetSet *set) const {
  SpriteAnimationIndexAssetSet *root = set->root();
  QString symbolName = "__generated_asset_animationscript_" + animationHash();
  if (root->containsGeneratedSymbol(symbolName)) {
    return symbolName;
  }

  FMA::linker::LinkerBlock *block = root->assetLinkerObject().createLinkerBlock(symbolName, root->animationScriptBank());
  block->write(mAnimationBuffer->getData(), mAnimationBuffer->getSize());

  return symbolName;
}

// -----------------------------------------------------------------------------
QString SpriteAnimationBuilder::frameSymbolName(SpriteAnimationIndexAssetSet *set) const {
  SpriteAnimationIndexAssetSet *root = set->root();
  QString symbolName = "__generated_asset_frames_" + frameHash();
  if (root->containsGeneratedSymbol(symbolName)) {
    return symbolName;
  }

  FMA::linker::LinkerBlock *block = root->assetLinkerObject().createLinkerBlock(symbolName, root->frameReferenceBank());
  block->write(mFrameBuffer->getData(), mFrameBuffer->getSize());

  return symbolName;
}
