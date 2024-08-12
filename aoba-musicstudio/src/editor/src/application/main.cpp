#include <QGuiApplication>
#include <QStyleFactory>
#include <QPalette>
#include <editor/application/Application.hpp>

using namespace Editor;

// ----------------------------------------------------------------------------
int main(int argc, char **argv) {
  QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
  QGuiApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);

  Application app(argc, argv);
    if (!app.initialize()) {
    return 1;
  }

  return app.exec();
}
