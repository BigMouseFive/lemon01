#include <winsock2.h>
#include "lemon01.h"
#include <qfile.h>
#include <QtWidgets/QApplication>
#include <qdir.h>
void setStyle(const QString &style) {
	QFile qss(style);
	qss.open(QFile::ReadOnly);
	qApp->setStyleSheet(qss.readAll());
	qss.close();
}


int main(int argc, char *argv[])
{
#if 0
	{
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
	}
#endif
	QApplication a(argc, argv);
	setStyle(":/Lemon01/black.qss");
	qRegisterMetaType<std::string>("std::string");
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	Lemon01 w;
	w.show();
	return a.exec();
}
