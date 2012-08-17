//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#ifndef __BLUELY_COMMON_EXCEPTION__
#define __BLUELY_COMMON_EXCEPTION__

#include "BluelyC.h"
#include <cstring>

#define BLUELY_EXCEPTION(pszFunction, pszMsg) bc::ut::CException(pszFunction, pszMsg, __FILE__, __LINE__)

namespace bc
{

namespace ut
{


class CException
{
public:
	CException(LPCSTR lpszFunction, LPCSTR lpszMessage, LPCSTR lpszFile,DWORD dwLine)
	{
		// Make sure the mouse cursor's shown
		ShowCursor(TRUE);

		// Allocate space ( additional byte for NULL terminator )
		m_lpszFunction = new char[strlen(lpszFunction) + 1];
		m_lpszMessage  = new char[strlen(lpszMessage) + 1];
		m_lpszFile     = new char[strlen(lpszFile) + 1];

		// Copy strings & line no
		strcpy( m_lpszFunction, lpszFunction);
		strcpy( m_lpszMessage, lpszMessage);
		strcpy( m_lpszFile, lpszFile);
		m_dwLine = dwLine;
	}

	CException(const CException &iEx)
	{
		// Copy line
		m_dwLine = iEx.Line();

		// Allocate function & message strings (adding 1 for NULL terminator)
		m_lpszFunction = new char[strlen(iEx.Function()) + 1];
		m_lpszMessage  = new char[strlen(iEx.Message()) + 1];
		m_lpszFile     = new char[strlen(iEx.File()) + 1];

		// Copy the strings
		strcpy(m_lpszFunction, iEx.Function());
		strcpy(m_lpszMessage, iEx.Message());
		strcpy(m_lpszFile, iEx.File());
	}

	~CException()
	{
		SAFE_DELETE_ARRAY( m_lpszFunction );
		SAFE_DELETE_ARRAY( m_lpszMessage );
		SAFE_DELETE_ARRAY( m_lpszFile );
	}

public:

	CException& operator =(const CException &iEx)
	{
		this->CException::~CException();

		// Copy line
		m_dwLine = iEx.Line();

		// Allocate fuction & message strings (adding 1 for NULL terminator)
		m_lpszFunction = new char[strlen(iEx.Function()) + 1];
		m_lpszMessage  = new char[strlen(iEx.Message()) + 1];
		m_lpszFile     = new char[strlen(iEx.File()) + 1];

		// Copy the strings
		strcpy(m_lpszFunction, iEx.Function());
		strcpy(m_lpszMessage, iEx.Message());
		strcpy(m_lpszFile, iEx.File());

		return (*this);
	}

public:

	virtual VOID DisplayMsgBox()
	{
		char szCaption[1024], szMessage[1024];

		sprintf(szCaption, "%s", m_lpszFunction);

		sprintf(szMessage, "Msg  : %s\nFile : %s\nLine : %ld" , m_lpszMessage, m_lpszFile, m_dwLine);

		MessageBox(NULL, szMessage, szCaption, MB_OK | MB_ICONERROR);
	}

	LPCSTR const Function() const { return m_lpszFunction; }
	LPCSTR const Message () const { return m_lpszMessage;  }
	LPCSTR const File    () const { return m_lpszFile;     }
	DWORD Line() const { return m_dwLine; }

protected:
	LPSTR m_lpszFunction;
	LPSTR m_lpszMessage;
	LPSTR m_lpszFile;
	DWORD m_dwLine;
};

}
}

#endif