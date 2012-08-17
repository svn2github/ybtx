#pragma once
#include "CMagicServer.h"
#include "CGlobalObjVarDefCfg.h"
#include "CAoiDimMgr.h"
#include "ErrLogHelper.h"
#include "BaseHelper.h"
#include "CFilterOpServer.h"
#include "CFighterDictator.h"
#include "CCoreSceneServer.h"
#include "CCoreObjectDictator.h"
#include "CSyncVariantServerHolder.h"
#include "CSkillInstServer.h"

template<typename ImpClass>
CCoreObjectDictator* CMagicServer::CreateCoreObj(const CFPos& pos)
{
	CCoreObjectDictator* pObj = NULL;
	if(GetFighter())
	{
		SQR_TRY
		{ 
			pObj = GetFighter()->GetScene()->CreateObjectDictator(pos,this,CGlobalObjVarDefCfg::GetInst()->GetIDByName("Magic"));
		}
		SQR_CATCH(exp)
		{
			LogExp(exp);
			return NULL;
		}
		SQR_TRY_END;
		m_uCoreObjID = pObj->GetGlobalID();
		pObj->EnableFollowerDelay(false);
		SafeDelete(m_pAoiDimMgr);
		m_pAoiDimMgr = new CAoiDimMgr;
		m_pAoiDimMgr->InitAoiType(m_eEyeSightType, m_eSizeType);
		pObj->SetPixelPos(pos);
		CSyncVariantServerHolder* pViewRootVariant = pObj->GetViewVariantHolder(eCTI_Static);
		pViewRootVariant->SetNumber<uint8>("Attribute/Type", eCT_Magic);
		pViewRootVariant->SetNumber<uint8>("Attribute/MagicType", GetMagicType());
		pViewRootVariant->SetNumber("Attribute/CategoryID", class_cast<ImpClass*>(this)->GetCfgSharedPtr()->GetId());
		pViewRootVariant->SetNumber("Attribute/SrcEntityID", GetFighter()->GetEntityID());
		pViewRootVariant->SetNumber("Attribute/TargetEntityID", m_uTargetEntityID);
		pViewRootVariant->SetNumber<uint8>("Attribute/ArrayState", eBAS_OutArray);
		m_pAoiDimMgr->SetSize(1, pObj);
	}
	return pObj;
}

template<typename ImpClass>
void CMagicServer::MakeFilterLimit(FilterLimit& filterLimit)
{
	filterLimit.fFilterRadius=float(class_cast<ImpClass*>(this)->GetCfgSharedPtr()->GetRadius()->GetDblValue(GetFighter(),m_pSkillInst->GetSkillLevel()-1));
	filterLimit.bIsValueMagicOp		= false;
	filterLimit.bTargetAliveCheckIsIgnored=m_pSkillInst->GetTargetAliveCheckIsIgnored();
}
