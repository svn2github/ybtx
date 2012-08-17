#include "stdafx.h"
#include "CTDimAudio.h"
#include "IAudioModule.h"

DYNAMIC_CREATE_EFFECTUNITBASE( eTDimAudio, IDS_AUDIO, CTDimAudio, CTDimAudioProp )

enum
{
	TDIMAUDIO_ORIGIN,			
	TDIMAUDIO_ADD_DISTANCE, 		
	TDIMAUDIO_REMOVE_DISTANCE,	
	TDIMAUDIO_ADD_DELAY_STOP,			
};

CTDimAudioProp::CTDimAudioProp( uint32 ID, IEffectClassDesc* pEffectClassDesc )
		: CEffectUnitProp( ID, pEffectClassDesc )
		, m_fPlayProbability(1.0f)
		, m_bDelayStop(false)
{
	// empty
}

CTDimAudioProp::~CTDimAudioProp(void)
{
	// empty
}

int32 CTDimAudioProp::LoadBin( IEffectRead& File )
{
	uint32 Version;
	File.read( (char*)&Version,					sizeof(uint32) );

	float temp;
	switch ( Version )
	{
	case TDIMAUDIO_ADD_DISTANCE:
		{
			File.read( (char*)&m_fPlayProbability,   sizeof( float ) );
			File.read( (char*)&temp, sizeof( float ) );
			File.read( (char*)&temp, sizeof( float ) );
			LoadStringT( File, m_szAudioName );
			m_szAudioName.clear();
		}
		break;

	case TDIMAUDIO_ADD_DELAY_STOP:
		File.read( &m_bDelayStop,	sizeof( bool ) );
	case TDIMAUDIO_REMOVE_DISTANCE:
		File.read( &m_fPlayProbability,	sizeof( float ) );
	default:
		LoadStringT( File, m_szAudioName );
	}

	if ( FAILED( CEffectUnitProp::LoadBin( File ) ) )
		return GERROR;

	return 0;
}

int32 CTDimAudioProp::SaveBin( IEffectWrite& File )
{
	uint32 Version = TDIMAUDIO_ADD_DELAY_STOP;
	File.write( &Version, sizeof(uint32) );

	File.write(&m_bDelayStop, sizeof(bool));
	File.write(&m_fPlayProbability,sizeof(float));

	SaveStringT(File, m_szAudioName.c_str());

	if ( FAILED( CEffectUnitProp::SaveBin( File ) ) )
		return GERROR;
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////
bool CTDimAudio::s_bIsEffectEditor = false;
CTDimAudio::CTDimAudio( IEffectUnitProp* pEffectUnitProp )
		: CEffectUnit( pEffectUnitProp )
{
	m_pAEmitter = GetAudioModule()->CreateAudioEmitter();
	m_bNeedPlay = CQuickRand::Rand2( 0.0f, 1.0f ) <= static_cast<CTDimAudioProp*>(pEffectUnitProp)->m_fPlayProbability;
}

CTDimAudio::~CTDimAudio(void)
{
	m_pAEmitter = NULL;
}

void CTDimAudio::SetStartTime( uint32 STime,uint32 DTime )
{
	CEffectUnit::SetStartTime( STime,DTime );
	if (s_bIsEffectEditor)
		m_bNeedPlay = CQuickRand::Rand2( 0.0f, 1.0f ) <= static_cast<CTDimAudioProp*>(m_pEffectUnitProp)->m_fPlayProbability;
}

bool CTDimAudio::SetProp( IEffectUnitProp* pEffectUnitProp )
{
	if ( !CEffectUnit::SetProp( pEffectUnitProp ) )
		return false;
	if (s_bIsEffectEditor)
	{
		const CTDimAudioProp* pProp = static_cast<CTDimAudioProp*>(m_pEffectUnitProp);
		if (m_strLastName != pProp->m_szAudioName)
		{
			// 特效编辑器cue名字改变时停止老的声音
			if (m_pAEmitter->IsPlaying())
				m_pAEmitter->Stop();
			m_bNeedPlay = true;
			m_strLastName = pProp->m_szAudioName;
		}
	}

	return true;
}

void CTDimAudio::SetRenderVelocity( float Velocity, int32 id  )
{
	CEffectUnit::SetRenderVelocity(Velocity,id);
}

const EFFECT_RESULT	CTDimAudio::RenderEx( const CMatrix& matWorld, RenderObjStyle* pRORS )
{
	const CTDimAudioProp* pProp = static_cast<CTDimAudioProp*>(m_pEffectUnitProp);

	EFFECT_RESULT result( NULL, true, false );

	if ( !IsInRenderTime() )
	{
		if( !pProp->m_bDelayStop && IsEndTime() )
		{
			if (m_pAEmitter->IsPlaying())
				m_pAEmitter->Stop();
		}
		return result;
	}

	float percent = GetUnitPercent();
	CVector3f offset = GetMapValue( pProp->m_vecOffset, percent );
	offset.FastMultiply( matWorld );

	if ( m_pAEmitter.isvalid() && !pProp->m_szAudioName.empty())
	{
		m_pAEmitter->SetAutoStop(!pProp->m_bDelayStop);
		m_pAEmitter->SetPosition(offset);
		// m_pAEmitter->SetVelocity(velocity);速度? 多普勒效应暂时用不着

		if (CheckNeedPlay())
		{
			m_pAEmitter->Play(pProp->m_szAudioName.c_str());
			m_bNeedPlay = false;
		}
	}

	return result;
}

//------------------------------------------------------------------------------
/**
	对于异步循环的音效, 只有第一次开始时才播放
 */
inline bool
CTDimAudio::CheckNeedPlay()
{
	const CTDimAudioProp* pProp = static_cast<CTDimAudioProp*>(m_pEffectUnitProp);
	if (pProp->m_bDelayStop)
	{
		// 非循环音效再次播放时强制结束
		if (m_bNeedPlay && m_pAEmitter->IsPlaying())
			m_pAEmitter->Stop();
		return m_bNeedPlay;
	}
	else
	{
		// 循环音效自动结束
		return !m_pAEmitter->IsPlaying();
	}
}

//------------------------------------------------------------------------------
void
CTDimAudio::SetVisible( bool isVisible )
{
	CEffectUnit::SetVisible(isVisible);
	if (s_bIsEffectEditor && !isVisible && m_pAEmitter->IsPlaying())
	{
		m_pAEmitter->Stop();
	}
}