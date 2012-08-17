#include "stdafx.h"
#include "NetworkAdaptersHelper.h"
#include <IPHlpApi.h>
#pragma comment(lib,"iphlpapi.lib")

namespace sqr
{
	void GetNetworkAdaptersInfo(std::vector<NetworkAdaptersInfo>& vec)
	{
		ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
		PIP_ADAPTER_INFO pAdapterInfo= NULL;
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);

		if (GetAdaptersInfo(pAdapterInfo,&ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
		{
			free(pAdapterInfo);
			pAdapterInfo = NULL;
			pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
		}

		if (GetAdaptersInfo(pAdapterInfo,&ulOutBufLen) == NO_ERROR)
		{
			PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
			while (pAdapter)
			{
				NetworkAdaptersInfo info;
				info.IPAddress = pAdapter->IpAddressList.IpAddress.String;
				char szAddress[10];
				for(DWORD i =0;i<pAdapter->AddressLength;++i)
				{
					sprintf_s(szAddress, "%.2x", pAdapter->Address[i]);
					info.PhysicalAddress.append(szAddress);
				}
				vec.push_back(info);
				pAdapter = pAdapter->Next;
			}
		}

		if (pAdapterInfo)
		{
			free(pAdapterInfo);
		}
	}
}