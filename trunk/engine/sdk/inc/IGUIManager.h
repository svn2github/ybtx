#pragma once
#include "TSingleton.h"
#include "SQRRootWnd.h"
#include "SQRGUIStyles.h"
#include "CRendererBase.h"
namespace sqr
{
	class IGUIManager 
		: public CRendererBase
		, public Singleton<IGUIManager>
		, public SQRGuiMallocObject
	{
	public:
		virtual SQRRootWnd*		GetRootWnd() const			= 0;
		virtual void			ShowWndArea(bool isShow)	= 0;
		virtual bool			isShowWndArea(void)			= 0;
		virtual const char* 	GetGuiPkgPath()				= 0;
		virtual void			SwitchHidenMode()			= 0;
		bool showlog;
	};
}