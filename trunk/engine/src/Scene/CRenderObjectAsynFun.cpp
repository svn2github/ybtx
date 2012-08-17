#include "stdafx.h"
#include "CRenderObject.h"
#include "CRenderObjectAsynFun.h"


bool FunDoAni::Do(void)
{
	m_pObject->DoAni(Action.c_str(),nPartId,(int32)bLoop,DelayTime,nStartFrame,nEndFrame ,fAniSpeed);
	return true;
}

bool FunShowPiece::Do(void)
{
	m_pObject->ShowPiece(m_szPieceName.c_str(),m_bShow);
	return true;
}

FunAddChild::FunAddChild( CRenderObject* pRenderObject, CRenderSpaceNode* pC,eLinkType lt, const char* szAttachName )
: CRenderObjectFunCommand(pRenderObject), pChild(pC),linkType(lt),szAttachName(const_cast<char*>(szAttachName))
{
}


bool FunAddChild::Do(void)
{
	m_pObject->AddChild( pChild,linkType,szAttachName.c_str() );
	return true;
}

//FunDeleteNoneActEffect::FunDeleteNoneActEffect(CRenderObject* pRenderObject, uint32 mask )
//: CRenderObjectFunCommand(pRenderObject), nMask(mask)
//{
//
//}
//
//bool FunDeleteNoneActEffect::Do(void)
//{
//	m_pObject->DelEffect(nMask);
//	return true;
//}



//bool FunAddEffect::Do(void)
//{
//	m_pObject->AddEffect(m_szFxFile.c_str(),m_szFx.c_str(),m_szClassName.c_str(),m_nPlayTime,m_pHandler);
//	return true;
//}

bool FunSetNextAnimation::Do(void)
{
	m_pObject->SetNextAnimation(m_AniName,m_FmPair,m_bLoop,m_DelayTime,m_AniSpeed);
	return true;
}

bool FunSetAnimateGroup::Do(void)
{
	m_pObject->SetAnimateGroup(m_pAnimationGroup);
	return true;
}

bool FunSetDefaultAniSep::Do(void)
{
	m_pObject->SetDefaultAniSep(m_id,m_DelayTime,m_AniSpeed);
	return true;
}

bool FunUpdateAniGrp::Do(void)
{
	m_pObject->_UpdateAniGrp();
	return true;
}

bool FunSetAniSep::Do(void)
{
	m_pObject->SetAniSep(m_AniName,m_id,m_bLoop,m_DelayTime,m_AniSpeed);
	return true;
}

bool FunSetAniFromSep::Do(void)
{
	m_pObject->SetAniFromSep(m_PartID,m_DelayTime);
	return true;
}