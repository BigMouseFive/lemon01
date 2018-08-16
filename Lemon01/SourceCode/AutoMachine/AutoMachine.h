#pragma once
#include <qthread.h>
#include <string>
#include <Windows.h>
#include <atlbase.h>

Q_DECLARE_METATYPE(std::string)

#define LOGIN_URL "https://uae.souq.com/ae-en/login.php"
class AutoMachine : public QThread{
	Q_OBJECT
public:
	AutoMachine(std::string name, std::string password, float percent, float lowwer, std::string loginUrl = LOGIN_URL, QObject* parent = nullptr);
	~AutoMachine();
	void killProcess();
signals:
	void success(std::string);
	void failed(std::string);
	void stop(std::string);
protected:
	void run();

public:
	std::string _cmdline;
	std::string _name;
	std::string _password;
	std::string _loginUrl;
	float _percent;
	float _lowwer;
	PROCESS_INFORMATION pinfo;
};
