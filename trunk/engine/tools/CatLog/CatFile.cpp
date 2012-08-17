#include "stdafx.h"
#include "CatHelper.h"
#include "CMapFileMgr.h"
#include <stdio.h>
#include <stdlib.h>
#include "bzip2/bzlib.h"
#include "CFetchMapFile.h"
#include "ErrLogHelper.h"

bool ParseHead(char* szBuf, uint32 uDataSize, uint32 uVersion, uint32& stOffset)
{
	//first read ybtx_errlog
	static char buf[] = "ybtx_errlog";
	static uint32 uLen = (uint32)strlen(buf);

	if(uDataSize < uLen + sizeof(uint32))
		return false;

	char* szHead = (char*)malloc(uLen + 1);
	memcpy(szHead, szBuf, uLen);

	for(uint32 i = 0; i < uLen; i++)
	{
		szHead[i] ^= 3;
	}
	szHead[uLen] = '\0';

	if(strcmp(buf, szHead) != 0)
	{
		free(szHead);
		return false;
	}
	free(szHead);

	uint32 uFileVersion;
	memcpy(&uFileVersion, szBuf + uLen, sizeof(uint32));

	uFileVersion ^= 0xeeee;
	if(uFileVersion != uVersion)
	{
		return false;
	}

	stOffset += (uLen + sizeof(uint32));

	return true;
}

char* GetFileBuffer(FILE* fdLog, uint32& uDataSize)
{
	//得到文件大小
	fseek(fdLog, 0, SEEK_END);
	uDataSize = ftell(fdLog);

	fseek(fdLog, 0, SEEK_SET);

	//将文件读入buf中
	char* buf = (char*)malloc(uDataSize + 1);
	fread(buf, sizeof(char), uDataSize, fdLog);

	buf[uDataSize] = '\0';

	return buf;
}

char* DecompressAndDecodeBuffer(char* szBuf, uint32 uDataSize, uint32& stOffset)
{
	uint16 uDest = 0;
	memcpy(&uDest, szBuf + stOffset, sizeof(uint16));

	stOffset += sizeof(uint16);

	uint16 uSrcLen = 0;
	memcpy(&uSrcLen, szBuf + stOffset, sizeof(uint16));	

	uint32 uCheckSrcLen = (uint32)(uDest * 1.01 + 640);
	if(uCheckSrcLen <= uSrcLen)
	{
		return NULL;
	}

	int32 uBufLen = (int32)uDataSize - (int32)stOffset;

	if((int32)uSrcLen > uBufLen)
		return NULL;

	char* szDest = (char*)malloc(uDest);
	char* szSrc = (char*)malloc(uSrcLen);

	stOffset += sizeof(uint16);

	memcpy(szSrc, szBuf + stOffset, uSrcLen);

	stOffset += uSrcLen;

	Decode(szSrc, uSrcLen);

	uint32 uDestLen = (uint32)uDest;
	bool bSuc = DeCompress(szDest, &uDestLen, szSrc, uSrcLen);
	free(szSrc);

	if(!bSuc)
	{
		return NULL;	
	}

	return szDest;
}

int DecompressAndDecodeToString(string& strLog, FILE* fdLog, uint32 uVersion)
{
	uint32 uDataSize = 0;
	char* szBuf = GetFileBuffer(fdLog, uDataSize);

	uint32 stOffset = 0;
	bool bHeadSuc = ParseHead(szBuf, uDataSize, uVersion, stOffset);

	if(!bHeadSuc)
	{
		strLog.append(szBuf);
		free(szBuf);
		return -1;
	}

	while(stOffset < uDataSize)
	{
		char* szDest = DecompressAndDecodeBuffer(szBuf, uDataSize, stOffset);

		if(!szDest)
			return -1;

		strLog.append(szDest);
		free(szDest);
	}
	return 0;
}

int DecompressAndDecodeFile(const char* szFileName, uint32 uVersion)
{
	FILE* fdLog = fopen(szFileName, "r+b");

	string strLog;

	int ret = DecompressAndDecodeToString(strLog, fdLog, uVersion);

	fclose(fdLog);

	if(ret == -1)
	{
		return -1;
	}

	fdLog = fopen(szFileName, "wb");

	fprintf(fdLog, "%s", strLog.c_str());

	fclose(fdLog);

	return 0;
}


void FindAndAppendSymbol(string& strLog, FILE* fdLog, uint32 uVersion)
{
	char sep[] = " \t\n";
	char* token;

	uint32 uFunAddr;

	while(true)
	{
		char buf[1024];

		if (fgets(buf, 1024, fdLog) == NULL)
			break;

		/*
		我们只处理一行只有
		Addr: xxxxxxxx    
		这样的情况				  
		*/

		string strInfo;

		bool bFilter = false;
		int32 pos = 0;

		//保存buf到szInfo
		strInfo.append(buf);

		token = strtok(buf, sep);
		while (token)
		{
			if (!bFilter)
			{
				//第一列为Addr:
				if (pos == 0 && strncmp(token, "Addr:", 5) == 0)
				{
					bFilter = true;
				}
			}
			else
			{
				//第二列为地址xxxxxxxx
				if(pos == 1)
				{
					sscanf(token, "%x", &uFunAddr);
				}
			}	

			++pos;
			token = strtok(NULL, sep);
		}

		//该行不需要我们处理
		if (!bFilter)
		{
			strLog.append(strInfo);
		}
		else
		{
			if (pos == 2)
			{
				const char* szFunName = CMapFileMgr::Inst().FindFunName(uFunAddr, uVersion);

				if (szFunName)
				{
					string().swap(strInfo);

					char szBuf[24];
					sprintf(szBuf, "Addr: 0x%x\t", uFunAddr);

					strInfo.append(szBuf);
					strInfo.append(szFunName);
				}

				strLog.append(strInfo).append("\n");
			}
			else
			{
				strLog.append(strInfo);
			}
		}

	}

}


void ParseErrInfo(const char* szFileName, uint32 uVersion, string& strLog)
{
	FILE* fdLog = fopen(szFileName, "rb");
	if(!fdLog)
		return;

	DecompressAndDecodeToString(strLog, fdLog, uVersion);

	fclose(fdLog);

	string szTmpName(szFileName);
	szTmpName.append(".tmp");
	FILE* fdTmp = fopen(szTmpName.c_str(), "wb");
	if(!fdTmp)
		return;

	fprintf(fdTmp, "%s", strLog.c_str());
	fclose(fdTmp);

	fdTmp = fopen(szTmpName.c_str(), "rb");
	if( !fdTmp )
		return;

	strLog.clear();
	FindAndAppendSymbol(strLog, fdTmp, uVersion);

	fclose(fdTmp);
	remove(szTmpName.c_str());
}

void ParseErrComm(const char* szFileName, uint32 uVersion, string& strLog)
{
	FILE* fdLog = fopen(szFileName, "rb");
	if(!fdLog)
		return;

	DecompressAndDecodeToString(strLog, fdLog, uVersion);

	fclose(fdLog);
}

int CatFile(CatFileNameData& data)
{
	string& strBaseName = data.m_strBaseName;

	//gas(c)_err_后面紧跟着的36个字符就是guid
	size_t guidpos = 7;
	if((strBaseName.find("gac_err_") != string::npos) || 
		(strBaseName.find("gas_err_") != string::npos))
	{
		guidpos = 8;
	}

	data.m_strGuid = strBaseName.substr(guidpos, 36);

	if(data.m_bGac && !data.m_bInfo)
	{
		//guid_time_name_v11111.info
		//user name 在time和v两个_之间
		size_t stTemp = strBaseName.find_last_of('_');
		size_t stEnd = strBaseName.find_last_of('_', stTemp - 1);
		size_t stBegin = strBaseName.find_last_of('_', stEnd - 1);

		if(stBegin != string::npos && stEnd != string::npos && stBegin < stEnd - 1)
		{	
			data.m_strUserName.append(strBaseName.substr(stBegin + 1, stEnd - stBegin - 1));
		}
	}


	if(data.m_bInfo)
		CFetchMapFile::Inst().Fetch(data.m_uVersion, data.m_bGac);

	string strLog;

	if(data.m_bInfo)
	{
		ParseErrInfo(data.m_strFileName.c_str(), data.m_uVersion, strLog);
	}
	else
	{
		ParseErrComm(data.m_strFileName.c_str(), data.m_uVersion, strLog);
	}

	//如果是gas，同时comm里面没有process name(很有可能是我们没有配置，那么就用默认的ybtx)
	if(!data.m_bGac && !data.m_bInfo)
	{
		if(strLog.find("Process Name") == string::npos)
		{
			data.m_strUserName = "ybtx";
		}
	}

	//如果存在username，同时我们只需要在comm文件里面有username的信息就可以了
	if(!data.m_strUserName.empty())
	{
		size_t stPos = strLog.find('\n');

		string strUserName("Process Name:");
		strUserName.append(data.m_strUserName).append("\n");
		strLog.insert(stPos + 1, strUserName);
	}

	if(data.m_strOutFile.empty())
	{
		cout << strLog.c_str() << endl;
	}
	else
	{
		FILE* fd = fopen(data.m_strOutFile.c_str(), "wb");
		if(!fd)
		{
			cout << strLog.c_str() << endl;
			return 0;
		}

		fwrite(strLog.c_str(), strLog.size(), 1, fd);
		fclose(fd);
	}

	return 0;
}


