#pragma once
#include "TSingleton.h"
#include "CAudioMallocObject.h"

namespace sqr
{

/// 3D空间的声音收听者.
/// 位置用世界坐标标识
/// 全局只有一个收听者
/// 收听者有位置, 朝向, 速度, 一般跟摄像机同步
class IAudioListener 
	: public Singleton<IAudioListener>
	, public CAudioMallocObject
{
public:
	virtual ~IAudioListener(){};
	/// 设置位置和朝向
	virtual void SetTransform(const CVector3f& orientFront, const CVector3f& orientTop, const CVector3f& position) = 0;
	/// 返回位置
	virtual const CVector3f& GetPosition() const = 0;
	/// 设置位置
	virtual void SetPosition(const CVector3f& pos) = 0;
	/// 返回朝向(上)
	virtual const CVector3f& GetOrientTop() const = 0;
	/// 设置朝向(上)
	virtual void SetOrientTop(const CVector3f& top) = 0;
	/// 返回朝向(前)
	virtual const CVector3f& GetOrientFront() const = 0;
	/// 设置朝向(前)
	virtual void SetOrientFront(const CVector3f& front) = 0;
	/// 设置速度
	virtual void SetVelocity(const CVector3f& velocity) = 0;
	/// 返回速度
	virtual const CVector3f& GetVelocity() const = 0;
};

}// namespace sqr
