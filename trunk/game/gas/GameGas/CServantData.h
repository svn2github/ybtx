#pragma once
#include "TEntityManager.h"
#include "TSingleton.h"
#include "CNpcAIDataMallocObject.h"

class CFighterDictator;
class CServantData
	: public CNpcAIDataMallocObject
{
public:
	CServantData(uint32 uID, const string& sName);
	inline uint32 GetID() const { return m_uID; }
	inline const string& GetName() const { return m_sName; }
	void InitServantProperty(CFighterDictator* pServant, CFighterDictator* pMaster)const;
	float m_MaxHealthPoint;		
	float m_MaxManaPoint;		
	float m_PhysicalDodgeValue;	
	float m_StrikeValue;				
	float m_MagicDodgeValue;	
	float m_MagicHitValue;			
	float m_Defence;					
	float m_ParryValue;
	float	m_AccuratenessValue;								
	float	m_ResilienceValue;		
	float	m_StrikeResistanceValue;	
	float m_PhysicalDPS;				
	float m_StrikeMultiValue;			
	float m_MagicDamageValue;			
	float m_NatureResistanceValue;		
	float m_DestructionResistanceValue;
	float m_EvilResistanceValue;	
private:
	uint32 m_uID;
	string m_sName;
};

class CServantDataMgr 
	: public TEntityManager< const CServantData, TIDIndex< const CServantData >, TNameIndex< const CServantData > >
	, public TSingleton< CServantDataMgr >
	, public CNpcAIDataMallocObject
{
	friend class TSingleton< CServantDataMgr >;
private:
	CServantDataMgr();
	~CServantDataMgr();
public:
	bool LoadConfig( const string& sFileName );
};
