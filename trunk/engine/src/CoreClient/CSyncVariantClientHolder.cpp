#include "stdafx.h"
#include "CSyncVariantClient.h"
#include "CSyncVariantClientHolder.h"
#include "ICypherVariantHolderHandler.h"
#include "TRootVariantPoolMgr.inl"
#include "CCypherVariantHolder.inl"
#include "CCypherVariantCodeTable.h"
#include "CConnClient.h"

CSyncVariantClientHolder::CSyncVariantClientHolder(uint32 uVarDefID, CCypherVariantCodeTable *pCodeTable, bool bForSync, ECodeTableIndex eCodeTableIndex)
:CCypherVariantHolderForDecode(pCodeTable)
,m_uDecodeTimes(0)
,m_uRealDecodeTimes(0)
,m_bForSync(bForSync)
,m_eCodeTableIndex(eCodeTableIndex)
,m_uVarDefID(uVarDefID)
,m_bDecodeBlock(true)
{
	GetRootVariant();
	InitVariantContainer();
}

CSyncVariantClientHolder::~CSyncVariantClientHolder()
{
	if(IntGetRootVariant())
	{
		static_cast<CSyncVariantClient*>(IntGetRootVariant())->RecycleSubTree();
		TRootVariantPoolMgr<CSyncVariantClient>::GetInst()->GetPool(m_uVarDefID, m_bForSync, m_eCodeTableIndex)->Push(IntGetRootVariant());
	}
}

CCypherVariant* CSyncVariantClientHolder::CreateVariant() const
{
	return new CSyncVariantClient;
}

CCypherVariant* CSyncVariantClientHolder::IntAddSubTreeForDecode(CCypherVariant* pRoot, uint32 uSubTreeType, uint32 uSubTreeIndex)
{
	return CCypherVariantHolderForDecode::IntAddSubTreeForDecode<CSyncVariantClient>(pRoot, uSubTreeType, uSubTreeIndex);
}

CSyncVariantClient* CSyncVariantClientHolder::GetRootVariant() const
{
	CSyncVariantClient* pRootVariant = static_cast<CSyncVariantClient*>(IntGetRootVariant());
	if(pRootVariant)
		return pRootVariant;

	//cout << this << " new Root " << (m_bForSync?"sync":"view") << endl;
	pRootVariant = const_cast<CSyncVariantClientHolder*>(this)->CCypherVariantHolder::AcquireRootVariantFromPool<CSyncVariantClient>(TRootVariantPoolMgr<CSyncVariantClient>::GetInst()->GetPool(m_uVarDefID, m_bForSync, m_eCodeTableIndex));
	pRootVariant->ResetDataMember();

	//CSyncVariantClient* pVariant = pRootVariant->GetMember("State");
	//if(pVariant)
	//{
	//	struct PrintCodeName
	//	{
	//		void operator() ( CCypherVariant* pVariant ) const
	//		{
	//			if(pVariant)
	//				cout << pVariant->GetCode()->GetName() << " " << pVariant->GetCode() << endl;
	//		}
	//	};
	//	PrintCodeName func;
	//	cout << "GetRootVariant begin " << endl;
	//	pVariant->ForEachMember(func);
	//	cout << "GetRootVariant end " << endl;
	//}

	return pRootVariant;
}


const char* CSyncVariantClientHolder::GetString( const char* szPathName )const
{
	return CCypherVariantHolderForDecode::GetString( szPathName );
}

template<typename ValueType>
ValueType CSyncVariantClientHolder::GetNumber( const char* szPathName ) const
{
	return CCypherVariantHolderForDecode::GetNumber<ValueType>( szPathName );
}

bool CSyncVariantClientHolder::IsEmptyTable( const char* szPathName )const
{
	return CCypherVariantHolderForDecode::IsEmptyTable( szPathName );
}


uint32 CSyncVariantClientHolder::GetDecodeTimes()const
{
	return m_uDecodeTimes;
}

uint32 CSyncVariantClientHolder::GetRealDecodeTimes()const
{
	return m_uRealDecodeTimes;
}

void CSyncVariantClientHolder::SetRealDecodeTimes(uint32 uChangeTimes)
{
	m_uRealDecodeTimes = uChangeTimes;
}

void CSyncVariantClientHolder::EnableDecode(bool bEnableDecode)
{
	m_bDecodeBlock = bEnableDecode;
}

void CSyncVariantClientHolder::Reset()
{
	ICypherVariantHolderHandler* pHandler = CCypherVariantHolderForDecode::GetHandler();
	if(pHandler)
		pHandler->OnClearTable(IntGetRootVariant());

	m_uDecodeTimes=0;
	static_cast<CSyncVariantClient*>(IntGetRootVariant())->SetEmptyTableForDecode(NULL);
}

void CSyncVariantClientHolder::RecycleSubTree()
{
	static_cast<CSyncVariantClient*>(IntGetRootVariant())->RecycleSubTree();
}

void CSyncVariantClientHolder::LogVariantValueChanged(const char* szPathName, uint32 uMsgLen)
{
	CConnClient::Inst()->LogMsgRecvTrafficForVariantDecode(szPathName, uMsgLen);
}

bool CSyncVariantClientHolder::EnumVariant(const char* path, bool(*callback)(const void* pContext, const char* szName, CCypherVariant& var))const
{
	return CCypherVariantHolderForDecode::EnumMember(path, GetHandler(), callback);
}

template bool CSyncVariantClientHolder::GetNumber<bool>(const char*)const;
template int8 CSyncVariantClientHolder::GetNumber<int8>(const char*)const;
template uint8 CSyncVariantClientHolder::GetNumber<uint8>(const char*)const;
template int16 CSyncVariantClientHolder::GetNumber<int16>(const char*)const;
template uint16 CSyncVariantClientHolder::GetNumber<uint16>(const char*)const;
template int32 CSyncVariantClientHolder::GetNumber<int32>(const char*)const;
template uint32 CSyncVariantClientHolder::GetNumber<uint32>(const char*)const;
template int64 CSyncVariantClientHolder::GetNumber<int64>(const char*)const;
template uint64 CSyncVariantClientHolder::GetNumber<uint64>(const char*)const;
template float CSyncVariantClientHolder::GetNumber<float>(const char*)const;
template double CSyncVariantClientHolder::GetNumber<double>(const char*)const;
