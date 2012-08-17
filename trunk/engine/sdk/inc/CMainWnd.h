#pragma once
#include "CDynamicObject.h"

namespace sqr
{
	class IMainWndHandler;

	class CMainWnd : public virtual CDynamicObject
	{
	private:
		friend class CTestSuiteCoreObjectClient;
		friend class CAppClient;

		IMainWndHandler*	m_pHandler;

		HICON			m_hIconSmall;
		HICON			m_hIconBig;

		HWND			m_hMainWnd;

		bool			m_bActive;
		BOOL			m_bFullWindow;
		float			m_fWndRatio;
		bool			m_bNeedTrackMouseEvent;

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		static ATOM RegisterClass(HINSTANCE hInstance,HICON hBigIcon,HICON hSmallIcon);
		LRESULT OnWndMessage(UINT uMsg,WPARAM wParam,LPARAM lParam);

		CMainWnd( IMainWndHandler* pHandler, HINSTANCE hInstance,int32 nWidth, int32 nHeight,
			const wchar_t* szTitle, const wchar_t* szBigIconFileName, const wchar_t* szSmallIconFileName );
		~CMainWnd();
	public:
		bool	IsActive()const;
		void	Show(bool);
		HWND	GetHwnd()const;
	};
}