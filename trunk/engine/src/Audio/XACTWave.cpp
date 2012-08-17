#include "StdAfx.h"
#include "XACTWave.h"
#include "XACTSoundBank.h"
#include "Algorithm.h"
#include "StringHelper.h"
#include "ErrLogHelper.h"
#include "CGraphicExp.h"
#include "TSqrAllocator.inl"

namespace sqr
{

//------------------------------------------------------------------------------
CXACTWave::CXACTWave()
: xactWave(NULL)
, xactVolume(1.0f)
, xactPitch(0)
{
	// empty
}

//------------------------------------------------------------------------------
CXACTWave::~CXACTWave()
{
	// empty
}

//------------------------------------------------------------------------------
void CXACTWave::Setup( const CSoundBankPtr& soundBank, index_t waveIndex, bool startPlayback, size_t loopCount )
{
	CWave::Setup(soundBank, waveIndex, startPlayback, loopCount);

	IXACT3WaveBank* xactWaveBank = static_cast<CXACTSoundBank*>(soundBank.get())->GetXACTWaveBank();
	HRESULT hr;
	if (startPlayback)
	{
		hr = xactWaveBank->Play(waveIndex, 0, 0, loopCount, &this->xactWave);
		if(FAILED(hr))
		{
			string msg = format("%d", waveIndex);
			GfkLogErr("xactWaveBankPlayFailed", msg.c_str()); 
		}
	}
	else
	{
		hr = xactWaveBank->Prepare(waveIndex, 0, 0, loopCount, &this->xactWave);
		{
			string msg = format("%d", waveIndex);
			GfkLogErr("xactWaveBankPrepareFailed", msg.c_str()); 
		}
	}

	Ast(NULL != this->xactWave);
}

//------------------------------------------------------------------------------
void CXACTWave::Destroy()
{
	Ast(NULL != this->xactWave);

	this->xactWave->Destroy();
	this->xactWave = 0;

	CWave::Destroy();
}

//------------------------------------------------------------------------------
void CXACTWave::SetVolume( float v )
{
	this->xactVolume = v;
	if (NULL != this->xactWave)
	{
		HRESULT hr = this->xactWave->SetVolume(this->xactVolume);
		if(FAILED(hr))
			GfkLogErr("set volume failed"); 
	}
}

//------------------------------------------------------------------------------
void CXACTWave::SetPitch( float p )
{
	// clamp to [-1,1];
	float l = (clamp(p, -1.0f, 1.0f) + 1.0f) * 0.5f;
	this->xactPitch = (XACTPITCH)lerp(float(XACTPITCH_MIN), float(XACTPITCH_MAX), l);
	if (NULL != this->xactWave)
	{
		HRESULT hr = this->xactWave->SetPitch(this->xactPitch);
		if(FAILED(hr))
			GfkLogErr("set pitch failed"); 
	}
}

//------------------------------------------------------------------------------
void CXACTWave::Play()
{
	Ast(NULL != this->xactWave);
	HRESULT hr = this->xactWave->Play();
	if(FAILED(hr))
		GfkLogErr("wave play failed"); 
}

//------------------------------------------------------------------------------
void CXACTWave::Stop()
{
	Ast(NULL != this->xactWave);
	HRESULT hr = this->xactWave->Stop(XACT_FLAG_STOP_RELEASE);
	if(FAILED(hr))
		GfkLogErr("wave stop failed"); 
}

//------------------------------------------------------------------------------
void CXACTWave::StopImmediate()
{
	Ast(NULL != this->xactWave);
	HRESULT hr = this->xactWave->Stop(XACT_FLAG_STOP_IMMEDIATE);
	if(FAILED(hr))
		GfkLogErr("wave stop immediate failed"); 
}

//------------------------------------------------------------------------------
void CXACTWave::Pause()
{
	Ast(NULL != this->xactWave);
	HRESULT hr = this->xactWave->Pause(TRUE);
	if(FAILED(hr))
		GfkLogErr("wave pause failed");
}

//------------------------------------------------------------------------------
void CXACTWave::Resume()
{
	Ast(NULL != this->xactWave);
	HRESULT hr = this->xactWave->Pause(FALSE);
	if(FAILED(hr))
		GfkLogErr("wave resume failed"); 
}

//------------------------------------------------------------------------------
SoundState::Mask CXACTWave::GetState() const
{
	Ast(NULL != this->xactWave);
	DWORD state = 0;
	HRESULT hr = this->xactWave->GetState(&state);
	if(FAILED(hr))
		GfkLogErr("wave get state failed"); 

	return state;
}

}// namespace sqr
