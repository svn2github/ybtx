#pragma once
#include "CAudioAdapterInfo.h"
#include "CWindowsVersionEnum.h"
#include "CCue.h"
#include "CWave.h"
#include "CSoundBank.h"
#include "CRefObject.h"
#include "CAudioMallocObject.h"

namespace sqr
{

class CAudioEmitter;

/// 音频引擎. 
class CAudioEngine 
	: public CRefObject
	, public CAudioMallocObject
{
public:
	/// 构造函数
	CAudioEngine();
	/// 析构函数
	virtual ~CAudioEngine();

	/// 取得适配器数量
	virtual size_t GetNumAdapters() const = 0;
	/// 返回适配器信息
	virtual CAudioAdapterInfo GetAdapterInfo(index_t adapterIndex) const = 0;
	/// 设置默认的适配器
	void SetDefaultAdapter(index_t adapterIndex);
	/// 返回默认适配器的索引
	index_t GetDefaultAdapter() const;
	
	/// 设置全局配置文件路径
	void SetGlobalSettingsPath(const AString& filename);
	/// 返回全局配置文件路径
	const AString& GetGlobalSettingsPath() const;

	/// 打开音频引擎
	virtual bool Open();
	/// 关闭音频引擎
	virtual void Close();
	/// 更新音频引擎
	virtual void Update();
	/// 音频引擎是否打开
	bool IsOpen() const;
	/// 控制win7，vista抽出耳塞情况
	void SetIsOpen(bool bOpen);
	/// 音频设备是否可用
	bool IsAudioDeviceValid() const;
	/// 是否支持3D效果
	bool IsSupport3D() const;

	/// 加载sound bank
	bool LoadSoundBank(const AString& filename, bool streaming);
	/// 卸载sound bank
	void UnloadSoundBank(const AString& filename);
	/// 返回sound bank的数量
	size_t GetNumSoundBanks() const;
	/// 返回指定索引的sound bank
	const CSoundBankPtr& GetSoundBankByIndex(index_t index);
	/// 根据sound bank名字得到sound bank index
	index_t GetSoundBankIndexByName(const AString& name) const;
	/// 返回指定名字的sound bank
	const CSoundBankPtr& GetSoundBankByName(const AString& name) const;
	/// 是否存在指定sound bank
	bool HasSoundBank(const AString& filename) const;

	/// 更新收听者的位置信息和速度
	void UpdateListener(const CVector3f& pos,
						const CVector3f& top,
						const CVector3f& front,
						const CVector3f& velocity);
	/// 设置收听者位置
	void SetListenerPosition(const CVector3f& pos);
	/// 返回收听者的位置
	const CVector3f& GetListenerPostion() const;
	/// 设置收听者朝向(上)
	void SetListenerOrientTop(const CVector3f& top);
	/// 返回收听者的朝向(上)
	const CVector3f& GetListenerOrientTop() const;
	/// 设置收听者朝向(前)
	void SetListenerOrientFront(const CVector3f& front);
	/// 返回收听者的朝向(前)
	const CVector3f& GetListenerOrientFront() const;
	/// 设置收听者的速度
	void SetListenerVelocity(const CVector3f& velocity);
	/// 返回收听者的速度
	const CVector3f& GetListenerVelocity() const;
	/// 初始化的时候设置音效库是否有效
	void  SetAudioDeviceIsValid(bool isValid);

	/// 播放或准备一个cue,使用emitter调用该接口，需返回一个cue对象
	const CCuePtr& PlayCue(const AString& cueName, bool play, bool enable3D, const CVector3f& position, const CVector3f& velocity);
	/// 播放或准备一个cue,全局播放音效，比如背景音乐，角色受击等，则不需要返回一个cue对象
	void  PlayGloablCue(const AString& cueName, bool play, bool enable3D, const CVector3f& position, const CVector3f& velocity);
	/// 停止所有cue的实例
	void  StopCue(const AString& cueName, bool immediate);
	bool  CueIsStop(const AString& cueName);

	/// 播放或准备一个wave
	const CWavePtr& PlayWave(const AString& waveName, bool play, size_t loopCount, float volume, float pitch);
	/// 停止所有wave的实例
	void StopWave(const AString& waveName, bool immediate);

	/// 查找cue和soundbank
	index_t FindCue(const AString& cueName, CSoundBankPtr& outSoundBank);
	/// 查找wave和soundbank
	index_t FindWave(const AString& waveName, CSoundBankPtr& outSoundBank);

	/// 设置全局变量
	virtual void SetGlobalVariable(const AString& name, float value) = 0;
	/// 取得全局变量
	virtual float GetGlobalVariable(const AString& name) = 0;

	/// 检测机子的os version
	void EstimateSystemVersion();

	void InsertAudioEmitter(CAudioEmitter *pAudioEmitter);
	void DeleteAudioEmitter(CAudioEmitter *pAudioEmitter);

protected:
	void DestroyStoppedCues();
	void DestroyStoppedWaves();
	void DestroyWavesResource();
	void DestroySoundBankResource();
	void DestroySoundResource();
	void NodityAmitterDestroySoundResource();

	virtual CSoundBank* CreateSoundBank() = 0;
	virtual CCue* CreateCue() = 0;
	virtual CWave* CreateWave() = 0;
protected:
	index_t defaultAdapterIndex;
	AString  globalSettingsPath;
	bool isOpen;
	//////////////////////////////////////////////////////////////////////////
	///win7 deal with
	bool isAudioDeviceValid;
	bool isLastAudioDeviceValid;
	static bool isDestroyResourceInGame;
	
	//////////////////////////////////////////////////////////////////////////
	bool isSupport3D;
	AMap<AString, CSoundBankPtr> soundBanks;
	AList<CCuePtr> activeNormalCues;		///normal cue, not need emitter to play 
	AList<CCuePtr> activeEmitterCues;///emitter to play cue
	AList<CWavePtr> activeWaves;

	set<CAudioEmitter*> setAudioEmitters;

	CVector3f listenerPosition;
	CVector3f listenerOrientTop;
	CVector3f listenerOrientFront;
	CVector3f listenerVelocity;
	HANDLE updateEvent;
	HANDLE detachEvent;
	HANDLE attachEvent;

public:
	static WindowsVersion eWindowsType;
};

extern CAudioEngine* GetAudioEngine();

//------------------------------------------------------------------------------
inline void CAudioEngine::SetDefaultAdapter( index_t adapterIndex )
{
	this->defaultAdapterIndex = adapterIndex;
}

//------------------------------------------------------------------------------
inline index_t CAudioEngine::GetDefaultAdapter() const
{
	return this->defaultAdapterIndex;
}

//------------------------------------------------------------------------------
inline void CAudioEngine::SetGlobalSettingsPath( const AString& filename )
{
	this->globalSettingsPath = filename;
}

//------------------------------------------------------------------------------
inline const AString& CAudioEngine::GetGlobalSettingsPath() const
{
	return this->globalSettingsPath;
}

//------------------------------------------------------------------------------
inline bool CAudioEngine::IsOpen() const
{
	return this->isOpen;
}

//------------------------------------------------------------------------------
inline void CAudioEngine::SetIsOpen(bool bOpen)
{
	this->isOpen = bOpen;
}

//------------------------------------------------------------------------------
inline bool CAudioEngine::IsAudioDeviceValid() const
{
	return this->isAudioDeviceValid && this->isOpen;
}

//------------------------------------------------------------------------------
inline bool CAudioEngine::IsSupport3D() const
{
	return this->isSupport3D;
}

//------------------------------------------------------------------------------
inline size_t CAudioEngine::GetNumSoundBanks() const
{
	return this->soundBanks.size();
}

//------------------------------------------------------------------------------
inline bool CAudioEngine::HasSoundBank( const AString& filename ) const
{
	return (this->soundBanks.end() != this->soundBanks.find(filename));
}

//------------------------------------------------------------------------------
inline void CAudioEngine::SetListenerVelocity( const CVector3f& velocity )
{
	this->listenerVelocity = velocity;
}

//------------------------------------------------------------------------------
inline const CVector3f& CAudioEngine::GetListenerVelocity() const
{
	return this->listenerVelocity;
}

//------------------------------------------------------------------------------
inline void
CAudioEngine::SetListenerPosition( const CVector3f& pos )
{
	this->listenerPosition = pos;
}

//------------------------------------------------------------------------------
inline const CVector3f&
CAudioEngine::GetListenerPostion() const
{
	return this->listenerPosition;
}

//------------------------------------------------------------------------------
inline void
CAudioEngine::SetListenerOrientTop( const CVector3f& top )
{
	this->listenerOrientTop = top;
}

//------------------------------------------------------------------------------
inline const CVector3f&
CAudioEngine::GetListenerOrientTop() const
{
	return this->listenerOrientTop;
}

//------------------------------------------------------------------------------
inline void
CAudioEngine::SetListenerOrientFront( const CVector3f& front )
{
	this->listenerOrientFront = front;
}

//------------------------------------------------------------------------------
inline const CVector3f&
CAudioEngine::GetListenerOrientFront() const
{
	return this->listenerOrientFront;
}

//------------------------------------------------------------------------------
inline void
CAudioEngine::SetAudioDeviceIsValid(bool isValid)
{
	this->isAudioDeviceValid = isValid;
}

}// namespace sqr
