#include "stdafx.h"
#include "CMetaSceneClient.h"
#include "CRenderAudio.h"
#include "CMetaRegion.h"
#include "CRenderScene.h"
#include "IAudioModule.h"
#include "StringHelper.h"

const int AudioRange = 12;

//------------------------------------------------------------------------------
CRenderAudio::CRenderAudio(const CRegionAI& AudioInfo,const SString& AudioName,ILogicHandler* pHandler) 
: CRenderNode(pHandler),m_uUnitSetIndex(0)
{
	m_pAEmitter = GetAudioModule()->CreateAudioEmitter();
	m_pAEmitter->SetAutoStop(true);

	m_uUnitSetIndex		= AudioInfo.SetIndex;
	SetPosition(AudioInfo.cX,AudioInfo.cY,AudioInfo.cZ);
	m_bPlayNotRepeat	= AudioInfo.bPlayNotRepeat;
	m_uPlayTimeLenMin	= AudioInfo.uPlayTimeLenMin;
	m_uPlayTimeLenMax	= AudioInfo.uPlayTimeLenMax;
	m_uPlayIntervalMin	= AudioInfo.uPlayIntervalMin;
	m_uPlayIntervalMax	= AudioInfo.uPlayIntervalMax;

	m_uCurRandomPlayTime = GetRandomValue(m_uPlayTimeLenMin,  m_uPlayTimeLenMax);
	m_uCurRandomInterval = GetRandomValue(m_uPlayIntervalMin, m_uPlayIntervalMax);

	m_strAudioName	= AudioName;

	CVector3f mMin = CVector3f(-30.0f, 0.0f,  -30.0f);
	CVector3f mMax = CVector3f( 30.0f, 60.0f,  30.0f);
	float fRange = eGridSpan * AudioRange;

	mMin = mMin  - CVector3f(fRange, fRange, fRange);
	mMax = mMax  + CVector3f(fRange, fRange, fRange);

	m_OrgAABB.setExtents(mMin, mMax);
	UpdateBox();

	m_pAEmitter->SetPosition(GetPosition());
	m_bNeedPlayBegin     = true;
	m_bNeedIntervalBegin = true;
	m_bPlayError		 = false;
}

//------------------------------------------------------------------------------
CRenderAudio::~CRenderAudio()
{
	m_bPlayError = false;
	m_pAEmitter  = NULL;
}

void	CRenderAudio::OnPreRender( uint32 uCurTime,CCamera* pCamera )
{
	if(!m_bPlayError)
		Render(uCurTime,NULL);
}

void	CRenderAudio::Render( void )
{

}

const AniResult CRenderAudio::Render(uint32 uCurTime, RenderObjStyle* pRORS)
{	
	SQR_TRY	
	{
		if(!IsVisible())
			return AniResult();

		if( m_bPlayNotRepeat )
		{
			if( m_bNeedPlayBegin )
			{
				m_uPlayTime = uCurTime;
				m_bNeedPlayBegin = false;
			}

			DWORD diff = uCurTime - m_uPlayTime;
			if( diff < m_uCurRandomPlayTime )
			{
				if (IsNeedPlay())
				{
					if(!m_pAEmitter->Play(m_strAudioName.c_str()))
						m_bPlayError = true;
				}
			}
			else
			{
				bool bIsPlaying = m_pAEmitter->IsPlaying();
				if( bIsPlaying )
					m_bPlayError = !m_pAEmitter->Stop();

				if( m_bNeedIntervalBegin )
				{
					m_uIntervalTime = uCurTime;
					m_bNeedIntervalBegin = false;
				}

				diff = uCurTime - m_uIntervalTime;
				if( diff > m_uCurRandomInterval )
				{
					m_uCurRandomPlayTime = GetRandomValue(m_uPlayTimeLenMin,  m_uPlayTimeLenMax);
					m_uCurRandomInterval = GetRandomValue(m_uPlayIntervalMin, m_uPlayIntervalMax);

					m_bNeedPlayBegin	  = m_bNeedIntervalBegin = true;
				}
			}
		}
		else
		{
			if (IsNeedPlay())
			{
				if(!m_pAEmitter->Play(m_strAudioName.c_str()))
					m_bPlayError = true;
			}
		}
	}
	SQR_CATCH(exp)
	{
		m_bPlayError = true;

		CVector3f pos = GetPosition();

		string msg;
		msg = format("%s, %f, %f", m_strAudioName.c_str(), pos.x, pos.z);
		exp.AppendMsg(msg.c_str());
		exp.AppendType("TerrainAudio::Render");
		
		GfkLogExpOnce(exp);
	}
	SQR_TRY_END;
	return AniResult();
}

bool CRenderAudio::IsNeedPlay()
{
	bool b = m_pAEmitter->IsPlaying();
	return !b;
}

void CRenderAudio::SetVisible( bool isVisible )
{
	if( isVisible != m_isVisible )
	{
		if( !isVisible )
		{
			if( m_pAEmitter->IsPlaying() )
				m_bPlayError = !m_pAEmitter->Stop();

			m_bNeedPlayBegin = m_bNeedIntervalBegin = true;
		}

		m_isVisible = isVisible;
	}
}

uint32 CRenderAudio::GetRandomValue(uint32 nLowBound, uint32 nHighBound)
{
	if ( nLowBound >= nHighBound )
		return nLowBound;

	uint32 n = nHighBound - nLowBound + 1;

	return nLowBound + rand() % n;
}