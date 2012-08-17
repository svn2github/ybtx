#pragma once
#include "ceffectunit.h"

namespace sqr
{
	class CDynamicLightPropCoder;
}

class EFFECT_API CDynamicLightProp : public CEffectUnitProp
{
protected:
	float		m_fRange;
	float		m_fMinStrong;
	float		m_fMaxStrong;
	void		_UpdateParams(void);
	friend class CDynamicLight;
	friend class CDynamicLightPropCoder;
public:
	CDynamicLightProp( uint32 ID, IEffectClassDesc* pEffectClassDesc );
	virtual ~CDynamicLightProp(void);

	virtual int32			LoadBin( IEffectRead& File );
	virtual int32			SaveBin( IEffectWrite& File );
	EBaseEffectUnit			GetUintType(void) { return eDynamicLight; }
};

class EFFECT_API CDynamicLight : public CEffectUnit//动态光
{
	DYNAMIC_DECLARE_EFFECTUNITBASE( CDynamicLight )
public:
	CDynamicLight( IEffectUnitProp* pEffectUnitProp );
	virtual ~CDynamicLight(void);

	const EFFECT_RESULT RenderEx( const CMatrix& matWorld,  RenderObjStyle* pRORS = NULL );	//渲染模型	
};
