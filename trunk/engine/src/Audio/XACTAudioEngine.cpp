#include "StdAfx.h"
#include "XACTAudioEngine.h"
#include "XACTCue.h"
#include "XACTWave.h"
#include "XACTSoundBank.h"
#include "IAudioSystem.h"
#include "IAudioModule.h"
#include "CAudioSystem.h"
#include "CWinPkgFile.h"
#include "StringHelper.h"
#include "ErrLogHelper.h"
#include "ThreadHelper.h"
#include "Dxerr.h"
#include "CGraphicExp.h"
#include "TSqrAllocator.inl"

#pragma comment(lib, "winmm.lib")

namespace sqr
{
	extern const wstring PATH_ALIAS_AUD;

bool CXACTAudioEngine::isNeedUpdateThread = false;
bool CXACTAudioEngine::isCloseAudioEngine = false;
bool CXACTAudioEngine::isDetachXactEngine = false;

//------------------------------------------------------------------------------
CXACTAudioEngine::CXACTAudioEngine()
: xactEngine(NULL)
, globalSettingsBuffer(NULL)
, globalSettingsBufferSize(0)
, updateThread(NULL)
{
	// create event
	this->waveBankPreparedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	// default settings
	this->globalSettingsPath = "Sound/globals.xgs";
}

//------------------------------------------------------------------------------
CXACTAudioEngine::~CXACTAudioEngine()
{
	this->mapVariableVolumes.clear();

	if (this->IsOpen())
	{
		this->Close();
	}

	//A thread must call CoUninitialize once for each successful call it has made to CoInitialize or CoInitializeEx
	CoUninitialize();

	GraphicErr::SetErrAudioState("CXACTAudioEngine析构完成");
}

//------------------------------------------------------------------------------
size_t CXACTAudioEngine::GetNumAdapters() const
{
	XACTINDEX numAdapters = 0;

	SQR_TRY	
	{
		HRESULT hr = this->xactEngine->GetRendererCount(&numAdapters);
		if (FAILED(hr))
		{
			MessageBox(NULL, "没有检测到音频设备, 请检查系统的\"Windows Audio\"服务是否启动", NULL, 0);
			
			return 0;
		}
		else
		{
			if( numAdapters == 0 && eWindowsType == Win7 )
			{
				MessageBox(NULL, "没有检测到音频设备, 请检查管理工具\\系统\\服务下的\"Windows Audio\"服务是否启动", NULL, 0);	
			}
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("GetNumAdapters");

		GfkLogAudioExpOnce(exp);
	}
	SQR_TRY_END;

	return numAdapters;
}

//------------------------------------------------------------------------------
CAudioAdapterInfo CXACTAudioEngine::GetAdapterInfo( index_t adapterIndex ) const
{
	Ast(NULL != this->xactEngine);
	if (InvalidIndex == adapterIndex)
	{
		adapterIndex = this->GetDefaultAdapterIndex();
	}

	CAudioAdapterInfo info;
	XACT_RENDERER_DETAILS details = {0};
	HRESULT hr = this->xactEngine->GetRendererDetails(adapterIndex, &details);
	if(SUCCEEDED(hr))
	{
		// convert names from unicode to ANSI
		char rendererId[XACT_RENDERER_ID_LENGTH + 1];
		char displayName[XACT_RENDERER_NAME_LENGTH + 1];
		WideCharToMultiByte(CP_ACP, 0, details.rendererID, -1, rendererId, sizeof(rendererId), NULL, NULL);
		WideCharToMultiByte(CP_ACP, 0, details.displayName, -1, displayName, sizeof(displayName), NULL, NULL);


		info.SetAdpaterId(rendererId);
		info.SetDisplayName(displayName);
		info.SetDefaultAdapter(TRUE == details.defaultDevice);
	}

	return info;
}

//------------------------------------------------------------------------------
index_t CXACTAudioEngine::GetDefaultAdapterIndex() const
{
	size_t num = this->GetNumAdapters();
	SQR_TRY	
	{
		for (index_t i = 0; i < num; i++)
		{
			XACT_RENDERER_DETAILS details = {0};
			HRESULT hr = this->xactEngine->GetRendererDetails(i, &details);
			if (FAILED(hr))
			{
				return InvalidIndex;
			}

			if (details.defaultDevice)
			{
				return i;
			}
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("GetDefaultAdapterIndex");
		GfkLogAudioExpOnce(exp);
	}
	SQR_TRY_END;
	
	return InvalidIndex;
}

//------------------------------------------------------------------------------
bool CXACTAudioEngine::InitializeEngine()
{
	SQR_TRY
	{
		// global settings
		if (!this->LoadGlobalSettings())
			return false;

		HRESULT hr;

		// adapter details
		if (InvalidIndex == this->defaultAdapterIndex)
		{
			this->defaultAdapterIndex = this->GetDefaultAdapterIndex();
		}

		if( this->defaultAdapterIndex == InvalidIndex )
			return false;

		// initialize engine
		XACT_RUNTIME_PARAMETERS xactRuntimeParams = {0};
		xactRuntimeParams.lookAheadTime = XACT_ENGINE_LOOKAHEAD_DEFAULT;
		xactRuntimeParams.pGlobalSettingsBuffer = this->globalSettingsBuffer;
		xactRuntimeParams.globalSettingsBufferSize = this->globalSettingsBufferSize;
		xactRuntimeParams.globalSettingsFlags = XACT_FLAG_GLOBAL_SETTINGS_MANAGEDATA; // this will tell XACT to delete[] the buffer when its unneeded

		XACT_RENDERER_DETAILS details = {0};
		hr = this->xactEngine->GetRendererDetails(this->defaultAdapterIndex, &details);
		if (FAILED(hr))
		{
			xactRuntimeParams.pRendererID = 0;
		}
		else
			xactRuntimeParams.pRendererID = details.rendererID;

		switch ( eWindowsType )
		{
		case WindowsVista:
			{
				wstring strAudioDeviceName = details.displayName;
				if( strAudioDeviceName.find(L"Digital") != -1 && strAudioDeviceName.find(L"SPDIF") == -1 )
				{
					isNeedUpdateThread = false;
				}
			}
			break;

		case Win7:
			{
				wstring strAudioDeviceName = details.displayName;
				if( strAudioDeviceName.find(L"S/PDIF") != -1 )
				{
					isNeedUpdateThread = false;
				}
			}
			break;

		default:
			break;
		}

		xactRuntimeParams.fnNotificationCallback = NotificationCallback;
 		hr = this->xactEngine->Initialize(&xactRuntimeParams);
		if (FAILED(hr))
			return false;

		ZeroMemory(&this->dspSettings, sizeof(this->dspSettings));
		ZeroMemory(&this->dspMatrixCoefficients, sizeof(this->dspMatrixCoefficients));
		ZeroMemory(&this->dspDelayTimes, sizeof(this->dspDelayTimes));
		ZeroMemory(&this->x3dAudioHandle, sizeof(this->x3dAudioHandle));

		// dsp settings for 3d audio
		WAVEFORMATEXTENSIBLE wfxFinalMixFormat = { 0 };
		hr = this->xactEngine->GetFinalMixFormat(&wfxFinalMixFormat);
		if (FAILED(hr))
		{
			AString msg = DXGetErrorDescription(hr);
			GfkDrvLog("XACT Engine failed to GetFinalMixFormat!", msg.c_str());
			return false;
		}

		WORD nDstChannelCount = wfxFinalMixFormat.Format.nChannels;
		if( nDstChannelCount > MAX_DSP_CHANNELS )
		{
			string msg = DXGetErrorDescription(hr);
			msg = format("%s, %d", msg.c_str(), nDstChannelCount);
			GfkDrvLog("MAX_DSP_CHANNELS", msg.c_str());
			this->isSupport3D = false;
		}

		this->dspSettings.pMatrixCoefficients = this->dspMatrixCoefficients;
		this->dspSettings.pDelayTimes = this->dspDelayTimes;
		this->dspSettings.SrcChannelCount = 1;
		this->dspSettings.DstChannelCount = nDstChannelCount;

		// initialize the XACT3D functions (3D only works if global settings are provided)
		this->isSupport3D = true;
		hr = XACT3DInitialize(this->xactEngine, this->x3dAudioHandle);
		if (FAILED(hr))
		{
			AString msg = DXGetErrorDescription(hr);
			GfkDrvLog("XACT3DInitialize Failed!", msg.c_str());	
			this->isSupport3D = false;
		}

		// register notifications
		XACT_NOTIFICATION_DESCRIPTION notifyDesc = {0};

		notifyDesc.type = XACTNOTIFICATIONTYPE_WAVEBANKPREPARED;
		notifyDesc.flags = XACT_FLAG_NOTIFICATION_PERSIST;
		notifyDesc.pvContext = this;
		///If pSoundBank is NULL, this entry should contain the value XACTINDEX_INVALID
		notifyDesc.cueIndex  = XACTINDEX_INVALID;
		hr = this->xactEngine->RegisterNotification(&notifyDesc);
		if (FAILED(hr))
		{
			AString msg = DXGetErrorDescription(hr);
			GfkDrvLog("RegisterNotification failed", msg.c_str());
			return false;
		}

		bool isAudioDeviceValid = this->EstimateCurAudioDeviceIsValid(true);
		CAudioEngine::SetAudioDeviceIsValid(isAudioDeviceValid);

		isNeedUpdateThread = true;
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("InitializeEngine");
		GfkAudioLogExp(exp);
		return false;
	}
	SQR_TRY_END;

	return true;
}

//------------------------------------------------------------------------------
/*
Loading界面的时候 主线程没有控制权，这样就不会Update音效库
否则，Loading 的时候背景音乐播放就会断断续续的
*/
///CreateAudioThread
void CXACTAudioEngine::CreateAudioThread()
{
	SQR_TRY
	{
		this->updateEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		this->detachEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		this->attachEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		ResetEvent(this->updateEvent);
		ResetEvent(this->detachEvent);
		ResetEvent(this->attachEvent);

		sqr::CreateThread(&this->updateThread, 1024, CXACTAudioEngine::ThreadProc, this); 
		
		Ast(NULL != this->updateThread);
		//声音线程的优先级不能调低，这回容易造成声音卡顿，相反应该调高
		SetThreadPriority(&this->updateThread, THREAD_PRIORITY_ABOVE_NORMAL);
		//ResumeThread(this->updateThread);
	}
	SQR_CATCH(exp)
	{
		exp.AppendMsg("CreateAudioThread faild");
		GfkLogExp(exp);
	}
	SQR_TRY_END;
}

//------------------------------------------------------------------------------
bool CXACTAudioEngine::Open()
{
	if( !OpenXACT() )
		return false;

	if(NULL == this->xactEngine)
		return false;

	if( !this->InitializeEngine() )
		return false;

	isCloseAudioEngine = false;

	// create update thread
	this->CreateAudioThread();

	CAudioEngine::Open();

	return true;
}

//------------------------------------------------------------------------------
void CXACTAudioEngine::Close()
{
	GraphicErr::SetErrAudioState("CXACTAudioEngine::Close()");

	isNeedUpdateThread = false;
	isCloseAudioEngine = true;

	this->ReleaseGlobalSettings();
	
	SetEvent(this->updateEvent);
	
	sqr::JoinThread(&this->updateThread);
	//CloseHandle(this->updateThread);
	
	CAudioEngine::Close();

	HRESULT hr = this->xactEngine->ShutDown();

	CloseHandle(this->waveBankPreparedEvent);
	CloseHandle(this->updateEvent);
	CloseHandle(this->detachEvent);

	this->CloseXACT();
}

//------------------------------------------------------------------------------
void CXACTAudioEngine::ReleaseGlobalSettings()
{
	this->globalSettingsBuffer = NULL;
	this->globalSettingsBufferSize = 0;
}

//------------------------------------------------------------------------------
CSoundBank* CXACTAudioEngine::CreateSoundBank()
{
	return new CXACTSoundBank();
}

//------------------------------------------------------------------------------
CCue* CXACTAudioEngine::CreateCue()
{
	return new CXACTCue();
}

//------------------------------------------------------------------------------
CWave* CXACTAudioEngine::CreateWave()
{
	return new CXACTWave();
}

//------------------------------------------------------------------------------
bool CXACTAudioEngine::OpenXACT()
{
	bool bOpenSuccess = false;
	if (NULL == this->xactEngine)
	{
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		if( FAILED( hr ) )
		{
			CoUninitialize();
			hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		}
		if( SUCCEEDED( hr ) )
		{
			SQR_TRY
			{
				hr = XACT3CreateEngine(0, &this->xactEngine);
				if( FAILED(hr) || this->xactEngine == NULL )
				{
					MessageBox(NULL, "您的dx版本不是最新的，请运行客户端目录下的dx9c文件夹中的安装文件", NULL, 0);

					isAudioDeviceValid = false;
					bOpenSuccess = false;
				}
				else
					bOpenSuccess = true;
			}
 			SQR_CATCH(exp)
 			{
 				exp.AppendType("CreateXACTEngine");
 				GfkLogAudioExpOnce(exp);
 				
 				isAudioDeviceValid = false;
 				bOpenSuccess = false;
 			}
 			SQR_TRY_END;
		}
		else
		{
			string msg("");
			switch (hr)
			{
			case RPC_E_CHANGED_MODE:
				msg = "RPC_E_CHANGED_MODE";
				break;

			case S_FALSE :
				msg = "S_FALSE ";
				break;

			case E_OUTOFMEMORY :
				msg = "E_OUTOFMEMORY ";
				break;

			case E_INVALIDARG :
				msg = "E_INVALIDARG ";
				break;

			case E_UNEXPECTED :
				msg = "E_UNEXPECTED ";
				break;

			default:
				msg = "Other";
				break;
			}
			GfkLogAudioErrOnce("CoInitializeEx Failed", msg);
			return false;
		}
	}
	else
	{
		bOpenSuccess = false;
		GfkLogAudioErrOnce("xactEngine is already open\n");
	}

	return bOpenSuccess;
}

//------------------------------------------------------------------------------
void CXACTAudioEngine::ReleaseEngine()
{

}

//------------------------------------------------------------------------------
void CXACTAudioEngine::CloseXACT()
{
	SafeRelease(this->xactEngine);
}

//------------------------------------------------------------------------------
bool CXACTAudioEngine::RestoreXactEngine()
{
	isNeedUpdateThread		  = false;
	isDetachXactEngine		  = true;
	this->defaultAdapterIndex = InvalidIndex;
	this->SetIsOpen(false);

	SQR_TRY
	{
		if ( WAIT_OBJECT_0 == WaitForSingleObject(this->detachEvent, INFINITE) )
		{
			ResetEvent(this->attachEvent);

			///reload audio resource
			if( this->xactEngine )
			{
				this->xactEngine->ShutDown();
				this->xactEngine->Release();
				this->xactEngine = NULL;
			}

			this->ReleaseGlobalSettings();

			HRESULT hr = XACT3CreateEngine(0, &this->xactEngine);
			if( FAILED(hr) || this->xactEngine == NULL )
			{
				isDetachXactEngine = false;
				SetEvent(this->attachEvent);

				return false;
			}

			if(this->InitializeEngine())
			{
				///restore volume
				CXACTAudioEngine::GetInst()->RestoreVolume();

				///reload audio resoure
				IAudioSystem* audioSystem = GetAudioModule()->GetAudioSystem();
				if (audioSystem)
				{
					audioSystem->LoadSoundBankFromXml("sound/config.xml");
					this->SetIsOpen(true);
				}
				else
				{
					isDetachXactEngine = false;
					SetEvent(this->attachEvent);
					GfkLogAudioErrOnce("AudioSystem is NULL");
					return false;
				}
			}
			else
			{
				isDetachXactEngine = false;
				SetEvent(this->attachEvent);
				return false;
			}

			isDetachXactEngine = false;
			SetEvent(this->attachEvent);
		}
	}
	SQR_CATCH(exp)
	{
		isDetachXactEngine = false;
		SetEvent(this->attachEvent);
		exp.AppendType("RestoreEngineFailed");
		GfkLogAudioExpOnce(exp);
		return 0;
	}
	SQR_TRY_END;

	return true;
}

//------------------------------------------------------------------------------
void CXACTAudioEngine::RestoreVolume()
{
	AMap<AString, float>::iterator beg = this->mapVariableVolumes.begin();
	AMap<AString, float>::iterator end = this->mapVariableVolumes.end();
	for ( beg; beg != end; ++beg )
	{
		AString variablename = beg->first;
		float  volumevalue  = beg->second; 

		this->SetGlobalVariable(variablename, volumevalue);
	}
}

//------------------------------------------------------------------------------
bool CXACTAudioEngine::LoadGlobalSettings()
{
	Ast(NULL == this->globalSettingsBuffer);

	CWinPkgFile	PkgFile;
	HANDLE hFile = PkgFile.Open(PATH_ALIAS_AUD.c_str(), this->globalSettingsPath.c_str());
	if (INVALID_HANDLE_VALUE == hFile)
	{
		if( !CAudioSystem::GetInst()->SoundFileIsExist(this->globalSettingsPath.c_str()) )
		{
			return false;
		}
		return false;
	}

	bool success = false;
	this->globalSettingsBufferSize = GetFileSize(hFile, NULL);
	if (INVALID_FILE_SIZE != this->globalSettingsBufferSize)
	{
		this->globalSettingsBuffer = CoTaskMemAlloc(this->globalSettingsBufferSize);
		if (NULL != this->globalSettingsBuffer)
		{
			DWORD bytesRead = 0;
			success = (0 != ReadFile(hFile, this->globalSettingsBuffer, this->globalSettingsBufferSize, &bytesRead, NULL));
		}
	}

	PkgFile.Close();

	if( !success )
	{
		if( this->globalSettingsBuffer )
			CoTaskMemFree(this->globalSettingsBuffer);
		this->globalSettingsBuffer = NULL;
		this->globalSettingsBufferSize = 0;
		GenErr("Read .xgs File Failed");
	}

	return success;
}

//------------------------------------------------------------------------------
void CXACTAudioEngine::WaitForWaveBankPrepared()
{
	while (WAIT_TIMEOUT == WaitForSingleObject(this->waveBankPreparedEvent, 10))
	{
		this->xactEngine->DoWork();
	}
}

//------------------------------------------------------------------------------
void WINAPI CXACTAudioEngine::NotificationCallback( const XACT_NOTIFICATION* notification )
{
	if (NULL == notification || NULL == notification->pvContext)
	{
		return;
	}
	if (XACTNOTIFICATIONTYPE_WAVEBANKPREPARED == notification->type)
	{
		CXACTAudioEngine* self = (CXACTAudioEngine*)notification->pvContext;
		SetEvent(self->waveBankPreparedEvent);
	}
}

//------------------------------------------------------------------------------
void
CXACTAudioEngine::SetGlobalVariable( const AString& name, float value )
{
	SQR_TRY
	{
		if( this->xactEngine )
		{
			XACTVARIABLEINDEX varIndex = this->xactEngine->GetGlobalVariableIndex(name.c_str());
			this->xactEngine->SetGlobalVariable(varIndex, value);
			this->mapVariableVolumes[name] = value;
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("SetGlobalVariable failed");
		GfkLogAudioExpOnce(exp);
	}
	SQR_TRY_END;
}

//------------------------------------------------------------------------------
float
CXACTAudioEngine::GetGlobalVariable( const AString& name )
{
	XACTVARIABLEVALUE value = 0;
	if( this->xactEngine )
	{
		XACTVARIABLEINDEX varIndex = this->xactEngine->GetGlobalVariableIndex(name.c_str());
		this->xactEngine->GetGlobalVariable(varIndex, &value);
	}

	else
		GfkLogAudioErrOnce("GetGlobalVariable failed");

	return value;
}

/*
win7 有2种情况需要考虑：
1. 游戏中，拔掉耳塞，再插入，没有声音，这种情况需要重新创建audio device，并load audio resource 需要实时检测什么时候
2. 游戏前，拔掉耳塞，游戏中插入，没有声音
*/
//------------------------------------------------------------------------------
bool
CXACTAudioEngine::EstimateCurAudioDeviceIsValid(const bool lastAudioDeviceValid)
{
	switch ( eWindowsType )
	{
	case WindowsXP:
		return true;

	case WindowsVista:
		{
			static int nLastCount = -1;
			int nCount = waveOutGetNumDevs();
			if( nCount == 1 )
			{
				nLastCount = nCount;
				isNeedUpdateThread = false;
				WAVEOUTCAPS woc;

				for (int i = 0; i < nCount; ++i)
				{
					MMRESULT hr = waveOutGetDevCaps(i, &woc, sizeof(WAVEOUTCAPS));
					if( hr == MMSYSERR_NOERROR )
					{
						AString strAudioDeviceName = woc.szPname;
						if( strAudioDeviceName.find("Digital") == -1 && strAudioDeviceName.find("SPDIF") == -1)
						{
							///游戏前抽掉耳塞，游戏中如果检测到插入耳塞，返回true
							if( lastAudioDeviceValid )
								isNeedUpdateThread = true;

							return true;
						}
						else
						{
							return false;
						}
					}
					else
						return false;
				}
			}
			else
			{
				int diff   = nLastCount - nCount;
				nLastCount = nCount;

				if( diff == 1 )
				{
					isNeedUpdateThread = false;
					return false;
				}

				return true;
			}

			///游戏前抽掉耳塞，游戏中如果一直没有插入耳塞，返回false
			isNeedUpdateThread = false;
			return false;
		}

	case Win7:
		{
			static int nLastCount = -1;
			int nCount = waveOutGetNumDevs();
			if( nCount == 1 )
			{
				nLastCount = nCount;
				isNeedUpdateThread = false;
				WAVEOUTCAPS woc;

				for (int i = 0; i < nCount; ++i)
				{
					MMRESULT hr = waveOutGetDevCaps(i, &woc, sizeof(WAVEOUTCAPS));
					if( hr == MMSYSERR_NOERROR )
					{				
						AString strAudioDeviceName = woc.szPname;
						if( strAudioDeviceName.find("S/PDIF") == -1 )
						{
							///游戏前抽掉耳塞，游戏中如果检测到插入耳塞，返回true
							if( lastAudioDeviceValid )
								isNeedUpdateThread = true;

							return true;
						}
						else
						{
							return false;
						}
					}
					else
						return false;
				}
			}
			else	
			{
				int diff   = nLastCount - nCount;
				nLastCount = nCount;

				if( diff == 1 )
				{
					isNeedUpdateThread = false;
					return false;
				}

				return true;
			}

			///游戏前抽掉耳塞，游戏中如果一直没有插入耳塞，返回false
			isNeedUpdateThread = false;
			return false;
		}


	default:
		return true;
	}
}

//------------------------------------------------------------------------------
void CXACTAudioEngine::ThreadProc( void* pSelf )
{
#ifdef _WIN32
	if(CatchErrorEnabled())
		_set_se_translator(TransESHFun);
#endif
	
	if( pSelf == NULL )
		GfkLogAudioErrOnce("pAudioEngineSelf == NULL");

	CXACTAudioEngine *engine = NULL;
	engine = (CXACTAudioEngine*)(pSelf);

	SQR_TRY
	{
		for(;;)
		{
			if (isDetachXactEngine)
			{
				SetEvent(engine->detachEvent);

				if( WAIT_OBJECT_0 == WaitForSingleObject(engine->attachEvent, INFINITE) )
				{
					ResetEvent(engine->detachEvent);
				}
			}
			else if(isCloseAudioEngine)
			{
				break;
			}
			else
			{
				if ( WAIT_TIMEOUT == WaitForSingleObject(engine->updateEvent, 100))
				{
					if( isNeedUpdateThread && engine->xactEngine )
					{
						HRESULT hr = engine->xactEngine->DoWork();
						if(hr)
						{
							string msg   = DXGetErrorDescription(hr);
							string error = DXGetErrorString(hr);

							msg = msg + "; " + error ;
							GfkLogAudioErrOnce("DoWork", msg.c_str());
						};
					}
				}
			}
		}
	}
	SQR_CATCH(exp)
	{
		string msg;
		msg = format("%d, %d, %d", isCloseAudioEngine, eWindowsType, isNeedUpdateThread);
		exp.AppendMsg(msg);
		GfkLogAudioExpOnce(exp);
	}
	SQR_TRY_END;
}

}// namespace sqr

