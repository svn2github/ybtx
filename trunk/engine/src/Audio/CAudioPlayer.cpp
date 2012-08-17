#include "StdAfx.h"
#include "CAudioPlayer.h"
#include "CAudioEngine.h"
#include "IAudioModule.h"
#include "ErrlogHelper.h"
#include "CGraphicExp.h"
#include "TSqrAllocator.inl"

namespace sqr
{
//------------------------------------------------------------------------------
CAudioPlayer::CAudioPlayer()
{
}

//------------------------------------------------------------------------------
CAudioPlayer::~CAudioPlayer()
{
}

//------------------------------------------------------------------------------
inline void CAudioPlayer::PlayCue( const char* cueName )
{
	static const CVector3f empty;
 	if (GetAudioEngine()->IsAudioDeviceValid())
		GetAudioEngine()->PlayGloablCue(cueName, true, false, empty, empty);
}

//------------------------------------------------------------------------------
inline void CAudioPlayer::Play3DCue( const char* cueName, const CVector3f& position, const CVector3f& velocity )
{
	if (GetAudioEngine()->IsAudioDeviceValid())
		GetAudioEngine()->PlayGloablCue(cueName, true, true, position, velocity);
}

//------------------------------------------------------------------------------
inline void CAudioPlayer::StopCue( const char* cueName )
{
	SQR_TRY	
	{
		if (GetAudioEngine()->IsAudioDeviceValid())
			GetAudioEngine()->StopCue(cueName, false);
	}
	SQR_CATCH(exp)
	{
		GfkLogExp(exp);
	}
	SQR_TRY_END;
}

//------------------------------------------------------------------------------
bool CAudioPlayer::CueIsStop(const char* cueName)
{
	if (GetAudioEngine()->IsAudioDeviceValid())
		return GetAudioEngine()->CueIsStop(cueName);

	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
AUDIO_API void PlayCue(const char* cueName)
{
#ifndef _DEBUG
	if (NULL == cueName || 0 == cueName[0])
		return;
#endif
	SQR_TRY	
	{
		Ast(CAudioPlayer::HasInst());
		CAudioPlayer::GetInst()->PlayCue(cueName);
	}
	SQR_CATCH(exp)
	{
		GfkLogExpOnce(exp);
	}
	SQR_TRY_END;
}
//------------------------------------------------------------------------------
AUDIO_API void Play3DCue(const char* cueName, const CVector3f& position, const CVector3f& velocity)
{
#ifndef _DEBUG
	if (NULL == cueName || 0 == cueName[0])
		return;
#endif
	SQR_TRY	
	{
		Ast(CAudioPlayer::HasInst());
		CAudioPlayer::GetInst()->Play3DCue(cueName, position, velocity);
	}
	SQR_CATCH(exp)
	{
		GfkLogExpOnce(exp);
	}
	SQR_TRY_END;
}
//------------------------------------------------------------------------------
AUDIO_API void StopCue(const char* cueName)
{
#ifndef _DEBUG
	if (NULL == cueName || 0 == cueName[0])
		return;
#endif
	CAudioPlayer::GetInst()->StopCue(cueName);
}
//------------------------------------------------------------------------------
AUDIO_API bool CueIsStop(const char* cueName)
{
#ifndef _DEBUG
	if (NULL == cueName || 0 == cueName[0])
		return false;
#endif
	
	return CAudioPlayer::GetInst()->CueIsStop(cueName);
}
}// namespace sqr
