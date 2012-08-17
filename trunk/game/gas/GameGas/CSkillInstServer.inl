#pragma once
#include "CSkillInstServer.h"
#include "CPropertyTransaction.h"
#include "CFighterMediator.h"
#include "CCharacterDictator.h"

template<typename Property>
class CPropertyAdderHolder:public CPropertyHolder
{
public:
	CPropertyAdderHolder(Property& property,int32 iValue)
		:m_Property(property)
		,m_Adder(iValue){}
	void SetProperty(int32 iValue)
	{
		m_Adder = iValue;
	}
	int32 GetValue(){return m_Adder;}
	void RevertPropertyValue(CFighterDictator* pFighter)
	{
		m_Property.SetAdder( pFighter,m_Property.GetAdder()-m_Adder );
	}
	void GetValueToStr(ostringstream& str)
	{
		str << m_Adder;
	}
private:
	Property& m_Property;
	int32		 m_Adder;
};

template<typename Property>
class CPropertyMultiplierHolder:public CPropertyHolder
{
public:
	CPropertyMultiplierHolder(Property& property,float fValue)
		:m_Property(property)
		,m_Multiplier(fValue){}
	void SetProperty(float fValue)
	{
		m_Multiplier = fValue;
	}
	float GetValue(){return m_Multiplier;}
	void RevertPropertyValue(CFighterDictator* pFighter)
	{
		m_Property.SetMultiplier( pFighter,m_Property.GetMultiplier()-m_Multiplier );
	}
	void GetValueToStr(ostringstream& str)
	{
		str << m_Multiplier;
	}
private:
	Property& m_Property;
	float	m_Multiplier;
};


template<typename PropertyType>
void CSkillInstServer::SetPropertyAdderChange(PropertyType& pProperty,uint32 uOwnerID,int32 iValue)
{
	if (iValue == 0)
	{
		return;
	}
	typedef CPropertyAdderHolder<PropertyType> CPropertyHolder;
	string PropertyName;
	size_t pos =  string::npos;
#ifdef _WIN32
	PropertyName=typeid(PropertyType).raw_name();
	PropertyName = PropertyName.substr(5,PropertyName.size()-7);
#else
	PropertyName=typeid(PropertyType).name();
   	pos = PropertyName.find("C");
   	PropertyName = PropertyName.substr(pos+1,PropertyName.size()-(pos+1));
#endif
	pos = PropertyName.find("Server");
	if (pos!=string::npos)
		PropertyName = PropertyName.substr(0,pos);
	CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(uOwnerID);
	if (m_bIsTalent&&pCharacter&&pCharacter->CastToCharacterMediator())
	{
		string strPropertyName = PropertyName+"Adder";
		float fValue = float(iValue);
		CFighterMediator* pFighter = pCharacter->GetFighter()->CastToFighterMediator();
		CPropertyTransactionMgr::MapPropertyChange& mapPropertyChange = pFighter->GetPropertyTransactionMgr()->GetPropertyChangeMap();
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
	}

	MapPropertyChange::iterator it = m_mapPropertyAdderChange.find(make_pair(PropertyName,uOwnerID));
	if (it!=m_mapPropertyAdderChange.end())
	{
		CPropertyHolder* pHolder = class_cast<CPropertyHolder*>(it->second);
		int32 Temp = pHolder->GetValue()+iValue;
		if (Temp==0)
		{
			delete pHolder;
			m_mapPropertyAdderChange.erase(it);
		}
		else
		{
			pHolder->SetProperty(Temp);
		}
	} 
	else
	{
		CPropertyHolder* pPropertyHolder = new CPropertyHolder(pProperty,iValue);
		m_mapPropertyAdderChange.insert(make_pair(make_pair(PropertyName,uOwnerID),pPropertyHolder));
	}
}

template<typename PropertyType>
void CSkillInstServer::SetPropertyMultiplierChange(PropertyType& pProperty,uint32 uOwnerID,float fValue)
{
	if (fValue == 0.0f)
	{
		return;
	}
	typedef CPropertyMultiplierHolder<PropertyType> CPropertyHolder;
	string PropertyName;
	size_t pos =  string::npos;
#ifdef _WIN32
	PropertyName=typeid(PropertyType).raw_name();
	PropertyName = PropertyName.substr(5,PropertyName.size()-7);
#else
	PropertyName=typeid(PropertyType).name();
	pos = PropertyName.find("C");
	PropertyName = PropertyName.substr(pos+1,PropertyName.size()-(pos+1));
#endif
	pos = PropertyName.find("Server");
	if (pos!=string::npos)
		PropertyName = PropertyName.substr(0,pos);
	CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(uOwnerID);
	if (m_bIsTalent&&pCharacter&&pCharacter->CastToCharacterMediator())
	{
		string strPropertyName = PropertyName+"Multiplier";
		CFighterMediator* pFighter = pCharacter->GetFighter()->CastToFighterMediator();
		CPropertyTransactionMgr::MapPropertyChange& mapPropertyChange = pFighter->GetPropertyTransactionMgr()->GetPropertyChangeMap();
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
	}
	MapPropertyChange::iterator it = m_mapPropertyMultiplierChange.find(make_pair(PropertyName,uOwnerID));
	if (it!=m_mapPropertyMultiplierChange.end())
	{

		CPropertyHolder* pHolder = class_cast<CPropertyHolder*>(it->second);
		float Temp = pHolder->GetValue()+fValue;
		if (Temp > -1.0e-5 && Temp < 1.0e-5)
		{
			delete pHolder;
			m_mapPropertyMultiplierChange.erase(it);
		}
		else
		{
			pHolder->SetProperty(Temp);
		}
	} 
	else
	{
		CPropertyHolder* pPropertyHolder = new CPropertyHolder(pProperty,fValue);
		m_mapPropertyMultiplierChange.insert(make_pair(make_pair(PropertyName,uOwnerID),pPropertyHolder));
	}
}




