#pragma once
#include "CEffectUnit.h"
#include "CModel.h"
#include "CQuaternion.h"

enum POLY_TRAIL_TYPE
{
	PTT_BLOCK		= 0,	// 块
	PTT_TRAIL		= 1,	// 带
	PTT_STRECTH		= 2,	// 拉伸

};

namespace sqr
{
	class CPolyTrailPropCoder;
}

class EFFECT_API CPolyTrailProp : public CEffectUnitProp
{
protected:
	CVector3f				m_vecRotate;
	float					m_MinTrailWidth;
	float					m_MaxTrailWidth;
	int32					m_Smooth;
	int32					m_DeclTime;
	int32					m_UnitLifeSpan;
	SPR_BLENDMODE			m_eBlendModel;
	SPR_BLENDOP				m_eTextOp;
	Z_TEST					m_eZTest;
	BOOL					m_bLightEnable;
	POLY_TRAIL_TYPE			m_eStrip;				// 0 single 
	BOOL					m_bSingleSide;
	IDNAME					m_sRefSkelatal;
	float					m_fRefractIndex;

	float					m_OrgScale;
	void					_UpdateParams(void);
	friend class CPolyTrail;
	friend class CPolyTrailPropCoder;

public:
	CPolyTrailProp( uint32 ID, IEffectClassDesc* pEffectClassDesc );
	virtual ~CPolyTrailProp(void);

	virtual int32				LoadBin( IEffectRead& File );
	virtual int32				SaveBin( IEffectWrite& File );
	EBaseEffectUnit				GetUintType(void) { return ePolyTrail; }
};

class EFFECT_API CPolyTrail : public CEffectUnit//多边形轨迹
{
	DYNAMIC_DECLARE_EFFECTUNITBASE( CPolyTrail )

	Trail*					m_pTrail;
	int16					m_nRefSkeletal;
	CMatrix					m_matRefence;
	int32					m_DeclTime;
	CQuaternion				m_LastQuat;
	CVector3f				m_LastVec;
	CVector3f				m_LastPos;
private:
	void UpdateToBuffer( const CPolyTrailProp* pProp, RenderObjStyle* pRORS );
	void UpdateToRefractBuffer(const CPolyTrailProp* pProp);
public:
	CPolyTrail( IEffectUnitProp* pEffectUnitProp );
	virtual ~CPolyTrail(void);

	virtual void				GetDesc( char * str )		{ sprintf( str, " 当前轨迹节数%d", m_TrailUnitNum ); }

	virtual void				OnLinked( CRenderable* pParentObj);
	virtual void				OnUnlinked( void );
	virtual const CMatrix&		GetMatrix();

	virtual void				Free();
	void						UpdateTrail( uint32 CurTime, bool bAlloc,const CMatrix& rMatrix = CMatrix::UNIT_MATRIX, const CVector3f& vScale = CVector3f::UNIT_SCALE );
	const EFFECT_RESULT			RenderEx( const CMatrix& matWorld,  RenderObjStyle* pRORS = NULL );	//渲染模型	
	Trail*						GetCurTrail(void);
	CVector3f					GetEndPos(Trail* pTrail);
	CVector3f					GetBeginPos(Trail* pTrail);
	//下面数据成员用于多边形回收
	int32							m_TrailUnitNum;
	EList<IEffectUnit*>::iterator	m_it;

	virtual void				RenderChild(IEffectUnit* pEffectUnit, uint32 uCurTime, RenderObjStyle* pRORS);
};
