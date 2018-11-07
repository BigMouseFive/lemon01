#ifndef LEMON01_H
#define LEMON01_H

#include <QtWidgets/QWidget>
#include "ui_lemon01.h"
#include "SourceCode\AutoMachine\AutoMachine.h"
#include <qmenu.h>
#include <qaction.h>
#include <map>
#include <string>
typedef struct{
	std::string name;
	std::string password;
	float percent;
	float lowwer;
	int times;
	float upload;
	int minute;
	int mode;
}Info;
class Lemon01 : public QWidget
{
	Q_OBJECT

public:
	Lemon01(QWidget *parent = 0);
	~Lemon01();

private slots:
	void SlotNewAutoMachine();
	void SlotAutoFinish(std::string);
	void SlotAutoFailed(std::string);
	void SlotAutoStop(std::string);
	void SlotListContextRequested(const QPoint&);
	void SlotDelAct(bool);
	void SlotCloseAct(bool);
	void SlotListItemClicked(QListWidgetItem *item);
	void SlotListItemClicked0(QListWidgetItem *, QListWidgetItem *);
	void SlotCompareTime();
private:
	int aaaab(); 
	void QuitThisSystem();
	void setEmpty();
	void initList();
	void createXml();
	void readXml();
	void delXml(std::string name);
	void writeXml(std::string name, std::string password, float percent, float lowwer, int times, float upload, int minute, int mode);
private:
	Ui::Lemon01Class ui;
	std::map<std::string, AutoMachine*> threadMap;
	std::map<std::string, Info*> infoMap;
	QAction* delAct;
	QAction* closeAct;
	std::string selectName;
	int _show[20];
	int hide[32];
	int _hide[32];
	char currentDir[200];
	char input[200];
	char filepath[200];
	char endTime[20];
	DWORD endtime;	
	int total;
	int lastret;
};

#endif // LEMON01_H
