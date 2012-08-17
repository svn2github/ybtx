#pragma once

#include <exdispid.h>
#include <olectl.h>
#include <exdisp.h>

struct IHTMLElement;

class CFrameSite;

namespace sqr
{

class ITexture;
class IWebBrowserEventProc;

class CWebBrowser
{
public:
	CWebBrowser(int w,int h,HWND parent=NULL);
	~CWebBrowser(void);

	//SQRWebBrowser USE
	virtual bool Create();
	virtual void UpdateTexture();	
	virtual void SetPostion(int x,int y);
	virtual void OpenURL(const char *);
	virtual void SetVisible(bool vis);

	virtual ITexture* GetTexture();
	virtual int	GetTexWidth();
	virtual int	GetTexHeight();

	bool		IsDocumentComplete();
	char*		GetCurrentURL() ;
	wchar_t*	GetSourceCode();

	//set call back func
	void SetWebEventCallBack(IWebBrowserEventProc*	_callback);

	//WebSiteUse
	virtual bool StopLoading();
	virtual bool Refresh();
	virtual bool Show(bool shown = true);
	
	virtual void AddText(const char *text);
	virtual void Clear();
	virtual void ScrollHTML(int x,int y);

	//Window events
	//IHTMLEvents *GetEvents() {return m_events;}
	virtual void OnSize(int x,int y, int w,int h);

	virtual void OnWindowMouseMsg(UINT msg,WPARAM wp,LPARAM lp);
	virtual void OnWindowKeyboardMsg(UINT msg,WPARAM wp,LPARAM lp);
	
	// InVoke events
	virtual bool OnMouseOver();
	virtual bool OnStartURL(const char *url,bool first);
	virtual void OnFinishURL(const char *url);
	virtual void OnProgressURL(long current, long maximum);
	virtual void OnSetStatusText(const char *text);
	virtual void OnUpdate();
	virtual void OnLink();
	virtual void OffLink();
	virtual void GetHTMLSize(int &wide,int &tall) 
	{
		wide=html_w ; tall=html_h;
	}

private:
	bool CheckIsLink(IHTMLElement *el, char *type);
public:
	//int textureID; // used by the engine code
	
	RECT		SetBounds();
	void		NewWindow(IDispatch **pIDispatch);
	void		SetHTMLSize(IHTMLElement *piElem);
	void		GetSize(int &wide,int &tall);
	HWND		GetHWND();
	HWND		GetIEHWND();
	
	IOleInPlaceActiveObject*	m_oleInPlaceActiveObject;  //处理加速键
protected:
	//Basic
	int					m_ViewWidth;
	int					m_ViewHeight;
	int					m_OffX,m_OffY;
	int					m_LastScreenX,m_LastScreenY;
	long				html_w,html_h;		// html page size
	long				html_x,html_y;		// html page offset
	char				m_currentUrl[512];	// the currently opened URL
	bool				m_specificallyOpened;
	bool				m_cleared;
	bool				m_newpage;
	//COM
	CFrameSite*					m_Site;
	IWebBrowser2*		m_webBrowser;
	IOleObject*			m_oleObject;
	IOleInPlaceObject*	m_oleInPlaceObject;
	IConnectionPoint*	m_connectionPoint;
	ITexture*			m_pTexture;

	//Window
	HWND				m_parent; 
	HWND				m_oleObjectHWND; 
	HWND				m_ieHWND;

	DWORD				m_adviseCookie; // cookies (magic numbers) used to hook into ActiveX events
	DWORD				m_HtmlEventsAdviseCookie;
	
	wchar_t*				m_TextCode;
	bool					m_IsCompleteLoad;
	IWebBrowserEventProc*	m_WebEventCallBack;

	//HOOK 
	DWORD				m_ThreadID;
	bool				m_IsHook;
	HHOOK				m_hHook;
	LPARAM				m_MousePos;
public:
	HHOOK				GetHook();
	LPARAM				GetMousePos();
};

inline void	CWebBrowser::GetSize(int &wide,int &tall) 
{	
	wide=m_ViewWidth ;
	tall=m_ViewHeight;  
}
inline HWND	CWebBrowser::GetHWND()	
{	
	return m_oleObjectHWND;	
}
inline HWND	CWebBrowser::GetIEHWND() 
{	
	return m_ieHWND;
}
inline ITexture* CWebBrowser::GetTexture()
{	
	return m_pTexture;	
}
inline int	CWebBrowser::GetTexWidth()
{
	return m_ViewWidth;	
}
inline int	CWebBrowser::GetTexHeight()
{	
	return m_ViewHeight;	
}
inline bool CWebBrowser::IsDocumentComplete()
{	
	return m_IsCompleteLoad;
}
inline char* CWebBrowser::GetCurrentURL() 
{	
	return m_currentUrl;	
}
inline wchar_t*	CWebBrowser::GetSourceCode()
{	
	return m_TextCode;	
}
//set call back func
inline void CWebBrowser::SetWebEventCallBack(IWebBrowserEventProc*	_callback)
{	
	m_WebEventCallBack=_callback;	
}
inline HHOOK CWebBrowser::GetHook()
{
	return m_hHook;
}
inline LPARAM CWebBrowser::GetMousePos()
{
	return m_MousePos;
}
//!namespace 
}
