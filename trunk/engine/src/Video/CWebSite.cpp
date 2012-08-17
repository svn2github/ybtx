#include "stdafx.h"
#include "CWebSite.h"
#include "CWebBrowser.h"

CFrameSite::CFrameSite(CWebBrowser* iecore)
{
	m_cRef = 0;

	m_IEWindow = iecore;
	m_bSupportsWindowlessActivation = true;
	m_bInPlaceLocked = false;
	m_bUIActive = false;
	m_bInPlaceActive = false;
	m_bWindowless = false;

	m_nAmbientLocale = 0;
	m_clrAmbientForeColor = ::GetSysColor(COLOR_WINDOWTEXT);
	m_clrAmbientBackColor = ::GetSysColor(COLOR_WINDOW);
	m_bAmbientUserMode = true;
	m_bAmbientShowHatching = true;
	m_bAmbientShowGrabHandles = true;
	m_bAmbientAppearance = true;
 
	m_hWndParent= NULL;
	m_hDCBuffer = NULL;
	//m_hWndParent = (HWND)m_IEWindow->GetHWND();

	m_IOleInPlaceFrame = new IE_IOleInPlaceFrame(this);
	m_IOleInPlaceFrame->AddRef();

	m_IOleInPlaceSiteWindowless = new IE_IOleInSiteWindowless(this);
	m_IOleInPlaceSiteWindowless->AddRef();

	m_IOleClientSite = new IE_IOleClientSite(this);
	m_IOleClientSite->AddRef();

	m_IOleControlSite = new IE_IOleControlSite(this);
	m_IOleControlSite->AddRef();

	m_IOleCommandTarget = new IE_IOleCommandTarget(this);
	m_IOleCommandTarget->AddRef();

	m_IOleItemContainer = new IE_IOleItemContainer(this);
	m_IOleItemContainer->AddRef();

	m_IDispatch = new IE_IDispatch(this);
	m_IDispatch->AddRef();

	m_DWebBrowserEvents2 = new IE_DWebBrowserEvents2(this);
	m_DWebBrowserEvents2->AddRef();

	m_DHTMLDocEvents2 = new IE_DHTMLDocEvents2(this);
	m_DHTMLDocEvents2->AddRef();

	m_IAdviseSink2 = new IE_IAdviseSink2(this);
	m_IAdviseSink2->AddRef();

	m_IAdviseSinkEx = new IE_IAdviseSinkEx(this);
	m_IAdviseSinkEx->AddRef();

	m_IDocHostUIHandler = new IE_IDocHostUIHandler(this);
	m_IDocHostUIHandler->AddRef();

}

CFrameSite::~CFrameSite(void)
{
	delete m_IOleInPlaceFrame;
	delete m_IOleInPlaceSiteWindowless;
	delete m_IOleClientSite;
	delete m_IOleControlSite;
	delete m_IOleCommandTarget;
	delete m_IOleItemContainer;
	delete m_IDispatch;
	delete m_DWebBrowserEvents2;
	delete m_DHTMLDocEvents2;
	delete m_IAdviseSink2;
	delete m_IAdviseSinkEx;
	delete m_IDocHostUIHandler;
}

void CFrameSite::Clear()
{
	m_IOleInPlaceFrame->Release();
	m_IOleInPlaceSiteWindowless->Release();
	m_IOleClientSite->Release();
	m_IOleControlSite->Release();
	m_IOleCommandTarget->Release();
	m_IOleItemContainer->Release();
	m_IDispatch->Release();
	m_DWebBrowserEvents2->Release();
	m_DHTMLDocEvents2->Release();
	m_IAdviseSink2->Release();
	m_IAdviseSinkEx->Release();
	m_IDocHostUIHandler->Release();
}

//IDispatch

HRESULT IE_IDispatch::GetIDsOfNames(REFIID riid, OLECHAR ** rgszNames, unsigned int cNames,
								 LCID lcid, DISPID * rgDispId)
{
	////DEBUG("IDispatch::GetIDsOfNames");
	return E_NOTIMPL;
}

HRESULT IE_IDispatch::GetTypeInfo(unsigned int iTInfo, LCID lcid, ITypeInfo ** ppTInfo)
{
	////DEBUG("IDispatch::GetTypeInfo");
	return E_NOTIMPL;
}

HRESULT IE_IDispatch::GetTypeInfoCount(unsigned int * pcTInfo)
{
	////DEBUG("IDispatch::GetTypeInfoCount");
	return E_NOTIMPL;
}

#include "tracehelper.h"

HRESULT IE_IDispatch::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid,
						  WORD wFlags, DISPPARAMS * pDispParams,
						  VARIANT * pVarResult, EXCEPINFO * pExcepInfo,
						  unsigned int * puArgErr)
{
	//DEBUG("IDispatch::Invoke");
	if (wFlags & DISPATCH_PROPERTYGET)
	{
		if (pVarResult == NULL) 
			return E_INVALIDARG;
		switch (dispIdMember)
		{
			case DISPID_AMBIENT_APPEARANCE:
				pVarResult->vt = VT_BOOL;
				pVarResult->boolVal = m_fs->m_bAmbientAppearance;
				break;

			case DISPID_AMBIENT_FORECOLOR:
				pVarResult->vt = VT_I4;
				pVarResult->lVal = (long) m_fs->m_clrAmbientForeColor;
				break;

			case DISPID_AMBIENT_BACKCOLOR:
				pVarResult->vt = VT_I4;
				pVarResult->lVal = (long) m_fs->m_clrAmbientBackColor;
				break;

			case DISPID_AMBIENT_LOCALEID:
				pVarResult->vt = VT_I4;
				pVarResult->lVal = (long) m_fs->m_nAmbientLocale;
				break;

			case DISPID_AMBIENT_USERMODE:
				pVarResult->vt = VT_BOOL;
				pVarResult->boolVal = m_fs->m_bAmbientUserMode;
				break;

			case DISPID_AMBIENT_SHOWGRABHANDLES:
				pVarResult->vt = VT_BOOL;
				pVarResult->boolVal = m_fs->m_bAmbientShowGrabHandles;
				break;

			case DISPID_AMBIENT_SHOWHATCHING:
				pVarResult->vt = VT_BOOL;
				pVarResult->boolVal = m_fs->m_bAmbientShowHatching;
				break;

			case DISPID_AMBIENT_DLCONTROL: // turn off javascript
				pVarResult->vt = VT_I4;
				//pVarResult->lVal = (long)(DLCTL_DLIMAGES|DLCTL_VIDEOS|DLCTL_BGSOUNDS|DLCTL_NO_SCRIPTS);
				pVarResult->lVal = (long)(DLCTL_DLIMAGES|DLCTL_VIDEOS|DLCTL_BGSOUNDS);
				break;

			default:
				return DISP_E_MEMBERNOTFOUND;
		}
		return S_OK;
	}
	//char dbtxt[200];
	//_snprintf(dbtxt,200,"$$$$$$$$$$$$$$$ Got ID %i\n",dispIdMember);
	//OutputDebugLog(dbtxt);
	switch (dispIdMember)
	{
		case DISPID_BEFORENAVIGATE2:
		{
			VARIANT * vurl = pDispParams->rgvarg[5].pvarVal;
			if(m_fs->m_IEWindow->OnStartURL((char*)_bstr_t(vurl->bstrVal),!m_bNewURL))
			{
				*pDispParams->rgvarg->pboolVal = VARIANT_FALSE;
			}
			else
			{ // this cancels the navigation
				*pDispParams->rgvarg->pboolVal = VARIANT_TRUE;
			}
			break;
		}
		case DISPID_PROGRESSCHANGE:
		{
			long current = pDispParams->rgvarg[1].lVal;
			long maximum = pDispParams->rgvarg[0].lVal;
			m_fs->m_IEWindow->OnProgressURL(current, maximum);
			break;
		}
		case DISPID_DOWNLOADCOMPLETE:
		{
			// for each DOWNLOADBEGIN there is a DOWNLOADCOMPLETE
			if(m_bNewURL>0) 
			{
				m_bNewURL--;
			}
			break;
		}

		case DISPID_DOCUMENTCOMPLETE:
		{
			// DON'T do this in DISPID_NAVIGATECOMPLETE2, the DHTML stuff is incomplete in that event
			// tell the main window to update
			if(m_bNewURL==0) // only allow the first url to get through :)
			{
				VARIANT * vurl = pDispParams->rgvarg[0].pvarVal;
				m_fs->m_IEWindow->OnFinishURL((char*)_bstr_t(vurl->bstrVal));
			}
			else
			{
				m_fs->m_IEWindow->OnUpdate(); // not finished the page, but a major "chunk" is done
			}
		}
		case DISPID_NAVIGATECOMPLETE2:
		{
			m_fs->m_IEWindow->OnUpdate();
			break;
		}
		case DISPID_DOWNLOADBEGIN:
		{
			// for each DOWNLOADBEGIN there is a DOWNLOADCOMPLETE
			m_bNewURL++;
			//ivgui()->DPrintf2("Begining download\n");
			break;
		}

		case DISPID_NEWWINDOW2:
		{ 
			//// stop new browser windows being opened if we are in the engine
			//if (!surface()->SupportsFeature(ISurface::OPENING_NEW_HTML_WINDOWS))
			//{
			//	m_bNewURL++;
			//	IDispatch *pIDispatch;
			//	 m_fs->m_window->NewWindow(&pIDispatch);
			//   *(pDispParams->rgvarg[1].ppdispVal) = pIDispatch;
			//	break;
			//}
			int a=0;
			break;
		}
	
		// IHTMLDocuments2 events
		case DISPID_HTMLDOCUMENTEVENTS_ONMOUSEOVER:
	     {
			if(m_fs->m_IEWindow->OnMouseOver())
			{
				m_bOnLink=true;
				m_fs->m_IEWindow->OnLink();
			}
			else if(m_bOnLink=true)
			{
				m_bOnLink=false;
				m_fs->m_IEWindow->OffLink();
			}
			break;
		 }
		case DISPID_COMMANDSTATECHANGE:
			{
				int a=0;
			}

	}
	return S_OK;
}

//IOleWindow

HRESULT IE_IOleInPlaceFrame::GetWindow(HWND * phwnd)
{
	//DEBUG("IOleWindow::GetWindow");
	if (phwnd == NULL) 
		return E_INVALIDARG;
	(*phwnd) = m_fs->m_hWndParent;
	return S_OK;
}

HRESULT IE_IOleInPlaceFrame::ContextSensitiveHelp(BOOL fEnterMode)
{
	//DEBUG("IOleWindow::ContextSensitiveHelp");
	return S_OK;
}

//IOleInPlaceUIWindow

HRESULT IE_IOleInPlaceFrame::GetBorder(LPRECT lprectBorder)
{
	//DEBUG("IOleInPlaceUIWindow::GetBorder");
	if (lprectBorder == NULL) return E_INVALIDARG;
	return INPLACE_E_NOTOOLSPACE;
}

HRESULT IE_IOleInPlaceFrame::RequestBorderSpace(LPCBORDERWIDTHS pborderwidths)
{
	//DEBUG("IOleInPlaceUIWindow::RequestBorderSpace");
	if (pborderwidths == NULL) return E_INVALIDARG;
	return INPLACE_E_NOTOOLSPACE;
}

HRESULT IE_IOleInPlaceFrame::SetBorderSpace(LPCBORDERWIDTHS pborderwidths)
{
	//DEBUG("IOleInPlaceUIWindow::SetBorderSpace");
	return S_OK;
}

HRESULT IE_IOleInPlaceFrame::SetActiveObject(IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName)
{
	//DEBUG("IOleInPlaceUIWindow::SetActiveObject");
	return S_OK;
}

//IOleInPlaceFrame

HRESULT IE_IOleInPlaceFrame::InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
	//DEBUG("IOleInPlaceFrame::InsertMenus");
	return S_OK;
}

HRESULT IE_IOleInPlaceFrame::SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject)
{
	//DEBUG("IOleInPlaceFrame::SetMenu");
	return S_OK;
}

HRESULT IE_IOleInPlaceFrame::RemoveMenus(HMENU hmenuShared)
{
	//DEBUG("IOleInPlaceFrame::RemoveMenus");
	return S_OK;
}

HRESULT IE_IOleInPlaceFrame::SetStatusText(LPCOLESTR pszStatusText)
{
	//DEBUG("IOleInPlaceFrame::SetStatusText");
////FIXME
//	if(m_fs->m_window->GetEvents())
//	{
//		char tmp[512];
//		WideCharToMultiByte(CP_ACP, 0, pszStatusText, -1, tmp,512, NULL, NULL);
//		m_fs->m_window->GetEvents()->OnSetStatusText(tmp);
//	}
	return S_OK;
}
 
HRESULT IE_IOleInPlaceFrame::EnableModeless(BOOL fEnable)
{
	//DEBUG("IOleInPlaceFrame::EnableModeless");
	return S_OK;
}

HRESULT IE_IOleInPlaceFrame::TranslateAccelerator(LPMSG lpmsg, WORD wID)
{
	//DEBUG("IOleInPlaceFrame::TranslateAccelerator");
	return E_NOTIMPL;
}

//IOleInPlaceSite

HRESULT IE_IOleInSiteWindowless::CanInPlaceActivate()
{
	//DEBUG("IOleInPlaceSite::CanInPlaceActivate");
	return S_OK;
}

HRESULT IE_IOleInSiteWindowless::OnInPlaceActivate()
{
	//DEBUG("**************IOleInPlaceSite::OnInPlaceActivate");
	m_fs->m_bInPlaceActive = true;
	return S_OK;
}

HRESULT IE_IOleInSiteWindowless::OnUIActivate()
{
	//DEBUG("*****IOleInPlaceSite::OnUIActivate");
	m_fs->m_bUIActive = true;
	return S_OK;
}

HRESULT IE_IOleInSiteWindowless::GetWindowContext(IOleInPlaceFrame **ppFrame,
									IOleInPlaceUIWindow **ppDoc,
									LPRECT lprcPosRect,
									LPRECT lprcClipRect,
									LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	//DEBUG("IOleInPlaceSite::GetWindowContext");
	if (ppFrame == NULL || ppDoc == NULL || lprcPosRect == NULL ||
		lprcClipRect == NULL || lpFrameInfo == NULL)
	{
		if (ppFrame != NULL) (*ppFrame) = NULL;
		if (ppDoc != NULL) (*ppDoc) = NULL;
		return E_INVALIDARG;
	}

	(*ppDoc) = (*ppFrame) = m_fs->m_IOleInPlaceFrame;
	(*ppDoc)->AddRef();
	(*ppFrame)->AddRef();

	int w,h;
	m_fs->m_IEWindow->GetSize(w, h);

	lprcPosRect->left =0; 
	lprcPosRect->top = 0;
	lprcPosRect->right = w;
	lprcPosRect->bottom = h;
	lprcClipRect->left = 0;
	lprcClipRect->top = 0;
	lprcClipRect->right = w;
	lprcClipRect->bottom = h;


	lpFrameInfo->fMDIApp = FALSE;
	lpFrameInfo->hwndFrame = m_fs->m_hWndParent;
	lpFrameInfo->haccel = NULL;
	lpFrameInfo->cAccelEntries = 0;

	return S_OK;
}

HRESULT IE_IOleInSiteWindowless::Scroll(SIZE scrollExtent)
{
	//DEBUG("IOleInPlaceSite::Scroll");
	return S_OK;
}

HRESULT IE_IOleInSiteWindowless::OnUIDeactivate(BOOL fUndoable)
{
	//DEBUG("IOleInPlaceSite::OnUIDeactivate");
	m_fs->m_bUIActive = false;
	return S_OK;
}

HRESULT IE_IOleInSiteWindowless::OnInPlaceDeactivate()
{
	//DEBUG("IOleInPlaceSite::OnInPlaceDeactivate");
	m_fs->m_bInPlaceActive = false;
	return S_OK;
}

HRESULT IE_IOleInSiteWindowless::DiscardUndoState()
{
	//DEBUG("IOleInPlaceSite::DiscardUndoState");
	return S_OK;
}

HRESULT IE_IOleInSiteWindowless::DeactivateAndUndo()
{
	//DEBUG("IOleInPlaceSite::DeactivateAndUndo");
	return S_OK;
}

HRESULT IE_IOleInSiteWindowless::OnPosRectChange(LPCRECT lprcPosRect)
{
	//DEBUG("IOleInPlaceSite::OnPosRectChange");
	return S_OK;
}

//IOleInPlaceSiteEx

HRESULT IE_IOleInSiteWindowless::OnInPlaceActivateEx(BOOL * pfNoRedraw, DWORD dwFlags)
{
	//DEBUG("IOleInPlaceSiteEx::OnInPlaceActivateEx");
	if (pfNoRedraw) (*pfNoRedraw) = FALSE;
	return S_OK;
}

HRESULT IE_IOleInSiteWindowless::OnInPlaceDeactivateEx(BOOL fNoRedraw)
{
	//DEBUG("************IOleInPlaceSiteEx::OnInPlaceDeactivateEx");
	return S_OK;
}

HRESULT IE_IOleInSiteWindowless::RequestUIActivate()
{
	//DEBUG("************IOleInPlaceSiteEx::RequestUIActivate");
	return S_OK;
}

//IOleInPlaceSiteWindowless

HRESULT IE_IOleInSiteWindowless::CanWindowlessActivate()
{
	//DEBUG("************IOleInPlaceSiteWindowless::CanWindowlessActivate");
	return (m_fs->m_bSupportsWindowlessActivation) ? S_OK : S_FALSE;
}

HRESULT IE_IOleInSiteWindowless::GetCapture()
{
	//DEBUG("************IOleInPlaceSiteWindowless::GetCapture");
	return S_FALSE;
}

HRESULT IE_IOleInSiteWindowless::SetCapture(BOOL fCapture)
{
	//DEBUG("************IOleInPlaceSiteWindowless::SetCapture");
	return S_FALSE;
}

HRESULT IE_IOleInSiteWindowless::GetFocus()
{
	//DEBUG("************IOleInPlaceSiteWindowless::GetFocus");
	return S_OK;
}

HRESULT IE_IOleInSiteWindowless::SetFocus(BOOL fFocus)
{
	//DEBUG("************IOleInPlaceSiteWindowless::SetFocus");
	return S_OK;
}

HRESULT IE_IOleInSiteWindowless::GetDC(LPCRECT pRect, DWORD grfFlags, HDC* phDC)
{
	//DEBUG("************IOleInPlaceSiteWindowless::GetDC");
	if (phDC == NULL) return E_INVALIDARG;

	if (grfFlags & OLEDC_NODRAW) 
	{
		(*phDC) = m_fs->m_hDCBuffer;
		return S_OK;
	}

	if (m_fs->m_hDCBuffer != NULL) return E_UNEXPECTED;

	return E_NOTIMPL;
}

HRESULT IE_IOleInSiteWindowless::ReleaseDC(HDC hDC)
{
	//DEBUG("************IOleInPlaceSiteWindowless::ReleaseDC");
	return E_NOTIMPL;
}

HRESULT IE_IOleInSiteWindowless::InvalidateRect(LPCRECT pRect, BOOL fErase)
{
	//DEBUG("************IOleInPlaceSiteWindowless::InvalidateRect");
	// Clip the rectangle against the object's size and invalidate it
	RECT rcI = { 0, 0, 0, 0 };
	RECT posRect=m_fs->m_IEWindow->SetBounds();
	if (pRect == NULL)
	{
		rcI = posRect;
	}
	else
	{
		IntersectRect(&rcI, &posRect, pRect);
	}
	::InvalidateRect(m_fs->m_hWndParent, &rcI, fErase);
 
	return S_OK;
}

HRESULT IE_IOleInSiteWindowless::InvalidateRgn(HRGN, BOOL)
{
	//DEBUG("************IOleInPlaceSiteWindowless::InvalidateRgn");
	return E_NOTIMPL;
}

HRESULT IE_IOleInSiteWindowless::ScrollRect(INT, INT, LPCRECT, LPCRECT)
{
	//DEBUG("************IOleInPlaceSiteWindowless::ScrollRect");
	return E_NOTIMPL;
}

HRESULT IE_IOleInSiteWindowless::AdjustRect(LPRECT)
{
	//DEBUG("************IOleInPlaceSiteWindowless::AdjustRect");
	return E_NOTIMPL;
}

HRESULT IE_IOleInSiteWindowless::OnDefWindowMessage(UINT, WPARAM, LPARAM, LRESULT*)
{
	//DEBUG("************IOleInPlaceSiteWindowless::OnDefWindowMessage");
	return E_NOTIMPL;
}

//IOleClientSite

HRESULT IE_IOleClientSite::SaveObject()
{
	//DEBUG("IOleClientSite::SaveObject");
	return S_OK;
}

HRESULT IE_IOleClientSite::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker,
							  IMoniker ** ppmk)
{
	//DEBUG("IOleClientSite::GetMoniker");
	return E_NOTIMPL;
}

HRESULT IE_IOleClientSite::GetContainer(LPOLECONTAINER * ppContainer)
{
	//DEBUG("IOleClientSite::GetContainer");
	if (ppContainer == NULL) return E_INVALIDARG;
	this->QueryInterface(IID_IOleContainer, (void**)(ppContainer));
	return S_OK;
}

HRESULT IE_IOleClientSite::ShowObject()
{
	//DEBUG("IOleClientSite::ShowObject");
	return S_OK;
}

HRESULT IE_IOleClientSite::OnShowWindow(BOOL fShow)
{
	//DEBUG("IOleClientSite::OnShowWindow");
	return S_OK;
}

HRESULT IE_IOleClientSite::RequestNewObjectLayout()
{
	//DEBUG("IOleClientSite::RequestNewObjectLayout");
	return E_NOTIMPL;
}

//IParseDisplayName

HRESULT IE_IOleItemContainer::ParseDisplayName(IBindCtx *pbc, LPOLESTR pszDisplayName,
									ULONG *pchEaten, IMoniker **ppmkOut)
{
	//DEBUG("IParseDisplayName::ParseDisplayName");
	return E_NOTIMPL;
}

//IOleContainer

HRESULT IE_IOleItemContainer::EnumObjects(DWORD grfFlags, IEnumUnknown **ppenum)
{
	//DEBUG("IOleContainer::EnumObjects");
	return E_NOTIMPL;
}

HRESULT IE_IOleItemContainer::LockContainer(BOOL fLock)
{
	//DEBUG("IOleContainer::LockContainer");
	return S_OK;
}

//IOleItemContainer

HRESULT IE_IOleItemContainer::GetObject(LPOLESTR pszItem, DWORD dwSpeedNeeded, 
							 IBindCtx * pbc, REFIID riid, void ** ppvObject)
{
	//DEBUG("IOleItemContainer::GetObject");
	if (pszItem == NULL) return E_INVALIDARG;
	if (ppvObject == NULL) return E_INVALIDARG;
	*ppvObject = NULL;
	return MK_E_NOOBJECT;
}

HRESULT IE_IOleItemContainer::GetObjectStorage(LPOLESTR pszItem, IBindCtx * pbc, 
									REFIID riid, void ** ppvStorage)
{
	//DEBUG("IOleItemContainer::GetObjectStorage");
	if (pszItem == NULL) return E_INVALIDARG;
	if (ppvStorage == NULL) return E_INVALIDARG;

	*ppvStorage = NULL;
	return MK_E_NOOBJECT;
}

HRESULT IE_IOleItemContainer::IsRunning(LPOLESTR pszItem)
{
	//DEBUG("IOleItemContainer::IsRunning");
	if (pszItem == NULL) return E_INVALIDARG;

	return MK_E_NOOBJECT;
}

//IOleControlSite

HRESULT IE_IOleControlSite::OnControlInfoChanged()
{
	//DEBUG("IOleControlSite::OnControlInfoChanged");
	return S_OK;
}

HRESULT IE_IOleControlSite::LockInPlaceActive(BOOL fLock)
{
	//DEBUG("IOleControlSite::LockInPlaceActive");
	m_fs->m_bInPlaceLocked = (fLock) ? true : false;
	return S_OK;
}

HRESULT IE_IOleControlSite::GetExtendedControl(IDispatch ** ppDisp)
{
	//DEBUG("IOleControlSite::GetExtendedControl");
	return E_NOTIMPL;
}

HRESULT IE_IOleControlSite::TransformCoords(POINTL * pPtlHimetric, POINTF * pPtfContainer, DWORD dwFlags)
{
	//DEBUG("IOleControlSite::TransformCoords");
	HRESULT hr = S_OK;

	if (pPtlHimetric == NULL)
	{
		return E_INVALIDARG;
	}
	if (pPtfContainer == NULL)
	{
		return E_INVALIDARG;
	}

	HDC hdc = ::GetDC(m_fs->m_hWndParent);
	::SetMapMode(hdc, MM_HIMETRIC);

	POINT rgptConvert[2];
	rgptConvert[0].x = 0;
	rgptConvert[0].y = 0;

	if (dwFlags & XFORMCOORDS_HIMETRICTOCONTAINER)
	{
		rgptConvert[1].x = pPtlHimetric->x;
		rgptConvert[1].y = pPtlHimetric->y;
		::LPtoDP(hdc, rgptConvert, 2);
		if (dwFlags & XFORMCOORDS_SIZE)
		{
			pPtfContainer->x = (float)(rgptConvert[1].x - rgptConvert[0].x);
			pPtfContainer->y = (float)(rgptConvert[0].y - rgptConvert[1].y);
		}
		else if (dwFlags & XFORMCOORDS_POSITION)
		{
			pPtfContainer->x = (float)rgptConvert[1].x;
			pPtfContainer->y = (float)rgptConvert[1].y;
		}
		else
		{
			hr = E_INVALIDARG;
		}
	}
	else if (dwFlags & XFORMCOORDS_CONTAINERTOHIMETRIC)
	{
		rgptConvert[1].x = (int)(pPtfContainer->x);
		rgptConvert[1].y = (int)(pPtfContainer->y);
		::DPtoLP(hdc, rgptConvert, 2);
		if (dwFlags & XFORMCOORDS_SIZE)
		{
			pPtlHimetric->x = rgptConvert[1].x - rgptConvert[0].x;
			pPtlHimetric->y = rgptConvert[0].y - rgptConvert[1].y;
		}
		else if (dwFlags & XFORMCOORDS_POSITION)
		{
			pPtlHimetric->x = rgptConvert[1].x;
			pPtlHimetric->y = rgptConvert[1].y;
		}
		else
		{
			hr = E_INVALIDARG;
		}
	}
	else
	{
		hr = E_INVALIDARG;
	}

	::ReleaseDC(m_fs->m_hWndParent, hdc); 
	return hr;
}

HRESULT IE_IOleControlSite::TranslateAccelerator(LPMSG pMsg, DWORD grfModifiers)
{
	//DEBUG("IOleControlSite::TranslateAccelerator");
	return E_NOTIMPL;
}

HRESULT IE_IOleControlSite::ShowContextMenu( DWORD dwID,POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved)
{
	//DEBUG("IOleControlSite::ShowContextMenu");
	return S_OK;
}


HRESULT IE_IOleControlSite::OnFocus(BOOL fGotFocus)
{
	//DEBUG("IOleControlSite::OnFocus");
	return S_OK;
}

HRESULT IE_IOleControlSite::ShowPropertyFrame()
{
	//DEBUG("IOleControlSite::ShowPropertyFrame");
	return E_NOTIMPL;
}

//IOleCommandTarget

HRESULT IE_IOleCommandTarget::QueryStatus(const GUID * pguidCmdGroup, ULONG cCmds, 
							   OLECMD * prgCmds, OLECMDTEXT * pCmdTet)
{
	//DEBUG("IOleCommandTarget::QueryStatus");
	if (prgCmds == NULL) 
		return E_INVALIDARG;
	bool bCmdGroupFound = false;

	for (ULONG nCmd = 0; nCmd < cCmds; nCmd++)
	{
		// unsupported by default
		prgCmds[nCmd].cmdf = 0;
	}

	if (!bCmdGroupFound) 
	{
		return OLECMDERR_E_UNKNOWNGROUP; 
	}
	return S_OK;
}

HRESULT IE_IOleCommandTarget::Exec(const GUID * pguidCmdGroup, DWORD nCmdID, 
						DWORD nCmdExecOpt, VARIANTARG * pVaIn, 
						VARIANTARG * pVaOut)
{
	//DEBUG("IOleCommandTarget::Exec");
	bool bCmdGroupFound = false;

	if (!bCmdGroupFound) 
	{ 
		return OLECMDERR_E_UNKNOWNGROUP; 
	}
	return OLECMDERR_E_NOTSUPPORTED;
}

//IAdviseSink

void STDMETHODCALLTYPE IE_IAdviseSink2::OnDataChange(FORMATETC * pFormatEtc, STGMEDIUM * pgStgMed)
{
	//DEBUG("IAdviseSink::OnDataChange");
}

void STDMETHODCALLTYPE IE_IAdviseSink2::OnViewChange(DWORD dwAspect, LONG lIndex)
{
	//DEBUG("IAdviseSink::OnViewChange");
	// redraw the control
	m_fs->m_IOleInPlaceSiteWindowless->InvalidateRect(NULL, FALSE);
}

void STDMETHODCALLTYPE IE_IAdviseSink2::OnRename(IMoniker * pmk)
{
	//DEBUG("IAdviseSink::OnRename");
}

void STDMETHODCALLTYPE IE_IAdviseSink2::OnSave()
{
	//DEBUG("IAdviseSink::OnSave");
}

void STDMETHODCALLTYPE IE_IAdviseSink2::OnClose()
{
	//DEBUG("IAdviseSink::OnClose");
}

//IAdviseSink2

void STDMETHODCALLTYPE IE_IAdviseSink2::OnLinkSrcChange(IMoniker * pmk)
{
	//DEBUG("IAdviseSink2::OnLinkSrcChange");
}

//IAdviseSinkEx

void STDMETHODCALLTYPE IE_IAdviseSinkEx::OnViewStatusChange(DWORD dwViewStatus)
{
	//DEBUG("IAdviseSinkEx::OnViewStatusChange");
}

//
HRESULT STDMETHODCALLTYPE IE_IDocHostUIHandler::GetHostInfo(DOCHOSTUIINFO __RPC_FAR *pInfo)
{
	pInfo->cbSize = sizeof(DOCHOSTUIINFO);
    pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER;
	return S_OK;
}

