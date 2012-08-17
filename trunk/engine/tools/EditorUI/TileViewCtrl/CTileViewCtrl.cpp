#include "StdAfx.h"
#include "CTileViewCtrl.h"
#include "TileViewCtrl/TileViewCtrlImp.h"
#include "MapEffectCtrl/MapEffectCtrlImp.h"
#include "ContextCtrl/CContextCtrl.h"
#include "dthelp.h"


#define  USE_OLD_RESOURCEDIR_NOTCODER
bool bIs3D = true;

System::Void CTileViewCtrl::createReset_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	std::string strResetName = MagStringToUnmagString(this->resetName_textBox->Text);

	CTileViewCtrlImp *pImp = NULL;
	pImp = (CTileViewCtrlImp*)m_pCtrl;
	if( pImp->CreateTileReset(strResetName) )
	{
		this->CreateTreeRoot();
		this->resets_listBox->Items->Add(this->resetName_textBox->Text);
	}
}

System::Void CTileViewCtrl::deleteReset_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	String^ clrResetName = this->resetName_textBox->Text;
	std::string strResetName = MagStringToUnmagString(this->resetName_textBox->Text);
	
	CTileViewCtrlImp *pImp = NULL;
	pImp = (CTileViewCtrlImp*)m_pCtrl;
	if( pImp->DeleteCurReset(strResetName) )
	{
		int nSelectIndex = this->resets_listBox->SelectedIndex;
		this->resets_listBox->Items->RemoveAt(nSelectIndex);
		this->resetName_textBox->Text = "";
	}
}

System::Void CTileViewCtrl::importReset_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	openFileDialog->Title			 = "选择图素包";
	openFileDialog->Filter			 = "ERP文件|*.erp||";
	openFileDialog->Multiselect		 = true;
	openFileDialog->RestoreDirectory = true;
	
	if( openFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK )
	{
		vector<string> resets;
		array<String^>^ resultFiles = openFileDialog->FileNames;
		int nLen = resultFiles->Length;

		for ( int i = 0; i < nLen; ++i )
		{
			String^ clrReset = resultFiles[i];
			string  strReset = MagStringToUnmagString(clrReset);
			resets.push_back(strReset);

			size_t npos = clrReset->LastIndexOf("\\");
			if( npos != -1 )
			{
				int nLen = clrReset->Length - npos - 1 - 4;
				clrReset = clrReset->Substring(npos+1, nLen);
				this->resets_listBox->Items->Add(clrReset);

				this->resetName_textBox->Text = clrReset;
			}
		}

		if( !resets.empty() )
		{
			CTileViewCtrlImp *pImp = NULL;
			pImp = (CTileViewCtrlImp*)m_pCtrl;
			pImp->ImportReset(resets);

			this->ResetUnitsTree();
		}
	}

}

System::Void CTileViewCtrl::SaveReset_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	CTileViewCtrlImp *pImp = NULL;
	pImp = (CTileViewCtrlImp*)m_pCtrl;
	string strResetName = MagStringToUnmagString(this->resetName_textBox->Text);
	pImp->SaveCurReset(strResetName);
}

System::Void CTileViewCtrl::patchSave_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	vector<string> resets;
	int nCount = this->resets_listBox->Items->Count;
	for (int m = 0; m < nCount; ++m)
	{
		String^ clrReset = this->resets_listBox->Items[m]->ToString();
		std::string strResetName = MagStringToUnmagString(clrReset);
		resets.push_back(strResetName);
	}

	if( !resets.empty() )
	{
		CTileViewCtrlImp *pImp = NULL;
		pImp = (CTileViewCtrlImp*)m_pCtrl;
		//pImp->PatchSaveCurReset(resets);
		if( pImp )
			pImp->PathResetFile();
	}
}

System::Void CTileViewCtrl::resets_listBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
{
	std::string strResetName("");
	String^ clrResetName = this->resetName_textBox->Text;
	if( clrResetName == "" )
		return;

	this->resetName_textBox->Text = clrResetName;
	strResetName  = MagStringToUnmagString(this->resets_listBox->Text);
	
	//选择active图素包
	CTileViewCtrlImp *pImp = NULL;
	pImp = (CTileViewCtrlImp*)m_pCtrl;
	pImp->SelectCurReset(strResetName);

	this->ResetUnitsTree();
}

System::Void CTileViewCtrl::groupType_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
{

}

System::Void CTileViewCtrl::createGroup_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	String^clrGroupName = this->tileGroupName_textBox->Text;
	std::string strGroupName = MagStringToUnmagString(clrGroupName);
	if( strGroupName.empty() )
	{
		MessageBox::Show( "您创建的图素包为空，请输入图素包名" );
		return;
	}

	if( this->nUnitTreeSelectImageInex == 1 || this->nUnitTreeSelectImageInex == 2 )
	{
		CTileViewCtrlImp *pImp = NULL;
		pImp = (CTileViewCtrlImp*)m_pCtrl;

		if( this->nUnitTreeSelectImageInex == 1 )
		{
			pImp->CreateResSetGroup(strGroupName, true, this->groupType_comboBox->SelectedIndex, false);
			this->CreateResSetGroupNode(clrGroupName, true);
		}
		else if( this->nUnitTreeSelectImageInex == 2 )
		{
			pImp->CreateResSetGroup(strGroupName, false, -1, false);
			this->CreateResSetGroupNode(clrGroupName, false);
		}
	}
	else
		MessageBox::Show( "请选择场景地表或者场景地表结点" );
}

System::Void CTileViewCtrl::PlantHideGroup_button2_Click( System::Object^ sender, System::EventArgs^ e )
{
	if( this->nUnitTreeSelectImageInex == 1  )
	{
		String^clrGroupName = this->tileGroupName_textBox->Text;
		std::string strGroupName = MagStringToUnmagString(clrGroupName);
		
		CTileViewCtrlImp *pImp = NULL;
		pImp = (CTileViewCtrlImp*)m_pCtrl;
		pImp->CreateResSetGroup(strGroupName, true, this->groupType_comboBox->SelectedIndex, true);

		this->CreateResSetGroupNode(clrGroupName, true);
	}
}

System::Void CTileViewCtrl::deleteGroup_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	if( this->nUnitTreeSelectImageInex == 6 || this->nUnitTreeSelectImageInex == 7 )
	{
		String^clrGroupName = this->unit_treeView->SelectedNode->Text;
		std::string strGroupName = MagStringToUnmagString(clrGroupName);

		CTileViewCtrlImp *pImp = NULL;
		pImp = (CTileViewCtrlImp*)m_pCtrl;
		if( pImp->DeleteReSetGroup(strGroupName, bIs3D) )
		{
			this->unit_treeView->Nodes->Remove(this->unit_treeView->SelectedNode);
		}
	}
	else
		MessageBox::Show( "请选择需要删除的具体的图素组包文件" );
}

System::Void CTileViewCtrl::bindGroupAni_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	if( this->nUnitTreeSelectImageInex == 6 || this->nUnitTreeSelectImageInex == 7 )
	{
		openFileDialog->Title			 = "选择场景物件";
		openFileDialog->Filter			 = "AGP文件|*.agp||";
		openFileDialog->Multiselect		 = false;
		openFileDialog->RestoreDirectory = true;

		if( openFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK )
		{
			String^ clrAgpName = openFileDialog->FileName;
			clrAgpName = clrAgpName->ToLower();
			string  strAgpName = MagStringToUnmagString(clrAgpName);

			String^clrGroupName = this->unit_treeView->SelectedNode->Text;
			std::string strGroupName = MagStringToUnmagString(clrGroupName);

			CTileViewCtrlImp *pImp = NULL;
			pImp = (CTileViewCtrlImp*)m_pCtrl;
			pImp->BindGroupAnimationGroup(strGroupName, strAgpName);
		}
	}
	else
		MessageBox::Show( "请选择需要整体绑定动画图素组包文件" );
}

System::Void CTileViewCtrl::unit_treeView_AfterSelect( System::Object^ sender, System::Windows::Forms::TreeViewEventArgs^ e )
{
	TreeNode ^seleceNode = this->unit_treeView->SelectedNode;
	this->nUnitTreeSelectImageInex = seleceNode->ImageIndex;
	if( this->nUnitTreeSelectImageInex == 8 || this->nUnitTreeSelectImageInex == 5 )
	{
		this->SelectGroupNode();
	}
	else
	{
		if( this->nUnitTreeSelectImageInex == 6 || this->nUnitTreeSelectImageInex == 7 )
		{
			this->SelectGroupUnit();
		}
		else
		{
			if( this->nUnitTreeSelectImageInex == 10 )
			{
				this->SelectGroupMultiUnit();
			}
			else
			{
				if( this->nUnitTreeSelectImageInex == 11 )
				{

				}
			}
		}
	}
}

System::Void CTileViewCtrl::createUnit_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	if( this->nUnitTreeSelectImageInex == 6 )
	{
		if( bIs3D )
		{
			//选择场景物件图素unit
			this->CreatTerrainSingleModelUnit();
		}
		else
		{
			//选择场景地表unit
			this->CreatTerrainTextureUnit();
		}
	}
	else
		MessageBox::Show( "请在图素包列表中选中具体的图素包文件" );
}

System::Void CTileViewCtrl::createSubUnits_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	if( this->nUnitTreeSelectImageInex == 6 )
	{
		this->CreatTerraiMultiModelUnit();
	}
	else
		MessageBox::Show( "请在图素包列表中选中具体的图素包文件" );
}

System::Void CTileViewCtrl::deleteUnit_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	if( this->nUnitTreeSelectImageInex == 5 || this->nUnitTreeSelectImageInex == 8 )
	{
		TreeNode^ pGroupNode = this->unit_treeView->SelectedNode->Parent;
		String^clrGroupName = pGroupNode->Text;
		std::string strGroupName = MagStringToUnmagString(clrGroupName);

		TreeNode^ pUnitNode = this->unit_treeView->SelectedNode;
		String^clrUnitName = pUnitNode->Text;
		std::string strUnitName = MagStringToUnmagString(clrUnitName);

		CTileViewCtrlImp *pImp = NULL;
		pImp = (CTileViewCtrlImp*)m_pCtrl;
		if( pImp->DeleteGroupUnit(strGroupName, strUnitName, bIs3D) )
		{
			this->unit_treeView->Nodes->Remove(pUnitNode);
		}
	}
	else
		MessageBox::Show( "请选择需要删除的图素" );
}

System::Void CTileViewCtrl::bindUnitAni_button3_Click( System::Object^ sender, System::EventArgs^ e )
{
	if( this->nUnitTreeSelectImageInex == 5 )
	{
		openFileDialog->Title			 = "选择场景物件";
		openFileDialog->Filter			 = "AGP文件|*.agp||";
		openFileDialog->Multiselect		 = false;
		openFileDialog->RestoreDirectory = true;

		if( openFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK )
		{
			String^ clrAgpName = openFileDialog->FileName;
			clrAgpName = clrAgpName->ToLower();
			string  strAgpName = MagStringToUnmagString(clrAgpName);

			TreeNode^ pUnitNode = this->unit_treeView->SelectedNode;
			String^clrUnitName = pUnitNode->Text;
			std::string strUnitName = MagStringToUnmagString(clrUnitName);

			CTileViewCtrlImp *pImp = NULL;
			pImp = (CTileViewCtrlImp*)m_pCtrl;
			pImp->BindUnitAnimationGroup(strUnitName, strAgpName);
		}
	}
	else
		MessageBox::Show( "请选择需要绑定动作的图素" );
}

System::Void CTileViewCtrl::unbindUnitAni_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	if( this->nUnitTreeSelectImageInex == 5 )
	{
		TreeNode^ pUnitNode = this->unit_treeView->SelectedNode;
		String^clrUnitName = pUnitNode->Text;
		std::string strUnitName = MagStringToUnmagString(clrUnitName);

		CTileViewCtrlImp *pImp = NULL;
		pImp = (CTileViewCtrlImp*)m_pCtrl;
		pImp->BindUnitAnimationGroup(strUnitName, "");
	}
	else
		MessageBox::Show( "请选择需要解除动画的图素" );
}

System::Void CTileViewCtrl::unbindUnitEffect_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	if( this->nUnitTreeSelectImageInex == 5 )
	{
		String^clrUnitName = this->unit_treeView->SelectedNode->Text;
		std::string strUnitName = MagStringToUnmagString(clrUnitName);

		CTileViewCtrlImp *pImp = NULL;
		pImp = (CTileViewCtrlImp*)m_pCtrl;
		pImp->BindUnitEffectName(strUnitName, "");
	}
	else
		MessageBox::Show( "请选择需要删除绑定特效的图素" );
}

System::Void CTileViewCtrl::cueMat_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
{
	String^ clrMatName = this->cueMat_comboBox->Text;
	string  strMatName = MagStringToUnmagString(clrMatName);

	CTileViewCtrlImp *pImp = NULL;
	pImp = (CTileViewCtrlImp*)m_pCtrl;
	int cueIndex = pImp->GetMaterialIndex(strMatName);
	this->cueMatIndex_textBox->Text = cueIndex.ToString();
}

System::Void CTileViewCtrl::setMat_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	string	strMaterialName;
	String^ clrMaterialName = this->cueMat_comboBox->Text;
	strMaterialName = MagStringToUnmagString(clrMaterialName);

	if( this->nUnitTreeSelectImageInex == 8 || this->nUnitTreeSelectImageInex == 6 )
	{
		CTileViewCtrlImp *pImp = NULL;
		pImp = (CTileViewCtrlImp*)m_pCtrl;

		if( this->nUnitTreeSelectImageInex == 8 )
		{
			String^clrUnitName = this->unit_treeView->SelectedNode->Text;
			std::string strUnitName = MagStringToUnmagString(clrUnitName);

			pImp->SetUnitMaterialCueIndex(strUnitName, strMaterialName);
		}
		else if( this->nUnitTreeSelectImageInex == 6 )
		{
			TreeNode^ pGroupNode = this->unit_treeView->SelectedNode;
			String^clrGroupName = pGroupNode->Text;
			std::string strGroupName = MagStringToUnmagString(clrGroupName);
			pImp->SetGroupMaterialCueIndex(strGroupName, strMaterialName);
		}
	}
	else
		MessageBox::Show( "请选择需要设置地表音效材质的图素或图素包" );
}

void CTileViewCtrl::InitCueMaterialCombox()
{
 	CTileViewCtrlImp *pImp = NULL;
 	pImp = (CTileViewCtrlImp*)m_pCtrl;
 	
 	vector<string> names;
 	pImp->LoadTerrainMaterialConfig(names);
 
 	size_t size = names.size();
 	for ( size_t m = 0; m < size; ++m )
 	{
 		String ^clrName = gcnew String(names[m].c_str());
 		this->cueMat_comboBox->Items->Add(clrName);
 	}
 
 	if( !names.empty() )
 	{
 		string firstMatName = names[0];
 		int  cueIndex = pImp->GetMaterialIndex(firstMatName);
 		this->cueMatIndex_textBox->Text = cueIndex.ToString();
 		this->cueMat_comboBox->SelectedIndex = 0;
 	}
}

void CTileViewCtrl::CreateTreeRoot()
{
	this->unit_treeView->Nodes->Clear();

	TreeNode^ pParentNode = gcnew TreeNode;
	pParentNode->Text = this->resetName_textBox->Text;
	pParentNode->ImageIndex = 0;
	pParentNode->SelectedImageIndex = 1;
	this->unit_treeView->Nodes->Add(pParentNode);
	
	TreeNode ^unit2DNode = gcnew TreeNode;
	unit2DNode->Text = "场景地表";
	unit2DNode->ImageIndex = 2;
	unit2DNode->SelectedImageIndex = 3;
	pParentNode->Nodes->Add(unit2DNode);

	TreeNode ^unit3DNode = gcnew TreeNode;
	unit3DNode->Text = "场景物件";
	unit3DNode->ImageIndex = 1;
	pParentNode->Nodes->Add(unit3DNode);

	pParentNode->Expand();
}

//selectimage = 6表示选择的是图素group，
void CTileViewCtrl::ResetUnitsTree()
{
	CTileViewCtrlImp *pImp = NULL;
	pImp = (CTileViewCtrlImp*)m_pCtrl;
 	if( pImp )
 	{
 		this->unit_treeView->Nodes->Clear();
 
 		TreeNode^ pParentNode = gcnew TreeNode;
 		pParentNode->Text = this->resetName_textBox->Text;
 		pParentNode->ImageIndex = 0;
 		pParentNode->SelectedImageIndex = 1;
 		this->unit_treeView->Nodes->Add(pParentNode);
 
 		TreeNode ^unit2DNode = gcnew TreeNode;
 		unit2DNode->Text = "场景地表";
 		unit2DNode->ImageIndex = 2;
 		unit2DNode->SelectedImageIndex = 3;
 		pParentNode->Nodes->Add(unit2DNode);
 
 		TreeNode ^unit3DNode = gcnew TreeNode;
 		unit3DNode->Text = "场景物件";
 		unit3DNode->ImageIndex = 1;
 		pParentNode->Nodes->Add(unit3DNode);
 
 		//3D
 		size_t group3DSize = pImp->GetReset3DGroupCount();
 		for ( size_t m = 0; m < group3DSize; ++m )
 		{
 			string strGroupName = "";
			pImp->GetReset3DGroupNameByIndex(m, strGroupName);
 			String^clrGroupName = gcnew String(strGroupName.c_str());
 
 			TreeNode^ p3DGroupNode = gcnew TreeNode;
 			p3DGroupNode->Text = clrGroupName;
 			p3DGroupNode->ImageIndex = 6;
 			p3DGroupNode->SelectedImageIndex = 7;
 			unit3DNode->Nodes->Add(p3DGroupNode);
 
 			size_t rsSetUninSize = pImp->GetReset3DGroupUnitCount(m);
 			for ( size_t n = 0; n < rsSetUninSize; ++n )
 			{
 				string strUnitName = "";
				pImp->GetReset3DUnitNameByIndex(m, n, strUnitName);
 
 				String^clrUnitName = gcnew String(strUnitName.c_str());
 				TreeNode^ p3DUnitNode = gcnew TreeNode;
 				p3DUnitNode->Text = clrUnitName;
 				p3DUnitNode->ImageIndex = 5;
 				p3DUnitNode->SelectedImageIndex = 7;
 				p3DGroupNode->Nodes->Add(p3DUnitNode);
 			}
 		}
 
 		//2D
 		size_t group2DSize = pImp->GetReset2DGroupCount();
 		for ( size_t m = 0; m < group2DSize; ++m )
		{
 			string strGroupName = "";
			pImp->GetReset2DGroupNameByIndex(m, strGroupName);
 			String^clrGroupName = gcnew String(strGroupName.c_str());
 
 			TreeNode^ p2DGroupNode = gcnew TreeNode;
 			p2DGroupNode->Text = clrGroupName;
 			p2DGroupNode->ImageIndex = 6;
 			p2DGroupNode->SelectedImageIndex = 7;
 			unit2DNode->Nodes->Add(p2DGroupNode);
 
 			size_t rsSetUninSize = pImp->GetReset2DGroupUnitCount(m);
 			for ( size_t n = 0; n < rsSetUninSize; ++n )
 			{
				string strUnitName = "";
				pImp->GetReset2DUnitNameByIndex(m, n, strUnitName);
 
 				String^clrUnitName = gcnew String(strUnitName.c_str());
 				TreeNode^ p2DUnitNode = gcnew TreeNode(clrUnitName, 8, 7);
 				p2DUnitNode->Text = clrUnitName;
 				p2DUnitNode->ImageIndex = 8;
 				p2DUnitNode->SelectedImageIndex = 7;
 				p2DGroupNode->Nodes->Add(p2DUnitNode);
 			}
 		}
 
 		pParentNode->Expand();
 		unit2DNode->Expand();
 		unit3DNode->Expand();
 	}
}

void CTileViewCtrl::CreateResGroup( const BYTE bResGroupMark, const bool bVisibleByConfigue )
{
	if ( this->tileGroupName_textBox->Text == "" )
	{
		MessageBox::Show( "新创建模型组名字不能为空" );
		return;
	}
}

void CTileViewCtrl::CreatTerrainSingleModelUnit(  )
{
	CTileViewCtrlImp *pImp = NULL;
	pImp = (CTileViewCtrlImp*)m_pCtrl;
	if( !pImp )
		return;

	openFileDialog->Title			 = "选择场景物件";
	openFileDialog->Filter			 = "MOD文件|*.mod||";
	openFileDialog->Multiselect		 = true;
	openFileDialog->RestoreDirectory = true;

	if( openFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK )
	{
		TreeNode^ pGroupNode = this->unit_treeView->SelectedNode;
		String^clrGroupName = pGroupNode->Text;
		std::string strGroupName = MagStringToUnmagString(clrGroupName);

		array<String^>^ resultFiles = openFileDialog->FileNames;
		int nLen = resultFiles->Length;

		for ( int i = 0; i < nLen; ++i )
		{
			String^ clrUnitName = resultFiles[i];
			clrUnitName = clrUnitName->ToLower();
			string  strUnitName = MagStringToUnmagString(clrUnitName);
			string itemName = "";

			//////////////////////////////////////////////////////////////////////////
			CTileViewCtrlImp *pImp = NULL;
			pImp = (CTileViewCtrlImp*)m_pCtrl;
			if( pImp->CreateModelUnit(strGroupName, strUnitName, itemName) )
			{
				TreeNode^ p3DUnitNode = gcnew TreeNode;
				p3DUnitNode->Text = gcnew String(itemName.c_str());
				p3DUnitNode->ImageIndex = 5;
				p3DUnitNode->SelectedImageIndex = 7;

				pGroupNode->Nodes->Add(p3DUnitNode);
			}

			pGroupNode->Expand();
		}
	}
}

void CTileViewCtrl::CreatTerraiMultiModelUnit()
{
	openFileDialog->Title			 = "选择场景物件";
	openFileDialog->Filter			 = "MOD文件|*.mod||";
	openFileDialog->Multiselect		 = true;
	openFileDialog->RestoreDirectory = true;

	if( openFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK )
	{
		TreeNode^ pGroupNode = this->unit_treeView->SelectedNode;
		String^clrGroupName = pGroupNode->Text;
		std::string strGroupName = MagStringToUnmagString(clrGroupName);

		array<String^>^ resultFiles = openFileDialog->FileNames;
		int nLen = resultFiles->Length;
		if( nLen == 0 )
			return;

		vector<string> modelUnits;
		for ( int i = 0; i < nLen; ++i )
		{
			String^ clrUnitName = resultFiles[i];
			clrUnitName = clrUnitName->ToLower();
			string  strUnitName = MagStringToUnmagString(clrUnitName);
			
			modelUnits.push_back(strUnitName);
		}

		//////////////////////////////////////////////////////////////////////////
		string itemName = "";
		CTileViewCtrlImp *pImp = NULL;
		pImp = (CTileViewCtrlImp*)m_pCtrl;
		if( pImp->CreateModelUnits(strGroupName, modelUnits, itemName) )
		{
			TreeNode^ p3DMultiUnitNode = gcnew TreeNode;
			p3DMultiUnitNode->Text = gcnew String(itemName.c_str());;
			p3DMultiUnitNode->ImageIndex = 10;
			pGroupNode->Nodes->Add(p3DMultiUnitNode);

			size_t size = modelUnits.size();
			for ( size_t m = 0; m < size; ++m )
			{
				TreeNode^ p3DUnitNode = gcnew TreeNode;
				pImp->MakePathToText(modelUnits[m], itemName);
				p3DUnitNode->Text = gcnew String(itemName.c_str());
				p3DUnitNode->ImageIndex = 11;
				
				p3DMultiUnitNode->Nodes->Add(p3DUnitNode);
			}
		}

		pGroupNode->Expand();
	}
}

void CTileViewCtrl::CreatTerrainTextureUnit( )
{
	openFileDialog->Title			 = "选择场景地表纹理";
	openFileDialog->Filter			 = "BMP文件|*.bmp;*dds||";
	openFileDialog->Multiselect		 = true;
	openFileDialog->RestoreDirectory = true;

	if( openFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK )
	{
		TreeNode^ pGroupNode = this->unit_treeView->SelectedNode;
		String^clrGroupName = pGroupNode->Text;
		std::string strGroupName = MagStringToUnmagString(clrGroupName);

		array<String^>^ resultFiles = openFileDialog->FileNames;
		int nLen = resultFiles->Length;

		for ( int i = 0; i < nLen; ++i )
		{
			String^ clrUnitName = resultFiles[i];
			clrUnitName = clrUnitName->ToLower();
			string  strUnitName = MagStringToUnmagString(clrUnitName);

			//////////////////////////////////////////////////////////////////////////
			string itemName;
			CTileViewCtrlImp *pImp = NULL;
			pImp = (CTileViewCtrlImp*)m_pCtrl;
			if( pImp->CreateTextureUnit(strGroupName, strUnitName, itemName) )
			{
				String^clrUnitName = gcnew String(itemName.c_str());
				TreeNode^ p2DUnitNode = gcnew TreeNode;
				p2DUnitNode->Text = clrUnitName;
				p2DUnitNode->ImageIndex = 8;
				p2DUnitNode->SelectedImageIndex = 7;
				pGroupNode->Nodes->Add(p2DUnitNode);
			}
		}

		pGroupNode->Expand();
	}
}

void CTileViewCtrl::CreateResSetGroupNode( String^ clrGroupName, bool mark )
{
	TreeNode^ pGroupNode = gcnew TreeNode();
	pGroupNode->Text = clrGroupName;
	pGroupNode->ImageIndex = 6;
	pGroupNode->SelectedImageIndex = 7;
	this->unit_treeView->SelectedNode->Nodes->Add(pGroupNode);
	this->unit_treeView->SelectedNode->Expand();
}

System::Void CTileViewCtrl::CTileViewCtrl_VisibleChanged( System::Object^ sender, System::EventArgs^ e )
{
	CTileViewCtrlImp *pImp = NULL;
	pImp = (CTileViewCtrlImp*)m_pCtrl;
	if( pImp )
	{
		bool bShow = this->Visible;
		static bool bFirst = true;
		if( bShow && bFirst )
		{
			this->InitCueMaterialCombox();
			bFirst = false;
		}
	}
}

void CTileViewCtrl::Update( void )
{
	if( this->nUnitTreeSelectImageInex == 5 )
	{
		CTileViewCtrlImp *pImp = NULL;
		pImp = (CTileViewCtrlImp*)m_pCtrl;
		if( pImp )
		{
			CMapEffectCtrlImp* pMapEffectImp = NULL;
			pMapEffectImp = (CMapEffectCtrlImp*)(this->cMapEffectCtrl->GetCoreCtrl());
			if( pMapEffectImp )
			{
				string effectname;
				pMapEffectImp->GetActiveEffectName(effectname);
				
				String^clrUnitName = this->unit_treeView->SelectedNode->Text;
				std::string strUnitName = MagStringToUnmagString(clrUnitName);
				pImp->BindUnitEffectName(strUnitName, effectname);
			}
		}
	}
	else
		MessageBox::Show( "请选择需要设置特效的物件图素" );
}

char* CTileViewCtrl::GetImpTypeName( void )
{
	return GET_IMP_NAME(CTileViewCtrlImp);
}

void* CTileViewCtrl::GetImpParam( void )
{
	return this->Handle.ToPointer();
}	

System::Void CTileViewCtrl::CTileViewCtrl_Load( System::Object^ sender, System::EventArgs^ e )
{
	CTileViewCtrlImp *pImp = NULL;
	pImp = (CTileViewCtrlImp*)m_pCtrl;
	if( pImp )
	{
		this->groupType_comboBox->SelectedIndex = 0;

		CContextCtrlImp* pContextImp = (CContextCtrlImp*)this->cContextComponent->GetCoreCtrl();
		CMapEffectCtrlImp* pMapEffectImp = NULL;
		pMapEffectImp = (CMapEffectCtrlImp*)(this->cMapEffectCtrl->GetCoreCtrl());
		if( pMapEffectImp )
		{
			pImp->SetMapEffectCtrlImp(pMapEffectImp);
			//pMapEffectImp->SetContext(pContext);
		}
		pImp->SetImpContext(pContextImp->GetContext());
	}
}	

void CTileViewCtrl::SelectGroupNode()
{
	TreeNode ^seleceNode = this->unit_treeView->SelectedNode;
	bool b3D = true;
	if( this->nUnitTreeSelectImageInex == 8 )
	{
		b3D = false;
	}
	else
	{
		if( this->nUnitTreeSelectImageInex == 5 )
		{
			b3D = true;
		}
	}

	TreeNode^ pGroupNode = seleceNode->Parent;
	String^clrGroupName = pGroupNode->Text;
	std::string strGroupName = 	MagStringToUnmagString(clrGroupName);

	String^clrUnitName = seleceNode->Text;
	std::string strUnitName = MagStringToUnmagString(clrUnitName);

	String^		clrResetName = this->resetName_textBox->Text;
	if( clrResetName == "" )
		return;
	std::string strResetName = MagStringToUnmagString(clrResetName);

	CTileViewCtrlImp *pImp = NULL;
	pImp = (CTileViewCtrlImp*)m_pCtrl;
	pImp->SelectActiveUnit(strResetName, strGroupName, strUnitName, b3D, true);

	if( !b3D )
	{
		int nMatIndex = pImp->GetUnitMaterialIndex(strUnitName);
		this->selectMatCueIndex_textBox->Text = nMatIndex.ToString();
	}
}

void CTileViewCtrl::SelectGroupUnit()
{
	TreeNode ^seleceNode = this->unit_treeView->SelectedNode;
	String^ strNodeText = this->unit_treeView->SelectedNode->Parent->Text;
	if( strNodeText == "场景地表" )
		bIs3D = false;
	else
	{
		if( strNodeText == "场景物件" )
			bIs3D = true;
	}

	String^clrGroupName = seleceNode->Text;
	std::string strGroupName = MagStringToUnmagString(clrGroupName);

	String^		clrResetName = this->resetName_textBox->Text;
	if( clrResetName == "" )
		return;

	std::string strResetName = MagStringToUnmagString(clrResetName);

	CTileViewCtrlImp *pImp = NULL;
	pImp = (CTileViewCtrlImp*)m_pCtrl;
	pImp->SelectActiveUnit(strResetName, strGroupName, "", bIs3D, false);
	seleceNode->Expand();

	if( this->lastSelectGroupNode )
	{
		this->lastSelectGroupNode->Collapse();
	}

	lastSelectGroupNode = this->unit_treeView->SelectedNode;
}

void CTileViewCtrl::SelectGroupMultiUnit()
{
	TreeNode ^seleceNode = this->unit_treeView->SelectedNode;
	if( seleceNode )
	{
		TreeNode^ pGroupNode = seleceNode->Parent;
		String^clrGroupName = pGroupNode->Text;
		std::string strGroupName = MagStringToUnmagString(clrGroupName);

		String^clrUnitName = seleceNode->Text;
		std::string strUnitName = MagStringToUnmagString(clrUnitName);

		String^		clrResetName = this->resetName_textBox->Text;
		if( clrResetName == "" )
			return;
		std::string strResetName = MagStringToUnmagString(clrResetName);

		CTileViewCtrlImp *pImp = NULL;
		pImp = (CTileViewCtrlImp*)m_pCtrl;
		pImp->SelectActiveUnit(strResetName, strGroupName, strUnitName, true, true);
	}
}