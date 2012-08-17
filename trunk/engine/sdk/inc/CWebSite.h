#pragma once

#include <oleidl.h>
#include <winerror.h>
#include <comdef.h>

#include <mshtml.h>			//for htmlDocument 2
#include <mshtmdid.h>		
#include <mshtmhst.h>		//for IDocHostUIHandler

#include <exdispid.h>
#include <olectl.h>
#include <exdisp.h>

#include <tchar.h>

#pragma comment(lib,"winmm.lib")


namespace sqr
{
	class CWebBrowser;
}

class IE_IOleInPlaceFrame;
class IE_IOleInSiteWindowless;
class IE_IOleClientSite;
class IE_IOleControlSite;
class IE_IOleCommandTarget;
class IE_IOleItemContainer;
class IE_IDispatch;
class IE_DWebBrowserEvents2;
class IE_DHTMLDocEvents2;
class IE_IAdviseSink2;
class IE_IAdviseSinkEx;
class IE_IDocHostUIHandler;

class CFrameSite :public IUnknown
{
	friend class CWebBrowser;
	friend class IE_IOleInPlaceFrame;
	friend class IE_IOleInSiteWindowless;
	friend class IE_IOleClientSite;
	friend class IE_IOleControlSite;
	friend class IE_IOleCommandTarget;
	friend class IE_IOleItemContainer;
	friend class IE_IDispatch;
	friend class IE_DWebBrowserEvents2;
	friend class IE_DHTMLDocEvents2;
	friend class IE_IAdviseSink2;
	friend class IE_IAdviseSinkEx;
	friend class IE_IDocHostUIHandler;

public:
	CFrameSite(CWebBrowser* iecore);
	~CFrameSite(void);

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID* ppv)
	{
		if(ppv == NULL ) return E_INVALIDARG;
		*ppv=0;
		if(riid == IID_IUnknown)
			*ppv = (IUnknown*)this;
		else if (riid == IID_IOleWindow || riid==IID_IOleInPlaceUIWindow || riid==IID_IOleInPlaceFrame)
			*ppv = m_IOleInPlaceFrame;
		else if (riid == IID_IOleInPlaceSite || riid==IID_IOleInPlaceSiteEx || riid==IID_IOleInPlaceSiteWindowless)
			*ppv = m_IOleInPlaceSiteWindowless;
		else if(riid == IID_IOleClientSite)
			*ppv = m_IOleClientSite;
		else if (riid == IID_IOleControlSite)
			*ppv = m_IOleControlSite;
		else if (riid == IID_IOleCommandTarget)
			*ppv = m_IOleCommandTarget;
		else if (riid == IID_IOleItemContainer|| riid == IID_IOleContainer || riid ==IID_IParseDisplayName)
			*ppv = m_IOleItemContainer;
		else if (riid == IID_IDispatch)
			*ppv = m_IDispatch;
		else if (riid == DIID_DWebBrowserEvents2)
			*ppv = m_DWebBrowserEvents2;
		else if (riid == DIID_HTMLDocumentEvents)
			*ppv = m_DHTMLDocEvents2;
		else if (riid == IID_IAdviseSink)
			*ppv = m_IAdviseSink2;
		else if (riid == IID_IAdviseSinkEx)
			*ppv = m_IAdviseSinkEx;
		else if (riid == IID_IDocHostUIHandler)
			*ppv = m_IDocHostUIHandler;
		if(*ppv == NULL)
			return (HRESULT) E_NOINTERFACE;

		AddRef();
		return S_OK;
	}

	ULONG STDMETHODCALLTYPE AddRef()
	{  
		return ++m_cRef;
	}

	ULONG STDMETHODCALLTYPE Release()
	{  
		--m_cRef;
		if(!m_cRef)
			delete this;
		return m_cRef;
	}

	void Clear();

public :
	int m_cRef;

	IE_IOleInPlaceFrame*		m_IOleInPlaceFrame;
	IE_IOleInSiteWindowless*	m_IOleInPlaceSiteWindowless;
	IE_IOleClientSite*			m_IOleClientSite;
	IE_IOleControlSite*			m_IOleControlSite;
	IE_IOleCommandTarget*		m_IOleCommandTarget;
	IE_IOleItemContainer*		m_IOleItemContainer;
	IE_IDispatch*				m_IDispatch;
	IE_DWebBrowserEvents2*		m_DWebBrowserEvents2;
	IE_DHTMLDocEvents2*			m_DHTMLDocEvents2;
	IE_IAdviseSink2*			m_IAdviseSink2;
	IE_IAdviseSinkEx*			m_IAdviseSinkEx;
	IE_IDocHostUIHandler*		m_IDocHostUIHandler;
	
	CWebBrowser*				m_IEWindow;
	HWND						m_hWndParent;
	HDC							m_hDCBuffer;

	bool	m_bSupportsWindowlessActivation;
	bool	m_bInPlaceLocked;
	bool	m_bInPlaceActive;
	bool	m_bUIActive;
	bool	m_bWindowless;

	LCID		m_nAmbientLocale;
	COLORREF	m_clrAmbientForeColor;
	COLORREF	m_clrAmbientBackColor;
	bool		m_bAmbientShowHatching;
	bool		m_bAmbientShowGrabHandles;
	bool		m_bAmbientUserMode;
	bool		m_bAmbientAppearance;
};

class IE_IOleInPlaceFrame : public IOleInPlaceFrame
{
public:
	IE_IOleInPlaceFrame(CFrameSite* fs) { m_fs = fs; }
	~IE_IOleInPlaceFrame() {}

	//IUnknown
	STDMETHODIMP QueryInterface(REFIID iid, void ** ppvObject) { return m_fs->QueryInterface(iid, ppvObject); }
	ULONG STDMETHODCALLTYPE AddRef() { return m_fs->AddRef(); }
	ULONG STDMETHODCALLTYPE Release() { return m_fs->Release(); }
	//IOleWindow
	STDMETHODIMP GetWindow(HWND*);
	STDMETHODIMP ContextSensitiveHelp(BOOL);
	//IOleInPlaceUIWindow
	STDMETHODIMP GetBorder(LPRECT);
	STDMETHODIMP RequestBorderSpace(LPCBORDERWIDTHS);
	STDMETHODIMP SetBorderSpace(LPCBORDERWIDTHS);
	STDMETHODIMP SetActiveObject(IOleInPlaceActiveObject*, LPCOLESTR);
	//IOleInPlaceFrame
	STDMETHODIMP InsertMenus(HMENU, LPOLEMENUGROUPWIDTHS);
	STDMETHODIMP SetMenu(HMENU, HOLEMENU, HWND);
	STDMETHODIMP RemoveMenus(HMENU);
	STDMETHODIMP SetStatusText(LPCOLESTR);
	STDMETHODIMP EnableModeless(BOOL);
	STDMETHODIMP TranslateAccelerator(LPMSG, WORD);
protected:
	CFrameSite * m_fs;
};


class IE_IOleInSiteWindowless : public IOleInPlaceSiteWindowless
{
public:
	IE_IOleInSiteWindowless(CFrameSite* fs) { m_fs = fs; }
	~IE_IOleInSiteWindowless() {}

	//IUnknown
	STDMETHODIMP QueryInterface(REFIID iid, void ** ppvObject) { return m_fs->QueryInterface(iid, ppvObject); }
	ULONG STDMETHODCALLTYPE AddRef() { return m_fs->AddRef(); }
	ULONG STDMETHODCALLTYPE Release() { return m_fs->Release(); }
	//IOleWindow
	STDMETHODIMP GetWindow(HWND* h)
	{ return m_fs->m_IOleInPlaceFrame->GetWindow(h); }
	STDMETHODIMP ContextSensitiveHelp(BOOL b)
	{ return m_fs->m_IOleInPlaceFrame->ContextSensitiveHelp(b); }
	//IOleInPlaceSite
	STDMETHODIMP CanInPlaceActivate();
	STDMETHODIMP OnInPlaceActivate();
	STDMETHODIMP OnUIActivate();
	STDMETHODIMP GetWindowContext(IOleInPlaceFrame**, IOleInPlaceUIWindow**, 
		LPRECT, LPRECT, LPOLEINPLACEFRAMEINFO);
	STDMETHODIMP Scroll(SIZE);
	STDMETHODIMP OnUIDeactivate(BOOL);
	STDMETHODIMP OnInPlaceDeactivate();
	STDMETHODIMP DiscardUndoState();
	STDMETHODIMP DeactivateAndUndo();
	STDMETHODIMP OnPosRectChange(LPCRECT);
	//IOleInPlaceSiteEx
	STDMETHODIMP OnInPlaceActivateEx(BOOL*, DWORD);
	STDMETHODIMP OnInPlaceDeactivateEx(BOOL);
	STDMETHODIMP RequestUIActivate();
	//IOleInPlaceSiteWindowless
	STDMETHODIMP CanWindowlessActivate();
	STDMETHODIMP GetCapture();
	STDMETHODIMP SetCapture(BOOL);
	STDMETHODIMP GetFocus();
	STDMETHODIMP SetFocus(BOOL);
	STDMETHODIMP GetDC(LPCRECT, DWORD, HDC*);
	STDMETHODIMP ReleaseDC(HDC);
	STDMETHODIMP InvalidateRect(LPCRECT, BOOL);
	STDMETHODIMP InvalidateRgn(HRGN, BOOL);
	STDMETHODIMP ScrollRect(INT, INT, LPCRECT, LPCRECT);
	STDMETHODIMP AdjustRect(LPRECT);
	STDMETHODIMP OnDefWindowMessage(UINT, WPARAM, LPARAM, LRESULT*);

protected:
	CFrameSite * m_fs;
};

class IE_IOleClientSite : public IOleClientSite
{
public:
	IE_IOleClientSite(CFrameSite* fs) { m_fs = fs; }
	~IE_IOleClientSite() {}

	//IUnknown
	STDMETHODIMP QueryInterface(REFIID iid, void ** ppvObject) { return m_fs->QueryInterface(iid, ppvObject); }
	ULONG STDMETHODCALLTYPE AddRef() { return m_fs->AddRef(); }
	ULONG STDMETHODCALLTYPE Release() { return m_fs->Release(); }
	//IOleClientSite
	STDMETHODIMP SaveObject();
	STDMETHODIMP GetMoniker(DWORD, DWORD, IMoniker**);
	STDMETHODIMP GetContainer(LPOLECONTAINER FAR*);
	STDMETHODIMP ShowObject();
	STDMETHODIMP OnShowWindow(BOOL);
	STDMETHODIMP RequestNewObjectLayout();
protected:
	CFrameSite * m_fs;
};

class IE_IOleControlSite : public IOleControlSite
{
public:
	IE_IOleControlSite(CFrameSite* fs) { m_fs = fs; }
	~IE_IOleControlSite() {}

	//IUnknown
	STDMETHODIMP QueryInterface(REFIID iid, void ** ppvObject) { return m_fs->QueryInterface(iid, ppvObject); }
	ULONG STDMETHODCALLTYPE AddRef() { return m_fs->AddRef(); }
	ULONG STDMETHODCALLTYPE Release() { return m_fs->Release(); }
	//IOleControlSite
	STDMETHODIMP OnControlInfoChanged();
	STDMETHODIMP LockInPlaceActive(BOOL);
	STDMETHODIMP GetExtendedControl(IDispatch**);
	STDMETHODIMP TransformCoords(POINTL*, POINTF*, DWORD);
	STDMETHODIMP ShowContextMenu(DWORD dwID,POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved);
	STDMETHODIMP TranslateAccelerator(LPMSG, DWORD);
	STDMETHODIMP OnFocus(BOOL);
	STDMETHODIMP ShowPropertyFrame();

protected:
	CFrameSite * m_fs;
};

class IE_IOleCommandTarget : public IOleCommandTarget
{
public:
	IE_IOleCommandTarget(CFrameSite* fs) { m_fs = fs; }
	~IE_IOleCommandTarget() {}

	//IUnknown
	STDMETHODIMP QueryInterface(REFIID iid, void ** ppvObject) { return m_fs->QueryInterface(iid, ppvObject); }
	ULONG STDMETHODCALLTYPE AddRef() { return m_fs->AddRef(); }
	ULONG STDMETHODCALLTYPE Release() { return m_fs->Release(); }
	//IOleCommandTarget
	STDMETHODIMP QueryStatus(const GUID*, ULONG, OLECMD[], OLECMDTEXT*);
	STDMETHODIMP Exec(const GUID*, DWORD, DWORD, VARIANTARG*, VARIANTARG*);
protected:
	CFrameSite * m_fs;
};

class IE_IOleItemContainer : public IOleItemContainer
{
public:
	IE_IOleItemContainer(CFrameSite* fs) { m_fs = fs; }
	~IE_IOleItemContainer() {}

	//IUnknown
	STDMETHODIMP QueryInterface(REFIID iid, void ** ppvObject) { return m_fs->QueryInterface(iid, ppvObject); }
	ULONG STDMETHODCALLTYPE AddRef() { return m_fs->AddRef(); }
	ULONG STDMETHODCALLTYPE Release() { return m_fs->Release(); }
	//IParseDisplayName
	STDMETHODIMP ParseDisplayName(IBindCtx*, LPOLESTR, ULONG*, IMoniker**);
	//IOleContainer
	STDMETHODIMP EnumObjects(DWORD, IEnumUnknown**);
	STDMETHODIMP LockContainer(BOOL);
	//IOleItemContainer
	STDMETHODIMP GetObject(LPOLESTR, DWORD, IBindCtx*, REFIID, void**);
	STDMETHODIMP GetObjectStorage(LPOLESTR, IBindCtx*, REFIID, void**);
	STDMETHODIMP IsRunning(LPOLESTR);
protected:
	CFrameSite * m_fs;
};

class IE_IDispatch : public IDispatch
{
public:
	IE_IDispatch(CFrameSite* fs) { m_fs = fs; m_bNewURL=0; m_bOnLink=false;}
	~IE_IDispatch() {}

	//IUnknown
	STDMETHODIMP QueryInterface(REFIID iid, void ** ppvObject) { return m_fs->QueryInterface(iid, ppvObject); }
	ULONG STDMETHODCALLTYPE AddRef() { return m_fs->AddRef(); }
	ULONG STDMETHODCALLTYPE Release() { return m_fs->Release(); }
	//IDispatch
	STDMETHODIMP GetIDsOfNames(REFIID, OLECHAR**, unsigned int, LCID, DISPID*);
	STDMETHODIMP GetTypeInfo(unsigned int, LCID, ITypeInfo**);
	STDMETHODIMP GetTypeInfoCount(unsigned int*);
	STDMETHODIMP Invoke(DISPID, REFIID, LCID, WORD, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);
protected:
	CFrameSite * m_fs;
	unsigned int m_bNewURL;
	bool m_bOnLink;
};


class IE_DWebBrowserEvents2 : public DWebBrowserEvents2
{
public:
	IE_DWebBrowserEvents2(CFrameSite* fs) { m_fs = fs; }
	~IE_DWebBrowserEvents2() {}

	//IUnknown
	STDMETHODIMP QueryInterface(REFIID iid, void ** ppvObject) { return m_fs->QueryInterface(iid, ppvObject); }
	ULONG STDMETHODCALLTYPE AddRef() { return m_fs->AddRef(); }
	ULONG STDMETHODCALLTYPE Release() { return m_fs->Release(); }
	//IDispatch
	STDMETHODIMP GetIDsOfNames(REFIID r, OLECHAR** o, unsigned int i, LCID l, DISPID* d)
	{ return m_fs->m_IDispatch->GetIDsOfNames(r, o, i, l, d); }
	STDMETHODIMP GetTypeInfo(unsigned int i, LCID l, ITypeInfo** t)
	{ return m_fs->m_IDispatch->GetTypeInfo(i, l, t); }
	STDMETHODIMP GetTypeInfoCount(unsigned int* i)
	{ return m_fs->m_IDispatch->GetTypeInfoCount(i); }
	STDMETHODIMP Invoke(DISPID d, REFIID r, LCID l, WORD w, DISPPARAMS* dp, 
		VARIANT* v, EXCEPINFO* e, UINT* u)
	{ return m_fs->m_IDispatch->Invoke(d, r, l, w, dp, v, e, u); }
protected:
	CFrameSite * m_fs;
};

class IE_DHTMLDocEvents2 : public HTMLDocumentEvents
{
public:
	IE_DHTMLDocEvents2(CFrameSite* fs) { m_fs = fs; }
	~IE_DHTMLDocEvents2() {}

	//IUnknown
	STDMETHODIMP QueryInterface(REFIID iid, void ** ppvObject) { return m_fs->QueryInterface(iid, ppvObject); }
	ULONG STDMETHODCALLTYPE AddRef() { return m_fs->AddRef(); }
	ULONG STDMETHODCALLTYPE Release() { return m_fs->Release(); }
	//IDispatch
	STDMETHODIMP GetIDsOfNames(REFIID r, OLECHAR** o, unsigned int i, LCID l, DISPID* d)
	{ return m_fs->m_IDispatch->GetIDsOfNames(r, o, i, l, d); }
	STDMETHODIMP GetTypeInfo(unsigned int i, LCID l, ITypeInfo** t)
	{ return m_fs->m_IDispatch->GetTypeInfo(i, l, t); }
	STDMETHODIMP GetTypeInfoCount(unsigned int* i)
	{ return m_fs->m_IDispatch->GetTypeInfoCount(i); }
	STDMETHODIMP Invoke(DISPID d, REFIID r, LCID l, WORD w, DISPPARAMS* dp, 
		VARIANT* v, EXCEPINFO* e, UINT* u)
	{ return m_fs->m_IDispatch->Invoke(d, r, l, w, dp, v, e, u); }
protected:
	CFrameSite * m_fs;
};


class IE_IAdviseSink2 : public IAdviseSink2
{
public:
	IE_IAdviseSink2(CFrameSite* fs) { m_fs = fs; }
	~IE_IAdviseSink2() {}

	//IUnknown
	STDMETHODIMP QueryInterface(REFIID iid, void ** ppvObject) { return m_fs->QueryInterface(iid, ppvObject); }
	ULONG STDMETHODCALLTYPE AddRef() { return m_fs->AddRef(); }
	ULONG STDMETHODCALLTYPE Release() { return m_fs->Release(); }
	//IAdviseSink
	void STDMETHODCALLTYPE OnDataChange(FORMATETC*, STGMEDIUM*);
	void STDMETHODCALLTYPE OnViewChange(DWORD, LONG);
	void STDMETHODCALLTYPE OnRename(IMoniker*);
	void STDMETHODCALLTYPE OnSave();
	void STDMETHODCALLTYPE OnClose();
	//IAdviseSink2
	void STDMETHODCALLTYPE OnLinkSrcChange(IMoniker*);
protected:
	CFrameSite * m_fs;
};

class IE_IAdviseSinkEx : public IAdviseSinkEx
{
public:
	IE_IAdviseSinkEx(CFrameSite* fs) { m_fs = fs; }
	~IE_IAdviseSinkEx() {}

	//IUnknown
	STDMETHODIMP QueryInterface(REFIID iid, void ** ppvObject) { return m_fs->QueryInterface(iid, ppvObject); }
	ULONG STDMETHODCALLTYPE AddRef() { return m_fs->AddRef(); }
	ULONG STDMETHODCALLTYPE Release() { return m_fs->Release(); }
	//IAdviseSink
	void STDMETHODCALLTYPE OnDataChange(FORMATETC* f, STGMEDIUM* s)
	{ m_fs->m_IAdviseSink2->OnDataChange(f, s); }
	void STDMETHODCALLTYPE OnViewChange(DWORD d, LONG l)
	{ m_fs->m_IAdviseSink2->OnViewChange(d, l); }
	void STDMETHODCALLTYPE OnRename(IMoniker* i)
	{ m_fs->m_IAdviseSink2->OnRename(i); }
	void STDMETHODCALLTYPE OnSave()
	{ m_fs->m_IAdviseSink2->OnSave(); }
	void STDMETHODCALLTYPE OnClose()
	{ m_fs->m_IAdviseSink2->OnClose(); }
	//IAdviseSinkEx
	void STDMETHODCALLTYPE OnViewStatusChange(DWORD);
protected:
	CFrameSite * m_fs;
};

class IE_IDocHostUIHandler : public IDocHostUIHandler
{
public:
	IE_IDocHostUIHandler(CFrameSite* fs) 	{ m_fs = fs; }
	~IE_IDocHostUIHandler() {}

	//IUnknown
	STDMETHODIMP QueryInterface(REFIID iid, void ** ppvObject) { return m_fs->QueryInterface(iid, ppvObject); }
	ULONG STDMETHODCALLTYPE AddRef() { return m_fs->AddRef(); }
	ULONG STDMETHODCALLTYPE Release() { return m_fs->Release(); }
	
	STDMETHODIMP GetHostInfo( 
		/* [out][in] */ DOCHOSTUIINFO __RPC_FAR *pInfo);

	STDMETHODIMP ShowContextMenu(
		/* [in] */ DWORD dwID,
		/* [in] */ POINT __RPC_FAR *ppt,
		/* [in] */ IUnknown __RPC_FAR *pcmdtReserved,
		/* [in] */ IDispatch __RPC_FAR *pdispReserved)		{ return S_FALSE; }
	
	STDMETHODIMP ShowUI( 
		/* [in] */ DWORD dwID,
		/* [in] */ IOleInPlaceActiveObject __RPC_FAR *pActiveObject,
		/* [in] */ IOleCommandTarget __RPC_FAR *pCommandTarget,
		/* [in] */ IOleInPlaceFrame __RPC_FAR *pFrame,
		/* [in] */ IOleInPlaceUIWindow __RPC_FAR *pDoc)		{ return S_FALSE; }
	
    STDMETHODIMP HideUI(void)								{ return S_OK; }
	
	STDMETHODIMP UpdateUI(void)								{ return S_OK; }
	
	STDMETHODIMP EnableModeless(BOOL fEnable) 				{ return S_OK; }
	
	STDMETHODIMP OnDocWindowActivate(BOOL fActivate) 		{ return S_OK; }
	
	STDMETHODIMP OnFrameWindowActivate(BOOL fActivate)		{ return S_OK; }
	
	STDMETHODIMP ResizeBorder( 
		/* [in] */ LPCRECT prcBorder,
		/* [in] */ IOleInPlaceUIWindow __RPC_FAR *pUIWindow,
		/* [in] */ BOOL fRameWindow) 						{ return S_OK; }
	
	STDMETHODIMP TranslateAccelerator( 
		/* [in] */ LPMSG lpMsg,
		/* [in] */ const GUID __RPC_FAR *pguidCmdGroup,
		/* [in] */ DWORD nCmdID) 							{ return S_FALSE; }
	
	STDMETHODIMP GetOptionKeyPath( 
		/* [out] */ LPOLESTR __RPC_FAR *pchKey,
		/* [in] */ DWORD dw)								{ return S_FALSE; }
	
	STDMETHODIMP GetDropTarget( 
		/* [in] */ IDropTarget __RPC_FAR *pDropTarget,
		/* [out] */ IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget) { return E_FAIL; }
	
	STDMETHODIMP GetExternal( 
		/* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch) 	{ ppDispatch = NULL; return S_FALSE; }
	
	STDMETHODIMP TranslateUrl( 
		/* [in] */ DWORD dwTranslate,
		/* [in] */ OLECHAR __RPC_FAR *pchURLIn,
		/* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut) 		{ ppchURLOut = NULL; return S_FALSE; }
	
	STDMETHODIMP FilterDataObject( 
		/* [in] */ IDataObject __RPC_FAR *pDO,
		/* [out] */ IDataObject __RPC_FAR *__RPC_FAR *ppDORet) 		{ ppDORet = NULL; return S_FALSE; }

protected:
	CFrameSite * m_fs;
};