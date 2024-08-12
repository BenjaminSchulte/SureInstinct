#pragma once

#include <QString>
#include <QMap>

namespace Aoba {
class TextMessage;
class TextDictionary;
class FontLetterRegistry;
class AssetLinkerObject;
class Project;
class TextTranslatableIndex;

class TextRegistry {
public:
  //! Constructor
  TextRegistry(Project *project);

  //! Deconstructor
  ~TextRegistry();

  //! Creates a new message
  TextMessage *createMessage(const QString &uniqueIdentifier, bool compressed=true);

  //! Returns a message (or null if the message is not created yet)
  inline TextMessage *message(const QString &uniqueIdentifier) const { return mMessages[uniqueIdentifier]; }

  //! Builds the text database
  bool build(FontLetterRegistry*, uint8_t dictionaryBank);

  //! Builds the text database
  bool buildTranslations();

  //! Returns all messages
  inline QList<TextMessage *> messages() const { return mMessages.values(); }

  //! Returns the dictionary
  inline TextDictionary *dictionary() const { return mDictionary; }

  //! Returns the linker object
  inline AssetLinkerObject *linkerObject() const { return mLinkerObject; }

  //! Returns the translatable index
  inline TextTranslatableIndex *translations() const { return mTranslations; }

private:
  //! The proejct
  Project *mProject;

  //! The translations
  TextTranslatableIndex *mTranslations;

  //! The linker object
  AssetLinkerObject *mLinkerObject = nullptr;

  //! The dictionary
  TextDictionary *mDictionary = nullptr;

  //! List of all texts
  QMap<QString, TextMessage*> mMessages;
};

}