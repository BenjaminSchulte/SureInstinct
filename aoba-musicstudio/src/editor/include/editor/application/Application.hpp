#pragma once

#include <QTranslator>
#include <QApplication>
#include <QCommandLineParser>

namespace Editor {
class MainWindow;
class ProjectApplication;
class DisplayConfiguration;

class Application : public QApplication {
  Q_OBJECT

public:
  Application(int &args, char **argv);
  ~Application();

  bool initialize();

protected:
  bool initializeConfiguration();
  bool initializeLanguage();
  bool initializeTitle();
  bool initializeMainWindow();
  bool loadApplicationConfig();

  QTranslator mTranslator;
  QCommandLineParser mCommandLine;

  MainWindow *mWindow;
  DisplayConfiguration *mDisplayConfiguration;
};

}