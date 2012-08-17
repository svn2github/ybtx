#include "stdafx.h"
#include "CSyncVariantServer.h"
#include "CCoreServerThreadMgr.h"
#include "CSyncVariantServerHolder.h"
#include "CCoreObjectDictator.h"
#include "CCoreObjVariantSyncStreamJob.h"
#include "CCoreServerJob.inl"
#include "CCypherVariant.inl"

CSyncVariantServer::CSyncVariantServer(CSyncVariantServerHolder* pHolder)
:m_pHolder(pHolder)
{
}

CSyncVariantServer::~CSyncVariantServer(void)
{
}

CCypherVariant* CSyncVariantServer::CreateVariant()const
{
	return new CSyncVariantServer(GetHolder());
}

void CSyncVariantServer::ResetDataMember()
{
	CSyncVariantServer* pParent = GetParent();
	if(pParent)
		m_pHolder = pParent->GetHolder();
	CCypherVariant::ResetDataMember();
}

void CSyncVariantServer::RecycleSubTree()
{
	IntRecycleSubTree<CSyncVariantServer>();
}

//void CSyncVariantServer::InitAllMember()
//{
//	CCypherVariant::InitAllMember(true);
//}

void CSyncVariantServer::SetString(const char* szValue)
{
	CCoreObjVariantSyncStreamJob* pJob =
		new CCoreObjVariantSyncStreamJob( m_pHolder->m_pCoreObj->GetGlobalID(), m_pHolder->m_bForSync, m_pHolder->m_eCodeTableIndex );

	CCypherVariant::SetString(szValue, &(pJob->m_Stream.GetInputStream()) );

	pJob->Add();
	//pJob->Test();
}

template<typename ValueType>
void CSyncVariantServer::SetNumber(ValueType Value)
{
	CCoreObjVariantSyncStreamJob* pJob =
		new CCoreObjVariantSyncStreamJob( m_pHolder->m_pCoreObj->GetGlobalID(), m_pHolder->m_bForSync, m_pHolder->m_eCodeTableIndex );

	CCypherVariant::SetNumber(Value, &(pJob->m_Stream.GetInputStream()) );

	pJob->Add();
	//pJob->Test();
}

CSyncVariantServer* CSyncVariantServer::SetMember(const char* szName)
{
	CCoreObjVariantSyncStreamJob* pJob =
		new CCoreObjVariantSyncStreamJob( m_pHolder->m_pCoreObj->GetGlobalID(), m_pHolder->m_bForSync, m_pHolder->m_eCodeTableIndex );

	CSyncVariantServer *pSyncVariant =
		static_cast<CSyncVariantServer*>( CCypherVariant::SetMember(szName, &(pJob->m_Stream.GetInputStream()) ) );

	pJob->Add();
	//pJob->Test();

	return pSyncVariant;
}

CSyncVariantServer* CSyncVariantServer::GetMember(const char* szName)const
{
	return static_cast<CSyncVariantServer*>( CCypherVariant::GetMember(szName) );
}

bool CSyncVariantServer::DelMember(const char* szName)
{
	CCoreObjVariantSyncStreamJob* pJob =
		new CCoreObjVariantSyncStreamJob( m_pHolder->m_pCoreObj->GetGlobalID(), m_pHolder->m_bForSync, m_pHolder->m_eCodeTableIndex );

	bool bRet = CCypherVariant::DelMember( szName, &(pJob->m_Stream.GetInputStream()) );

	if( bRet )
	{
		pJob->Add();
		//pJob->Test();
	}
	else
	{
		cout << "delete job " << endl;
		delete pJob;
	}

	return bRet;
}

void CSyncVariantServer::SetEmptyTable()
{
	CCoreObjVariantSyncStreamJob* pJob =
		new CCoreObjVariantSyncStreamJob( m_pHolder->m_pCoreObj->GetGlobalID(), m_pHolder->m_bForSync, m_pHolder->m_eCodeTableIndex );

	CCypherVariant::SetEmptyTable( &(pJob->m_Stream.GetInputStream()) );

	pJob->Add();
	//pJob->Test();
}

template void CSyncVariantServer::SetNumber<bool>(bool Value);
template void CSyncVariantServer::SetNumber<int8>(int8 Value);
template void CSyncVariantServer::SetNumber<uint8>(uint8 Value);
template void CSyncVariantServer::SetNumber<int16>(int16 Value);
template void CSyncVariantServer::SetNumber<uint16>(uint16 Value);
template void CSyncVariantServer::SetNumber<int32>(int32 Value);
template void CSyncVariantServer::SetNumber<uint32>(uint32 Value);
template void CSyncVariantServer::SetNumber<int64>(int64 Value);
template void CSyncVariantServer::SetNumber<uint64>(uint64 Value);
template void CSyncVariantServer::SetNumber<float>(float Value);
template void CSyncVariantServer::SetNumber<double>(double Value);
