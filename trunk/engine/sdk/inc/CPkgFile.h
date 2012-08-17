#pragma once

#include "CommonDefs.h"
#include "BaseTypes.h"
#include "ExpHelper.h"
#include <list>
#include <map>
#include <vector>
#include "ThreadTypes.h"
#include "PkgCommon.h"
using namespace std;


namespace sqr
{
	enum ReadOrder
	{
		eRO_Normal,
		eRO_Pack,
		eRO_File,
	};

	class CBufFile;
	class CRefBufFile;
	class CPathMgr;
	class CPackage;

#ifdef _WIN32
	typedef void (*ASYN_CALLBACK)( void*, CRefBufFile& BufFile );
#endif

	class COMMON_API CPkgFile
	{
	public:
		typedef list<wstring>					SearchPathList;	
		typedef map<wstring, SearchPathList>	PathAliasMap;

	public:
		CPkgFile();
		CPkgFile(const wchar_t* szAlias, const char* szFileName, bool bBinary = true);

		~CPkgFile();

		//如果我们要在多线程中去进行包的操作，那么得现在主线程中把tls的key初始化
		static void			
			InitTLSKey();

		static void			
			UnitTLSKey();

		//如果多线程需要进行包的操作，那么以下两个函数在多线程启动和结束的时候调用
		static void			
			InitTLSData(); 

		static void			
			UnitTLSData();

		static void			
			ClearSearchPath();				// 清空搜索路径

		static void			
			AddMapPkgName(const wchar_t* key, const wchar_t* value); // 载入包名映射表

		static void			
			AddMapPkgPreName(const wchar_t* key, const wchar_t* value);

		static const wchar_t* 
			GetPkgName(const wchar_t* key);

		static const wchar_t* 
			GetPkgPreName(const wchar_t* key);

		static void				
			AddLoadPath(const wchar_t* alias, const wchar_t* szPath);

		static const wchar_t*	
			GetLoadPath(const wchar_t* alias);

		int32	Open(const wchar_t* szAliasPath, const char *szFileName, bool bBinary = true, ReadOrder eReadFirst = eRO_Normal);
		void	Close();
		int32	Seek(int pos,int origin=SEEK_SET);
		int32	Tell() const
		{
			return m_uPosition;
		}
		int32	Read(void *buffer,size_t len);
		int32	read(void *buffer,size_t len);

		const string
				GetCurPkgName( const char* szFileName );

		bool	IsInPackage()const
		{
			return m_uOffset > 0;
		}

		bool	IsValid()const
		{
			return m_hFile != NULL;
		}

		int32	Size()const
		{
			return m_uLength;
		}

		int32	Offset()const
		{
			return m_uOffset;
		}

		int32	GetCheckCode() const;
		void	SetCheckCode(uint32 uCheckCode);

		operator bool () const
		{
			return IsValid();
		}

		static int
				GetPackInfo(CPackage** pPkg, const wchar_t* szAliasPath, const char *szFileName, uint32& uPos, uint32& uLength, bool& bXOR, uint32& uCheckCode );

#ifdef _WIN32
		friend class	CReadFileCallBackData;
		static bool		IsEnableAsyncRead();
		static void		EnableAsyncRead( bool bEnable );
		static void		EnableAsyncReadDoFlush(bool bEnable);
		static void		EnableGlobeAsyncRead(bool bGlobeEnalbe);
		static bool		Flush( uint64 uElapseTime );
		static bool		ReadEmpty(void);
		static int32	ReadWholeFile( const wchar_t* szAliasPath, const char *szFileName, void* pContext, ASYN_CALLBACK pFun );
		static void		ReadThreadInit(void);
		static void     ReadThreadUnit(void);
#endif

		static wstring				
			FindPathInclude(const wstring& path);

		static wstring				
			PkgFileMapping(const wstring& old_name);

		static PathAliasMap&		
			GetPathAliasMap();

		static SearchPathList		
			GetSearchList(const wchar_t* alias);

		static const wstring PATH_ALIAS_DEFAULT;

	private:
		CPkgFile(const CPkgFile &);
		CPkgFile& operator = (const CPkgFile &);
		void	InitData();
		
		static CPathMgr				
			CreatePathMgr(const wchar_t* Path);

		static int
			LoadAndOpenPkg( CPackage** ppPkg, const wchar_t* pkg_name, const wchar_t* szAlias, const char* file_name, uint32& uPos, uint32& uLength, bool& bXOR, uint32& uCheckCode );

	// ---- data member ---------------------------------------------------------------------------
		
	private:
		typedef map<wstring, wstring>	PkgFileNameMap;
		typedef map<wstring, wstring>	MapLoadPath_t;
		
		void*						m_hFile;
		uint32						m_uOffset;
		uint32						m_uPosition;
		uint32						m_uLength;
		uint32						m_uCheckCode;
		bool						m_bXor;
		bool						m_bBin;
		
		static PkgFileNameMap		ms_mapPkgName;
		static PkgFileNameMap		ms_mapPkgPreName;
		static PathAliasMap			ms_mapPathAlias;
		static MapLoadPath_t		ms_mapLoadPath;	
	};

}

