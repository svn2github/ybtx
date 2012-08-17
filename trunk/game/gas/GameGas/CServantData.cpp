#include "stdafx.h"
#include "CServantData.h"
#include "CFighterDictator.h"
#include "FighterProperty.inl"

void CServantData::InitServantProperty(CFighterDictator* pServant, CFighterDictator* pMaster)const
{
	uint32 MaxHealthPoint=uint32(m_MaxHealthPoint*pMaster->m_HealthPoint.Get(pMaster));
	uint32 MaxManaPoint=uint32(m_MaxManaPoint*pMaster->m_ManaPoint.Get(pMaster));	
	int32 PhysicalDodgeValue=int32(m_PhysicalDodgeValue*pMaster->m_PhysicalDodgeValue.Get(pMaster));
	uint32 StrikeValue=uint32(m_StrikeValue*pMaster->m_StrikeValue.Get(pMaster));
	int32 MagicDodgeValue=int32(m_MagicDodgeValue*pMaster->m_MagicDodgeValue.Get(pMaster));		
	uint32 MagicHitValue=uint32(m_MagicHitValue*pMaster->m_MagicHitValue.Get(pMaster));		
	int32 Defence=int32(m_Defence*pMaster->m_Defence.Get(pMaster));					
	uint32 PhysicalDPS=uint32(m_PhysicalDPS*pMaster->m_PhysicalDPS.Get(pMaster));			
	uint32 StrikeMultiValue=uint32(m_StrikeMultiValue*pMaster->m_StrikeMultiValue.Get(pMaster));				
	uint32 MagicDamageValue=uint32(m_MagicDamageValue*pMaster->m_MagicDamageValue.Get(pMaster));			
	int32 NatureResistanceValue=int32(m_NatureResistanceValue*pMaster->m_NatureResistanceValue.Get(pMaster));		
	int32 DestructionResistanceValue=int32(m_DestructionResistanceValue*pMaster->m_DestructionResistanceValue.Get(pMaster));
	int32 EvilResistanceValue=int32(m_EvilResistanceValue*pMaster->m_EvilResistanceValue.Get(pMaster));	
	int32 ParryValue=int32(m_ParryValue*pMaster->m_ParryValue.Get(pMaster));	
	int32 AccuratenessValue=int32(m_AccuratenessValue*pMaster->m_AccuratenessValue.Get(pMaster));	
	int32 ResilienceValue=int32(m_ResilienceValue*pMaster->m_ResilienceValue.Get(pMaster));	
	int32 StrikeResistanceValue=int32(m_StrikeResistanceValue*pMaster->m_StrikeResistanceValue.Get(pMaster));	

	pServant->m_HealthPoint.SetAdder(pServant,MaxHealthPoint);
	pServant->m_ManaPoint.SetAdder(pServant,MaxManaPoint);		
	pServant->m_PhysicalDodgeValue.SetAdder(pServant,PhysicalDodgeValue);	
	pServant->m_StrikeValue.SetAdder(pServant,StrikeValue);			
	pServant->m_MagicDodgeValue.SetAdder(pServant,MagicDodgeValue);		
	pServant->m_MagicHitValue.SetAdder(pServant,MagicHitValue);			
	pServant->m_Defence.SetAdder(pServant,Defence);					
	pServant->m_PhysicalDPS.SetAdder(pServant,PhysicalDPS);				
	pServant->m_StrikeMultiValue.SetAdder(pServant,StrikeMultiValue);			
	pServant->m_MagicDamageValue.SetAdder(pServant,MagicDamageValue);			
	pServant->m_NatureResistanceValue.SetAdder(pServant,NatureResistanceValue);		
	pServant->m_DestructionResistanceValue.SetAdder(pServant,DestructionResistanceValue);
	pServant->m_EvilResistanceValue.SetAdder(pServant,EvilResistanceValue);	
	pServant->m_ParryValue.SetAdder(pServant,ParryValue);		
	pServant->m_AccuratenessValue.SetAdder(pServant,AccuratenessValue);
	pServant->m_ResilienceValue.SetAdder(pServant,ResilienceValue);	
	pServant->m_StrikeResistanceValue.SetAdder(pServant,StrikeResistanceValue);
}
