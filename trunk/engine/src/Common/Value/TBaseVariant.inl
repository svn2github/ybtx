#include "TBaseVariant.h"
#include <locale>
#include "TSqrAllocator.inl"

template <typename ImpVariant>
TBaseVariant<ImpVariant>::TBaseVariant()
:m_pData( NULL )
,m_bDataNotInit(true)
{
}

template <typename ImpVariant>
TBaseVariant<ImpVariant>::TBaseVariant(const TBaseVariant& other)
:m_pData( NULL )
,m_bDataNotInit(true)
{
}

template <typename ImpVariant>
TBaseVariant<ImpVariant>::~TBaseVariant()
{
}

template<typename ImpVariant>
inline bool TBaseVariant<ImpVariant>::IsType(EVariantType eType) const
{
	return static_cast<const ImpVariant*>(this)->IntIsType(eType);
}

template<typename ImpVariant>
inline void TBaseVariant<ImpVariant>::SetType(EVariantType eType)
{
	static_cast<ImpVariant*>(this)->IntSetType(eType);
}

template <typename ImpVariant>
const char* TBaseVariant<ImpVariant>::GetString()const
{
	if( !IsType( eVT_String) )
		GenErr("TBaseVariant::GetString:Type is not a string!");
	if(m_bDataNotInit)
		GenErr("TBaseVariant::GetString: DataUninitialized");

	return static_cast<const char*>(m_pData);
}

template <typename ImpVariant>
char* TBaseVariant<ImpVariant>::IntGetString()
{
	return static_cast<char*>(m_pData);
}

template <typename ImpVariant>
void TBaseVariant<ImpVariant>::IntFreeString()
{
	SqrDelete(m_pData, CVariantMallocObject::GetPool());
	m_pData = NULL;
}


template <typename ImpVariant>
bool TBaseVariant<ImpVariant>::IsTable()const
{
	return IsType(eVT_Table);
}

template <typename ImpVariant>
bool TBaseVariant<ImpVariant>::IsEmptyTable() const
{
	if( !IsType( eVT_Table ) )
		GenErr("TBaseVariant::IsEmptyTable:Type is not a table!");

	return m_pData == NULL;
}

template <typename ImpVariant>
void TBaseVariant<ImpVariant>::SetEmptyTable(ostream* pStrm)
{
	static_cast<ImpVariant*>(this)->FreeDataMem();
	SetType(eVT_Table);
	m_pData=NULL;

	static_cast<ImpVariant*>(this)->IntSetEmptyTable(pStrm);
}

template <typename ImpVariant>
void TBaseVariant<ImpVariant>::SetString(const char* szValue,ostream* pStrm)
{
	const size_t stSize=strlen(szValue)+1;
	
	if( !IsType(eVT_String) || SqrGetMemSize( m_pData ) < stSize )
	{
		static_cast<ImpVariant*>(this)->FreeDataMem();
		SetType(eVT_String);
		m_pData = SqrNew(stSize, CVariantMallocObject::GetPool());
	}

	strncpy((char*)m_pData,szValue,stSize);
	m_bDataNotInit = false;
}

//ValueType只支持c++内建数值类型
//将当前值作为ValueType类型获取其值，如果当前值不是ValueType类型，则产生异常。
template <typename ImpVariant>
template <typename ValueType>
ValueType TBaseVariant<ImpVariant>::GetNumber()const
{
	if ( !IsType(TVariantTraits<ValueType>::GetId()) )
	{
		ostringstream strm;
		strm<<"TBaseVariant::GetInteger:Type is not a "<<TVariantTraits<ValueType>::GetName();
		GenErr(strm.str());
	}
	if(m_bDataNotInit)
		GenErr("TBaseVariant::GetNumber: DataUninitialized");

	return IntGetNumber<ValueType>();
}


//ValueType只支持c++内建数值类型
//将当前值作为ValueType类型获取其值，如果当前值不是ValueType类型，则产生异常。
template <typename ImpVariant>
template <typename ValueType>
const ValueType* TBaseVariant<ImpVariant>::GetNumberPtr()const
{
	if ( !IsType(TVariantTraits<ValueType>::GetId()) )
	{
		ostringstream strm;
		strm<<"TBaseVariant::GetInteger:Type is not a "<<TVariantTraits<ValueType>::GetName();
		GenErr(strm.str());
	}
	if(m_bDataNotInit)
		GenErr("TBaseVariant::GetNumber: DataUninitialized");

	return IntGetNumberPtr<ValueType>();
}

template <typename ImpVariant>
template <typename ValueType>
ValueType TBaseVariant<ImpVariant>::IntGetNumber()const
{
	return *IntGetNumberPtr<ValueType>();
}

template <typename ImpVariant>
template <typename ValueType>
const ValueType* TBaseVariant<ImpVariant>::IntGetNumberPtr()const
{
	return static_cast<const ValueType*>(m_pData);
}

template <typename ImpVariant>
template <typename ValueType>
void TBaseVariant<ImpVariant>::SetNumber(ValueType Value,ostream* pStrm)
{
	if ( !IsType(TVariantTraits<ValueType>::GetId()) )
	{
		static_cast<ImpVariant*>(this)->FreeDataMem();
		SetType(TVariantTraits<ValueType>::GetId());
		m_pData= new(m_szNumBuffer)ValueType;
	}
	*static_cast<ValueType*>(m_pData) = Value;
	m_bDataNotInit = false;
}


template <typename ImpVariant>
ImpVariant* TBaseVariant<ImpVariant>::SetMember(const char* szName,ostream* strm)
{
	if( !IsType(eVT_Table) )
		GenErr("CVariant::GetMember:Type is not a table!");

	return static_cast<ImpVariant*>(this)->IntSetMember(szName, strm);
}

template <typename ImpVariant>
ImpVariant* TBaseVariant<ImpVariant>::GetMember(const char* szName)const
{
	if( !IsType(eVT_Table) )
		GenErr("CVariant::GetMember:Type is not a table!");

	return static_cast<const ImpVariant*>(this)->IntGetMember(szName);
}

template <typename ImpVariant>
bool TBaseVariant<ImpVariant>::DelMember(const char* szName,ostream* strm)
{
	if( !IsType(eVT_Table) )
		GenErr("CVariant::GetMember:Type is not a table!");

	return static_cast<ImpVariant*>(this)->IntDelMember(szName, strm);
}

template <typename ImpVariant>
bool TBaseVariant<ImpVariant>::EnumMember(const void* pContext,bool(*pFunc)(const void* pContext,const char* szKey,ImpVariant& Member))const
{
	if( IsEmptyTable() )
		return true;

	return static_cast<const ImpVariant*>(this)->IntEnumMember(pContext, pFunc);
}
