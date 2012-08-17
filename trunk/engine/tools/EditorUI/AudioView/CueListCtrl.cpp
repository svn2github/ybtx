#include "StdAfx.h"
#include "CueListCtrl.h"
#include "AudioViewCtrl/CueListCtrlImp.h"
#include "dthelp.h"

void CueListCtrl::Update( void )
{
	CCueListCtrlImp *pImp = NULL;
	pImp = (CCueListCtrlImp*)m_pCtrl;
	if( pImp )
	{
	}
}

char* CueListCtrl::GetImpTypeName( void )
{
	return GET_IMP_NAME(CCueListCtrlImp);
}

void* CueListCtrl::GetImpParam( void )
{
	return this->Handle.ToPointer();
}

void CueListCtrl::InitCueTreeView()
{
	CCueListCtrlImp *pImp = (CCueListCtrlImp*)m_pCtrl;
	if( pImp )
	{
		pImp->LoadMusicResource();

		TreeNode^ pParentNode = gcnew TreeNode;
		pParentNode->Text = gcnew String("Audio");
		pParentNode->ImageIndex = 0;
		this->cue_treeView->Nodes->Add(pParentNode);

		size_t nMusicTypeCount = pImp->GetMusicTypeCount();
		for ( size_t m = 0; m < nMusicTypeCount; ++m )
		{
			string strMuiscTypeName;
			pImp->GetMusicTypeName(m, strMuiscTypeName);

			TreeNode^ pMusicTypeNode = gcnew TreeNode;
			pMusicTypeNode->Text = gcnew String(strMuiscTypeName.c_str());
			pMusicTypeNode->ImageIndex = 1;
			pParentNode->Nodes->Add(pMusicTypeNode);

			size_t nCueCount = pImp->GetEachMusicTypeCount(m);
			for ( size_t n = 0; n < nCueCount; ++n )
			{
				string strCueName;
				pImp->GetCueName(strMuiscTypeName, n, strCueName);

				TreeNode^ pCueNode = gcnew TreeNode;
				pCueNode->Text = gcnew String(strCueName.c_str());
				pCueNode->ImageIndex = 2;
				pMusicTypeNode->Nodes->Add(pCueNode);
			}
		}

		pParentNode->Expand();
	}
}

void CueListCtrl::SetImpCueName(System::String^ str)
{
	if (str)
	{
		string  strCueName = MagStringToUnmagString(str);

		CCueListCtrlImp *pImp = (CCueListCtrlImp*)m_pCtrl;
		pImp->SetCurSelectCueName(strCueName);
	}
}

const char*	CueListCtrl::GetImpCueName()
{
	CCueListCtrlImp *pImp = (CCueListCtrlImp*)m_pCtrl;
	return pImp->GetCurSelectCueName();
}

System::Void CueListCtrl::cue_treeView_AfterSelect( System::Object^ sender, System::Windows::Forms::TreeViewEventArgs^ e )
{
	TreeNode ^seleceNode = this->cue_treeView->SelectedNode;
	if( seleceNode )
	{
		if( seleceNode->ImageIndex == 2 )
		{
			String^ clrCueName = seleceNode->Text;
			string  strCueName = MagStringToUnmagString(clrCueName);

			CCueListCtrlImp *pImp = (CCueListCtrlImp*)m_pCtrl;
			pImp->SetCurSelectCueName(strCueName);
		}
	}
}

System::Void sqr_tools::CueListCtrl::CueListCtrl_Load( System::Object^ sender, System::EventArgs^ e )
{
	InitCueTreeView();
}