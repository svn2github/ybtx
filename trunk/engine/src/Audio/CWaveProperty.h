#pragma once
#include "CAudioMallocObject.h"

namespace sqr
{

/// wave属性.
/// 源音频格式建议44kHz 16bit PCM .wav文件(CD音质), 否则可能会不识别
/// 音质可以在选择压缩格式时设置, WMA的话100%的音质压缩率也很高
/// @note WaveName只有在WaveBank选中了"Friendly Names"后才有
class CWaveProperty : public CAudioMallocObject
{
public:
	enum Format
	{
		FormatPCM,		///< 不压缩
		FormatXMA,		///< XBOX上用的
		FormatADPCM,	///< 低压缩率
		FormatWMA		///< 高压缩率(软解码)
	};
public:
	/// 构造函数
	CWaveProperty();

	/// wave名字
	void SetWaveName(const AString& waveName);
	const AString& GetWaveName() const;

	/// wave格式
	void SetFormat(Format f);
	const Format GetFormat() const;

	/// 声道数
	void SetNumChannels(const size_t n);
	const size_t GetNumChannels() const;

	/// 采样率
	void SetSamplesPerSecond(size_t n);
	const size_t GetSamplesPerSecond() const;

	/// 比特率
	void SetBitsPerSample(size_t n);
	const size_t GetBitsPerSample() const;

private:
	AString waveName;
	Format format;
	size_t numChannels;
	size_t samplesPerSecond;
	size_t bitsPerSample;
};

//------------------------------------------------------------------------------
inline CWaveProperty::CWaveProperty()
: format(FormatPCM)
, numChannels(0)
, samplesPerSecond(0)
, bitsPerSample(0)
{
	// empty
}

//------------------------------------------------------------------------------
inline void CWaveProperty::SetWaveName( const AString& waveName )
{
	this->waveName = waveName;
}

//------------------------------------------------------------------------------
inline const AString& CWaveProperty::GetWaveName() const
{
	return this->waveName;
}

//------------------------------------------------------------------------------
inline void CWaveProperty::SetFormat( Format f )
{
	this->format = f;
}

//------------------------------------------------------------------------------
inline const CWaveProperty::Format CWaveProperty::GetFormat() const
{
	return this->format;
}

//------------------------------------------------------------------------------
inline void CWaveProperty::SetNumChannels( const size_t n )
{
	this->numChannels = n;
}

//------------------------------------------------------------------------------
inline const size_t CWaveProperty::GetNumChannels() const
{
	return this->numChannels;
}

//------------------------------------------------------------------------------
inline void CWaveProperty::SetSamplesPerSecond( size_t n )
{
	this->samplesPerSecond = n;
}

//------------------------------------------------------------------------------
inline const size_t CWaveProperty::GetSamplesPerSecond() const
{
	return this->samplesPerSecond;
}

//------------------------------------------------------------------------------
inline void CWaveProperty::SetBitsPerSample( size_t n )
{
	this->bitsPerSample = n;
}

//------------------------------------------------------------------------------
inline const size_t CWaveProperty::GetBitsPerSample() const
{
	return this->bitsPerSample;
}

}// namespace sqr

