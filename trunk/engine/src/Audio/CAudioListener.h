#pragma once
#include "IAudioListener.h"

namespace sqr
{

/// 3D空间的声音收听者.
/// 位置用世界坐标标识
/// 只有一个收听者 //?为什么只有一个收听者,如此推理摄像机也应该是只有一个
/// 收听者有位置, 朝向, 速度, 一般跟摄像机同步
class CAudioListener
	: public IAudioListener 
{
public:
	/// 设置位置和朝向
	void SetTransform(const CVector3f& orientFront, const CVector3f& orientTop, const CVector3f& position);
	/// 返回位置
	const CVector3f& GetPosition() const;
	/// 设置位置
	void SetPosition(const CVector3f& pos);
	/// 返回朝向(上)
	const CVector3f& GetOrientTop() const;
	/// 设置朝向(上)
	void SetOrientTop(const CVector3f& top);
	/// 返回朝向(前)
	const CVector3f& GetOrientFront() const;
	/// 设置朝向(前)
	void SetOrientFront(const CVector3f& front);
	/// 设置速度
	void SetVelocity(const CVector3f& velocity);
	/// 返回速度
	const CVector3f& GetVelocity() const;

public:
	/// 构造函数
	CAudioListener();
	/// 析构函数
	~CAudioListener();
};

}// namespace sqr
