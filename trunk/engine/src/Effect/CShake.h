#pragma once
#include "ceffectunit.h"

namespace sqr
{
	class CShakePropCoder;
}

class EFFECT_API CShakeProp : public CEffectUnitProp
{
protected:
	CVector3f				m_vecDirCtrl;
	CVector3f				m_vecFrequence;
	float					m_fStrength;
	bool					m_isCtrlParent;

	friend class CShake;
	friend class CShakePropCoder;
public:
	CShakeProp( uint32 ID, IEffectClassDesc* pEffectClassDesc );
	virtual ~CShakeProp(void);

	virtual int32		LoadBin( IEffectRead& File );
	virtual int32		SaveBin( IEffectWrite& File );
	EBaseEffectUnit		GetUintType(void) { return eShake; }
};

class EFFECT_API CShake : public CEffectUnit//镜头震动
{
	DYNAMIC_DECLARE_EFFECTUNITBASE( CShake )
	float m_fRotateYDegree;
public:
	CShake( IEffectUnitProp* pEffectUnitProp );
	virtual ~CShake(void);
	void				SetRotate( float fY );
	const EFFECT_RESULT RenderEx( const CMatrix& matWorld,  RenderObjStyle* pRORS = NULL );	//渲染模型	
};
