// MapEditorDoc.cpp :  CMapEditorDoc 类的实现
//

#include "stdafx.h"
#include "MapEditor.h"
#include <CTileEditApp.h>
#include "MapEditorDoc.h"
#include ".\mapeditordoc.h"
#include <Engine.h>
#include <SharedTexture.h>
#include <WireBox.h>
#include "NewMapDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMapEditorDoc

IMPLEMENT_DYNCREATE(CMapEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CMapEditorDoc, CDocument)
END_MESSAGE_MAP()


// CMapEditorDoc 构造/析构

CMapEditorDoc::CMapEditorDoc()
{
	// TODO: 在此添加一次性构造代码

}

CMapEditorDoc::~CMapEditorDoc()
{
}

BOOL CMapEditorDoc::OnNewDocument()
{
	CString s = this->GetTitle();
	if ( this->GetTitle() != "无标题" && this->GetTitle() != "Untitled" )
		CTileEditApp::GetInstance()->GetTerrainMesh()->SaveAsLocal((const char*)this->GetTitle());

	if (!CDocument::OnNewDocument())
		return FALSE;

	static bool bFirst = true;
	
	if ( !bFirst )
	{
		CNewMapDialog d;
		d.DoModal();
		this->SetTitle("新世界");
	}
	
	if ( bFirst ) bFirst = false;
	
	return TRUE;
}

// CMapEditorDoc 序列化

void CMapEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}


// CMapEditorDoc 诊断

#ifdef _DEBUG
void CMapEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMapEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMapEditorDoc 命令

BOOL CMapEditorDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if ( this->GetTitle() != "无标题" && this->GetTitle() != "Untitled" )
		CTileEditApp::GetInstance()->GetTerrainMesh()->SaveAsLocal((const char*)this->GetTitle());
	
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
		
	CTileEditApp::GetInstance()->GetTerrainMesh()->LoadAsLocal(lpszPathName);
	
	// TODO:  在此添加您专用的创建代码
	
	return TRUE;
}

BOOL CMapEditorDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	CTileEditApp::GetInstance()->GetTerrainMesh()->SaveAsLocal(lpszPathName);
	return true;
}
