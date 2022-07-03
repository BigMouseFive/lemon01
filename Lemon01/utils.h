#pragma once
#include <WinSock2.h>
#include <time.h>
#pragma comment(lib, "WS2_32.lib")
DWORD GetTimeFromServer(char *ip_addr)
{
	// 参数ip_addr:表示指定的时间服务器IP地址
	// 返回：自1900年1月1日午0时0分0秒至今的毫秒数 或 0（表示获取失败）
	// 默认的时间服务器为"国家授时中心"
	// 定义WSADATA结构体对象
	WSADATA date;
	WORD w = MAKEWORD(2, 0);// 定义版本号码
	if (::WSAStartup(w, &date) != 0)// 初始化套接字库
		return 0;
	SOCKET s;// 定义连接套接字句柄
	DWORD  m_serverTime;// 定义接收信息保存变量
	s = ::socket(AF_INET, SOCK_STREAM, 0);// 创建TCP套接字
	if (INVALID_SOCKET == s)
	{
		::closesocket(s);// 关闭套接字句柄
		::WSACleanup();// 释放套接字库
		return 0;
	}
	sockaddr_in addr;// 定义套接字地址结构
	addr.sin_family = AF_INET;// 初始化地址结构
	addr.sin_port = htons(37);
	addr.sin_addr.S_un.S_addr = inet_addr(ip_addr);
	if (::connect(s, (sockaddr*)&addr, sizeof(addr)) != 0)// 连接
	{
		::closesocket(s);// 关闭套接字句柄
		::WSACleanup();// 释放套接字库
		return 0;
	}
	if (::recv(s, (char *)&m_serverTime, 4, MSG_PEEK) <= 0)// 接收
	{
		::closesocket(s);// 关闭套接字句柄
		::WSACleanup();// 释放套接字库
		return 0;
	}

	::closesocket(s);// 关闭套接字句柄
	::WSACleanup();// 释放套接字库
	m_serverTime = ::ntohl(m_serverTime);// 网络字节顺序转换为主机字节顺序
	return m_serverTime;
}
DWORD GetTimeFromSystem(){
	time_t t = time(nullptr);
	DWORD nowTime = 0;
	nowTime = t + 2208988800;
	return nowTime;
}
DWORD GetTime(){
	char a[20][20];
	DWORD time ;
	/*strcpy(a[0], "129.6.15.29");
	strcpy(a[1], "128.138.140.44");
	strcpy(a[2], "129.6.15.28");
	strcpy(a[3], "210.72.145.8");
	for (int i = 3; i >= 0; --i){
		time = GetTimeFromServer(a[i]);
		if (time != 0) return time;
	}*/
	time = GetTimeFromSystem();
	return time;
}

SYSTEMTIME FormatServerTime(DWORD serverTime = 0)
{
	FILETIME      ftNew;
	SYSTEMTIME    stNew;

	stNew.wYear = 1900;
	stNew.wMonth = 1;
	stNew.wDay = 1;
	stNew.wHour = 0;
	stNew.wMinute = 0;
	stNew.wSecond = 0;
	stNew.wMilliseconds = 0;
	::SystemTimeToFileTime(&stNew, &ftNew);

	/*  将SYSTEMTIME结构设定为1900年1月1日午夜（0时）。
	并将这个SYSTEMTIME结构传递给SystemTimeToFileTime，将此结构转化为FILETIME结构。
	FILETIME实际上只是由两个32位元的DWORD一起组成64位元的整数，
	用来表示从1601年1月1日至今间隔为100奈秒（nanosecond）的间隔数。 */

	LARGE_INTEGER li;			//64位大整数
	li = *(LARGE_INTEGER *)&ftNew;
	if (serverTime == 0)
		serverTime = GetTime();
	li.QuadPart += (LONGLONG)10000000 * serverTime;
	ftNew = *(FILETIME *)&li;
	::FileTimeToSystemTime(&ftNew, &stNew);

	// 返回时间（注意：这里返回的是格林尼治时间，与北京时间相差8小时）
	return stNew;
}

bool firstflag = true;
int CompareTime(DWORD endtime, int nowtime){
	if (nowtime == 0){
		nowtime = ::GetTime();
	}
	if (nowtime == 0){
		return 300;
	}
	else{
		DWORD difftime;
		if (endtime <= nowtime){
			difftime = nowtime - endtime;
			difftime = difftime / 3600;
			if (difftime >= 48)
				return 302;
			if (firstflag) return difftime;
			else if (difftime == 12)
				return difftime;
			else if (difftime == 24)
				return difftime;
			else if (difftime == 36)
				return difftime;
		}
	}
	return 301;
}