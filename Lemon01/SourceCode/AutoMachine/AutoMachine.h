#pragma once
#include <qthread.h>
#include <string>
#include <Windows.h>
#include <atlbase.h>
#include "DataModel.h"
#include <map>
#include <qstandarditemmodel.h>

Q_DECLARE_METATYPE(std::string)

class AutoMachine : public QThread{
	Q_OBJECT
public:
	AutoMachine(std::string name, int type, QObject* parent = nullptr);
	~AutoMachine();
	void SetType(int type);
	void killProcess();
	void PLay();
	void Stop();
	void Pause();
	void SetStopState();
	void UpdateAttr(CPAttr& attr, std::vector<CPComplexAttr>& vec);
	void ReadAttr();
	CPAttr* GetCPAttr();
	int GetState();
	std::map<std::string, CPComplexAttr>* GetCPComplexAttr();
	std::vector<std::string>* GetWhiteEans();

signals:
	void SigSuccess(std::string);
	void SigFailed(std::string);
	void SigStop(std::string);
protected:
	void run();

private slots:
	void onTimer();

public:
	std::string _name;
	std::string _cmdline;
	int _type;
	char _execPath[200];
	CPAttr cpAttr;
	int timeStamp;
	std::map<std::string, CPComplexAttr> cpComplexAttr;
	std::vector<std::string> whiteEans;
 	QStandardItemModel* model;
	PROCESS_INFORMATION pinfo;
};
