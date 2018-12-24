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
	headers << "EAN" << QStringLiteral("֪ͨ");
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
		//TODO::ˢ��tableview
	}
}

void AutoMachine::run(){
	char buffer[200];
	sprintf(buffer, "\"%s\" %s",
		_cmdline.c_str(),
		_name.c_str()
		);
	
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
		ATL::CA2T(_execPath), //ָ������Ŀ¼ 
		&StartInfo, //�ӽ��������������ʾ 
		&pinfo); //���ڴ���½��̵ķ�����Ϣ
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

		//ͨ��ShellExecuteEx() ��ɱ����Ӧ�����ĸļ۳���
		//�����֮ǰ��system()�ķ�ʽ��Ŀ����Ϊ�˲�����һ��������һ��
		SHELLEXECUTEINFO ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = NULL;
		ShExecInfo.lpFile = L"cmd.exe";//���õĳ�����
		ShExecInfo.lpParameters = szBuf;//���ó���������в���
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = SW_HIDE;//����״̬Ϊ����
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