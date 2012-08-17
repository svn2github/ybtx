#pragma once
#include "CEffectUnit.h"
#include "CPieceGroup.h"
#include "CAnimationGroup.h"
#include "CSkeletalFrame.h"
#include "CModel.h"
#include "ClipPlane.h"
//#include "CAsynFunParamsMgr.h"

namespace sqr
{
	class CModelConsolePropCoder;
}

class EFFECT_API CModelConsoleProp : public CEffectUnitProp
{
protected:
	CVector3f				m_vecRotate;
	EString					m_szARPName;
	int32					m_nPiece0;
	int32					m_nRS0;
	int32					m_nPiece1;
	int32					m_nRS1;
	int32					m_nPiece2;
	int32					m_nRS2;
	int32					m_nPiece3;
	int32					m_nRS3;
	int32					m_nPiece4;
	int32					m_nRS4;
	int32					m_nPiece5;
	int32					m_nRS5;
	EString					m_szARAName;
	int32					m_Animation;
	BOOL					m_bSynch;
	
	float					m_fMinXSize;	// 最大/最小尺寸
	float					m_fMinYSize;
	float					m_fMinZSize;

	float					m_fMaxXSize;
	float					m_fMaxYSize;
	float					m_fMaxZSize;

	SPR_FACE				m_eFaceType;
	float					m_fStartAngle;

	CVector2f				m_vecTextMoveVel;
	float					m_fTextAngularVel;
	float					m_fTextMaxScale;
	float					m_fTextMinScale;

	float					m_AngularVelocity;
	bool					m_bSetTextureMatrix;

	CPieceGroup*			m_pPieceGroup;
	CAnimationGroup*		m_pAnimationGroup;
	CSkeletalFrame*			m_pSkeletalFrame;
	//CAnimationsRes*		m_pAnimationGroup;
	int32					m_Mark;
	int						m_BirthTime;
	int						m_DeleteTime;
	EString					m_SkeName;


	EString					m_szParentSk;	// 与之连接的父骨骼名字
	EString					m_szSk;			// 连接的子骨骼名字
	int32					m_nSkIdx;

	ClipPlane				m_ClipPlane;
	EMap<float, float>		m_ClipDistMap;
	EMap<float, float>		m_mapRefract;
	bool					m_bEnableShadow;// 是否开启阴影
	void					_UpdateParams(void);
	friend class CModelConsole;
	friend class CModelConsolePropCoder;

public:
	CModelConsoleProp( uint32 ID, IEffectClassDesc* pEffectClassDesc );
	~CModelConsoleProp();
	bool					IsValid();

	virtual int32			LoadBin( IEffectRead& File );
	virtual int32			SaveBin( IEffectWrite& File );
	bool					CaculateProp();
	EBaseEffectUnit			GetUintType(void) { return eModelConsole; }
};

class EFFECT_API CModelConsole : public CEffectUnit//模型特效
{
	DYNAMIC_DECLARE_EFFECTUNITBASE( CModelConsole )

	float					m_CurAngle;
	CModel*					m_pModel;
	float					m_lastPercent;
	bool					m_mark;
	float					m_move;
	int						m_count;
	int						m_counter;
	IEffectUnitProp*		m_pLoadUnitProp;
	CMatrix					m_ParentMat;
	float					m_DelayRender;
	float					m_BirthRender;
	IDNAME					m_AniName;
public:
	CModelConsole( IEffectUnitProp* pEffectUnitProp );
	virtual ~CModelConsole(void);
	virtual bool SetProp( IEffectUnitProp* pEffectUnitProp );
	void SetVisible( bool isVisible );
	void SetStartTime( uint32 STime,uint32 DTime );
	void ResetTime( uint32 STime,uint32 DTime );
	void OnUnlinked( void );
	void						RenderChild(IEffectUnit* pEffectUnit, uint32 uCurTime, RenderObjStyle* pRORS);
	const EFFECT_RESULT			RenderEx( const CMatrix& matWorld,  RenderObjStyle* pRORS = NULL );	//渲染模型	
	const EFFECT_RESULT			RenderEx( const CMatrix& matWorld,uint32 uCurTime,  RenderObjStyle* pRORS = NULL);
	virtual const AniResult		Render( const CMatrix& matWorld, uint32 uCurTime,  RenderObjStyle* pRORS = NULL );		//渲染模型	
	virtual void				Free();
	void						RejustTime(uint32& uCurTime);
};
