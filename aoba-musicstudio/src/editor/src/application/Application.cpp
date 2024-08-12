#include <QDebug>
#include <QStyleFactory>
#include <editor/application/Application.hpp>
#include <editor/application/MainWindow.hpp>
#include <editor/application/DisplayConfiguration.hpp>
#include <editor/render/RenderTheme.hpp>
#include <editor/playback/AudioManager.hpp>

using namespace Editor;

// ----------------------------------------------------------------------------
Application::Application(int &argc, char **argv)
  : QApplication(argc, argv)
  , mWindow(nullptr)
{
  mCommandLine.addHelpOption();
  mCommandLine.addPositionalArgument("file", QCoreApplication::translate("main", "File to open"));
  mCommandLine.process(arguments());
}

// ----------------------------------------------------------------------------
Application::~Application() {
  delete mWindow;
  delete mDisplayConfiguration;
}

// ----------------------------------------------------------------------------
bool Application::initialize() {
  return 
       initializeTitle()
    && loadApplicationConfig()
    && initializeConfiguration()
    && initializeLanguage()
    && initializeMainWindow();
}

// ----------------------------------------------------------------------------
bool Application::initializeLanguage() {
  QString defaultLocale = QLocale::system().name();
  defaultLocale.truncate(defaultLocale.lastIndexOf('_'));

  qDebug() << "System language is: " << defaultLocale;

  if (!mTranslator.load("en_qm", ":/translations/")) {
    qDebug() << "Could not load fallback language 'en'";
  }

  if (!mTranslator.load(defaultLocale + "_qm", ":/translations/")) {
    qWarning() << "No translations for your language found.";
  }

  installTranslator(&mTranslator);
  return true;
}

// ----------------------------------------------------------------------------
bool Application::initializeTitle() {
  setOrganizationDomain(QLatin1String(APPLICATION_DOMAIN));
# if defined(Q_OS_MAC) || defined(Q_OS_WIN)
    setApplicationName(QLatin1String(APPLICATION_NAME));
# else
    setApplicationName(QLatin1String(APPLICATION_NAME));
# endif
  setApplicationDisplayName(QLatin1String(APPLICATION_DISPLAY_NAME " v" VERSION_STRING));

# ifdef Q_OS_MAC
    setAttribute(Qt::AA_DontShowIconsInMenus);
# endif

  return true;
}

// ----------------------------------------------------------------------------
bool Application::loadApplicationConfig() {
  /*QString basePath(mProjectApplication->getUserConfigurationFolder());
  QFileInfo configFile(basePath + QDir::separator() + "application.json");

  if (!configFile.exists()) {
    log::info() << "No application config found at " << configFile.filePath();
    return true;
  }

  log::todo() << "Load config from " << configFile.filePath();
*/
  return true;
}

// ----------------------------------------------------------------------------
bool Application::initializeConfiguration() {
  mDisplayConfiguration = new DisplayConfiguration();

  setStyle(QStyleFactory::create("Fusion"));
  setPalette(mDisplayConfiguration->theme()->palette());
  setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");

  return true;
}

// ----------------------------------------------------------------------------
bool Application::initializeMainWindow() {
  mWindow = new MainWindow(nullptr, mDisplayConfiguration);
  mWindow->show();

  setActiveWindow(mWindow);
# if defined(Q_OS_WIN) && QT_VERSION >= 0x050700
    QWindowsWindowFunctions::setWindowActivationBehavior(QWindowsWindowFunctions::AlwaysActivateWindow);
# endif

  if (!mCommandLine.positionalArguments().isEmpty()) {
    mWindow->loadSong(mCommandLine.positionalArguments().last());
  }

  return true;
}
