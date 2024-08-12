#pragma once

#include <QString>
#include <QVector>
#include <fma/symbol/SymbolReference.hpp>
#include "../font/FontLetterRegistry.hpp"
#include "./TextCommand.hpp"

namespace Aoba {
class FontInterface;
class TextRegistry;

struct TextMessagePart {
  TextMessagePart() {}
  TextMessagePart(const QString &text) : text(text), command(TextCommand::WRITE_TEXT) {}
  TextMessagePart(TextCommand command) : command(command) {}
  TextMessagePart(TextCommand command, int value) : command(command), referenceLength(value) {}
  TextMessagePart(TextCommand command, const QString &reference, int referenceLength) : command(command), reference(reference), referenceLength(referenceLength) {}
  QString text;
  TextCommand command;
  QString reference;
  int referenceLength;
};

struct TextMessageEstimation {
  uint32_t minLength = 0;
  uint32_t maxLength = 0;
};

class TextMessage {
public:
  //! Constructor
  TextMessage(TextRegistry *reg, const QString &uniqueIdentifier, bool compressed);

  //! Builds this text
  bool build(FontLetterRegistry*);

  //! Marks the message as important
  void setIsImportant() {
    mIsImportant = true;
  }

  //! Adds a script
  void addScript(const QString &text);

  //! Adds a constant text
  void addConstantText(const QString &text);

  //! Adds a command
  void addCommand(const QString &cmd, const QStringList &args);

  //! Returns a global reference (example: __text_index)
  QString globalReference();

  //! Returns the text without commands
  QString textWithoutCommands() const;

  //! Adds a font
  void useInFont(FontInterface *font);

  //! Returns all letters used in this text
  void countLetters(FontLetterIndex &letters) const;

  //! Returns whether the text can be compressed
  inline bool isCompressed() const { return mCompressed; }

  //! Estimates the text length
  void estimateTextLength(TextMessageEstimation &result) const;

protected:
  //! Whether the text can be modified
  bool mCanBeModified = true;

  //! The text registry
  TextRegistry *mRegistry;

  //! The unique identifier
  QString mUniqueIdentifier;

  //! Whether the text is compressed
  bool mCompressed;

  //! Whether the text letters are important
  bool mIsImportant = false;

  //! Text message part
  QVector<TextMessagePart> mParts;
};

}