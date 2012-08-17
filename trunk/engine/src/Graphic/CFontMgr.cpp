#include "stdafx.h"
#include "CFontMgr.h"
#include "ErrLogHelper.h"
#include "CPkgFile.h"
#include "ExpHelper.h"
#include "TSqrAllocator.inl"
#include "GraphicBase.h"
#include "DebugHelper.h"

CFontMgr::CFontMgr()
	: m_nLangIndex(0)
{
}

void CFontMgr::AddFontInfo( const wchar_t* name, const char* file )
{
	if ( name == NULL || file == NULL )
		LogErr("CFontMgr::AddFontInfo - param name or file is null.");

	m_GDIFontInfoList.push_back(make_pair(name, file));
}


void sqr::CFontMgr::ClearFontInfo()
{
	m_GDIFontInfoList.clear();
}

bool CFontMgr::LoadGDIFonts()
{
	CPkgFile pkgfile;

	GDIFontInfoList::iterator it = m_GDIFontInfoList.begin();
	for ( ; it != m_GDIFontInfoList.end(); ++it )
	{
		GString szCurFile = it->second;
		GString szFontPath = m_szLangFolder + "/fonts/" + szCurFile;

		// 打开并读取ttf字体字节流
		if ( pkgfile.Open(PATH_ALIAS_LAN.c_str(), szFontPath.c_str()) != eFE_SUCCESS )
			return false;
		Ast(pkgfile.IsValid());
		int32  size = pkgfile.Size();
		uint8 *pBuf = new uint8[size];
		Ast(pBuf);
		pkgfile.Read(pBuf, size);

		// 注册ttf字体到内存
		DWORD dummy = 0;
		HANDLE hFont = AddFontMemResourceEx(pBuf, size, 0, &dummy);
		if ( hFont != 0 )
			m_hFontVec.push_back(hFont);
		else
		{
			stringstream ss;
			CError err("注册ttf字体到内存失败");
			ss << szFontPath.c_str() << " err code: " << GetLastError();
			GfkLogPkgErr(err, ss.str().c_str(), pkgfile);
		}

		delete pBuf;
		pBuf = 0;
		pkgfile.Close();
	}

	return true;
}

void CFontMgr::UnloadGdiFonts()
{
	FONTHandleList::iterator it = m_hFontVec.begin();
	for_each( m_hFontVec.begin(), m_hFontVec.end(), ptr_fun(RemoveFontMemResourceEx) );
}

bool CFontMgr::SetLangIndex( int32 lang_index )
{
	if ( m_nLangIndex == lang_index )
		return false;

	m_nLangIndex = lang_index;
	return true;
}

int32 CFontMgr::GetLangIndex() const
{
	return m_nLangIndex;
}

bool CFontMgr::SetOriLangFolder(const char* lang_folder)
{
	Ast(lang_folder != NULL);
	if ( m_szOriLangFolder == lang_folder )
		return false;

	m_szOriLangFolder = lang_folder;
	return true;
}

bool CFontMgr::SetLangFolder(const char* lang_folder)
{
	Ast(lang_folder != NULL);
	if ( m_szLangFolder == lang_folder )
		return false;
	
	m_szLangFolder = lang_folder;
	return true;
}

const char* CFontMgr::GetOriLangFolder()
{
	return m_szOriLangFolder.c_str();
}


const char* CFontMgr::GetLangFolder()
{
	return m_szLangFolder.c_str();
}

void CFontMgr::SetDisFontName( const char* name )
{
	m_szDisFontName = name;
}

const char* CFontMgr::GetDisFontName()
{
	return m_szDisFontName.c_str();
}

const wchar_t* CFontMgr::GetGDIFontName( size_t nID )
{
	Ast(nID>=0 && nID<m_GDIFontInfoList.size())
	
	return m_GDIFontInfoList[nID].first.c_str();
}

void CFontMgr::SetGDIDefFontName(const wchar_t* name)
{
	m_szDefFontName = name;
}

const wchar_t* CFontMgr::GetGDIDefFontName()
{
	return m_szDefFontName.c_str();
}
