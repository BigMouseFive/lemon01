#pragma once
#include <winsock2.h>
#include <vector>
#include <string>

class CTemporary
{
public:
    CTemporary() {};

    bool GetMacAddress(std::vector<std::string>& vtMacAddress);

private:
	bool _IsPhysicalAdapter(const std::wstring& sAdapterName);
};