#include "stdafx.h"
#include "CMapObjBaseDialog.h"
#include "TerrainMeshObject.h"
#include "CEditModel.h"
#include "EditGuiMgr\EditGuiMgr.h"
#include "SQRRootWnd.h"
#include "CMapEditApp.h"
#include "Operator.h"
#include "Operator.inl"
#include "ModelGroup.h"

namespace sqr
{
	CMapObjBaseDialog::CMapObjBaseDialog() : m_pEditorObject(NULL)
	{
	}

	CMapObjBaseDialog::CMapObjBaseDialog(const string& aucname, CTObject *pObject)
	{
		m_strGuiAucName = aucname;
		m_pEditorObject = pObject;
		this->CreateObjRenderDialog(m_pEditorObject);
	}

	CMapObjBaseDialog::~CMapObjBaseDialog()
	{
	}

	void CMapObjBaseDialog::CreateObjRenderDialog(CTObject *pObject)
	{
		m_pEditorObject = pObject;
		if( this->CreateFromResEx(m_strGuiAucName.c_str(), CEditGuiMgr::GetInst()->GetGuiRootWnd(), false, false) == GERROR )
			MessageBox(0, "AddCueWnd.auc资源创建失败", "提示", 0);
		else
		{
			this->ShowWnd(true);
//			this->GetDlgChild("WndNum")->SetWndText("test");

// 			WND_IMAGE_LIST DefaultImage;
// 			CFPos rectPos(0.0f, 0.0f);
// 			CFRect rt;
// 			this->GetDlgChild("WndNum")->GetLogicRect(rt);
// 
// 			DefaultImage.AddImage( m_pBaseDialog->GetGraphic(), -1, "", &rt, rectPos, 0xff0000ff, 0 );
// 	
// 			this->GetDlgChild("WndNum")->SetWndBkImage(IMAGE_PARAM(SM_BS_BK, IP_ENABLE), DefaultImage);
		}
	}

	void CMapObjBaseDialog::SetDialogWndImageParam( const string& wndName, const string& texname, const uint32 uColor )
	{
		WND_IMAGE_LIST DefaultImage;
		CFPos rectPos(0.0f, 0.0f);
		CFRect rt;
		this->GetDlgChild(wndName.c_str())->GetLogicRect(rt);
		DefaultImage.AddImage( this->GetGraphic(), -1, texname.c_str(), &rt, rectPos, uColor, 0 );
		this->GetDlgChild(wndName.c_str())->SetWndBkImage(IMAGE_PARAM(SM_BS_BK, IP_ENABLE), DefaultImage);
	}

	void CMapObjBaseDialog::SetDialogWndText(const string& wndName, const string& text)
	{
		this->GetDlgChild(wndName.c_str())->SetWndText(text.c_str());
	}
}