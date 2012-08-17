#pragma once

#include "Console.h"

namespace sqr
{
	class CConsole
	{
	private:
		friend bool sqr::ShowConsole(bool bShow);
		
		HANDLE	m_hConsoleScreen;

		HANDLE	m_hConsoleInput;
		HANDLE	m_hConsoleOutput;
		HANDLE	m_hConsoleError;

		FILE	m_fdOldStdIn;
		FILE	m_fdOldStdOut;
		FILE	m_fdOldStdErr;

		bool Init();

		CConsole(void);
		~CConsole(void);
	};
}