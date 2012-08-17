// TextureEditor.h : TextureEditor 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h” 
#endif

#include "resource.h"       // 主符号


// CTextureEditorApp:
// 有关此类的实现，请参阅 TextureEditor.cpp
//

class CDocManagerEx : public CDocManager
{		
public:
	CDocManagerEx(){};
	virtual ~CDocManagerEx(){};
	
	virtual BOOL DoPromptFileName(CString& fileName, UINT nIDSTitle,
		DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate)
	{
		CFileDialog dlgFile(bOpenFileDialog);
		static TCHAR SaveStr[]="贴图文件 DDS文件  (*.dds)\0*.dds\0";
		static TCHAR OpenStr[]="贴图文件 (*.tex)\0*.tex\0BMP文件  (*.bmp)\0*.bmp\0TGA文件  (*.tga)\0*.tga\0JPG文件  (*.jpg)\0*.jpg\0DDS文件  (*.dds)\0*.dds\0";
		
		CString title;
		VERIFY(title.LoadString(nIDSTitle));
		
		dlgFile.m_ofn.Flags |= lFlags|OFN_NOCHANGEDIR;
		dlgFile.m_ofn.nMaxCustFilter++;		
		dlgFile.m_ofn.lpstrFilter = bOpenFileDialog?OpenStr:SaveStr;
		dlgFile.m_ofn.lpstrTitle = title;
		dlgFile.m_ofn.lpstrFile = fileName.GetBuffer(_MAX_PATH);
			
		INT_PTR nResult = dlgFile.DoModal();
		fileName.ReleaseBuffer();
		if ( 0 == bOpenFileDialog )
		{
			// 根据用户选择的扩展名类型索引生成扩展名	
			CString	strFileExt;
			int		nFilterIndex	= dlgFile.m_ofn.nFilterIndex;

			strFileExt	= ".dds";
	
			// 去掉原有扩展名，变成用户选择的扩展名
			int DotPos = fileName.Find( ".", max( fileName.GetLength() - 4, 0 ) ); 
			if (  DotPos >= 0 )
			{
				fileName.Delete(DotPos, 10);
			}
			fileName.Append(strFileExt);
		}
		
		

		return nResult == IDOK;
	}
};

class CTextureEditorApp : public CWinApp
{
public:
	CTextureEditorApp();


// 重写
public:
	virtual BOOL InitInstance();

// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CTextureEditorApp theApp;
