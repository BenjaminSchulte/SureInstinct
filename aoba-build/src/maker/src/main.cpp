#include <QApplication>
#include <maker/MainWindow.hpp>

#include <aoba/log/Log.hpp>
#include <aoba/log/ConsoleLog.hpp>
#include <QDirIterator>

int main(int argc, char *argv[]) {
	Aoba::ConsoleLog consoleLog;
	Aoba::Log::instance()->addAdapter(&consoleLog);

	QApplication application(argc, argv);
	application.setApplicationName("AOBA Game Studio");
	application.setOrganizationName("AobaStudio");
	application.setOrganizationDomain("auba-studio.com");

	MainWindow window;
	if (argc > 1) {
	  window.loadProject(argv[1]);
	}
	window.show();

	return application.exec();
}
