#include "StdAfx.h"
#include "CAudioListener.h"
#include "CAudioEngine.h"
#include "TSqrAllocator.inl"

namespace sqr
{
//------------------------------------------------------------------------------
CAudioListener::CAudioListener()
{
}

//------------------------------------------------------------------------------
CAudioListener::~CAudioListener()
{
}

//------------------------------------------------------------------------------
void
CAudioListener::SetTransform( const CVector3f& orientFront, const CVector3f& orientTop, const CVector3f& position )
{
	CAudioEngine* audioEngine = GetAudioEngine();
	audioEngine->SetListenerOrientFront(orientFront);
	audioEngine->SetListenerOrientTop(orientTop);
	audioEngine->SetListenerPosition(position);
}

//------------------------------------------------------------------------------
void CAudioListener::SetVelocity( const CVector3f& velocity )
{
	GetAudioEngine()->SetListenerVelocity(velocity);
}

//------------------------------------------------------------------------------
const CVector3f& CAudioListener::GetVelocity() const
{
	return GetAudioEngine()->GetListenerVelocity();
}

//------------------------------------------------------------------------------
const CVector3f&
CAudioListener::GetPosition() const
{
	return GetAudioEngine()->GetListenerPostion();
}

//------------------------------------------------------------------------------
void
CAudioListener::SetPosition( const CVector3f& pos )
{
	GetAudioEngine()->SetListenerPosition(pos);
}

//------------------------------------------------------------------------------
const CVector3f&
CAudioListener::GetOrientTop() const
{
	return GetAudioEngine()->GetListenerOrientFront();
}

//------------------------------------------------------------------------------
void
CAudioListener::SetOrientTop( const CVector3f& top )
{
	GetAudioEngine()->SetListenerOrientTop(top);
}

//------------------------------------------------------------------------------
const CVector3f&
CAudioListener::GetOrientFront() const
{
	return GetAudioEngine()->GetListenerOrientFront();
}

//------------------------------------------------------------------------------
void
CAudioListener::SetOrientFront( const CVector3f& front )
{
	GetAudioEngine()->SetListenerOrientFront(front);
}
}// namespace sqr
