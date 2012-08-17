#include "stdafx.h"
#include "CLanguage.h"
#include "CGraphicExp.h"
#include "CPkgFile.h"
#include "TNedAllocator.inl"

CLanguage::CLanguage(const GString &szLanName)
	: m_szLanName(szLanName)
{
}

CLanguage::~CLanguage()
{
}

const GWString& CLanguage::GetGDIFontName(int32 nID)
{
	Ast(nID >=0 && nID < (int32)m_GDIFontInfo.size());
	return m_GDIFontInfo[nID].szName;
}

const GWString& CLanguage::GetGDIDefFontName()
{
	return m_defFontName;
}

int32 sqr::CLanguage::GetGDIFontCount() const
{
	return m_GDIFontInfo.size();
}

const GString& CLanguage::GetDisFontFileName() const
{
	return m_szDisFontFile;
}

void CLanguage::AddGDIFontInfo(const GWString &szName, const GString &szFile)
{
	GDIFontInfo newGDIFontInfo(szName, szFile);
	m_GDIFontInfo.push_back(newGDIFontInfo);
}

void CLanguage::SetGDIDefFontName(const GWString &szName)
{
	m_defFontName = szName;
}

void CLanguage::SetDisFontFileName(const GString &szFile)
{
	m_szDisFontFile = szFile;
}

void CLanguage::SetLanFolder(const GString &szFolder)
{
	m_szLanFolder = szFolder;
}

void CLanguage::LoadGDIFonts()
{
	CPkgFile pkgfile;

	GDIFontInfoVector::iterator it = m_GDIFontInfo.begin();
	for ( ; it != m_GDIFontInfo.end(); ++it )
	{
		GString szCurFile = it->szFile;
		szCurFile.insert(0, "../gui/tgt/language/fonts/");
		
		// 打开并读取ttf字体字节流
		pkgfile.Open(szCurFile.c_str());
		Ast(pkgfile.IsValid());
		int32  size = pkgfile.Size();
		uint8 *pBuf = new uint8[size];
		Ast(pBuf);
		pkgfile.Read(pBuf, size);

		// 注册ttf字体到内存
		DWORD dummy = 0;
		HANDLE hFont = AddFontMemResourceEx(pBuf, size, 0, &dummy);
		Ast(hFont);
		m_hFontVec.push_back(hFont);

		delete pBuf;
		pBuf = 0;
		pkgfile.Close();
	}
}

void CLanguage::UnloadGdiFonts()
{
	Ast(!m_hFontVec.empty());

	FONTHandleVector::iterator it = m_hFontVec.begin();
	for ( ; it != m_hFontVec.end(); ++it )
	{
		Ast(RemoveFontMemResourceEx(*it));
	}
}

const GString& CLanguage::GetLanName() const
{
	return m_szLanName;
}

const GString& CLanguage::GetLanFolder() const
{
	return m_szLanFolder;
}
