#pragma once

#include "IEffectUnit.h"
#include "IEffectManager.h"
#include "BaseHelper.h"

class CEffectFile;

namespace sqr
{
	class CEffectPropCoder;
}

class CBaseEffectGroup : public IEffectGroup
{
	uint32					m_EffectRef;

protected:
	CBaseEffectGroup(void):m_EffectRef(0){}
	virtual ~CBaseEffectGroup(){}
	virtual void DeleteThis(){};
	


public:
	virtual const char*	GetName() const;
	
	UINT			AddEffectRef()				
	{ 
		return ++m_EffectRef; 
	}

	virtual void	EffectRelease()				
	{ 
		if( --m_EffectRef == 0 && GetRefCount() == 0 )
			DeleteThis(); 
	};

	virtual void	Release();
	virtual void	Free();
};

struct AttachKey
{
	AttachKey() : nTrack(-1), nKeyStartTime(-1){}
	int nTrack;
	int nKeyStartTime;
};

class CEffectProp
{
	friend class CEffectPropCoder;

protected:

	IEffectManager*							m_pEffectMgr;
	CBaseEffectGroup*						m_pEffectGroup;
	int32									m_Ref;

	EVector< EMap< int, IEffectUnitProp* > >	m_Tracks;
	EMap< IEffectUnitProp*, AttachKey >		m_AttachKey;

	int										m_TimeRange;
	EVector<EString>						m_AttachAniName;
	EString									m_UserDesc;
	EString									m_szFullEffectName;
	int										m_nEffectNamePos;
	bool									m_bLoadRes;
	bool									m_bFrameSynch;       //添加帧同步信息
	bool									m_bForbidScale;

public:
	CEffectProp( CBaseEffectGroup* pEffectGroup, IEffectManager* pEffectMgr, const EString& szName ) 
		: m_pEffectGroup( pEffectGroup )
		, m_pEffectMgr(pEffectMgr)
		, m_szFullEffectName(szName)
		, m_Ref(1)
		, m_bLoadRes(false)
		, m_TimeRange(0)
		, m_bFrameSynch(false)
		, m_bForbidScale(false)
	{
		m_nEffectNamePos = (int)m_szFullEffectName.length();
		const char* pStr = m_szFullEffectName.c_str();
		for( ; m_nEffectNamePos; m_nEffectNamePos-- )
			if( pStr[ m_nEffectNamePos - 1 ] == '\\' )
				break;
	}
	virtual CEffectProp::~CEffectProp()
	{
		for( int i = 0; i < (int)m_Tracks.size(); i++ )
		{
			EMap<int,IEffectUnitProp*>& nTrack = m_Tracks[i];
			for( EMap<int,IEffectUnitProp*>::iterator it = nTrack.begin(); it != nTrack.end(); it++ )
				SafeRelease( it->second );
		}
	}

	const char*					GetEffectFileName();
	const char*					GetEffectName()							{ return m_szFullEffectName.c_str() + m_nEffectNamePos; }
	const char*					GetFullEffectName()						{ return m_szFullEffectName.c_str(); }
	int							GetAttachAniNum()						{ return (int)m_AttachAniName.size(); }
	const char*					GetAttachAniName( int Num )				{ return m_AttachAniName[Num].c_str(); }
	const char*					GetUserDesc()							{ return m_UserDesc.c_str(); }
	int							GetTimeRange()							{ return m_TimeRange; }
	int							GetTracksNum()							{ return (int)m_Tracks.size(); }
	EMap<int,IEffectUnitProp*>&	GetTrack( int nNum )					{ return m_Tracks[nNum]; }
	AttachKey					GetAttachKey( IEffectUnitProp* pProp )	{ return m_AttachKey[pProp]; }
	EMap< IEffectUnitProp*, AttachKey >& GetAttachKeys()				{ return m_AttachKey;}

	int32						AddRef()								{ m_pEffectGroup->AddEffectRef(); return ++m_Ref; }
	void						Release()								{ if( m_Ref ){ m_Ref--; m_pEffectGroup->EffectRelease(); } }

	int		LoadBin( CEffectFile& File, uint32 dwVer );
	int		LoadFullRes();
	bool		GetForbidScale()	{ return m_bForbidScale; }
	bool		GetFrameSynch()		{ return m_bFrameSynch;}
	bool		GetLoadRes()		{ return m_bLoadRes;}

	void	_FreeRes(void);
	void	_Reclaim(void);
};

class CEffectGroup : public CBaseEffectGroup
{
protected:

	EString						m_szName;
	IEffectManager*				m_pEffectMgr;
	EMap< EString, CEffectProp* >	m_EffectProps;
	bool						m_bValid;

	CEffectGroup( const CEffectGroup & );
	CEffectGroup& operator = ( const CEffectGroup & );
	CEffectGroup( IEffectManager* pEffectMgr, const char* szFileName );

	friend class CEffectManager;
	friend class CEffectPropCoder;

	bool						m_isOnRead;

public:
	
	virtual ~CEffectGroup(void);
	virtual void DeleteThis();
	const char*	GetName() const {return m_szName.c_str();}

	int						BinLoad( CEffectFile& File );
	CEffectProp*			GetEffectProp( const char* szEffectName );
	virtual uint32			GetEffectNum();
	virtual const char*		GetEffectName( uint32 Num );
	virtual bool			IsValid(){ return m_bValid; };
	virtual CEffectProp*	CreateEffectPropFun(const char* szEffect);

	void	_FreeRes(void);
	void	_Reclaim(void);
};
