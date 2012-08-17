#include "StdAfx.h"
#include "CBoneBoxListCtrl.h"
#include "BoneBoxListCtrl/CBoneBoxListCtrlImp.h"

#define BoneBoxListCtrlImp ((CBoneBoxListCtrlImp*)GetCoreCtrl())

char* sqr_tools::CBoneBoxListCtrl::GetImpTypeName( void )
{
	return GET_IMP_NAME(CBoneBoxListCtrlImp);
}

void* sqr_tools::CBoneBoxListCtrl::GetImpParam( void )
{
	return this->Handle.ToPointer();
}

void sqr_tools::CBoneBoxListCtrl::Update( void )
{
	if (BoneBoxListCtrlImp)
	{
		this->BoneBoxTreeView->Items->Clear();
		BoneBoxListCtrlImp->UpdateList();
		for (int i = 0; i < BoneBoxListCtrlImp->GetBoneNum(); i++)
		{
			std::string name = BoneBoxListCtrlImp->GetBoneNameByIndex(i);
			this->BoneBoxTreeView->Items->Add(gcnew String(name.c_str()));
		}
	}
}

ListViewItem^ sqr_tools::CBoneBoxListCtrl::FindNode( String^ value )
{
	for (int i = 0; i < this->BoneBoxTreeView->Items->Count; i++)
	{
		if (0 == String::Compare(this->BoneBoxTreeView->Items[i]->Text, value))
			return this->BoneBoxTreeView->Items[i];
	}
	return nullptr;
}

System::Void sqr_tools::CBoneBoxListCtrl::BoneBoxTreeView_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (BoneBoxListCtrlImp)
	{
		if (this->BoneBoxTreeView->FocusedItem)
		{
			std::string strCPlus = MagStringToUnmagString(this->BoneBoxTreeView->FocusedItem->Text);
			BoneBoxListCtrlImp->SetSelectedBoneName(strCPlus);
		}
		else
			BoneBoxListCtrlImp->SetSelectedBoneName("");
	}
}

System::Void sqr_tools::CBoneBoxListCtrl::删除ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
{
	if (BoneBoxListCtrlImp)
	{
		if (this->BoneBoxTreeView->SelectedItems->Count == 0)
			return;

		int count = this->BoneBoxTreeView->SelectedItems->Count;
		for (int i = 0; i < count; i++)
		{
			std::string strCPlus = MagStringToUnmagString(this->BoneBoxTreeView->SelectedItems[0]->Text);
			BoneBoxListCtrlImp->DelSkeletalBox(strCPlus);
			this->BoneBoxTreeView->Items->Remove(this->BoneBoxTreeView->SelectedItems[0]);
		}
	}
}

System::Void sqr_tools::CBoneBoxListCtrl::增加点选盒子ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
{
	if (BoneBoxListCtrlImp)
	{
		std::string result = BoneBoxListCtrlImp->AddPickBoneBox();
		if (!result.empty())
		{
			Update();
			String^ str = gcnew String(result.c_str());
			ListViewItem^ resultItem = FindNode(str);
			resultItem->Selected = true;
		}
	}
}

System::Void sqr_tools::CBoneBoxListCtrl::加载默认骨骼ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
{
	if (BoneBoxListCtrlImp)
	{
		BoneBoxListCtrlImp->LoadDefaultBone();
		Update();
	}
}

System::Void sqr_tools::CBoneBoxListCtrl::头骨包围盒ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
{
	if (BoneBoxListCtrlImp)
	{
		BoneBoxListCtrlImp->AddHead();
		Update();
	}
}