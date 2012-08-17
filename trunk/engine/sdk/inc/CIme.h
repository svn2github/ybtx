#pragma once
#include "SQRWnd.h"

namespace sqr
{

class CIme : public virtual CDynamicObject
{
	HWND					m_hWnd;
	bool					m_bEnableIme;   //启用、关闭输入法
	HIMC					m_hImc;			//输入法上下文
	HKL						m_hCurKL;		//当前输入法
	uint32					m_uCharBuf;		//非标准输入法缓冲
	WND_IMAGE_LIST			m_ImeImage;		//输入法贴图

protected:
	void					SetImeIcon();
	virtual void			OnImeChanged() {};
	CIme( HWND hWnd );
	~CIme(void);

public:
	static void				InitIme( HWND hWnd );
	static void				UnitIme();
	static CIme*&			GetIme();

	LRESULT					ImeProc( uint32 message, WPARAM wParam, LPARAM lParam );
	bool					CorrectCharMsg( UINT& uMsg, WPARAM& wParam );
	void					EnableIme( bool bEnable );
	const WND_IMAGE_LIST*	GetGuiImeImage();
};

}