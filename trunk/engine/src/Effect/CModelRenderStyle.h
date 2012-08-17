#pragma once
#include "CEffectUnit.h"
#include "CModel.h"
#include "ClipPlane.h"

namespace sqr
{
	class CModelRenderStylePropCoder;
}

class EFFECT_API CModelRenderStyleProp : public CEffectUnitProp
{
protected:
	EVector<EString>			m_ApplyClass;
	CColor					m_SpecularColor;
	CVector2f				m_vecTextMoveVel;
	float					m_fTextAngularVel;

	BOOL					m_bDiffuseAmbient;
	BOOL					m_bSpecular;
	bool					m_bSetTextureMatrix;
	ClipPlane				m_ClipPlane;
	EMap<float, float>		m_ClipDistMap;
	void					_UpdateParams(void);
	friend class CModelRenderStyle;
	friend class CModelRenderStylePropCoder;
public:
	CModelRenderStyleProp( uint32 ID, IEffectClassDesc* pEffectClassDesc );
	virtual ~CModelRenderStyleProp(void);

	virtual int32			LoadBin( IEffectRead& File );
	virtual int32			SaveBin( IEffectWrite& File );
	void CaculateProp();
	EBaseEffectUnit		GetUintType(void) { return eModelRenderStyle; }
};

class EFFECT_API CModelRenderStyle : public CEffectUnit//渲染类型
{
	DYNAMIC_DECLARE_EFFECTUNITBASE( CModelRenderStyle )

	static EMap< CRenderable*, CModelRenderStyle* > s_LinkList;

	CModelRenderStyle*			m_pPreUnit;
	CModelRenderStyle*			m_pNexUnit;
	bool						m_bNeedClear;

	void			SetRenderStyleToParent( uint32 CurTime, const CMatrix& matWorld );

public:
	CModelRenderStyle( IEffectUnitProp* pEffectUnitProp );
	virtual ~CModelRenderStyle(void);

	virtual bool SetProp( IEffectUnitProp* pEffectUnitProp );

	virtual void	OnLinked( CRenderable* pParentObj );
	virtual void	OnUnlinked( void );

	const EFFECT_RESULT RenderEx( const CMatrix& matWorld,  RenderObjStyle* pRORS = NULL );	//渲染模型	
};
