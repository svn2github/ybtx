#pragma once
#include "IEffect.h"
#include "IEffectUnit.h"

class CEffectManager;
class CEffectProp;

//////////////////////////////////////////////////////////////////////////

class CEffect :	public IEffect
{
public:
	struct EFFECTKEY
	{
		EFFECTKEY();
		IEffectUnit*				pParentEffectUnit;
		IEffectUnit*				pEffectUnit;
	};

protected:
	uint32							m_CurTime;
	float							m_fCurTime;

	CSkeletalFrame*					m_PreSkeFrame;
	CBaseModel*						m_pModel;

	bool							m_IsAniEnd;
	//bool							m_bFollowBone;

	const IDNAME*					m_FrameString;

	//CVector3f						m_vecScale;
	//CVector3f						m_vecOff;
	//CAxisAlignedBox				m_AABB;

	AniResult						m_Result;

	bool							m_bVisible;

	//EMap<EString,CRenderable*>			m_CenterLink;		//和物体中心挂钩的模型

	void							ResetUnitStartTime(uint32 uCurTime);
	//void							ResetVelocityUnit(uint32 uCurTime);
	//void							ResetNormalUnit(uint32 uCurTime);
	//EString						m_EffectName;
	//float							m_fVelocity;

protected:

	static	float					st_EffectLod;
	static	bool					st_CameraShake;
	CEffectManager*					m_pEffectMgr;
	CEffectProp*					m_pEffectProp;
	EVector< EMap< int, EFFECTKEY > >	m_Tracks;
public:
	static	void				SetEffectRenderLod(float rLod);
	static	float				GetEffectRenderLod(void);

	static	void				SetCameraShake(bool b);
	static	bool				GetCameraShake(void);

public:

	CEffect( CEffectManager* pEffectMgr, CEffectProp* pEffectProp );
	CEffect( CEffectManager* pEffectMgr);
	virtual ~CEffect(void);

	void InitEffect(CEffectProp* pEffectProp );
	virtual ERenderObjType			GetType();
	virtual bool					IsValid();;
	virtual bool					IsFollowBone(CRenderable* pParentObj);
	virtual const char*				GetEffectName();
	virtual const char*				GetFullEffectName();
	virtual int						GetAttachAniNum();
	virtual const char*				GetAttachAniName( int Num );
	virtual const char*				GetUserDesc();
	virtual int						GetTimeRange();
	virtual void					SetRenderCallback( IEffectUnitHandler* pUnitHandler, const char* szUnitName );
	virtual void					SetRenderCallback( IEffectUnitHandler* pUnitHandler, EBaseEffectUnit EUnit );
	//virtual const CVector3f&		GetBoxScale();
	//virtual const CVector3f&		GetBoxOff();
	virtual const float				GetAABBHeight();
	virtual IntersectFlag			IntersectRay( const CMatrix& transform, const CVector3f& rayPos, const CVector3f& rayDir ){return IF_IMPOSSIBLE;}

	virtual void					OnLinked( CRenderable* pParentObj );
	virtual void					OnUnlinked( void );
	//virtual void					OnPreRender( const AniResult& ParentAniRes, float FrameRatio );
	virtual void					GetCurAnimationInfo( float& CurFrame, uint32& CurTime );
	//virtual bool					SetNextAnimation( IDNAME AniName, FramePair FmPair, bool bLoop = true, int32 DelayTime = 200 /*毫秒*/, float AniSpeed = -1.0f  );

	virtual const AniResult		Render( const CMatrix& matWorld,  RenderObjStyle* pRORS = NULL );	//渲染模型	
	virtual bool				UpdateTime(uint32 uCurTime);
	virtual bool				UpdateEfx(uint32 uCurTime, bool bReset = true);
	virtual void				SetVisible( bool isVisible );
	virtual bool				IsFrameSynch(void);
	virtual void				SetUnitVisibleOnRender(int track,int key);

};

//------------------------------------------------------------------------------
inline void
CEffect::SetEffectRenderLod( float rLod )
{
	st_EffectLod = rLod;
}

//------------------------------------------------------------------------------
inline float
CEffect::GetEffectRenderLod( void )
{
	return st_EffectLod;
}

//------------------------------------------------------------------------------
inline void
CEffect::SetCameraShake( bool b )
{
	st_CameraShake = b;
}

//------------------------------------------------------------------------------
inline bool
CEffect::GetCameraShake( void )
{
	return st_CameraShake;
}
