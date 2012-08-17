#pragma once
#include <string>
#include <vector>
using namespace std;

namespace sqr
{
#pragma pack(push,1)
union VERSION
{
	uint32  dwVersion;
	struct
	{
		uint32 wBuildVersion;		// for little ending system
		uint8 byLoVersion;
		uint8 byHiVersion;
	};
};
#pragma pack(pop)

struct UpdatePacket
{
	VERSION  ver;
	string   url;
};

typedef vector<UpdatePacket> PatchList;
}