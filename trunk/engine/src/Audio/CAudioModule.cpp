#include "StdAfx.h"
#include "CAudioModule.h"
#include "CAudioSystem.h"
#include "CAudioPlayer.h"
#include "CAudioListener.h"
#include "CAudioEmitter.h"
#include "TSqrAllocator.inl"
#include "CGraphicExp.h"

namespace sqr
{
//------------------------------------------------------------------------------
CAudioModule::CAudioModule()
{
}

//------------------------------------------------------------------------------
CAudioModule::~CAudioModule()
{
}

//------------------------------------------------------------------------------
IAudioSystem* CAudioModule::GetAudioSystem()
{
	if( CAudioSystem::HasInst() )
		return CAudioSystem::GetInst();
	else
		return NULL;
}

//------------------------------------------------------------------------------
IAudioPlayer* CAudioModule::GetAudioPlayer()
{
	return CAudioPlayer::GetInst();
}

//------------------------------------------------------------------------------
IAudioListener* CAudioModule::GetAudioListener()
{
	return CAudioListener::GetInst();
}

//------------------------------------------------------------------------------
IAudioEmitterPtr CAudioModule::CreateAudioEmitter()
{
	IAudioEmitter* emitter  = new CAudioEmitter();
	return IAudioEmitterPtr(emitter);
}

bool CAudioModule::HasAudioSystem()
{
	return CAudioSystem::HasInst();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
AUDIO_API IAudioModule* GetAudioModule()
{
	SQR_TRY
	{
		if (!CAudioModule::HasInst())
		{
			CAudioModule::Create();
			new CAudioSystem;
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("GetAudioModule");
		GfkLogAudioExpOnce(exp);
		return NULL;
	}
	SQR_TRY_END;

	return CAudioModule::GetInst();
}

}// namespace sqr
