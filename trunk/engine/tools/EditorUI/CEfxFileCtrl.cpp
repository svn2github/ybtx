#include "StdAfx.h"
#include "CEfxFileCtrlImp.h"
#include "CEfxFileCtrl.h"
#include "CLRStringHelper.h"

using namespace System::Text;
namespace sqr_tools
{
	char* CEfxFileCtrl::GetImpTypeName( void )
	{
		return GET_IMP_NAME(CEfxFileCtrlImp);
	}

	System::Void  CEfxFileCtrl::InitOtherComponent()
	{
		EfxFiletreeView->BeginUpdate();

		m_RootTreeNode = gcnew TreeNode("特效",0,0);
		EfxFiletreeView->Nodes->Add(m_RootTreeNode);

		EfxFiletreeView->EndUpdate();
	}

	System::Void  CEfxFileCtrl::DestroyOtherComponent()
	{
		EfxFiletreeView->BeginUpdate();
		EfxFiletreeView->CollapseAll();
		EfxFiletreeView->EndUpdate();
	}

	void CEfxFileCtrl::Update(void)
	{
		EfxFiletreeView->BeginUpdate();
		CEfxFileCtrlImp* EfxFileimp = (CEfxFileCtrlImp*)m_pCtrl;
		// 更新 tree
		EfxFiletreeView->Nodes->Clear();
		m_RootTreeNode = gcnew TreeNode("特效", 0, 0);
		EfxFiletreeView->Nodes->Add(m_RootTreeNode);
		
		TreeNode^ TempSelectedNode = m_RootTreeNode;
		bool TempBeSel = false;
		TempBeSel = EfxFileimp->IsSelectTreeNode(NULL, NULL);

		map<string, vector<string>> EffectName;
		EfxFileimp->GetEffectNames( EffectName );

		for( map<string, vector<string>>::iterator it = EffectName.begin(); it != EffectName.end(); ++it )
		{
			String^ clrForderName = gcnew String(it->first.c_str());
			TreeNode^ tempForder = gcnew TreeNode(clrForderName, 1, 2);
			m_RootTreeNode->Nodes->Add(tempForder);
			if (!TempBeSel)
			{
				TempBeSel = EfxFileimp->IsSelectTreeNode(it->first.c_str(), NULL);
				if (TempBeSel)
					TempSelectedNode = tempForder;
			}

			size_t size = it->second.size();
			for( size_t i = 0; i < size; ++i )
			{
				String^ clrEfxName = gcnew String(it->second[i].c_str());
				TreeNode^ tempEfxNode = gcnew TreeNode(clrEfxName, 1, 1);
				tempForder->Nodes->Add(tempEfxNode);
				if (!TempBeSel)
				{
					TempBeSel = EfxFileimp->IsSelectTreeNode(it->first.c_str(), it->second[i].c_str());
					if (TempBeSel)
						TempSelectedNode = tempForder;
				}
			}
		}

		EfxFiletreeView->EndUpdate();
		EfxFiletreeView->ExpandAll();

		if (TempBeSel)
			TempSelectedNode->Checked = true; 
	}

	System::Void CEfxFileCtrl::CreateButton_Click( System::Object^ sender, System::EventArgs^ e )
	{
		System::String^ creatText = NewNodeEditBox->Text;
		std::string strCPlus;
		CLRMarshalString(creatText, strCPlus);

		CEfxFileCtrlImp* EfxFileimp = (CEfxFileCtrlImp*)m_pCtrl;
		TreeNode^ TempSelectedNode = EfxFiletreeView->SelectedNode;
		uint32 succ = EfxFileimp->OnCreateNew( strCPlus.c_str() );
		// 直接更新显示，用刷新的话树的状态就都展开了
		if (succ == 1)
		{
			TreeNode^ tempForder = gcnew TreeNode(creatText, 1, 2);
			TempSelectedNode->Nodes->Add(tempForder);
			EfxFiletreeView->SelectedNode = tempForder;
		}
		else if(succ == 2)
		{
			TreeNode^ tempEffect = gcnew TreeNode(creatText, 1, 1);
			TempSelectedNode->Nodes->Add(tempEffect);
			EfxFiletreeView->SelectedNode = tempEffect;
			TempSelectedNode->ExpandAll();
		}
	}

	System::Void CEfxFileCtrl::OpenEfxFile_Click( System::Object^ sender, System::EventArgs^ e )
	{
		if (EfxOpenFileDialog->ShowDialog() == DialogResult::OK)
		{
			for ( int i = 0; i < EfxOpenFileDialog->FileNames->Length; ++i )
			{
				String^ strClr = EfxOpenFileDialog->FileNames[i];
				std::string strCPlus;
				CLRMarshalString(strClr, strCPlus);

				CEfxFileCtrlImp* EfxFileimp = (CEfxFileCtrlImp*)m_pCtrl;
				EfxFileimp->LoadEffectFile(strCPlus.c_str());
			}
		}
	}

	System::Void CEfxFileCtrl::OpenEfxFileFolder_Click( System::Object^ sender, System::EventArgs^ e )
	{
		// 导入特效文件夹
		if (EfxFolderBrowserDialog->ShowDialog() == DialogResult::OK)
		{
			// 清空
			ClearFillTree_Click(sender, e);
			// 加载
			String^ strClr = EfxFolderBrowserDialog->SelectedPath;
			std::string strCPlus;
			CLRMarshalString(strClr, strCPlus);

			CEfxFileCtrlImp* EfxFileimp = (CEfxFileCtrlImp*)m_pCtrl;
			EfxFileimp->LoadEffectForder(strCPlus.c_str());
		}
	}

	System::Void CEfxFileCtrl::UintTheEffect_Click( System::Object^ sender, System::EventArgs^ e )
	{

	}

	System::Void CEfxFileCtrl::ClearFillTree_Click( System::Object^ sender, System::EventArgs^ e )
	{
		CEfxFileCtrlImp* EfxFileimp = (CEfxFileCtrlImp*)m_pCtrl;
		EfxFileimp->ClearLoadEffect();
	}

	System::Void CEfxFileCtrl::SaveFile_Click( System::Object^ sender, System::EventArgs^ e )
	{
		CEfxFileCtrlImp* EfxFileimp = (CEfxFileCtrlImp*)m_pCtrl;
		if (EfxFileimp->IsSaveAs())
		{
			// 转调save as
			SaveFileAs_Click( sender, e );
		}
		else
			EfxFileimp->OnEffectSave();
	}

	System::Void CEfxFileCtrl::SaveFileAs_Click( System::Object^ sender, System::EventArgs^ e )
	{
		if (EfxSaveFileDialog->ShowDialog() == DialogResult::OK)
		{
			String^ strClr = EfxSaveFileDialog->FileName;
			std::string strCPlus;
			CLRMarshalString(strClr, strCPlus);

			CEfxFileCtrlImp* EfxFileimp = (CEfxFileCtrlImp*)m_pCtrl;
			EfxFileimp->OnEffectSaveAs(strCPlus.c_str());
		}
	}

	System::Void CEfxFileCtrl::EfxFiletreeView_DoubleClick( System::Object^ sender, System::EventArgs^ e )
	{
		SetSelectNode(true);
	}

	System::Void CEfxFileCtrl::EfxFiletreeView_AfterSelect( System::Object^ sender, System::Windows::Forms::TreeViewEventArgs^ e )
	{
		SetSelectNode(false);
	}

	System::Void CEfxFileCtrl::SetSelectNode(bool refresh)
	{
		CEfxFileCtrlImp* EfxFileimp = (CEfxFileCtrlImp*)m_pCtrl;
		TreeNode^ TempSelectedNode = EfxFiletreeView->SelectedNode;
		if (TempSelectedNode)
		{
			if (TempSelectedNode == m_RootTreeNode)
				EfxFileimp->SelectTreeNode(true, NULL, NULL);
			else if (TempSelectedNode->Parent == m_RootTreeNode)
			{
				System::String^ nodeText = TempSelectedNode->Text;
				std::string strCPlus;
				CLRMarshalString(nodeText, strCPlus);
				EfxFileimp->SelectTreeNode(false, strCPlus.c_str(), NULL);
			}
			else if (TempSelectedNode->Parent->Parent == m_RootTreeNode)
			{
				System::String^ forderText = TempSelectedNode->Parent->Text;
				System::String^ effectText = TempSelectedNode->Text;
				std::string strForderCPlus, strEffectCPlus;
				CLRMarshalString(forderText, strForderCPlus);
				CLRMarshalString(effectText, strEffectCPlus);
				EfxFileimp->SelectTreeNode(false, strForderCPlus.c_str(), strEffectCPlus.c_str());
			}
			if (refresh)
				EfxFileimp->ResetEditEffect();
		}
		
	}

	System::Void CEfxFileCtrl::EfxFiletreeView_KeyDown( System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e )
	{
		if(e->KeyValue == 46)	// delete pressed
		{
			CEfxFileCtrlImp* EfxFileimp = (CEfxFileCtrlImp*)m_pCtrl;
			EfxFileimp->DeleteCurSelectedNode();
		}
	}
}
