#include "StdAfx.h"
#include "CTextureViewCltr.h"
#include "TextureViewCtrl/TextureViewCtrlImp.h"
#include "dthelp.h"

System::Void CTextureViewCltr::layerOne_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
	if(this->layerOne_radioButton->Checked)
	{
		CTextureViewCtrlImp *pImp = (CTextureViewCtrlImp*)m_pCtrl;
		if( pImp )
			pImp->SetHotLayer(0);
	}
}

System::Void CTextureViewCltr::layerOneCliff_checkBox_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{

}

System::Void CTextureViewCltr::layerTwo_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
	if(this->layerTwo_radioButton->Checked)
	{
		CTextureViewCtrlImp *pImp = (CTextureViewCtrlImp*)m_pCtrl;
		if( pImp )
			pImp->SetHotLayer(1);
	}
}

System::Void CTextureViewCltr::addAlpha_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
	if(this->addAlpha_radioButton->Checked)
	{
		CTextureViewCtrlImp *pImp = (CTextureViewCtrlImp*)m_pCtrl;
		if( pImp )
			pImp->SetWayType(0);
	}
}

System::Void CTextureViewCltr::reduceAlpha_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
	if(this->reduceAlpha_radioButton->Checked)
	{
		CTextureViewCtrlImp *pImp = (CTextureViewCtrlImp*)m_pCtrl;
		if( pImp )
			pImp->SetWayType(1);
	}
}

System::Void CTextureViewCltr::smoothAlpha_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{

}

System::Void CTextureViewCltr::eraseAlpha_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
	if(this->eraseAlpha_radioButton->Checked)
	{
		CTextureViewCtrlImp *pImp = (CTextureViewCtrlImp*)m_pCtrl;
		if( pImp )
			pImp->SetWayType(2);
	}
}

System::Void CTextureViewCltr::dirX_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
	if(this->dirX_radioButton->Checked)
	{
		CTextureViewCtrlImp *pImp = (CTextureViewCtrlImp*)m_pCtrl;
		if( pImp )
			pImp->SetDirType(0);
	}
}

System::Void CTextureViewCltr::dirZ_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
	if(this->dirZ_radioButton->Checked)
	{
		CTextureViewCtrlImp *pImp = (CTextureViewCtrlImp*)m_pCtrl;
		if( pImp )
			pImp->SetDirType(1);
	}
}

System::Void CTextureViewCltr::dirXZ_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
	if(this->dirXZ_radioButton->Checked)
	{
		CTextureViewCtrlImp *pImp = (CTextureViewCtrlImp*)m_pCtrl;
		if( pImp )
			pImp->SetDirType(2);
	}
}

System::Void CTextureViewCltr::brushStrenth_trackBar_Scroll( System::Object^ sender, System::EventArgs^ e )
{
	float fAlphaStrength = float(this->brushStrenth_trackBar->Value);
	CTextureViewCtrlImp *pImp = (CTextureViewCtrlImp*)m_pCtrl;
	pImp->SetAlphaStrength(fAlphaStrength);
}

System::Void CTextureViewCltr::changeSingleTexture_button_Click( System::Object^ sender, System::EventArgs^ e )
{

}

System::Void CTextureViewCltr::SingleTextureToGroup_button_Click( System::Object^ sender, System::EventArgs^ e )
{

}

System::Void CTextureViewCltr::GroupTextureToGroup_button_Click( System::Object^ sender, System::EventArgs^ e )
{

}

System::Void CTextureViewCltr::deleteSingleTexture_button_Click( System::Object^ sender, System::EventArgs^ e )
{

}

System::Void CTextureViewCltr::deleteGroupTexture_button_Click( System::Object^ sender, System::EventArgs^ e )
{

}

void CTextureViewCltr::InsertTextureResetTreeView( String^ resetname )
{
	CTextureViewCtrlImp *pImp = NULL;
	pImp = (CTextureViewCtrlImp*)m_pCtrl;
	if( pImp )
 	{
 		TreeNode^ pParentNode = gcnew TreeNode();
 		pParentNode->Text = resetname;
 		pParentNode->ImageIndex = 0;
 		this->texture_treeView->Nodes->Add(pParentNode);

		//2D
		size_t group2DSize = pImp->GetReset2DGroupCount();
		for ( size_t m = 0; m < group2DSize; ++m )
		{
			string strGroupName = "";
			pImp->GetReset2DGroupNameByIndex(m, strGroupName);
			String^clrGroupName = gcnew String(strGroupName.c_str());

			TreeNode^ p2DGroupNode = gcnew TreeNode;
			p2DGroupNode->Text = clrGroupName;
			p2DGroupNode->ImageIndex = 1;
			pParentNode->Nodes->Add(p2DGroupNode);

			size_t rsSetUninSize = pImp->GetReset2DGroupUnitCount(m);
			for ( size_t n = 0; n < rsSetUninSize; ++n )
			{
				string strUnitName = "";
				pImp->GetReset2DUnitNameByIndex(m, n, strUnitName);

				TreeNode^ p2DUnitNode = gcnew TreeNode();
				p2DUnitNode->Text = gcnew String(strUnitName.c_str());
				p2DUnitNode->ImageIndex = 2;
				p2DUnitNode->SelectedImageIndex = 3;
				p2DGroupNode->Nodes->Add(p2DUnitNode);
			}
		}

 		pParentNode->Expand();
 	}
}

void sqr_tools::CTextureViewCltr::Update( void )
{
	CTextureViewCtrlImp *pImp = NULL;
	pImp = (CTextureViewCtrlImp*)m_pCtrl;
	if( pImp )
	{
		string  strRessetName;
		pImp->GetResSetName(strRessetName);
		String^ resetname = gcnew String(strRessetName.c_str());
		this->InsertTextureResetTreeView(resetname);
	}
}

char* CTextureViewCltr::GetImpTypeName( void )
{
	return GET_IMP_NAME(CTextureViewCtrlImp);
}

void* CTextureViewCltr::GetImpParam( void )
{
	return this->Handle.ToPointer();
}

System::Void CTextureViewCltr::texture_treeView_NodeMouseClick( System::Object^ sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^ e )
{
// 	TreeNode ^seleceNode = this->texture_treeView->SelectedNode;
// 	SelectTreeNode(seleceNode);
}

System::Void CTextureViewCltr::texture_treeView_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e)
{
	TreeNode ^seleceNode = this->texture_treeView->SelectedNode;
	SelectTreeNode(seleceNode);
}

System::Void CTextureViewCltr::SelectTreeNode( TreeNode ^seleceNode )
{
	if( seleceNode )
	{
		CTextureViewCtrlImp *pImp = NULL;
		pImp = (CTextureViewCtrlImp*)m_pCtrl;
		if( !pImp )
			return;

		int nSelectImageIndex = seleceNode->ImageIndex;
		if( nSelectImageIndex == 1 )
		{
			TreeNode^ pGroupNode = seleceNode;
			TreeNode^ pReSetNode = pGroupNode->Parent;
			if( !pReSetNode )
				return;

			String^ clrGroupName = pGroupNode->Text;
			String^ clrResetName = pReSetNode->Text;

			std::string strGroupName = 	MagStringToUnmagString(clrGroupName);
			std::string strResetName = 	MagStringToUnmagString(clrResetName);


			pImp->SelectActiveUnit(strResetName, strGroupName, "", false);
		}
		else
		{
			if( nSelectImageIndex == 2 )
			{
				TreeNode^ pGroupNode = seleceNode->Parent;
				if( !pGroupNode )
					return;
				TreeNode^ pReSetNode = pGroupNode->Parent;
				if( !pReSetNode )
					return;

				String^ clrGroupName = pGroupNode->Text;
				String^ clrResetName = pReSetNode->Text;

				std::string strGroupName = MagStringToUnmagString(clrGroupName);
				std::string strResetName = MagStringToUnmagString(clrResetName);


				String^ clrUnitName = seleceNode->Text;
				std::string strUnitName = MagStringToUnmagString(clrUnitName);

				pImp->SelectActiveUnit(strResetName, strGroupName, strUnitName, true);
			}
		}
	}
}

System::Void CTextureViewCltr::CTextureViewCltr_Load( System::Object^ sender, System::EventArgs^ e )
{
	char* ImpName = GetImpTypeName();
	if( ImpName == NULL )
		return;
}