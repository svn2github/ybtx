#include "stdafx.h"
#include "CEfxKeyUintCtrlImp.h"
#include <string>
#include "CEditUnitPropHelp.h"
#include "CEditEffectTrack.h"

DEF_UI_IMP(CEfxKeyUintCtrlImp);
CEfxKeyUintCtrlImp::CEfxKeyUintCtrlImp(void* param)
: m_pEditKey(NULL)
{

}

CEfxKeyUintCtrlImp::~CEfxKeyUintCtrlImp(void)
{

}

void CEfxKeyUintCtrlImp::SetEditKeyContent(void* content)
{
	if (content)
	{
		EDITEFFECTKEY* editKey = static_cast<EDITEFFECTKEY*>(content);
		m_pEditKey = editKey;
	}
	else
		m_pEditKey = NULL;
}

void* CEfxKeyUintCtrlImp::GetEditKeyContent()
{
	if (m_pEditKey)
		return   m_pEditKey->GetEffectUnitProp();
	else
		return NULL;
}

std::string CEfxKeyUintCtrlImp::GetEditKeyText()
{
	std::string str1;
	std::string str2;

	if (GetEffectClassName())
		str1 = GetEffectClassName();
	if (GetCustomName())
		str2 = GetCustomName();

	if( !str2.empty() )
		str1 = str1 + " - " + str2;

	return str1;
}

const char* CEfxKeyUintCtrlImp::GetEffectClassName()
{
	if (m_pEditKey)
	{
		CEditUnitPropHelp* pUnit = m_pEditKey->GetEffectUnitProp();
		return pUnit->GetUnitProp()->GetEffectName();
	}
	return NULL;
}

const char* CEfxKeyUintCtrlImp::GetCustomName()
{
	if (m_pEditKey)
	{
		CEditUnitPropHelp* pUnit = m_pEditKey->GetEffectUnitProp();
		return pUnit->GetUnitProp()->GetCustomName();
	}
	return NULL;
}