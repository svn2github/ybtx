#include "stdafx.h"
#include "CEfxTrackCtrlImp.h"
#include "CEditEffectTrack.h"
#include "CEditEffectProp.h"
#include "CEditUnitPropHelp.h"

DEF_UI_IMP(CEfxTrackCtrlImp);
CEfxTrackCtrlImp::CEfxTrackCtrlImp(void* param)
: m_pEditTrack(NULL)
, m_pEditEfxProp(NULL)
{

}

CEfxTrackCtrlImp::~CEfxTrackCtrlImp(void)
{

}

void CEfxTrackCtrlImp::SetEditTrack(void* editTrack, void* pEditProp)
{
	m_pEditTrack = (CEditTrack*)editTrack;
	m_pEditEfxProp = (CEditEffectProp*)pEditProp;
}

int32 CEfxTrackCtrlImp::GetKeyCount()
{
	if (m_pEditTrack)
	{
		map<int,EDITEFFECTKEY>& CurTrackKeys = m_pEditTrack->GetEditKeys();
		return CurTrackKeys.size();
	}
	return 0;
}
void* CEfxTrackCtrlImp::GetTrackKey(int32 idx)
{
	int32 pos = GetKeyPos(idx);
	if (pos != -1)
		return (void*)(m_pEditTrack->GetEditKeyPtr(pos));
	else
		return NULL;
}

int32 CEfxTrackCtrlImp::GetKeyPos(int32 idx)
{
	if (m_pEditTrack)
	{
		map<int,EDITEFFECTKEY>& CurTrackKeys = m_pEditTrack->GetEditKeys();
		map<int,EDITEFFECTKEY>::iterator itr = CurTrackKeys.begin();
		int i = 0;
		for (; i < idx && itr != CurTrackKeys.end(); ++i)
			++itr;
		if (i == idx && itr != CurTrackKeys.end())
			return itr->first;
	}
	return -1;
}

void CEfxTrackCtrlImp::GetKeyPosXAndWidth(int& posX, int& iwidth, int idx,  int iLength, float scale)
{
	posX = 0; iwidth = 0;
	if (!m_pEditEfxProp)
		return;
	int32 pos = GetKeyPos(idx);
	EDITEFFECTKEY* editKey = (EDITEFFECTKEY*)GetTrackKey(idx);
	if (!editKey)
		return;	
	CEditUnitPropHelp* pUnit = editKey->GetEffectUnitProp();

	// X POS
	posX  = (int)(pos * scale / m_pEditEfxProp->GetTimeRange() * iLength );
	// width
	iwidth = (int)(pUnit->GetUnitProp()->GetTimeRang() * iLength 
					/ m_pEditEfxProp->GetTimeRange() * scale);

}