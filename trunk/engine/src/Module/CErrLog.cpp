#include "stdafx.h"
#include "CErrLog.h"
#include "jenkins.h"
#include "CError.h"
#include "ErrLogHelper.h"
#include <time.h>
#include "CErrLogCompress.h"
#include "CErrLogEncrypt.h"
#include "UtfCvs.h"
#include "CErrLogMgr.h"

namespace sqr{

struct ErrInfo: public CMallocObject
{
public:
	ErrInfo(uint32 uErrId)
		: m_uErrId(uErrId)
		, m_uErrNum(0)
	{}
	uint32	m_uErrId;       //错误码
	uint32	m_uErrNum;		//错误发生次数
};



class CErrInfoData : public CMallocObject
{
public:
	uint32 ErrInfo2ID(const char* str, uint32 len)
	{
		if (str == NULL)
			return 0;
		return sqr::hash::jenkins::hash((uint8*)str, len, len);
	}
	ErrInfo* GetErrInfo(const char* szErrInfo)
	{
		uint32 uLen = strlen(szErrInfo);
		uint32 uErrId = ErrInfo2ID(szErrInfo, uLen);

		MapErrInfo_t::iterator iter = m_mapErrInfo.find(uErrId);

		if(iter == m_mapErrInfo.end())
		{
			ErrInfo* pErrInfo = new ErrInfo(uErrId);
			m_mapErrInfo.insert(make_pair(uErrId, pErrInfo));
			return pErrInfo;
		}

		return iter->second;
	}
	typedef set<uint32, less<uint32>, TMallocAllocator<uint32> >	SetErrInfo_t;
	SetErrInfo_t			m_setErrInfo;
private:
	typedef map<uint32, ErrInfo*, less<uint32>, TMallocAllocator<pair<uint32, ErrInfo*> > >	MapErrInfo_t;
	MapErrInfo_t			m_mapErrInfo;
};
}

CErrLog::CErrLog()
:m_fdErrComm(NULL)
,m_fdErrInfo(NULL)
{
	m_pErrInfoData = new CErrInfoData();
}

CErrLog::~CErrLog()
{
	delete m_pErrInfoData;
	CloseLogFile();
}

void CErrLog::CreateLogFile(const wchar_t* szErrFileName,const char* szDeviceInfo)
{
	wstring strErrFileName = szErrFileName;
#ifdef _WIN32 
	m_fdErrComm = _wfopen((strErrFileName+L".comm").c_str(),L"wb");
	m_fdErrInfo = _wfopen((strErrFileName+L".info").c_str(), L"wb");
#else
	char* strErrComm = U16_to_U8((strErrFileName+L".comm").c_str());
	char* strErrInfo = U16_to_U8((strErrFileName+L".info").c_str());
	m_fdErrComm = fopen(strErrComm, "wb");
	m_fdErrInfo = fopen(strErrInfo, "wb");
	FreeUtfCvsDest(strErrComm);
	FreeUtfCvsDest(strErrInfo);
#endif	
	if (m_fdErrInfo&&m_fdErrComm)
	{
		WriteHeadLog(m_fdErrInfo);
		WriteHeadLog(m_fdErrComm);
		HandleAndWriteLog(m_fdErrComm, szDeviceInfo);
	}
	else
	{
		CErrLogMgr::Inst().SetErrLogEnabled(false);
	}
}

void CErrLog::CloseLogFile()
{
	if (m_fdErrComm)
	{
		fclose(m_fdErrComm);
		m_fdErrComm = NULL;
	}

	if (m_fdErrInfo)
	{
		fclose(m_fdErrInfo);
		m_fdErrInfo = NULL;
	}
}

void CErrLog::WriteErrLog(const CError& exp,const char* szErrlogInfo)
{
	if (!m_fdErrInfo||!m_fdErrComm)
	{
		return;
	}

	const char* szErrorType = exp.ErrorTitle();
	const char* szStackInfo = exp.StackInfo();

	CLogStringStream strKey;
	strKey << szErrorType << szStackInfo;

	//我们通过stack来进行判断是否是同一类错误
	ErrInfo* pErrInfo = m_pErrInfoData->GetErrInfo(strKey.str().c_str());

	uint32 uErrId = pErrInfo->m_uErrId;
	uint32 uErrNum = (++pErrInfo->m_uErrNum);

	//看是否是第一次插入，如果是，则记录info信息
	bool bSuc = m_pErrInfoData->m_setErrInfo.insert(uErrId).second;

	//如果是第一次记录，则把错误详细堆栈信息记录到info文件上面
	if (bSuc)
	{
		CLogStringStream str;
		str << "编号: " << uErrId << endl;
		str << "信息: " << szErrorType << endl;

		if(szStackInfo[0] != '\0')
		{
			str << "堆栈: " << endl;
			FilterStackInfo(szStackInfo, str);
			str << endl;					
		}

		const char* szExpandInfo = exp.ExpandInfo();
		if(szExpandInfo[0] != '\0')
		{
			str << "额外信息: " << endl;
			str << szExpandInfo << endl;
		}

		const char* szCategory = exp.Category();
		if(szCategory[0] != '\0')
		{
			str << "种类: " << szCategory << endl;
		}

		str << endl;

		HandleAndWriteLog(m_fdErrInfo, str.str().c_str());
	}

	CLogStringStream sCommMsg;
	sCommMsg << exp.ErrorMsg();

	const char* sMemInfo = exp.MemInfo();
	if (sMemInfo[0] != '\0')
	{
		sCommMsg << "\tMemInfo:" << sMemInfo;
	}

	HandleAndWriteLog(m_fdErrComm, szErrlogInfo);
	WriteCommLog(uErrId, uErrNum, sCommMsg.str().c_str());
}

//过滤堆栈信息，只保存堆栈地址
void CErrLog::FilterStackInfo(const char* szName, CLogStringStream& strm)
{
	if(!ErrLogEncrypted())
	{
		strm << szName;
		return;
	}

	//在windows情况下，我们只记录堆栈地址，不记录详细信息，
	//因为以后不会对外提供mapfile，所以现在即使有mapfile，我们也当成没有mapfile处理
#ifdef _WIN32

	string strFilter;
	char* szStr = (char*)malloc(strlen(szName) + 1);
	strcpy(szStr, szName);

	char seps[] = "\n";
	char* token = strtok(szStr, seps);
	while(token != NULL)
	{
		//如果这一行有Addr:,表明是一个c++的堆栈，我们就要处理了
		if(strstr(token, "Addr:"))
		{
			string strToken(token);
			uint32 pos = strToken.find('\t');
			string subToken = strToken.substr(0, pos);
			strFilter.append(subToken).append("\n");
		}
		else
		{
			strFilter.append(token).append("\n");		
		}
		token = strtok(NULL, seps);
	}
	free(szStr);
	strm << strFilter.c_str();

#else
	strm << szName;
#endif
}

void CErrLog::WriteHeadLog(FILE* fd)
{
	if(!ErrLogEncrypted())
	{
		return;
	}

	static char buf[] = "ybtx_errlog";
	static uint32 uLen = strlen(buf);
	char* szHead = (char*)malloc(uLen);
	for(uint32 i = 0; i < uLen; i++)
	{
		szHead[i] = buf[i] ^ 3;
	}

	fwrite(szHead, sizeof(char), uLen, fd);  
	free(szHead);

	uint32 uVersion = GetCurrentRevision();
	uVersion ^= 0xeeee;
	fwrite(&uVersion, sizeof(uint32), 1, fd); //写入版本号
	fflush(fd);
}

void CErrLog::WriteCommLog(uint32 uErrId, uint32 uErrNum, const char* szMsg)
{
	//在每次写comm之前，我们会把一些err msg记录到comm里面

	//把错误发生次数，以及时间记录到log文件上面
	time_t T;
	struct tm * timenow;

	time ( &T );
	timenow = localtime ( &T ); 

	ostringstream str;
	str << uErrId;
	str << " \t" << timenow->tm_year + 1900 << "-" << 
		timenow->tm_mon + 1<< "-" <<
		timenow->tm_mday << "-" <<
		timenow->tm_hour << ":" <<
		timenow->tm_min << ":" <<
		timenow->tm_sec;
	str << "\t" << uErrNum << "\t";		

	if(szMsg)
	{
		string strMsg(szMsg);
		FilterErrorMsg(strMsg);
		str << strMsg.c_str() << endl;
	}

	HandleAndWriteLog(m_fdErrComm, str.str().c_str());
}

void CErrLog::HandleAndWriteLog(FILE* fd, const char* szErrLog)
{
	if(!ErrLogEncrypted())
	{
		fprintf(fd, "%s", szErrLog);
		fflush(fd);
		return;
	}

	//first compress the buffer
	uint32 uSourceLen = strlen(szErrLog) + 1;

	//得到适宜的dest大小
	uint32 uDestLen = (uint32)(uSourceLen * 1.01 + 640);
	char* szDest = (char*)malloc(uDestLen);

	CErrLogCompress::Inst().Compress(szDest, &uDestLen, szErrLog, uSourceLen);

	//then encode the buffer
	CErrLogEncrypt::Inst().Encode(szDest, uDestLen);

	//write the log

	//first write the source len and the compress len
	uint16 uSrc = (uint16)uSourceLen;    //由于我们的buffer的长度不可能超过2^16，所以用uint16存储
	uint16 uDest = (uint16)uDestLen;

	fwrite(&uSrc, sizeof(uint16), 1, fd); 
	fwrite(&uDest, sizeof(uint16), 1, fd);

	fwrite(szDest, sizeof(char), uDestLen, fd);
	fflush(fd);

	free(szDest);
}

void CErrLog::FilterErrorMsg(string& strMsg)
{
	size_t stSize = strMsg.size();
	for(size_t i = 0; i < stSize; i++)
	{
		if(strMsg[i] == '\n')
			strMsg[i] = ' ';
	}
}
