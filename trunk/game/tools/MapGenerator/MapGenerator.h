// MapGenerator.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMapGeneratorApp:
// See MapGenerator.cpp for the implementation of this class
//
class FractalTerrain;

class CMapGeneratorApp : public CWinApp
{
public:
	CMapGeneratorApp();

	FractalTerrain* GetInstFractal() const;

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()

private:
	FractalTerrain *m_pFractal;
};

extern CMapGeneratorApp theApp;