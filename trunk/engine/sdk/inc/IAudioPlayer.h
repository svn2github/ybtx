#pragma once
#include "CVector3.h"
#include "CAudioMallocObject.h"

namespace sqr
{

/// 声音播放器.
/// 为了方便使用而做的封装
/// 只存在一个实例, 不需要客户端手动销毁
/// 不建议直接使用wave, 要用的话调用AudioEngine吧
/// cue的各种属性尽量在Microsoft Cross-Platform Audio Creation Tool (XACT)中调节
class IAudioPlayer : public CAudioMallocObject
{
public:
	/// 播放一个声音
	virtual void PlayCue(const char* cueName) = 0;
	/// 播放一个3D声音
	virtual void Play3DCue(const char* cueName, const CVector3f& position, const CVector3f& velocity) = 0;
	/// 停止所有cueName的实例
	virtual void StopCue(const char* cueName) = 0;
};

}// namespace sqr
