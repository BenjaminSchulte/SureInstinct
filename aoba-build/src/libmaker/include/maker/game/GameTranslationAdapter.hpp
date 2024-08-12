#pragma once

namespace Maker {
class GameProject;
class GameConfigNode;

class GameTranslationAdapter {
public:
  //! Constructor
  GameTranslationAdapter(GameProject *project) : mProject(project) {}

  //! Deconstructor
  virtual ~GameTranslationAdapter() = default;

  //! Configures the adapter
  virtual bool configure(const GameConfigNode &node) = 0;

  //! Loads the translations
  virtual bool load() = 0;

  //! Saves the translations
  virtual bool save() = 0;

protected:
  //! The project
  GameProject *mProject;
};

}