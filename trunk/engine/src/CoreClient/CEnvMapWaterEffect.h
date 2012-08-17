#pragma once
#include "CWaterEffect.h"
#include "GraphicBase.h"
/**
	@class CEnvMapWaterEffect

	环境映射水效果.
	可以做为低端机器纹理动画水的替代方案.
	会反射立方体环境映射贴图.
 */
namespace sqr
{

class CEnvMapWaterEffect : public CWaterEffect
{
public:
	CEnvMapWaterEffect();
	virtual ~CEnvMapWaterEffect();

	/// 更新
	void OnFrameUpdate();
	/// 渲染开始
	void OnRenderBegin();

private:
	ITexture*	m_pEnvironmentMap;
	ITexture*	m_pWaveNormalMap[2];
	CVector2f	m_WaveSpeed[2];
	CVector2f	m_WaveOffset[2];
};

}// namespace sqr
