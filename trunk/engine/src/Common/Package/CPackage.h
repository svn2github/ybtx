#pragma once

#include <string>
#include "ThreadTypes.h"
#include "PkgCommon.h"

namespace sqr
{

	class CPackage
	{
	public:
		CPackage();
		~CPackage();	
		bool Open( const char* szFileName, uint32& uPos, uint32& uLength, bool& bXOR, uint32& uCheckCode );
		void* GetFile(){ return m_pFile; }
		void* GetBinFile(){ return m_pFileBin; }
		void* GetPreUseFile(){ return m_PreUseFile; }
		void  SetPreUseFile( void* pPreUser ){ m_PreUseFile = pPreUser; }
#ifdef _WIN32
		void* GetAsynFile(){ return	m_pAsSynFile; }
#endif
		static int Load( CPackage** ppPkg, const wchar_t *szName );
		static int OpenFile(void** ppFileOut, const wchar_t *szName, bool bBinary);
		static int32 ReadFile(void* hFile, void * pBuffer, size_t size);
		static uint32 FileSize(void* hFile);
		static void SeekFile(void* hFile,size_t pos);
		static void CloseFile(void* hFile);

		static void InitTLSKey();
		static void UnitTLSKey();

		static void InitTLSData();
		static void UnitTLSData();

	private:
		bool				m_bXor;
		map<uint32, PackFileIndex>	m_FileIndex;
		void*				m_pFile;
		void*				m_pFileBin;

#ifdef _WIN32
		void*				m_pAsSynFile;
#endif
		void*				m_PreUseFile;

		typedef std::map<int, CPackage>								MapPackage_t;
		static MapPackage_t& _CPackage_map();

		static MapPackage_t		ms_mapPackage;

		friend class map<int, CPackage>;

		static	HTLSKEY			ms_tlsPackageKey;
	};
}


