#include "StdAfx.h"
#include "CSetViewCtrl.h"
#include "SetViewCtrl/SetViewCtrlImp.h"
#include "dthelp.h"

System::Void CSetViewCtrl::insertReset_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	openFileDialog->Title			 = "选择图素包";
	openFileDialog->Filter			 = "ERP文件|*.erp||";
	openFileDialog->Multiselect		 = true;
	openFileDialog->RestoreDirectory = true;

	if( openFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK )
	{
		array<String^>^ resultFiles = openFileDialog->FileNames;
		int nLen = resultFiles->Length;
		CSetViewCtrlImp *pImp = NULL;
		pImp = (CSetViewCtrlImp*)m_pCtrl;

		for ( int i = 0; i < nLen; ++i )
		{
			String^ clrReset = resultFiles[i];
			string  strReset = MagStringToUnmagString(clrReset);

			size_t npos = clrReset->LastIndexOf("\\");
			if( npos != -1 )
			{
				int nLen = clrReset->Length - npos - 1 - 4;
				clrReset = clrReset->Substring(npos+1, nLen);

				string  resetname = MagStringToUnmagString(clrReset);
				//如果存在已有的图素包集，就将原有的删掉，添加现新加的
				if ( pImp->ResetIsExit(resetname) )
				{
					//RemoveOldSet(clrReset);
					String^ log = gcnew String(clrReset);
					log = log + "图素包已存在";
					MessageBox::Show( log );
					return;
				}

				pImp->LoadReset(strReset);
				this->InsertObjectResetTreeView(clrReset);
			}
		}
	}
}

System::Void CSetViewCtrl::deleteReset_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	TreeNode ^seleceNode = this->objectReset_treeView->SelectedNode;
	if( seleceNode )
	{
		this->objectReset_treeView->Nodes->Remove(seleceNode);
		
		String^ clrResSetName = seleceNode->Text;
		string  strResetName  = MagStringToUnmagString(clrResSetName);
		CSetViewCtrlImp *pImp = (CSetViewCtrlImp*)m_pCtrl;
		pImp->DeleteCurReset(strResetName);
	}
}

System::Void CSetViewCtrl::selectObjectTreeViewNode( TreeNode ^seleceNode )
{
	if( seleceNode )
	{
		CSetViewCtrlImp *pImp = NULL;
		pImp = (CSetViewCtrlImp*)m_pCtrl;
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

			pImp->SelectActiveUnit(strResetName, strGroupName, "", true);
		}
		else
		{
			if( nSelectImageIndex == 2 )
			{
				TreeNode^ pGroupNode = seleceNode->Parent;
				if( !pGroupNode )
					return;

				TreeNode^ preSetNode = pGroupNode->Parent;
				if( !preSetNode )
					return;

				String^ clrGroupName = pGroupNode->Text;
				String^ clrResetName = preSetNode->Text;
				String^ clrUnitName = seleceNode->Text;
				
				std::string strGroupName = MagStringToUnmagString(clrGroupName);
				std::string strResetName = MagStringToUnmagString(clrResetName);
				std::string strUnitName = MagStringToUnmagString(clrUnitName);
				
				pImp->SelectActiveUnit(strResetName, strGroupName, strUnitName, true);
				return;
			}
		}
	}
}

System::Void CSetViewCtrl::nextRS_button_Click( System::Object^ sender, System::EventArgs^ e )
{
}

System::Void CSetViewCtrl::editRS_button_Click( System::Object^ sender, System::EventArgs^ e )
{
}

System::Void CSetViewCtrl::mirrorX_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
}

System::Void CSetViewCtrl::mirrorZ_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
}

System::Void CSetViewCtrl::noMirror_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
}

System::Void CSetViewCtrl::copyObject_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
}

System::Void CSetViewCtrl::copyTerrain_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
}

System::Void CSetViewCtrl::copyAll_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
}

void CSetViewCtrl::InsertObjectResetTreeView(String^ resetname)
{
	CSetViewCtrlImp *pImp = NULL;
	pImp = (CSetViewCtrlImp*)m_pCtrl;
 	if( pImp )
 	{
 		TreeNode^ pParentNode = gcnew TreeNode;
 		pParentNode->Text = resetname;
 		pParentNode->ImageIndex = 0;
 		pParentNode->SelectedImageIndex = 0;
 		this->objectReset_treeView->Nodes->Add(pParentNode);
 
		//3D
		size_t group3DSize = pImp->GetReset3DGroupCount();
		for ( size_t m = 0; m < group3DSize; ++m )
		{
			string strGroupName = "";
			pImp->GetReset3DGroupNameByIndex(m, strGroupName);
			String^clrGroupName = gcnew String(strGroupName.c_str());

			TreeNode^ p3DGroupNode = gcnew TreeNode;
			p3DGroupNode->Text = clrGroupName;
			p3DGroupNode->ImageIndex = 1;
			pParentNode->Nodes->Add(p3DGroupNode);

			size_t rsSetUninSize = pImp->GetReset3DGroupUnitCount(m);
			for ( size_t n = 0; n < rsSetUninSize; ++n )
			{
				string strUnitName = "";
				pImp->GetReset3DUnitNameByIndex(m, n, strUnitName);

				String^clrUnitName = gcnew String(strUnitName.c_str());
				TreeNode^ p3DUnitNode = gcnew TreeNode;
				p3DUnitNode->Text = clrUnitName;
				p3DUnitNode->ImageIndex = 2;
				p3DUnitNode->SelectedImageIndex = 3;
				p3DGroupNode->Nodes->Add(p3DUnitNode);
			}
		}
 	}
}

void CSetViewCtrl::RemoveOldSet( String^ resetname )
{
	int count = this->objectReset_treeView->Nodes->Count;
	for( int i = 0; i < count; ++i )
	{
		TreeNode^ node = this->objectReset_treeView->Nodes[i];
		String^ strNodeText  = node->Text;
		if( strNodeText == resetname )
		{
			this->objectReset_treeView->Nodes->Remove(node);
			return;
		}
	}
}

void CSetViewCtrl::Update( void )
{
	CSetViewCtrlImp *pImp = NULL;
	pImp = (CSetViewCtrlImp*)m_pCtrl;
	if( pImp )
	{
		string  strRessetName;
		pImp->GetResSetName(strRessetName);
		String^ resetname = gcnew String(strRessetName.c_str());
		this->InsertObjectResetTreeView(resetname);
	}
}

char* CSetViewCtrl::GetImpTypeName( void )
{
	return GET_IMP_NAME(CSetViewCtrlImp);
}

void* CSetViewCtrl::GetImpParam( void )
{
	return this->Handle.ToPointer();
}

System::Void CSetViewCtrl::CSetViewCtrl_Load( System::Object^ sender, System::EventArgs^ e )
{
	char* ImpName = GetImpTypeName();
	if( ImpName == NULL )
		return;
}

System::Void CSetViewCtrl::objectReset_treeView_MouseUp( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
{
	TreeNode ^mouseUpNode = this->objectReset_treeView->GetNodeAt(e->X, e->Y);
	if( mouseUpNode )
		selectObjectTreeViewNode(mouseUpNode);
	else
	{
		//控件空白区
		return;
	}
}