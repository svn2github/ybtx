#include "stdafx.h"
#include "CrcFileCheck.h"
#include "CodeCvs.h"
CrcFileCheck::CrcFileCheck()
{
}
CrcFileCheck::~CrcFileCheck()
{

}
uint32 CrcFileCheck::DoCrc32Check( const wstring& strFile )
{
#ifdef _WIN32
	FILE* fp = _wfopen(strFile.c_str(), L"rb");
#else
	FILE* fp = fopen(utf16_to_utf8(strFile).c_str(),"rb");
#endif
	if (!fp)
	{
		stringstream strMsg;
		strMsg<<"无法打开文件"<<utf16_to_utf8(strFile);
		GenErr(strMsg.str());
		return 0;
	}
	fseek(fp, 0, SEEK_END);
	uint32 uLength = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	uint8* pData = new uint8[uLength];
	fread(pData, sizeof(uint8), uLength, fp);
	fclose(fp);
	uint32 CrcCode = Crc32(pData,uLength);
	delete pData;
	pData = NULL;
	return  CrcCode;
}
void CrcFileCheck::ReadCrcCodeToMap(const wstring& FileName )
{
#ifdef _WIN32
	FILE* pFile = _wfopen(FileName.c_str(), L"rb");
#else
	FILE* pFile = fopen(utf16_to_utf8(FileName).c_str(),"rb");
#endif
	if (pFile)
	{
		char szLine[260];
		memset(szLine, '\0', sizeof(szLine));
		while (fgets(szLine, 260, pFile) != NULL)
		{
			char* szCode = strchr(szLine, '\t');
			if (szCode != NULL)
			{
				int uPos = (int)(szCode - szLine);
				char* szFileName = new char[uPos + 1];
				strncpy(szFileName, szLine, uPos);
				szFileName[uPos] = '\0';
				szCode++;
#ifdef _WIN32
				m_mFile2CrcCode.insert(make_pair(szFileName, _atoi64(szCode)));
#else
				m_mFile2CrcCode.insert(make_pair(szFileName, atoll(szCode)));
#endif
				delete[] szFileName;
				szFileName = NULL;
			}
		}
		fclose(pFile);
	}
}

