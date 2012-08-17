// ExpPlugin.h : main header file for the ExpPlugin DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "PropSelectDlg.h"
TCHAR* GetString( int id );

// CModelExpApp
// See ExpPlugin.cpp for the implementation of this class
//

#define _PIECE_EXPORT_CLASSID			Class_ID(0x58454f5d, 0x62704256)
#define _SKELETAL_EXPORT_CLASSID			Class_ID(0x1fc76892, 0x6bdb3a32)
#define _ANIGROUP_EXPORT_CLASSID			Class_ID(0x5D3F0711, 0x2A16D367)

class CPieceExp : public SceneExport
{
public:
	CPieceExp(void);
	~CPieceExp(void);
	
	int          ExtCount()				    { return 1; };
	const TCHAR* Ext( int n );

	// descriptions and messages
	const TCHAR* LongDesc()				    { return _T( " Piece File" ); };
	const TCHAR* ShortDesc()			    { return _T( " Piece File" ); };
	const TCHAR* AuthorName()				{ return _T( "" ); };
	const TCHAR* CopyrightMessage()			{ return _T( "" ); };
	const TCHAR* OtherMessage1()			{ return _T( "" ); };
	const TCHAR* OtherMessage2()			{ return _T( "" ); };
	unsigned int Version()					{ return 100; };
	void  ShowAbout( HWND hWnd )			{ MessageBox( hWnd,"The  Piece Export.","About",0 ); };

	BOOL  SupportsOptions( int ext, DWORD options );
	// do the actual export
	int DoExport( const TCHAR* name, ExpInterface* ei, Interface* ip, BOOL suppressPrompts=FALSE, DWORD options=0 );
};

class CPieceClassDesc : public ClassDesc2
{
public:
	int IsPublic()							{ return TRUE; }
	void* Create( BOOL loading = FALSE )	{ return new CPieceExp; }
	const TCHAR* ClassName()				{ return GetString( IDS_PIECECLASS ); }
	SClass_ID SuperClassID()				{ return SCENE_EXPORT_CLASS_ID; }
	Class_ID ClassID()						{ return _PIECE_EXPORT_CLASSID; }
	const TCHAR* Category()					{ return _T(""); }
	HINSTANCE HInstance()					{ return ::AfxGetInstanceHandle(); }

	const TCHAR* InternalName()				{ return _T(" Piece Export"); }
};

class CSkeletalExp : public SceneExport
{
public:
	CSkeletalExp(void);
	~CSkeletalExp(void);
	
	int          ExtCount()					{ return 1; };
	const TCHAR* Ext( int n );

	// descriptions and messages
	const TCHAR* LongDesc()					{ return _T( " Skeletal File" ); };
	const TCHAR* ShortDesc()				{ return _T( " Skeletal File" ); };
	const TCHAR* AuthorName()				{ return _T( "" ); };
	const TCHAR* CopyrightMessage()			{ return _T( "" ); };
	const TCHAR* OtherMessage1()			{ return _T( "" ); };
	const TCHAR* OtherMessage2()			{ return _T( "" ); };
	unsigned int Version()					{ return 100; };
	void  ShowAbout( HWND hWnd )			{ MessageBox( hWnd,"The  Skeletal Export.","About",0 ); };

	BOOL  SupportsOptions( int ext, DWORD options );
	// do the actual export
	int DoExport( const TCHAR* name, ExpInterface* ei, Interface* ip, BOOL suppressPrompts=FALSE, DWORD options=0 );
};

class CSkeletalClassDesc : public ClassDesc2
{
public:

	int IsPublic()							{ return TRUE; }
	void* Create( BOOL loading = FALSE )	{ return new CSkeletalExp; }
	const TCHAR* ClassName()				{ return GetString( IDS_SKELETALCLASS ); }
	SClass_ID SuperClassID()				{ return SCENE_EXPORT_CLASS_ID; }
	Class_ID ClassID()						{ return _SKELETAL_EXPORT_CLASSID; }
	const TCHAR* Category()					{ return _T(""); }
	HINSTANCE HInstance()					{ return ::AfxGetInstanceHandle(); }

	const TCHAR* InternalName()				{ return _T(" Animate Export"); }
};

class CAniGroupExp : public SceneExport
{
public:
	CAniGroupExp(void);
	~CAniGroupExp(void);

	int          ExtCount()					{ return 1; };
	const TCHAR* Ext( int n );

	// descriptions and messages
	const TCHAR* LongDesc()					{ return _T( " AGP File" ); };
	const TCHAR* ShortDesc()				{ return _T( " AGP File" ); };
	const TCHAR* AuthorName()				{ return _T( "" ); };
	const TCHAR* CopyrightMessage()			{ return _T( "" ); };
	const TCHAR* OtherMessage1()			{ return _T( "" ); };
	const TCHAR* OtherMessage2()			{ return _T( "" ); };
	unsigned int Version()					{ return 100; };
	void  ShowAbout( HWND hWnd )			{ MessageBox( hWnd,"The  AGP Export.","About",0 ); };

	BOOL  SupportsOptions( int ext, DWORD options );
	// do the actual export
	int DoExport( const TCHAR* name, ExpInterface* ei, Interface* ip, BOOL suppressPrompts=FALSE, DWORD options=0 );
};

class CAniGroupClassDesc : public ClassDesc2
{
public:

	int IsPublic()							{ return TRUE; }
	void* Create( BOOL loading = FALSE )	{ return new CAniGroupExp; }
	const TCHAR* ClassName()				{ return GetString( IDS_SKELETALCLASS ); }
	SClass_ID SuperClassID()				{ return SCENE_EXPORT_CLASS_ID; }
	Class_ID ClassID()						{ return _ANIGROUP_EXPORT_CLASSID; }
	const TCHAR* Category()					{ return _T(""); }
	HINSTANCE HInstance()					{ return ::AfxGetInstanceHandle(); }

	const TCHAR* InternalName()				{ return _T(" Agp Export"); }
};



class CModelExpApp : public CWinApp
{
public:

// Overrides
public:
	CModelExpApp();
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
