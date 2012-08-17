#pragma once

#include <objbase.h>
#include <atlbase.h>
#include <atlwin.h>

class CFrameSiteATL;

namespace sqr
{

class IWebBrowserEventProc;

class CWebBrowserATL
{
public:
	CWebBrowserATL(int w,int h,HWND parent=NULL);
	~CWebBrowserATL(void);
	virtual bool Create();
	virtual void UpdatePosition();	
	virtual void UpdateTexture();	
	virtual void SetPostion(int x,int y);
	virtual void OpenURL(const char *);
	virtual void SetVisible(bool vis);
	virtual char* GetCurrentURL() ;
	virtual wchar_t* GetSourceCode();

	virtual bool OnStartURL(const char *url,bool first);
	virtual void OnFinishURL(const char * url);

private:
	static int		ATLRef;

protected:
	HWND			m_Parent;
	HWND			m_AtlHwnd;
	CAxWindow*		m_AtlWin;
	int				m_Width;
	int				m_Height;
	int				m_OffX;
	int				m_OffY;
	int				m_LastScreenX;
	int				m_LastScreenY;
	
	CFrameSiteATL*		m_Site;
	IWebBrowser2*		pwb;
	IConnectionPoint*	m_connectionPoint;	
	DWORD				m_adviseCookie;
	DWORD				m_HtmlEventsAdviseCookie;
	char				m_currentUrl[512];
	wchar_t*			m_TextCode;

public:
	void SetWebEventCallBack(IWebBrowserEventProc*	_callback);
protected:
	IWebBrowserEventProc*	m_WebEventCallBack;
};

inline void CWebBrowserATL::SetWebEventCallBack(IWebBrowserEventProc*	_callback)
{	
	m_WebEventCallBack=_callback;	
}

inline char* CWebBrowserATL::GetCurrentURL()
{
	return m_currentUrl;
}

inline wchar_t* CWebBrowserATL::GetSourceCode()
{
	return m_TextCode;
}

//!namespace
}