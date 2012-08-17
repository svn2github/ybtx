#include "StdAfx.h"
#include "CAudioEmitter.h"
#include "CAudioEngine.h"
#include "ErrLogHelper.h"
#include "CGraphicExp.h"
#include "StringHelper.h"
#include "TSqrAllocator.inl"

namespace sqr
{

//------------------------------------------------------------------------------
CAudioEmitter::CAudioEmitter()
: refCount(0)
, isAutoStop(true)
, isValid(true)
{
	this->position.Init();
	this->velocity.Init();

	SQR_TRY	
	{
		WindowsVersion eWindowsType = GetAudioEngine()->eWindowsType;
		switch (eWindowsType)
		{
		case WindowsVista:
		case Win7:
			GetAudioEngine()->InsertAudioEmitter(this);
			break;
		default:
			break;
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("CAudioEmitter Failed");
		exp.AppendMsg(strCueName.c_str());
		GfkLogAudioExpOnce(exp);
	}
	SQR_TRY_END;
}

//------------------------------------------------------------------------------
CAudioEmitter::~CAudioEmitter()
{
	SQR_TRY	
	{
		WindowsVersion eWindowsType = GetAudioEngine()->eWindowsType;
		switch (eWindowsType)
		{
		case WindowsVista:
		case Win7:
			GetAudioEngine()->DeleteAudioEmitter(this);
			break;
		default:
			break;
		}

		Ast(0 == this->refCount);
		if (this->IsAutoStop() && this->IsPlaying())
		{
			this->Stop();
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("~CAudioEmitter Failed");
		exp.AppendMsg(strCueName.c_str());
		GfkLogAudioExpOnce(exp);
	}
	SQR_TRY_END;
}

//------------------------------------------------------------------------------
bool CAudioEmitter::Play( const char* cueName )
{
	SQR_TRY	
	{
		if (NULL == cueName || 0 == cueName[0])
			return false;

		Ast(GetAudioEngine());
		if (GetAudioEngine()->IsAudioDeviceValid())
		{
			this->strCueName = cueName;
			this->cue = GetAudioEngine()->PlayCue(cueName, true, true, this->position, this->velocity);
			// update variables
			if (!this->cue.isvalid())
			{
				//GfkLogErr("audioEmitter::cue=null", cueName.c_str());
				return false;
			}
			
			for (AMap<AString, float>::iterator it = this->variableMap.begin();
				it != this->variableMap.end(); ++it)
			{
				this->cue->SetVariable(it->first, it->second);
			}
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("EmitterPlay");
		exp.AppendMsg(cueName);
		GfkLogAudioExpOnce(exp);

		return false;
	}
	SQR_TRY_END;

	return true;
}

//------------------------------------------------------------------------------
bool CAudioEmitter::Stop()
{
	if (GetAudioEngine()->IsAudioDeviceValid())
	{
		SQR_TRY	
		{
			if ( this->cue.isvalid() )
			{
				SoundState::Mask curStateMask = this->cue->GetState();
				if( curStateMask == SoundState::Invalid )
					return false;
				else
				{
					if( 0 != (SoundState::Playing & curStateMask) )
					{
						bool bStop = this->cue->Stop();
						return bStop;
					}
				}
			}
		}
		SQR_CATCH(exp)
		{
			exp.AppendType("EmitterStop");
			GfkLogAudioExpOnce(exp);
		}
		SQR_TRY_END;
	}

	return false;
}

//------------------------------------------------------------------------------
bool CAudioEmitter::IsPlaying() const
{ 
	if (GetAudioEngine()->IsAudioDeviceValid())
	{
		SQR_TRY	
		{
			if (this->cue.isvalid())
				return this->cue->IsPlaying();
			else
				return false;
		}
		SQR_CATCH(exp)
		{
			exp.AppendType("EmitterIsPlaying");
			GfkLogAudioExpOnce(exp);
			return true;
		}
		SQR_TRY_END;
	}

	return false;
}

//------------------------------------------------------------------------------
bool CAudioEmitter::IsStopped() const
{
	if (GetAudioEngine()->IsAudioDeviceValid())
	{
		SQR_TRY	
		{
			if (this->cue.isvalid())
			{
				return this->cue->IsStopped();
			}
		}
		SQR_CATCH(exp)
		{
			exp.AppendType("EmitterIsStopped");
			GfkLogAudioExpOnce(exp);
			return true;
		}
		SQR_TRY_END;
	}

	return true;
}

//------------------------------------------------------------------------------
void CAudioEmitter::SetVelocity( const CVector3f& v )
{
	if (GetAudioEngine()->IsAudioDeviceValid())
	{
		this->velocity = v;
		if (this->cue.isvalid())
		{
			this->cue->SetVelocity(this->velocity);
		}
	}
}

//------------------------------------------------------------------------------
void
CAudioEmitter::SetPosition( const CVector3f& pos )
{
	if (GetAudioEngine()->IsAudioDeviceValid())
	{
		this->position = pos;
		if (this->cue.isvalid())
		{
			this->cue->SetPosition(this->position);
		}
	}
}

//------------------------------------------------------------------------------
void
CAudioEmitter::SetVariable( const char* name, float value )
{
	if (GetAudioEngine()->IsAudioDeviceValid())
	{
		this->variableMap[name] = value;
		if (this->IsPlaying())
		{
			this->cue->SetVariable(name, value);
		}
	}
}

//------------------------------------------------------------------------------
void
CAudioEmitter::SetAutoStop( bool b )
{
	this->isAutoStop = b;
}

//------------------------------------------------------------------------------
bool
CAudioEmitter::IsAutoStop() const
{
	return this->isAutoStop || !this->variableMap.empty();
}

void CAudioEmitter::Noticfy()
{
	SQR_TRY	
	{
		if ( this->cue.isvalid() )
		{
			this->isValid = false;
			
			this->cue->Destroy();
			this->cue = NULL;
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendMsg("Noticfy Failed");
		GfkLogExpOnce(exp);
	}
	SQR_TRY_END;
}
}// namespace sqr


void CAudioEmitter::InValid()
{

};