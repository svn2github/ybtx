#include "stdafx.h"
#include "CStaticAttribMgr.h"
#include "CPropertyCfg.h"
#include "CTxtTableFile.h"
#include "GameCommon.h"
#include "CCfgCalc.inl"

CStaticAttribMgr& CStaticAttribMgr::Inst()
{
	static CStaticAttribMgr _inst;
	return _inst;
}

CStaticAttribMgr::CStaticAttribMgr()
{
}

uint32 CStaticAttribMgr::CalcBasicMaxHealthPoint(EClass eClass,uint32 uLevel)
{
	return CPropertyCfg::GetPropertyCfg( eClass, uLevel)->m_pMaxHP->GetIntValue(uLevel-1);
}

uint32 CStaticAttribMgr::CalcBasicMaxManaPoint(EClass eClass,uint32 uLevel)
{
	return CPropertyCfg::GetPropertyCfg( eClass, uLevel)->m_pMaxMP->GetIntValue(uLevel-1);
}

void CStaticAttribMgr::InitAllClassStaticAttrib()
{
	for( uint32 uClass = 1; uClass < eCL_Count; ++uClass )
	{
		EClass eClass = EClass(uClass);
		if (!CPropertyCfg::IsClassInTable(eClass))
			continue;

		for( uint32 uLevel = 1; uLevel < eMaxLevel + 1; ++uLevel )
		{
			InitBaseAttribs(eClass,uLevel);

			CPropertyCfg* pPropertyCfg = CPropertyCfg::GetPropertyCfg( eClass, uLevel );
			if (!pPropertyCfg)
				continue;

			InitStaticAttribs(eClass, uLevel);
		}
	}
}

CStaticAttribs& CStaticAttribMgr::GetStaticAttribs(EClass eClass, uint32 uLevel)
{
	if (uLevel > eMaxLevel || eClass > eCL_Count)
	{
		ostringstream oss;
		oss << "Level " << uLevel << ", Class " << eClass << " is error" << endl;
		GenErr(oss.str());
	}

	if (!m_bStaticAttribsInited[uLevel][eClass])
	{
		//if (!CPropertyCfg::IsClassInTable(eClass))
		//	GenErr(oss.str());

		//CPropertyCfg* pPropertyCfg = CPropertyCfg::GetPropertyCfg( eClass, uLevel );
		//if (!pPropertyCfg)
		//	GenErr(oss.str());

		InitBaseAttribs(eClass,uLevel);
		InitStaticAttribs(eClass, uLevel);

		m_bStaticAttribsInited[uLevel][eClass] = true;
	}

	return m_aryBasicAttrValue[uLevel][eClass];
}

void CStaticAttribMgr::InitBaseAttribs(EClass eClass, uint32 uLevel)
{
	CStaticAttribs* pBasicAttr = &m_aryBasicAttrValue[uLevel][eClass];
	pBasicAttr->m_MaxComboPoint	= 10;
	pBasicAttr->m_MaxRagePoint		= 100;
	pBasicAttr->m_MaxEnergyPoint	= 100;

	pBasicAttr->m_RPUpdateValue	= 0;
	pBasicAttr->m_EPUpdateValue	= 0;
	pBasicAttr->m_RPProduceRate	= 0;

	pBasicAttr->m_RunSpeed =0;
	pBasicAttr->m_WalkSpeed = 0;

	pBasicAttr->m_Defence					= 0;
	pBasicAttr->m_PhysicalDodgeValue		= 0;
	pBasicAttr->m_ParryValue				= 0;
	pBasicAttr->m_AccuratenessValue		= 0;
	pBasicAttr->m_MagicHitValue			= 0;
	pBasicAttr->m_ResilienceValue			= 0;
	pBasicAttr->m_StrikeResistanceValue			= 0;

	pBasicAttr->m_MaxHealthPoint		=0;
	pBasicAttr->m_MaxManaPoint			= 0;
	pBasicAttr->m_HPUpdateRate			= 0;
	pBasicAttr->m_MPUpdateRate			= 0;
	pBasicAttr->m_RevertPer			= 0;

	pBasicAttr->m_PhysicalDPS				=0;
	pBasicAttr->m_StrikeValue			= 0;
	pBasicAttr->m_StrikeMultiValue			= 0;	

	pBasicAttr->m_MagicDamageValue			= 0;
	pBasicAttr->m_MagicDodgeValue			= 0;

	pBasicAttr->m_NatureResistanceValue		= 0;
	pBasicAttr->m_DestructionResistanceValue	= 0;
	pBasicAttr->m_EvilResistanceValue			= 0;

	pBasicAttr->m_ValidityCoefficient		= 0;
	pBasicAttr->m_NatureSmashValue			= 0;
	pBasicAttr->m_DestructionSmashValue	= 0;
	pBasicAttr->m_EvilSmashValue			= 0;
	pBasicAttr->m_DefenceSmashValue		= 0;

}

void CStaticAttribMgr::InitStaticAttribs(EClass eClass, uint32 uLevel)
{
	CPropertyCfg* pPropertyCfg = CPropertyCfg::GetPropertyCfg( eClass, uLevel );
	if(!pPropertyCfg)
	{
		ostringstream strm;
		strm << eClass << " " << uLevel << endl;
		GenErr("Init StaticAttribute Err ", strm.str()); 
	}

	CStaticAttribs* bav = &m_aryBasicAttrValue[uLevel][eClass];

	if ((eClass > eCL_Npc && eClass < eCL_Special))		// ·ÇÍæ¼Ò
	{
		bav->m_Defence				= pPropertyCfg->m_pDefence->GetIntValue(uLevel-1);
		bav->m_PhysicalDodgeValue	= pPropertyCfg->m_pPhysicalDodge->GetIntValue(uLevel-1);
		bav->m_ParryValue			= pPropertyCfg->m_pParryValue->GetIntValue(uLevel-1);
		bav->m_AccuratenessValue			= pPropertyCfg->m_pAccuratenessValue->GetIntValue(uLevel-1);		
		bav->m_MagicHitValue			= pPropertyCfg->m_pMagicHitValue->GetIntValue(uLevel-1);					
		bav->m_ResilienceValue			= pPropertyCfg->m_pResilienceValue->GetIntValue(uLevel-1);					
		bav->m_StrikeResistanceValue			= pPropertyCfg->m_pStrikeResistanceValue->GetIntValue(uLevel-1);		

	}
	else if((eClass>eCL_NoneClass&&eClass<eCL_Npc))	// Íæ¼Ò
	{
		bav->m_MaxComboPoint	= 10;
		bav->m_MaxRagePoint		= 100;
		bav->m_MaxEnergyPoint	= 100;

		bav->m_RPUpdateValue	= 2;
		bav->m_EPUpdateValue	= 3;
		bav->m_RPProduceRate	= 1;

		bav->m_RunSpeed =3.0f;
		bav->m_WalkSpeed = 1.5f;

		bav->m_Defence					= 0;
		bav->m_PhysicalDodgeValue		= 0;
		bav->m_ParryValue				= 0;
		bav->m_AccuratenessValue		= 0;
		bav->m_MagicHitValue			= 0;
		bav->m_ResilienceValue			= 0;
		bav->m_StrikeResistanceValue	= 0;

	}

	bav->m_MaxHealthPoint		= pPropertyCfg->m_pMaxHP->GetIntValue(uLevel-1);
	bav->m_MaxManaPoint			= pPropertyCfg->m_pMaxMP->GetIntValue(uLevel-1);
	bav->m_HPUpdateRate			= pPropertyCfg->m_pHPUpdateRate;
	bav->m_MPUpdateRate			= pPropertyCfg->m_pMPUpdateRate;
	bav->m_RevertPer			= pPropertyCfg->m_pRevertPer;

	bav->m_PhysicalDPS				= pPropertyCfg->m_pDPS->GetIntValue(uLevel-1);
	bav->m_StrikeValue			= pPropertyCfg->m_pStrikeValue->GetIntValue(uLevel-1);
		bav->m_StrikeMultiValue			= pPropertyCfg->m_pStrikeMultiValue->GetIntValue(uLevel-1);	

		bav->m_MagicDamageValue			= pPropertyCfg->m_pMagicDamage->GetIntValue(uLevel-1);
	bav->m_MagicDodgeValue			= pPropertyCfg->m_pMagicDodge->GetIntValue(uLevel-1);

		bav->m_NatureResistanceValue		= pPropertyCfg->m_pNatureResistance->GetIntValue(uLevel-1);
		bav->m_DestructionResistanceValue	= pPropertyCfg->m_pDestructionResistance->GetIntValue(uLevel-1);
		bav->m_EvilResistanceValue			= pPropertyCfg->m_pEvilResistance->GetIntValue(uLevel-1);

		bav->m_ValidityCoefficient		= pPropertyCfg->m_pValidityCoefficient;
		bav->m_NatureSmashValue			= pPropertyCfg->m_NatureSmashValue->GetIntValue(uLevel-1);
		bav->m_DestructionSmashValue	= pPropertyCfg->m_DestructionSmashValue->GetIntValue(uLevel-1);
		bav->m_EvilSmashValue			= pPropertyCfg->m_EvilSmashValue->GetIntValue(uLevel-1);
		bav->m_DefenceSmashValue		= pPropertyCfg->m_DefenceSmashValue->GetIntValue(uLevel-1);
	}
