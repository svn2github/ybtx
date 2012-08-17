#include "stdafx.h"
#include "CrcCheck.h"
#include "CrcAlgorithm.h"

using namespace sqr;

#define PATH_MAX 512

CCrcChecking::CCrcChecking(void) : m_szCrcFileName("../../etc/gac/CrcCode.txt")
								 , m_nMaxLen(8192)
{
	ReadCrcCodeToMap();
}

CCrcChecking::~CCrcChecking(void)
{
}

void CCrcChecking::ReadCrcCodeToMap()
{
	FILE* pFile = NULL;
	fopen_s(&pFile, m_szCrcFileName, "rb");
	if (pFile)
	{
		char szLine[PATH_MAX];
		memset(szLine, '\0', sizeof(szLine));
		while (fgets(szLine, PATH_MAX, pFile) != NULL)
		{
			char* szCode = strchr(szLine, '\t');
			if (szCode != NULL)
			{
				int uPos = (int)(szCode - szLine);
				char* szFileName = new char[uPos + 1];
				strncpy_s(szFileName, uPos + 1, szLine, uPos);
				szFileName[uPos] = '\0';
				szCode++;
				m_hmapCrcCode.insert(make_pair(szFileName, szCode));
				delete[] szFileName;
				szFileName = NULL;
			}
		}
		fclose(pFile);
	}
}

bool CCrcChecking::Check()
{
	typedef hash_map<string, string>::iterator IterHmapSS;
	bool bSucc = true;
	for (IterHmapSS iter = m_hmapCrcCode.begin(); iter != m_hmapCrcCode.end(); ++iter)
	{
		string strFile = iter->first;
		if (!DoCheck(strFile.c_str()))
		{
			bSucc = false;
		}
	}
	return bSucc;
}

bool CCrcChecking::DoCheck( const string& strFileName )
{
	FILE* pFile = 0;
	fopen_s(&pFile, strFileName .c_str(), "rb");
	if (!pFile)
	{
		string strMsg = "无法打开文件：";
		strMsg.append(strFileName);
		cout << strMsg.c_str() << endl;
		return false;
	}
	fseek(pFile, 0, SEEK_END);
	uint uLength = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	uint32 uCrc32Code = 0xffffffff;
	while (uLength)
	{
		int nLen = uLength < m_nMaxLen ? uLength : m_nMaxLen;
		uint8* pData = new uint8[nLen];
		if (pData == NULL)
		{
			fclose(pFile);
			return false;
		}
		int nRead = (int)fread(pData, sizeof(uint8), nLen, pFile);
		if (!nRead)
		{
			break;
		}
		uCrc32Code = Crc32WithInit(pData, nRead, uCrc32Code);

		uLength -= nRead;

		delete[] pData;
		pData = NULL;
	}

	uCrc32Code = ~uCrc32Code;

	fclose(pFile);

	string strFileRealName = strFileName ;
	size_t uPos = strFileName .rfind("/");
	if (uPos != -1)
	{
		strFileRealName = strFileName .substr(uPos + 1, strFileName .length() - uPos - 1);
	}

	hash_map<string, string>::iterator iter = m_hmapCrcCode.find(strFileName );
	int64 uCode;
	if (iter == m_hmapCrcCode.end() || (uCode = _atoi64(iter->second.c_str())) != uCrc32Code)
	{
		string strMsg = "文件：";
		strMsg.append(strFileRealName);
		strMsg.append(" 校验码错误");
		cout << strMsg.c_str() << endl;
		return false;
	}
	return true;
}
