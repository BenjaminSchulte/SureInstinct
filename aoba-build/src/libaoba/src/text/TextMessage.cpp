#include <aoba/log/Log.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <aoba/font/Font.hpp>
#include <aoba/font/FontLetterRegistry.hpp>
#include <aoba/asset/AssetLinkerObject.hpp>
#include <aoba/text/TextMessage.hpp>
#include <aoba/text/TextRegistry.hpp>
#include <aoba/text/TextDictionary.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
TextMessage::TextMessage(TextRegistry *reg, const QString &uniqueIdentifier, bool compressed)
  : mRegistry(reg)
  , mUniqueIdentifier(uniqueIdentifier)
  , mCompressed(compressed)
{
}

// -----------------------------------------------------------------------------
QString TextMessage::globalReference() {
  return "__asset_text_" + mUniqueIdentifier;
}

// -----------------------------------------------------------------------------
bool TextMessage::build(FontLetterRegistry *letters) {
  TextMessageEstimation size;
  estimateTextLength(size);

  if (size.maxLength >= TEXT_MAX_LENGTH) {
    Aoba::log::warn("Text exceeds maximum length of " + QString::number(TEXT_MAX_LENGTH) + " characters");
    return false;
  } else if (size.minLength >= TEXT_MAX_LENGTH) {
    Aoba::log::warn("Text MIGHT exceed maximum length of " + QString::number(TEXT_MAX_LENGTH) + " characters");
  }


  FMA::linker::LinkerBlock *block = mRegistry->linkerObject()->createLinkerBlock(globalReference());

  block->symbol(globalReference().toStdString());
  for (const TextMessagePart &part : mParts) {
    switch (part.command) {
      // Default text
      case TextCommand::WRITE_TEXT:
        mRegistry->dictionary()->write(block, part.text, letters, mCompressed);
        break;

      case TextCommand::NEW_LINE:
        block->writeNumber(TEXT_CMD_NEW_LINE, 1);
        break;

      case TextCommand::NEW_PAGE:
        block->writeNumber(TEXT_CMD_NEW_PAGE, 1);
        break;

      case TextCommand::OPTION:
        block->writeNumber(TEXT_CMD_OPTION, 1);
        break;

      case TextCommand::WAIT_FOR_CONFIRM:
        block->writeNumber(TEXT_CMD_CHOOSE_OPTION, 1);
        break;

      case TextCommand::SET_COLOR:
        block->writeNumber(TEXT_CMD_SET_COLOR0 + part.referenceLength, 1);
        break;

      case TextCommand::SET_ALIGN:
        block->writeNumber(TEXT_CMD_ALIGN_LEFT + part.referenceLength, 1);
        break;

      case TextCommand::WAIT:
        block->writeNumber(TEXT_CMD_DELAY, 1);
        block->writeNumber(part.referenceLength, 1);
        break;

      case TextCommand::PRINT_VARIABLE:
        block->writeNumber(TEXT_CMD_PRINT_VARIABLE, 1);
        block->writeNumber(part.referenceLength, 2);
        break;

      default:
        Aoba::log::warn("Unsupported text command " + QString::number((int)part.command) + " in build");
    }
  }
  mRegistry->dictionary()->writeEndOfText(block);

  return true;
}

// -----------------------------------------------------------------------------
void TextMessage::useInFont(FontInterface *font) {
  mCanBeModified = false;

  FontLetterIndex index;
  countLetters(index);
  QMapIterator<uint32_t, int> it(index);
  while (it.hasNext()) {
    it.next();
    font->addUsedLetter(it.key(), it.value());
  }
}

// -----------------------------------------------------------------------------
void TextMessage::addConstantText(const QString &text) {
  if (!mCanBeModified) {
    Aoba::log::warn("Tried to edit TextMessage which is locked");
    return;
  }
  if (text.isEmpty()) {
    return;
  }

  mParts.push_back(TextMessagePart(text));
}

// -----------------------------------------------------------------------------
void TextMessage::addCommand(const QString &_cmd, const QStringList &args) {
  QString cmd = _cmd.toLower();

  if (cmd == "ok" || cmd == "select") {
    mParts.push_back(TextMessagePart(TextCommand::WAIT_FOR_CONFIRM));
  } else if (cmd == "nl") {
    mParts.push_back(TextMessagePart(TextCommand::NEW_LINE));
  } else if (cmd == "o") {
    mParts.push_back(TextMessagePart(TextCommand::OPTION));
  } else if ((cmd == "wait" || cmd == "delay") && args.size() == 1) {
    mParts.push_back(TextMessagePart(TextCommand::WAIT, args[0].toInt()));
  } else if (cmd == "np") {
    mParts.push_back(TextMessagePart(TextCommand::NEW_PAGE));
  } else if ((cmd == "c" || cmd == "color") && args.size() == 1) {
    mParts.push_back(TextMessagePart(TextCommand::SET_COLOR, args[0].toInt()));
  } else if (cmd == "left") {
    mParts.push_back(TextMessagePart(TextCommand::SET_ALIGN, 0));
  } else if (cmd == "center") {
    mParts.push_back(TextMessagePart(TextCommand::SET_ALIGN, 1));
  } else if (cmd == "right") {
    mParts.push_back(TextMessagePart(TextCommand::SET_ALIGN, 2));
  } else if (cmd == "v") {
    mParts.push_back(TextMessagePart(TextCommand::PRINT_VARIABLE, args[0].toInt()));
  } else {
    (void)args;
    Aoba::log::warn("Unknown text command " + cmd);
  }
}

// -----------------------------------------------------------------------------
void TextMessage::addScript(const QString &text) {
  if (!mCanBeModified) {
    Aoba::log::warn("Tried to edit TextMessage which is locked");
    return;
  }

  int textStart = 0;
  int left;
  int length = text.length();

  while ((left = text.indexOf('<', textStart)) != -1) {
    if (left > textStart) {
      addConstantText(text.mid(textStart, left - textStart));
    }

    int right = text.indexOf('>', left);
    if (right == -1) {
      break;
    }
    
    QString fullCommand = text.mid(left + 1, right - left - 1);
    if (!fullCommand.isEmpty()) {
      QStringList args = fullCommand.split(",");
      QString command = args.front();
      args.pop_front();

      addCommand(command, args);
    }

    textStart = right + 1;
  }

  if (textStart < length) {
    addConstantText(text.mid(textStart));
  }
}

// -----------------------------------------------------------------------------
void TextMessage::countLetters(FontLetterIndex &letters) const {
  FontLetterIndex localCounter;

  for (const TextMessagePart &part : mParts) {
    for (int i=part.text.length() - 1; i>=0; i--) {
      uint32_t letter = part.text[i].unicode();

      letters[letter]++;

      if (mIsImportant && !localCounter.contains(letter)) {
        localCounter.insert(letter, 1);
        letters[letter] += 0x1000000;
      }
    }
  }
}

// -----------------------------------------------------------------------------
QString TextMessage::textWithoutCommands() const {
  QString msg;

  for (const TextMessagePart &part : mParts) {
    msg += part.text;
  }

  return msg;
}

// -----------------------------------------------------------------------------
void TextMessage::estimateTextLength(TextMessageEstimation &result) const {
  // End of text command
  result.minLength += 1;
  result.maxLength += 1;

  for (const TextMessagePart &part : mParts) {
    switch (part.command) {
      // Constant text
      case TextCommand::WRITE_TEXT:
        result.minLength += part.text.length();
        result.maxLength += part.text.length();
        break;

      // Commands with effect on the length
      case TextCommand::PRINT_VARIABLE:
        result.minLength += 1;
        result.maxLength += 6;
        break;

      // Commands with parameter
      case TextCommand::WAIT:
        result.minLength += 2;
        result.maxLength += 2;
        break;

      // Commands without effect on the length
      case TextCommand::SET_COLOR:
      case TextCommand::SET_ALIGN:
      case TextCommand::NEW_LINE:
      case TextCommand::NEW_PAGE:
      case TextCommand::OPTION:
      case TextCommand::WAIT_FOR_CONFIRM:
        result.minLength++;
        result.maxLength++;
        break;

      default:
        Aoba::log::warn("Unknown text command " + QString::number((int)part.command) + " in estimateTextLength. Result might be invalid");
    }
  }
}

// -----------------------------------------------------------------------------
