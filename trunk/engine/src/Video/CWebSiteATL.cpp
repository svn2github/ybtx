#include "stdafx.h"
#include "CWebSiteATL.h"
#include "CWebBrowserATL.h"

CFrameSiteATL::CFrameSiteATL(CWebBrowserATL* win)
{
	m_cRef = 0;
	m_WebWin = win;

	m_IDispatch = new IE_IDispatch_ATL(this);
	m_IDispatch->AddRef();

	m_DWebBrowserEvents2 = new IE_DWebBrowserEvents2_ATL(this);
	m_DWebBrowserEvents2->AddRef();
}

CFrameSiteATL::~CFrameSiteATL(void)
{
	delete m_IDispatch;
	delete m_DWebBrowserEvents2;
}

void CFrameSiteATL::Clear()
{
	m_IDispatch->Release();
	m_DWebBrowserEvents2->Release();
}

//IDispatch

HRESULT IE_IDispatch_ATL::GetIDsOfNames(REFIID riid, OLECHAR ** rgszNames, unsigned int cNames,
								 LCID lcid, DISPID * rgDispId)
{
	return E_NOTIMPL;
}

HRESULT IE_IDispatch_ATL::GetTypeInfo(unsigned int iTInfo, LCID lcid, ITypeInfo ** ppTInfo)
{
	return E_NOTIMPL;
}

HRESULT IE_IDispatch_ATL::GetTypeInfoCount(unsigned int * pcTInfo)
{
	return E_NOTIMPL;
}


//#include "tracehelper.h"
HRESULT IE_IDispatch_ATL::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid,
						  WORD wFlags, DISPPARAMS * pDispParams,
						  VARIANT * pVarResult, EXCEPINFO * pExcepInfo,
						  unsigned int * puArgErr)
{
	if (wFlags & DISPATCH_PROPERTYGET)
	{
		if (pVarResult == NULL) 
			return E_INVALIDARG;
		switch (dispIdMember)
		{
			case DISPID_AMBIENT_APPEARANCE:
				break;
			case DISPID_AMBIENT_FORECOLOR:
				break;
			case DISPID_AMBIENT_BACKCOLOR:
				break;
			case DISPID_AMBIENT_LOCALEID:
				break;
			case DISPID_AMBIENT_USERMODE:
				break;
			case DISPID_AMBIENT_SHOWGRABHANDLES:
				break;
			case DISPID_AMBIENT_SHOWHATCHING:
				break;
			case DISPID_AMBIENT_DLCONTROL: // turn off javascript
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
			if(m_fs->m_WebWin->OnStartURL((char*)_bstr_t(vurl->bstrVal),!m_bNewURL))
			{
				*pDispParams->rgvarg->pboolVal = VARIANT_FALSE;
			}
			else
			{ 
				*pDispParams->rgvarg->pboolVal = VARIANT_TRUE;
			}
			break;
		}
		case DISPID_PROGRESSCHANGE:
		{
			break;
		}
		case DISPID_DOWNLOADCOMPLETE:
		{
			break;
		}
		case DISPID_DOCUMENTCOMPLETE:
		{
			VARIANT * vurl = pDispParams->rgvarg[0].pvarVal;
			m_fs->m_WebWin->OnFinishURL((char*)_bstr_t(vurl->bstrVal));
			break;
		}
		case DISPID_NAVIGATECOMPLETE2:
		{
			break;
		}
		case DISPID_DOWNLOADBEGIN:
		{
			break;
		}
		case DISPID_NEWWINDOW2:
		{ 
			break;
		}
		case DISPID_HTMLDOCUMENTEVENTS_ONMOUSEOVER:
	     {
			break;
		 }
	}
	return S_OK;
}
