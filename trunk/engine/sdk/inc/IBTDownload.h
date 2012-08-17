#pragma once
#include <string>
#include <vector>
using namespace std;

namespace sqr
{
#pragma pack(push,1)
typedef union
{
	DWORD	dwVersion;
	struct
	{
		WORD wBuildVersion;		// for little ending system
		BYTE byLoVersion;
		BYTE byHiVersion;
	};
}VERSION;
#pragma pack(pop)

struct UpdatePacket
{
	VERSION		ver;
	string		url;
};

typedef vector<UpdatePacket> PatchList;

class IBTDownload
{
public:
	virtual void GetDownloadedPacket(PatchList& path) = 0;
	virtual long long GetFileLen() = 0;
};
}