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

class CTimer
{
private:
	CTimer() : m_dwLastTime(0), m_iDeltaTime(0), m_dwThisTime(0), m_dwTimePerEF(1000/EFPS), m_dwFrameTime(0), m_bStop(true) { }
	~CTimer() { }
	static CTimer * one;

public:
	static CTimer * GetInstance() { if ( !one ) one = new CTimer; return one;} 

	void Start();
	bool IsOverEF();
	void PauseUpdate();
	bool IsStop(); 
	void Down();
	DWORD GetFrameTime();
	void Clear();
	void ResumeUpdate();

public:
	static DWORD GetSystemTime() { return ::timeGetTime(); }

private:
	DWORD	m_dwLastTime;
	int		m_iDeltaTime;
	DWORD	m_dwThisTime;
	DWORD	m_dwTimePerEF;
	DWORD	m_dwFrameTime;
	BOOL	m_bStop;

	enum
	{
		EFPS = 35
	};
};

}
}