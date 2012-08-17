#pragma once

class CFindWindow
{
public:
	CFindWindow(void);
public:
	~CFindWindow(void);

	LPCSTR m_classname;            
	HWND m_hWnd;                    

	void FindWin(HWND hwndParent, LPCSTR classname);

private:
	static BOOL CALLBACK FindChildClassHwnd(HWND hwndParent,LPARAM lParam);
};
