#pragma once
#include "CRefObject.h"
#include "CCueProperty.h"
#include "CWaveProperty.h"
#include "CAudioMallocObject.h"

namespace sqr
{

SmartPointer(CSoundBank);

/// 声音"银行".
/// wave bank + sound bank封装到了一起, 这就要求它们两个必需同名
/// @note 流式加载的需要把wavebank的type设置为Streaming
class CSoundBank 
	: public CRefObject
	, public CAudioMallocObject
{
public:
	/// 构造函数
	CSoundBank();
	/// 析构函数
	virtual ~CSoundBank();

	/// 初始化
	virtual bool Setup(const AString& filename, bool streaming);
	/// 销毁
	virtual void Destroy();
	/// 是否初始化了
	bool IsValid() const;

	/// 返回sound bank名字
	const AString& GetFileName() const;
	/// 是否流式加载
	bool IsStreaming() const;

	/// 返回Cue数目
	size_t GetNumCues() const;
	/// 取得Cue属性
	const CCueProperty& GetCueProperty(index_t cueIndex) const;
	/// 是否存在指定Cue
	bool HasCue(const AString& cueName) const;
	/// 根据Cue名字查找索引
	index_t FindCueIndexByName(const AString& cueName) const;

	/// 返回Wave数目
	size_t GetNumWaves();
	/// 取得Wave属性
	const CWaveProperty& GetWaveProperty(index_t waveIndex) const;
	/// 是否存在指定Wave
	bool HasWave(const AString& waveName) const;
	/// 根据Wave名字查找索引
	index_t FindWaveIndexByName(const AString& waveName) const;

	/// 停止所有Cue的实例
	virtual void StopCue(index_t cueIndex, bool immediate) = 0;
	/// 停止所有Wave的实例
	virtual void StopWave(index_t waveIndex, bool immediate) = 0;

	/// 返回 XACT sound bank 的指针
	virtual IXACT3SoundBank* GetXACTSoundBank() const = 0;
	/// 返回 XACT wave bank 的指针
	virtual IXACT3WaveBank* GetXACTWaveBank() const = 0;

	AString GetSoundBankFileName() const;

protected:
	AString filename;
	bool isValid;
	bool isStreaming;
	AVector<CCueProperty> cueProperties;
	AVector<CWaveProperty> waveProperties;
	AMap<AString, index_t> cueIndexMap;
	AMap<AString, index_t> waveIndexMap;
};



//------------------------------------------------------------------------------
inline bool CSoundBank::IsValid() const
{
	return this->isValid;
}

//------------------------------------------------------------------------------
inline const AString& CSoundBank::GetFileName() const
{
	return this->filename;
}

//------------------------------------------------------------------------------
inline bool CSoundBank::IsStreaming() const
{
	return this->isStreaming;
}

//------------------------------------------------------------------------------
inline size_t CSoundBank::GetNumCues() const
{
	return this->cueIndexMap.size();
}

//------------------------------------------------------------------------------
inline size_t CSoundBank::GetNumWaves()
{
	return this->waveIndexMap.size();
}

//------------------------------------------------------------------------------
inline const CCueProperty& CSoundBank::GetCueProperty( index_t cueIndex ) const
{
	return this->cueProperties[cueIndex];
}

//------------------------------------------------------------------------------
inline bool CSoundBank::HasCue( const AString& cueName ) const
{
	return (this->cueIndexMap.end() != this->cueIndexMap.find(cueName));
}

//------------------------------------------------------------------------------
inline const CWaveProperty& CSoundBank::GetWaveProperty( index_t waveIndex ) const
{
	return this->waveProperties[waveIndex];
}

//------------------------------------------------------------------------------
inline bool CSoundBank::HasWave( const AString& waveName ) const
{
	return (this->waveIndexMap.end() != this->waveIndexMap.find(waveName));
}

//------------------------------------------------------------------------------
inline index_t CSoundBank::FindWaveIndexByName( const AString& waveName ) const
{
	AMap<AString, index_t>::const_iterator it = this->waveIndexMap.find(waveName);
	if (this->waveIndexMap.end() != it)
		return it->second;
	else
		return InvalidIndex;
}

}// namespace sqr
