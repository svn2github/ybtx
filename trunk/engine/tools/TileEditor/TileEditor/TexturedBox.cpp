// TexturedBox.cpp : 实现文件
//

#include "stdafx.h"
#include "MapEditor.h"
#include "TexturedBox.h"
#include ".\texturedbox.h"
#include <Operator.h>
#include <Render.h>
#include <Scene.h>
#include <GUITexturedBox.h>
#include <SharedTexture.h>
#include "FileTree.h"
#include <CTileEditApp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CTexturedBox

CTexturedBox * CTexturedBox::ms_pHot = NULL;

IMPLEMENT_DYNAMIC(CTexturedBox, CButton)

CTexturedBox::CTexturedBox(DWORD id)
{
	m_id = id;
	m_pNode = NULL;
	m_pTexture = NULL;
}

CTexturedBox::~CTexturedBox()
{
	m_pNode = NULL;
	m_pTexture->Drop();
}

BEGIN_MESSAGE_MAP(CTexturedBox, CButton)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_KEYUP()
END_MESSAGE_MAP()

void CTexturedBox::SetHot(bool b)
{
	if ( !b )
	{ 
		m_pNode->SetHot(b);
		return;
	}
	
	if ( ms_pHot ) ms_pHot->SetHot(false);
	ms_pHot = this;
	m_pNode->SetHot(b);
	CTileEditApp::GetInstance()->GetOperator()->SetTextureHandleHot(m_pNode->GetTextureHandle());
}

void CTexturedBox::SetMove(bool b)
{
	m_pNode->SetMove(b);
}

void CTexturedBox::Init(DWORD dwWidth, DWORD dwHeight, const char * strTex)
{
	m_pTexture = CSharedTexture::GetTexture(strTex);
	m_pNode = CSceneManager::GetInstance()->AddNodeGUICheckBox(dwWidth, dwHeight, m_pTexture->GetTextureHandle(), this->GetSafeHwnd());
}

void CTexturedBox::ResetTexture(const char * strTex)
{
	//@ m_pTexture->Drop();
	m_pTexture 	= CSharedTexture::GetTexture(strTex);
	m_pNode->SetTextureHandle(m_pTexture->GetTextureHandle());
}

void CTexturedBox::OnMouseMove(UINT nFlags, CPoint point)
{
	this->SetMove(true);	
}

void CTexturedBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	this->SetHot(true);
	ui::CCheckBox::SetHotID(m_id);
	CButton::OnLButtonUp(nFlags, point);
}