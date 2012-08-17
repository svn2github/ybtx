/*
*	CPathMgr.h	文件路径管理器，管理资源路径操作
*/

#ifndef _VFS_CPATHMGR_H_
#define _VFS_CPATHMGR_H_


#ifndef _WIN32
#include <ftw.h>
#else
#include <Windows.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif


#define  PATHLEN  1024

namespace sqr
{
	enum FTW_FLAG
	{
#ifndef _WIN32
		_FTW_FILE = FTW_F,
		_FTW_DIR = FTW_D,
		_FTW_SL = FTW_SL,
		_FTW_NS = FTW_NS,
		_FTW_TMP,
#else
		_FTW_FILE = FILE_ATTRIBUTE_NORMAL,
		_FTW_DIR = FILE_ATTRIBUTE_DIRECTORY,
		_FTW_TMP = FILE_ATTRIBUTE_TEMPORARY,
		_FTW_SL,
		_FTW_NS,

#endif
	};

	enum FTW_RESULT
	{
		_FTW_STOP,
		_FTW_IGNORE,
		_FTW_CONTINUNE,
	};

#ifndef _WIN32
#	define _stat stat
#endif

	typedef FTW_RESULT(* FILE_PROC)( const wchar_t*, const struct _stat *, FTW_FLAG, void* );

	//严重注意，这个函数不会搜索任何名为".svn"目录的内容
	int32		FileTreeWalk(const wchar_t* szDir, FILE_PROC pfnFileProc, void* pContext );
	bool		MakeDirectory( const wchar_t* szDirectory );
	bool		MakeDirectoryUTF8(const char * szDirectory);

	class CPathMgr
	{
	public:
		CPathMgr(const char* szPathName);
		CPathMgr(const wchar_t* szPathName = NULL);
		virtual ~CPathMgr();

		// 取得本程序的根路径
		const wchar_t*	GetRootPath() const;
		
		// 改变程序的根路径
		void			ChangeRootPath(const wchar_t* RootPath);
		
		// 得到当前路径
		const wchar_t*	GetCurPath() const;
		
		// 设置当前路径,传入的是相对于根目录的相对路径
		void			SetCurPath(const wchar_t* szPathName=NULL);

		//该函数会修改内部的CurPath变量。
		const wchar_t* GetFullPath(const wchar_t* szPath);

	private:
		void			initialize(const wchar_t* szPathName);

	protected:
		wchar_t  m_szRootPath[PATHLEN];  // 启动路径
		wchar_t  m_szCurrPath[PATHLEN];  // 当前路径
	}; // End of class CPathMgr


}// End of namespace Vfs

#endif // End of #define _VFS_FILEPATHMGR_H_

