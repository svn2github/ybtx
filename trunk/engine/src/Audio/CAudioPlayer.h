#pragma once
#include "IAudioPlayer.h"
#include "TSingleton.h"

namespace sqr
{

/// 声音播放器.
/// 为了方便使用而做的封装
/// 定义为单件
/// 不建议直接使用wave, 要用的话调用AudioEngine吧
/// cue的各种属性尽量在Microsoft Cross-Platform Audio Creation Tool (XACT)中调节
class CAudioPlayer
	: public IAudioPlayer
	, public Singleton<CAudioPlayer>
{
public:
	/// 播放一个声音
	void PlayCue(const char* cueName);
	/// 播放一个3D声音
	void Play3DCue(const char* cueName, const CVector3f& position, const CVector3f& velocity);
	/// 停止所有cueName的实例
	void StopCue(const char* cueName);
	/// cue是否播放结束
	bool CueIsStop(const char* cueName);
public:
	/// 构造函数
	CAudioPlayer();
	/// 析构函数
	~CAudioPlayer();
};

}// namespace sqr
