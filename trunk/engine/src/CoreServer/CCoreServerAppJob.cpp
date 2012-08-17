#include "stdafx.h"
#include "CCoreServerJob.inl"
#include "CCoreServerAppJob.h"
#include "CSyncAppServer.h"
#include "CSyncTimeSystemServer.h"
#include "CLog.h"
#include "CCoreServerThreadMgr.h"
#include "CCoreServerAppResult.h"
#include "CCoreServerResult.inl"
#include "ThreadHelper.h"

CCoreServerAppQuitJob::CCoreServerAppQuitJob()
{
}

CCoreServerAppQuitJob::~CCoreServerAppQuitJob()
{
	CSyncAppServer::Inst()->Quit();
}

CCoreServerAppCleanJob::CCoreServerAppCleanJob()
{
}

CCoreServerAppCleanJob::~CCoreServerAppCleanJob()
{
	CSyncAppServer::Inst()->IntCleanUp();
	(new CCoreServerAppCleanResult())->Add();
}

CCorePrintRegisterTickNumJob::CCorePrintRegisterTickNumJob(size_t stCppTickNum, size_t stLuaTickNum)
: m_stCppTickNum(stCppTickNum)
, m_stLuaTickNum(stLuaTickNum)
{
}

namespace sqr
{
	static bool gs_bTickLogExist = false;
}

CCorePrintRegisterTickNumJob::~CCorePrintRegisterTickNumJob()
{
	size_t stNum = CSyncTimeSystemServer::Inst()->GetRegisterTickNum();

	const char* mode = gs_bTickLogExist ? "ab" : "wb";
	if(!gs_bTickLogExist)
		gs_bTickLogExist = true;

	char buf[256];
	sprintf(buf, "Tick_%d.log", GetCurPID());
	CLog* pLog = CLog::CreateLog(buf, mode);

	sprintf(buf, "逻辑线程CppTick数量: %u\t\t逻辑线程LuaTick数量: %u\t\t引擎线程Tick数量: %u", (uint32)m_stCppTickNum, (uint32)m_stLuaTickNum, (uint32)stNum);

	pLog->EnableTime(false);
	pLog->Write(buf);

	pLog->Release();
}


CCorePrintRegisterTickInfoJob::CCorePrintRegisterTickInfoJob(string* strInfo)
: m_strInfo(strInfo)
{
}

CCorePrintRegisterTickInfoJob::~CCorePrintRegisterTickInfoJob()
{
	const char* mode = gs_bTickLogExist ? "ab" : "wb";
	if(!gs_bTickLogExist)
		gs_bTickLogExist = true;

	string strInfo;
	CSyncTimeSystemServer::Inst()->GetRegisterTickInfo(strInfo);

	
	char buf[256];
	sprintf(buf, "Tick_%d.log", GetCurPID());

	CLog* pLog = CLog::CreateLog(buf, mode);

	pLog->EnableTime(false);
	pLog->Write(m_strInfo->c_str());
	pLog->Write(strInfo.c_str());

	pLog->Release();

	delete m_strInfo;
}
