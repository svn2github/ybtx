#include "stdafx.h"
#include "StaticText.h"
#include "CodeCvs.h"

CStaticText::CStaticText( const char* szCurLanTxtFileName, const char* szOriTxtFileName ) //m_szTextFileName("../../lang/StaticText.txt")
{
	m_szTxtFileName = new char[strlen(szOriTxtFileName) + 1];
	strcpy(m_szTxtFileName, szOriTxtFileName);
	if(strcmp(szCurLanTxtFileName, szOriTxtFileName) == 0)
	{
		m_szCurLanTxtFileName = NULL;
	}
	else
	{
		m_szCurLanTxtFileName = new char[strlen(szCurLanTxtFileName) + 1];
		strcpy(m_szCurLanTxtFileName, szCurLanTxtFileName);
	}
	InitHashmap();
}

CStaticText::~CStaticText()
{
	typedef hash_map<int, const wchar_t*>::iterator IterHmap;
	for (IterHmap iter=m_hmapTransInfo.begin(); iter!=m_hmapTransInfo.end(); ++iter)
	{
		delete[] iter->second;
		iter->second = NULL;
	}

	for (IterHmap iter=m_hmapCurLanTransInfo.begin(); iter!=m_hmapCurLanTransInfo.end(); ++iter)
	{
		delete[] iter->second;
		iter->second = NULL;
	}

	delete[] m_szTxtFileName;
	m_szTxtFileName = NULL;

	delete[] m_szCurLanTxtFileName;
	m_szCurLanTxtFileName = NULL;
}

void CStaticText::InitHashmap()
{
	m_hmapCurLanTransInfo.clear();
	if (m_szCurLanTxtFileName && m_txtTabFile.Load(m_szCurLanTxtFileName))
	{
		int nRowNum = m_txtTabFile.GetHeight();
		for (int nRow=1; nRow<nRowNum; nRow++)
		{
			const char* szIndex = m_txtTabFile.GetString(nRow, 0);
			int nIndex = atoi(szIndex);
			const char* szContent = m_txtTabFile.GetString(nRow, 1);
			
			wstring strContent = utf8_to_utf16(szContent);
			wchar_t* szWideContent = new wchar_t[strContent.length() + 1];
			wcscpy(szWideContent, strContent.c_str());

			m_hmapCurLanTransInfo.insert(make_pair(nIndex, szWideContent));
		}
	}
	m_txtTabFile.Clear();

	if (m_txtTabFile.Load(m_szTxtFileName))
	{
		int nRowNum = m_txtTabFile.GetHeight();
		for (int nRow=1; nRow<nRowNum; nRow++)
		{
			const char* szIndex = m_txtTabFile.GetString(nRow, 0);
			int nIndex = atoi(szIndex);

			hash_map<int, const wchar_t*>::iterator itr = m_hmapCurLanTransInfo.find(nIndex);
			if(itr != m_hmapCurLanTransInfo.end())
			{
				m_hmapTransInfo.insert(make_pair(nIndex, itr->second));
			}
			else
			{
				const char* szContent = m_txtTabFile.GetString(nRow, 1);

				//wstring strContent = gbk_to_utf16(szContent);
				//wchar_t* szWideContent = new wchar_t[strContent.length() + 1];
				//wcscpy(szWideContent, strContent.c_str());

				wstring strContent = utf8_to_utf16(szContent);
				wchar_t* szWideContent = new wchar_t[strContent.length() + 1];
				wcscpy(szWideContent, strContent.c_str());

				//int uSize = int(strlen(szContent) + 1);
				//wchar_t* szWideContent = new wchar_t[uSize];
				//MultiByteToWideChar(CP_ACP, 0, szContent, -1, szWideContent, uSize);

				m_hmapTransInfo.insert(make_pair(nIndex, szWideContent));
			}
		}
	}
}

wstring CStaticText::GetText( int nIndex, ... )
{
	if (m_hmapTransInfo.find(nIndex) == m_hmapTransInfo.end())
	{
		return L"";
	}
	else
	{
		va_list vaArgs;
		va_start(vaArgs, nIndex);

		const wchar_t* szFormat = m_hmapTransInfo[nIndex];
		wchar_t* szTemp = new wchar_t[wcslen(szFormat) + 1];
		wcscpy(szTemp, szFormat);
		const wchar_t* p = wcschr(szTemp, L'%');
		wstring strRetValue = L"";
		wchar_t* szTempFormat = NULL;
		wchar_t szTempContent[256];

		while (p != NULL)
		{
			if ((*p < L'A' || *p > L'z') && *p != L'\0')
			{
				p++;
			}
			if (*p == L'\0')
			{
				break;
			}
			p++;
			int nChar = (int)(p - szTemp);
			szTempFormat = new wchar_t[nChar + 1];
			wcsncpy(szTempFormat, szTemp, nChar);
			szTempFormat[nChar] = L'\0';
			switch(szTempFormat[nChar - 1])
			{
			case L's':
				{
					const wchar_t* szValue = va_arg(vaArgs, const wchar_t*);
					wsprintf(szTempContent, szTempFormat, szValue);
					strRetValue.append(szTempContent);
					break;
				}
			case L'd':
				{
					int nValue = va_arg(vaArgs, int);
					wsprintf(szTempContent, szTempFormat, nValue);
					strRetValue.append(szTempContent);
					break;
				}
			case L'u':
				{
					unsigned long uValue = va_arg(vaArgs, unsigned long);
					wsprintf(szTempContent, szTempFormat, uValue);
					strRetValue.append(szTempContent);
					break;
				}
			case L'f':
				{
					double dValue = va_arg(vaArgs, double);
					wsprintf(szTempContent, szTempFormat, dValue);
					strRetValue.append(szTempContent);
					break;
				}
			default:
				break;
			}

			delete[] szTempFormat;
			szTempFormat = NULL;

			if (*p == L'\0')
			{
				break;
			}
			
			wcscpy(szTemp, p);
			p = wcschr(szTemp, L'%');
		}
		if (*szTemp != L'\0')
		{
			strRetValue.append(szTemp);
		}
		va_end(vaArgs);

		delete[] szTemp;
		szTemp = NULL;

		return strRetValue;
	}
}
