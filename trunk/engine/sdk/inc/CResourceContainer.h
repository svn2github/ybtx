#pragma once
#include "CPkgFile.h"
#include "TSingleton.h"
#include "CGraphicMallocObject.h"

namespace sqr
{

class CBaseGraphicRead : public CGraphicMallocObject
{
protected:
	CBaseGraphicRead**	m_ppMyself;
	GString			m_szName;
	GWString		m_szAlias;
public:
	CBaseGraphicRead( const string& szName, const wstring& szAlias );
	virtual ~CBaseGraphicRead();

	virtual void	OnRead( CRefBufFile& GraphicFile ) = 0;
	virtual int32	CreateResFile();
	static  void	OnReadAsyn( void* pBaseRead, CRefBufFile& GraphicFile );
	virtual bool	IsValid()
	{
		return !m_ppMyself;
	}
	void			SetValid();

	inline const char* GetName() const
	{
		return m_szName.c_str();
	}

	inline const wchar_t* GetAlias() const
	{
		return m_szAlias.c_str();
	}
};

enum EResType
{
	eRT_eArp	= 0,
	eRT_eAra	= 1,
	eRT_eRs		= 2,
	eRT_eTex	= 3,
	eRT_eFont   = 4,
	eRT_eCl		= 5,
	eRT_eAgp	= 6,

	eRT_eCount,
};

class CResourceContainer;
class CResourceRecycle;

class CResUnit : public CBaseGraphicRead
{
	friend class CResourceRecycle;
protected:
	CResourceRecycle*	m_pRecycle;
	CResourceContainer* m_pContainer;	
	EResType			m_eType;
	bool				m_IsRecycle;
	uint32				m_uSize;
	bool				m_bInRecycle;
public:
	CResUnit( const string& szName, const wstring& szAlias, EResType eType );
	virtual ~CResUnit();
	virtual int32	AddRef() = 0;
	virtual void	SetBufSize( uint32 uSize );
	
	int32	CreateResFile();
	void	FreeRes();
	uint32	GetBufSize()
	{
		return m_uSize;
	};
	void	Reclaim();
	bool	IsRecycle(void) { return m_IsRecycle; }

	void	SetContainerAndRecycle( CResourceContainer* pContainer, CResourceRecycle* pRecycle );

	CResourceContainer* Container() const
	{
		return m_pContainer;
	}
	CResourceRecycle* Recycle() const
	{
		return m_pRecycle;
	}
protected:
	virtual void _FreeRes(void) {};
	virtual void _Reclaim(void) {};
};

class CResourceContainer  : public CGraphicMallocObject
{
protected:
	GMap< GString, CResUnit* > m_ResContainer;

public:
	~CResourceContainer();
	CResUnit* GetResFromContainer( const string& Name, EResType eType );
	void AddResToContainer( const string& Name, CResUnit* Res, EResType eType );
	void Erase( const string& Name, EResType eType );
};

class CResourceRecycle
{
	uint32	m_uCurBufSize;
	uint32	m_uMaxBufSize;

protected:
	typedef GList< CResUnit* >						RecyclePool;
	typedef GMap< GString,RecyclePool::iterator >	ResourceContainer;
	RecyclePool			m_ResPool;
	ResourceContainer	m_ResContainer;

public:

	CResourceRecycle( uint32 uBufSize ) : m_uMaxBufSize( uBufSize ), m_uCurBufSize(0) {}
	~CResourceRecycle();
	CResUnit* GetResFromContainer( const string& Name, EResType eType );

	void AddResToContainer( const string& Name, CResUnit* Res, EResType eType );
	bool Erase( const string& Name, EResType eType );


	uint32 MaxBufSize();
	void SetMaxBufSize(uint32 uBufSize);
	uint32 CurBufSize();
	void ChangeSize( int32 nSize );
	void Clear();
	void OptimizeRecycle(void);
};

class CResMgr 
	: public Singleton<CResMgr>
	, public CGraphicMallocObject
{
	CResourceContainer		m_VideoMemContainer;
	CResourceRecycle		m_VideoMemRecycle;
	CResourceContainer		m_SystemMemContainer;
	CResourceRecycle		m_SystemMemRecycle;
public:
	CResMgr();

	static uint32 GetVedioMemSize(void);

	CResUnit* FindResInVedioMem( const string& szName, EResType eType )
	{
		CResUnit* pRes = m_VideoMemContainer.GetResFromContainer( szName, eType );
		if ( !pRes )
			pRes = m_VideoMemRecycle.GetResFromContainer( szName, eType );
		return pRes;
	}

	CResUnit* FindResInSystemMem( const string& szName, EResType eType )
	{
		CResUnit* pRes = m_SystemMemContainer.GetResFromContainer( szName, eType );
		if ( !pRes )
			pRes = m_SystemMemRecycle.GetResFromContainer( szName, eType );
		return pRes;
	}

	void Clear()
	{
		m_SystemMemRecycle.Clear();
		m_VideoMemRecycle.Clear();
	}

	void OptimizeRecycle(void)
	{
		m_SystemMemRecycle.OptimizeRecycle();
		m_VideoMemRecycle.OptimizeRecycle();
	}

	CResourceContainer* VideoMemContainer()
	{
		return &m_VideoMemContainer;
	}

	CResourceRecycle* VideoMemRecycle()
	{
		return &m_VideoMemRecycle;
	}

	CResourceContainer* SystemMemContainer()
	{
		return &m_SystemMemContainer;
	}

	CResourceRecycle* SystemMemRecycle()
	{
		return &m_SystemMemRecycle;
	}
};

inline uint32 CResourceRecycle::MaxBufSize()
{
	return m_uMaxBufSize;
}

inline void CResourceRecycle::SetMaxBufSize(uint32 uBufSize)
{
	m_uMaxBufSize = uBufSize;
}

inline uint32 CResourceRecycle::CurBufSize()
{
	return m_uCurBufSize;
}

}
