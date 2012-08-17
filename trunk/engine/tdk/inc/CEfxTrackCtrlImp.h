#pragma once
#include "CCtrlBase.h"

class CEditEffectProp;
class CEditTrack;
class EDITEFFECTKEY;

namespace sqr_tools
{
	class CEditObserver;

	class CEfxTrackCtrlImp 
		: public CCtrlBase
	{
		REG_UI_IMP(CEfxTrackCtrlImp);
	public:
		CEfxTrackCtrlImp(void* param);
		~CEfxTrackCtrlImp(void);

		virtual int32				GetKeyCount();
		virtual void*				GetTrackKey(int32 idx);
		virtual int32				GetKeyPos(int32 idx);
		virtual void				GetKeyPosXAndWidth(int32& posX, int32& iwidth, int32 idx, int32 iLength, float scale = 1.0f);
	public:
		virtual void				SetEditTrack(void* editTrack, void* pEditProp);
	private:
		CEditTrack*			m_pEditTrack;
		CEditEffectProp*	m_pEditEfxProp;
	};
}