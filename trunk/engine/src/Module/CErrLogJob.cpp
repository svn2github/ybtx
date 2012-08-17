#include "stdafx.h"
#include "CErrLogJob.h"
#include "CErrLog.h"
#include "CError.h"

	CCreateErrLogJob::CCreateErrLogJob(const wchar_t* szFileName, const char* szDeviceInfo)
	{
		m_strFileName=szFileName;
		m_strDeviceInfo=szDeviceInfo;
	}

	CCreateErrLogJob::~CCreateErrLogJob()
	{
		CErrLog::Inst().CreateLogFile(m_strFileName.c_str(),m_strDeviceInfo.c_str());
	}


	CCloseErrLogJob::~CCloseErrLogJob()
	{
		CErrLog::Inst().CloseLogFile();
	}

	CWriteErrLogJob::CWriteErrLogJob(const CError& exp,const char* szErrlogInfo)
	{
		pExp = new CError(exp);
		m_strErrlogInfo=szErrlogInfo;
	}

	CWriteErrLogJob::~CWriteErrLogJob()
	{
		CErrLog::Inst().WriteErrLog(*pExp,m_strErrlogInfo.c_str());
		delete pExp;
}

