#pragma once

namespace sqr
{
	template<typename HolderType, void(HolderType::*pFun)() >
	class TTickFunctionHandler;

	/*
		这个类用来给内部测试用的，当设置了为testing client之后，
		会在桌面右下角添加一个程序运行的图标
		然后当有错误发生的时候，这个图标就会变成错误警告状态，持续十秒
		在回复原状态，当用户把鼠标放在这个图标上面时候，
		会显示上次错误发生的时间
	*/
	class CErrNotifyIcon
	{
	public:
		static void Init(HWND hWnd, HINSTANCE hInstance);
		static void Unit();
		static CErrNotifyIcon*& Inst();
		
		void ShowErrIcon(const char* szMsg);
		void ShowAppIcon();

	private:
		CErrNotifyIcon(HWND hWnd, HINSTANCE hInstance);
		~CErrNotifyIcon();
		
		void AddNotifyIcon();
		void DelNotifyIcon();

		void ModifyNotifyIcon();

	private:
		NOTIFYICONDATA*	m_pData;
		HWND			m_hWnd;
		HINSTANCE		m_hInstance;

		HICON			m_hErrIcon;  //出现错误时候的icon
		HICON			m_hAppIcon;  //游戏正常运行时候的icon
		
		typedef TTickFunctionHandler<CErrNotifyIcon,&CErrNotifyIcon::ShowAppIcon> TickFuncHandler_t;
		TickFuncHandler_t* m_pTickShowErrIcon;

	};

}