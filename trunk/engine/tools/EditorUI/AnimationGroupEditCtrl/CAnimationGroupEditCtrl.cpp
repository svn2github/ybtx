#include "StdAfx.h"
#include "CAnimationGroupEditCtrl.h"
#include "AnimationGroupEditCtrl/CAnimationGroupEditImp.h"
//#include "ContextMsg.h"

#define AnimationGroupEditImp ((CAnimationGroupEditImp*)GetCoreCtrl())

char* sqr_tools::CAnimationGroupEditCtrl::GetImpTypeName( void )
{
	return GET_IMP_NAME(CAnimationGroupEditImp);
}

void* sqr_tools::CAnimationGroupEditCtrl::GetImpParam( void )
{
	m_iLastValue = 0;
	return this->Handle.ToPointer();
}

System::Void sqr_tools::CAnimationGroupEditCtrl::AnimationGroupList_Click( System::Object^ sender, System::EventArgs^ e )
{

}

System::Void sqr_tools::CAnimationGroupEditCtrl::AnimationGroupList_AfterSelect( System::Object^ sender, System::Windows::Forms::TreeViewEventArgs^ e )
{
	if (AnimationGroupEditImp)
	{
		String^ strClr = this->AnimationGroupList->SelectedNode->Text;
		int index = strClr->IndexOf(",");
		strClr = strClr->Substring(0, index);
		std::string strCPlus = MagStringToUnmagString(strClr);
		AnimationGroupEditImp->ChangeAnimation(strCPlus);
	}
}

void sqr_tools::CAnimationGroupEditCtrl::Update( void )
{
	if (AnimationGroupEditImp)
	{
		this->AnimationGroupList->Nodes->Clear();

		for (int i = 0; i < AnimationGroupEditImp->GetCount(); i++)
		{
			std::string animName = AnimationGroupEditImp->GetAnimNameByIndex(i);
			this->AnimationGroupList->Nodes->Add(gcnew String(animName.c_str()));
		}
	}
}

System::Void sqr_tools::CAnimationGroupEditCtrl::加入ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
{
	if (AnimationGroupEditImp)
	{
		String^ fullName = gcnew String(AnimationGroupEditImp->GetLastAGPFile().c_str());
		String^ pathName, ^fileName;
		int index = fullName->LastIndexOf("\\");
		if (-1 != index)
		{
			pathName = fullName->Substring(0, index);
			fileName = fullName->Substring(index+1);
		}

		String^ AgpInfo = gcnew String("");
		OpenFileDialog^ openFileDialog = gcnew OpenFileDialog();
		openFileDialog->Filter = gcnew String("*.agp|*.agp|*.xml|*.xml");
		if (-1 != index)
		{
			openFileDialog->InitialDirectory = pathName;
			openFileDialog->FileName = fileName;
		}
		if(openFileDialog->ShowDialog() == DialogResult::OK)
		{
			FileAttributes fileAttri = File::GetAttributes(openFileDialog->FileName);
			bool value = bool(FileAttributes::ReadOnly & fileAttri);
			if ( value )
			{
				AgpInfo += openFileDialog->FileName + "\n";
			}

			String^ strClr = openFileDialog->FileName;
			std::string strCPlus = MagStringToUnmagString(strClr);

			String^ msg = gcnew String(AnimationGroupEditImp->AddAgp(strCPlus).c_str());
			MessageBox::Show(msg, "提示", MessageBoxButtons::OK);


			if (String::Compare(openFileDialog->FileName, fullName) != 0)
			{
				std::string strCPlus = MagStringToUnmagString(openFileDialog->FileName);
				AnimationGroupEditImp->SaveNewAGPFile(strCPlus);
			}

			this->AnimationGroupList->Nodes->Clear();

			for (int i = 0; i < AnimationGroupEditImp->GetCount(); i++)
			{
				std::string animName = AnimationGroupEditImp->GetAnimNameByIndex(i);
				this->AnimationGroupList->Nodes->Add(gcnew String(animName.c_str()));
			}
		}

		if (AgpInfo->Length > 0)
		{
			AgpInfo +="文件是只读的,只有svn获取修改权限才可以保存!!";
			MessageBox::Show(AgpInfo, "提示", MessageBoxButtons::OK);
		}
	}
}

System::Void sqr_tools::CAnimationGroupEditCtrl::清除agpToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
{
	if (AnimationGroupEditImp)
	{
		this->AnimationGroupList->Nodes->Clear();
		AnimationGroupEditImp->ClearAgp();
	}
}

System::Void sqr_tools::CAnimationGroupEditCtrl::保存agpToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
{
	if (AnimationGroupEditImp)
	{
		String^ AgpInfo = gcnew String("");
		SaveFileDialog^ openFileDialog = gcnew SaveFileDialog();
		openFileDialog->Filter = gcnew String("*.agp|*.agp|*.xml|*.xml");
		if(openFileDialog->ShowDialog() == DialogResult::OK)
		{
			if(File::Exists(openFileDialog->FileName))
			{
				FileAttributes fileAttri = File::GetAttributes(openFileDialog->FileName);
				bool value = bool(FileAttributes::ReadOnly & fileAttri);
				if ( value )
				{
					AgpInfo += openFileDialog->FileName + "\n";
				}
			}
		}

		if (AgpInfo->Length > 0)
		{
			AgpInfo +="文件是只读的,只有svn获取修改权限才可以保存!!";
			MessageBox::Show(AgpInfo, "提示", MessageBoxButtons::OK);
		}
		else
		{
			if (openFileDialog->FileName->Length == 0)
				return;

			String^ strClr = openFileDialog->FileName;
			std::string strCPlus = MagStringToUnmagString(strClr);
			String^ msg = gcnew String(AnimationGroupEditImp->SaveAgp(strCPlus).c_str());
			MessageBox::Show(msg, "提示", MessageBoxButtons::OK);
		}
	}
}

System::Void sqr_tools::CAnimationGroupEditCtrl::AnimationGroupList_MouseClick( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
{
	if( e->Button == System::Windows::Forms::MouseButtons::Right )
	{
		this->AnimationGroupList->SelectedNode = nullptr;
	}
}