#include "StdAfx.h"
#include "XACTSoundBank.h"
#include "XACTAudioEngine.h"
#include "CWinPkgFile.h"
#include "ErrLogHelper.h"
#include "StringHelper.h"
#include "CGraphicExp.h"
#include "Dxerr.h"
#include "TSqrAllocator.inl"
#include "CAudioSystem.h"
#include "CBufFile.h"
#include "DebugHelper.h"
#include "CAudioPkgChecker.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_AUD;

//------------------------------------------------------------------------------
CXACTSoundBank::CXACTSoundBank()
: xactSoundBank(NULL)
, xactWaveBank(NULL)
, soundBankData(NULL)
, waveBankData(NULL)
{
	// empty
}

//------------------------------------------------------------------------------
CXACTSoundBank::~CXACTSoundBank()
{
	// empty
}

//------------------------------------------------------------------------------
bool CXACTSoundBank::Setup( const AString& filename, bool streaming )
{
	CSoundBank::Setup(filename, streaming);

	if (!this->SetupWaveBank())
	{
		CSoundBank::Destroy();
		return false;
	}

	if (!this->SetupSoundBank())
	{
		CSoundBank::Destroy();
		return false;
	}

	HRESULT hr;
	// cue properties
	XACTINDEX numCues = 0;
	hr = this->xactSoundBank->GetNumCues(&numCues);
	if (FAILED(hr))
	{
		GfkLogErr("GetNumCues failed");
		return false;
	}

	if (numCues > 0)
	{
		this->cueProperties.reserve(numCues);
		for (index_t i = 0; i < numCues; ++i)
		{
			XACT_CUE_PROPERTIES property;
			hr = this->xactSoundBank->GetCueProperties(i, &property);
			if (FAILED(hr))
			{
				GfkLogErr("GetCueProperties failed", this->filename.c_str());
			}

			CCueProperty cueProperty;
			cueProperty.SetCueName(property.friendlyName);
			cueProperty.SetInteractive(TRUE == property.interactive);
			cueProperty.SetMaxInstances(property.maxInstances);
			cueProperty.SetNumVariations(property.numVariations);

			this->cueProperties.push_back(cueProperty);
			this->cueIndexMap.insert(make_pair(cueProperty.GetCueName(), this->cueProperties.size() - 1));
		}
	}
	// wave properties
	DWORD dwState = 0;
	this->xactWaveBank->GetState(&dwState);
	if( dwState == SoundState::Preparing )
		GfkLogErr("WaveBankIsPreparing", this->filename.c_str());
	else
	{
		XACTINDEX  numWaves = 0;
		hr  = this->xactWaveBank->GetNumWaves(&numWaves);
		if (FAILED(hr))
		{
			GfkLogErr("GetNumWaves failed", this->filename.c_str());
			return false;
		}

		if (numWaves > 0)
		{
			this->waveProperties.reserve(numWaves);
			for (index_t i = 0; i < numWaves; i++)
			{
				XACT_WAVE_PROPERTIES property;
				hr = this->xactWaveBank->GetWaveProperties(i, &property);

				if (FAILED(hr))
				{
					GfkLogErr("GetWaveProperties failed");
					return false;
				}

				CWaveProperty waveProperty;
				waveProperty.SetWaveName(property.friendlyName);
				waveProperty.SetFormat((CWaveProperty::Format)property.format.wFormatTag);
				waveProperty.SetNumChannels(property.format.nChannels);
				if (WAVEBANKMINIFORMAT_BITDEPTH_8 == property.format.wBitsPerSample)
					waveProperty.SetBitsPerSample(8);
				else
					waveProperty.SetBitsPerSample(16);
				waveProperty.SetSamplesPerSecond(property.format.nSamplesPerSec);

				this->waveProperties.push_back(waveProperty);
				this->waveIndexMap.insert(make_pair(waveProperty.GetWaveName(), this->waveProperties.size() - 1));
			}
		}
	}

	return true;
}

//------------------------------------------------------------------------------
void CXACTSoundBank::Destroy()
{
	if(!this->IsValid())
	{
		//GfkLogErr("SoundBank is not valid when is destroyed!");
		GfkLogErr("CXACTSoundBank destroyed failed");
	}

	this->DestroySoundBank();
	this->DestroyWaveBank();
	CSoundBank::Destroy();
}

//------------------------------------------------------------------------------
void CXACTSoundBank::StopCue( index_t cueIndex, bool immediate )
{
	Ast(0 != this->xactSoundBank);
	DWORD dwFlags = immediate ? XACT_FLAG_SOUNDBANK_STOP_IMMEDIATE : 0;
	HRESULT hr = this->xactSoundBank->Stop(cueIndex, dwFlags);
	if(FAILED(hr))
		GfkLogErr("soundbank stop failed"); 
}

//------------------------------------------------------------------------------
void CXACTSoundBank::StopWave( index_t waveIndex, bool immediate )
{
	Ast(0 != this->xactWaveBank);
	DWORD dwFlags = immediate ? XACT_FLAG_STOP_IMMEDIATE : XACT_FLAG_STOP_RELEASE;
	HRESULT hr = this->xactWaveBank->Stop(waveIndex, dwFlags);
	if(FAILED(hr))
		GfkLogErr("soundbank stop failed"); 
}

//------------------------------------------------------------------------------
bool CXACTSoundBank::SetupSoundBank()
{
	Ast(NULL == this->xactSoundBank);
	Ast(NULL == this->soundBankData);
	Ast(!this->filename.empty());
	IXACT3Engine* xactEngine = CXACTAudioEngine::GetInst()->GetXACTEngine();
	Ast(NULL != xactEngine);
	HRESULT hr;

	// load file into memory
	CWinPkgFile	PkgFile;
	HANDLE hFile = PkgFile.Open(PATH_ALIAS_AUD.c_str(), (this->filename+".xsb").c_str());
	if (INVALID_HANDLE_VALUE != hFile)
	{
		DWORD fileSize = GetFileSize(hFile, NULL);
		if (-1 != fileSize)
		{
			DWORD bytesRead;
			this->soundBankData = new byte[fileSize];
			if( 0 != ReadFile(hFile, this->soundBankData, fileSize, &bytesRead, NULL) )
			{
				// create sound bank
				hr = xactEngine->CreateSoundBank(this->soundBankData, fileSize, 0, 0, &this->xactSoundBank);
				if (FAILED(hr) || NULL == this->xactSoundBank)
				{
					string dxmsg   = DXGetErrorDescription(hr);

					uint32 check_code = CAudioPkgChecker::GetInst()->GetCheckCode((this->filename+".xsb").c_str());
					CRefBufFile tempBufFile((const char*)this->soundBankData, fileSize, check_code);
					
					if ( !CheckPkgCrash(tempBufFile) )
					{
						string msg = format("%s, %s", this->filename.c_str(), dxmsg.c_str());
						GfkLogErr("CreateSoundBank ", msg.c_str());
					}

					PkgFile.Close();
					return false;
				}
			}
		}
		PkgFile.Close();
		return true;
	}
	else
	{
		if( !CAudioSystem::GetInst()->SoundFileIsExist((this->filename + ".xsb").c_str()) )
		{
			return false;
		}
		return false;
	}
}

//------------------------------------------------------------------------------
bool CXACTSoundBank::SetupWaveBank()
{
	Ast(NULL == this->xactWaveBank);
	Ast(NULL == this->waveBankData);
	Ast(!this->filename.empty());
	IXACT3Engine* xactEngine = CXACTAudioEngine::GetInst()->GetXACTEngine();
	Ast(NULL != xactEngine);
	HRESULT hr;

	HANDLE hFile;
	CWinPkgFile	PkgFile;
	if (this->isStreaming)
	{		
		hFile = PkgFile.Open( PATH_ALIAS_AUD.c_str(), (this->filename + ".xwb").c_str());
		if (INVALID_HANDLE_VALUE == hFile)
		{
			if( !CAudioSystem::GetInst()->SoundFileIsExist((this->filename + ".xwb").c_str()) )
			{
				return false;
			}
			return false;
		}
		XACT_WAVEBANK_STREAMING_PARAMETERS xactStreamParams = {0};
		xactStreamParams.file		= hFile;
		xactStreamParams.packetSize = 64;
		xactStreamParams.offset     = 0;
		hr = xactEngine->CreateStreamingWaveBank(&xactStreamParams, &this->xactWaveBank);

		if (FAILED(hr) || NULL == this->xactWaveBank)
		{
			GfkLogErr("CreateStreamingWaveBank", this->filename.c_str());

			return false;
		}
	}
	else
	{
		hFile = PkgFile.Open(PATH_ALIAS_AUD.c_str(), (this->filename + ".xwb").c_str());
		if (INVALID_HANDLE_VALUE == hFile)
		{
			if( !CAudioSystem::GetInst()->SoundFileIsExist((this->filename + ".xwb").c_str()) )
			{
				return false;
			}
			return false;
		}
		DWORD fileSize = GetFileSize(hFile, NULL);
		if (-1 != fileSize)
		{
			HANDLE hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, fileSize, NULL);
			if (0 != hMapFile)
			{
				this->waveBankData = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
				if(0 != this->waveBankData)
				{
					hr = xactEngine->CreateInMemoryWaveBank(this->waveBankData,
						fileSize, 0, 0, &this->xactWaveBank);

					if (FAILED(hr) || NULL == this->xactWaveBank)
					{
						string dxmsg   = DXGetErrorDescription(hr);

						CPkgFile tempPkgFile;
						int nRet = tempPkgFile.Open(PATH_ALIAS_AUD.c_str(), (this->filename + ".xwb").c_str());
						uint32 check_code = CAudioPkgChecker::GetInst()->GetCheckCode((this->filename+".xwb").c_str());
						tempPkgFile.SetCheckCode(check_code);

						if ( !CheckPkgCrash(tempPkgFile) )
						{
							string msg = format("%s.xwb, %s", this->filename.c_str(), dxmsg.c_str());
							GfkLogErr("CreateInMemoryWaveBank", msg.c_str());
						}

						CloseHandle(hMapFile);
						PkgFile.Close();

						return false;
					}
				}
				CloseHandle(hMapFile);
			}
		}
		PkgFile.Close();
	}

	// wait until the wave bank is prepared
	CXACTAudioEngine::GetInst()->WaitForWaveBankPrepared();

	return true;
}

//------------------------------------------------------------------------------
void CXACTSoundBank::DestroySoundBank()
{
	Ast(NULL != this->xactSoundBank);
	Ast(NULL != this->soundBankData);

	this->xactSoundBank->Destroy();
	this->xactSoundBank = NULL;

	SafeDelAry(this->soundBankData);
}

//------------------------------------------------------------------------------
void CXACTSoundBank::DestroyWaveBank()
{
	Ast(NULL != this->xactWaveBank);

	this->xactWaveBank->Destroy();
	this->xactWaveBank = NULL;

	if (NULL != this->waveBankData)
	{
		UnmapViewOfFile(this->waveBankData);
		this->waveBankData = NULL;
	}
}

IXACT3SoundBank* CXACTSoundBank::GetXACTSoundBank() const
{
	return this->xactSoundBank;
}

IXACT3WaveBank* CXACTSoundBank::GetXACTWaveBank() const
{
	return this->xactWaveBank;
}
}// namespace sqr
