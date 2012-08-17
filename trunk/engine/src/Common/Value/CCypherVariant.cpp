#include "stdafx.h"
#include "CCypherVariant.h"
#include "CCypherVariantCodeTable.h"
#include "ExpHelper.h"
#include <locale>
#include "TSqrAllocator.inl"
#include "TVariantStream.h"
#include "TVariantStream.inl"

CCypherVariant::CCypherVariant()
:m_pCode(NULL)
,m_pParent(NULL)
,m_pData(NULL)
,m_bDataNotInit(true)
{
	m_eType = eVT_Table;
}

CCypherVariant::~CCypherVariant()
{
	if (m_pParent)
	{
		Ast(m_pParent->IntGetMemberByIndex(m_pCode->GetIndex()) == this);
		(*m_pParent->GetMemberMap())[m_pCode->GetIndex()] = NULL;
	}
	FreeDataMem();
	if(m_pCode && (m_pCode->IsDynCode() || (m_pCode->IsTreeRoot() && m_pCode->IsCloned())))
	{
		delete m_pCode;
	}
}

bool CCypherVariant::IsValid()const
{
	return !m_bDataNotInit;
}

bool CCypherVariant::IsTable()const
{
	return IntIsType(eVT_Table);
}

bool CCypherVariant::IsEmptyTable() const
{
	if( !IntIsType( eVT_Table ) )
		GenErr("CCypherVariant::IsEmptyTable:Type is not a table!");

	return m_pData == NULL;
}

CCypherVariant::MemberMap_t* CCypherVariant::GetMemberMap()
{
	return (MemberMap_t*)m_pData;
}

const CCypherVariant::MemberMap_t* CCypherVariant::GetMemberMap() const
{
	return (const MemberMap_t*)m_pData;
}

void CCypherVariant::SetMemberMap(MemberMap_t* pMemberMap)
{
	m_pData = pMemberMap;
}

CCypherVariant* CCypherVariant::CreateVariant()const
{
	return new CCypherVariant;
}

template<typename ValueType>
void CCypherVariant::WriteNumber(CBaseCypherVariantInputStream& strm)
{
	strm.Write( static_cast<char*>(m_pData) , sizeof(ValueType) );
}

void CCypherVariant::FreeDataMem()
{
	switch( m_eType )
	{
	case eVT_String:
		IntFreeString();
		break;
	case eVT_Table:
		{
			MemberMap_t* pMemberMap = GetMemberMap();
			if (!pMemberMap)
				break;
			for (size_t index = 0; index < pMemberMap->size(); ++index)
			{
				delete (*pMemberMap)[index];
			}
			delete pMemberMap;
			SetMemberMap(NULL);
		}
		break;
	default:
		break;
	}
}

void CCypherVariant::WritePath(CBaseCypherVariantInputStream& strm)
{
	CBaseCypherVariantCode* pCode = m_pCode;
	if (pCode->GetParent())
	{
		strm << pCode->GetName();
		pCode = pCode->GetParent();
		while (pCode->GetParent())
		{
			strm << "/";
			strm << pCode->GetName();
			pCode = pCode->GetParent();
		}
	}
	strm.Write("\0",1);
}

template<typename CodeType>
void CCypherVariant::WriteCypherCode(CBaseCypherVariantInputStream& strm)
{
	//假設little endian
	uint64 uCode = m_pCode->GetID();
	strm.Write( reinterpret_cast<char*>(&uCode),sizeof(CodeType) );
}

void CCypherVariant::WriteCypherPath(CBaseCypherVariantInputStream& strm)
{
	uint32 uCode = class_cast<const CCypherVariantDynCodeForEncode*>(m_pCode)->GetDynamicId();
	strm.Write( reinterpret_cast<char*>(&uCode),sizeof(uint32) );
}

void CCypherVariant::WriteHeaderAndPath(CBaseCypherVariantInputStream& strm,ECypherVariantAction eAction)
{
	CCVCFirstByte Byte;
	Byte.m_uAction=eAction;

	Ast( m_pCode );
	if( m_pCode->IsDynCode())
	{
		Byte.m_uPathType = eCVPT_Path;
		strm.Write( reinterpret_cast<char*>( &Byte ),sizeof( Byte ) );
		WriteCypherPath(strm);
	}
	else
	{
		if( m_pCode->GetID()<=0xff )
		{
			Byte.m_uPathType=eCVPT_Code8;
			strm.Write( reinterpret_cast<char*>( &Byte ),sizeof( Byte) );
			WriteCypherCode<uint8>( strm );
		}
		else if ( m_pCode->GetID()<=0xffff )
		{
			Byte.m_uPathType=eCVPT_Code16;
			strm.Write( reinterpret_cast<char*>( &Byte ),sizeof( Byte) );
			WriteCypherCode<uint16>( strm );
		}
		else 
		{
			Byte.m_uPathType=eCVPT_Code32;
			strm.Write( reinterpret_cast<char*>( &Byte ),sizeof( Byte) );
			WriteCypherCode<uint32>( strm );
		}	
	}
}

void CCypherVariant::Encode(CBaseCypherVariantInputStream& strm)
{
	if (!this)
		return;
	
	if( GetType() == eVT_Table )
	{
		MemberMap_t* pMemberMap = GetMemberMap();

		if( pMemberMap )
		{
			WriteHeaderAndPath( strm,eCVA_Clr_Table );
			for( MemberMap_t::iterator it=pMemberMap->begin(); it!=pMemberMap->end() ; ++it )
				(*it)->DoEncode( strm );
		}
	}
	else
	{
		DoEncode( strm );
	}
}

void CCypherVariant::DoEncode(CBaseCypherVariantInputStream& strm)
{
	if (!this)
		return;

	WriteHeaderAndPath( strm,ECypherVariantAction(GetType()) );

	//写入内容
	switch( GetType() )
	{
	case eVT_Bool:
		WriteNumber<bool>(strm);
		break;
	case eVT_Int8:
		WriteNumber<int8>(strm);
		break;
	case eVT_Uint8:
		WriteNumber<uint8>(strm);
		break;
	case eVT_Int16:
		WriteNumber<int16>(strm);
		break;
	case eVT_Uint16:
		WriteNumber<uint16>(strm);
		break;
	case eVT_Int32:
		WriteNumber<int32>(strm);
		break;
	case eVT_Uint32:
		WriteNumber<uint32>(strm);
		break;
	case eVT_Int64:
		WriteNumber<int64>(strm);
		break;
	case eVT_Uint64:
		WriteNumber<uint64>(strm);
		break;
	case eVT_Float:
		WriteNumber<float>(strm);
		break;
	case eVT_Double:
		WriteNumber<double>(strm);
		break;
	case eVT_String:
		strm<<IntGetString();
		strm.Write("\0",1);
		break;
	case eVT_Table:
		{
			MemberMap_t* pMemberMap = GetMemberMap();
			if( pMemberMap )
			{
				for( MemberMap_t::iterator it=pMemberMap->begin(); it!=pMemberMap->end() ; ++it )
					(*it)->Encode( strm );
			}

		}
		break;
	default:		
		{
			ostringstream strm;
			strm<<"Invalid Variant type:"<<GetType();
			GenErr(strm.str());
		}
	}
}

void CCypherVariant::CheckVariableName(const char* szName) const
{
	Ast( m_pCode );

	if (m_pCode->GetEntity(szName) == NULL)
	{
		ostringstream oss;
		oss << "CodeTable中找不到结点:" << szName;
		GenErr(oss.str().c_str());
	}
}

CCypherVariant* CCypherVariant::GetRoot()const
{
	const CCypherVariant* pRoot = this;
	while (pRoot->m_pParent)
		pRoot = pRoot->m_pParent;

	return const_cast<CCypherVariant*>(pRoot);
}

CCypherVariant* CCypherVariant::GetParent()const
{
	return m_pParent;
}

void CCypherVariant::DelVariant(CCypherVariant* pVariant)
{
	//CBaseCypherVariantCode* pChildCode = const_cast<CBaseCypherVariantCode*>(pVariant->GetCode());

	delete pVariant;

	//if (pChildCode->IsDynCode() || (pChildCode->IsTreeRoot() && pChildCode->IsCloned()))
	//{
	//	//delete code
	//	delete class_cast<CCypherVariantDynCode*>(pChildCode);
	//}
}

bool CCypherVariant::IntDelMember(const char* szName,CBaseCypherVariantInputStream* pStrm)
{
	CCypherVariant* pVariant=GetMember(szName);
	if( !pVariant )
		return false;

	//if(m_pCode->IsTreeRoot())
	//{
	//	int i = 11;
	//}
	
	if( pStrm )
		pVariant->WriteHeaderAndPath( *pStrm , eCVA_Del_Member );

	if(m_pCode->IsTreeRoot() && m_pCode->IsCloned())
	{
		pVariant->RecycleSubTree();
		return true;
	}

	DelVariant(pVariant);
	return true;
}

template<typename ValueType>
ValueType CCypherVariant::GetNumber() const
{
	if ( !IntIsType(TVariantTraits<ValueType>::GetId()) )
	{
		ostringstream strm;
		strm<<"CCypherVariant::GetInteger:Type is not a "<<TVariantTraits<ValueType>::GetName();
		GenErr(strm.str());
	}
	if(m_bDataNotInit)
		GenErr("CCypherVariant::GetNumber: DataUninitialized");

	return *static_cast<const ValueType*>(m_pData);
}

template<typename ValueType>
void CCypherVariant::SetNumber(ValueType Value,CBaseCypherVariantInputStream* pStrm)
{
	if ( !IntIsType(TVariantTraits<ValueType>::GetId()) )
	{
		FreeDataMem();
		IntSetType(TVariantTraits<ValueType>::GetId());
		m_pData= new(m_szNumBuffer)ValueType;
	}
	*static_cast<ValueType*>(m_pData) = Value;
	m_bDataNotInit = false;

	if( !pStrm )
		return;

	Encode( *pStrm );
}

void CCypherVariant::SetEmptyTable(CBaseCypherVariantInputStream* pStrm)
{
	FreeDataMem();
	IntSetType(eVT_Table);
	m_pData=NULL;

	IntSetEmptyTable(pStrm);
}

CCypherVariant* CCypherVariant::SetMember(const char* szName,CBaseCypherVariantInputStream* strm)
{
	if( !IntIsType(eVT_Table) )
		GenErr("CCypherVariant::GetMember:Type is not a table!");

	return IntSetMember(szName, strm);
}

CCypherVariant* CCypherVariant::GetMember(const char* szName)const
{
	if( !IntIsType(eVT_Table) )
		GenErr("CCypherVariant::GetMember:Type is not a table!");

	return IntGetMember(szName);
}

bool CCypherVariant::DelMember(const char* szName,CBaseCypherVariantInputStream* strm)
{
	if( !IntIsType(eVT_Table) )
		GenErr("CCypherVariant::GetMember:Type is not a table!");

	return IntDelMember(szName, strm);
}

void CCypherVariant::SetString(const char* szValue,CBaseCypherVariantInputStream* pStrm)
{
	const size_t stSize=strlen(szValue)+1;
	
	if( !IntIsType(eVT_String) || SqrGetMemSize( m_pData ) < stSize )
	{
		FreeDataMem();
		IntSetType(eVT_String);
		m_pData = SqrNew(stSize, CCypherVariantMallocObject::GetPool());
	}

	strncpy((char*)m_pData,szValue,stSize);
	m_bDataNotInit = false;


	if( !pStrm )
		return;

	Encode( *pStrm );
}

const char* CCypherVariant::GetString()const
{
	if( !IntIsType( eVT_String) )
		GenErr("CCypherVariant::GetString:Type is not a string!");
	if(m_bDataNotInit)
		GenErr("CCypherVariant::GetString: DataUninitialized");

	return static_cast<const char*>(m_pData);
}

void CCypherVariant::SetEmptyTableForDecode(CBaseCypherVariantInputStream* pStrm)
{
	if (m_pCode->IsCloned() && m_pCode->IsTreeRoot())
	{
		class_cast<CCypherVariantTreeRootCodeForDecode*>(m_pCode)->clear();
	}

	if( !pStrm )
		return;

	WriteHeaderAndPath( *pStrm, eCVA_Clr_Table );
}

void CCypherVariant::IntSetEmptyTable(CBaseCypherVariantInputStream* pStrm)
{
	if (m_pCode->IsCloned() && m_pCode->IsTreeRoot())
	{
		class_cast<CCypherVariantTreeRootCodeForEncode*>(m_pCode)->clear();
	}

	if( !pStrm )
		return;

	WriteHeaderAndPath( *pStrm, eCVA_Clr_Table );
}

CCypherVariant* CCypherVariant::InnerSetMember(CBaseCypherVariantCode* pCode, bool bEncode, CBaseCypherVariantInputStream* pStrm)
{
	Ast(pCode);
	CCypherVariant* pVariant = InnerSetMember(pCode);

	if ( pCode->IsTreeRoot() )
	{
		CCypherVariantTreeRootCodeForEncode* pTreeRootCode = class_cast<CCypherVariantTreeRootCodeForEncode*>(pCode);
		if(bEncode)
		{
			pCode = pTreeRootCode->clone(m_pCode);
		}
		else
		{
			pCode = new CCypherVariantTreeRootCodeForDecode(pTreeRootCode->GetID(), pTreeRootCode->GetName(), m_pCode);
			for (CCypherVariantTreeRootCodeForDecode::SubTreeType_t::iterator it = pTreeRootCode->m_vecSubTreeType.begin();
				it != pTreeRootCode->m_vecSubTreeType.end(); ++it)
			{
				class_cast<CCypherVariantTreeRootCodeForDecode*>(pCode)->m_vecSubTreeType.push_back(*it);
			}
			class_cast<CCypherVariantTreeRootCodeForDecode*>(pCode)->m_bIsClone = true;
			class_cast<CCypherVariantTreeRootCodeForDecode*>(pCode)->m_uIndex = pTreeRootCode->m_uIndex;
		}

		//下面的代码以后将用DynCode池来代替
		CBaseCypherVariantCode* pOldCode = pVariant->m_pCode;
		if(pOldCode && pOldCode->IsTreeRoot() && pOldCode->IsCloned())
		{
			delete pOldCode;
		}
	}

	pVariant->m_pCode = pCode;
	uint32 uIndex = pVariant->m_pCode->GetIndex();
	if(GetMemberByIndex(uIndex) != pVariant)
		GenErr("index err");

	if( pStrm )
		pVariant->WriteHeaderAndPath( *pStrm, eCVA_Set_Table );

	return pVariant;
}

CCypherVariant* CCypherVariant::InnerSetMember(const char* szName,bool bEncode,CBaseCypherVariantInputStream* pStrm)
{
	CBaseCypherVariantCode* pCode = m_pCode->GetEntity(szName);
	if( !pCode )
	{
		ostringstream strm;
		strm << "Can't find variant code:" << szName;
		GenErr(strm.str());
	}
	return InnerSetMember(pCode, bEncode, pStrm);
}

CCypherVariant* CCypherVariant::IntSetMember(const char* szName,CBaseCypherVariantInputStream* pStrm)
{
	return InnerSetMember(szName, true, pStrm);
}

CCypherVariant* CCypherVariant::SetMemberForDecode(CBaseCypherVariantCode* pCode, CBaseCypherVariantInputStream* strm)
{
	if( !IntIsType(eVT_Table) )
		GenErr("CCypherVariant::GetMember:Type is not a table!");

	return InnerSetMember(pCode, false, strm);
}

CCypherVariant* CCypherVariant::SetMemberForDecode(const char* szName, CBaseCypherVariantInputStream* strm)
{
	return SetMemberForDecode(m_pCode->GetEntity(szName), strm);
}

CCypherVariant* CCypherVariant::IntGetMember(const char* szName)const
{
	CBaseCypherVariantCode* pCode = m_pCode->GetEntity(szName);
	if(!pCode)
		return NULL;
	CCypherVariant* pVariant = IntGetMemberByIndex(pCode->GetIndex());

	if(pVariant && !pVariant->IsValid())
	{
		return NULL;
	}
	return pVariant;
}

CCypherVariant* CCypherVariant::IntGetMemberByIndex(uint32 uIndex)const
{
	const MemberMap_t* pMemberMap = GetMemberMap();
	if(!pMemberMap)
		return NULL;

	size_t size = pMemberMap->size();
	Ast(size > uIndex);

	return (*pMemberMap)[uIndex];
}

const char* CCypherVariant::GetName() const
{
	return m_pCode->GetName(); 
}

CCypherVariant* CCypherVariant::InnerSetMember(CBaseCypherVariantCode* pCode)
{
	Ast(pCode);

	MemberMap_t* pMemberMap = GetMemberMap();
	if(!pMemberMap){
		pMemberMap = new MemberMap_t;
		SetMemberMap(pMemberMap);
		size_t count = m_pCode->GetChildrenCount();
		pMemberMap->resize(count, NULL);
	}

	size_t index = pCode->GetIndex();
	size_t size = pMemberMap->size();

	if ( size <= index){
		pMemberMap->resize(index+1);
		for (size_t i = size; i <= index; ++i)
			(*pMemberMap)[i] = NULL;
	}

	CCypherVariant* pVariant = IntGetMemberByIndex(uint32(index));
	if(!pVariant)
	{
		pVariant = CreateVariant();
		(*pMemberMap)[index] = pVariant;
	}

	pVariant->m_pParent = this;
	m_bDataNotInit = false;
	return pVariant;
}

CCypherVariant* CCypherVariant::InnerSetMember(const char* szName)
{
	return InnerSetMember(m_pCode->GetEntity(szName));
}

void CCypherVariant::SetMemberWithVariant(CCypherVariant* pVariant)
{
	MemberMap_t* pMemberMap = GetMemberMap();
	if(!pMemberMap)
	{
		pMemberMap = new MemberMap_t;
		SetMemberMap(pMemberMap);
		size_t count = m_pCode->GetChildrenCount();
		pMemberMap->resize(count);
		for (size_t i = 0; i < count; ++i)
		{
			(*pMemberMap)[i] = NULL;
		}
	}

	size_t index = pVariant->GetCode()->GetIndex();
	size_t size = pMemberMap->size();

	if ( size <= index)
	{
		pMemberMap->resize(index+1);
		for(size_t i = size; i <= index; ++i)
			(*pMemberMap)[i] = NULL;
	}

	Ast(IntGetMemberByIndex(uint32(index)) == NULL);

	(*pMemberMap)[index] = pVariant;
	pVariant->m_pParent = this;
	m_bDataNotInit = false;
	pVariant->ResetDataMember();
}

CCypherVariant* CCypherVariant::SetMemberWithCode(CBaseCypherVariantCode* pCode)
{
	Ast(pCode);
	CCypherVariant* pVariant=InnerSetMember(pCode);
	pVariant->m_pCode = pCode;
	uint32 uIndex = pVariant->m_pCode->GetIndex();
	if(IntGetMemberByIndex(uIndex) != pVariant)
		GenErr("index err");

	return pVariant;
}

bool CCypherVariant::IntEnumMember(const void* pContext,
			bool(CallBack)(const void* pContext,const char* szKey,CCypherVariant& Member))const
{
	const MemberMap_t* pMemberMap = GetMemberMap();

	for( size_t index = 0; index < pMemberMap->size(); ++index ){
		CCypherVariant* pVariant = (*pMemberMap)[index];
		if (!pVariant)
			continue;

		if( !CallBack( pContext, pVariant->m_pCode->GetName(), *pVariant ))
			return false;
	}

	return true;
}

//void CCypherVariant::InitAllMember(bool bEncode)
//{
//	if(!m_pCode)
//		return;
//
//	CBaseCypherVariantCode::NameMapType& mapChildrenCode = m_pCode->m_NameMap;
//	CBaseCypherVariantCode::NameMapType::iterator it_cur = mapChildrenCode.begin();
//	CBaseCypherVariantCode::NameMapType::iterator it_end = mapChildrenCode.end();
//	for(; it_cur != it_end; ++it_cur) 
//	{
//		CCypherVariant* pVariant;
//		if(bEncode)
//		{
//			pVariant = IntSetMember(it_cur->first.c_str());
//		}
//		else
//		{
//			pVariant = SetMemberForDecode(it_cur->first.c_str());
//		}
//		pVariant->InitAllMember(bEncode);
//	}
//}

CCypherVariant* CCypherVariant::GetMemberByIndex(uint32 uIndex)const
{
	if( !IntIsType(eVT_Table) )
		GenErr("CCypherVariant::GetMemberByIndex:Type is not a table!");

	const MemberMap_t* pMemberMap = GetMemberMap();
	if(!pMemberMap || uIndex >= pMemberMap->size())
		return NULL;

	return IntGetMemberByIndex(uIndex);
}

void CCypherVariant::ResetDataMember()
{
	switch( m_eType )
	{
	case eVT_Bool:
	case eVT_Int8:
	case eVT_Uint8:
	case eVT_Int16:
	case eVT_Uint16:
	case eVT_Int32:
	case eVT_Uint32:
	case eVT_Int64:
	case eVT_Uint64:
	case eVT_Float:
	case eVT_Double:
	case eVT_String:
		m_bDataNotInit = true;
		break;
	case eVT_Table:
		{
			m_bDataNotInit = true;
			MemberMap_t* pMemberMap = GetMemberMap();
			if (!pMemberMap)
				break;
			for (size_t index = 0; index < pMemberMap->size(); ++index)
			{
				CCypherVariant* pVariant = (*pMemberMap)[index];
				if(!pVariant)
					continue;
				pVariant->ResetDataMember();
			}
		}
		break;
	default:
		break;
	}
}

void CCypherVariant::IntFreeString()
{
	SqrDelete(m_pData, CCypherVariantMallocObject::GetPool());
	m_pData = NULL;
}
char* CCypherVariant::IntGetString()
{
	return static_cast<char*>(m_pData);
}

bool CCypherVariant::EnumMember(const void* pContext,bool(*pFunc)(const void* pContext,const char* szKey,CCypherVariant& Member))const
{
	if( IsEmptyTable() )
		return true;

	return IntEnumMember(pContext, pFunc);
}

template bool CCypherVariant::GetNumber<bool>()const;
template int8 CCypherVariant::GetNumber<int8>()const;
template uint8 CCypherVariant::GetNumber<uint8>()const;
template int16 CCypherVariant::GetNumber<int16>()const;
template uint16 CCypherVariant::GetNumber<uint16>()const;
template int32 CCypherVariant::GetNumber<int32>()const;
template uint32 CCypherVariant::GetNumber<uint32>()const;
template int64 CCypherVariant::GetNumber<int64>()const;
template uint64 CCypherVariant::GetNumber<uint64>()const;
template float CCypherVariant::GetNumber<float>()const;
template double CCypherVariant::GetNumber<double>()const;

template void CCypherVariant::SetNumber<bool>(bool Value,CBaseCypherVariantInputStream*);
template void CCypherVariant::SetNumber<int8>(int8 Value,CBaseCypherVariantInputStream*);
template void CCypherVariant::SetNumber<uint8>(uint8 Value,CBaseCypherVariantInputStream*);
template void CCypherVariant::SetNumber<int16>(int16 Value,CBaseCypherVariantInputStream*);
template void CCypherVariant::SetNumber<uint16>(uint16 Value,CBaseCypherVariantInputStream*);
template void CCypherVariant::SetNumber<int32>(int32 Value,CBaseCypherVariantInputStream*);
template void CCypherVariant::SetNumber<uint32>(uint32 Value,CBaseCypherVariantInputStream*);
template void CCypherVariant::SetNumber<int64>(int64 Value,CBaseCypherVariantInputStream*);
template void CCypherVariant::SetNumber<uint64>(uint64 Value,CBaseCypherVariantInputStream*);
template void CCypherVariant::SetNumber<float>(float Value,CBaseCypherVariantInputStream*);
template void CCypherVariant::SetNumber<double>(double Value,CBaseCypherVariantInputStream*);
