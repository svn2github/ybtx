
#pragma once
#include "CVector3.h"


namespace sqr
{

struct RENDERSTATE;

class IEffectUnitEx
{
public:
	virtual void				Release( void )		= 0;
	virtual RENDERSTATE*	OnRender( void )	= 0;

	virtual void				SetPos( CVector3f pos )		= 0;
	virtual void				SetSize( float fSize )		= 0;
	virtual void				SetColor( uint32 color )	= 0;
};

}