#pragma once
#include "CEffectUnit.h"
#include "IAudioEmitter.h"

namespace sqr
{
	class CTDimAudioPropCoder;
}

class EFFECT_API CTDimAudioProp : public CEffectUnitProp
{
protected:
	EString					m_szAudioName;
	float					m_fPlayProbability;
	bool					m_bDelayStop;///< 特效结束后是否继续播放
	friend class CTDimAudio;
	friend class CTDimAudioPropCoder;
public:
	CTDimAudioProp( uint32 ID, IEffectClassDesc* pEffectClassDesc );
	virtual ~CTDimAudioProp(void);

	virtual int32				LoadBin( IEffectRead& File );
	virtual int32				SaveBin( IEffectWrite& File );
	EBaseEffectUnit				GetUintType(void) { return eTDimAudio; }
};

class EFFECT_API CTDimAudio : public CEffectUnit//音效
{
	DYNAMIC_DECLARE_EFFECTUNITBASE( CTDimAudio )

	bool	m_bNeedPlay;
	IAudioEmitterPtr m_pAEmitter;
	EString m_strLastName;
private:
	bool CheckNeedPlay();

public:
	CTDimAudio( IEffectUnitProp* pEffectUnitProp );
	virtual ~CTDimAudio(void);
	virtual void				SetVisible( bool isVisible );
	virtual void				SetRenderVelocity( float Velocity , int32 id = -1 );
	virtual void				SetStartTime( uint32 STime,uint32 DTime );
	virtual bool				SetProp( IEffectUnitProp* pEffectUnitProp );
	const EFFECT_RESULT			RenderEx( const CMatrix& matWorld,  RenderObjStyle* pRORS = NULL );

	static bool s_bIsEffectEditor;
};
