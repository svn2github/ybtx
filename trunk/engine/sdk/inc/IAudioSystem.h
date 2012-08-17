#pragma once
#include "TSingleton.h"
#include "CAudioMallocObject.h"

namespace sqr
{

/**
	@class IAudioSystem

	音频系统.
	默认全局配置是"Sound/globals.xgs"
	没有全局配置也能工作, 但3D没效果
	一般来说音乐(文件比较大)用流创建, 音效(要求响应快)直接载入内存

	SoundBank的载入可能通过这种xml来进行:
	<Audio>
		<SoundBank name="ambient" stream="true"/>
		<SoundBank name="gui" stream="false"/>
		<SoundBank name="item" stream="false"/>
		<SoundBank name="music" stream="true"/>
		<SoundBank name="npc" stream="false"/>
		<SoundBank name="skill" stream="false"/>
		<SoundBank name="speech" stream="true"/>
		<SoundBank name="weapon" stream="false"/>
	</Audio>
 */
class IAudioSystem
	: public Singleton<IAudioSystem>
	, public CAudioMallocObject
{
public:
	virtual ~IAudioSystem(){}
	/// 设置全局配置文件路径
	virtual void SetGlobalSettingsPath(const char* path) = 0;
	/// 返回全局配置文件路径
	virtual const char* GetGlobalSettingsPath() const = 0;

	/// 启动音频系统
	virtual bool Open() = 0;
	/// 关闭音频系统
	virtual void Close() = 0;
	/// 音频系统是否启动
	virtual bool IsOpen() const = 0;
	/// 更新(每帧调用)
	virtual void Update() = 0;

	/// 加载soundbank
	virtual bool LoadSoundBank(const char* path, bool streaming) = 0;
	/// 卸载soundbank
	virtual void UnloadSoundBank(const char* path) = 0;
	/// 批量加载soundbank
	virtual bool LoadSoundBankFromXml(const char* xmlFile) = 0;

	/// 设置全局变量
	virtual void SetGlobalVariable(const char* name, float value) = 0;
	/// 取得全局变量
	virtual float GetGlobalVariable(const char* name) = 0;
	
	/// Set SpeedOfSound Variable Value
	virtual void SetSpeedOfSoundVariable(float value) = 0;
	/// Get SpeedOfSound Variable Value
	virtual float GetSpeedOfSoundVariable(const char* name) = 0;

	/// 工具用的接口.
	//@{
	virtual size_t GetNumSoundBanks() const = 0;
	virtual const char* GetSoundBankName(index_t index) const = 0;
	virtual size_t GetNumCues(index_t soundBankIndex) const = 0;
	virtual const char* GetCueName(index_t soundBankIndex, index_t cueIndex) const = 0;
	virtual index_t GetSoundBankIndexByName( const char* name ) const = 0;
	//@}

	/// 判断玩家资源是否存在
	virtual bool SoundFileIsExist(const char* name) = 0;
};

}// namespace sqr
