#include "StdAfx.h"
#include "XACTCue.h"
#include "XACTSoundBank.h"
#include "XACTAudioEngine.h"
#include "ErrLogHelper.h"
#include "StringHelper.h"
#include "Dxerr.h"
#include "CGraphicExp.h"
#include "TSqrAllocator.inl"

namespace sqr
{

const float CURVE_DISTANCE_SCALER = 64.0f;

//------------------------------------------------------------------------------
CXACTCue::CXACTCue()
: xactCue(NULL)
{
	// empty
}

//------------------------------------------------------------------------------
CXACTCue::~CXACTCue()
{
	this->Destroy();
}

//------------------------------------------------------------------------------
void CXACTCue::Setup( const CSoundBankPtr& soundBank, const AString& strCueName, index_t cueIndex, bool startPlayback, bool enable3D )
{
	CCue::Init(strCueName, cueIndex, startPlayback, enable3D);

	if( !soundBank.isvalid() )
	{
		GfkLogAudioErrOnce("soundBank is null", strCueName.c_str());
		return;
	}

	IXACT3SoundBank* xactSoundBank = NULL;
	SQR_TRY
	{
		xactSoundBank = (soundBank.get())->GetXACTSoundBank();
		if( xactSoundBank == NULL )
		{
			GfkLogAudioErrOnce("Null xactSoundBank", strCueName.c_str());
			return;
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendMsg(strCueName.c_str());
		exp.AppendType("soundBank.get()");
		GfkLogAudioExpOnce(exp);
		return;
	}
	SQR_TRY_END;

	HRESULT hr;
	if (startPlayback)
	{
		SQR_TRY
		{
			hr = xactSoundBank->Play(cueIndex, 0, 0, &this->xactCue);
			if(FAILED(hr))
			{
				string msg   = DXGetErrorDescription(hr);
				string error = DXGetErrorString(hr);
				GetAudioEngine()->SetAudioDeviceIsValid(false);

				msg = msg + "; " + error + "; " + strCueName.c_str();
				GfkLogAudioErrOnce("SoundBank Play", msg.c_str());
				return;
			}
		}
		SQR_CATCH(exp)
		{
			string msg;
			bool mark = CXACTAudioEngine::GetInst()->isDestroyResourceInGame;
			msg = format("%s, %d, %d, %d", strCueName.c_str(), cueIndex, mark, GetAudioEngine()->eWindowsType);
			exp.AppendMsg(msg);
			exp.AppendType("Play");

			GfkLogAudioExpOnce(exp);

			this->xactCue = NULL;
		}
		SQR_TRY_END;
		
	}
	else
	{
		SQR_TRY
		{

			hr = xactSoundBank->Prepare(cueIndex, 0, 0, &this->xactCue);
			if(FAILED(hr))
			{
				string msg = format("%s, %d", strCueName.c_str(), cueIndex);
				GfkLogAudioErrOnce("SoundBank Prepare", msg.c_str());
				return;
			}
		}
		SQR_CATCH(exp)
		{
			exp.AppendType("Prepare");
			GfkLogAudioExpOnce(exp);
		}
		SQR_TRY_END;
	}
}

//------------------------------------------------------------------------------
void CXACTCue::Destroy()
{
	SQR_TRY
	{
		if(NULL != this->xactCue)
		{
			IXACT3Cue* ptemp = this->xactCue;
			this->xactCue = NULL;
			HRESULT hr = ptemp->Destroy();
			if(FAILED(hr))
			{
				AString dxmsg = DXGetErrorDescription(hr);
				string msg = format("%s, %s", this->strCueName.c_str(), dxmsg.c_str());
				GfkLogAudioErrOnce("xactCue destroy failed", msg.c_str()); 
			}
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("XACTCueDestroy");
		exp.AppendMsg(this->strCueName.c_str());
		GfkLogAudioExpOnce(exp);
	}
	SQR_TRY_END;
}

//------------------------------------------------------------------------------
void CXACTCue::Play()
{
	if(NULL != this->xactCue)
	{
		HRESULT hr = this->xactCue->Play();
		if(FAILED(hr))
		{
			string msg = DXGetErrorDescription(hr);
			GfkLogAudioErrOnce("xactCue play failed", msg.c_str()); 
		}
	}
}

//------------------------------------------------------------------------------
bool CXACTCue::Stop()
{
	SQR_TRY
	{
		if(NULL != this->xactCue)
		{
			HRESULT hr = this->xactCue->Stop(XACT_FLAG_CUE_STOP_RELEASE);
			if(FAILED(hr))
			{
				string msg = DXGetErrorDescription(hr);
				GfkLogAudioErrOnce("xactCue stop failed", msg.c_str()); 
				return false;
			}
		}
	}
	SQR_CATCH(exp)
	{
		string msg;
		msg = format("%s, %d", this->strCueName.c_str(), GetAudioEngine()->eWindowsType);
		exp.AppendMsg(msg);
		exp.AppendType("XACTCueStop");
		GfkLogAudioExpOnce(exp);
		return false;
	}
	SQR_TRY_END;

	return true;
}

//------------------------------------------------------------------------------
void CXACTCue::StopImmediate()
{
	if(NULL != this->xactCue)
	{
		HRESULT hr = this->xactCue->Stop(XACT_FLAG_CUE_STOP_IMMEDIATE);
		if(FAILED(hr))
		{
			string msg = DXGetErrorDescription(hr);
			GfkLogAudioErrOnce("xactCue stopImmediate failed", msg.c_str()); 
		};
	}
}

//------------------------------------------------------------------------------
void CXACTCue::Pause()
{
	if(NULL != this->xactCue)
	{
		HRESULT hr = this->xactCue->Pause(TRUE);
		if(FAILED(hr))
		{
			string msg = DXGetErrorDescription(hr);
			GfkLogAudioErrOnce("xactCue pause failed", msg.c_str()); 
		}
	}
}

//------------------------------------------------------------------------------
void CXACTCue::Resume()
{
	if(NULL != this->xactCue)
	{
		HRESULT hr = this->xactCue->Pause(FALSE);
		if(FAILED(hr))
		{
			string msg = DXGetErrorDescription(hr);
			GfkLogAudioErrOnce("xactCue resume failed", msg.c_str()); 
		}
	}
}

//------------------------------------------------------------------------------
SoundState::Mask CXACTCue::GetState() const
{
	DWORD state = 0;

	SQR_TRY
	{
		if (NULL != this->xactCue)
		{
			HRESULT hr = this->xactCue->GetState(&state);
			if(FAILED(hr))
			{
				string msg = DXGetErrorDescription(hr);
				GfkLogAudioErrOnce("xactCue getState failed", msg.c_str()); 
			}
		}
		else
			return SoundState::Invalid;
	}
	SQR_CATCH(exp)
	{
		string msg;
		msg = format("%s, %d", this->strCueName.c_str(), GetAudioEngine()->eWindowsType);
		exp.AppendMsg(msg);
		exp.AppendType("GetState");
		GfkLogAudioExpOnce(exp);
		return SoundState::Invalid;
	}
	SQR_TRY_END;

	return state;
}

//------------------------------------------------------------------------------
void CXACTCue::Update()
{
	UINT Flag = 0;

	SQR_TRY
	{
		CXACTAudioEngine* audioEngine = CXACTAudioEngine::GetInst();
		if (!this->is3DEnabled || !(this->IsPlaying() || this->IsPreparing()) || !audioEngine->isSupport3D || NULL == this->xactCue)
		{
			return;
		}

		Flag = 1;

		// listener
		X3DAUDIO_LISTENER x3dListener = { 0 };
		x3dListener.pCone = NULL;
		x3dListener.OrientFront = (X3DAUDIO_VECTOR&)audioEngine->GetListenerOrientFront();
		x3dListener.OrientTop = (X3DAUDIO_VECTOR&)audioEngine->GetListenerOrientTop();
		x3dListener.Position = (X3DAUDIO_VECTOR&)audioEngine->GetListenerPostion();
		x3dListener.Velocity = (X3DAUDIO_VECTOR&)audioEngine->GetListenerVelocity();
		Flag = 2;

		// emitter
		X3DAUDIO_EMITTER x3dEmitter = { 0 };
		x3dEmitter.pCone = NULL;
		// the following need to be orthonormal
		x3dEmitter.OrientFront = (X3DAUDIO_VECTOR&)CVector3f(0.0f, 0.0f, 1.0f);
		x3dEmitter.OrientTop = (X3DAUDIO_VECTOR&)CVector3f(0.0f, 1.0f, 0.0f);
		
		x3dEmitter.Position = (X3DAUDIO_VECTOR&)this->position;
		x3dEmitter.Velocity = (X3DAUDIO_VECTOR&)this->velocity;
		// 3d parameters
		x3dEmitter.InnerRadius = CURVE_DISTANCE_SCALER * 5.0f;
		x3dEmitter.InnerRadiusAngle = X3DAUDIO_PI * 0.25f;
		
		x3dEmitter.ChannelCount = 1;	 // emitter ChannelCount and DSP Setting's SrcChannelCount must match
		x3dEmitter.ChannelRadius = 0.0f; // this will be set by XACT3DCalculate if ChannelCount > 1.
		x3dEmitter.pChannelAzimuths = NULL;

		// will be set by XACT3DCalculate
		x3dEmitter.pVolumeCurve = x3dEmitter.pLFECurve
			= x3dEmitter.pLPFDirectCurve
			= x3dEmitter.pLPFReverbCurve
			= x3dEmitter.pReverbCurve
			= NULL;

		x3dEmitter.CurveDistanceScaler = CURVE_DISTANCE_SCALER;
		x3dEmitter.DopplerScaler = 1.0f;

		Flag = 3;

		// calculation
		HRESULT hr = XACT3DCalculate(audioEngine->x3dAudioHandle, 
			&x3dListener, &x3dEmitter, &audioEngine->dspSettings);
		
		if(FAILED(hr))
		{
			string msg = DXGetErrorDescription(hr);
			GfkLogAudioErrOnce("XACT3DCalculate failed", msg.c_str()); 
			return;
		}

		Flag = 4;

		// apply
		DWORD state;
		this->xactCue->GetState(&state);
		Flag = 5;
		hr = XACT3DApply(&audioEngine->dspSettings, this->xactCue);
		Flag = 6;
		if(FAILED(hr))
		{
			string msg = DXGetErrorDescription(hr);
			GfkLogAudioErrOnce("XACT3DApply failed", msg.c_str()); 
		}
	}
	SQR_CATCH(exp)
	{
		string log = this->strCueName.c_str();
		exp.AppendMsg(log);

		switch(Flag)
		{
		case 0:
			exp.AppendType("_Cue:IsOpen()");
			break;
		case 1:
			exp.AppendType("_Cue:listener");
			break;
		case 2:
			exp.AppendType("_Cue:emitter");
			break;
		case 3:		
			exp.AppendType("_Cue:calculate");
			break;
		case 4:		
			exp.AppendType("_Cue:getstate");
			break;
		case 5:		
			exp.AppendType("_Cue:apply");
			break;
		case 6:		
			exp.AppendType("_Cue:FAILED(hr)");
			break;
		default:
			exp.AppendType("_Cue:other");
		}
		
		GfkLogAudioExpOnce(exp);
	}
	SQR_TRY_END;
}

//------------------------------------------------------------------------------
void
CXACTCue::SetVariable( const AString& name, float value )
{
	if (NULL != this->xactCue)
	{
		XACTVARIABLEINDEX varIndex = this->xactCue->GetVariableIndex(name.c_str());
		this->xactCue->SetVariable(varIndex, value);
	}
}

//------------------------------------------------------------------------------
float
CXACTCue::GetVariable( const AString& name )
{
	if (NULL != this->xactCue)
	{
		XACTVARIABLEINDEX varIndex = this->xactCue->GetVariableIndex(name.c_str());
		float value = 0;
		this->xactCue->GetVariable(varIndex, &value);
		return value;
	}
	else
		return 0.0f;
}

}// namespace sqr
