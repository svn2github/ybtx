#ifndef  __lo_UNKNOWN_H__
#define  __lo_UNKNOWN_H__

#include <common/lo/lodefine.h>
#include <assert.h>

/** 定义 */
#ifndef DEFINE_GUID_LO

#define DEFINE_GUID_LO(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	const t_loGUID name = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

#define EXTERN_GUID_LO(name) extern const t_loGUID name;

#endif // DEFINE_GUID_LO

// uuid 值
#if defined(WIN32) || defined(WINCE)

#include <windows.h>
#include <unknwn.h>

#define t_loREFID REFIID
#define t_loGUID  GUID
typedef IUnknown   IloUnknown;

#define COMPARE_REFIID(left,right) left == right 

#else

typedef struct
{
	unsigned long  Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char  Data4[ 8 ];
}t_loGUID;



#define t_loREFID t_loGUID
BOOL compare_ref_id(const t_loREFID &left,const t_loREFID& );
#define COMPARE_REFIID(left,right) compare_ref_id(left,right)

BOOL compare_ref_id(const t_loREFID &left,const t_loREFID &right)
{
	BOOL bEque = ( 
	  (left.Data1 == right.Data1) &&
	  (left.Data2 == right.Data2) &&
	  (left.Data3 == right.Data3)
	  ) ;

	if( bEque )
	{
		for( int i = 0; i < 8 ; i ++ )
		{
			if( left.Data4[i]  != right.Data4[i] ) return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}


/** 基类接口 */
struct  /*__declspec(novtable)*/ IloUnknown
{
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(t_loREFID riid,void ** ppvObject) = 0;

	virtual ULONG STDMETHODCALLTYPE AddRef( void) = 0;

	virtual ULONG STDMETHODCALLTYPE Release( void) = 0;
};

// {C47EE582-7E56-4f0a-9BFF-772A1210ABF6}
DEFINE_GUID_LO(IID_IUnknown,
			   0xc47ee582, 0x7e56, 0x4f0a, 0x9b, 0xff, 0x77, 0x2a, 0x12, 0x10, 0xab, 0xf6);

#endif /* !WIN32 */



#ifndef DECLARE_IUNKNOWN_LO

//声明 IloUnknown 接口函数
#define DECLARE_IUNKNOWN_LO \
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(t_loREFID riid, void **ppv) { \
	return GetOwner()->QueryInterface(riid,ppv);}\
	virtual ULONG   STDMETHODCALLTYPE AddRef() {\
	return GetOwner()->AddRef();}\
	virtual ULONG   STDMETHODCALLTYPE Release() {\
	return GetOwner()->Release();}

#endif // DECLARE_IUNKNOWN_LO


/** 接口方式 */
DEFINE_NAMESPACE(locom)

/** 简单智能接口，类似于 CComPtr 
 *
 * @author loach 
 *
 * @date 2009-07-12
*/
template <typename T>
class CloComPtrT
{
public:
	CloComPtrT(){
		pUnknown_ = NULL;
	}
	CloComPtrT(T* p){
		pUnknown_ = p;
		AddRefM_lo(pUnknown_);
	}
	~CloComPtrT(){
		ReleaseM_lo(pUnknown_);
	}
	operator T*() const{
		return pUnknown_;
	}
	T& operator*() const
	{	
		return *pUnknown_;
	}
	T* operator ->() const{
		return pUnknown_;
	}

	T** operator&()
	{
		return &pUnknown_;
	}

	bool operator!() const
	{
		return (pUnknown_ == NULL);
	}

	bool operator != (T* pT) const
	{
		return !operator==(pT);
	}

	bool operator == (T* pT) const
	{
		return pUnknown_ == pT;
	}

	void AddRef()
	{
		if (pUnknown_)
		{
			pUnknown_->AddRef();
		}
	}
	// Release the interface and set to NULL
	void Release()
	{
		T* pTemp = pUnknown_;
		if (pTemp)
		{
			pUnknown_ = NULL;
			pTemp->Release();
		}
	}
	// Attach to an existing interface (does not AddRef)
	void Attach(T* p2)
	{
		T* o = pUnknown_;
		pUnknown_ = p2;
		ReleaseM_lo(o);
	}
	// Detach the interface (does not Release)
	T* Detach()
	{
		T* pt = pUnknown_;
		pUnknown_ = NULL;
		return pt;
	}
	T* pUnknown_;
};

struct IloNonDelegatingUnknown
{
	virtual HRESULT STDMETHODCALLTYPE  NonDelegatingQueryInterface( t_loREFID riid,void** ppvObject) = 0;
	virtual ULONG   STDMETHODCALLTYPE     NonDelegatingAddRef( void ) = 0;
	virtual ULONG  STDMETHODCALLTYPE      NonDelegatingRelease( void ) = 0;
};

/**
* @ingroup loa
*
* @author loach
*
* @data 2009-05-11
* 
* @brief 不知明的接口类
*/
class loNovtableM CloUnknown : public IloNonDelegatingUnknown
{
private:
	/* Owner of this object */
	IloUnknown* m_pUnk;

protected:
	/// 引用计数
	volatile long m_lRef;

public:
	/// Constructor
	CloUnknown (IloUnknown* pUnk){
		m_lRef = 0;
		m_pUnk = ( pUnk != 0 ? pUnk : reinterpret_cast<IloUnknown*>( static_cast<IloNonDelegatingUnknown*>(this) ) );
	}
	/// Destructor
	virtual ~CloUnknown (){};
public:
	/// 获取自己的
	IloUnknown* GetOwner(void) {
		return m_pUnk;
	};

	virtual HRESULT STDMETHODCALLTYPE NonDelegatingQueryInterface( t_loREFID riid,void** ppvObject)
	{
		if ( !ppvObject )
			return E_POINTER;

		if ( COMPARE_REFIID(riid ,IID_IUnknown) ){
			IloUnknown* pUnk = (IloUnknown*)( (IloNonDelegatingUnknown*)this);
			pUnk->AddRef();
			*ppvObject = pUnk;
			return S_OK;
		}
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}

	virtual ULONG STDMETHODCALLTYPE  NonDelegatingAddRef( void )
	{
#ifdef WIN32
		InterlockedIncrement( &m_lRef );
#else
//#error Please code
		m_lRef ++;
#endif
		return m_lRef;
	}

	virtual ULONG STDMETHODCALLTYPE  NonDelegatingRelease(void)
	{
#ifdef WIN32
		assert(m_lRef>0);
		if( 0 == InterlockedDecrement( &m_lRef ) ){
			delete this;
			return( 0 );
		}
#else
//#error Please code
		assert(m_lRef>0);
		m_lRef --;
		if( m_lRef <= 0 )
		{
			delete this;
			return 0;
		}
#endif
		return m_lRef;
	}
};

END_NAMESPACE(locom)

#endif /* __lo_UNKNOWN_H__ */