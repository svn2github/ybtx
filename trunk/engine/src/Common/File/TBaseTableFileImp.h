#pragma once 
#include "CDataFileAllocator.h"
#include "CLock.h"
#include <map>

namespace sqr 
{
	class CVariant;
	class CPkgFile;
	class CError;

	template<typename T>
	class TBaseTableFileImp : public CDataFileMallocObject
	{
	public:
		TBaseTableFileImp(bool bFieldNameEnabled);
		~TBaseTableFileImp();

		void AddRef();
		void DelRef();

		static void Lock();
		static void UnLock();

		typedef vector<const char*, CDataFileAllocator<const char*> > VecFileName_t;
		static pair<T*, bool> GetImp(const wchar_t* szAliasPath, VecFileName_t& vecFileName);

		bool Load();

		bool Reload(VecFileName_t& vecFileName);

		int32 GetHeight();
		int32 GetWidth();

		const char* GetString(int32 nRow, int32 nCol);
		const char* GetString(int32 nRow, const char* szColName);
		const char* GetString(int32 nOffsetIndex);

		void ClearData();

	protected:
		typedef vector<char, CDataFileAllocator<char> > VecBuf_t;
		typedef vector<int32, CDataFileAllocator<int32> >				VecOffset_t;


		struct str_less : public binary_function<const char*, const char*, bool>
		{
			bool operator()(const char* szLeft, const char* szRight) const 
			{
				return (strcmp(szLeft, szRight) < 0) ? true : false;
			}
		};

		typedef map<const char*, int32, str_less, 
			CDataFileAllocator<pair<const char*, int32> > >		MapName2Col_t;

		typedef basic_string<char, std::char_traits<char>, CDataFileAllocator<char> > String_t;
		typedef basic_string<wchar_t, std::char_traits<wchar_t>, CDataFileAllocator<wchar_t> > WString_t;

		//typedef map<String_t, int32, less<String_t>, 
		//	CDataFileAllocator<pair<String_t, int32> > >		MapName2Col_t;

		class TableFileData
		{
		public:
			VecBuf_t					vecBuf;
			VecOffset_t					vecOffset;
			MapName2Col_t				mapName2Col;
			int32						nWidth;
			int32						nHeight;
			bool						bFieldNameEnabled;  //第一行是否为列名
		};


		//for table file
		bool LoadTableFile(const wchar_t* szAliasPath, const char* szFileName, TableFileData& data);

		bool TestAndHandleFirstRow(const char* szFileName, const char* szName, int32 nCol, size_t stOffset, bool bFirst, TableFileData& data);
		//解析文件第一行
		bool ParseFirstRow(const char* szFileName, size_t& stNextPos, TableFileData& data);
		//解析剩余的文件
		bool ParseLeftRow(const char* szFileName, size_t stNextPos, TableFileData& data);

		//for xml file
		bool LoadXMLFileDerived(const wchar_t* szAliasPath, const char* szFileName);

		bool LoadDerived();

		bool ReloadDerived(VecFileName_t& vecFileName);

		void SetFileRowOffsetDerived(const char* szFileName);

		void OnExceptionDerived(CError& error);

		int32 GetCol(const char* szColName);
		const char * GetNamefromCol(int32 Col);

		void GenLoadError(const char* szFile, const string& szType);
		void GenLoadError(const char* szFile, const string& szType, const string& szMsg);

	protected:
		uint32						m_uRefCount;

#ifdef _WIN32
		struct string_less : public binary_function<String_t, String_t, bool>
		{
			bool operator()(const String_t& szLeft, const String_t& szRight) const 
			{
				return szLeft < szRight;
			}
		};

		typedef hash_map<String_t, T*, hash_compare<String_t, string_less>, 
			CDataFileAllocator<pair<String_t, T*> > >	MapKey2Imp_t;

		typedef hash_set<String_t, hash_compare<String_t, string_less>,
			CDataFileAllocator<String_t> > SetFileName_t;
#else
		struct string_hash     
		{                                                                                                                            
			size_t operator()(const String_t& str) const                                                                               	{                                                                                                                        
				return __stl_hash_string(str.c_str());                                                                               
			}                                                                                                                        
		};    

		struct string_equal
		{
			bool operator()(const String_t& szLeft, const String_t& szRight) const
			{
				return szLeft == szRight;
			}
		};

		typedef hash_map<String_t, T*, string_hash, string_equal, 
			CDataFileAllocator<pair<String_t, T*> > > MapKey2Imp_t;

		typedef hash_set<String_t, string_hash, string_equal,
			CDataFileAllocator<String_t> >	SetFileName_t;
#endif

		static MapKey2Imp_t			ms_mapKey2Imp;

		String_t					m_sKey;

		WString_t					m_sAliasPath;
		SetFileName_t				m_setFileName;

		static CLock				ms_Lock;

		TableFileData				m_TableData;
	};

}
