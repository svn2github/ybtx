#pragma once

#include <hash_map>
#include <cstdarg>
#include "TxtTableFile.h"

using namespace stdext;

class CStaticText
{
public:
	CStaticText( const char* szCurLanTxtFileName, const char* szOriTxtFileName );
	~CStaticText();
	wstring GetText( int nIndex , ... );

private:
	void InitHashmap();

private:
	char*						m_szTxtFileName;
	char*						m_szCurLanTxtFileName;
	hash_map<int, const wchar_t*>	m_hmapTransInfo;
	hash_map<int, const wchar_t*>	m_hmapCurLanTransInfo;
	CTxtTableFile					m_txtTabFile;
};




