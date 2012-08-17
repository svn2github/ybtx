#pragma once
#include "BaseTypes.h"
#include <vector>

namespace sqr
{
	class CPkgDir
	{
	public:
		CPkgDir();
		CPkgDir(const wchar_t* alias, const char* szDirName,const char* Filter = "");
		~CPkgDir();

		uint32 Open(const wchar_t* alias, const char *szDirName,const char* Filter = "");

		uint32			GetFileNum(void);
		void			GetFileNameW(uint32 Index , wstring& str);
		void			GetFileName(uint32 Index,   string& str);

		void			GetFullFileNameW(uint32 Index, wstring& str);
		void			GetFullFileName(uint32 Index,  string& str);

		void		Register(const string& filename,const string& filter);
	protected:
		typedef vector<string>	FileList;
		FileList	m_lstFileName;
		string		m_strDirPath;
#if _DEBUG
		string		m_strFilter;
#endif
	};

	inline uint32 
		CPkgDir::GetFileNum(void)
	{
		return (uint32)m_lstFileName.size();
	}
}
