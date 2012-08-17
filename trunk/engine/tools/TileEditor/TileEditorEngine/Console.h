//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#ifndef __BLUELY_COMMON_CONSOLE__
#define __BLUELY_COMMON_CONSOLE__

#include "BluelyC.h"
#include "exception.h"

#define LogConsole(msg) bc::ut::CConsole::GetInstance() \
		<< msg << bc::ut::ECM_ENDL;

namespace bc
{
namespace ut
{

//  Logging Console manipulators
enum EConsoleManipulator
{
	ECM_ENDL,
	ECM_TAB,
	ECM_INDENT,
	ECM_UNINDENT
};

class CConsole
{
public:
	static CConsole & GetInstance() { if ( !one ) one = new CConsole; return * one;} 

private:
	CConsole()
	{
		Init();
	}

	~CConsole()
	{
		Free();
	}
	
	static CConsole * one;

public:
	VOID Indent()
	{
		// Keep track of the indentation
		m_uiIndent += 4;
		
		// Update coordinates of cursor
		COORD Delta;
		Delta.X = 4;
		Delta.Y = 0;
		IncCursorPos(Delta);
	}

	VOID UnIndent()
	{
		// Keep track of the indentation
		m_uiIndent -= 4;
		
		// Update coordinates of cursor
		COORD Delta;
		Delta.X = -4;
		Delta.Y = 0;
		IncCursorPos(Delta);
	}
	
	// Output
	CConsole& operator <<(LPCSTR pszVal);
	CConsole& operator <<(bool  bVal);
	CConsole& operator <<(INT   iVal);
	CConsole& operator <<(UINT  uiVal);
	CConsole& operator <<(DWORD dwVal);
	CConsole& operator <<(FLOAT fVal);
	CConsole& operator <<(EConsoleManipulator Manip);
	
private:
	VOID Init();
	VOID Free();
	
	// Moves the cursor position with the given 'Delta'
	VOID IncCursorPos(COORD Delta)
	{
		// Get current cursor position.
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(m_hOutput, &csbi);
		// Add movement
		Delta.X += csbi.dwCursorPosition.X;
		Delta.Y += csbi.dwCursorPosition.Y;
		// Update cursor position
		SetConsoleCursorPosition(m_hOutput, Delta);
	}
	
	// Writes the given zero-terminated text buffer to the console
	VOID Write(LPCSTR pszText)
	{
		DWORD dwNumCharsWritten;
		if ( !WriteConsole(m_hOutput, pszText, static_cast<DWORD>(strlen(pszText)), &dwNumCharsWritten, NULL) )
		{
			throw BLUELY_EXCEPTION("CConsole::Write", "WriteConsole failed");
		}
	}
	
	// Handle to std out
	HANDLE m_hOutput;
	// Internal buffer for type-to-string conversion
	CHAR   m_szBuf[512];
	// Internal tracking of indentation
	UINT   m_uiIndent;
};

}
}

#endif