#pragma once
#include "CRenderNode.h"
#include "CRenderSpaceNode.h"
#include "IAudioEmitter.h"

namespace sqr
{
class CMetaRegion;
struct CRegionAI;

class CRenderAudio 
	: public CRenderNode
{
	uint16			m_uUnitSetIndex;
	uint32			m_uPlayTimeLenMin;		///播放时间长度最小值
	uint32			m_uPlayTimeLenMax;		///播放时间长度最大值
	uint32			m_uPlayIntervalMin;		///2段播放时间间隔长度最小值
	uint32			m_uPlayIntervalMax;		///2段播放时间间隔长度最大值
	uint32			m_uCurRandomPlayTime;	///当前随机播放时间长度
	uint32			m_uCurRandomInterval;	///当前随机2段播放时间间隔长度
	uint32			m_uPlayTime;			///当前已经播放多久时间
	uint32			m_uIntervalTime;		///当前间隔多久时间
	bool			m_bPlayNotRepeat;
	bool			m_bNeedPlayBegin;
	bool			m_bNeedIntervalBegin;

	IAudioEmitterPtr m_pAEmitter;
	SString			 m_strAudioName;
	bool			 m_bPlayError;
	bool	IsNeedPlay();
	uint32	GetRandomValue(uint32 nLowBound, uint32 nHighBound);
public:
	CRenderAudio(const CRegionAI& AudioInfo, const SString& AudioName,ILogicHandler* pHandler = NULL );
	~CRenderAudio();

	void	OnPreRender( uint32 uCurTime,CCamera* pCamera );
	void	Render( void );
	bool	UpdateTime(uint32 uCurTime){ return false; };


	const AniResult		Render( uint32 uCurTime, RenderObjStyle* pRORS = NULL );
	void				SetVisible( bool isVisible );
	ERenderObjType		GetType(void)	{ return ROT_AUDIO; };
};

}