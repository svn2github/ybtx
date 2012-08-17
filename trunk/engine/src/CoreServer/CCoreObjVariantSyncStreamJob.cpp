#include "stdafx.h"
#include "CCoreObjVariantSyncStreamJob.h"
#include "CSyncCoreObjectDictator.h"
#include "CCoreObjectDictator.h"
#include "CCacheVariantServerHolder.h"
#include "CCoreServerJob.inl"
#include "TVariantJobAllocator.inl"
#include "Memory.h"
#include "ThreadHelper.h"
#include "TVariantStream.inl"


template class basic_stringstream<char,std::char_traits<char>,TVariantJobAllocator<char> >;
template class basic_string<char,std::char_traits<char>,TVariantJobAllocator<char> >;

CCoreObjVariantSyncStreamJob::CCoreObjVariantSyncStreamJob(uint32 uObjID, bool bForSync, ECodeTableIndex eCodeTableIndex)
:CCoreServerObjJob(uObjID)
,m_bForSync(bForSync)
,m_eCodeTableIndex(eCodeTableIndex)
{
}

CCoreObjVariantSyncStreamJob::~CCoreObjVariantSyncStreamJob()
{
	CSyncCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	CSyncCoreObjectDictator* pDicObj = pCoreObj->CastToObjDictator();
	Ast( pDicObj );
	CCacheVariantServerHolder* pVariantHolder = m_bForSync ? pDicObj->GetSyncVariantHolder(m_eCodeTableIndex) : pDicObj->GetViewVariantHolder(m_eCodeTableIndex);
	pVariantHolder->DecodeAndSyncStream(m_Stream);
}

//这个变量在CAppServer的构造函数里面设置正确值
HTHREADID CCoreObjVariantSyncStreamJob::ms_CreateJobThreadID=0;
