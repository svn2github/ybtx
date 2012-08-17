#include "StdAfx.h"
#include "ClrMsgDefine.h"
#include "CPieceUnitEditCtrl.h"
#include "PieceUnitEditCtrl\CPieceUnitEditImp.h"

#define PieceUnitEditImp ((CPieceUnitEditImp*)GetCoreCtrl())

char* sqr_tools::CPieceUnitEditCtrl::GetImpTypeName( void )
{
	return GET_IMP_NAME(CPieceUnitEditImp);
}

void* sqr_tools::CPieceUnitEditCtrl::GetImpParam( void )
{
	return this->Handle.ToPointer();
}

void sqr_tools::CPieceUnitEditCtrl::Update(void)
{
	UpdateNodes();
}

System::Void sqr_tools::CPieceUnitEditCtrl::UpdateNodes(void)
{
	if (PieceUnitEditImp)
	{
		this->PieceUnitTreeView->Nodes->Clear();

		CPieceUnitEditImp::PieceUnitInfoVector pieceUnitInfoVector = PieceUnitEditImp->GetPieceUnitInfo();
		CPieceUnitEditImp::PieceUnitInfoVector::iterator PieceUnitInfoVector_itr = pieceUnitInfoVector.begin();

		while (PieceUnitInfoVector_itr != pieceUnitInfoVector.end())
		{
			CPieceUnitEditImp::CPieceUnitInfo pieceUnitInfo = *PieceUnitInfoVector_itr;

			TreeNode^ rootNode = gcnew TreeNode(gcnew String(pieceUnitInfo.name.c_str()));
			rootNode->ImageIndex = 0;
			rootNode->SelectedImageIndex = 0;
			this->PieceUnitTreeView->Nodes->Add(rootNode);

			TreeNode^ RSNode = gcnew TreeNode(gcnew String(pieceUnitInfo.RS_Name.c_str()));
			RSNode->ImageIndex = 1;
			RSNode->SelectedImageIndex = 1;
			rootNode->Nodes->Add(RSNode);

			CPieceUnitEditImp::CPieceUnitInfo::RSMAP::iterator RSMAP_itr = pieceUnitInfo.RS_Names.begin();
			while( RSMAP_itr != pieceUnitInfo.RS_Names.end() )
			{
				TreeNode^ RSNameNode = gcnew TreeNode(gcnew String(RSMAP_itr->first.c_str()));
				RSNameNode->ImageIndex = 2;
				RSNameNode->SelectedImageIndex = 2;
				RSNode->Nodes->Add(RSNameNode);

				CPieceUnitEditImp::CPieceUnitInfo::STRVECTOR::iterator  STRVECTOR_itr = RSMAP_itr->second.begin();
				while( STRVECTOR_itr != RSMAP_itr->second.end() )
				{
					TreeNode^ RSTexNode = gcnew TreeNode(gcnew String((*STRVECTOR_itr).c_str()));
					RSTexNode->ImageIndex = 3;
					RSTexNode->SelectedImageIndex = 3;
					RSNameNode->Nodes->Add(RSTexNode);
					STRVECTOR_itr++;
				}

				RSMAP_itr++;
			}

			TreeNode^ VertexAniNode = gcnew TreeNode(gcnew String(pieceUnitInfo.Ani_Name.c_str()));
			VertexAniNode->ImageIndex = 4;
			VertexAniNode->SelectedImageIndex = 4;
			rootNode->Nodes->Add(VertexAniNode);

			CPieceUnitEditImp::CPieceUnitInfo::STRVECTOR::iterator STRVECTOR_itr = pieceUnitInfo.Ani_Names.begin();
			while ( STRVECTOR_itr != pieceUnitInfo.Ani_Names.end() )
			{
				TreeNode^ VertexAniNameNode = gcnew TreeNode(gcnew String((*STRVECTOR_itr).c_str()));
				VertexAniNameNode->ImageIndex = 5;
				VertexAniNameNode->SelectedImageIndex = 5;
				VertexAniNode->Nodes->Add(VertexAniNameNode);
				STRVECTOR_itr++;
			}

			PieceUnitInfoVector_itr++;
		}
	}
}

System::Void sqr_tools::CPieceUnitEditCtrl::TreeNode_AfterSelect( System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e )
{
	if (PieceUnitEditImp)
	{
		if ( 0 == this->PieceUnitTreeView->SelectedNode->ImageIndex )
		{
			std::string str_CPlus;
			String^ str_Clr = this->PieceUnitTreeView->SelectedNode->Text;
			str_CPlus = MagStringToUnmagString(str_Clr);
			PieceUnitEditImp->SelectChanged(str_CPlus);
		}
		else if ( 2 == this->PieceUnitTreeView->SelectedNode->ImageIndex )
		{
			PieceUnitEditImp->SelectChanged("");
			String^ rsName = this->PieceUnitTreeView->SelectedNode->Text;
			int sta_index = rsName->LastIndexOf("(") + 1;
			int end_index = rsName->LastIndexOf(")") - 1;
			rsName = rsName->Substring(sta_index, end_index - sta_index + 1);
		}
		else
		{
			PieceUnitEditImp->SelectChanged("");
		}
	}
}

System::Void sqr_tools::CPieceUnitEditCtrl::PieceUnitTreeView_KeyDown( System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e )
{
	if (PieceUnitEditImp)
	{
		if (this->PieceUnitTreeView->SelectedNode == nullptr)
		{
			if (0 == this->PieceUnitTreeView->Nodes->Count)
				return;

			this->PieceUnitTreeView->SelectedNode = this->PieceUnitTreeView->Nodes[this->PieceUnitTreeView->Nodes->Count - 1];
		}

		switch(e->KeyValue)
		{
		case Keys::Delete:
			{
				if ( 0 == this->PieceUnitTreeView->SelectedNode->ImageIndex )
				{
					String^ strClr = this->PieceUnitTreeView->SelectedNode->Text;
					std::string strCPlus =	MagStringToUnmagString(strClr);
					PieceUnitEditImp->DeletePiece(strCPlus);
					this->PieceUnitTreeView->Nodes->Remove(this->PieceUnitTreeView->SelectedNode);
				}
				else if ( 2 == this->PieceUnitTreeView->SelectedNode->ImageIndex )
				{
					int RSIndex;
					String^ strRSClr = this->PieceUnitTreeView->SelectedNode->Text;
					for (int i = 0; i < this->PieceUnitTreeView->SelectedNode->Parent->Nodes->Count; i++)
					{
						if (String::Compare(strRSClr, this->PieceUnitTreeView->SelectedNode->Parent->Nodes[i]->Text) == 0)
						{
							RSIndex = i;
							break;
						}
					}
					String^ strClr = this->PieceUnitTreeView->SelectedNode->Parent->Parent->Text;
					std::string  strCPlus = MagStringToUnmagString(strClr);
					PieceUnitEditImp->DeleteRS(strCPlus, RSIndex);
					this->PieceUnitTreeView->Nodes->Remove(this->PieceUnitTreeView->SelectedNode);
				}
			}
			break;
		}
	}
}

System::Void sqr_tools::CPieceUnitEditCtrl::PieceUnitTreeView_MouseDown( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
{
	if( e->Button == System::Windows::Forms::MouseButtons::Right )
	{
		TreeNode^ node = this->PieceUnitTreeView->GetNodeAt( e->X, e->Y );
		if (node)
		{
			this->增加ToolStripMenuItem->Enabled = false;
			this->删除ToolStripMenuItem->Enabled = false;
			this->PieceUnitTreeView->SelectedNode = node;
			if (0 == this->PieceUnitTreeView->SelectedNode->ImageIndex || 2 == this->PieceUnitTreeView->SelectedNode->ImageIndex)
			{
				this->删除ToolStripMenuItem->Enabled = true;
			} 
			else if (1 == this->PieceUnitTreeView->SelectedNode->ImageIndex)
			{
				this->增加ToolStripMenuItem->Enabled = true;
			}
		}
	}
}

System::Void sqr_tools::CPieceUnitEditCtrl::增加ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
{
	if (PieceUnitEditImp)
	{
		if (this->PieceUnitTreeView->SelectedNode == nullptr)
			return;

		OpenFileDialog^ openFileDialog = gcnew OpenFileDialog();
		if(openFileDialog->ShowDialog() == DialogResult::OK)
		{
			String^ strFullRSNameClr = openFileDialog->FileName;
			String^ strPieceNameClr = this->PieceUnitTreeView->SelectedNode->Parent->Text;
			std::string strPieceNameCPlus, strFullRSNameCPlus;
			strPieceNameCPlus = MagStringToUnmagString(strPieceNameClr);
			strFullRSNameCPlus = MagStringToUnmagString(strFullRSNameClr);
			PieceUnitEditImp->CreateRS(strPieceNameCPlus, strFullRSNameCPlus);
			TreeNode^ node = gcnew TreeNode(gcnew String(strFullRSNameCPlus.c_str()));
			node->ImageIndex = 2;
			node->SelectedImageIndex = 2;
			this->PieceUnitTreeView->SelectedNode->Nodes->Add(node);
		}
	}
}

System::Void sqr_tools::CPieceUnitEditCtrl::删除ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
{
	if (PieceUnitEditImp)
	{
		if (this->PieceUnitTreeView->SelectedNode == nullptr)
			return;

		if ( 0 == this->PieceUnitTreeView->SelectedNode->ImageIndex )
		{
			String^ strClr = this->PieceUnitTreeView->SelectedNode->Text;
			std::string strCPlus = MagStringToUnmagString(strClr);
			PieceUnitEditImp->DeletePiece(strCPlus);
			this->PieceUnitTreeView->Nodes->Remove(this->PieceUnitTreeView->SelectedNode);
		}
		else if ( 2 == this->PieceUnitTreeView->SelectedNode->ImageIndex )
		{
			int RSIndex;
			String^ strRSClr = this->PieceUnitTreeView->SelectedNode->Text;
			for (int i = 0; i < this->PieceUnitTreeView->SelectedNode->Parent->Nodes->Count; i++)
			{
				if (String::Compare(strRSClr, this->PieceUnitTreeView->SelectedNode->Parent->Nodes[i]->Text) == 0)
				{
					RSIndex = i;
					break;
				}
			}
			String^ strClr = this->PieceUnitTreeView->SelectedNode->Parent->Parent->Text;
			std::string  strCPlus = MagStringToUnmagString(strClr);
			PieceUnitEditImp->DeleteRS(strCPlus, RSIndex);
			this->PieceUnitTreeView->Nodes->Remove(this->PieceUnitTreeView->SelectedNode);
		}
	}
}

System::Void sqr_tools::CPieceUnitEditCtrl::PieceUnitTreeView_AfterLabelEdit( System::Object^ sender, System::Windows::Forms::NodeLabelEditEventArgs^ e )
{
	if (PieceUnitEditImp)
	{
		e->CancelEdit = true;
		if(this->PieceUnitTreeView->SelectedNode->ImageIndex != 2)
		{
			return;
		}

		int RSIndex;
		TreeNode^ ParentNode = this->PieceUnitTreeView->SelectedNode->Parent;
		TreeNode^ SelectedNode = this->PieceUnitTreeView->SelectedNode;
		String^ newRSName = e->Label;
		String^ oldRSName = e->Node->Text;

		if (!newRSName)
			return;

		int sta_index = oldRSName->LastIndexOf("(") + 1;
		int end_index = oldRSName->LastIndexOf(")") - 1;
		oldRSName = oldRSName->Substring(sta_index, end_index - sta_index + 1);

		for (int i = 0; i < ParentNode->Nodes->Count; i++)
		{
			if (ParentNode->Nodes[i] == SelectedNode)
			{
				RSIndex = i;
				continue;
			}

			String^ RSName = ParentNode->Nodes[i]->Text;
			sta_index = RSName->LastIndexOf("(") + 1;
			end_index = RSName->LastIndexOf(")") - 1;
			RSName = RSName->Substring(sta_index, end_index - sta_index + 1);

			if (String::Compare(newRSName, RSName) == 0)
			{
				MessageBox::Show("命名重复!", "提示", MessageBoxButtons::OK);
				return;
			}
		}

		String^ result = e->Node->Text->Replace(oldRSName, newRSName);
		this->PieceUnitTreeView->SelectedNode->Text = result;
		TreeNode^ PieceNode = this->PieceUnitTreeView->SelectedNode->Parent->Parent;
		std::string strPiece, strRSName;
		strPiece  = MagStringToUnmagString(PieceNode->Text);
		strRSName = MagStringToUnmagString(newRSName);
		PieceUnitEditImp->SetRSIndexName(strPiece, RSIndex, strRSName);
	}
}