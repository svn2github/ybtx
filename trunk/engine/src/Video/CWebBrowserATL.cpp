#include "stdafx.h"
#include "CWebBrowserATL.h"
#include "CWebSiteATL.h"
#include "IWebBrowserEventProc.h"

CComModule _Module;

int CWebBrowserATL::ATLRef=0;

CWebBrowserATL::CWebBrowserATL(int w,int h,HWND parent)
{
	m_Parent=parent;
	m_Width=w;
	m_Height=h;

	if(ATLRef==0)
	{
		GUID guid;
		_Module.Init(NULL,::GetModuleHandle(NULL),&guid);
	}
	ATLRef++;

	m_AtlHwnd=NULL;
	m_AtlWin=NULL;
	pwb=NULL;
	m_TextCode=NULL;
	m_WebEventCallBack=NULL;
	m_OffX=0;
	m_OffY=0;
	m_LastScreenX=0;
	m_LastScreenY=0;
}

CWebBrowserATL::~CWebBrowserATL(void)
{
	ATLRef--;	
	if(m_connectionPoint)
	{
		m_connectionPoint->Unadvise(m_adviseCookie);
		m_connectionPoint->Release();
	}
	if(m_Site)
	{
		m_Site->Clear();
		m_Site->Release();
	}
	if(m_AtlWin)
	{
		m_AtlWin->DestroyWindow();
		m_AtlWin=0;
	}
}

bool CWebBrowserATL::Create()
{
	m_Site=new CFrameSiteATL(this);
	m_Site->AddRef();

	RECT rc;
	rc.left=0;
	rc.top=0;
	rc.right=m_Width;
	rc.bottom=m_Height;
	m_AtlWin=new CAxWindow();
	m_AtlHwnd=m_AtlWin->Create(m_Parent,rc,0,WS_POPUP);
	if(!m_AtlHwnd)
		return false;
	//ie
	LPOLESTR pszName=OLESTR("shell.Explorer.2");
	HRESULT hret=m_AtlWin->CreateControl(pszName);
	hret=m_AtlWin->QueryControl(__uuidof(IWebBrowser2),(void**)&pwb);
	if(FAILED(hret))
		return false;
	IConnectionPointContainer * cpContainer=NULL;
	hret=m_AtlWin->QueryControl(__uuidof(IConnectionPointContainer),(void**)&cpContainer);
	if(FAILED(hret))
		return false;
	hret = cpContainer->FindConnectionPoint(DIID_DWebBrowserEvents2, &m_connectionPoint);
	if(FAILED(hret))
		return false;
	m_connectionPoint->Advise(m_Site->m_DWebBrowserEvents2, &m_adviseCookie);
	cpContainer->Release();
	return true;
}

void CWebBrowserATL::UpdateTexture()
{
	UpdatePosition();
}

void CWebBrowserATL::SetPostion(int x,int y)
{
	if(m_AtlHwnd && m_Parent)
	{
		m_OffX=x;
		m_OffY=y;
		POINT cp;	cp.x=x;	cp.y=y;
		::ClientToScreen(m_Parent,&cp);
		::SetWindowPos(m_AtlHwnd,HWND_TOP,cp.x,cp.y,m_Width,m_Height,SWP_NOSIZE|SWP_HIDEWINDOW);
		m_LastScreenX=cp.x;
		m_LastScreenY=cp.y;
	}
}

void CWebBrowserATL::UpdatePosition()
{
	if(!m_AtlHwnd)
		return ;
	if(m_Parent)
	{
		POINT cp;	cp.x=m_OffX;	cp.y=m_OffY;
		::ClientToScreen(m_Parent,&cp);
		if(m_LastScreenX!=cp.x || m_LastScreenY!=cp.y)
		{
			::SetWindowPos(m_AtlHwnd,HWND_TOP,cp.x,cp.y,m_Width,m_Height,SWP_NOSIZE|SWP_HIDEWINDOW);
			m_LastScreenX=cp.x;
			m_LastScreenY=cp.y;
		}
	}
}

void CWebBrowserATL::SetVisible(bool vis)
{
	if(!m_AtlHwnd)
		return ;
	HWND m_tmpParent=::GetParent(m_AtlHwnd);
	if(m_tmpParent && ::IsIconic(m_tmpParent))
	{
		ShowWindow(m_AtlHwnd,SW_HIDE);
	}
	else
	{
		if(vis)
			ShowWindow(m_AtlHwnd,SW_NORMAL);
		else
			ShowWindow(m_AtlHwnd,SW_HIDE);
	}
}

void CWebBrowserATL::OpenURL(const char *url)
{
	if (!url || !*url)
		return;
	VARIANTARG navFlag, targetFrame, postData, headers,vUrl;
	navFlag.vt = VT_EMPTY; 
	targetFrame.vt = VT_EMPTY;
	postData.vt = VT_EMPTY;
	headers.vt = VT_EMPTY;
	vUrl.vt=VT_BSTR;
	_bstr_t bstr(url);
	vUrl.bstrVal=bstr;
	HRESULT hret;
	if(pwb)
		hret = pwb->Navigate2(&vUrl, &navFlag, &targetFrame, &postData, &headers); //调用Invoke
}

//回调
bool CWebBrowserATL::OnStartURL(const char *url,bool first)
{ 
	// this is reset for each new page load
	if(m_HtmlEventsAdviseCookie!=0 && m_connectionPoint)
		m_connectionPoint->Unadvise(m_HtmlEventsAdviseCookie);
	strncpy(m_currentUrl,url,512);
	//跳转回调
	if(m_WebEventCallBack)
		m_WebEventCallBack->OnForward();
	return true;
}

void CWebBrowserATL::OnFinishURL(const char * url)
{
	IDispatch* pDisp ;
	pwb->get_Document(&pDisp);

	IHTMLDocument2* pHTMLDocument2;
	HRESULT hret;
	hret = pDisp->QueryInterface( IID_IHTMLDocument2, (void**)&pHTMLDocument2 );
	if (hret == S_OK)
	{	
		IHTMLElement *pElement;
		hret = pHTMLDocument2->get_body(&pElement);
		if (hret == S_OK)
		{	
			pElement->get_outerHTML(&m_TextCode);
			pElement->Release();
		}
		pHTMLDocument2->Release();
	}
	pDisp->Release();
	if(m_WebEventCallBack)
		m_WebEventCallBack->OnDocumentComplete();
}