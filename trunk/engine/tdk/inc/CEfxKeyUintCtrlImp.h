#pragma once
#include "CCtrlBase.h"
#include "AutoPropData.h"

class EDITEFFECTKEY;
namespace sqr_tools
{
	class CEditObserver;

	class CEfxKeyUintCtrlImp 
		: public CCtrlBase
	{
		REG_UI_IMP(CEfxKeyUintCtrlImp);
	public:
		CEfxKeyUintCtrlImp(void* param);
		~CEfxKeyUintCtrlImp(void);

	public:
		virtual void			SetEditKeyContent(void* content);
		virtual void*			GetEditKeyContent();
		virtual std::string		GetEditKeyText();
		virtual const char*		GetEffectClassName();
		virtual const char*		GetCustomName();
	private:
		EDITEFFECTKEY*			m_pEditKey;
	};
}