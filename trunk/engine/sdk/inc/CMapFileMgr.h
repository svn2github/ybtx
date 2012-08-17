#pragma once
#include <vector>
#include <map>
#include "CMallocObject.h"
#include "TMallocAllocator.h"
#include "ModuleDefs.h"

namespace sqr
{

	class CAddrFunContainer;

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4275)	//dll linkage
#pragma warning(disable:4251)	//dll linkage
#endif

	class CMapFile;

	class MODULE_API CMapFileMgr : public CMallocObject
	{
	public:
		CMapFileMgr();
		~CMapFileMgr();

		static CMapFileMgr& Inst();

	void HandleMapFile(const wchar_t* szFileName, uint32 uVersion = 0);

		const char* FindFunName(uint32 uFunAddr, uint32 uVersion = 0);

		void ClearMapFile();

	private:
		CMapFile* GetMapFile(uint32 uVersion);

	private:
		typedef std::map<uint32, CMapFile*, less<uint32>, 
			TMallocAllocator<pair<uint32, CMapFile*> > >	MapVersionToMapFile_t;
		MapVersionToMapFile_t		m_mapVersionToMapFile;
	};


#ifdef _WIN32
#pragma warning(pop)
#endif
}

