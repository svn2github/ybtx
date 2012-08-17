#pragma once
#include "IAudioSystem.h"
#include "CAudioAdapterInfo.h"

namespace sqr
{

typedef AVector<CAudioAdapterInfo> CAudioAdapterInfoList;

/// 音频系统核心服务.
class CAudioSystem
	: public IAudioSystem
{
public:
	/// 设置全局配置文件路径
	void SetGlobalSettingsPath(const char* path);
	/// 返回全局配置文件路径
	const char* GetGlobalSettingsPath() const;

	/// 启动音频系统
	bool Open();
	/// 关闭音频系统
	void Close();
	/// 音频系统是否启动
	bool IsOpen() const;
	/// 更新(每帧调用)
	void Update();

	/// 返回音频适配器列表
	const CAudioAdapterInfoList& GetAudioAdapterList();
	/// 选择适配器
	void SetAdapterIndex(index_t adapterIndex);
	/// 返回正在使用的适配器索引
	index_t GetAdapterIndex() const;

	/// 加载soundbank
	bool LoadSoundBank(const char* path, bool streaming);
	/// 卸载soundbank
	void UnloadSoundBank(const char* path);
	/// 批量加载soundbank
	bool LoadSoundBankFromXml(const char* xmlFile);

	/// 设置全局变量
	void SetGlobalVariable(const char* name, float value);
	/// 取得全局变量
	float GetGlobalVariable(const char* name);
	
	/// Set SpeedOfSound Variable Value
	virtual void SetSpeedOfSoundVariable(float value);
	/// Get SpeedOfSound Variable Value
	virtual float GetSpeedOfSoundVariable(const char* name);

	/// 工具用的接口.
	//@{
	/// 返回加载的sound bank数目
	size_t GetNumSoundBanks() const;
	/// 返回sound bank的名字
	const char* GetSoundBankName(index_t index) const;
	/// 返回sound bank中的cue数目
	size_t GetNumCues(index_t soundBankIndex) const;
	/// 返回sound bank中指定cue的名字
	const char* GetCueName(index_t soundBankIndex, index_t cueIndex) const;
	/// 返回name指定的sound bank index
	index_t GetSoundBankIndexByName( const char* name ) const;
	//@}

	void CollectSoundDirFileInfo();

	bool SoundFileIsExist(const char* name);
public:
	friend class TSingleton<CAudioSystem>;
	CAudioSystem();
	~CAudioSystem();

private:
	bool isOpen;
	CAudioAdapterInfoList adapterList;
	uint64 lastUpdateTime;
	int  nLoadXmlCount;

	static const uint UPDATE_INTERVAL;	///< 更新间隔(频率)

	ASet<AString> listSoundDirFileName;
};



//------------------------------------------------------------------------------
inline bool CAudioSystem::IsOpen() const
{
	return this->isOpen;
}

//------------------------------------------------------------------------------
inline const CAudioAdapterInfoList& CAudioSystem::GetAudioAdapterList()
{
	return this->adapterList;
}

}// namespace sqr
