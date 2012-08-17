#pragma once
#include <vector>

namespace sqr
{
	struct  NetworkAdaptersInfo
	{
		std::string IPAddress;
		std::string PhysicalAddress;
	};

	void GetNetworkAdaptersInfo(std::vector<NetworkAdaptersInfo>& vec);
}