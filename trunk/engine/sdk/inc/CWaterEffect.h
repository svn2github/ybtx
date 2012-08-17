#pragma once
#include "CRefObject.h"
#include "CSceneMallocObject.h"
#include "CShaderConstParams.h"
/**
	@class CWaterEffect

	水面效果基类.
	为了方便, 使用D3D的效果框架.
	具体的参数在子类中进行管理.
 */
namespace sqr
{
class CGpuProgram;
class CGpuProgramParameters;
SmartPointer(CWaterEffect);

class CWaterEffect 
	: public CRefObject
	, public CSceneMallocObject
{
public:
	CWaterEffect();
	virtual ~CWaterEffect();

	/// 返回effect id
	SHADER_HANDLE GetEffectID() const;
	/// 设置effect id
	void SetEffectID(SHADER_HANDLE id);

	/// 返回当前时间(ms)
	uint GetTime() const;
	/// 返回与上帧的时间差
	uint GetTimeElapsed() const;

	/// 更新
	virtual void OnFrameUpdate();
	/// 渲染开始
	virtual void OnRenderBegin();
	/// 渲染结束
	virtual void OnRenderEnd();

protected:
	SHADER_HANDLE effectId;

	uint absoluteTime;
	uint lastFrameTime;
};

//------------------------------------------------------------------------------
inline SHADER_HANDLE
CWaterEffect::GetEffectID() const
{
	return this->effectId;
}

//------------------------------------------------------------------------------
inline uint
CWaterEffect::GetTime() const
{
	return this->absoluteTime;
}

//------------------------------------------------------------------------------
inline uint
CWaterEffect::GetTimeElapsed() const
{
	return (this->absoluteTime - this->lastFrameTime);
}

}// namespace sqr
