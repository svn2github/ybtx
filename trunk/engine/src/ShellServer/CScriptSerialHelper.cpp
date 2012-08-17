#include "stdafx.h"
#include "CScriptSerialHelper.h"
#include "CScriptThreadMgr.h"

void CSerialHelper::PostPackFun(uint32 uThreadIndex, 
								const char* strType, size_t stTypeSize,
								const char* vecBuf, size_t stBufSize)
{
	CScriptThreadMgr::Inst()->PostPackFunJob(uThreadIndex, strType, stTypeSize, vecBuf, stBufSize);
}

ON_UNPACKFUNCTION CSerialHelper::ms_OnUnPackFun = NULL;

//----------------------------------------------------------------------------

void CThreadSerialHelper::PostPackFun(uint32 uThreadIndex, 
									  const char* strType, size_t stTypeSize,
									  const char* vecBuf, size_t stBufSize)
{
	CScriptThreadMgr::Inst()->PostPackFunRes(strType, stTypeSize, vecBuf, stBufSize);
}

ON_UNPACKFUNCTION CThreadSerialHelper::ms_OnUnPackFun = NULL;

