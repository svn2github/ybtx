#pragma once
#include "CEffectUnit.h"

namespace sqr
{
	class CNullPropCoder;
}

class EFFECT_API CNullProp : public CEffectUnitProp
{
protected:
	enum SNULL_FACE
	{
		SNLF_NORMAL			= 0,
		SNLF_TERRAIN		= 1,
		SNLF_TERRAIN_ROTATE	= 2,
		SNLF_FORCE_DWORD	= 0xFFFFFFFF
	};

protected:
	bool					m_bDelay;
	bool					m_bForbidScale;
	CVector3f				m_vecRotate;
	float					m_fAngularVelocity;
	BOOL					m_bFolowSpeed;
	void					_UpdateParams(void);
	SNULL_FACE				m_eFaceType;

	friend class CNull;
	friend class CNullPropCoder;
public:
	CNullProp( uint32 ID, IEffectClassDesc* pEffectClassDesc );
	virtual ~CNullProp(void);

	virtual int32				LoadBin( IEffectRead& File );
	virtual int32				SaveBin( IEffectWrite& File );
	EBaseEffectUnit				GetUintType(void) { return eNull; }
};

class EFFECT_API CNull : public CEffectUnit//¿Õ
{
	DYNAMIC_DECLARE_EFFECTUNITBASE( CNull )

	float					m_CurAngle;
	bool					m_bDelay;
public:
	CNull( IEffectUnitProp* pEffectUnitProp );
	virtual ~CNull(void);
	void ResetTime( uint32 STime,uint32 DTime );
	void SetStartTime( uint32 STime,uint32 DTime );
	const EFFECT_RESULT RenderEx( const CMatrix& matWorld,  RenderObjStyle* pRORS = NULL );	//äÖÈ¾Ä£ÐÍ	
	//virtual void				RenderChild(IEffectUnit* pEffectUnit, uint32 uCurTime, RenderObjStyle* pRORS);
};