#pragma once
#include "CRefObject.h"
#include "CCueProperty.h"
#include "SoundState.h"
#include "CAudioMallocObject.h"

/**
	@class CCue

	抽象的声音单位.
	为了减少复杂性, 把朝向给去掉了. 只保留位置和速度.
 */

namespace sqr
{

SmartPointer(CCue);
SmartPointer(CSoundBank);

class CCue 
	: public CRefObject
	, public CAudioMallocObject
{
protected:
	/// 析构函数
	virtual ~CCue();

public:
	/// 构造函数
	CCue();

	/// 释放
	virtual void Destroy();

	/// 初始化
	virtual void Setup(const CSoundBankPtr& soundBank, const AString& strCueName, index_t cueIndex, bool startPlayback, bool enable3D) = 0;
	void Init(const AString& strCueName, index_t cueIndex, bool startPlayback, bool enable3D);

	/// 是否合法
	bool IsValid() const;

	/// 返回在soundbank中的索引
	uint GetCueIndex() const;

	AString GetCueName() const;
	
	/// 播放
	virtual void Play() = 0;
	/// 停止
	virtual bool Stop() = 0; ///返回值代表stop是否成功，如果出现结构化异常，也表示stop不成功，返回false
	/// 立即停止
	virtual void StopImmediate() = 0;
	/// 暂停
	virtual void Pause() = 0;
	/// 继续
	virtual void Resume() = 0;
	/// 更新3D信息
	virtual void Update() = 0;
	
	/// 是否准备中
	bool IsPreparing() const;
	/// 是否准备完成
	bool IsPrepared() const;
	/// 是否播放中
	bool IsPlaying() const;
	/// 是否停止中
	bool IsStopping() const;
	/// 是否停止了
	bool IsStopped() const;
	/// 是否暂停了
	bool IsPaused() const;
	/// 是否3D cue
	bool Is3DEnabled() const;
	
	/// 返回当前状态
	virtual SoundState::Mask GetState() const = 0;

	/// 设置变量
	virtual void SetVariable(const AString& name, float value) = 0;
	/// 取得变量
	virtual float GetVariable(const AString& name) = 0;

	/// 返回位置
	const CVector3f& GetPosition() const;
	/// 设置位置
	void SetPosition(const CVector3f& pos);
	/// 返回世界空间的速度
	const CVector3f& GetVelocity() const;
	/// 设置世界空间的速度
	void SetVelocity(const CVector3f& v);

protected:
	index_t cueIndex;
	CVector3f position;
	CVector3f velocity;
	bool is3DEnabled;
	AString strCueName;
};

//------------------------------------------------------------------------------
inline bool CCue::IsValid() const
{
	return InvalidIndex != this->cueIndex;
}

//------------------------------------------------------------------------------
inline index_t CCue::GetCueIndex() const
{
	return this->cueIndex;
}

//------------------------------------------------------------------------------
inline AString CCue::GetCueName() const
{
	return this->strCueName;
}

//------------------------------------------------------------------------------
inline bool CCue::IsPreparing() const
{
	return 0 != (SoundState::Preparing & this->GetState());
}

//------------------------------------------------------------------------------
inline bool CCue::IsPrepared() const
{
	return 0 != (SoundState::Preparing & this->GetState());
}

//------------------------------------------------------------------------------
inline bool CCue::IsPlaying() const
{
	return 0 != (SoundState::Playing & this->GetState());
}

//------------------------------------------------------------------------------
inline bool CCue::IsStopping() const
{
	return 0 != (SoundState::Stopping & this->GetState());
}

//------------------------------------------------------------------------------
inline bool CCue::IsStopped() const
{
	return 0 != (SoundState::Stopped & this->GetState());
}

//------------------------------------------------------------------------------
inline bool CCue::IsPaused() const
{
	return 0 != (SoundState::Paused & this->GetState());
}

//------------------------------------------------------------------------------
inline bool CCue::Is3DEnabled() const
{
	return this->is3DEnabled;
}

//------------------------------------------------------------------------------
inline const CVector3f& CCue::GetVelocity() const
{
	return this->velocity;
}

//------------------------------------------------------------------------------
inline void CCue::SetVelocity( const CVector3f& v )
{
	this->velocity = v;
}

//------------------------------------------------------------------------------
inline const CVector3f&
CCue::GetPosition() const
{
	return this->position;
}

//------------------------------------------------------------------------------
inline void
CCue::SetPosition( const CVector3f& pos )
{
	this->position = pos;
}

}// namespace sqr

