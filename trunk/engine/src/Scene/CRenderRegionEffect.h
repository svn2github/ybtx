#pragma once
#include "TSingleton.h"
#include "CSceneMallocObject.h"
#include "CVector3.h"

namespace sqr
{
	class CRenderable;
	struct CRgnAmbientFxInfo;

	class CRegionAmbientEffec : public CSceneMallocObject
	{
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
		CRenderable*	m_pEffectRenderObj;


		bool	IsNeedPlay(const uint32 dwTime);
		uint32	GetRandomValue(uint32 nLowBound, uint32 nHighBound);

	public:
		CRegionAmbientEffec();
		~CRegionAmbientEffec();

		bool CreateAmbientEffect( const CRgnAmbientFxInfo* pAmbFxInfo );
		void Render( const uint32 uCurTime, const CVector3f& efxpos );
	};

	class CRegionAmbientEffectMgr : public CSceneMallocObject
	{
	private:
		SVector<CRegionAmbientEffec*>	m_pRegAmbientEffectList;
		void DestroyRegionAmbientEffect();

	public:
		CRegionAmbientEffectMgr();
		~CRegionAmbientEffectMgr();

		void CreateRegionAmbientEffect(const SVector<CRgnAmbientFxInfo*>& rgnAmbFxInfoList);
		void Render(const uint32 uCurTime, const CVector3f& efxpos);
	};

}