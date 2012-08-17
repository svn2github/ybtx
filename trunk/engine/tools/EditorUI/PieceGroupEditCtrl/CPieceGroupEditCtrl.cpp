#include "StdAfx.h"
#include "CPieceGroupEditCtrl.h"
#include "PieceGroupEditCtrl/CPieceGroupEditImp.h"

#define PieceGroupEditImp ((CPieceGroupEditImp*)GetCoreCtrl())

public ref class CustomNode : TreeNode
{
private: 
	bool isReadOnly;
	bool isRoot;
	String^ strFullPath;

public: 
	CustomNode()
	{
	}

	CustomNode(String^ text, String^ fullPath, bool readOnly, bool root, int imageIndex, int selectedImageIndex) 
		: TreeNode(text, imageIndex, selectedImageIndex)
	{
		isReadOnly = readOnly;
		isRoot = root;
		strFullPath = fullPath;
	}

public:
	bool IsReadOnly()
	{
		return isReadOnly; 
	}

	bool IsRoot()
	{
		return isRoot;
	}

	String^ GetFullPath(void)
	{
		return strFullPath;
	}
};

char* CPieceGroupEditCtrl::GetImpTypeName( void )
{
	return GET_IMP_NAME(CPieceGroupEditImp);
}

void* CPieceGroupEditCtrl::GetImpParam( void )
{
	return this->Handle.ToPointer();
}

System::Void CPieceGroupEditCtrl::AddMesh( void )//System::Object^ sender, System::EventArgs^ e )
{
	if (PieceGroupEditImp)
	{
		String^ fullName = gcnew String(PieceGroupEditImp->GetLastModelFile().c_str());
		String^ pathName, ^fileName;
		int index = fullName->LastIndexOf("\\");
		if (-1 != index)
		{
			pathName = fullName->Substring(0, index);
			fileName = fullName->Substring(index+1);
		}

		String^ modelName = gcnew String("");

		OpenFileDialog^ openFileDialog = gcnew OpenFileDialog();
		openFileDialog->Filter = gcnew String("*.mod|*.mod");
		openFileDialog->Multiselect = true;
		openFileDialog->RestoreDirectory = false;
		if (-1 != index)
		{
			openFileDialog->InitialDirectory = pathName;
			openFileDialog->FileName = fileName;
		}
		if(openFileDialog->ShowDialog() == DialogResult::OK)
		{
			for ( int i = 0; i < openFileDialog->FileNames->Length; i++ )
			{
				FileAttributes fileAttri = File::GetAttributes(openFileDialog->FileNames[i]);
				bool value = bool(FileAttributes::ReadOnly & fileAttri);
				if ( value )
				{
					modelName += openFileDialog->FileNames[i] + "\n";
				}

				String^ strClr = openFileDialog->FileNames[i];
				std::string strCPlus = MagStringToUnmagString(strClr);

				STRINGVECTOR strVector;
				PieceGroupEditImp->AddPieceGroup(strCPlus, strVector);
				UpdateNode(strVector, bool(value));
			}

			if (String::Compare(openFileDialog->FileNames[0], fullName) != 0)
			{
				std::string strCPlus = MagStringToUnmagString(openFileDialog->FileNames[0]);
				PieceGroupEditImp->SaveNewModelFile(strCPlus);
			}
		}

		if (modelName->Length > 0)
		{
			modelName +="文件是只读的,只有svn获取修改权限才可以保存!!";
			MessageBox::Show(modelName, "提示", MessageBoxButtons::OK);
		}
		//默认选中第一个piecegroup
		if (this->PieceGroupTree->Nodes->Count)
		{
			this->PieceGroupTree->SelectedNode = this->PieceGroupTree->Nodes[0];
			TreeNode_DoubleClick(nullptr, nullptr);
		}
	}
}

System::Void sqr_tools::CPieceGroupEditCtrl::SaveMesh( void )
{
	if (PieceGroupEditImp)
	{
		if(!this->PieceGroupTree->SelectedNode)
			return;

		String^ strClr = ((CustomNode^)this->PieceGroupTree->SelectedNode)->GetFullPath();
		std::string strCPlus = MagStringToUnmagString(strClr);
		PieceGroupEditImp->SaveMesh(strCPlus);
	}
}

System::Void sqr_tools::CPieceGroupEditCtrl::SaveMeshAs( void )
{
	if (PieceGroupEditImp)
	{
		if(!this->PieceGroupTree->SelectedNode)
			return;

		FolderBrowserDialog^ folderDialog = gcnew FolderBrowserDialog;
		if (folderDialog->ShowDialog() == DialogResult::OK)
		{
			String^ folderName = folderDialog->SelectedPath;
			String^ nodeName = this->PieceGroupTree->SelectedNode->Text;
			String^ strClr = folderName + nodeName;
			std::string strCPlus = MagStringToUnmagString(strClr);
			PieceGroupEditImp->SaveMesh(strCPlus);
		}
	}
}

System::Void sqr_tools::CPieceGroupEditCtrl::SaveAllMesh( void )
{
	if (PieceGroupEditImp)
	{
		String^ modelName = gcnew String("");
		for (int i = 0; i < this->PieceGroupTree->Nodes->Count; i++)
		{
			CustomNode^ node = (CustomNode^)this->PieceGroupTree->Nodes[i];
			if (node->IsReadOnly())
			{
				modelName += node->Text + "\n";
			}
			else
			{
				String^ strClr = node->Text;
				std::string strCPlus = MagStringToUnmagString(strClr);
				PieceGroupEditImp->SelectChanged(strCPlus);
				strClr = node->GetFullPath();
				strCPlus = MagStringToUnmagString(strClr);
				PieceGroupEditImp->SaveMesh(strCPlus);
			}
		}

		if (modelName->Length > 0)
		{
			modelName +="文件是只读的,只有svn获取修改权限才可以保存!!";
			MessageBox::Show(modelName, "提示", MessageBoxButtons::OK);
		}
		else
			MessageBox::Show("全部保存成功!", "提示", MessageBoxButtons::OK);
	}
}

System::Void sqr_tools::CPieceGroupEditCtrl::ClearAllMesh( void )
{
	if (PieceGroupEditImp)
	{
		this->PieceGroupTree->Nodes->Clear();
		PieceGroupEditImp->DeleteAllGroup();
	}
}

void sqr_tools::CPieceGroupEditCtrl::Update(void)
{
}

System::Void sqr_tools::CPieceGroupEditCtrl::UpdateNode( std::vector<string> strVector, bool isReadOnly )
{
	CustomNode^ rootNode;
	STRINGVECTOR nameVector = strVector;
	for (unsigned int i = 0; i < nameVector.size(); i++)
	{
		if (!i)
		{
			String^ fullPath = gcnew String(nameVector[i].c_str());
			int index = fullPath->LastIndexOf("\\") + 1;
			String^ name = fullPath->Substring(index);
			rootNode = gcnew CustomNode(name, fullPath, isReadOnly, true, 0, 0);
			this->PieceGroupTree->Nodes->Add(rootNode);
		}
		else
		{
			String^ name = gcnew String(nameVector[i].c_str());		
			CustomNode^ childNode = gcnew CustomNode(name, "", false, false, 1, 1);
			rootNode->Nodes->Add(childNode);
		}
	}
}

System::Void sqr_tools::CPieceGroupEditCtrl::TreeNode_DoubleClick( System::Object^ sender, System::EventArgs^ e )
{
	if (PieceGroupEditImp)
	{
		String^ strClr = this->PieceGroupTree->SelectedNode->Text;
		std::string strCPlus = MagStringToUnmagString(strClr);
		PieceGroupEditImp->SelectChanged(strCPlus);
	}
}

System::Void sqr_tools::CPieceGroupEditCtrl::PieceGroupTree_BeforeExpand(System::Object^  sender, System::Windows::Forms::TreeViewCancelEventArgs^  e)
{
	if (m_bExpand)
		e->Cancel = false;
	else
		e->Cancel = true;
	m_bExpand = true;
}

System::Void sqr_tools::CPieceGroupEditCtrl::PieceGroupTree_BeforeCollapse(System::Object^  sender, System::Windows::Forms::TreeViewCancelEventArgs^  e)
{
	if (m_bExpand)
		e->Cancel = false;
	else
		e->Cancel = true;
	m_bExpand = true;
}

System::Void sqr_tools::CPieceGroupEditCtrl::PieceGroupTree_MouseDown( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
{
	m_bExpand = !(e->Clicks > 1);
	if( e->Button == System::Windows::Forms::MouseButtons::Right )
		this->PieceGroupTree->SelectedNode = nullptr;
}

System::Void sqr_tools::CPieceGroupEditCtrl::打开模型ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
{
	AddMesh();
}

System::Void sqr_tools::CPieceGroupEditCtrl::保存模型ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
{
	SaveMesh();
}

System::Void sqr_tools::CPieceGroupEditCtrl::保存模型为ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
{
	SaveMeshAs();
}

System::Void sqr_tools::CPieceGroupEditCtrl::保存全部ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
{
	SaveAllMesh();
}

System::Void sqr_tools::CPieceGroupEditCtrl::关闭所有模型ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
{
	ClearAllMesh();
}