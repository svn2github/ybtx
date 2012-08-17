#pragma once
#include "CEffectUnit.h"
#include "CPostFilter.h"

namespace sqr
{
	class CCameraFilterPropCoder;
}

enum CAMERAFILTER_RATIO
{
	CAMERAFILTER_RATIO_NONE			= 0,
	CAMERAFILTER_RATIO_CLAMP		= 1,
	CAMERAFILTER_RATIO_DWORD		= 0xFFFFFFFF
};

class EFFECT_API CCameraFilterProp : public CEffectUnitProp
{
	friend class CCameraFilterPropCoder;
protected:
	SPR_BLENDMODE			m_eBlendModel;
	Z_TEST					m_eZTest;
	float					m_xScale;
	float					m_yScale;
	float					m_zScale;
	bool					m_bFilterParent;
	CAMERAFILTER_RATIO		m_eCFilterTp;
	//ÂÇ¾µÌí¼Ó½¥ÏÔ½¥Òþ¹¦ÄÜ
	int						m_BirthTime;
	int						m_DeleteTime;
	friend class CCameraFilter;
public:
	CCameraFilterProp( uint32 ID, IEffectClassDesc* pEffectClassDesc );
	virtual ~CCameraFilterProp(void);

	virtual int32			LoadBin( IEffectRead& File );
	virtual int32			SaveBin( IEffectWrite& File );
	EBaseEffectUnit			GetUintType(void) { return eCameraFilter; }
};

class EFFECT_API CCameraFilter : public CEffectUnit//ÂË¾µ
{
	DYNAMIC_DECLARE_EFFECTUNITBASE( CCameraFilter )
	CPostFilter*				m_pPostFilter;
	float					m_DelayRender;
	float					m_BirthRender;
	bool					m_bFinishFirstPlay;
public:
	CCameraFilter( IEffectUnitProp* pEffectUnitProp );
	virtual ~CCameraFilter(void);
	const EFFECT_RESULT	RenderEx( const CMatrix& matWorld,uint32 uCurTime,  RenderObjStyle* pRORS = NULL);
	const EFFECT_RESULT RenderEx( const CMatrix& matWorld,  RenderObjStyle* pRORS = NULL );	//äÖÈ¾Ä£ÐÍ	
	virtual void		Free();
};
