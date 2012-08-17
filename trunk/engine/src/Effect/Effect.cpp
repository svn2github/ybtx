// EffectDll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "CEffectClassDesc.h"

std::string LoadResString( WORD ID )
{
	char Buf[1024];
	LoadString( GetModuleHandle(NULL), ID, Buf, 1024 );
	return Buf;
}

// This is an example of an exported function.
IEffectClassDesc* CreateEffectClassDesc(void)
{
	return new CEffectClassDesc();
}