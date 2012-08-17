#include "stdafx.h"
#include "CWebBrowser.h"
#include "CWebSite.h"
#include "CGraphic.h"
#include "CodeCvs.h"
#include "CFindWindow.h"
#include "IWebBrowserEventProc.h"

#define ASSERT assert

std::vector<CWebBrowser *> html_windows;

LRESULT CALLBACK GetMsgProc( int code, WPARAM wParam, LPARAM lParam )
{
	CWebBrowser *win=NULL;

	MSG *msg = reinterpret_cast<MSG *>(lParam);
	for(unsigned int i=0;i<html_windows.size();i++)
	{
		if(html_windows[i]->GetIEHWND()==msg->hwnd)
		{
			win=html_windows[i];
			break;
		}
	}
	if(msg && win)
	{
		for(unsigned int i=0;i<html_windows.size();i++)
		{
			if(html_windows[i]->m_oleInPlaceActiveObject)
				html_windows[i]->m_oleInPlaceActiveObject->TranslateAcceleratorA(msg);
		}
		return CallNextHookEx(win->GetHook() ,code,wParam,lParam);
	}
	return 0;
}

CWebBrowser::CWebBrowser(int w,int h,HWND parent)
{
	m_parent=parent;
	m_ViewWidth=w;
	m_ViewHeight=h;
	m_IsCompleteLoad=true;
	m_TextCode=NULL;
	m_WebEventCallBack=NULL;
	m_HtmlEventsAdviseCookie=0;
	m_IsHook=false;
	m_hHook=NULL;
	m_oleInPlaceActiveObject=NULL;
}

CWebBrowser::~CWebBrowser(void)
{
	if(m_IsHook && m_hHook!=NULL)
	{
		::UnhookWindowsHookEx(m_hHook);
	}

	if(m_pTexture)
	{
		m_pTexture->Release();
	}
	if(m_oleInPlaceActiveObject)
	{
		m_oleInPlaceActiveObject->Release();
	}
	if(m_oleInPlaceObject)
	{
		m_oleInPlaceObject->InPlaceDeactivate();
		m_oleInPlaceObject->UIDeactivate();
		m_oleInPlaceObject->Release();
	}
	if(m_connectionPoint)
	{
		m_connectionPoint->Unadvise(m_adviseCookie);
		m_connectionPoint->Release();
	}
	if(m_oleObject)
	{
		m_oleObject->Close(OLECLOSE_NOSAVE);  //Stop Doverb
		m_oleObject->SetClientSite(NULL);
		m_oleObject->Release();
	}
	
	if(m_webBrowser)
	{
		m_webBrowser->Quit();
		m_webBrowser->Release();
	}

	if(m_Site)
	{
		m_Site->Clear();
		m_Site->Release();
	}
}

bool CWebBrowser::Create()
{
	HRESULT hret;
	IUnknown *p;
	//创建Web浏览器对象
	hret = CoCreateInstance(CLSID_WebBrowser, NULL, CLSCTX_ALL, IID_IUnknown, (void**)(&p));
	hret = p->QueryInterface(IID_IOleInPlaceObject, (void**)(&m_oleInPlaceObject));	//IOleWindow	控制显示多少 已经OLE容器句柄
	hret = p->QueryInterface(IID_IOleInPlaceActiveObject, (void**)(&m_oleInPlaceActiveObject));	//
	//自己实现的接口 要设置到开始创建的对象中
	m_Site= new CFrameSite(this);
	m_Site->AddRef();
	//获取嵌入对象和对象状态
	hret = p->QueryInterface(IID_IOleObject, (void**)(&m_oleObject));
	DWORD dwMiscStatus;
	m_oleObject->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
	bool m_bSetClientSiteFirst = false;
	if (dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST)
	{
		m_bSetClientSiteFirst = true;
	}
	bool m_bVisibleAtRuntime = true;
	if (dwMiscStatus & OLEMISC_INVISIBLEATRUNTIME)
	{
		m_bVisibleAtRuntime = false;
	}
	//为对象设置容器(该容器为自己的容器)
	if (m_bSetClientSiteFirst) 
		m_oleObject->SetClientSite(m_Site->m_IOleClientSite);
	//获取容器句柄(只有在设置容器后 句柄才存在)
	hret = m_oleInPlaceObject->GetWindow(&m_oleObjectHWND);

	IPersistStreamInit * psInit = NULL;
	hret = p->QueryInterface(IID_IPersistStreamInit, (void**)(&psInit));
	if (SUCCEEDED(hret) && psInit != NULL)
		hret = psInit->InitNew();		//初始化对象到默认状态

	//获取"控制对象显示大小"的对象
	hret = p->QueryInterface(IID_IOleInPlaceObject, (void**)(&m_oleInPlaceObject));
	
	//设置对象显示大小
	RECT posRect;
	posRect.left = 0;
	posRect.top = 0;
	posRect.right = m_ViewWidth;
	posRect.bottom = m_ViewHeight;
	m_oleInPlaceObject->SetObjectRects(&posRect, &posRect);

	//激活嵌入对象
	MSG msg;
	memset(&msg,0,sizeof(msg));
	if (m_bVisibleAtRuntime) 
	{
		hret = m_oleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, &msg,m_Site->m_IOleClientSite, 0, m_parent, &posRect);
		assert(SUCCEEDED(hret));
	}

	hret = m_oleInPlaceObject->GetWindow(&m_oleObjectHWND);
	if (!m_bSetClientSiteFirst) 
		m_oleObject->SetClientSite(m_Site->m_IOleClientSite);

	if(m_oleObjectHWND)
	{
		::ShowWindow(m_oleObjectHWND,SW_HIDE);
		::SetWindowLong(m_oleObjectHWND,GWL_STYLE,WS_POPUP);
		::SetWindowLong(m_oleObjectHWND,GWL_EXSTYLE,WS_EX_TOOLWINDOW);
	}
	
	//开始消息方面的连接与处理
	// Get IWebBrowser2 Interface
	hret = p->QueryInterface(IID_IWebBrowser2, (void**)(&m_webBrowser));
	IConnectionPointContainer * cpContainer;
	hret = p->QueryInterface(IID_IConnectionPointContainer, (void**)(&cpContainer));
	hret = cpContainer->FindConnectionPoint(DIID_DWebBrowserEvents2, &m_connectionPoint);
	m_connectionPoint->Advise(m_Site->m_DWebBrowserEvents2, &m_adviseCookie);
	cpContainer->Release();
	p->Release();
	// setup the webbrowser object
	m_webBrowser->put_MenuBar(VARIANT_FALSE);
	m_webBrowser->put_AddressBar(VARIANT_FALSE);
	m_webBrowser->put_StatusBar(VARIANT_FALSE);
	m_webBrowser->put_ToolBar(VARIANT_FALSE);
	m_webBrowser->put_RegisterAsBrowser(VARIANT_TRUE);
	m_webBrowser->put_RegisterAsDropTarget(VARIANT_TRUE);
	m_webBrowser->put_Silent(VARIANT_TRUE);

	//创建接收纹理
	CGraphic::GetInst()->CreateEmptyTexture(m_ViewWidth,m_ViewHeight,1, TF_RGB24,&m_pTexture);
	return true;
}

void CWebBrowser::SetPostion(int x,int y)
{
	if(m_oleObjectHWND && m_parent)
	{
		m_OffX=x;
		m_OffY=y;
		POINT cp;	cp.x=x;	cp.y=y;
		::ClientToScreen(m_parent,&cp);
		::SetWindowPos(m_oleObjectHWND,HWND_TOP,cp.x,cp.y,m_ViewWidth,m_ViewHeight,SWP_NOSIZE|SWP_HIDEWINDOW);
		m_LastScreenX=cp.x;
		m_LastScreenY=cp.y;
	}

	return ;
}

void CWebBrowser::SetVisible(bool vis)
{
	vis=true;
	if(!m_oleObjectHWND)
		return ;
	if(m_parent && ::IsIconic(m_parent))
	{
		ShowWindow(m_oleObjectHWND,SW_HIDE);
	}
	else
	{
		if(vis)
			ShowWindow(m_oleObjectHWND,SW_NORMAL);
		else
			ShowWindow(m_oleObjectHWND,SW_HIDE);
	}
}

void CWebBrowser::UpdateTexture()
{
	return ;
	if(m_ViewWidth==0 || m_ViewHeight==0) // have loaded a page yet
		return; 
	RECT posRect=SetBounds();
	if (m_webBrowser&& m_pTexture)
	{
		m_webBrowser->put_Height(m_ViewHeight);
		m_webBrowser->put_Width(m_ViewWidth);
		ScrollHTML(html_x,html_y);
		::RedrawWindow(m_oleObjectHWND, NULL, NULL, RDW_INTERNALPAINT);
		::ShowWindow(m_oleObjectHWND,SW_HIDE);
		//OleDraw
		VARIANT_BOOL flag=false;
		m_webBrowser->get_Busy(&flag);
		READYSTATE state;
		m_webBrowser->get_ReadyState(&state);
		HRESULT hr=::OleDraw(m_webBrowser,DVASPECT_CONTENT,m_pTexture->GetHDC(),&posRect);
	}
	return;
}

void CWebBrowser::OnSize(int x,int y,int w_in,int h_in)
{	
	m_ViewWidth=w_in;
	m_ViewHeight=h_in;
	html_x=x;
	html_y=y;
	RECT posRect;
	posRect.left = 0;
	posRect.top = 0;
	posRect.right = m_ViewWidth;
	posRect.bottom= m_ViewHeight;
	int panel_x=0;
	int panel_y=0;
	if (m_oleInPlaceObject) 
	{
		int hret=m_oleInPlaceObject->SetObjectRects(&posRect, &posRect);
		hret++;
	}
}

void CWebBrowser::OnWindowMouseMsg(UINT msg,WPARAM wp,LPARAM lp)
{
	if (!m_ieHWND)
		return; 
	::PostMessage(m_ieHWND, msg, wp, lp);
}

void CWebBrowser::OnWindowKeyboardMsg(UINT msg,WPARAM wp,LPARAM lp)
{
	if(msg==WM_CHAR)
		return ;
	if (m_ieHWND && ::PostMessage(m_ieHWND, msg, wp, lp)==0) 
		return;
}

void CWebBrowser::NewWindow(IDispatch **pIDispatch)
{
	if(m_webBrowser)
		m_webBrowser->get_Application(pIDispatch);
}

bool CWebBrowser::Show(bool shown)
{
	if (m_webBrowser) 
		m_webBrowser->put_Visible(shown);
	return true;
}

bool CWebBrowser::StopLoading()
{
	HRESULT hret=m_webBrowser->Stop();
	if(SUCCEEDED(hret))
		return true;
	return false;
}

RECT CWebBrowser::SetBounds()
{
	RECT posRect;
	posRect.left = 0;
	posRect.top = 0;
	posRect.right = m_ViewWidth;
	posRect.bottom = m_ViewHeight;

	if (m_oleInPlaceObject) 
	{
		int hret=m_oleInPlaceObject->SetObjectRects(&posRect, &posRect);
		if(SUCCEEDED(hret))
			hret++;
	}
	return posRect;
}
//WebBrowser URL
bool CWebBrowser::Refresh()
{
	OpenURL(m_currentUrl);
	return true;
}

void CWebBrowser::OpenURL(const char *url)
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
	m_specificallyOpened = true;
	HRESULT hret;
	hret = m_webBrowser->Navigate2(&vUrl, &navFlag, &targetFrame, &postData, &headers); //调用Invoke
}

bool CWebBrowser::OnStartURL(const char * url,bool first)
{
	m_IsCompleteLoad=false;

	if(m_HtmlEventsAdviseCookie!=0) // this is reset for each new page load
		m_connectionPoint->Unadvise(m_HtmlEventsAdviseCookie);

	strncpy(m_currentUrl,url,512);
	if (m_specificallyOpened)
		m_specificallyOpened = false;
	m_newpage=false;
	if(m_WebEventCallBack)
		m_WebEventCallBack->OnForward();
	return true;
}

void CWebBrowser::OnFinishURL(const char * url)
{
	if(!url)
		return ;
	static bool hooked=false; // this is only done ONCE for all HTML windows :)
	m_newpage=true;
	m_cleared=false;
	// You need to query the IHTMLElement interface here to get it later, I have no idea why...
	IDispatch* pDisp ;
	m_webBrowser->get_Document(&pDisp);

	IHTMLDocument2* pHTMLDocument2;
	HRESULT hret;
	hret = pDisp->QueryInterface( IID_IHTMLDocument2, (void**)&pHTMLDocument2 );
	if (hret == S_OK)
	{	
		IHTMLElement *pElement;
		hret = pHTMLDocument2->get_body(&pElement);
		if(pElement)
			pElement->get_outerHTML(&m_TextCode);
		pElement->Release();
		pHTMLDocument2->Release();
	}
	pDisp->Release();

	m_IsCompleteLoad=true;

	CFindWindow f_win;
	f_win.FindWin(m_oleObjectHWND,"Internet Explorer_Server");
	m_ieHWND=f_win.m_hWnd;
	
	html_windows.clear();
	html_windows.push_back(this);
	
	if(!m_IsHook && m_ieHWND)
	{
		DWORD tid = ::GetWindowThreadProcessId(m_ieHWND, NULL); 
		m_hHook=::SetWindowsHookEx(WH_GETMESSAGE,GetMsgProc,NULL,tid);
		if(m_hHook==NULL)
		{
			DWORD err=::GetLastError();
			err++;
		}
		else
		{
			m_ThreadID = tid;
			m_IsHook=true;		
		
		}
	}	

	if(m_WebEventCallBack)
		m_WebEventCallBack->OnDocumentComplete();
}

// Purpose: called as the URL download progresses
void CWebBrowser::OnProgressURL(long current, long maximum)
{
}
// Purpose: callback to provide useful status info from the IE component
void CWebBrowser::OnSetStatusText(const char *text)
{
}
// Purpose: called when a repaint is needed
void CWebBrowser::OnUpdate()
{
}
// Purpose: called when the cursor goes over a link
void CWebBrowser::OnLink()
{
}
// Purpose: called when the cursor leaves a link
void CWebBrowser::OffLink()
{
}

// Purpose: returns whether the HTML element is a <a href></a> element
bool CWebBrowser::CheckIsLink(IHTMLElement *el, char *type)
{
	BSTR bstr;
	bool IsLink=false;
	el->get_tagName(&bstr);
	_bstr_t p  = _bstr_t(bstr);
	if (bstr)
	{	
		const char *tag = static_cast<char *>(p);
		if( !stricmp(tag,type))
		{
			IsLink=true;
		}
		SysFreeString(bstr);
	}
	return IsLink;
}

//HTML DOCUMENT
void CWebBrowser::Clear()
{
	IDispatch* pDisp ;
	m_webBrowser->get_Document(&pDisp);
	if (pDisp != NULL )
	{
		IHTMLDocument2* pHTMLDocument2;
		HRESULT hr;
		hr = pDisp->QueryInterface( IID_IHTMLDocument2, (void**)&pHTMLDocument2 );
		if (hr == S_OK)
		{
			pHTMLDocument2->close();
			pHTMLDocument2->Release();
		}
		pDisp->Release();
	}
}

// Purpose: adds the string "text" to the end of the current HTML page.
void CWebBrowser::AddText(const char *text)
{
	IDispatch* pDisp ;
	m_webBrowser->get_Document(&pDisp);
	IHTMLDocument2* pHTMLDocument2;
	HRESULT hret;
	hret = pDisp->QueryInterface( IID_IHTMLDocument2, (void**)&pHTMLDocument2 );
	if (hret == S_OK)
	{
		wchar_t *tmp= new wchar_t[strlen(text)+1];
		// Creates a new one-dimensional array
		if(tmp)
		{
			IHTMLElement *pElement;
			::MultiByteToWideChar(CP_ACP,0,text,strlen(text)+1,tmp,strlen(text)+1);
			//BSTR bstr = SysAllocString(tmp);
			HRESULT hret = pHTMLDocument2->get_body(&pElement);
			if( hret == S_OK && pElement)
			{
				BSTR where = L"beforeEnd";
				pElement->insertAdjacentHTML(where,tmp);
				pElement->Release();
			}
		}
		pHTMLDocument2->Release();
	}
	pDisp->Release();
}

// Purpose: returns whether the HTML element is a <a href></a> element
void CWebBrowser::SetHTMLSize(IHTMLElement *piElem)
{
	IHTMLBodyElement *piBody;
	IHTMLTextContainer *piCont;
	piElem->QueryInterface(IID_IHTMLBodyElement,(void **)&piBody);
	if (!piBody)
		return;
	piBody->put_scroll(_bstr_t("no"));
	piBody->QueryInterface(IID_IHTMLTextContainer,(void **)&piCont);
	piCont->get_scrollWidth(&html_w);
	piCont->get_scrollHeight(&html_h);
	piCont->put_scrollTop(html_x);
	if(html_w>m_ViewWidth) 
	{
		m_ViewWidth=html_w;
	}
	if(html_h>m_ViewHeight) 
	{
		m_ViewHeight=html_h;
	}
	piCont->Release();
	piBody->Release();
	piElem->Release();
}

// Purpose: scrolls the html element by x pixels down and y pixels across
void CWebBrowser::ScrollHTML(int x,int y)
{
	// You need to query the IHTMLElement interface here to get it later, I have no idea why...
	IDispatch* pDisp ;
	m_webBrowser->get_Document(&pDisp);
	if(pDisp)
	{
		IHTMLDocument2* pHTMLDocument2;
		HRESULT hret;
		hret = pDisp->QueryInterface( IID_IHTMLDocument2, (void**)&pHTMLDocument2 );
		if (hret == S_OK && pHTMLDocument2)
		{	
			IHTMLElement *pElement;
			hret = pHTMLDocument2->get_body(&pElement);

			if(hret == S_OK && pElement)
			{
				IHTMLBodyElement *piBody;
				IHTMLTextContainer *piCont;
				pElement->QueryInterface(IID_IHTMLBodyElement,(void **)&piBody);
				if (!piBody)
					return;
				piBody->QueryInterface(IID_IHTMLTextContainer,(void **)&piCont);
				piCont->put_scrollLeft(x);
				piCont->put_scrollTop(y);
				piCont->Release();
				piBody->Release();
				pElement->Release();
			}
			pHTMLDocument2->Release();
		}
		pDisp->Release();
	}
}

// Purpose: called when the cursor enters a new HTML Element (i.e link, image,etc)
bool CWebBrowser::OnMouseOver()
{
	bool IsLink=false;
	IDispatch* pDisp ;
	m_webBrowser->get_Document(&pDisp);
	IHTMLDocument2* pHTMLDocument2;
	HRESULT hret;
	hret = pDisp->QueryInterface( IID_IHTMLDocument2, (void**)&pHTMLDocument2 );
	if (hret == S_OK)
	{	
		IHTMLWindow2* pParentWindow;
		HRESULT hr = pHTMLDocument2->get_parentWindow(&pParentWindow);
			if (SUCCEEDED(hr))
			{
				IHTMLEventObj* pEvtObj;
				hr = pParentWindow->get_event(&pEvtObj);
				pParentWindow->Release();
	
				if (SUCCEEDED(hr) && pEvtObj)  // sometimes it can succeed BUT pEvtObj is null, nice eh?
				{
					IHTMLElement *el;
					pEvtObj->get_srcElement(&el);

					if(CheckIsLink(el,"A"))
					{
						IsLink=true;
					}
					else
					{
						IHTMLElement *pel=el,*oldpel=NULL;
						while(IsLink==false && SUCCEEDED(pel->get_parentElement(&pel)))
						{
							if(oldpel!=NULL)
							{
								oldpel->Release();
							}
							if(pel==NULL || pel==oldpel) // this goes up the nested elements until it hits the <HTML> tag and then it just keeps returing that (silly thing...)
							{
								break;
							}
							if(CheckIsLink(pel,"A"))
							{
								IsLink=true;
							}
							oldpel=pel;	
						} 
						if(pel!=NULL)
						{
							pel->Release();
						}
					} //CheckIsLink
					el->Release();
				} // if got event
			} // if got window
		pHTMLDocument2->Release();
	} // if got document
	pDisp->Release();
	return IsLink;
}
