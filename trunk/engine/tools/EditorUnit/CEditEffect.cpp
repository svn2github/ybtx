#include "stdafx.h"
#include ".\CEditEffect.h"
#include "CEditEffectManager.h"
#include "BaseHelper.h"
#include <float.h>
#include "CEffect.h"
#include "CModel.h"
#include "Algorithm.h"
#include "StringHelper.h"
#include "CEditEffectBase.h"
#include "CEditUnitPropHelp.h"
#include "CEditEffectProp.h"

CEditEffect::CEditEffect( CEditEffectManager* pEffectMgr, CEffectProp* pEffectProp )
: CEffect(pEffectMgr,pEffectProp)
, m_RenderAble(NULL)
{
	
	((CEditEffectProp*)pEffectProp)->AddEditEffect(this);
}

CEditEffect::~CEditEffect(void)
{
	((CEditEffectProp*)m_pEffectProp)->DelEditEffect(this);
}

CEditEffectProp* CEditEffect::GetEditEffectProp()
{
	if (m_pEffectProp)
	{
		CEditEffectProp* pEditEProp = (CEditEffectProp*)m_pEffectProp;
		return pEditEProp;
	}
	else
		return NULL;
}

void CEditEffect::OnLinked( CRenderable* pParentObj )
{
	m_RenderAble = pParentObj;
	CEffect::OnLinked( pParentObj );
}

void CEditEffect::OnUnlinked(void)
{
	m_RenderAble = NULL;
	CEffect::OnUnlinked();
}

void CEditEffect::OnEffectUintLinked( EFFECTKEY* EditKey )
{
	// 同步链接信息到 effect uint
	Ast(EditKey && "editkey is null!");
	if ( !m_RenderAble )
		return;
	IEffectUnit* pEffectUnit = EditKey->pEffectUnit;
	pEffectUnit->OnLinked( m_RenderAble );
	if( NULL == EditKey->pParentEffectUnit )
	{
		int nSkeletalID = -1;
		int nSocketID = -1;

		IEffectUnitProp* pEffectUnitProp = pEffectUnit->GetProp();
		eUpdatePosType eType = pEffectUnitProp->GetUpdatePosType();

		if( m_PreSkeFrame )
		{
			if( eType == UP_NODEOFFSET || eType == UP_NODE )
				nSkeletalID = m_PreSkeFrame->GetSkeletalIDByName( pEffectUnitProp->GetAttachName() );
			else if( eType == UP_SOCKETOFFSET || eType == UP_SOCKET )
				m_PreSkeFrame->GetIDBySocketName( nSkeletalID, nSocketID, pEffectUnitProp->GetAttachName() );
		}

		pEffectUnit->SetBindID((int16)nSkeletalID,(uint8)nSocketID);
	}
}

float	 CEditEffect::GetCurFrame()
{
	return m_fCurTime;
}

const AniResult CEditEffect::Render( const CMatrix& matWorld, uint32 uCurTime,  RenderObjStyle* pRORS)
{
	if (m_pEffectProp)
	{
		CEffect::UpdateTime(uCurTime);
		return CEffect::Render(matWorld,pRORS);
	}
	else
	{
		AniResult Result( NULL, 0, true, false );
		return Result;
	}
}

void CEditEffect::UpdateLink(void)
{
	m_PreSkeFrame = NULL;
	CEffect::OnLinked(m_pModel);
}

CEffect::EFFECTKEY& CEditEffect::GetEffectKey(int ln,int pos)
{
	return m_Tracks[ln][pos];
}

EMap< int, CEffect::EFFECTKEY >& CEditEffect::GetEffectKeys(int ln)
{
	return m_Tracks[ln];
}

void CEditEffect::DeleteEffectTrack(int ln)
{
	EVector<EMap< int, EFFECTKEY >>::iterator iter = m_Tracks.begin() + ln;
	m_Tracks.erase(iter);
}

void CEditEffect::CopyTrackBindInfo(int source,int dest)
{
	return;
	for (EMap< int, EFFECTKEY >::const_iterator iter = m_Tracks[source].begin();iter != m_Tracks[source].end();++iter)
	{
		EFFECTKEY& destKey = m_Tracks[dest][iter->first];
		destKey.pEffectUnit->OnLinked(m_pModel);
		destKey.pEffectUnit->SetBindID(iter->second.pEffectUnit->GetBindSkeletalId(),iter->second.pEffectUnit->GetBindSocketId());
	}
}

void CEditEffect::SetUnitVisibleOnRender(int track,int key)
{
	IEffectUnit* pEffectUnitRender = m_Tracks[track][key].pEffectUnit;
	IEffectUnitProp* pEffectUnitProp = pEffectUnitRender->GetProp();
	CEditUnitPropHelp* pPropHelp = ((CEditEffectManager*)m_pEffectMgr)->EffectUnitPropCast_ToPropHelp(pEffectUnitProp);
	pEffectUnitRender->SetVisible(pPropHelp->GetUnitVisible());
}