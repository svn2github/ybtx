#include "stdafx.h"
#include <vector>
#include "CodeCvs.h"

const int PATH_SIZE = 4096;

string utf16_to_utf8( const wstring& strSrc )
{
	string strTmp;
	const int nUtf8Len = WideCharToMultiByte(CP_UTF8, 0, strSrc.c_str(), static_cast<int>(strSrc.size()), NULL, 0, NULL, NULL);
	vector<char> vecTmp(nUtf8Len + 1);
	WideCharToMultiByte(CP_UTF8, 0, strSrc.c_str(), static_cast<int>(strSrc.size()), &vecTmp[0], nUtf8Len, NULL, NULL);
	strTmp.assign(vecTmp.begin(), vecTmp.end() - 1);
	return strTmp;
}

wstring utf8_to_utf16( const string& strSrc )
{
	wstring strTmp;
	const int nWcsLen = MultiByteToWideChar(CP_UTF8, 0, strSrc.c_str(), static_cast<int>(strSrc.size()), NULL, 0);
	vector<wchar_t> vecTmp(nWcsLen + 1);
	MultiByteToWideChar(CP_UTF8, 0, strSrc.c_str(), static_cast<int>(strSrc.size()), &vecTmp[0], nWcsLen);
	strTmp.assign(vecTmp.begin(), vecTmp.end() - 1);
	return strTmp;
}

wstring gbk_to_utf16( const string& strSrc )
{
	wstring strUnicode(strSrc.size(), 0);
	
	const string strOldLoc = setlocale(LC_CTYPE, NULL);
	setlocale(LC_CTYPE, ".936");
	size_t unicodeLength = mbstowcs(const_cast<wchar_t*>(strUnicode.data()), strSrc.c_str(), strSrc.size());
	setlocale(LC_CTYPE, strOldLoc.c_str());

	if (unicodeLength == -1)
		return L"___________mbcs to utf16 failed_______YBTXDlg____";

	strUnicode.resize(unicodeLength);
	return strUnicode;
}

void string_replace( wstring& strBig, const wstring& strSrc, const wstring& strDst )
{
	wstring::size_type pos=0;
	wstring::size_type srclen = strSrc.size();
	wstring::size_type dstlen = strDst.size();
	while( (pos=strBig.find(strSrc, pos)) != wstring::npos)
	{
		strBig.replace(pos, srclen, strDst);
		pos += dstlen;
	}
}

wstring GetSetting( const wchar_t* file, wchar_t* strName, wchar_t* strDefault, wchar_t* dst_path )
{
	int dwRet;
	wstring strRet;
	wstring strFileName(dst_path);

	wchar_t buf[1024];
	wstring ini_file(file);

	strFileName += ini_file;
	dwRet = GetPrivateProfileStringW(L"Setting", strName, strDefault, buf, sizeof(buf), strFileName.c_str());

	strRet = buf;
	return strRet;
}

void FixCurrentDir(wchar_t* CurPath)
{
	GetCurrentDirectory(PATH_SIZE, CurPath);
	size_t len = wcslen(CurPath);
	if(len<1) 
		return;
	if( CurPath[len-1] != L'\\')
	{
		CurPath[len] = L'\\';
	}
}

bool TryDeleteFile( const wchar_t* szFilePath, DWORD& dwResult )
{
	for (int i=0; i<1000; ++i)
	{
		if (DeleteFileW(szFilePath))
			return true;

		dwResult = GetLastError();

		switch(dwResult)
		{
		case ERROR_FILE_NOT_FOUND:
			return true;
		case ERROR_ACCESS_DENIED:
			break;
		default:
			return false;
		}

		Sleep(40);
	}

	return false;
}
