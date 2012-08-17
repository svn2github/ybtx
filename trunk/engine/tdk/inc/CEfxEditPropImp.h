#pragma once
#include "CCtrlBaseObserver.h"

class CEditTrack;
class CEditEffectManager;
class CEditEffectProp;

namespace sqr_tools
{
	// 整个特效编辑的核心，根据可编辑特效生成对应控件 
	class CEfxEditPropImp 
		: public CCtrlBaseObserver
	{
		REG_UI_IMP(CEfxEditPropImp);
	public:
		CEfxEditPropImp(void* param);
		~CEfxEditPropImp(void);

		virtual uint32		GetTracksNum();
		virtual void		GetEditTrack( int Num, void** editTrack );
		virtual void*		GetEditEffectProp();

		// over ride
		virtual DWORD	Filter(void);
		virtual void	ActiveNode( CRenderNode* pNode );

		//------------------------------------------------------------------
	private:
		virtual void _InitEditEffectManger();
		virtual void _UpdateCLR();

	private:
		CEditEffectProp*		m_pEditEffectProp;			// 核心
		CEditEffectManager*		m_pEffectManager;
	};
}