#pragma once
#include "TSingleton.h"

namespace sqr
{
class SQRRootWnd;

class CEditGuiMgr: public Singleton<CEditGuiMgr>
{
	SQRRootWnd *m_pGUIRootWnd;
private:
	void InitLanguageSystem();

public:
	CEditGuiMgr(void);
	~CEditGuiMgr(void);

	///creat gui root wnd
	void CreateGuiRootWnd(RECT rect);
	void CreateDialogFromRes(const char* szWindowNameInResFile);

	void ResizeGuiRootWnd(RECT rect);
	SQRRootWnd* GetGuiRootWnd();

	LRESULT RecieveMsg( uint32 message, WPARAM wParam, LPARAM lParam );
};
}
