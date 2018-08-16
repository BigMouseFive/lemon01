#define  _CRT_SECURE_NO_WARNINGS
#include "MacAddress.h"
#include <cstdio>
#include <iphlpapi.h>
#include <atlbase.h>
#pragma comment(lib, "Iphlpapi.lib")
char a[300];
bool CTemporary::GetMacAddress(std::vector<std::string>& vtMacAddress)
{
	ULONG unAddrBuffSize = 0;
	ULONG unResult = GetAdaptersAddresses(AF_UNSPEC,
		GAA_FLAG_SKIP_ANYCAST, NULL, NULL, &unAddrBuffSize);
	if (ERROR_BUFFER_OVERFLOW != unResult)
	{
		return FALSE;
	}

	PIP_ADAPTER_ADDRESSES pAdapterSet =	(PIP_ADAPTER_ADDRESSES)new BYTE[unAddrBuffSize];
	ZeroMemory(pAdapterSet, unAddrBuffSize);
	if (ERROR_SUCCESS != GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_SKIP_ANYCAST, NULL, pAdapterSet, &unAddrBuffSize))
	{
		delete[]pAdapterSet;
		return FALSE;
	}

	PIP_ADAPTER_ADDRESSES pIndex = pAdapterSet;
	while (pIndex)
	{
		PIP_ADAPTER_ADDRESSES pAdapter = pIndex;
		pIndex = pIndex->Next;

		std::wstring tmp = ATL::CA2T(pAdapter->AdapterName);
		if (FALSE == _IsPhysicalAdapter(tmp))
		{
			continue;
		}
		memset(a, 0, sizeof(a));
		sprintf(a, "%.2X%.2X%.2X%.2X%.2X%.2X"
			, pAdapter->PhysicalAddress[0], pAdapter->PhysicalAddress[1]
			, pAdapter->PhysicalAddress[2], pAdapter->PhysicalAddress[3]
			, pAdapter->PhysicalAddress[4], pAdapter->PhysicalAddress[5]);
		std::string sTmp(a);
		vtMacAddress.push_back(sTmp);
	}

	delete[]pAdapterSet;

	return TRUE;
}

bool CTemporary::_IsPhysicalAdapter(const std::wstring& sAdapterName)
{
	std::wstring sKey(L"");
	sKey = L"SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\" + sAdapterName + L"\\Connection";
	bool bResult = FALSE;
	ATL::CRegKey regKey;
	ULONG unKeyLen = 0;
	PTSTR pKeyValue = NULL;
	do
	{
		int ret = regKey.Create(HKEY_LOCAL_MACHINE, ATL::CW2W(sKey.c_str()));
		if (ERROR_SUCCESS != ret)
		{
			break;
		}

		regKey.QueryStringValue(_T("PnPInstanceId"), pKeyValue, &unKeyLen);
		if (0 == unKeyLen)
		{
			break;
		}

		pKeyValue = new TCHAR[unKeyLen];
		ZeroMemory(pKeyValue, sizeof(TCHAR)*(unKeyLen));
		if (ERROR_SUCCESS != regKey.QueryStringValue(
			_T("PnPInstanceId"), pKeyValue, &unKeyLen))
		{
			break;
		}

		if (0 != _tcsncmp(pKeyValue, _T("PCI\\"), 4))
		{
			break;
		}

		bResult = TRUE;
	} while (FALSE);

	if (pKeyValue)
	{
		delete[]pKeyValue;
	}

	return bResult;
}