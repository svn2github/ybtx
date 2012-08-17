/* 
 +------------------------------------------------------+
 | 引导技能特效											|
 | Milk													|
 | 2007													|
 +------------------------------------------------------+
*/

#pragma once
#include "CEffectUnit.h"
#include "CModel.h"

namespace sqr
{
	class CLeadingPropCoder;
}

// 属性
class EFFECT_API CLeadingProp : public CEffectUnitProp
{
protected:
	enum eLP_LKType
	{	
		LPLK_IMM		= 0,
		LPLK_JUMP		= 1,
		LPLK_JUMPONCE	= 2,
	};

	enum eLP_StartScr
	{
		LPSS_Player		= 0,
		LPSS_LIST		= 1,
		LPSS_LINE		= 2,
	};

	float  m_fStartAngle;
	float  m_fRotSpeed;
	int32  m_nPieceCount;
	int32  m_nDouCount;
	float  m_fDouPercent;
	float  m_fDouInterval;
	float  m_fUVSpeed;

	SPR_FACE						m_eFaceType;
	SPR_BLENDMODE					m_eBlendModel;
	SPR_BLENDOP						m_eTextOp;
	Z_TEST							m_eZTest;
	BOOL							m_bLightEnable;
	float							m_fZBias;
	eLP_LKType						m_eLType;
	eLP_StartScr					m_eSSType;
	int32							m_JumpNum;
	int32							m_JumpSpeed;
	friend class CLeading;
	friend class CLeadingPropCoder;
public:
	CLeadingProp( uint32 ID, IEffectClassDesc* pEffectClassDesc );
	virtual ~CLeadingProp(void);

	virtual int32				LoadBin( IEffectRead& File );
	virtual int32				SaveBin( IEffectWrite& File );
	EBaseEffectUnit				GetUintType(void) { return eFlareSprite; }
};


// 实现
class EFFECT_API CLeading : public CEffectUnit//引导
{
	DYNAMIC_DECLARE_EFFECTUNITBASE( CLeading )
	float	  m_CurAngle;
	float	  m_CurDouTime;
	EVector<float>	  m_CutDouBias;
	EVector<float>	  m_CutDouBiasV;
	int		  m_OldDouCount;
	float	  m_CurUVBias;
	float		m_CurNum;
	float		m_Key;
	EMap< float, CVector3f >	m_DestVector;
public:
	CLeading( IEffectUnitProp* pEffectUnitProp );
	void	SetStartTime( uint32 STime,uint32 DTime );
	void	ResetTime( uint32 STime,uint32 DTime );
	virtual ~CLeading(void);
	EMap<float,CVector3f>& GetDestMap(void) { return m_DestVector; }				
	void				ChangeDouCount();
	
	void				SetRenderCallback( IEffectUnitHandler* pUnitHandler );
	const EFFECT_RESULT RenderEx( const CMatrix& matWorld,  RenderObjStyle* pRORS = NULL );
};
