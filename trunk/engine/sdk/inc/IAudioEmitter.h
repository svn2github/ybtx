#pragma once
#include "TRefPointer.h"
#include "CAudioMallocObject.h"

namespace sqr
{

SmartPointer(IAudioEmitter);
/// 3D空间的声音发射器.
/// 位置用世界坐标标识
/// 多普勒效应只与速度有关
class IAudioEmitter : public CAudioMallocObject
{
public:
	/// 增加引用计数
	virtual void AddRef() = 0;
	/// 减少引用计数
	virtual void Release() = 0;

	/// 返回位置
	virtual const CVector3f& GetPosition() const = 0;
	/// 设置位置
	virtual void SetPosition(const CVector3f& pos) = 0;
	/// 设置速度
	virtual void SetVelocity(const CVector3f& v) = 0;
	/// 返回速度
	virtual const CVector3f& GetVelocity() const = 0;

	/// 播放声音
	virtual bool Play(const char* cueName) = 0;
	/// 停止
	virtual bool Stop() = 0;

	/// 是否播放中
	virtual bool IsPlaying() const = 0;
	/// 是否停止
	virtual bool IsStopped() const = 0;

	/// 设置变量
	virtual void SetVariable(const char* name, float value) = 0;
	/// 取得变量
	virtual float GetVariable(const char* name) = 0;

	/// 设置自动结束
	virtual void SetAutoStop(bool b) = 0;
	/// 是否自动结束
	virtual bool IsAutoStop() const = 0;

protected:
	/// 析构函数
	virtual ~IAudioEmitter() {};
};

}// namespace sqr
