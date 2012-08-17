#pragma once
#include "CEffectUnit.h"
#include "CModel.h"

namespace sqr
{
	class CLightningPropCoder;
}

// 实现闪电
// 属性
class EFFECT_API CLightningProp : public CEffectUnitProp
{
protected:
	CVector3f				m_vecRotate;
	CVector3f				m_vecNormal;
	float					m_fStartAngle;
	float					m_fWidthRatio;
	float					m_fMinSize;
	float					m_fMaxSize;
	SPR_FACE				m_eFaceType;
	SPR_BLENDMODE			m_eBlendModel;
	Z_TEST					m_eZTest;
	BOOL					m_bLightEnable;
	float					m_fZBias;
	CVector3f				m_vecSrc;			// 起点
	float					m_fDitherMax;		// 震动幅度
	float					m_fDitherMin;		// 震动幅度
	int32					m_nSubNum;			// 分段数目

	CMatrix					m_matNormal;
	float					m_AngularVelocity;
	void					_UpdateParams(void);
	friend class CLightning;
	friend class CLightningPropCoder;

public:
	CLightningProp( uint32 ID, IEffectClassDesc* pEffectClassDesc );
	virtual ~CLightningProp(void);

	virtual int32		LoadBin( IEffectRead& File );
	virtual int32		SaveBin( IEffectWrite& File );
	EBaseEffectUnit		GetUintType(void) { return eModelConsole; }
};


// 实现
class EFFECT_API CLightning : public CEffectUnit//闪电
{
	DYNAMIC_DECLARE_EFFECTUNITBASE( CLightning )

	float					m_CurAngle;
	int32					m_nSubTick;

public:
	CLightning( IEffectUnitProp* pEffectUnitProp );
	virtual ~CLightning(void);
	const EFFECT_RESULT RenderEx( const CMatrix& matWorld,  RenderObjStyle* pRORS = NULL );	//渲染模型	
};
