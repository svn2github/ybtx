#include "stdafx.h"
#include "CScriptJob.inl"
#include "CScriptSerialHelper.h"
#include "CScriptThread.h"
#include "IScriptThreadHandler.h"
#include "CFileWatcherData.h"


CScriptPostPackJob::CScriptPostPackJob(const char* strType, size_t stTypeSize,
									   const char* vecBuf, size_t stBufSize, 
									   CScriptThread* pThread)
: m_stTypeSize(stTypeSize)
, m_stBufSize(stBufSize)
, m_pScript(pThread->GetScript())
{
	m_pBuffer = CloneData(vecBuf, m_stBufSize, pThread);
	m_szType = (char*)(CloneData(strType, m_stTypeSize, pThread));
}


CScriptPostPackJob::~CScriptPostPackJob()
{
	CThreadSerialHelper::ms_OnUnPackFun(m_pScript, m_szType, m_stTypeSize, (char*)m_pBuffer, m_stBufSize);	
}


CChangeFileJob::CChangeFileJob(CFileWatcherData* pData, CScriptThread* pThread)
: m_pThread(pThread)
{
	m_pData = new CFileWatcherData(*pData);	
}

CChangeFileJob::~CChangeFileJob()
{
	m_pThread->GetHandler()->OnFileChanged(m_pData);
	m_pData->Release();
}
