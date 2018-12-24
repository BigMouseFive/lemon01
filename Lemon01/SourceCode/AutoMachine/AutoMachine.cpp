#include "AutoMachine.h"
#include <iostream>
#include "DataManager.h"
#include <qapplication.h>
#include <qtimer.h>
#include <qdebug.h>

#define CMD_LINE "DNCAT\\dist\\deprecated\\deprecated.exe"
typedef struct EnumHWndsArg
{
	std::vector<HWND> *vecHWnds;
	DWORD dwProcessId;
}EnumHWndsArg, *LPEnumHWndsArg;

//
//struct handle_data {
//	unsigned long process_id;
//	HWND best_handle;
//};
//
//BOOL IsMainWindow(HWND handle)
//{
//	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
//}
//
//
//HWND FindMainWindow(unsigned long process_id)
//{
//	handle_data data;
//	data.process_id = process_id;
//	data.best_handle = 0;
//	EnumWindows(EnumWindowsCallback, (LPARAM)&data);
//	return data.best_handle;
//}
//
//BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
//{
//	handle_data& data = *(handle_data*)lParam;
//	unsigned long process_id = 0;
//	GetWindowThreadProcessId(handle, &process_id);
//	if (data.process_id != process_id || !IsMainWindow(handle)) {
//		return TRUE;
//	}
//	data.best_handle = handle;
//	return FALSE;
//}
//BOOL CALLBACK lpEnumFunc(HWND hwnd, LPARAM lParam)
//{
//	EnumHWndsArg *pArg = (LPEnumHWndsArg)lParam;
//	DWORD  processId;
//	GetWindowThreadProcessId(hwnd, &processId);
//	if (processId == pArg->dwProcessId)
//	{
//		pArg->vecHWnds->push_back(hwnd);
//	}
//	return TRUE;
//}
//
//
//void GetHWndsByProcessID(DWORD processID, std::vector<HWND> &vecHWnds)
//{
//	EnumHWndsArg wi;
//	wi.dwProcessId = processID;
//	wi.vecHWnds = &vecHWnds;
//	EnumWindows(lpEnumFunc, (LPARAM)&wi);
//}


AutoMachine::AutoMachine(std::string name, QObject* parent)
	: QThread(parent)
	, _name(name)
	, timeStamp(0)
{
	cpAttr.lowwer = 0;
	cpAttr.max_percent = 0.15;
	cpAttr.max_times = 5;
	cpAttr.minute = 0;
	cpAttr.my_shop = "";
	cpAttr.percent = 0.10;
	cpAttr.shop = name;
	cpComplexAttr.clear();
	ReadAttr();
	cpAttr.control = MACHINE_STOP;

	pinfo.hProcess = nullptr;
	pinfo.hThread = nullptr;
	pinfo.dwThreadId = 0;
	pinfo.dwProcessId = 0;

	QStringList headers;
	headers << "EAN" << QStringLiteral("通知");
	model = new QStandardItemModel(this);
	model->setHorizontalHeaderLabels(headers);
	model->setColumnCount(2);
	
	_cmdline = CMD_LINE;
	memset(_execPath, 0, sizeof(_execPath));
	GetModuleFileNameA(NULL, _execPath, 200);
	int i = 0;
	int j = 0;
	while (_execPath[i] != '\0'){
		if (_execPath[i] == '\\'){
			_execPath[i] = '/';
			j = i;
		}
		i++;
	}
	_execPath[j] = '\0';
	strncat(_execPath, "/DNCAT/dist/deprecated/", 23);
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	timer->start(2000);
}
AutoMachine::~AutoMachine(){
	killProcess();
}

void AutoMachine::onTimer(){
	std::vector<ShopNotice> vec;
	DataManager::GetInstance()->GetNotice(_name, timeStamp, vec);
	if (vec.size() > 0){
		int row = model->rowCount() - 1;
		for (auto iter = vec.begin(); iter != vec.end(); ++iter, ++row){
			timeStamp = iter->timeStamp;
			QStandardItem *item0 = new QStandardItem(QString::fromStdString(iter->ean));
			QStandardItem *item1 = new QStandardItem(QString::fromStdString(iter->notice));
			model->setItem(row, 0, item0);
			model->setItem(row, 1, item1);
		}
		//TODO::刷新tableview
	}
}

void AutoMachine::run(){
	char buffer[200];
	sprintf(buffer, "\"%s\" %s",
		_cmdline.c_str(),
		_name.c_str()
		);
	
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
		ATL::CA2T(_execPath), //指定工作目录 
		&StartInfo, //子进程主窗口如何显示 
		&pinfo); //用于存放新进程的返回信息
	if (ret){
		cpAttr.control = MACHINE_PLAY;
		WaitForSingleObject(pinfo.hProcess, INFINITE);
		pinfo.hProcess = nullptr;
		pinfo.hThread = nullptr;
		pinfo.dwThreadId = 0;
		pinfo.dwProcessId = 0;
		emit SigStop(_name);
	}
	else{
		pinfo.hProcess = nullptr;
		pinfo.hThread = nullptr;
		pinfo.dwThreadId = 0;
		pinfo.dwProcessId = 0;
		emit SigFailed(_name);
	}
}

void AutoMachine::killProcess(){
	if (pinfo.dwProcessId > 0){
		//char buffer[100];
		//sprintf(buffer, "taskkill.exe /PID %d /T /F", pinfo.dwProcessId);

		/*
		wchar_t szBuf[200];
		wsprintf(szBuf, L"/C taskkill.exe /PID %d /T /F", pinfo.dwProcessId);

		//通过ShellExecuteEx() 来杀死对应开启的改价程序
		//替代了之前的system()的方式，目的是为了不会有一个窗口闪一下
		SHELLEXECUTEINFO ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = NULL;
		ShExecInfo.lpFile = L"cmd.exe";//调用的程序名
		ShExecInfo.lpParameters = szBuf;//调用程序的命令行参数
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = SW_HIDE;//窗口状态为隐藏
		ShExecInfo.hInstApp = NULL;
		ShellExecuteEx(&ShExecInfo);
		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);*/
	}
}

void AutoMachine::PLay(){
	if (cpAttr.control == MACHINE_STOP){
		this->start();
	}
	cpAttr.control = MACHINE_PLAY;
	DataManager::GetInstance()->UpdateControl(cpAttr.control, cpAttr.shop);
}
void AutoMachine::Stop(){
	killProcess();
	SetStopState();
}
void AutoMachine::Pause(){
	cpAttr.control = MACHINE_PAUSE;
	DataManager::GetInstance()->UpdateControl(cpAttr.control, cpAttr.shop);
}
void AutoMachine::SetStopState(){
	cpAttr.control = MACHINE_STOP;
	DataManager::GetInstance()->UpdateControl(cpAttr.control, cpAttr.shop);
}

void AutoMachine::UpdateAttr(CPAttr& attr, std::vector<CPComplexAttr>& vec){
	cpAttr.Copy(attr);
	DataManager::GetInstance()->AddCPAttr(cpAttr);
	std::vector<std::string> eans;
	for (auto it = vec.begin(); it != vec.end(); ++it){
		auto iter = cpComplexAttr.find(it->ean);
		eans.push_back(it->ean);
		if (iter != cpComplexAttr.end()){
			if (iter->second.least_price != it->least_price ||
				iter->second.max_times != it->max_times){
				iter->second.least_price = it->least_price;
				iter->second.max_times = it->max_times;
				DataManager::GetInstance()->AddCPComplexAttr(iter->second, _name);
			}
		}
		else{
			cpComplexAttr[it->ean] = *it;
			DataManager::GetInstance()->AddCPComplexAttr(*it, _name);
		}
	}
	for (auto it = cpComplexAttr.begin(); it != cpComplexAttr.end(); ){
		auto iter = std::find(std::begin(eans), std::end(eans), it->first);
		if (iter == std::end(eans)){
			DataManager::GetInstance()->DelCPComplexAttr(it->first, _name);
			it = cpComplexAttr.erase(it);
		}
		else{
			++it;
		}
	}
}
void AutoMachine::ReadAttr(){
	if (DataManager::GetInstance()->GetCPAttr(cpAttr, _name) != SQL_OK){
	}
	if (DataManager::GetInstance()->GetCPComplexAttr(cpComplexAttr, _name) != SQL_OK){
	}
}
CPAttr* AutoMachine::GetCPAttr(){
	return &cpAttr;
}
std::map<std::string, CPComplexAttr>* AutoMachine::GetCPComplexAttr(){
	return &cpComplexAttr;
}
int AutoMachine::GetState(){
	return cpAttr.control;
}