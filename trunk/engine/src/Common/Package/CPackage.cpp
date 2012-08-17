#include "stdafx.h"
#include "BaseHelper.h"
#include "ExpHelper.h"

#ifdef _WIN32
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include "CPackage.h"
#include "CPkgFile.h"
#include "PkgCommon.h"
#include "ThreadHelper.h"
#include "CodeCvs.h"

#ifdef _WIN32
#include <io.h>
#include <errno.h>
#endif


HTLSKEY CPackage::ms_tlsPackageKey = INVALID_TLS_KEY;
CPackage::MapPackage_t CPackage::ms_mapPackage;

void CPackage::InitTLSKey()
{
	TLS_CreateKey(&ms_tlsPackageKey);
}

void CPackage::UnitTLSKey()
{
	TLS_DestroyKey(ms_tlsPackageKey);
	ms_tlsPackageKey = INVALID_TLS_KEY;
}

void CPackage::InitTLSData()
{
	MapPackage_t* pMapPackage = new MapPackage_t;
	TLS_SetValue(ms_tlsPackageKey, pMapPackage);
}

void CPackage::UnitTLSData()
{
	MapPackage_t* pMapPackage = (MapPackage_t*)TLS_GetValue(ms_tlsPackageKey);
	delete pMapPackage;
	TLS_SetValue(ms_tlsPackageKey, NULL);
}

CPackage::MapPackage_t& CPackage::_CPackage_map()
{
	MapPackage_t* pMapPackage = (MapPackage_t*)TLS_GetValue(ms_tlsPackageKey);
	if(!pMapPackage)
		return ms_mapPackage;

	return *pMapPackage;
}

int CPackage::Load(CPackage** ppPkg, const wchar_t *szName)
{
	*ppPkg = 0;
	string szPkgNameU8 = UCS2ToUtf8(szName);
	int id = FileName2Id( szPkgNameU8.c_str() );

	MapPackage_t::iterator iter = _CPackage_map().find( id );
	if ( iter!=_CPackage_map().end() )
	{
		*ppPkg = &iter->second;
		return eFE_SUCCESS;
	}

	void* pFile = 0;
	int openResult = OpenFile( &pFile, szName, true );
	if ( !pFile )
		return openResult;

	PackageHead		packHead;
	PackFileIndex	packHeadIndex;
	if ( !ReadPackageHeadInfo(packHead, packHeadIndex, (FILE*)pFile) )
	{
		CloseFile( pFile );
		return eFE_PKG_DAMAGED;
	}

	CPackage& Package = _CPackage_map()[id];
	SeekToFileIndexList((FILE*)pFile, packHead);
	for( int32 i = 0; i < packHead.number; i++ )
	{
		PackFileIndex curFileIndex;
		ReadPackageFileIndex(curFileIndex, packHead, (FILE*)pFile);
		Package.m_FileIndex.insert(make_pair(curFileIndex.uid, curFileIndex));
	}
	Package.m_bXor			= (packHead.bXor & 0x000000ff) != 0;
	Package.m_pFileBin		= pFile;
	OpenFile( &Package.m_pFile, szName, false );

#ifdef _WIN32
	Package.m_pAsSynFile	= CreateFileW( szName, GENERIC_READ, FILE_SHARE_READ,NULL,OPEN_EXISTING, 0, NULL );
#endif
	*ppPkg = &Package;
	return eFE_SUCCESS;
}

int CPackage::OpenFile( void** ppFileOut, const wchar_t * szName, bool bBinary )
{
	*ppFileOut = 0;

#ifdef _WIN32
	*ppFileOut = _wfopen( szName, bBinary ? L"rb" : L"r" );
	if ( (*ppFileOut) == 0  )
	{
		errno_t err;
		if ( _waccess(szName, 0) == -1 )
		{
			_get_errno(&err);
			switch ( err )
			{
			case ENOENT:
				return eFE_RAW_FILE_NAME_OR_PATH_NOT_FOUND;
				break;

			case EACCES:
				return eFE_RAW_FILE_CANNOT_BE_ACCESS;
				break;

			case EINVAL:
				return eFE_RAW_FILE_OPEN_INVALID_PARAM;
				break;
			}
		}
		else
		{
			return eFE_RAW_FILE_CANNOT_BE_ACCESS;
		}
	}

#else
	*ppFileOut = fopen( utf16_to_utf8(szName).c_str(), bBinary ? "rb" : "r" );
	if ( 0 == *ppFileOut )
		return eFE_PKG_OPEN_FAILED;

#endif

	return eFE_SUCCESS;
}

int32 CPackage::ReadFile(void* hFile, void * pBuffer, size_t size)
{
	return (int32)fread(pBuffer, 1, size, (FILE*)hFile );
}

uint32 CPackage::FileSize(void* hFile)
{
	fseek( (FILE*)hFile, 0, SEEK_END );
	int32 nLength = ftell( (FILE*)hFile );
	fseek( (FILE*)hFile, 0, SEEK_SET );
	return nLength;
}

void CPackage::SeekFile(void* hFile,size_t pos)
{
	fseek( (FILE*)hFile, (long)pos, SEEK_SET );
}

void CPackage::CloseFile(void* hFile)
{
	fclose( (FILE*)hFile );
}

CPackage::CPackage()
: m_pFile( NULL )
, m_pFileBin( NULL )
#ifdef _WIN32
, m_pAsSynFile( NULL )
#endif
{
	m_bXor = false;
}

CPackage::~CPackage()
{
	if( m_pFile )
		CloseFile( m_pFile );
	if( m_pFileBin )
		CloseFile( m_pFileBin );

#ifdef _WIN32
	if( m_pAsSynFile )
		CloseHandle( m_pAsSynFile );
#endif
}

bool CPackage::Open( const char* szFileName, uint32& uPos, uint32& uLength, bool& bXOR, uint32& uCheckCode )
{
	map<uint32, PackFileIndex>::const_iterator it = m_FileIndex.find( FileName2Id(szFileName) ); // 寻找当前文件名对应的uID
	if( it == m_FileIndex.end() )
		return false;

	uPos	= it->second.offset;
	uLength = it->second.size;
	uCheckCode = it->second.check_code;
	bXOR	= m_bXor;
	
	return true;
}
