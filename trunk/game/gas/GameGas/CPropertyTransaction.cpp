#include "stdafx.h"
#include "CPropertyTransaction.h"
#include "CFighterMediator.h"
#include "ErrLogHelper.h"

CPropertyTransactionMgr::MapBaseProperty CPropertyTransactionMgr::ms_mapBaseProperty;
uint32 CPropertyTransactionMgr::ms_uCount=0;

template<typename ValueType ,const ValueType CStaticAttribs::*OriginValue>
class CBaseValuePropertyHolder:public CBaseValueHolder
{
	float GetBaseValue(CFighterDictator* pFighter)
	{
		const CStaticAttribs& Attribs=
			CStaticAttribMgr::Inst().GetStaticAttribs(pFighter->CppGetClass(),pFighter->CppGetLevel());

		 return float(Attribs.*OriginValue);
	}
};

void CPropertyTransactionMgr::BuildBasePropertyTable()
{
	ms_mapBaseProperty.insert(make_pair("HealthPoint",new CBaseValuePropertyHolder<uint32,&CStaticAttribs::m_MaxHealthPoint>()));
	ms_mapBaseProperty.insert(make_pair("ManaPoint",new CBaseValuePropertyHolder<uint32,&CStaticAttribs::m_MaxManaPoint>()));
	ms_mapBaseProperty.insert(make_pair("HPUpdateRate",new CBaseValuePropertyHolder<float,&CStaticAttribs::m_HPUpdateRate>()));
	ms_mapBaseProperty.insert(make_pair("MPUpdateRate",new CBaseValuePropertyHolder<float,&CStaticAttribs::m_MPUpdateRate>()));
	ms_mapBaseProperty.insert(make_pair("RevertPer",new CBaseValuePropertyHolder<float,&CStaticAttribs::m_RevertPer>()));
	ms_mapBaseProperty.insert(make_pair("Defence",new CBaseValuePropertyHolder<int32,&CStaticAttribs::m_Defence>()));
	ms_mapBaseProperty.insert(make_pair("PhysicalDPS",new CBaseValuePropertyHolder<uint32,&CStaticAttribs::m_PhysicalDPS>()));
	ms_mapBaseProperty.insert(make_pair("StrikeMultiValue",new CBaseValuePropertyHolder<uint32,&CStaticAttribs::m_StrikeMultiValue>()));
	ms_mapBaseProperty.insert(make_pair("StrikeValue",new CBaseValuePropertyHolder<uint32,&CStaticAttribs::m_StrikeValue>()));
	ms_mapBaseProperty.insert(make_pair("PhysicalDodgeValue",new CBaseValuePropertyHolder<int32,&CStaticAttribs::m_PhysicalDodgeValue>()));
	ms_mapBaseProperty.insert(make_pair("MagicDodgeValue",new CBaseValuePropertyHolder<int32,&CStaticAttribs::m_MagicDodgeValue>()));
	ms_mapBaseProperty.insert(make_pair("MagicHitValue",new CBaseValuePropertyHolder<uint32,&CStaticAttribs::m_MagicHitValue>()));
	ms_mapBaseProperty.insert(make_pair("ParryValue",new CBaseValuePropertyHolder<int32,&CStaticAttribs::m_ParryValue>()));
	ms_mapBaseProperty.insert(make_pair("ResilienceValue",new CBaseValuePropertyHolder<int32,&CStaticAttribs::m_ResilienceValue>()));
	ms_mapBaseProperty.insert(make_pair("StrikeResistanceValue",new CBaseValuePropertyHolder<int32,&CStaticAttribs::m_StrikeResistanceValue>()));
	ms_mapBaseProperty.insert(make_pair("AccuratenessValue",new CBaseValuePropertyHolder<uint32,&CStaticAttribs::m_AccuratenessValue>()));
	ms_mapBaseProperty.insert(make_pair("MagicDamageValue",new CBaseValuePropertyHolder<uint32,&CStaticAttribs::m_MagicDamageValue>()));
	ms_mapBaseProperty.insert(make_pair("NatureResistanceValue",new CBaseValuePropertyHolder<int32,&CStaticAttribs::m_NatureResistanceValue>()));
	ms_mapBaseProperty.insert(make_pair("DestructionResistanceValue",new CBaseValuePropertyHolder<int32,&CStaticAttribs::m_DestructionResistanceValue>()));
	ms_mapBaseProperty.insert(make_pair("EvilResistanceValue",new CBaseValuePropertyHolder<int32,&CStaticAttribs::m_EvilResistanceValue>()));
}

void CPropertyTransactionMgr::ReleaseBasePropertyTable()
{
	MapBaseProperty::const_iterator it = ms_mapBaseProperty.begin();
	for (;it != ms_mapBaseProperty.end();++it)
	{
		delete (*it).second;
	}
	ms_mapBaseProperty.clear();
}

CPropertyTransactionMgr::CPropertyTransactionMgr(CFighterMediator* pFighter)
:m_pFighter(pFighter)
{

}

CPropertyTransactionMgr::~CPropertyTransactionMgr()
{
	MapTransaction::iterator it_begin = m_mapTransaction.begin();
	MapTransaction::iterator it_end = m_mapTransaction.end();
	for (;it_begin!=it_end;++it_begin)
	{
		ostringstream strErr; 
		strErr<<"事务ID为："<<it_begin->first<<endl;
		LogErr("装备属性未撤销",strErr.str()+(it_begin->second)->MakeLog());
		delete it_begin->second;
	}
}

uint32 CPropertyTransactionMgr::CreateTransactionID()
{
	uint32 uID = ++ms_uCount;
	m_mapTransaction.insert(make_pair(uID,new CPropertyTransaction(this)));
	return uID;
}

void CPropertyTransactionMgr::CommitTransaction(uint32 uTransactionID)
{
	MapTransaction::iterator it = m_mapTransaction.find(uTransactionID);
	if (it!=m_mapTransaction.end())
	{
		if (!it->second->IsLock())
		{
			it->second->Lock();
		}
		else
		{
			ostringstream strErr; 
			strErr<<"事务ID为："<<uTransactionID<<endl;
			LogErr("装备属性事务提交时已锁定",strErr.str()+(it->second)->MakeLog());
		}
	}
	else
	{
		ostringstream strErr; 
		strErr<<"事务ID为："<<uTransactionID<<endl;
		LogErr("事务不存在",strErr.str());
	}
}

void CPropertyTransactionMgr::EnableTransaction(uint32 uTransactionID)
{
	MapTransaction::iterator it = m_mapTransaction.find(uTransactionID);
	if (it!=m_mapTransaction.end())
	{
		if (it->second->IsLock())
		{
			it->second->ChangeState(eTS_Enable);
		}
		else
		{
			ostringstream strErr; 
			strErr<<"事务ID为："<<uTransactionID<<endl;
			LogErr("修改装备属性状态时事务未锁定",strErr.str()+(it->second)->MakeLog());
		}
	
	}
	else
	{
		ostringstream strErr; 
		strErr<<"事务ID为："<<uTransactionID<<endl;
		LogErr("事务不存在",strErr.str());
	}
}

void CPropertyTransactionMgr::DisableTransaction(uint32 uTransactionID)
{
	MapTransaction::iterator it = m_mapTransaction.find(uTransactionID);
	if (it!=m_mapTransaction.end())
	{
		if (it->second->IsLock())
		{
			it->second->ChangeState(eTS_Disable);
		}
		else
		{
			ostringstream strErr; 
			strErr<<"事务ID为："<<uTransactionID<<endl;
			LogErr("修改装备属性状态时事务未锁定",strErr.str()+(it->second)->MakeLog());
		}
	}
	else
	{
		ostringstream strErr; 
		strErr<<"事务ID为："<<uTransactionID<<endl;
		LogErr("事务不存在",strErr.str());
	}
}

void CPropertyTransactionMgr::DestroyTransaction(uint32 uTransactionID)
{
	MapTransaction::iterator it = m_mapTransaction.find(uTransactionID);
	if (it!=m_mapTransaction.end())
	{
		delete it->second;
		m_mapTransaction.erase(it);
	}
	else
	{
		ostringstream strErr; 
		strErr<<"事务ID为："<<uTransactionID<<endl;
		LogErr("事务不存在",strErr.str());
	}
}

void CPropertyTransactionMgr::ChangeTransactionPropertyValue(uint32 uTransactionID, const string& strPropertyName, float fValue)
{
	MapTransaction::iterator it = m_mapTransaction.find(uTransactionID);
	if (it!=m_mapTransaction.end())
	{
		if (!it->second->IsLock())
		{
			it->second->ChangePropertyValue(strPropertyName,fValue);
		}
		else
		{
			ostringstream strErr; 
			strErr<<"事务ID为："<<uTransactionID<<endl;
			LogErr("修改装备属性时事务已锁定",strErr.str()+(it->second)->MakeLog());
		}
	}
	else
	{
		ostringstream strErr; 
		strErr<<"事务ID为："<<uTransactionID<<endl;
		LogErr("事务不存在",strErr.str());
	}
}

float	CPropertyTransactionMgr::GetPropertyValue(const string& strPropertyName)
{
	float Adder = 0.0f;
	float	Multiplier = 1.0f;
	float BaseValue = 0.0f;
	MapBaseProperty::iterator it_base = ms_mapBaseProperty.find(strPropertyName);
	if (it_base!=ms_mapBaseProperty.end())
	{
		BaseValue = (it_base->second)->GetBaseValue(m_pFighter);
	} 
	MapPropertyChange::iterator it_adder = m_mapPropertyChange.find(strPropertyName+"Adder");
	if (it_adder!=m_mapPropertyChange.end())
	{
		Adder = it_adder->second;
	} 
	CPropertyTransactionMgr::MapPropertyChange::iterator it_multiplier = m_mapPropertyChange.find(strPropertyName+"Multiplier");
	if (it_multiplier!=m_mapPropertyChange.end())
	{
		Multiplier += it_multiplier->second;
	} 
	return (BaseValue+Adder) * Multiplier;
}

CPropertyTransaction::CPropertyTransaction(CPropertyTransactionMgr* pMgr)
:m_eLockState(eTLS_UnLock)
,m_eState(eTS_Ready)
,m_pMgr(pMgr)
{

}

CPropertyTransaction::~CPropertyTransaction()
{
	SetProperty::iterator it_begin = m_setProperty.begin();
	SetProperty::iterator it_end = m_setProperty.end();
	for (;it_begin!=it_end;++it_begin)
	{
		if (eTS_Enable==m_eState ) ChangeValue((*it_begin)->m_strPropertyName,-(*it_begin)->m_fValue);
		delete *it_begin;
	}
}

void CPropertyTransaction::Lock()
{
	m_eLockState = eTLS_Locked;
	ChangeState(eTS_Enable);
}

void CPropertyTransaction::ChangeState(ETransactionState eState)
{
	if (eState == m_eState)
	{
		return;
	}
	m_eState = eState;
	SetProperty::iterator it_begin = m_setProperty.begin();
	SetProperty::iterator it_end = m_setProperty.end();
	for (;it_begin!=it_end;++it_begin)
	{
		float fValue = eTS_Enable==m_eState?(*it_begin)->m_fValue:-(*it_begin)->m_fValue;
		ChangeValue((*it_begin)->m_strPropertyName,fValue);
	}
}

void CPropertyTransaction::ChangePropertyValue(const string& strPropertyName, float fValue)
{
	CRegistPropertyHolder* pHolder =new CRegistPropertyHolder(strPropertyName,fValue);
	m_setProperty.insert(pHolder);
}

string CPropertyTransaction::MakeLog()
{
	ostringstream strErr; 
	SetProperty::iterator it_begin = m_setProperty.begin();
	SetProperty::iterator it_end = m_setProperty.end();
	for (;it_begin!=it_end;++it_begin)
	{
		strErr<<"装备属性名："<<(*it_begin)->m_strPropertyName<<"   数值："<<(*it_begin)->m_fValue<<endl;
	}
	return strErr.str();
}

void CPropertyTransaction::ChangeValue(const string& strPropertyName, float fValue)
{
	CPropertyTransactionMgr::MapPropertyChange& mapPropertyChange = m_pMgr->GetPropertyChangeMap();
	CPropertyTransactionMgr::MapPropertyChange::iterator it = mapPropertyChange.find(strPropertyName);
	if (it!=mapPropertyChange.end())
	{

		float Result = it->second;
		float Temp = Result+fValue;
		if (Temp > -1.0e-5 && Temp < 1.0e-5)
		{
			mapPropertyChange.erase(it);
		}
		else
		{
			it->second=Temp;
		}
	} 
	else
	{
		mapPropertyChange.insert(make_pair(strPropertyName,fValue));
	}
	(class_cast<TPropertyHolder<CFighterDictator>* >(m_pMgr->GetOwner()))->CppChangePropertyValue(strPropertyName.c_str(),fValue);
}
