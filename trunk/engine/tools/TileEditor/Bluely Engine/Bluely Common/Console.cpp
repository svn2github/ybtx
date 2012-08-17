//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//	All Rights Reserved	
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#include "Console.h"
#include "exception.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace bc
{
namespace ut
{

CConsole * CConsole::one = NULL;

VOID CConsole::Free()
{
	FreeConsole();
}

VOID CConsole::Init()
{
	m_uiIndent = 0;
	m_hOutput = NULL;

	// create the console
	if ( !AllocConsole() )
	{
		throw BLUELY_EXCEPTION("CConsole::Create", "AllocConsole failed");
	}
	
	// get the handle to std out
	m_hOutput = GetStdHandle(STD_OUTPUT_HANDLE);

	if ( INVALID_HANDLE_VALUE == m_hOutput )
	{
		throw BLUELY_EXCEPTION( "CConsole::Create()", "GetStdHandle() - Output handle - failed" );
	}

	// console title
	SetConsoleTitle("Bluely Logging Console(BLC) - Ver(0.1)");
}

// output operations
CConsole & CConsole::operator <<(LPCSTR lpszVal)
{
	Write(lpszVal);
	return (*this);
}

CConsole & CConsole::operator <<(bool bVal)
{
	if ( bVal )
	{
		Write("true");
	}
	else
	{
		Write("false");
	}
	return (*this);
}

CConsole & CConsole::operator <<(INT iVal)
{
	sprintf(m_szBuf, "%d", iVal);
	Write(m_szBuf);
	return (*this);
}

CConsole & CConsole::operator <<(UINT uiVal)
{
	sprintf(m_szBuf, "%d", uiVal);
	Write(m_szBuf);
	return (*this);
}

CConsole & CConsole::operator <<(DWORD dwVal)
{
	sprintf(m_szBuf, "%ld", dwVal);
	Write(m_szBuf);
	return (*this);
}

CConsole & CConsole::operator <<(FLOAT fVal)
{
	sprintf(m_szBuf, "%f", fVal);
	Write(m_szBuf);
	return (*this);
}

CConsole & CConsole::operator <<(EConsoleManipulator Manip)
{
	switch ( Manip )
	{
	case ECM_ENDL:
		// move to the beginning of a new line, then indent
		// y Displacement : 1
		// x Displacement : m_wIndent ( from the beginning of the line )
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(m_hOutput, &csbi);
		COORD CursorPos;
		CursorPos.X = m_uiIndent;
		CursorPos.Y = csbi.dwCursorPosition.Y + 1;
		SetConsoleCursorPosition(m_hOutput, CursorPos);
		break;
		
	case ECM_TAB:
		// tab is 4-characters wide
		Write("    ");
	break;
		
	case ECM_INDENT:
		Indent();
	break;
		
	case ECM_UNINDENT:
		UnIndent();
	break;
		
	default: Write("Unhandled manipulator passed");
	}

	return (*this);
}

}
}