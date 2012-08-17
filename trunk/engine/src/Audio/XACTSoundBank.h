#pragma once
#include "CSoundBank.h"

namespace sqr
{

class CXACTSoundBank : public CSoundBank
{
public:
	/// 构造函数
	CXACTSoundBank();
	/// 析构函数
	virtual ~CXACTSoundBank();

	/// 初始化
	bool Setup(const AString& filename, bool streaming);
	/// 销毁
	void Destroy();

	/// 停止所有Cue的实例
	void StopCue(index_t cueIndex, bool immediate);
	/// 停止所有Wave的实例
	void StopWave(index_t waveIndex, bool immediate);

	/// 返回 XACT sound bank 的指针
	IXACT3SoundBank* GetXACTSoundBank() const;
	/// 返回 XACT wave bank 的指针
	IXACT3WaveBank* GetXACTWaveBank() const;

private:
	bool SetupSoundBank();
	bool SetupWaveBank();
	void DestroySoundBank();
	void DestroyWaveBank();

private:
	IXACT3SoundBank* xactSoundBank;
	IXACT3WaveBank* xactWaveBank;
	void* soundBankData;
	void* waveBankData;
};

}// namespace sqr
