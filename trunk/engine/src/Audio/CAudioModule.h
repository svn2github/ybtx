#pragma once
#include "IAudioModule.h"
#include "TSingleton.h"
#include "TRefPointer.h"

namespace sqr
{

class IAudioSystem;
class IAudioPlayer;
class IAudioListener;
SmartPointer(IAudioEmitter);

/// 音频模块.
class CAudioModule
	: public IAudioModule
	, public Singleton<CAudioModule>
{
public:
	/// 取得音频库
	IAudioSystem* GetAudioSystem();
	/// 取得播放器
	IAudioPlayer* GetAudioPlayer();
	/// 取得收听者
	IAudioListener* GetAudioListener();
	/// 创建一个声音发射器
	IAudioEmitterPtr CreateAudioEmitter();
	bool			 HasAudioSystem();

public:
	CAudioModule();
	~CAudioModule();
};

}// namespace sqr
