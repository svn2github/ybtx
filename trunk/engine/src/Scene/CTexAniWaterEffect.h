#pragma once
#include "CWaterEffect.h"
/**
	@class CTexAniWaterEffect

	基于纹理动画的水.
	每帧换一张纹理, 以此来实现波浪效果.
	对低端显卡适用. 序列纹理不容易制作, 表现力也差一些.(参考WOW)
 */
namespace sqr
{
class ITexture;
SmartPointer(CDx9Texture);

class CTexAniWaterEffect : public CWaterEffect
{
public:
	CTexAniWaterEffect();
	virtual ~CTexAniWaterEffect();

	/// 返回纹理序列
	//const SVector<ITexture*>& GetAnimationTextures() const;
	/// 设置纹理序列
	void SetAnimationTextures(const SVector<ITexture*>& t);

	/// 更新
	void OnFrameUpdate();
	/// 渲染开始
	void OnRenderBegin();

private:
	SVector<ITexture*> textures;
	index_t currentTextureIndex;
};

}// namespace sqr
