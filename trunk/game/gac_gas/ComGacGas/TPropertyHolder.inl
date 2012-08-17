#pragma once
#include "TPropertyHolder.h"
#include "CFighterCalInfo.h"
#include "TFighterCtrlInfo.inl"
#include "PropertyAdapter.inl"
#include "FighterProperty.inl"
#include "BaseHelper.h"
#include "TFighterCtrlInfo_inl.inl"
#include "ErrLogHelper.h"

template<typename ImpClass>
map<string, typename TPropertyHolder<ImpClass>::Adapter_t*> TPropertyHolder<ImpClass>::ms_mapAdapter;


template<typename ImpClass>
TPropertyHolder<ImpClass>::TPropertyHolder(void)
{
}

template<typename ImpClass>
TPropertyHolder<ImpClass>::~TPropertyHolder(void)
{
}

template<typename ImpClass>
map<string,uint32>& TPropertyHolder<ImpClass>::GetMapName2Index()
{
	static map<string,uint32> l_Inst;
	return l_Inst;
}

template<typename ImpClass>
map<string,typename TPropertyHolder<ImpClass>::Adapter_t*>& TPropertyHolder<ImpClass>::GetMapAdapter()
{
	return ms_mapAdapter;
}


template<typename ImpClass>
vector<string>& TPropertyHolder<ImpClass>::GetVecName()
{
	static vector<string> l_Inst;
	return l_Inst;
}

template<typename ImpClass>
vector<typename TPropertyHolder<ImpClass>::Adapter_t*>& TPropertyHolder<ImpClass>::GetVecAdapter()
{
	static vector<Adapter_t*> l_Inst;
	return l_Inst;
}

template<typename ImpClass>
void TPropertyHolder<ImpClass>::ReserveVectors()
{
	GetVecName().resize(ePID_Count * ePFT_Count);
	GetVecAdapter().resize(ePID_Count * ePFT_Count);
}

template<typename ImpClass>
void TPropertyHolder<ImpClass>::PrepareAgileProperties()
{	
	typedef typename ImpClass::AgileInfo_t	AgileInfo_t;

#define CreateAdaptersForAgileProperty( Name ) \
	CreateAdapter<TImpAdderAdapter<AgileInfo_t,typename AgileInfo_t::Name##_t,&AgileInfo_t::m_##Name> >(ms_mapAdapter, GetVecAdapter(), GetVecName(), ePID_##Name * ePFT_Count + ePFT_Adder);\
	CreateAdapter<TImpMultiplierAdapter<AgileInfo_t,typename AgileInfo_t::Name##_t,&AgileInfo_t::m_##Name> >(ms_mapAdapter, GetVecAdapter(), GetVecName(), ePID_##Name * ePFT_Count + ePFT_Multiplier);\
	CreateAdapter<TImpValueAdapter<AgileInfo_t,typename AgileInfo_t::Name##_t,&AgileInfo_t::m_##Name> >(ms_mapAdapter, GetVecAdapter(), GetVecName(), ePID_##Name * ePFT_Count + ePFT_Value);\
	CreateAdapter<TImpAgileValueAdapter<AgileInfo_t,typename AgileInfo_t::Name##_t,&AgileInfo_t::m_##Name> >(ms_mapAdapter, GetVecAdapter(), GetVecName(),  ePID_##Name * ePFT_Count + ePFT_Agile)

	CreateAdaptersForAgileProperty(HealthPoint);
	CreateAdaptersForAgileProperty(ManaPoint);
	CreateAdaptersForAgileProperty(ComboPoint);
	CreateAdaptersForAgileProperty(EnergyPoint);
	CreateAdaptersForAgileProperty(RagePoint);

#undef CreateAdaptersForAgileProperty
}

template<typename ImpClass>
void TPropertyHolder<ImpClass>::UnloadAdapterMap()
{
	ClearMap(ms_mapAdapter);
}

template<typename ImpClass>
typename TPropertyHolder<ImpClass>::Adapter_t*
TPropertyHolder<ImpClass>::GetAdapter(uint32 uIndex)
{
	if( uIndex > GetVecAdapter().size() )
		GenErr("Property index out of bound.");
	return GetVecAdapter()[uIndex];
}


template<typename ImpClass>
const TCHAR* TPropertyHolder<ImpClass>::GetPropertyName(uint32 uIndex)
{
	if( uIndex > GetVecName().size() )
		GenErr("Property index out of bound.");
	return GetVecName()[uIndex].c_str();
}

template<typename ImpClass>
uint32 TPropertyHolder<ImpClass>::GetPropertyIndex(const TCHAR* szName)
{
	map<string,uint32>::iterator it=GetMapName2Index().find(szName);
	if( it == GetMapName2Index().end() )
	{
		ostringstream strm;
		strm<<"Invalid property name "<<szName;
		GenErr(strm.str());
	}
	return it->second;
}

template<typename ImpClass>
float TPropertyHolder<ImpClass>::CppGetPropertyValue(const TCHAR* szName)const
{
	SQR_TRY
	{
		return CppGetPropertyValue( GetPropertyIndex(szName) );
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
		return 0.0f;
	}
	SQR_TRY_END;
	return 0.0f;
}

template<typename ImpClass>
float TPropertyHolder<ImpClass>::CppGetPropertyValue(uint32 uIndex)const
{
	SQR_TRY
	{
		typename TPropertyHolder<ImpClass>::Adapter_t* pAdapter = GetAdapter(uIndex);
		if(pAdapter)
		{
			return pAdapter->Get( class_cast<const ImpClass*>(this) );
		}
		else
		{
			ostringstream strm;
			strm<<"Cannot find PropertyHolder Adapter " << uIndex;
			GenErr(strm.str());
		}
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
		return 0.0f;
	}
	SQR_TRY_END;
	return 0.0f;
}

template<typename ImpClass>
void TPropertyHolder<ImpClass>::CppSetPropertyValue(const TCHAR* szName,float fValue)
{
	SQR_TRY
	{
		CppSetPropertyValue( GetPropertyIndex(szName) , fValue );

	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;
}

template<typename ImpClass>
void TPropertyHolder<ImpClass>::CppSetPropertyValue(uint32 uIndex,float fValue)
{
	//cout << "SetProperty\t" << GetPropertyName(uIndex) << "\tValue\t" << fValue << endl;
	SQR_TRY
	{
		GetAdapter(uIndex)->Set(class_cast<ImpClass*>(this), fValue);
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;
}

template<typename ImpClass>
void TPropertyHolder<ImpClass>::CppChangePropertyValue(const TCHAR* szName,float fDiff)
{
	SQR_TRY
	{
		CppChangePropertyValue( GetPropertyIndex(szName), fDiff );
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;

}

template<typename ImpClass>
void TPropertyHolder<ImpClass>::CppChangePropertyValue(uint32 uIndex,float fDiff)
{
	//cout << "ChangeProperty\t" << GetPropertyName(uIndex)  << "\tValue\t" << fDiff << endl;
	SQR_TRY
	{
		float fCurValue = GetAdapter(uIndex)->Get( class_cast<const ImpClass*>(this) );
		GetAdapter(uIndex)->Set(class_cast<ImpClass*>(this), fCurValue + fDiff);
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;
}

template<typename ImpClass>
void TPropertyHolder<ImpClass>::CppIncreasePropertyValue(const TCHAR* szName,float fDiff)
{
	SQR_TRY
	{
		CppIncreasePropertyValue( GetPropertyIndex(szName), fDiff );
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;
}

template<typename ImpClass>
void TPropertyHolder<ImpClass>::CppIncreasePropertyValue(uint32 uIndex,float fDiff)
{
	SQR_TRY
	{
		float fCurValue = GetAdapter(uIndex)->Get( class_cast<const ImpClass*>(this) );
		GetAdapter(uIndex)->Set(class_cast<ImpClass*>(this), fCurValue + fDiff);
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;
}

template<typename ImpClass>
void TPropertyHolder<ImpClass>::CppDecreasePropertyValue(const TCHAR* szName,float fDiff)
{
	SQR_TRY
	{
		CppDecreasePropertyValue( GetPropertyIndex(szName), fDiff );
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;
}

template<typename ImpClass>
void TPropertyHolder<ImpClass>::CppDecreasePropertyValue(uint32 uIndex,float fDiff)
{
	SQR_TRY
	{
		float fCurValue = GetAdapter(uIndex)->Get( class_cast<const ImpClass*>(this) );
		GetAdapter(uIndex)->Set(class_cast<ImpClass*>(this), fCurValue + fDiff);
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;
}

template<typename ImpClass>
void TPropertyHolder<ImpClass>::OnAgileValueChanged(uint32 uValueTypeID, float fValue)const
{
}

template<typename ImpClass>
void TPropertyHolder<ImpClass>::OnCalValueChanged(uint32 uValueTypeID, float fValue)const
{
}

template<typename ImpClass>
void TPropertyHolder<ImpClass>::OnCtrlValueChanged(uint32 uValue, uint32 uValueTypeID, bool bSet)const
{
}


template<typename ImpClass>
bool TPropertyHolder<ImpClass>::CppSetCtrlState(EFighterCtrlState eState,bool bSet)
{
	ImpClass* pThis=class_cast<ImpClass*>(this);
	return pThis->IntSetCtrlState(pThis,eState,bSet);
}
