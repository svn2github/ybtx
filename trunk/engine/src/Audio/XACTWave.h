#pragma once
#include "CWave.h"

namespace sqr
{

class CXACTWave : public CWave
{
public:
	/// 构造函数
	CXACTWave();
	/// 析构函数
	virtual ~CXACTWave();

	/// 初始化
	void Setup(const CSoundBankPtr& soundBank, index_t waveIndex, bool startPlayback, size_t loopCount);
	/// 释放
	void Destroy();

	/// 设置音量0(静音), 1(原音量)
	void SetVolume(float v);
	/// 设置音调[-1.0, 1.0]
	void SetPitch(float p);

	/// 播放
	void Play();
	/// 停止
	void Stop();
	/// 立即停止
	void StopImmediate();
	/// 暂停
	void Pause();
	/// 继续
	void Resume();

	/// 返回当前状态
	SoundState::Mask GetState() const;

private:
	IXACT3Wave* xactWave;
	XACTVOLUME xactVolume;
	XACTPITCH xactPitch;
};

}// namespace sqr
