#include "stdafx.h"
#include "Console.h"

namespace sqr
{
	VOID MECConsole::Free()
	{
		FreeConsole();
	}

	VOID MECConsole::Init()
	{
		m_uiIndent = 0;
		m_hOutput = NULL;

		// create the console
		AllocConsole();


		// get the handle to std out
		m_hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	}

	// output operations
	MECConsole & MECConsole::operator <<(LPCSTR lpszVal)
	{
		Write(lpszVal);
		return (*this);
	}

	MECConsole & MECConsole::operator <<(bool bVal)
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

	MECConsole & MECConsole::operator <<(INT iVal)
	{
		sprintf(m_szBuf, "%d", iVal);
		Write(m_szBuf);
		return (*this);
	}

	MECConsole & MECConsole::operator <<(UINT uiVal)
	{
		sprintf(m_szBuf, "%d", uiVal);
		Write(m_szBuf);
		return (*this);
	}

	MECConsole & MECConsole::operator <<(DWORD dwVal)
	{
		sprintf(m_szBuf, "%ld", dwVal);
		Write(m_szBuf);
		return (*this);
	}

	MECConsole & MECConsole::operator <<(FLOAT fVal)
	{
		sprintf(m_szBuf, "%.8f", fVal);
		Write(m_szBuf);
		return (*this);
	}

	MECConsole & MECConsole::operator <<(EConsoleManipulator Manip)
	{
		switch ( Manip )
		{
			case ECM_CARRIAGE_RETURN:
			{
				// move to the beginning of a current line

				CONSOLE_SCREEN_BUFFER_INFO csbi;
				GetConsoleScreenBufferInfo(m_hOutput, &csbi);
				COORD CursorPos;
				CursorPos.X = 0;
				CursorPos.Y = csbi.dwCursorPosition.Y;
				SetConsoleCursorPosition(m_hOutput, CursorPos);
			}
			break;

			case ECM_ENDL:
			{
				// move to the beginning of a new line, then indent
				// y Displacement : 1
				// x Displacement : m_wIndent ( from the beginning of the line )
				CONSOLE_SCREEN_BUFFER_INFO csbi;
				GetConsoleScreenBufferInfo(m_hOutput, &csbi);
				COORD CursorPos;
				CursorPos.X = m_uiIndent;
				CursorPos.Y = csbi.dwCursorPosition.Y + 1;
				SetConsoleCursorPosition(m_hOutput, CursorPos);
			}
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

			default:
				Write("Unhandled manipulator passed");
		}

		return (*this);
	}
}