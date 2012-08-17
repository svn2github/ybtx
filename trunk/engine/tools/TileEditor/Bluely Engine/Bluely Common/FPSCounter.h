//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "BluelyC.h"

namespace bc{
namespace ut{

class CFPSCounter
{
private:
	CFPSCounter() : m_fps(0),m_dwStartTime(0),m_dwFrameCnt(0) { }
	~CFPSCounter() { }
	static CFPSCounter * one;

public:
	static CFPSCounter * GetInstance() { if ( !one ) one = new CFPSCounter; return one;}

	DWORD	m_fps;
	DWORD   m_dwStartTime;
	DWORD   m_dwFrameCnt;

public:
	DWORD GetFPS();
	void RegisterFPS();
};

}
}
