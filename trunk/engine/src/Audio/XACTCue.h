#pragma once
#include "CCue.h"

namespace sqr
{

class CXACTCue : public CCue
{
private:
	/// 析构函数
	virtual ~CXACTCue();

public:
	/// 构造函数
	CXACTCue();

	/// 释放
	void Destroy();

	/// 初始化
	void Setup(const CSoundBankPtr& soundBank, const AString& strCueName, index_t cueIndex, bool startPlayback, bool enable3D);

	/// 播放
	void Play();
	/// 停止
	bool Stop();
	/// 立即停止
	void StopImmediate();
	/// 暂停
	void Pause();
	/// 继续
	void Resume();
	/// 更新3D信息
	void Update();

	/// 返回当前状态
	SoundState::Mask GetState() const;

	/// 设置变量
	void SetVariable(const AString& name, float value);
	/// 取得变量
	float GetVariable(const AString& name);

private:
	IXACT3Cue* xactCue;
};

}// namespace sqr

