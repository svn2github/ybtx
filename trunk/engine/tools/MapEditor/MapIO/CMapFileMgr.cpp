#include "stdafx.h"
#include "CMapFileMgr.h"

FourCC CMapFileMgr::ID_FILE('MAPF');
FourCC CMapFileMgr::ID_HEAD('MAPH');
FourCC CMapFileMgr::ID_MAP_ENVIRONMENT('MENV');
FourCC CMapFileMgr::ID_MAP_TEXTURE('MTEX');
FourCC CMapFileMgr::ID_MAP_REGION('MRGN');
FourCC CMapFileMgr::ID_MAP_MODEL('MMDL');
FourCC CMapFileMgr::ID_MAP_OBJECT_INFO('MOPI');
FourCC CMapFileMgr::ID_MAP_WATER('MRWT');
FourCC CMapFileMgr::ID_MAP_LIGHTMAP('MRLM');
#define MAP_NEW_VER 1

CMapFileMgr::CMapFileMgr(const string& filename,bool isPkg)
:CFileFormatMgr(filename,isPkg)
{
	m_FileType = 'MAPF';
	m_stHeadSize = sizeof(FileHeader);
}

CMapFileMgr::~CMapFileMgr()
{
}

bool CMapFileMgr::Save()
{
	CoderMap::iterator it;
	m_dwVer = MAP_NEW_VER;
// 	if(BeginWrite())
// 	{
// 		it = m_ChunkCoder.find(ID_MAP_TEXTURE);
// 		if(it!=m_ChunkCoder.end())
// 			it->second->Code();
// 
// 		it = m_ChunkCoder.find(ID_MAP_ENVIRONMENT);
// 		if(it!=m_ChunkCoder.end())
// 			it->second->Code();
// 
// 		it = m_ChunkCoder.find(ID_MAP_MODEL);
// 		if(it!=m_ChunkCoder.end())
// 			it->second->Code();
// 
// 		it = m_ChunkCoder.find(ID_MAP_REGION);
// 		if(it!=m_ChunkCoder.end())
// 			it->second->Code();
// 
// 		it = m_ChunkCoder.find(ID_MAP_OBJECT_INFO);
// 		if(it!=m_ChunkCoder.end())
// 			it->second->Code();
// 
// 		it = m_ChunkCoder.find(ID_MAP_WATER);
// 		if(it!=m_ChunkCoder.end())
// 			it->second->Code();
// 
// 		it = m_ChunkCoder.find(ID_MAP_LIGHTMAP);
// 		if(it!=m_ChunkCoder.end())
// 			it->second->Code();
// 
// 		EndWrite();
// 		return true;
// 	}	
	return false;
}

void CMapFileMgr::ReadHeadInfo(const CDataChunk& inData)
{
	if(inData.getSize()!=m_stHeadSize)
		return;
	memcpy((char*)&fileHeader,inData.getBuffer(),m_stHeadSize);
}

void CMapFileMgr::WriteHeadInfo()
{
	fileHeader.create_date = 0;
	memcpy(fileHeader.author,"EndDuke:Test-MP\0",sizeof(fileHeader.author));
	m_realFile.write((char*)&ID_HEAD,sizeof(DWORD));
	m_realFile.write((char*)&m_stHeadSize,sizeof(DWORD));
	m_realFile.write((char*)&fileHeader,m_stHeadSize);
}