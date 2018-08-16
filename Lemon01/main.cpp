#include <winsock2.h>
#include "lemon01.h"
#include <qfile.h>
#include <QtWidgets/QApplication>


void setStyle(const QString &style) {
	QFile qss(style);
	qss.open(QFile::ReadOnly);
	qApp->setStyleSheet(qss.readAll());
	qss.close();
}


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	setStyle(":/Lemon01/black.qss");
	qRegisterMetaType<std::string>("std::string");
	Lemon01 w;
	w.show();
	return a.exec();
}
