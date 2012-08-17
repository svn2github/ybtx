#ifndef _CMapFile_Mgr_
#define _CMapFile_Mgr_

#include "CFileFormatMgr.h"
namespace sqr
{
class CMapFileMgr : public CFileFormatMgr
{
public:
	static FourCC ID_FILE;
	static FourCC ID_HEAD;
	static FourCC ID_MAP_ENVIRONMENT;
	static FourCC ID_MAP_TEXTURE;
	static FourCC ID_MAP_REGION;
	static FourCC ID_MAP_MODEL;
	static FourCC ID_MAP_OBJECT_INFO;
	static FourCC ID_MAP_WATER;
	static FourCC ID_MAP_LIGHTMAP;
public:
	struct FileHeader
	{
		DWORD create_date;
		char  author[16];
	};
public:
	CMapFileMgr(const string& filename,bool isPkg = false);
	~CMapFileMgr();
	bool Save();
protected:
	void ReadHeadInfo(const CDataChunk& inData);
	void WriteHeadInfo();
	FileHeader	fileHeader;
};
}

#endif