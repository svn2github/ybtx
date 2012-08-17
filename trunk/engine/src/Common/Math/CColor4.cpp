#include "stdafx.h"
#include "CColor4.h"
#include "CColor4.inl"
#include "TSqrAllocator.inl"

//------------------------------------------------------------------------------
namespace sqr
{

	CColor4 CColor4::Black( 0.f,0.f,0.f, 1.0f);
	CColor4 CColor4::Gray( 0.5f,0.5f,0.5f, 1.0f );
	CColor4 CColor4::White( 1.f,1.f,1.f, 1.0f );
	CColor4 CColor4::Red( 1.f,0.f,0.f, 1.0f );
	CColor4 CColor4::Green( 0.f,1.f,0.f, 1.0f );
	CColor4 CColor4::Blue( 0.f,0.f,1.f, 1.0f );
	CColor4 CColor4::Magenta( 1.f,0.f,1.f, 1.0f );
	CColor4 CColor4::Cyan( 0.f,1.f,1.f, 1.0f );
	CColor4 CColor4::Yellow( 1.f,1.f,0.f, 1.0f );

}// namespace sqr

