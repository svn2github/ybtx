#pragma once
#include "CEffectUnit.h"
#include "CModel.h"

// 实现公告板等
class EFFECT_API CSpriteProp : public CEffectUnitProp
{
public:
	CVector3f				m_vecRotate;
	CVector3f				m_vecNormal;
	float					m_fStartAngle;
	float					m_fWidthRatio;
	float					m_fMinSize;
	float					m_fMaxSize;
	SPR_FACE				m_eFaceType;
	SPR_BLENDMODE			m_eBlendModel;
	Z_TEST					m_eZTest;
	SPR_BLENDOP				m_eTextOp;
	BOOL					m_bLightEnable;
	float					m_fZBias;

	BOOL					m_isShowFrame;

	CMatrix					m_matNormal;
	float					m_AngularVelocity;
	int						m_BirthTime;
	int						m_DeleteTime;
	void					_UpdateParams(void);
public:
	CSpriteProp( uint32 ID, IEffectClassDesc* pEffectClassDesc );
	virtual ~CSpriteProp(void);
	virtual int32			LoadBin( IEffectRead& File );
	virtual int32			SaveBin( IEffectWrite& File );
	virtual bool			GetMaxSize(float& fValue);
	void					CaculateProp();
	virtual void			_CalculateAABB();

	EBaseEffectUnit			GetUintType(void) { return eSprite; }
};

class EFFECT_API CSprite : public CEffectUnit//公告板
{
	DYNAMIC_DECLARE_EFFECTUNITBASE( CSprite )

protected:
	CMatrix					m_ParentMat;
	float					m_CurAngle;
	float					m_DelayRender;
	float					m_BirthRender;
public:
	CSprite( IEffectUnitProp* pEffectUnitProp );
	virtual ~CSprite(void);
	void				ResetTime( uint32 STime,uint32 DTime );
	virtual void		OnLinked( CRenderable* pParentObj  );
	virtual void		OnUnlinked(void);
	const EFFECT_RESULT	RenderEx( const CMatrix& matWorld,uint32 uCurTime,  RenderObjStyle* pRORS = NULL);
	const EFFECT_RESULT	RenderEx( const CMatrix& matWorld,  RenderObjStyle* pRORS = NULL );	//渲染模型	
	virtual void		Free();
};
