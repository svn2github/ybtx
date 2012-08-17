#include "stdafx.h"
#include "CVariant.h"
#include <locale>
#include "ExpHelper.h"
#include "TSqrAllocator.inl"
//#include "TBaseVariant.inl"

//template class TBaseVariant<CVariant>;

CVariant::CVariant()
:m_pParent( NULL )
,m_eType(eVVT_Table)
,m_pData(NULL)
{
}

CVariant::CVariant(const CVariant& OtVar)
:m_pParent( NULL )
,m_eType(eVVT_Table)
,m_pData(NULL)
{
	Assign(OtVar);
}

CVariant::~CVariant()
{
	if( m_pParent )
	{
		MemberMap_t* pMembers=m_pParent->GetMemberMap();

		pMembers->erase( m_it );
		if( pMembers->empty() )
		{
			delete pMembers;
			SetMemberMap(NULL);
		}
	}

	FreeDataMem();
}

const CVariant& CVariant::operator=(const CVariant& OtVar)
{
	Assign(OtVar);
	return *this;
}

CVariant::MemberMap_t* CVariant::GetMemberMap()
{
	return (MemberMap_t*)m_pData;
}

const CVariant::MemberMap_t* CVariant::GetMemberMap() const
{
	return (const MemberMap_t*)m_pData;
}

void CVariant::SetMemberMap(MemberMap_t* pMemberMap)
{
	m_pData = pMemberMap;
}

void CVariant::IntSetType(EVariantType eType)
{
	m_eType = EValueVariantType(eType);
}

bool CVariant::IntIsType(EVariantType eType) const
{
	return m_eType == EValueVariantType(eType);
}

template<typename NumberType>
void CVariant::AssignNumber(const CVariant& OtVar)
{
	SetNumber<NumberType>(OtVar.GetNumber<NumberType>());
}

bool CVariant::AssignMember(const void* pContext,const char* szKey,CVariant& OtVar)
{
	CVariant* pThis=static_cast<CVariant*>(const_cast<void*>(pContext));
	pThis->SetMember(szKey)->Assign(OtVar);
	return true;
}

void CVariant::Assign(const CVariant& OtVar)
{
	if ( &OtVar == this )
		return;
		
	switch( OtVar.GetType() )
	{
	case eVVT_Bool:
		AssignNumber<bool>(OtVar);
		break;
	case eVVT_Int8:
		AssignNumber<int8>(OtVar);
		break;
	case eVVT_Uint8:
		AssignNumber<uint8>(OtVar);
		break;
	case eVVT_Int16:
		AssignNumber<int16>(OtVar);
		break;
	case eVVT_Uint16:
		AssignNumber<uint16>(OtVar);
		break;
	case eVVT_Int32:
		AssignNumber<int32>(OtVar);
		break;
	case eVVT_Uint32:
		AssignNumber<uint32>(OtVar);
		break;
	case eVVT_Int64:
		AssignNumber<int64>(OtVar);
		break;
	case eVVT_Uint64:
		AssignNumber<uint64>(OtVar);
		break;
	case eVVT_Float:
		AssignNumber<float>(OtVar);
		break;
	case eVVT_Double:
		AssignNumber<double>(OtVar);
		break;
	case eVVT_String:
		SetString(OtVar.GetString());
		break;
	case eVVT_Table:
		SetEmptyTable();
		OtVar.EnumMember( this,&AssignMember );
		break;
	case eVVT_CStr:
		SetCStr(OtVar.GetCStr());
		break;
	default:
		GenErr("Invalid variant type");
	}
}

void CVariant::IntFreeString()
{
	SqrDelete(m_pData, CVariantMallocObject::GetPool());
	m_pData = NULL;
}

void CVariant::FreeDataMem()
{
	switch( m_eType )
	{
	case eVVT_String:
		IntFreeString();
		break;
	case eVVT_Table:
		{
			while( GetMemberMap() )
				delete GetMemberMap()->begin()->second;
		}
		break;
	default:
		break;
	}
}

void CVariant::SetString(const char* szValue)
{
	const size_t stSize=strlen(szValue)+1;
	
	if( !IntIsType(eVT_String) || SqrGetMemSize( m_pData ) < stSize )
	{
		FreeDataMem();
		IntSetType(eVT_String);
		m_pData = SqrNew(stSize, CVariantMallocObject::GetPool());
	}

	strncpy((char*)m_pData,szValue,stSize);
}

const char* CVariant::GetString()const
{
	if( !IntIsType( eVT_String) )
		GenErr("TBaseVariant::GetString:Type is not a string!");

	return static_cast<const char*>(m_pData);
}

CVariant* CVariant::CreateVariant()const
{
	return new CVariant;
}

template <typename ValueType>
void CVariant::SetNumber(ValueType Value)
{
	if ( !IntIsType(TVariantTraits<ValueType>::GetId()) )
	{
		FreeDataMem();
		IntSetType(TVariantTraits<ValueType>::GetId());
		m_pData= new(m_szNumBuffer)ValueType;
	}
	*static_cast<ValueType*>(m_pData) = Value;
}

template <typename ValueType>
ValueType CVariant::GetNumber()const
{
	if ( !IntIsType(TVariantTraits<ValueType>::GetId()) )
	{
		ostringstream strm;
		strm<<"CVariant::GetNumber:Type is not a "<<TVariantTraits<ValueType>::GetName();
		GenErr(strm.str());
	}

	return *static_cast<const ValueType*>(m_pData);
}

template<typename ValueType>
const ValueType* CVariant::GetNumberPtr()const
{
	if ( !IntIsType(TVariantTraits<ValueType>::GetId()) )
	{
		ostringstream strm;
		strm<<"CVariant::GetNumberPtr:Type is not a "<<TVariantTraits<ValueType>::GetName();
		GenErr(strm.str());
	}

	return static_cast<const ValueType*>(m_pData);
}

CVariant* CVariant::IntGetMember(const char* szName)const
{
	//CheckVariableName( szName );

	const MemberMap_t* pMemberMap = GetMemberMap();
	if( !pMemberMap )
		return NULL;

	MemberMap_t::const_iterator it=pMemberMap->find(szName);

	if( it == pMemberMap->end() )
		return NULL;

	return it->second;
}

bool CVariant::IntEnumMember(const void* pContext,bool(*CallBack)(const void* pContext,const char*,CVariant&))const
{
	const MemberMap_t* pMemberMap = GetMemberMap();

	MemberMap_t theMap=*pMemberMap;

	MemberMap_t::iterator it=theMap.begin();
	MemberMap_t::iterator itEnd=theMap.end();

	for( ;it!=itEnd ; ++it )
		if( !CallBack( pContext, it->first.c_str(),*it->second ) )
			return false;

	return true;
}

void CVariant::CheckVariableName(const char* szName) const
{
	if( !szName )
		GenErr("Variable name is NULL!");

	if( *szName == '\0' )
		GenErr("Variable name can't be empty!");

	const locale& lc=locale::classic();

	if( !isalpha(szName[0],lc) )
	{
		ostringstream strm;
		strm<<"Variable must start with alphabetic character,requested name is:\""<<szName<<"\"!";
		GenErr(strm.str());
	}

	const char* p = szName + 1;

	for( ; *p && (isalnum(*p,lc) || *p=='_') ; ++p );

	if( *p != '\0' )
	{
		ostringstream strm;
		strm<<"Variable must be composed by alphabetic,numeric or underline character,requested name is:\""<<szName<<"\"!";
		GenErr(strm.str());
	}
}

CVariant* CVariant::GetMember(const char* szName)const
{
	if( !IntIsType(eVT_Table) )
		GenErr("CVariant::GetMember:Type is not a table!");

	return IntGetMember(szName);
}

CVariant* CVariant::SetMember(const char* szName)
{
	if( !IntIsType(eVT_Table) )
		GenErr("CVariant::GetMember:Type is not a table!");

	return IntSetMember(szName);
}

CVariant* CVariant::IntSetMember(const char* szName)
{
	CVariant* pVariant;

	MemberMap_t* pMemberMap = GetMemberMap();

	if( !pMemberMap )
	{
		pMemberMap = new MemberMap_t;
		SetMemberMap(pMemberMap);
	}

	pair<MemberMap_t::iterator,bool> p=
		pMemberMap->insert( make_pair(szName,static_cast<CVariant*>(NULL)) );

	if( !p.second )
	{
		pVariant=p.first->second;
		pVariant->SetEmptyTable();
	}
	else
	{
		pVariant			=	CreateVariant();
		p.first->second		=	pVariant;
		pVariant->m_pParent	=	this;
		pVariant->m_it		=	p.first;
	}

	return pVariant;
}

bool CVariant::DelMember(const char* szName)
{
	if( !IntIsType(eVT_Table) )
		GenErr("CVariant::GetMember:Type is not a table!");

	return IntDelMember(szName);
}

bool CVariant::IsEmptyTable() const
{
	if( !IntIsType( eVT_Table ) )
		GenErr("TBaseVariant::IsEmptyTable:Type is not a table!");

	return m_pData == NULL;
}

void CVariant::SetEmptyTable()
{
	FreeDataMem();
	IntSetType(eVT_Table);
	m_pData=NULL;
}

bool CVariant::IntDelMember(const char* szName)
{
	MemberMap_t* pMemberMap = GetMemberMap();

	MemberMap_t::iterator it=pMemberMap->find( szName );

	if( it == pMemberMap->end() )
		return false;

	delete it->second;

	return true;
}

CVariant* CVariant::GetRoot()const
{
	const CVariant* pRoot=this;

	while( pRoot->m_pParent )
		pRoot=pRoot->m_pParent;

	return const_cast<CVariant*>(pRoot);
}

CVariant* CVariant::GetParent()const
{
	return m_pParent;
}


const CVariant::MemberMap_t::iterator& CVariant::GetIt()const
{
	return m_it;
}


void CVariant::SetCStr(const char* szCStr)
{
	FreeDataMem();

	m_eType = eVVT_CStr;

	m_pData = const_cast<char*>(szCStr);
}

const char* CVariant::GetCStr() const 
{
	if (GetType() != eVVT_CStr)
	{
		GenErr("CVariant::GetCStr:Type is not a cstr!");
	}
	return static_cast<const char*>(m_pData);
}

void CVariant::SetPtr(const void* pObjectPtr)
{
	FreeDataMem();

	m_eType = eVVT_Ptr;

	m_pData = const_cast<void*>(pObjectPtr);
}

void CVariant::SetPtr(const CBaseObject* pObject)
{
	SetPtr((const void*)pObject);
}

const void* CVariant::GetPtr() const
{
	if (GetType() != eVVT_Ptr)
	{
		GenErr("CVariant::GetObjPtr:Type is not a ObjPtr!");
	}
	return const_cast<const void*>(m_pData);
}

bool CVariant::EnumMember(const void* pContext,bool(*pFunc)(const void* pContext,const char* szKey,CVariant& Member))const
{
	if( IsEmptyTable() )
		return true;

	return IntEnumMember(pContext, pFunc);
}

template void CVariant::SetNumber<bool>(bool Value);
template void CVariant::SetNumber<int8>(int8 Value);
template void CVariant::SetNumber<uint8>(uint8 Value);
template void CVariant::SetNumber<int16>(int16 Value);
template void CVariant::SetNumber<uint16>(uint16 Value);
template void CVariant::SetNumber<int32>(int32 Value);
template void CVariant::SetNumber<uint32>(uint32 Value);
template void CVariant::SetNumber<int64>(int64 Value);
template void CVariant::SetNumber<uint64>(uint64 Value);
template void CVariant::SetNumber<float>(float Value);
template void CVariant::SetNumber<double>(double Value);

template const bool* CVariant::GetNumberPtr<bool>()const;
template const int8* CVariant::GetNumberPtr<int8>()const;
template const uint8* CVariant::GetNumberPtr<uint8>()const;
template const int16* CVariant::GetNumberPtr<int16>()const;
template const uint16* CVariant::GetNumberPtr<uint16>()const;
template const int32* CVariant::GetNumberPtr<int32>()const;
template const uint32* CVariant::GetNumberPtr<uint32>()const;
template const int64* CVariant::GetNumberPtr<int64>()const;
template const uint64* CVariant::GetNumberPtr<uint64>()const;
template const float* CVariant::GetNumberPtr<float>()const;
template const double* CVariant::GetNumberPtr<double>()const;

template bool CVariant::GetNumber<bool>()const;
template int8 CVariant::GetNumber<int8>()const;
template uint8 CVariant::GetNumber<uint8>()const;
template int16 CVariant::GetNumber<int16>()const;
template uint16 CVariant::GetNumber<uint16>()const;
template int32 CVariant::GetNumber<int32>()const;
template uint32 CVariant::GetNumber<uint32>()const;
template int64 CVariant::GetNumber<int64>()const;
template uint64 CVariant::GetNumber<uint64>()const;
template float CVariant::GetNumber<float>()const;
template double CVariant::GetNumber<double>()const;

//template bool TBaseVariant<CVariant>::GetNumber<bool>()const;
//template int8 TBaseVariant<CVariant>::GetNumber<int8>()const;
//template uint8 TBaseVariant<CVariant>::GetNumber<uint8>()const;
//template int16 TBaseVariant<CVariant>::GetNumber<int16>()const;
//template uint16 TBaseVariant<CVariant>::GetNumber<uint16>()const;
//template int32 TBaseVariant<CVariant>::GetNumber<int32>()const;
//template uint32 TBaseVariant<CVariant>::GetNumber<uint32>()const;
//template int64 TBaseVariant<CVariant>::GetNumber<int64>()const;
//template uint64 TBaseVariant<CVariant>::GetNumber<uint64>()const;
//template float TBaseVariant<CVariant>::GetNumber<float>()const;
//template double TBaseVariant<CVariant>::GetNumber<double>()const;
//
//template const bool* TBaseVariant<CVariant>::GetNumberPtr<bool>()const;
//template const int8* TBaseVariant<CVariant>::GetNumberPtr<int8>()const;
//template const uint8* TBaseVariant<CVariant>::GetNumberPtr<uint8>()const;
//template const int16* TBaseVariant<CVariant>::GetNumberPtr<int16>()const;
//template const uint16* TBaseVariant<CVariant>::GetNumberPtr<uint16>()const;
//template const int32* TBaseVariant<CVariant>::GetNumberPtr<int32>()const;
//template const uint32* TBaseVariant<CVariant>::GetNumberPtr<uint32>()const;
//template const int64* TBaseVariant<CVariant>::GetNumberPtr<int64>()const;
//template const uint64* TBaseVariant<CVariant>::GetNumberPtr<uint64>()const;
//template const float* TBaseVariant<CVariant>::GetNumberPtr<float>()const;
//template const double* TBaseVariant<CVariant>::GetNumberPtr<double>()const;
//
//
//template void TBaseVariant<CVariant>::SetNumber<bool>(bool Value, ostream* strm);
//template void TBaseVariant<CVariant>::SetNumber<int8>(int8 Value, ostream* strm);
//template void TBaseVariant<CVariant>::SetNumber<uint8>(uint8 Value, ostream* strm);
//template void TBaseVariant<CVariant>::SetNumber<int16>(int16 Value, ostream* strm);
//template void TBaseVariant<CVariant>::SetNumber<uint16>(uint16 Value, ostream* strm);
//template void TBaseVariant<CVariant>::SetNumber<int32>(int32 Value, ostream* strm);
//template void TBaseVariant<CVariant>::SetNumber<uint32>(uint32 Value, ostream* strm);
//template void TBaseVariant<CVariant>::SetNumber<int64>(int64 Value, ostream* strm);
//template void TBaseVariant<CVariant>::SetNumber<uint64>(uint64 Value, ostream* strm);
//template void TBaseVariant<CVariant>::SetNumber<float>(float Value, ostream* strm);
//template void TBaseVariant<CVariant>::SetNumber<double>(double Value, ostream* strm);
