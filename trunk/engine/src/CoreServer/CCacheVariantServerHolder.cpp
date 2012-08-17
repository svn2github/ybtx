#include "stdafx.h"
#include "ISend.h"
#include "CCacheVariantServerHolder.h"
#include "CCacheVariantServer.h"
#include "CSynConnServer.h"
#include "CSyncCoreObjectMediator.h"
#include "PrtlGas2GacObjectDef.h"
#include "CSyncAppServer.h"
#include "IMulticast.h"
#include "CSynConnMgrServer.h"
#include "CCoreObjVariantSyncStreamJob.h"
#include "CCoreServerJob.inl"
#include "CSyncVariantServerHolder.h"
#include "CObjVarDefCfg.h"
#include "TRootVariantPoolMgr.inl"
#include "CCypherVariantHolder.inl"
#include "CAppConfigServer.h"
#include "TVariantJobAllocator.inl"
#include "TVariantStream.inl"


CCacheVariantServerHolder::CCacheVariantServerHolder(uint32 uVarDefID, CCypherVariantCodeTable *pCodeTable, CSyncCoreObjectDictator* pCoreObj, bool bForSync, ECodeTableIndex eCodeTableIndex)
:CCypherVariantHolderForDecode(pCodeTable)
{
	m_bForSync		= bForSync;
	m_eCodeTableIndex = eCodeTableIndex;
	m_pMulticast	= bForSync?pCoreObj->m_pSyncMulticast:pCoreObj->m_pViewMulticast;
	m_pCoreObj		= pCoreObj ;
	m_uEncodeTimes	= 0 ;
	m_uRealEncodeTimes	= 0 ;
	m_bDirtyFlag	= true ;
	m_uVarDefID		= uVarDefID;

	GetRootVariant()->m_pHolder = this;
	InitVariantContainer();
}

CCacheVariantServerHolder::~CCacheVariantServerHolder()
{
	if(IntGetRootVariant())
	{
		static_cast<CCacheVariantServer*>(IntGetRootVariant())->RecycleSubTree();
		TRootVariantPoolMgr<CCacheVariantServer>::GetInst()->GetPool(m_uVarDefID, m_bForSync, m_eCodeTableIndex)->Push(IntGetRootVariant());
	}
}

CCypherVariant* CCacheVariantServerHolder::CreateVariant() const
{
	CCacheVariantServer *pVariant = new CCacheVariantServer(NULL);
	pVariant->m_pHolder = const_cast<CCacheVariantServerHolder*>( this );
	
	return pVariant;
}

CCypherVariant* CCacheVariantServerHolder::IntAddSubTreeForDecode(CCypherVariant* pRoot, uint32 uSubTreeType, uint32 uSubTreeIndex)
{
	return CCypherVariantHolderForDecode::IntAddSubTreeForDecode<CCacheVariantServer>(pRoot, uSubTreeType, uSubTreeIndex);
}

void CCacheVariantServerHolder::UpdateStringBufferIfNeeded()
{
	if (!IsDirty())
		return;

	m_sInitSyncData.Clear();
	IntGetRootVariant()->Encode(m_sInitSyncData.GetInputStream());
	m_sInitSyncData.Finalize();
	SetDirty(false);
}

CCacheVariantServer* CCacheVariantServerHolder::GetRootVariant() const
{
	CCacheVariantServer* pRootVariant = static_cast<CCacheVariantServer*>(IntGetRootVariant());
	if(pRootVariant)
		return pRootVariant;
	pRootVariant = const_cast<CCacheVariantServerHolder*>(this)->CCypherVariantHolder::AcquireRootVariantFromPool<CCacheVariantServer>(TRootVariantPoolMgr<CCacheVariantServer>::GetInst()->GetPool(m_uVarDefID, m_bForSync, m_eCodeTableIndex));
	if(pRootVariant->m_pHolder != this)
	{
		pRootVariant->m_pHolder = const_cast<CCacheVariantServerHolder*>(this);
		pRootVariant->ResetDataMember();
	}
	return pRootVariant;
}


template <class CmdType>
void CCacheVariantServerHolder::MulticastCmd(CCacheVariantServerHolder* pThis,const char* szBuffer,size_t stSize,CSynConnServer* pConn)
{
	CmdType cmd;
	cmd.uqbGlobalID = pThis->m_pCoreObj->GetGlobalID();
	cmd.uEncodeTimes = pThis->m_uEncodeTimes;
	cmd.SetDataLength( stSize );

	CSyncCoreObjectDictator::MulticastCmdForAoi(&cmd, sizeof(cmd),pThis->m_pMulticast);
	CSyncCoreObjectDictator::MulticastCmdForAoi(szBuffer, (uint32)stSize,pThis->m_pMulticast);
	
	if (pConn)
	{
		pConn->SendCoreCmd( &cmd, sizeof(cmd) );
		pConn->SendCoreCmd( szBuffer, (uint32)stSize );
	}
}


CCacheVariantServerHolder::MulticastCmdFunc CCacheVariantServerHolder::GetMulticastCmdFunc(size_t uSize)
{
	if (m_eCodeTableIndex == eCTI_Static)
	{
		if ( m_pCoreObj->m_pSyncMulticast == m_pMulticast )
		{
			if (uSize <= 0xFF)
			{
				return &MulticastCmd<CGas2GacOC_Static_Sync_Variant_Small_Data>;
			}
			else if (uSize <= 0xFFFF)
			{
				return &MulticastCmd<CGas2GacOC_Static_Sync_Variant_Middle_Data>;
			}
			else
			{
				return &MulticastCmd<CGas2GacOC_Static_Sync_Variant_Big_Data>;
			}
		}
		else
		{
			if (uSize <= 0xFF)
			{
				return &MulticastCmd<CGas2GacOC_Static_View_Variant_Small_Data>;
			}
			else if (uSize <= 0xFFFF)
			{
				return &MulticastCmd<CGas2GacOC_Static_View_Variant_Middle_Data>;
			}
			else
			{
				return &MulticastCmd<CGas2GacOC_Static_View_Variant_Big_Data>;
			}
		}
	}
	else
	{
		if ( m_pCoreObj->m_pSyncMulticast == m_pMulticast )
		{
			if (uSize <= 0xFF)
			{
				return &MulticastCmd<CGas2GacOC_Agile_Sync_Variant_Small_Data>;
			}
			else if (uSize <= 0xFFFF)
			{
				return &MulticastCmd<CGas2GacOC_Agile_Sync_Variant_Middle_Data>;
			}
			else
			{
				return &MulticastCmd<CGas2GacOC_Agile_Sync_Variant_Big_Data>;
			}
		}
		else
		{
			if (uSize <= 0xFF)
			{
				return &MulticastCmd<CGas2GacOC_Agile_View_Variant_Small_Data>;
			}
			else if (uSize <= 0xFFFF)
			{
				return &MulticastCmd<CGas2GacOC_Agile_View_Variant_Middle_Data>;
			}
			else
			{
				return &MulticastCmd<CGas2GacOC_Agile_View_Variant_Big_Data>;
			}
		}
	}
}


template <class CmdType>
void CCacheVariantServerHolder::UniquecastCmd(CCacheVariantServerHolder* pThis,const char* szBuffer,size_t stSize,CSynConnServer* pConn)
{
	CmdType cmd;
	cmd.uqbGlobalID = pThis->m_pCoreObj->GetGlobalID();
	cmd.uEncodeTimes = pThis->m_uEncodeTimes;
	cmd.SetDataLength( (uint32)stSize );

	pConn->SendCoreCmd( &cmd, sizeof(cmd) );
	pConn->SendCoreCmd( szBuffer, (uint32)stSize );
}


CCacheVariantServerHolder::UniquecastCmdFunc CCacheVariantServerHolder::GetUniquecastCmdFunc(size_t uSize)
{
	if (m_eCodeTableIndex == eCTI_Static)
	{
		if ( m_pCoreObj->m_pSyncMulticast== m_pMulticast )
		{
			if (uSize <= 0xFF)
			{
				return &UniquecastCmd<CGas2GacOC_Static_Sync_Variant_Small_Data>;
			}
			else if (uSize <= 0xFFFF)
			{
				return &UniquecastCmd<CGas2GacOC_Static_Sync_Variant_Middle_Data>;
			}
			else
			{
				return &UniquecastCmd<CGas2GacOC_Static_Sync_Variant_Big_Data>;
			}
		}
		else
		{
			if (uSize <= 0xFF)
			{
				return &UniquecastCmd<CGas2GacOC_Static_View_Variant_Small_Data>;
			}
			else if (uSize <= 0xFFFF)
			{
				return &UniquecastCmd<CGas2GacOC_Static_View_Variant_Middle_Data>;
			}
			else
			{
				return &UniquecastCmd<CGas2GacOC_Static_View_Variant_Big_Data>;
			}
		}
	}
	else
	{
		if ( m_pCoreObj->m_pSyncMulticast== m_pMulticast )
		{
			if (uSize <= 0xFF)
			{
				return &UniquecastCmd<CGas2GacOC_Agile_Sync_Variant_Small_Data>;
			}
			else if (uSize <= 0xFFFF)
			{
				return &UniquecastCmd<CGas2GacOC_Agile_Sync_Variant_Middle_Data>;
			}
			else
			{
				return &UniquecastCmd<CGas2GacOC_Agile_Sync_Variant_Big_Data>;
			}
		}
		else
		{
			if (uSize <= 0xFF)
			{
				return &UniquecastCmd<CGas2GacOC_Agile_View_Variant_Small_Data>;
			}
			else if (uSize <= 0xFFFF)
			{
				return &UniquecastCmd<CGas2GacOC_Agile_View_Variant_Middle_Data>;
			}
			else
			{
				return &UniquecastCmd<CGas2GacOC_Agile_View_Variant_Big_Data>;
			}
		}
	}
}


template<typename StreamType>
void CCacheVariantServerHolder::SyncStringToAll( const StreamType& str )
{
	CSyncCoreObjectMediator* pMediator = m_pCoreObj->CastToObjMediator();
	CSynConnServer *pExtraConn = pMediator ? pMediator->GetConnection() : NULL;
	
	GetMulticastCmdFunc( str.Size() )(this,str.Data(),str.Size(),pExtraConn);
}

void CCacheVariantServerHolder::SyncToConn(CSynConnServer *pConn)
{
	CCacheVariantServer* pVariant=static_cast<CCacheVariantServer*>(IntGetRootVariant());

	if ( (pVariant->GetType() == eVT_Table) && pVariant->IsEmptyTable() )
		return;

	UpdateStringBufferIfNeeded();
	
	GetUniquecastCmdFunc( m_sInitSyncData.Size() )(this, m_sInitSyncData.Data(),m_sInitSyncData.Size(), pConn);
}

void CCacheVariantServerHolder::IncEncodeTime()
{
	++m_uEncodeTimes;
}

void CCacheVariantServerHolder::IncRealEncodeTime()
{
	++m_uRealEncodeTimes;
}

uint32 CCacheVariantServerHolder::GetRealEncodeTimes()
{
	return m_uRealEncodeTimes;
}


bool CCacheVariantServerHolder::NoTargetToSync()const
{
	CSyncCoreObjectMediator* pMediator = m_pCoreObj->CastToObjMediator();
	CSynConnServer *pExtraConn = pMediator ? pMediator->GetConnection() : NULL;

	return !pExtraConn && m_pMulticast->Empty();
}

void CCacheVariantServerHolder::SyncStreamAndSetFlag()
{
	IncEncodeTime();
	IncRealEncodeTime();

	SyncStringToAll( m_sInitSyncData );
	m_sInitSyncData.Clear();
	m_sInitSyncData.Str("");
}

void CCacheVariantServerHolder::DecodeAndSyncStream( CVariantSyncStream& strm )
{
	Decode(strm);
	IncRealEncodeTime();

	if( !NoTargetToSync() )
	{
		IncEncodeTime();
		SyncStringToAll( strm );
	}

	SetDirty();
}
