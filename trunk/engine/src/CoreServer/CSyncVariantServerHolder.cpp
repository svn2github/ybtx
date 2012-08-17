#include "stdafx.h"
#include "CSyncVariantServerHolder.h"
#include "CCoreObjectDictator.h"
#include "CSyncVariantServer.h"
#include "CCacheVariantServer.h"
#include "CCoreServerThreadMgr.h"
#include "CCoreObjVariantSyncStreamJob.h"
#include "CCoreServerJob.inl"
#include "CObjVarDefCfg.h"
#include "TRootVariantPoolMgr.inl"
#include "CCypherVariantHolder.inl"
#include "TVariantStream.inl"

CSyncVariantServerHolder::CSyncVariantServerHolder(uint32 uVarDefID,CCypherVariantCodeTable *pCodeTable,CCoreObjectDictator* pCoreObj,bool bForSync,ECodeTableIndex eCodeTableIndex)
:CCypherVariantHolder(pCodeTable)
,m_pCoreObj(pCoreObj)
,m_bForSync(bForSync)
,m_eCodeTableIndex(eCodeTableIndex)
,m_uVarDefID(uVarDefID)
{
	GetRootVariant()->m_pHolder = this;
}

CSyncVariantServerHolder::~CSyncVariantServerHolder(void)
{
	if(IntGetRootVariant())
	{
		static_cast<CSyncVariantServer*>(IntGetRootVariant())->RecycleSubTree();
		TRootVariantPoolMgr<CSyncVariantServer>::GetInst()->GetPool(m_uVarDefID, m_bForSync, m_eCodeTableIndex)->Push(IntGetRootVariant());
	}
}

CCypherVariant* CSyncVariantServerHolder::CreateVariant() const
{
	CSyncVariantServer *pVariant = new CSyncVariantServer(NULL);
	pVariant->m_pHolder = const_cast<CSyncVariantServerHolder*>( this );
	return pVariant;
}

CCypherVariant* CSyncVariantServerHolder::IntAddSubTree(CCypherVariant* pRoot, const char* newName, uint32 uSubTreeType)
{
	return CCypherVariantHolder::IntAddSubTree<CSyncVariantServer>(pRoot, newName, uSubTreeType);
}
		
CSyncVariantServer* CSyncVariantServerHolder::GetRootVariant() const
{
	CSyncVariantServer* pRootVariant = static_cast<CSyncVariantServer*>(IntGetRootVariant());
	if(pRootVariant)
		return pRootVariant;

	pRootVariant = const_cast<CSyncVariantServerHolder*>(this)->CCypherVariantHolder::AcquireRootVariantFromPool<CSyncVariantServer>(TRootVariantPoolMgr<CSyncVariantServer>::GetInst()->GetPool(m_uVarDefID, m_bForSync, m_eCodeTableIndex));
	if(pRootVariant->m_pHolder != this)
	{
		pRootVariant->m_pHolder = const_cast<CSyncVariantServerHolder*>(this);
		pRootVariant->ResetDataMember();
	}
	return pRootVariant;
}

const char* CSyncVariantServerHolder::GetString( const char* szPathName )const
{
	return CCypherVariantHolder::GetString( szPathName );
}

CSyncVariantServer* CSyncVariantServerHolder::SetString( const char* szPathName, const char* szValue)
{
	CCoreObjVariantSyncStreamJob* pJob = new CCoreObjVariantSyncStreamJob( m_pCoreObj->GetGlobalID(), m_bForSync, m_eCodeTableIndex );

	CSyncVariantServer* pVariant = 
		static_cast<CSyncVariantServer*>(CCypherVariantHolder::SetString(szPathName, szValue, &(pJob->m_Stream.GetInputStream())));

	pJob->Add();
	//pJob->Test();

	return pVariant;
}

template<typename ValueType>
ValueType CSyncVariantServerHolder::GetNumber( const char* szPathName ) const
{
	return CCypherVariantHolder::GetNumber<ValueType>( szPathName );
}

template<typename ValueType>
CSyncVariantServer* CSyncVariantServerHolder::SetNumber( const char* szPathName, ValueType Value)
{
	CCoreObjVariantSyncStreamJob* pJob = new CCoreObjVariantSyncStreamJob( m_pCoreObj->GetGlobalID(), m_bForSync, m_eCodeTableIndex );

	CSyncVariantServer* pVariant =
		static_cast<CSyncVariantServer*>(CCypherVariantHolder::SetNumber<ValueType>(szPathName, Value, &(pJob->m_Stream.GetInputStream())));

	pJob->Add();
	//pJob->Test();
	
	return pVariant;
}


void CSyncVariantServerHolder::SetDynRoot( const char* szPathName)
{
	CCoreObjVariantSyncStreamJob* pJob = new CCoreObjVariantSyncStreamJob( m_pCoreObj->GetGlobalID(), m_bForSync, m_eCodeTableIndex );
	CCypherVariantHolder::SetMember(szPathName, &(pJob->m_Stream.GetInputStream()));
	pJob->Add();
	//pJob->Test();
}

void CSyncVariantServerHolder::SetEmptyTable( const char* szPathName )
{
	CCoreObjVariantSyncStreamJob* pJob = new CCoreObjVariantSyncStreamJob( m_pCoreObj->GetGlobalID(), m_bForSync, m_eCodeTableIndex );

	CCypherVariantHolder::SetEmptyTable(szPathName, &(pJob->m_Stream.GetInputStream()));

	pJob->Add();
	//pJob->Test();
}

bool CSyncVariantServerHolder::IsEmptyTable( const char* szPathName )const
{
	return CCypherVariantHolder::IsEmptyTable( szPathName );
}

CSyncVariantServer* CSyncVariantServerHolder::AddSubTree(const char* path, const char* subTreeName, const char* newName)
{
	//CCoreObjVariantSyncStreamJob* pJob = new CCoreObjVariantSyncStreamJob( m_pCoreObj->GetGlobalID(), m_bForSync );

	CSyncVariantServer* pVariant = static_cast<CSyncVariantServer*>(CCypherVariantHolder::AddSubTree(path, subTreeName, newName));

	//pVariant->Encode( pJob->m_Stream );
	
	//pJob->Add();
	//pJob->Test();

	return pVariant;
}

bool CSyncVariantServerHolder::DelMember(const char* szPathName)
{
	CCoreObjVariantSyncStreamJob* pJob = new CCoreObjVariantSyncStreamJob( m_pCoreObj->GetGlobalID(), m_bForSync, m_eCodeTableIndex );

	bool bRet = CCypherVariantHolder::DelMember(szPathName, &(pJob->m_Stream.GetInputStream()));
	
	if(bRet)
	{
		pJob->Add();
		//pJob->Test();
	}
	else
	{
		//cout << "delete job in holder" << endl;
		delete pJob;
	}
	
	return bRet;
}

template CSyncVariantServer* CSyncVariantServerHolder::SetNumber<bool>(const char*,bool Value);
template CSyncVariantServer* CSyncVariantServerHolder::SetNumber<int8>(const char*,int8 Value);
template CSyncVariantServer* CSyncVariantServerHolder::SetNumber<uint8>(const char*,uint8 Value);
template CSyncVariantServer* CSyncVariantServerHolder::SetNumber<int16>(const char*,int16 Value);
template CSyncVariantServer* CSyncVariantServerHolder::SetNumber<uint16>(const char*,uint16 Value);
template CSyncVariantServer* CSyncVariantServerHolder::SetNumber<int32>(const char*,int32 Value);
template CSyncVariantServer* CSyncVariantServerHolder::SetNumber<uint32>(const char*,uint32 Value);
template CSyncVariantServer* CSyncVariantServerHolder::SetNumber<int64>(const char*,int64 Value);
template CSyncVariantServer* CSyncVariantServerHolder::SetNumber<uint64>(const char*,uint64 Value);
template CSyncVariantServer* CSyncVariantServerHolder::SetNumber<float>(const char*,float Value);
template CSyncVariantServer* CSyncVariantServerHolder::SetNumber<double>(const char*,double Value);

template bool CSyncVariantServerHolder::GetNumber<bool>(const char*)const;
template int8 CSyncVariantServerHolder::GetNumber<int8>(const char*)const;
template uint8 CSyncVariantServerHolder::GetNumber<uint8>(const char*)const;
template int16 CSyncVariantServerHolder::GetNumber<int16>(const char*)const;
template uint16 CSyncVariantServerHolder::GetNumber<uint16>(const char*)const;
template int32 CSyncVariantServerHolder::GetNumber<int32>(const char*)const;
template uint32 CSyncVariantServerHolder::GetNumber<uint32>(const char*)const;
template int64 CSyncVariantServerHolder::GetNumber<int64>(const char*)const;
template uint64 CSyncVariantServerHolder::GetNumber<uint64>(const char*)const;
template float CSyncVariantServerHolder::GetNumber<float>(const char*)const;
template double CSyncVariantServerHolder::GetNumber<double>(const char*)const;
