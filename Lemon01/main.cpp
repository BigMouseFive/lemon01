#include <winsock2.h>
#include "lemon01.h"
#include <qfile.h>
#include <QtWidgets/QApplication>
#include <qdir.h>
#include <tlhelp32.h>

void setStyle(const QString &style) {
	QFile qss(style);
	qss.open(QFile::ReadOnly);
	qApp->setStyleSheet(qss.readAll());
	qss.close();
}
void Wchar_tToString(std::string& szDst, wchar_t* wchar)
{
	wchar_t* wText = wchar;
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);
	char* psText;
	psText = new char[dwNum];
	WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);
	szDst = psText;
	delete[]psText;
}
//通过判断是否有同名的任务已经在运行了  存在问题：复制一个执行文件改个名字就可以破解了
bool isProcessRunA(std::string process_name)
{
	int g_ProcessRun = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) {
		return NULL;
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	for (BOOL ret = Process32First(hSnapshot, &pe); ret; ret = Process32Next(hSnapshot, &pe)) {
		wchar_t* process_str = pe.szExeFile;
		std::string  current_process_name;
		Wchar_tToString(current_process_name, process_str);
		if (current_process_name == process_name) {
			g_ProcessRun++;
		}
	}
	CloseHandle(hSnapshot);
	return g_ProcessRun > 1;
}
bool isProcessRunB(HANDLE& hMutex)
{
	hMutex = CreateMutex(NULL, FALSE, TEXT("EchinzeMutex"));
	if (GetLastError() == ERROR_ALREADY_EXISTS){
		return true;
	}
	else{
		return false;
	}
}
int main(int argc, char *argv[])
{
#if 0
	{
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
	}
#endif
	HANDLE hMutex;
	if (isProcessRunB(hMutex)){
		ReleaseMutex(hMutex);
		return -1;
	}
	QApplication a(argc, argv);
	setStyle(":/Lemon01/black.qss");
	qRegisterMetaType<std::string>("std::string");
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	Lemon01 w;
	w.show();
	a.exec();
	ReleaseMutex(hMutex);
	return 0;
}
