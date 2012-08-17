#include "stdafx.h"
#include "CEfxEditPropImp.h"
#include "CEditEffectManager.h"
#include "CRenderObject.h"
#include "CEditEffectProp.h"
#include "CEditModel.h"

namespace sqr_tools
{
	DEF_UI_IMP(CEfxEditPropImp);
	CEfxEditPropImp::CEfxEditPropImp(void* param)
		: m_pEditEffectProp(NULL)
		, m_pEffectManager(NULL)
	{	
	}

	CEfxEditPropImp::~CEfxEditPropImp(void)
	{
	}

	void CEfxEditPropImp::_InitEditEffectManger()
	{
		m_pEffectManager = static_cast<CEditEffectManager*>(CEditEffectManager::GetInst());
	}

	DWORD CEfxEditPropImp::Filter(void)
	{
		return CRenderObject::st_RenderObjectType;
	}

	void CEfxEditPropImp::_UpdateCLR()
	{
		//¸üÐÂ tree
		CCtrlBase::Update();
	}

	void CEfxEditPropImp::ActiveNode( CRenderNode* pNode )
	{
		if (!CEditEffectManager::HasInst() || !pNode)
			return;
		CRenderObject* pTempRo = static_cast<CRenderObject*>(pNode);
		IEffect* pEfx = (static_cast<CEditModel*>(pTempRo))->GetFirstEffect();
		CEditEffect* pTempEfx = static_cast<CEditEffect*>(pEfx);

		if (pTempEfx)
		{
			CEditEffectProp* pTempEfxPro = pTempEfx->GetEditEffectProp();
			m_pEditEffectProp = pTempEfxPro;
		}
		else
			m_pEditEffectProp = NULL;

		_UpdateCLR();
	}

	uint32 CEfxEditPropImp::GetTracksNum()
	{
		if (m_pEditEffectProp)
			return m_pEditEffectProp->GetTracksNum();	
		else
			return 0;
	}
	
	void CEfxEditPropImp::GetEditTrack( int Num, void** editTrack )
	{
		if (m_pEditEffectProp)
			*editTrack = m_pEditEffectProp->GetEditTrackPtr(Num);
		else
			*editTrack = NULL;
	}
	
	void* CEfxEditPropImp::GetEditEffectProp()
	{
		return m_pEditEffectProp;
	}
}