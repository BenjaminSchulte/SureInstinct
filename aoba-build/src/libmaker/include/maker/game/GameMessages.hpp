#pragma once

#include <QMap>

namespace Aoba {
  class TextMessage;
}

namespace Maker {
class GameProject;
class GameConfigNode;
class GameLinkResult;
class GameTranslationAdapter;

class GameMessages {
public:
  //! Constructor
  GameMessages(GameProject *project)
    : mProject(project)
  {}

  //! Deconstructor
  ~GameMessages();

  //! Configures the messages
  bool configure();

  //! Loads the global messages
  bool load();

  //! Builds the messages
  bool build();

  //! Links messages
  bool linkMessages(GameLinkResult &result);

  //! Loads all translations
  bool loadTranslations() const;

  //! Saves all translations
  bool saveTranslations() const;

protected:
  //! Creates a message
  Aoba::TextMessage *createMessage(const GameConfigNode &node, const QString &key);

  //! Creates an adapter
  void createAdapter(const GameConfigNode &config);

  //! The translation adapter
  GameTranslationAdapter *mInputAdapter = nullptr;

  //! The translation adapter
  GameTranslationAdapter *mOutputAdapter = nullptr;

  //! The project
  GameProject *mProject;

  //! Global messages
  QMap<QString, Aoba::TextMessage*> mGlobalMessages;

  //! The dictionary bank
  uint8_t mDictionaryBank = 0xD0;
};

}