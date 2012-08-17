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

namespace sqr
{
	class CWebBrowserATL;
}

class IE_IDispatch_ATL;
class IE_DWebBrowserEvents2_ATL;

class CFrameSiteATL :public IUnknown
{
	friend class CWebBrowserAtl;
	friend class IE_IDispatch;
	friend class IE_DWebBrowserEvents2;

public:
	CFrameSiteATL(CWebBrowserATL* win);
	~CFrameSiteATL(void);

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID* ppv)
	{
		if(ppv == NULL ) return E_INVALIDARG;
		*ppv=0;
		if(riid == IID_IUnknown)
			*ppv = (IUnknown*)this;
		else if (riid == IID_IDispatch)
			*ppv = m_IDispatch;
		else if (riid == DIID_DWebBrowserEvents2)
			*ppv = m_DWebBrowserEvents2;
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
	IE_IDispatch_ATL*				m_IDispatch;
	IE_DWebBrowserEvents2_ATL*		m_DWebBrowserEvents2;
	CWebBrowserATL*					m_WebWin;
};

class IE_IDispatch_ATL : public IDispatch
{
public:
	IE_IDispatch_ATL(CFrameSiteATL* fs) 
	{ 
		m_fs = fs; 
		m_bNewURL=0;
		m_bOnLink=false;
	}
	~IE_IDispatch_ATL()
	{}

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
	CFrameSiteATL * m_fs;
	unsigned int m_bNewURL;
	bool m_bOnLink;
};


class IE_DWebBrowserEvents2_ATL : public DWebBrowserEvents2
{
public:
	IE_DWebBrowserEvents2_ATL(CFrameSiteATL* fs)
	{ 
		m_fs = fs; 
	}
	~IE_DWebBrowserEvents2_ATL() 
	{}

	//IUnknown
	STDMETHODIMP QueryInterface(REFIID iid, void ** ppvObject) 
	{ 
		return m_fs->QueryInterface(iid, ppvObject); 
	}
	ULONG STDMETHODCALLTYPE AddRef() 
	{ return m_fs->AddRef(); }
	ULONG STDMETHODCALLTYPE Release() 
	{ return m_fs->Release(); }
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
	CFrameSiteATL * m_fs;
};
