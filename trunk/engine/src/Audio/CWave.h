#pragma once
#include "CRefObject.h"
#include "CWaveProperty.h"
#include "SoundState.h"
#include "CAudioMallocObject.h"

namespace sqr
{

SmartPointer(CWave);
SmartPointer(CSoundBank);

class CWave 
	: public CRefObject
	, public CAudioMallocObject
{
public:
	/// 构造函数
	CWave();
	/// 析构函数
	virtual ~CWave();

	/// 初始化
	virtual void Setup(const CSoundBankPtr& soundBank, index_t waveIndex, bool startPlayback, size_t loopCount);
	/// 释放
	virtual void Destroy();
	/// 是否合法
	bool IsValid() const;

	/// 返回附属的soundbank
	const CSoundBankPtr& GetSoundBank() const;
	/// 返回在soundbank中的索引
	index_t GetWaveIndex() const;
	/// 返回wave的属性信息
	const CWaveProperty& GetWaveProperty() const;

	/// 设置音量0(静音), 1(原音量)
	virtual void SetVolume(float v) = 0;
	/// 设置音调[-1.0, 1.0]
	virtual void SetPitch(float p) = 0;

	/// 播放
	virtual void Play() = 0; 
	/// 停止
	virtual void Stop() = 0;
	/// 立即停止
	virtual void StopImmediate() = 0;
	/// 暂停
	virtual void Pause() = 0;
	/// 继续
	virtual void Resume() = 0;

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

	/// 返回当前状态
	virtual SoundState::Mask GetState() const = 0;

protected:
	CSoundBankPtr soundBank;
	index_t waveIndex;
};


//------------------------------------------------------------------------------
inline bool CWave::IsValid() const
{
	return InvalidIndex != this->waveIndex;
}

//------------------------------------------------------------------------------
inline const CSoundBankPtr& CWave::GetSoundBank() const
{
	return this->soundBank;
}

//------------------------------------------------------------------------------
inline index_t CWave::GetWaveIndex() const
{
	return this->waveIndex;
}

//------------------------------------------------------------------------------
inline bool CWave::IsPreparing() const
{
	return 0 != (SoundState::Preparing & this->GetState());
}

//------------------------------------------------------------------------------
inline bool CWave::IsPrepared() const
{
	return 0 != (SoundState::Prepared & this->GetState());
}

//------------------------------------------------------------------------------
inline bool CWave::IsPlaying() const
{
	return 0 != (SoundState::Playing & this->GetState());
}

//------------------------------------------------------------------------------
inline bool CWave::IsStopping() const
{
	return 0 != (SoundState::Stopping & this->GetState());
}

//------------------------------------------------------------------------------
inline bool CWave::IsStopped() const
{
	return 0 != (SoundState::Stopped & this->GetState());
}

//------------------------------------------------------------------------------
inline bool CWave::IsPaused() const
{
	return 0 != (SoundState::Paused & this->GetState());
}

}// namespace sqr
