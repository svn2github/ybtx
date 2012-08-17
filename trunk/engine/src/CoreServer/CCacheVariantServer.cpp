#include "stdafx.h"
#include "CCacheVariantServer.h"
#include "CCacheVariantServerHolder.h"
#include "TimeHelper.h"
#include "CCypherVariant.inl"
#include "TVariantStream.inl"

CCacheVariantServer::CCacheVariantServer(CCacheVariantServerHolder *pHolder)
:m_pHolder(pHolder)
{
}

CCacheVariantServer::~CCacheVariantServer(void)
{
}

CCypherVariant* CCacheVariantServer::CreateVariant()const
{
	CCacheVariantServer* pSyncVariantServerImp = new CCacheVariantServer(GetHolder());
	return pSyncVariantServerImp;
}

void CCacheVariantServer::ResetDataMember()
{
	CCacheVariantServer* pParent = GetParent();
	if(pParent)
		m_pHolder = pParent->GetHolder();
	CCypherVariant::ResetDataMember();
}

void CCacheVariantServer::RecycleSubTree()
{
	IntRecycleSubTree<CCacheVariantServer>();
}

//void CCacheVariantServer::InitAllMember()
//{
//	CCypherVariant::InitAllMember(false);
//}

void CCacheVariantServer::SetEmptyTable()
{
	if( GetHolder()->NoTargetToSync() )
	{
		CCypherVariant::SetEmptyTableForDecode( NULL );
	}
	else
	{
		CCypherVariant::SetEmptyTableForDecode(&(GetHolder()->m_sInitSyncData.GetInputStream()));
		GetHolder()->SyncStreamAndSetFlag();
	}

	GetHolder()->SetDirty();
}

bool CCacheVariantServer::IsEmptyTable() const
{
	return CCypherVariant::IsEmptyTable();
}

const char* CCacheVariantServer::GetString() const
{
	return CCypherVariant::GetString();
}

void CCacheVariantServer::SetString(const char* szValue)
{
	if( GetHolder()->NoTargetToSync() )
	{
		CCypherVariant::SetString(szValue, NULL);
	}
	else
	{
		CCypherVariant::SetString(szValue, &(GetHolder()->m_sInitSyncData.GetInputStream()));
		GetHolder()->SyncStreamAndSetFlag();
	}

	GetHolder()->SetDirty();
}


CCacheVariantServer* CCacheVariantServer::SetMember(const char* szName)
{
	CCacheVariantServer *pSyncVariant;

	if( GetHolder()->NoTargetToSync() )
	{
		pSyncVariant = static_cast<CCacheVariantServer*>( CCypherVariant::SetMemberForDecode(szName, NULL) );
	}
	else
	{
		pSyncVariant = static_cast<CCacheVariantServer*>( CCypherVariant::SetMemberForDecode(szName, &(GetHolder()->m_sInitSyncData.GetInputStream()) ) );

		if( pSyncVariant )
			GetHolder()->SyncStreamAndSetFlag();
	}

	if( pSyncVariant )
		GetHolder()->SetDirty();
	
	return pSyncVariant;
}


bool CCacheVariantServer::DelMember(const char* szName)
{
	bool bRet;

	if( GetHolder()->NoTargetToSync() )
	{
		bRet = CCypherVariant::DelMember(szName, NULL);
	}
	else
	{
		bRet = CCypherVariant::DelMember(szName, &(GetHolder()->m_sInitSyncData.GetInputStream()));
		if ( bRet )
			GetHolder()->SyncStreamAndSetFlag();
	}

	if( bRet )
		GetHolder()->SetDirty();

	return bRet;
}

CCacheVariantServer* CCacheVariantServer::GetMember(const char* szName)const
{
	return static_cast<CCacheVariantServer*>( CCypherVariant::GetMember(szName) );
}


template<typename ValueType>
ValueType CCacheVariantServer::GetNumber() const
{
	return CCypherVariant::GetNumber<ValueType>();
}

//ValueType只支持c++内建数值类型
//将当前值设置成一个ValueType数值
template<typename ValueType>
void CCacheVariantServer::SetNumber(ValueType Value)
{
	if( GetHolder()->NoTargetToSync() )
	{
		CCypherVariant::SetNumber(Value, NULL);
	}
	else
	{
		CCypherVariant::SetNumber(Value, &(GetHolder()->m_sInitSyncData.GetInputStream()));
		GetHolder()->SyncStreamAndSetFlag();
	}

	GetHolder()->SetDirty();
}

template bool CCacheVariantServer::GetNumber<bool>()const;
template int8 CCacheVariantServer::GetNumber<int8>()const;
template uint8 CCacheVariantServer::GetNumber<uint8>()const;
template int16 CCacheVariantServer::GetNumber<int16>()const;
template uint16 CCacheVariantServer::GetNumber<uint16>()const;
template int32 CCacheVariantServer::GetNumber<int32>()const;
template uint32 CCacheVariantServer::GetNumber<uint32>()const;
template int64 CCacheVariantServer::GetNumber<int64>()const;
template uint64 CCacheVariantServer::GetNumber<uint64>()const;
template float CCacheVariantServer::GetNumber<float>()const;
template double CCacheVariantServer::GetNumber<double>()const;

template void CCacheVariantServer::SetNumber<bool>(bool Value);
template void CCacheVariantServer::SetNumber<int8>(int8 Value);
template void CCacheVariantServer::SetNumber<uint8>(uint8 Value);
template void CCacheVariantServer::SetNumber<int16>(int16 Value);
template void CCacheVariantServer::SetNumber<uint16>(uint16 Value);
template void CCacheVariantServer::SetNumber<int32>(int32 Value);
template void CCacheVariantServer::SetNumber<uint32>(uint32 Value);
template void CCacheVariantServer::SetNumber<int64>(int64 Value);
template void CCacheVariantServer::SetNumber<uint64>(uint64 Value);
template void CCacheVariantServer::SetNumber<float>(float Value);
template void CCacheVariantServer::SetNumber<double>(double Value);
