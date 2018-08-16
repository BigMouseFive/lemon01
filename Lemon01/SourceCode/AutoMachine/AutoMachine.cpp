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
AutoMachine::AutoMachine(std::string name, std::string password, float percent, float lowwer, std::string loginUrl, QObject* parent)
	: QThread(parent)
	, _name(name)
	, _password(password)
	, _loginUrl(loginUrl)
	, _percent(percent)
	, _lowwer(lowwer){
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
	sprintf(buffer, "\"%s\" %s %s %.2f %.2f",
		_cmdline.c_str(),
		_name.c_str(),
		_password.c_str(),
		_percent,
		_lowwer
		);
	//int ret = WinExec(buffer, SW_SHOW);
	STARTUPINFO StartInfo;
	//对程序的启动信息不作任何设定，全部清0 
	memset(&StartInfo, 0, sizeof(STARTUPINFO));
	StartInfo.cb = sizeof(STARTUPINFO);//设定结构的大小 
	StartInfo.wShowWindow = SW_HIDE;
	BOOL ret = CreateProcess(
		NULL, //启动程序路径名 
		ATL::CA2T(buffer), //参数（当exeName为NULL时，可将命令放入参数前） 
		NULL, //使用默认进程安全属性 
		NULL, //使用默认线程安全属性 
		FALSE,//句柄不继承 
		NORMAL_PRIORITY_CLASS, //使用正常优先级 
		NULL, //使用父进程的环境变量 
		NULL, //指定工作目录 
		&StartInfo, //子进程主窗口如何显示 
		&pinfo); //用于存放新进程的返回信息
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
		//DWORD processId = pinfo.dwProcessId;
		//PROCESSENTRY32 processEntry = { 0 };
		//processEntry.dwSize = sizeof(PROCESSENTRY32);
		////给系统内的所有进程拍一个快照
		//HANDLE handleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		////遍历每个正在运行的进程
		//if (Process32First(handleSnap, &processEntry)){
		//	BOOL isContinue = TRUE;
		//	//终止子进程
		//	do{
		//		if (processEntry.th32ParentProcessID == processId){
		//			HANDLE hChildProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry.th32ProcessID);
		//			if (hChildProcess){
		//				TerminateProcess(hChildProcess, 0);
		//				CloseHandle(hChildProcess);
		//			}
		//		}
		//		isContinue = Process32Next(handleSnap, &processEntry);
		//	} while (isContinue);

		//	HANDLE hBaseProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
		//	if (hBaseProcess){
		//		TerminateProcess(hBaseProcess, 0);
		//		CloseHandle(hBaseProcess);
		//	}
		//}
	}
}