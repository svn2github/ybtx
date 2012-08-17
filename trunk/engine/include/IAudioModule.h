#pragma once
#include "IAudioSystem.h"
#include "IAudioPlayer.h"
#include "IAudioListener.h"
#include "IAudioEmitter.h"
#include "CAudioMallocObject.h"

#if defined(_WIN32) && defined(_USRDLL)

#ifdef AUDIO_EXPORTS
#define AUDIO_API extern "C" __declspec(dllexport)
#else
#define AUDIO_API extern "C" __declspec(dllimport)
#endif

#else

#define AUDIO_API

#endif


namespace sqr
{

/// 音频模块.
/// IAudioSystem, IAudioPlayer, IAudioListener为内部单件, 不用手动释放
/// IAudioEmitter请使用智能指针IAudioEmitterPtr进行引用计数的管理
class IAudioModule : public CAudioMallocObject
{
public:
	/// 取得音频库
	virtual IAudioSystem*    GetAudioSystem() = 0;
	/// 取得播放器
	virtual IAudioPlayer*    GetAudioPlayer() = 0;
	/// 取得收听者
	virtual IAudioListener*  GetAudioListener() = 0;
	/// 创建一个声音发射器
	virtual IAudioEmitterPtr CreateAudioEmitter() = 0;
	/// AudioSystem是否已经创建
	virtual bool			 HasAudioSystem()	= 0;
};

/// IAudioPlayer的快捷调用.
/// 必需在IAudioSystem启动后才可以调用
//@{
AUDIO_API void PlayCue(const char* cueName);
AUDIO_API void Play3DCue(const char* cueName, const CVector3f& position, const CVector3f& velocity);
AUDIO_API void StopCue(const char* cueName);
AUDIO_API bool CueIsStop(const char* cueName);
//@}

AUDIO_API IAudioModule* GetAudioModule();

}// namespace sqr
