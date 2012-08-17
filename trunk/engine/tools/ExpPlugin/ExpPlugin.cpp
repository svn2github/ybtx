// ExpPlugin.cpp : Defines the initialization routines for the DLL.
//
#include "stdafx.h"
#include "Exp.h"
#include "ExpPlugin.h"
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// #define OUTPUT_DEBUG
// CModelExpApp 

BEGIN_MESSAGE_MAP(CModelExpApp, CWinApp)
END_MESSAGE_MAP()

static const WORD MAX_CONSOLE_LINES = 500;

// The one and only CModelExpApp object
CModelExpApp					theApp;
static CPieceClassDesc		PieceClassDesc;
//static CSkeletalClassDesc	SkeletalClassDesc;
static CAniGroupClassDesc	AnimateGroupClassDesc;
//static CSoftBodyClassDesc	SoftBodyClassDesc;

// CModelExpApp construction
CModelExpApp::CModelExpApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

#ifdef OUTPUT_DEBUG
	int hConHandle;
	long lStdHandle;
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	FILE *fp;
	// allocate a console for this app
	AllocConsole();
	// set the screen buffer to be big enough to let us scroll text
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), 
		&coninfo);
	coninfo.dwSize.Y = MAX_CONSOLE_LINES;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), 
		coninfo.dwSize);
	// redirect unbuffered STDOUT to the console
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );
	// redirect unbuffered STDIN to the console
	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "r" );
	*stdin = *fp;
	setvbuf( stdin, NULL, _IONBF, 0 );
	// redirect unbuffered STDERR to the console
	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stderr = *fp;
	setvbuf( stderr, NULL, _IONBF, 0 );
	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog 
	// point to console as well
	ios::sync_with_stdio();
#endif
}


// CModelExpApp initialization
BOOL CModelExpApp::InitInstance()
{		
	CWinApp::InitInstance();

	return TRUE;
}
/////////////////////////////////////////////////////////////
//返回资源字符串
TCHAR* GetString( int id )
{
	static TCHAR buf[256];
	return LoadString( ::AfxGetInstanceHandle(), id, buf, sizeof(buf) ) ? buf : NULL;
}

//输出支持的CLASS数量
__declspec(dllexport) int LibNumberClasses()
{
	return 2;
}

//输出支持的CLASS指针
__declspec(dllexport) ClassDesc *LibClassDesc(int ClassNum)
{
	switch(ClassNum) 
	{
	//case 0: 
		//return &SkeletalClassDesc;
	case 0: 
		return &PieceClassDesc;
	case 1:
		return &AnimateGroupClassDesc;
	//case 2:
	//	return &SoftBodyClassDesc;
	}
	return 0;
}

//输出CLASS的描述字符串
__declspec(dllexport) const TCHAR* LibDescription()
{
	return GetString(IDS_EXP_DESCRIPTION);
}

//返回3DMAX版本
__declspec(dllexport) unsigned long LibVersion()
{
	return VERSION_3DSMAX;
}

////////////////////////////////////////////////
DWORD WINAPI ProgressBarUpdate( LPVOID arg )
{
	return 0;
}

////////////////////////////////////////////////////////////
//构造函数
CPieceExp::CPieceExp(void)
{
}

//析构函数
CPieceExp::~CPieceExp(void)
{
}

const TCHAR* CPieceExp::Ext( int n )
{ 
	switch( n )
	{ 
	case 0:
		return _T("MOD");
	} 
	
	return _T(""); 
}

//所支持的功能选项
BOOL CPieceExp::SupportsOptions( int ext, DWORD options )
{
	if( ext==0 && options == SCENE_EXPORT_SELECTED )
		return TRUE;
	else
		return FALSE;
}

//输出模型
int CPieceExp::DoExport( const TCHAR* name, ExpInterface* ei, Interface* ip, BOOL suppressPrompts, DWORD options )
{	
	locale loc = locale::global(locale(""));
	string str = name;
	TCHAR Buf[5];
	memcpy( Buf, name + strlen( name ) - 3, 4 );
	_strlwr_s( Buf ,sizeof(Buf));
	if( strcmp( Buf, "mod" ) )
		str += ".mod";

	//////////////////////////////////////////////////////////////////////////
	CFileFind   fileFind;
	bool		IsExist ;
	CString		strPath;
	strPath = str.c_str();
	if(!fileFind.FindFile(strPath))   
		IsExist = false;
	else
		IsExist = true;
	fileFind.Close();
	//////////////////////////////////////////////////////////////////////////

	ip->ProgressStart(GetString(IDS_PROGRESS_MSG), TRUE, ProgressBarUpdate, NULL);	
	CPiecePropDlg Dlg;
	int hr = (int)Dlg.DoModal( str.c_str(), ip, (options&SCENE_EXPORT_SELECTED) ? TRUE : FALSE ,IsExist);
	ip->ProgressEnd();

	locale::global(loc);
	return hr;
}

////////////////////////////////////////////////////////////
//构造函数
CSkeletalExp::CSkeletalExp(void)
{
}

//析构函数
CSkeletalExp::~CSkeletalExp(void)
{
}

const TCHAR* CSkeletalExp::Ext( int n )
{ 
	switch( n )
	{ 
	case 0:
		return _T("SKE");
	} 
	
	return _T(""); 
}

//所支持的功能选项
BOOL CSkeletalExp::SupportsOptions( int ext, DWORD options )
{
	if( ext == 0 && options == SCENE_EXPORT_SELECTED )
		return TRUE;
	else
		return FALSE;
}

//输出模型
int CSkeletalExp::DoExport( const TCHAR* name, ExpInterface* ei, Interface* ip, BOOL suppressPrompts, DWORD options )
{	
	locale loc = locale::global(locale(""));
	string str = name;
	TCHAR Buf[5];

	memcpy( Buf, name + strlen( name ) - 3, 4 );
	_strlwr_s( Buf ,sizeof(Buf));
	if( strcmp( Buf, "ske" ) )
		str += ".ske";

	//////////////////////////////////////////////////////////////////////////
	CFileFind   fileFind;
	bool		IsExist ;
	CString		strPath;
	strPath = str.c_str();
	if(!fileFind.FindFile(strPath))   
		IsExist = false;
	else
		IsExist = true;
	fileFind.Close();
	
	//////////////////////////////////////////////////////////////////////////
	
	ip->ProgressStart(GetString(IDS_PROGRESS_MSG), TRUE, ProgressBarUpdate, NULL);	
	CSkeletalProp Dlg;
	int hr = (int)Dlg.DoModal( str.c_str(), ip, (options&SCENE_EXPORT_SELECTED) ? TRUE : FALSE ,IsExist );
	ip->ProgressEnd();

	locale::global(loc);
	return hr;
}

////////////////////////////////////////////////////////////
//构造函数
CAniGroupExp::CAniGroupExp(void)
{
}

//析构函数
CAniGroupExp::~CAniGroupExp(void)
{
}

const TCHAR* CAniGroupExp::Ext( int n )
{ 
	switch( n )
	{ 
	case 0:
		return _T("AGP");
	} 

	return _T(""); 
}

//所支持的功能选项
BOOL CAniGroupExp::SupportsOptions( int ext, DWORD options )
{
	if( ext == 0 && options == SCENE_EXPORT_SELECTED )
		return TRUE;
	else
		return FALSE;
}

//输出模型
int CAniGroupExp::DoExport( const TCHAR* name, ExpInterface* ei, Interface* ip, BOOL suppressPrompts, DWORD options )
{	
	locale loc = locale::global(locale(""));
	string str = name;
	TCHAR Buf[5];

	memcpy( Buf, name + strlen( name ) - 3, 4 );
	_strlwr_s( Buf ,sizeof(Buf));
	if( strcmp( Buf, "agp" ) )
		str += ".agp";

	//////////////////////////////////////////////////////////////////////////
	CFileFind   fileFind;
	bool		IsExist ;
	CString		strPath;
	strPath = str.c_str();
	if(!fileFind.FindFile(strPath))   
		IsExist = false;
	else
		IsExist = true;
	fileFind.Close();

	//////////////////////////////////////////////////////////////////////////
	ip->ProgressStart(GetString(IDS_PROGRESS_MSG), TRUE, ProgressBarUpdate, NULL);	
	CAniGroupProp Dlg;
	int hr = (int)Dlg.DoModal( str.c_str(), ip, (options&SCENE_EXPORT_SELECTED) ? TRUE : FALSE ,IsExist );
	ip->ProgressEnd();

	locale::global(loc);
	return hr;
}


//////////////////////////////////////////////////////////////
////构造函数
//CSoftBodyExp::CSoftBodyExp(void)
//{
//}
//
////析构函数
//CSoftBodyExp::~CSoftBodyExp(void)
//{
//} 
//
//const TCHAR* CSoftBodyExp::Ext( int n )
//{ 
//	switch( n )
//	{ 
//	case 0:
//		return _T("ASB");
//	} 
//
//	return _T(""); 
//}
//
////所支持的功能选项
//BOOL CSoftBodyExp::SupportsOptions( int ext, DWORD options )
//{
//	if( ext == 0 && options == SCENE_EXPORT_SELECTED )
//		return TRUE;
//	else
//		return FALSE;
//}

////输出模型
//int CSoftBodyExp::DoExport( const TCHAR* name, ExpInterface* ei, Interface* ip, BOOL suppressPrompts, DWORD options )
//{	
//	string str = name;
//	TCHAR Buf[5];
//	memcpy( Buf, name + strlen( name ) - 3, 4 );
//	strlwr( Buf );
//	if( strcmp( Buf, "asb" ) )
//		str += ".asb";
//
//	ip->ProgressStart(GetString(IDS_PROGRESS_MSG), TRUE, ProgressBarUpdate, NULL);	
//	CSoftBodyProp Dlg;
//	int hr =  Dlg.DoModal( str.c_str(), ip, (options&SCENE_EXPORT_SELECTED) ? TRUE : FALSE );
//	ip->ProgressEnd();
//
//	return hr;
//}