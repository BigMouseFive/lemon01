#include "AutoMachine.h"
#include <iostream>
#define CMD_LINE "lemon02.exe"
typedef struct EnumHWndsArg
{
	std::vector<HWND> *vecHWnds;
	DWORD dwProcessId;
}EnumHWndsArg, *LPEnumHWndsArg;

BOOL CALLBACK lpEnumFunc(HWND hwnd, LPARAM lParam)
{
	EnumHWndsArg *pArg = (LPEnumHWndsArg)lParam;
	DWORD  processId;
	GetWindowThreadProcessId(hwnd, &processId);
	if (processId == pArg->dwProcessId)
	{
		pArg->vecHWnds->push_back(hwnd);
	}
	return TRUE;
}
AutoMachine::AutoMachine(std::string name, std::string password, float percent, float lowwer, int times, float upload, int minute, int mode, std::string loginUrl, QObject* parent)
	: QThread(parent)
	, _name(name)
	, _password(password)
	, _loginUrl(loginUrl)
	, _percent(percent)
	, _lowwer(lowwer)
	, _times(times)
	, _upload(upload)
	, _mintue(minute)
	, _mode(mode)
	{
	pinfo.hProcess = nullptr;
	pinfo.hThread = nullptr;
	pinfo.dwThreadId = 0;
	pinfo.dwProcessId = 0;
	_cmdline = CMD_LINE;
}
AutoMachine::~AutoMachine(){
	killProcess();
}

void AutoMachine::run(){
	char buffer[200];
	sprintf(buffer, "\"%s\" %s %s %.2f %.2f %d %.2f %d %d",
		_cmdline.c_str(),
		_name.c_str(),
		_password.c_str(),
		_percent,
		_lowwer,
		_times,
		_upload,
		_mode,
		_mintue
		);
	
	//int ret = WinExec(buffer, SW_SHOW);
	STARTUPINFO StartInfo;
	//�Գ����������Ϣ�����κ��趨��ȫ����0 
	memset(&StartInfo, 0, sizeof(STARTUPINFO));
	StartInfo.cb = sizeof(STARTUPINFO);//�趨�ṹ�Ĵ�С 
	StartInfo.wShowWindow = SW_HIDE;
	BOOL ret = CreateProcess(
		NULL, //��������·���� 
		ATL::CA2T(buffer), //��������exeNameΪNULLʱ���ɽ�����������ǰ�� 
		NULL, //ʹ��Ĭ�Ͻ��̰�ȫ���� 
		NULL, //ʹ��Ĭ���̰߳�ȫ���� 
		FALSE,//������̳� 
		NORMAL_PRIORITY_CLASS, //ʹ���������ȼ� 
		NULL, //ʹ�ø����̵Ļ������� 
		NULL, //ָ������Ŀ¼ 
		&StartInfo, //�ӽ��������������ʾ 
		&pinfo); //���ڴ���½��̵ķ�����Ϣ
	if (ret){
		emit success(_name);
		WaitForSingleObject(pinfo.hProcess, INFINITE);
		pinfo.hProcess = nullptr;
		pinfo.hThread = nullptr;
		pinfo.dwThreadId = 0;
		pinfo.dwProcessId = 0;
		emit stop(_name);
	}
	else{
		pinfo.hProcess = nullptr;
		pinfo.hThread = nullptr;
		pinfo.dwThreadId = 0;
		pinfo.dwProcessId = 0;
		emit failed(_name);
	}
}

void GetHWndsByProcessID(DWORD processID, std::vector<HWND> &vecHWnds)
{
	EnumHWndsArg wi;
	wi.dwProcessId = processID;
	wi.vecHWnds = &vecHWnds;
	EnumWindows(lpEnumFunc, (LPARAM)&wi);
}
void AutoMachine::killProcess(){
	if (pinfo.dwProcessId > 0){
		char szBuf[200];
		sprintf(szBuf, "\"c:\\windows\\system32\\taskkill.exe\" /PID %d /T /F", pinfo.dwProcessId);
		system(szBuf);
	}
}