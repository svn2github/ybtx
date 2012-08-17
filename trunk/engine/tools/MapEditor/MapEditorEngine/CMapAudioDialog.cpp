#include "stdafx.h"
#include "CMapAudioDialog.h"
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
	CMapAudioDialog::CMapAudioDialog()
	{
	}

	CMapAudioDialog::CMapAudioDialog(CTObject *pObject) : CMapObjBaseDialog("Bag_ChkBtn_04", pObject)
	{
	}

	CMapAudioDialog::~CMapAudioDialog()
	{
	}

	void CMapAudioDialog::OnLButtonDown( uint32 nFlags, int32 x, int32 y )
	{
		COperator *pOperator = CMapEditApp::GetInst()->GetOperator();
		if(pOperator && m_pEditorObject)
		{
			pOperator->SetObjectHot(m_pEditorObject);

			CEditModelGroup *pModelGroup = m_pEditorObject->GetModelGroup();
			if(pModelGroup)
			{
				string strCueName = pModelGroup->GetSceneCueName();
				this->GetDlgChild("WndNum")->SetWndText(strCueName.c_str());

				this->SetDialogWndImageParam("BtnCD", "", 0xff0000ff);
			}
		}
	}
}