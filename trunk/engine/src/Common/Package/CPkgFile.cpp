#include "stdafx.h"
#include "ExpHelper.h"
#include "ErrLogHelper.h"
#include "CPkgFile.h"

#include "CPackage.h"
#include "CBufFile.h"
#include "ThreadHelper.h"
#include <algorithm>
#include "CTimeCheckPoint.h"
#include "CReadThread.h"

#include "CPathMgr.h"
#include "CLock.inl"
#include "CodeCvs.h"
#include "UtfCvs.h"

#ifndef _WIN32
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#define  MAX_PATH  PATH_MAX
#else
#include <direct.h>
#endif


#ifndef _WIN32
#define _chdir chdir
#define _mkdir mkdir
#define _getcwd getcwd
#endif

using namespace sqr;


#include "TraceHelper.h"
using namespace sqr;
#define CHECK_FNTIME(g_ElapseTime)	\
	CTimeCheckPoint uTimer;\
	uint64 g_uTime=0;\
{\
	g_uTime = uTimer.GetHDElapse();\
}

#define CHECK_FNEND(szTmpMsg,szFileName) \
	g_uTime = uTimer.GetHDElapse() - g_uTime;\
	if(g_uTime>10000)\
{\
	DebugOut("%s time = %I64d,%s",szTmpMsg,g_uTime,szFileName);\
}

//#define PRINT_DMA_LOADING( uBeginTime )
#define PRINT_DMA_LOADING( uBeginTime ) printf("Time:%I64d Loaded!\n",uBeginTime)
//#define PRINT_DMA_LOADING( uBeginTime ) printf( "Time:%d, %s Loaded!\n", (int32)TimeCheck.GetElapse(), pCallBack->m_szFileName.c_str() )
/*
#define PRINT_DMA_LOADING( uBeginTime ) \
struct ___\
{ \
multimap<uint32,string> m_LoadFile;\
~___()\
{\
FILE* fp = _wfopen( L"c:\\a.txt", L"w" );\
for( multimap<uint32,string>::iterator it = m_LoadFile.begin(); it != m_LoadFile.end(); it++ )\
fprintf( fp, "Time:%d, %s\n", it->first, it->second.c_str() );\
}\
}; \
static ___ s____; \
s____.m_LoadFile.insert( pair<uint32,string>( (uint32)( TimeCheck.GetElapse() - uBeginTime ), pCallBack->m_szFileName ) ); \
*/

//	enum { openfail=1,packagefail=2,openfailinpackage=3, extend };

namespace sqr
{
	inline static void BufferXor(char * buffer,uint32 uPosition, size_t uBytes)
	{
		const uint8* szPackKey=GetPackKey();

		for( unsigned i = 0; i < uBytes; i++ )
			buffer[i] = buffer[i]^szPackKey[( uPosition + i )&0xf];
	}


#ifdef _WIN32
	class CReadFileCallBackData
	{
	private:
		friend class CPkgFile;
		ASYN_CALLBACK	m_pFun;
		void*			m_pContext;
		CRefBufFile*	m_pBuff;
		HANDLE			m_hFile;
		bool			m_bXor;
		uint32			m_uCheckCode;
		OVERLAPPED		m_OverLapped;

		static CLock							ms_Lock;
		static list<CReadFileCallBackData*>		ms_listLoaded;
		static list<CReadFileCallBackData*>		ms_listNotLoad;
		static int32							ms_nLoading;
		static bool								ms_bEnableAsyncRead;
		static bool								ms_bGlobeEnableAsyncRead;


	public:

		string			m_szFileName;
		CReadFileCallBackData(ASYN_CALLBACK pFun, void * pContext, size_t size, HANDLE pHandle,uint32 offset ,bool bXor, uint32 uCheckCode, const char* szFileName)
			:m_pFun(pFun), m_pContext(pContext), m_hFile(pHandle), m_bXor(bXor), m_uCheckCode(uCheckCode)
		{
			m_szFileName			= szFileName;
			m_OverLapped.Offset		= offset;
			m_OverLapped.OffsetHigh = 0;
			m_OverLapped.hEvent		= this;
			m_pBuff = new CRefBufFile( new char[size], size, offset, uCheckCode);
			m_pBuff->AddRef();
			//m_strBuff.resize(size);
		}

		~CReadFileCallBackData()
		{
			if( !m_OverLapped.Offset )
				CloseHandle(m_hFile);
			m_pBuff->Release();
		}

		void *		  GetContext(){return m_pContext;}
		const char *  GetBuff(){return m_pBuff->GetBuf();}
		size_t		  GetBuffSize(){return m_pBuff->GetBufSize();}
		CRefBufFile*  GetRefBufFile(){ return m_pBuff; }
		ASYN_CALLBACK GetFun(){return m_pFun;}
		OVERLAPPED*	  GetOverLapped(){return &m_OverLapped;}
		bool          GetXor(){return m_bXor;}

		static void FileIOCompletionRoutine(uint32 dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
		{
			ms_Lock.Lock();
			ms_listLoaded.push_back(static_cast<CReadFileCallBackData *>(lpOverlapped->hEvent));
			ms_Lock.Unlock();
		}

		static bool Flush(uint64 uElapseTime)
		{
			CTimeCheckPoint TimeCheck;

			bool bResult = false;

			//CHECK_FNTIME(uElapseTime);
			//if(uElapseTime==-1)
			//	return bResult;

			while( TimeCheck.GetHDElapse() < uElapseTime )
			{
				ms_Lock.Lock();
				if( ms_listLoaded.empty() )
				{
					ms_Lock.Unlock();
					break;
				}
				CReadFileCallBackData* pCallBack = ms_listLoaded.front();
				ms_listLoaded.erase( ms_listLoaded.begin() );
				ms_Lock.Unlock();

				if( pCallBack->GetXor() )
					BufferXor(const_cast<char *>(pCallBack->GetBuff()), 0, pCallBack->GetBuffSize());

				/*uint64 uTime=GetProcessTime();*/

				SQR_TRY
				{
					//pBufFile->AddRef();
					pCallBack->GetFun()(pCallBack->GetContext(), *(pCallBack->GetRefBufFile()) );
					//pBufFile->Release();
				}
				SQR_CATCH(exp)
				{
					LogExp(exp);
				}
				SQR_TRY_END;

				delete pCallBack;
				Ast( CReadFileCallBackData::ms_nLoading );
				CReadFileCallBackData::ms_nLoading--;
			}
			bResult = !ms_listLoaded.empty();
			return bResult;
		}
	};

	int32							CReadFileCallBackData::ms_nLoading				= 0;
	list<CReadFileCallBackData*>	CReadFileCallBackData::ms_listLoaded;
	list<CReadFileCallBackData*>	CReadFileCallBackData::ms_listNotLoad;
	bool							CReadFileCallBackData::ms_bEnableAsyncRead		= false;
	bool							CReadFileCallBackData::ms_bGlobeEnableAsyncRead	= true;
	CLock							CReadFileCallBackData::ms_Lock;	
#endif

	wstring CPkgFile::PkgFileMapping( const wstring& old_name )
	{
		if ( ms_mapPkgName.find(old_name) != CPkgFile::ms_mapPkgName.end() )
			return CPkgFile::ms_mapPkgName[old_name];
		else
			return old_name;
	}

	CPathMgr CPkgFile::CreatePathMgr(const wchar_t* Path)
	{
		if ( Path == NULL )
		{
			const int BufSize = 2048;
			wchar_t CurrentPath[BufSize];

#ifdef _WIN32
			_wgetcwd(CurrentPath, _countof(CurrentPath));
#else
			char szBufTmp[MAX_PATH];
			getcwd(szBufTmp, sizeof(szBufTmp));
			wcscpy(CurrentPath, utf8_to_utf16(szBufTmp).c_str());
#endif

			return CPathMgr(CurrentPath);
		}
		else
		{
			return CPathMgr(Path);
		}
	}

	int CPkgFile::LoadAndOpenPkg( CPackage** ppPkg, const wchar_t* pkg_name, const wchar_t* szAlias, const char* file_name, uint32& uPos, uint32& uLength, bool& bXOR, uint32& uCheckCode )
	{
		wstring szPkgName(pkg_name);
		szPkgName += L".pkg";

		SearchPathList	pathList		= GetSearchList(szAlias);
		int				ret				= eFE_SUCCESS;

		for ( SearchPathList::iterator it = pathList.begin(); it != pathList.end(); ++it )
		{
			wstring sLoadPath = *it;
			sLoadPath.append(szPkgName);
			ret = CPackage::Load( ppPkg, sLoadPath.c_str() );
			if ( ret == eFE_SUCCESS )
				break;
		}
		
		if ( ret != eFE_SUCCESS )
		{
			*ppPkg = 0;
			return ret;
		}

		if( !(*ppPkg)->Open( file_name, uPos, uLength, bXOR, uCheckCode ) ) // 打开所要文件
		{
			*ppPkg = 0;
			return eFE_PKG_SUB_FILE_OPEN_FAILED;
		}

		return ret;
	}

	CPkgFile::SearchPathList CPkgFile::GetSearchList( const wchar_t* alias )
	{
		if ( alias == 0 || wstring(alias) == L"" || ms_mapPathAlias.find(alias) == ms_mapPathAlias.end() )
			return ms_mapPathAlias[PATH_ALIAS_DEFAULT];
		
		SearchPathList ret = ms_mapPathAlias[alias];
		ret.insert(ret.end(), ms_mapPathAlias[PATH_ALIAS_DEFAULT].begin(), ms_mapPathAlias[PATH_ALIAS_DEFAULT].end());

		return ret;
	}

	const string CPkgFile::GetCurPkgName( const char* szFileName )
	{
		string sFullName( szFileName );
		string sPkgName;
		string sFileName;
		string::size_type pos;
		pos = sFullName.find_first_of("\\/", 0);
		if( (pos == string::npos) || (pos + 2 >= sFullName.size()) )
			return szFileName;

		sPkgName  = sFullName.substr(0,pos);
		for (string::iterator it = sPkgName.begin(); it != sPkgName.end(); ++it)
		{
			if (*it >= 'A' && *it<='Z')
				*it += 'a' - 'A';
		}

		wstring wcsPkgName = Utf8ToUCS2(sPkgName);
		const wchar_t* wszPkgName = GetPkgName(wcsPkgName.c_str());
		if ( wszPkgName == NULL )
			wszPkgName = GetPkgPreName(wcsPkgName.c_str());
		if ( wszPkgName == NULL )
			return szFileName;
	
		return utf16_to_utf8(wszPkgName).c_str();
	}

	int32 CPkgFile::GetCheckCode() const
	{
		return m_uCheckCode;
	}

	void CPkgFile::SetCheckCode( uint32 uCheckCode )
	{
		if ( m_uCheckCode != 0 )
			return;

		m_uCheckCode = uCheckCode;
	}
}

// 静态成员初声明

CPkgFile::PathAliasMap			CPkgFile::ms_mapPathAlias;
CPkgFile::PkgFileNameMap		CPkgFile::ms_mapPkgName;
CPkgFile::PkgFileNameMap		CPkgFile::ms_mapPkgPreName;
CPkgFile::MapLoadPath_t			CPkgFile::ms_mapLoadPath;
const wstring CPkgFile::PATH_ALIAS_DEFAULT = L"def";

CPkgFile::CPkgFile()
	: m_hFile(0)
	, m_uOffset(0)
	, m_uPosition(0)
	, m_uLength(0)
	, m_uCheckCode(0)
	, m_bXor(false)
{
	InitData();
}

CPkgFile::CPkgFile(const wchar_t* szAliasPath, const char* szFileName, bool bBinary)
: m_hFile(0)
, m_uOffset(0)
, m_uPosition(0)
, m_uLength(0)
, m_uCheckCode(0)
, m_bXor(false)
{
	InitData();
	Open(szAliasPath, szFileName, bBinary);
}

CPkgFile::~CPkgFile()
{
	Close();
}


void CPkgFile::InitData()
{
	PathAliasMap& mapPathAlias = GetPathAliasMap();
	if (mapPathAlias.empty())
	{
		const int BufSize = 2048;
		
		wchar_t CurrentPath[BufSize];
#ifdef _WIN32
		_wgetcwd(CurrentPath, _countof(CurrentPath));
#else
		char szBufTmp[MAX_PATH];
		getcwd(szBufTmp, sizeof(szBufTmp));
		wcscpy(CurrentPath, utf8_to_utf16(szBufTmp).c_str());
#endif
	
		mapPathAlias.insert( make_pair(PATH_ALIAS_DEFAULT.c_str(), SearchPathList()) );

		wstring szCurPath(CurrentPath);
		szCurPath.append(L"/");
		mapPathAlias[PATH_ALIAS_DEFAULT].push_back(szCurPath.c_str());
	}
}

void CPkgFile::InitTLSKey()
{
	CPackage::InitTLSKey();
}

void CPkgFile::UnitTLSKey()
{
	CPackage::UnitTLSKey();
}

void CPkgFile::InitTLSData()
{
	CPackage::InitTLSData();
}

void CPkgFile::UnitTLSData()
{
	CPackage::UnitTLSData();
}

CPkgFile::PathAliasMap& CPkgFile::GetPathAliasMap()
{
	return ms_mapPathAlias;
}

void CPkgFile::ClearSearchPath()
{
	ms_mapPathAlias.clear();
}

void CPkgFile::AddMapPkgName(const wchar_t* key, const wchar_t* value)
{
	CPkgFile::ms_mapPkgName.insert(make_pair(key, value));
}

const wchar_t* CPkgFile::GetPkgName(const wchar_t* key)
{
	if (!key)
	{
		return NULL;
	}

	PkgFileNameMap::iterator iter = ms_mapPkgName.find(key);
	if (iter != ms_mapPkgName.end())
	{
		return iter->second.c_str();
	}

	return NULL;
}

void CPkgFile::AddMapPkgPreName(const wchar_t* key, const wchar_t* value)
{
	CPkgFile::ms_mapPkgPreName.insert(make_pair(key, value));
}

const wchar_t* CPkgFile::GetPkgPreName(const wchar_t* key)
{
	if (!key)
	{
		return NULL;
	}

	PkgFileNameMap::iterator iter = ms_mapPkgPreName.find(key);
	if (iter != ms_mapPkgPreName.end())
	{
		return iter->second.c_str();
	}

	return NULL;
}

void CPkgFile::AddLoadPath(const wchar_t* alias, const wchar_t* szPath)
{
	if ( alias == 0 || wstring(alias) == L"" )
		alias = PATH_ALIAS_DEFAULT.c_str();

	wstring sPathFile(szPath);
	sPathFile.append(L"/");
	ms_mapPathAlias[alias].push_back(sPathFile);
}

const wchar_t* CPkgFile::GetLoadPath(const wchar_t* alias)
{
	if ( alias == 0 || wstring(alias) == L"" )
		alias = PATH_ALIAS_DEFAULT.c_str();

	SearchPathList& pathList = ms_mapPathAlias[alias];
	return pathList.front().c_str();
}

void CPkgFile::Close()
{
	if (IsValid())
	{
		if( !IsInPackage() )
			CPackage::CloseFile( m_hFile );
		m_hFile = 0;
		m_uOffset = 0;
		m_uPosition = 0;
		m_uLength = 0;
		m_bXor = false;
	}
#ifdef _WIN32
	//CReadThread::Unit();
#endif
}

int32 CPkgFile::Seek( int pos, int origin )
{
	if (!IsValid())
		return -1;

	int32 nNewPos = 0;

	switch(origin)
	{
	case SEEK_SET:
		nNewPos = pos;
		break;
	case SEEK_END:
		nNewPos = m_uLength + pos;
		break;
	case SEEK_CUR:
		nNewPos = m_uPosition + pos;
		break;
	default:
		GenErr("Invalid seek origin.");
	}

	if(( nNewPos < 0 ) || ( nNewPos > static_cast<int32>(m_uLength) ))
		return -1;

	if( !IsInPackage() )
	{
		if( static_cast<int32>(m_uPosition) != nNewPos )
			CPackage::SeekFile( m_hFile, nNewPos + m_uOffset );
	}
	else 
	{
		CPackage* pPackage = (CPackage*)m_hFile;
		if( static_cast<int32>(m_uPosition) != nNewPos || pPackage->GetPreUseFile() != this )
			CPackage::SeekFile( m_bBin ? pPackage->GetBinFile() : pPackage->GetFile(), nNewPos + m_uOffset );
		pPackage->SetPreUseFile( this );
	}
	m_uPosition = nNewPos;
	return 0;
}


int32 CPkgFile::read(void *buffer, size_t len)
{
	return Read(buffer,len);
}


int32 CPkgFile::Read(void *buffer, size_t len)
{
	if (!IsValid())
		return -1;

	uint32 uNewPos =m_uPosition + len;
	if ( uNewPos > m_uLength )
		uNewPos = m_uLength;
	if(uNewPos < m_uPosition)
		return -1;

	void* pFile = m_hFile;
	if( IsInPackage() )
	{
		CPackage* pPackage = (CPackage*)m_hFile;
		pFile = m_bBin ? pPackage->GetBinFile() : pPackage->GetFile();
		if( pPackage->GetPreUseFile() != this )
			Seek( m_uPosition );
	}
	len = uNewPos - m_uPosition;
	uint32 uBytes = CPackage::ReadFile( pFile, buffer, len );
	if( m_bXor )
		BufferXor(static_cast<char *>(buffer),m_uPosition,uBytes);
	m_uPosition = uNewPos;
	return uBytes;
}

int32 CPkgFile::Open(const wchar_t* szAliasPath, const char *szFileName, bool bBinary, ReadOrder eReadFirst)
{
	Close();

	// 判断读取的是否是绝对路径
#ifdef _WIN32
	if ( 0 != szFileName && szFileName[1] == ':')
#else
	if ( 0 != szFileName && szFileName[0] == '/')
#endif
	{
		CPackage::OpenFile( &m_hFile, gbk_to_utf16(szFileName).c_str(), bBinary );

		if( m_hFile )
		{
			m_uOffset = 0;
			m_bXor = 0;
			m_uLength = CPackage::FileSize( m_hFile );
			return eFE_SUCCESS;
		}
	}

	m_bBin		= bBinary;
	m_uPosition = 0;

	bool bReadPackOnly = false;
#ifdef _WIN32
	switch (eReadFirst)
	{
	case eRO_Pack:
		bReadPackOnly = true;
		break;
	case eRO_File:
		bReadPackOnly = false;
		break;
	default :
		bReadPackOnly = ReadPkgOnlyEnabled();
	}
#endif

	int ret;
	if(bReadPackOnly)
	{
		ret = GetPackInfo( (CPackage**)(&m_hFile), szAliasPath, szFileName, m_uOffset, m_uLength, m_bXor, m_uCheckCode );
		if( ret == eFE_SUCCESS )
		{
			((CPackage*)m_hFile)->SetPreUseFile( NULL );
			Seek( 0 );
			
		}
		return ret;
	}
	else
	{
		// 先读包外，再读包内

		// 在此进行根路径拼接
		wchar_t buf[1024];

#ifdef _WIN32
		_wgetcwd(buf, _countof(buf));
#else
		char szBufTmp[MAX_PATH];
		getcwd(szBufTmp, sizeof(szBufTmp));
		wcscpy(buf, utf8_to_utf16(szBufTmp).c_str());
#endif

		if ( szAliasPath == 0 || wstring(szAliasPath) == L"" )
		{
			szAliasPath = PATH_ALIAS_DEFAULT.c_str();
		}

		SearchPathList	pathList  = GetSearchList(szAliasPath);
		wstring			sLoadPath;
		for ( SearchPathList::iterator it = pathList.begin(); it != pathList.end(); ++it )
		{
			sLoadPath = *it + gbk_to_utf16(szFileName);
			CPackage::OpenFile( &m_hFile, sLoadPath.c_str(), bBinary );

			if( m_hFile )
				break;
		}
		
		if( m_hFile )
		{
			m_uOffset = 0;
			m_bXor = 0;
			m_uLength = CPackage::FileSize( m_hFile );
			return eFE_SUCCESS;
		}
		else
		{
			ret = GetPackInfo( (CPackage**)(&m_hFile), szAliasPath, szFileName, m_uOffset, m_uLength, m_bXor, m_uCheckCode );
			if( ret != eFE_SUCCESS )
				return ret;

			((CPackage*)m_hFile)->SetPreUseFile( NULL );
			Seek( 0 );
			return eFE_SUCCESS;
		}
	}
}

int CPkgFile::GetPackInfo(CPackage** ppPkg, const wchar_t* szAliasPath, const char *szFileName, uint32& uPos, uint32& uLength, bool& bXOR, uint32& uCheckCode )
{
	*ppPkg = 0;

	int32 nBegin = 0;
	size_t i = 0;
	const unsigned int BUFFER_SIZE = 4096;
	char szFoundName[BUFFER_SIZE];
	memset(szFoundName, 0, sizeof(szFoundName));
	size_t Len = strlen(szFileName);
	if (Len > BUFFER_SIZE)
	{
		stringstream ss;
		string strFileName(szFileName);
		ss << "file path is max then buffer size. " << " Len(" << strFileName.substr(0, 255) << "..." << strFileName.substr(strFileName.size()-255)<< ")=" << Len; 
		GenErr(ss.str().c_str());
	}
#ifdef _WIN32
	strcpy_s(szFoundName, BUFFER_SIZE, szFileName);
#else
	strcpy(szFoundName, szFileName);
#endif

	char* pFoundName = szFoundName;

	// 将文件名中的 "\" 全部改为 "/"
	for (i=0; i<Len; ++i)
	{
		if (pFoundName[i] == '\\')
			pFoundName[i] = '/';
	}

	// 过滤文件名中重复的 "/" 符号
	size_t insertPos = 0;
	bool flag = false;;
	for (i=0; i<Len; ++i)
	{
		if ( pFoundName[i] == '/' )
		{
			if ( flag )
				continue;
			else
				flag = true;
		}
		else
		{
			flag = false;
		}

		pFoundName[insertPos] = pFoundName[i];
		++insertPos;
	}

	if (insertPos < Len)
		pFoundName[insertPos] = '\0';

	// ./ 和没有./都表示当前路径，因为从包里读必须没有./，所有要把./去掉
	// 一直把./或者../都过滤掉
	for(;;)
	{
		if( pFoundName[0] == '.' && pFoundName[1] == '/' )
		{
			nBegin += 2;
			pFoundName += 2;
		}
		else if( pFoundName[0] == '.' && szFileName[1] == '.'&&  pFoundName[2] == '/' )
		{
			nBegin += 3;
			pFoundName += 3;
		}
		else
			break;
	}

	string sFullName( pFoundName );
	string sPkgName;
	string sFileName;
	string::size_type pos;
	pos = sFullName.find_first_of("\\/", 0);
	if( (pos == string::npos) || (pos + 2 >= sFullName.size()) )
		return eFE_NOT_A_PACKAGE;

	sPkgName  = sFullName.substr(0,pos);
	for (string::iterator it = sPkgName.begin(); it != sPkgName.end(); ++it)
	{
		if (*it >= 'A' && *it<='Z')
			*it += 'a' - 'A';
	}

	int ret = 0;
	wstring wcsPkgName = Utf8ToUCS2(sPkgName);
	if ( ms_mapPkgPreName.find(wcsPkgName) != ms_mapPkgPreName.end() )
	{
		wstring		new_pkg_name = ms_mapPkgPreName[wcsPkgName];
		
		ret = LoadAndOpenPkg(ppPkg, new_pkg_name.c_str(), szAliasPath, sFullName.c_str(), uPos, uLength, bXOR, uCheckCode) ;
		if ( ret == eFE_SUCCESS )
			return ret;
	}

	if (CPkgFile::ms_mapPkgName.find(wcsPkgName) != CPkgFile::ms_mapPkgName.end())
		wcsPkgName = CPkgFile::ms_mapPkgName[wcsPkgName];

	sFileName = sFullName.substr( pos + 1 );
	ret = LoadAndOpenPkg(ppPkg, wcsPkgName.c_str(), szAliasPath, sFileName.c_str(), uPos, uLength, bXOR, uCheckCode);
	return ret;
}

#ifdef _WIN32
bool CPkgFile::IsEnableAsyncRead()
{
	return CReadFileCallBackData::ms_bEnableAsyncRead&CReadFileCallBackData::ms_bGlobeEnableAsyncRead;
}

void CPkgFile::ReadThreadInit(void)
{
	CReadThread::Init();
}

void CPkgFile::ReadThreadUnit(void)
{
	Flush( static_cast<uint64>(-1) );
	CReadThread::Unit();
}

void CPkgFile::EnableAsyncRead( bool bEnable )
{
	//这个函数有逻辑错误
	if(CReadFileCallBackData::ms_bEnableAsyncRead == bEnable)
		return;

	CReadFileCallBackData::ms_bEnableAsyncRead = bEnable;
	//if(!bEnable)
	//{
		//CReadThread::Unit();
	//}
	//if(bEnable)
	//{
	//	CReadThread::Init();
	//}
}

void CPkgFile::EnableGlobeAsyncRead(bool bGlobeEnalbe)
{
	CReadFileCallBackData::ms_bGlobeEnableAsyncRead = bGlobeEnalbe;
	EnableAsyncRead(bGlobeEnalbe);
}


bool CPkgFile::Flush(uint64 uElapseTime)
{
	return CReadFileCallBackData::Flush(uElapseTime);
}

bool CPkgFile::ReadEmpty(void)
{
	return CReadFileCallBackData::ms_listLoaded.empty();
}

void CPkgFile::EnableAsyncReadDoFlush( bool bEnable )//多用于首次进入某个场景使用
{
	if(CReadFileCallBackData::ms_bEnableAsyncRead == bEnable)
		return;

	CReadFileCallBackData::ms_bEnableAsyncRead = bEnable;
	if(!bEnable)
	{
		Flush(-1);
		//CReadThread::Unit();
	}
	//else
	//{
	//	CReadThread::Init();
	//}
}

int32 CPkgFile::ReadWholeFile( const wchar_t* szAliasPath, const char *szFileName, void* pContext, ASYN_CALLBACK pFun )
{
	if( IsEnableAsyncRead() )
	{
		bool bXOR;
		HANDLE hFile;
		uint32 uPos, uLength;
		CReadFileCallBackData * pCallBack;

		int ret = 0;
		bool bReadSuc = false;
		if(ReadPkgOnlyEnabled())
		{
			uint32		uCheckCode	= 0;
			CPackage*	pPackage	= 0;
			ret = GetPackInfo( &pPackage, szAliasPath, szFileName, uPos, uLength, bXOR, uCheckCode );
			if(ret != eFE_SUCCESS)
				return -1;

			hFile = pPackage->GetAsynFile();
			if( hFile == INVALID_HANDLE_VALUE )		
				return -1;

			pCallBack = new CReadFileCallBackData(pFun, pContext, uLength, hFile,uPos,bXOR, uCheckCode, szFileName);
			bReadSuc = true;
		}
		else
		{
			// 先读包外，再读包内

			wstring wcs_buf(L"");
			if ( false == gbk_to_utf16(wcs_buf, szFileName) )
			{			
				LogErr("字符串转换出错 - CPkgFile::ReadWholeFile() wcs_buf", szFileName);
			}

			SearchPathList	pathList  = GetSearchList(szAliasPath);
			wstring			sLoadPath;
			for ( SearchPathList::iterator it = pathList.begin(); it != pathList.end(); ++it )
			{
				sLoadPath = *it + wcs_buf;
				hFile = CreateFileW( sLoadPath.c_str(), GENERIC_READ, FILE_SHARE_READ,NULL,OPEN_EXISTING, 0, NULL );//不打包这个地方会造成卡顿,因为文件数量比较多,每个文件都要createfile一次,打包完之后就没这个问题
				if( INVALID_HANDLE_VALUE != hFile )
					break;
			}
			
			if( hFile != INVALID_HANDLE_VALUE )		
			{
				uLength = GetFileSize (hFile, NULL) ; 
				if( uLength == 0xFFFFFFFF ) 
				{
					bReadSuc = false;
				}
				else
				{
					bReadSuc = true;
				}
			}

			if ( bReadSuc )
			{
				pCallBack = new CReadFileCallBackData(pFun, pContext, uLength, hFile, 0, false, 0, szFileName);
			}
			else
			{
				uint32		uCheckCode	= 0;
				CPackage*	pPackage	= 0;
				ret = GetPackInfo( &pPackage, szAliasPath, szFileName, uPos, uLength, bXOR, uCheckCode );
				if( pPackage == NULL )
				{
					return -1;
				}

				hFile = pPackage->GetAsynFile();
				if( hFile == INVALID_HANDLE_VALUE )
				{
					return -1;
				}

				bReadSuc = true;
				pCallBack = new CReadFileCallBackData(pFun, pContext, uLength, hFile, uPos, bXOR, uCheckCode, szFileName);
			}
		}

		if(bReadSuc)
		{
			CReadThread::Inst()->Read( hFile,const_cast<char*>(pCallBack->GetBuff()),(uint32)pCallBack->GetBuffSize(),
				pCallBack->GetOverLapped(),&CReadFileCallBackData::FileIOCompletionRoutine );
			CReadFileCallBackData::ms_nLoading++;
			return uLength;

		}
		else
			return -1;
	}
	else
	{
		CPkgFile PkgFile;
		if( PkgFile.Open( szAliasPath, szFileName ) != eFE_SUCCESS )
			return -1;

		char* pBuf = new char[PkgFile.Size()];
		PkgFile.read( pBuf, PkgFile.Size() );
		CRefBufFile* pBufFile = new CRefBufFile( pBuf, PkgFile.Size(), PkgFile.Offset(), PkgFile.GetCheckCode() );
		pBufFile->SetFileName(szFileName);
		pBufFile->AddRef();
		pFun( pContext, *pBufFile );
		pBufFile->Release();
		return PkgFile.Size();
	}
}

std::wstring CPkgFile::FindPathInclude( const wstring& path )
{
	PathAliasMap&			mapPathAlias	= GetPathAliasMap();
	PathAliasMap::iterator	ait				= mapPathAlias.begin();

	for ( ; ait != mapPathAlias.end(); ++ait )
	{
		SearchPathList& pathList = (*ait).second;
		for ( SearchPathList::iterator it = pathList.begin(); it != pathList.end(); ++it )
		{
			wstring cur_path = *it;
			if ( cur_path.find(path) != cur_path.npos )
				return cur_path;
		}
	}

	return L"";
}

#endif
