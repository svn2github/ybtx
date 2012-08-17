#include "StdAfx.h"
#include "CAudioEngine.h"
#include "XACTAudioEngine.h"
#include "CCue.h"
#include "CWave.h"
#include "CSoundBank.h"
#include "CodeCvs.h"
#include "ErrLogHelper.h"
#include "CGraphicExp.h"
#include "TimeHelper.h"
#include <Windows.h>
#include "CAudioEmitter.h"
#include "StringHelper.h"
#include "TSqrAllocator.inl"
#include "TSingleton.h"
#include "CAudioPkgChecker.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_AUD = L"res";

	WindowsVersion CXACTAudioEngine::eWindowsType = WindowsXP;
	bool		   CAudioEngine::isDestroyResourceInGame = false;

//------------------------------------------------------------------------------
CAudioEngine::CAudioEngine()
: defaultAdapterIndex(InvalidIndex)
, isOpen(false)
, isAudioDeviceValid(true)
, isLastAudioDeviceValid(true)
, isSupport3D(false)
{
	this->listenerPosition.Init(0.0f, 0.0f, 0.0f);
	this->listenerOrientTop.Init(0.0f, 1.0f, 0.0f);
	this->listenerOrientFront.Init(0.0f, 0.0f, 1.0f);
	this->listenerVelocity.Init(0.0f, 0.0f, 0.0f);

	this->EstimateSystemVersion();
}

//------------------------------------------------------------------------------
CAudioEngine::~CAudioEngine()
{
	if ( CAudioPkgChecker::HasInst() )
		CAudioPkgChecker::Destroy();

	// empty
	GraphicErr::SetErrAudioState("CAudioEngine析构完成");
}

//------------------------------------------------------------------------------
bool CAudioEngine::Open()
{
	Ast(this->soundBanks.empty());
	this->isOpen = true;

	if ( !CAudioPkgChecker::HasInst() )
		CAudioPkgChecker::Create();

	return true;
}

//------------------------------------------------------------------------------
void CAudioEngine::Close()
{
	if((this->IsOpen()))
	{
		this->isOpen = false;
		this->DestroySoundResource();
		this->setAudioEmitters.clear();
		CAudioEngine::isDestroyResourceInGame = true;
		GraphicErr::SetErrAudioState("DestroySoundResource finish");
	}
}

//------------------------------------------------------------------------------
void CAudioEngine::Update()
{
	SQR_TRY
	{
		if( eWindowsType == Win7 || eWindowsType == WindowsVista )
		{
			isAudioDeviceValid = CXACTAudioEngine::GetInst()->EstimateCurAudioDeviceIsValid(isLastAudioDeviceValid);

// 			///如果上一刻音效设备为invalid，而这一刻为valid，则需要restore xactengine
 			if( isAudioDeviceValid && !isLastAudioDeviceValid )
 			{
				/////release engine 
				this->NodityAmitterDestroySoundResource();

 				if( !CXACTAudioEngine::GetInst()->RestoreXactEngine() )
				{
					isLastAudioDeviceValid = isAudioDeviceValid;
					return;
				}
 			}

			isLastAudioDeviceValid = isAudioDeviceValid;
		}

		if( isAudioDeviceValid )
		{
			this->DestroyStoppedCues();
			this->DestroyStoppedWaves();

			// update 3d effects
			for (AList<CCuePtr>::iterator it = this->activeNormalCues.begin();
				it != this->activeNormalCues.end(); ++it)
			{
				CCuePtr& cue = (*it);
				cue->Update();
			}

			for (AList<CCuePtr>::iterator it = this->activeEmitterCues.begin();
				it != this->activeEmitterCues.end(); ++it)
			{
				CCuePtr& cue = (*it);
				cue->Update();
			}
		}

	}
	SQR_CATCH(exp)
	{
		exp.AppendType("Update");
		GfkLogAudioExpOnce(exp);
	}
	SQR_TRY_END;
}

//------------------------------------------------------------------------------
bool CAudioEngine::LoadSoundBank( const AString& filename, bool streaming )
{
	if(this->HasSoundBank(filename))
		GfkLogErr("SoundBankHasLoad", filename.c_str());

	CSoundBankPtr newSoundBank = this->CreateSoundBank();
	if (newSoundBank->Setup(filename, streaming))
	{
		this->soundBanks.insert(make_pair(filename, newSoundBank));
		return true;
	}

	return false;
}

//------------------------------------------------------------------------------
void CAudioEngine::UnloadSoundBank( const AString& filename )
{
	Ast(this->HasSoundBank(filename));
	this->soundBanks[filename]->Destroy();
	this->soundBanks.erase(this->soundBanks.find(filename));
}

//------------------------------------------------------------------------------
const CSoundBankPtr& CAudioEngine::GetSoundBankByIndex( index_t index )
{
	index_t i = 0;
	for (AMap<AString, CSoundBankPtr>::const_iterator it = this->soundBanks.begin();
		it != this->soundBanks.end(); ++it)
	{
		if (i == index)
		{
			return it->second;
		}
		++i;
	}
	static CSoundBankPtr nullPtr;
	return nullPtr;
}

//------------------------------------------------------------------------------
index_t CAudioEngine::GetSoundBankIndexByName(const AString &name) const
{
	index_t i = 0;
	AString strSoundBankName = "";

	for (AMap<AString, CSoundBankPtr>::const_iterator it = this->soundBanks.begin();
		it != this->soundBanks.end(); ++it)
	{
		strSoundBankName = it->first;
		
		if ( strSoundBankName.compare(name) == 0 )
		{
			return i;
		}
		++i;
	}
	return 0;
}

//------------------------------------------------------------------------------
const CSoundBankPtr& CAudioEngine::GetSoundBankByName( const AString& name ) const
{
	AMap<AString, CSoundBankPtr>::const_iterator it = this->soundBanks.find(name);
	if (it != this->soundBanks.end())
	{
		return it->second;
	}
	else
	{
		static CSoundBankPtr nullPtr;
		return nullPtr;
	}
}

//------------------------------------------------------------------------------
const CCuePtr& CAudioEngine::PlayCue( const AString& cueName, bool play, bool enable3D, const CVector3f& position, const CVector3f& velocity )
{
	Ast(this->IsOpen());

	CSoundBankPtr soundBank;
	index_t cueIndex = this->FindCue(cueName, soundBank);
	if (InvalidIndex != cueIndex)
	{
			CCuePtr newCue = this->CreateCue();
			if( !newCue.isvalid() )
			{
				GfkLogAudioErrOnce("CreateCueFailed", cueName.c_str());
				
				static CCuePtr nullPtr;
				return nullPtr;
			}

			newCue->Setup(soundBank, cueName, cueIndex, play, enable3D);
			this->activeEmitterCues.push_back(newCue);
			// this must be done after the cue is active, or there is a delay on 3d effect [11/29/2008 xoyojank]
			if (enable3D)
			{
				newCue->SetPosition(position);
				newCue->SetVelocity(velocity);
				newCue->Update();
			}
			return this->activeEmitterCues.back();
	}
	else
	{
		static CCuePtr nullPtr;
		
		string log = cueName.c_str();
		ArtErr("播放音效缺失:" + log);
		
		return nullPtr;
	}
}

//------------------------------------------------------------------------------
void CAudioEngine::PlayGloablCue(const AString& cueName, bool play, bool enable3D, const CVector3f& position, const CVector3f& velocity)
{
	Ast(this->IsOpen());

	CSoundBankPtr soundBank;
	index_t cueIndex = this->FindCue(cueName, soundBank);
	if (InvalidIndex != cueIndex)
	{
		SQR_TRY
		{
			CCuePtr newCue = this->CreateCue();
			if( !newCue.isvalid() )
			{
				GfkLogAudioErrOnce("CreateCueFailed", cueName.c_str());
				return;
			}

			newCue->Setup(soundBank, cueName, cueIndex, play, enable3D);
			this->activeNormalCues.push_back(newCue);
			// this must be done after the cue is active, or there is a delay on 3d effect [11/29/2008 xoyojank]
			if (enable3D)
			{
				newCue->SetPosition(position);
				newCue->SetVelocity(velocity);
				newCue->Update();
			}
		}
		SQR_CATCH(exp)
		{
			string log = cueName.c_str();
			exp.AppendMsg(log);
			exp.AppendType("PlayGloablCue");

			GfkLogAudioExpOnce(exp);
		}
		SQR_TRY_END;
	}
	else
	{
		string log = cueName.c_str();
		ArtErr("播放音效缺失:" + log);
	}
}

//------------------------------------------------------------------------------
void CAudioEngine::StopCue( const AString& cueName, bool immediate )
{
	SQR_TRY
	{
		Ast(this->IsOpen());

		CSoundBankPtr soundBank;
		index_t cueIndex = this->FindCue(cueName, soundBank);
		if (InvalidIndex != cueIndex)
		{
			soundBank->StopCue(cueIndex, immediate);
		}
		else
		{
			string log = cueName.c_str();
			ArtErr("停止音效缺失:" + log);
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("StopCue");
		exp.AppendMsg(cueName.c_str());

		GfkLogAudioExpOnce(exp);
	}
	SQR_TRY_END;
}

//------------------------------------------------------------------------------
bool CAudioEngine::CueIsStop(const AString& cueName)
{
	SQR_TRY
	{
		for (AList<CCuePtr>::iterator it = this->activeNormalCues.begin();	it != this->activeNormalCues.end(); ++it)
		{
			CCuePtr& cue = (*it);
			AString temp = cue->GetCueName();
			if( cueName == temp && !cue->IsStopped())
				return false;
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("CueIsStop");
		exp.AppendMsg(cueName.c_str());

		GfkLogAudioExpOnce(exp);
		return false;
	}
	SQR_TRY_END;

	return true;
}

//------------------------------------------------------------------------------
const CWavePtr& CAudioEngine::PlayWave( const AString& waveName, bool play, size_t loopCount, float volume, float pitch )
{
	Ast(this->IsOpen());
	Ast(!waveName.empty());
	CSoundBankPtr soundBank;
	index_t waveIndex = this->FindWave(waveName, soundBank);
	if (InvalidIndex != waveIndex)
	{
		CWavePtr newWave = this->CreateWave();
		newWave->SetVolume(volume);
		newWave->SetPitch(pitch);
		newWave->Setup(soundBank, waveIndex, play, loopCount);
		this->activeWaves.push_back(newWave);
		return this->activeWaves.back();
	}
	else
	{
		printf("PlayWave() not find: %s\n", waveName.c_str());
		static CWavePtr nullPtr;
		return nullPtr;
	}
}

//------------------------------------------------------------------------------
void CAudioEngine::StopWave( const AString& waveName, bool immediate )
{
	Ast(this->IsOpen());
	Ast(!waveName.empty());
	CSoundBankPtr soundBank;
	index_t waveIndex = this->FindWave(waveName, soundBank);
	if (InvalidIndex != waveIndex)
	{
		soundBank->StopWave(waveIndex, immediate);
	}
	else
	{
		printf("StopWave() not find: %s\n", waveName.c_str());
	}
}

//------------------------------------------------------------------------------
index_t CAudioEngine::FindCue( const AString& cueName, CSoundBankPtr& outSoundBank )
{
	for (AMap<AString, CSoundBankPtr>::iterator it = this->soundBanks.begin();
		it != this->soundBanks.end(); ++it)
	{
		CSoundBankPtr& soundBank = it->second;
		index_t cueIndex = soundBank->FindCueIndexByName(cueName);
		if (InvalidIndex != cueIndex)
		{
			outSoundBank = soundBank;
			return cueIndex;
		}
	}
	return InvalidIndex;
}

//------------------------------------------------------------------------------
index_t CAudioEngine::FindWave( const AString& waveName, CSoundBankPtr& outSoundBank )
{
	Ast(!waveName.empty());
	for (AMap<AString, CSoundBankPtr>::iterator it = this->soundBanks.begin();
		it != this->soundBanks.end(); ++it)
	{
		CSoundBankPtr& soundBank = it->second;
		index_t waveIndex = soundBank->FindWaveIndexByName(waveName);
		if (InvalidIndex != waveIndex)
		{
			outSoundBank = soundBank;
			return waveIndex;
		}
	}
	return InvalidIndex;
}

//------------------------------------------------------------------------------
void CAudioEngine::DestroyStoppedCues()
{
	SQR_TRY
	{
		for (AList<CCuePtr>::iterator it = this->activeNormalCues.begin();	it != this->activeNormalCues.end();)
		{
			CCuePtr& cue = (*it);

			bool isStop = true;
			SQR_TRY
			{
				SoundState::Mask curStateMask = cue->GetState();
				if( curStateMask == SoundState::Invalid )
					isStop = true;
				else
					isStop = cue->IsStopped();
			}
			SQR_CATCH(exp)
			{
				string msg;
				msg = format("%d, %d", eWindowsType, CAudioEngine::isDestroyResourceInGame);
				exp.AppendMsg(msg);
				exp.AppendType("DesStoppedCue");
				GfkLogAudioExpOnce(exp);
				isStop = true;
			}
			SQR_TRY_END;

			if (isStop)
			{
				this->activeNormalCues.erase(it++);
			}
			else
			{
				++it;
			}
		}

		for (AList<CCuePtr>::iterator it = this->activeEmitterCues.begin();	it != this->activeEmitterCues.end();)
		{
			CCuePtr& cue = (*it);

			bool isStop = true;
			SQR_TRY
			{
				SoundState::Mask curStateMask = cue->GetState();
				if( curStateMask == SoundState::Invalid )
					isStop = true;
				else
					isStop = cue->IsStopped();
			}
			SQR_CATCH(exp)
			{
				string msg;
				msg = format("%d, %d", eWindowsType, CAudioEngine::isDestroyResourceInGame);
				exp.AppendMsg(msg);
				exp.AppendType("DesStoppedCue");
				GfkLogAudioExpOnce(exp);
				isStop = true;
			}
			SQR_TRY_END;

			if (isStop)
			{
				this->activeEmitterCues.erase(it++);
			}
			else
			{
				++it;
			}
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("DestroyStoppedCues");
		GfkLogAudioExpOnce(exp);
	}
	SQR_TRY_END;
}

//------------------------------------------------------------------------------
void CAudioEngine::DestroyStoppedWaves()
{
	SQR_TRY
	{
		for (AList<CWavePtr>::iterator it = this->activeWaves.begin(); it != this->activeWaves.end();)
		{
			CWavePtr& wave = (*it);
			if (wave.isvalid() && wave->IsStopped())
			{
				wave->Destroy();
				this->activeWaves.erase(it++);
			}
			else
			{
				++it;
			}
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("DestroyStoppedWaves");
		GfkLogAudioExpOnce(exp);
	}
	SQR_TRY_END;
}

//------------------------------------------------------------------------------
void CAudioEngine::DestroyWavesResource()
{
	for (AList<CWavePtr>::iterator it = this->activeWaves.begin(); it != this->activeWaves.end(); ++it)
	{
		CWavePtr& wave = (*it);
		wave->Destroy();
	}

	this->activeWaves.clear();
}

//------------------------------------------------------------------------------
void CAudioEngine::DestroySoundBankResource()
{
	for (AMap<AString, CSoundBankPtr>::iterator it = this->soundBanks.begin(); it != this->soundBanks.end(); ++it)
	{
		CSoundBankPtr& soundBank = it->second;
		soundBank->Destroy();
	}

	this->soundBanks.clear();
}

//------------------------------------------------------------------------------
void CAudioEngine::DestroySoundResource()
{
	SQR_TRY
	{
		GraphicErr::SetErrAudioState("Start DestroySoundResource");

		// clear cues 
		for (AList<CCuePtr>::iterator it = this->activeNormalCues.begin(); it != this->activeNormalCues.end();)
		{
			this->activeNormalCues.erase(it++);
		}
		this->activeNormalCues.clear();

		for (AList<CCuePtr>::iterator it = this->activeEmitterCues.begin(); it != this->activeEmitterCues.end();)
		{
			this->activeEmitterCues.erase(it++);
		}
		this->activeEmitterCues.clear();
		//////////////////////////////////////////////////////////////////////////

		// clear waves
		this->DestroyWavesResource();

		// clear soundbanks
		this->DestroySoundBankResource();
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("DestroySoundResource");
		GfkLogAudioExpOnce(exp);
	}
	SQR_TRY_END;
}

//------------------------------------------------------------------------------
void CAudioEngine::NodityAmitterDestroySoundResource()
{
	CAudioEngine::isDestroyResourceInGame = true;
	GraphicErr::SetErrAudioState("StartNodityAmitter");

	SQR_TRY
	{
		SQR_TRY
		{
			// clear cues 
			for (AList<CCuePtr>::iterator it = this->activeNormalCues.begin(); it != this->activeNormalCues.end();)
			{
				this->activeNormalCues.erase(it++);
			}
			this->activeNormalCues.clear();

			activeEmitterCues.clear();
		}
		SQR_CATCH(exp)
		{
			exp.AppendType("ClearCues");
			GfkLogAudioExpOnce(exp);
		}
		SQR_TRY_END;

		set<CAudioEmitter*>::iterator beg = setAudioEmitters.begin();
		set<CAudioEmitter*>::iterator end = setAudioEmitters.end();
		for ( beg; beg != end; ++ beg )
		{
			SQR_TRY
			{
				CAudioEmitter *pAudioEmitter = *beg;
				if( pAudioEmitter )
					pAudioEmitter->Noticfy();
			}
			SQR_CATCH(exp)
			{
				exp.AppendType("pAudioEmitter->Noticfy");
				GfkLogAudioExpOnce(exp);
			}
			SQR_TRY_END;
		}

		//////////////////////////////////////////////////////////////////////////

		// clear waves
		this->DestroyWavesResource();

		// clear soundbanks
		this->DestroySoundBankResource();
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("NodityAmitterDestroySoundResource");
		GfkLogAudioExpOnce(exp);
	}
	SQR_TRY_END;

	GraphicErr::SetErrAudioState("NodityAmitterFinish");
}

//------------------------------------------------------------------------------
inline void
CAudioEngine::UpdateListener( const CVector3f& pos,
							  const CVector3f& top,
							  const CVector3f& front,
							  const CVector3f& velocity )
{
	this->listenerPosition = pos;
	this->listenerOrientTop = top;
	this->listenerOrientFront = front;
	this->listenerVelocity = velocity;
}

void CAudioEngine::EstimateSystemVersion()
{
	OSVERSIONINFOEX osvi;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	GetVersionEx ( (OSVERSIONINFO *) &osvi);

	switch (osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		{
			if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 && osvi.wProductType == VER_NT_WORKSTATION )
				eWindowsType = WindowsVista;

			else if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1 )
				eWindowsType = Win7;

			if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
				eWindowsType = WindowsXP;
		}
		break;

// 	case VER_PLATFORM_WIN32s:
// 		printf ("Microsoft Win32s\n");
// 		break;

	default:
		break;
	}
}

void CAudioEngine::InsertAudioEmitter( CAudioEmitter *pAudioEmitter )
{
	this->setAudioEmitters.insert(pAudioEmitter);
}

void CAudioEngine::DeleteAudioEmitter( CAudioEmitter *pAudioEmitter )
{
	size_t size = this->setAudioEmitters.size();
	set<CAudioEmitter*>::iterator iter = this->setAudioEmitters.find(pAudioEmitter);
	if( iter != this->setAudioEmitters.end() )
	{
		this->setAudioEmitters.erase(iter);
	}
}
}// namespace sqr
