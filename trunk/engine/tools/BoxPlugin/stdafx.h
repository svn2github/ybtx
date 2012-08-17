// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__10DD0601_49F1_4F6E_B238_13869FA3CC4E__INCLUDED_)
#define AFX_STDAFX_H__10DD0601_49F1_4F6E_B238_13869FA3CC4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define SINGLEREF

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "GLobD3d.h"
#include "BonesPro.H"
#include "iparamm2.h"
#include <afxdhtml.h>
TCHAR* GetString( int id );
#endif




