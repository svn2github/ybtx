#pragma once
#include "IAudioEmitter.h"
#include "TRefPointer.h"

namespace sqr
{

SmartPointer(CCue);
SmartPointer(CAudioEmitter);
/// 3D空间的声音发射器.
/// 位置用世界坐标标识
/// 多普勒效应只与速度有关
class CAudioEmitter : public IAudioEmitter
{
public:
	/// 构造函数
	CAudioEmitter();
	/// 增加引用计数
	void AddRef();
	/// 减少引用计数
	void Release();

	/// 返回位置
	const CVector3f& GetPosition() const;
	/// 设置位置
	void SetPosition(const CVector3f& pos);
	/// 设置速度
	void SetVelocity(const CVector3f& v);
	/// 返回速度
	const CVector3f& GetVelocity() const;

	/// 播放声音
	bool Play(const char* cueName);
	/// 停止
	bool Stop();

	/// 是否播放中
	bool IsPlaying() const;
	/// 是否停止
	bool IsStopped() const;

	/// 设置变量
	void SetVariable(const char* name, float value);
	/// 取得变量
	float GetVariable(const char* name);

	/// 设置自动结束
	void SetAutoStop(bool b);
	/// 是否自动结束
	bool IsAutoStop() const;

	void Noticfy();
private:
	/// 析构函数
	virtual ~CAudioEmitter();

	void	InValid();

private:
	volatile int refCount;

	CCuePtr cue;
	CVector3f position;
	CVector3f velocity;
	AMap<AString, float> variableMap;
	bool isAutoStop;
	AString strCueName;
	bool   isValid;
};


//------------------------------------------------------------------------------
inline void CAudioEmitter::AddRef()
{
	InterlockedIncrement((volatile long*)&this->refCount);
}

//------------------------------------------------------------------------------
inline void CAudioEmitter::Release()
{
	Ast(this->refCount > 0);
	InterlockedDecrement((volatile long*)&this->refCount);
	if (0 == this->refCount)
	{
		delete this;
	}
}

//------------------------------------------------------------------------------
inline const CVector3f& CAudioEmitter::GetVelocity() const
{
	return this->velocity;
}

//------------------------------------------------------------------------------
inline const CVector3f&
CAudioEmitter::GetPosition() const
{
	return this->position;
}

//------------------------------------------------------------------------------
inline float
CAudioEmitter::GetVariable( const char* name )
{
	return this->variableMap[name];
}

}// namespace sqr
