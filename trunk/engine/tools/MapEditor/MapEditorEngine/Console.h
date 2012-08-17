#pragma once
#include "TSingleton.h"

#define LogConsole(msg) *sqr::MECConsole::GetInst() \
	<< msg << sqr::ECM_ENDL;

#define LogConsoleCR(msg) *sqr::MECConsole::GetInst() \
	<< msg << sqr::ECM_CARRIAGE_RETURN;

// Console控制台打印信息
namespace sqr
{
	//  Logging Console manipulators
	enum EConsoleManipulator
	{
		ECM_ENDL,
		ECM_TAB,
		ECM_INDENT,
		ECM_UNINDENT,
		ECM_CARRIAGE_RETURN
	};

	class MECConsole : public TSingleton<MECConsole>
	{
	private:
		friend class TSingleton<MECConsole>;
		MECConsole()
		{
			Init();
		}

		~MECConsole()
		{
			Free();
		}

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
		MECConsole& operator <<(LPCSTR pszVal);
		MECConsole& operator <<(bool  bVal);
		MECConsole& operator <<(INT   iVal);
		MECConsole& operator <<(UINT  uiVal);
		MECConsole& operator <<(DWORD dwVal);
		MECConsole& operator <<(FLOAT fVal);
		MECConsole& operator <<(EConsoleManipulator Manip);

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
			WriteConsole(m_hOutput, pszText, static_cast<DWORD>(strlen(pszText)), &dwNumCharsWritten, NULL);
		}

		// Handle to std out
		HANDLE m_hOutput;
		// Internal buffer for type-to-string conversion
		CHAR   m_szBuf[512];
		// Internal tracking of indentation
		UINT   m_uiIndent;
	};
}