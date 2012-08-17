#include "StdAfx.h"
#include "CBoneBoxEditCtrl.h"
#include "BoneBoxEditCtrl/CBoneBoxEditCtrlImp.h"

#define BoneBoxEditCtrlImp ((CBoneBoxEditCtrlImp*)GetCoreCtrl())

void* sqr_tools::CBoneBoxEditCtrl::GetImpParam( void )
{
	return this->Handle.ToPointer();
}

char* sqr_tools::CBoneBoxEditCtrl::GetImpTypeName( void )
{
	return GET_IMP_NAME(CBoneBoxEditCtrlImp);
}

void sqr_tools::CBoneBoxEditCtrl::Update( void )
{
	if (BoneBoxEditCtrlImp)
	{
		if (BoneBoxEditCtrlImp->GetUpdateList())
		{
			this->BoneListTreeView->Nodes->Clear();
			for (int i = 0; i < BoneBoxEditCtrlImp->GetBoxNum(); i++)
			{
				String^ str = gcnew String(BoneBoxEditCtrlImp->GetBoxNameByIndex(i).c_str());
				TreeNode^ node = gcnew TreeNode(str);
				this->BoneListTreeView->Nodes->Add(node);
			}
			BoneBoxEditCtrlImp->Reset();
		}

		//this->BoneListTreeView->Refresh();

		float max_x = 0.00000f,max_y = 0.00000f,max_z = 0.00000f,
			  min_x = 0.00000f,min_y = 0.00000f,min_z = 0.00000f;

		BoneBoxEditCtrlImp->GetBoxInfo(max_x, max_y, max_z, min_x, min_y, min_z);
		this->XMaxNumericUpDown->Text = Convert::ToString(max_x);
		this->YMaxNumericUpDown->Text = Convert::ToString(max_y);
		this->ZMaxNumericUpDown->Text = Convert::ToString(max_z);
		this->XMinNumericUpDown->Text = Convert::ToString(min_x);
		this->YMinNumericUpDown->Text = Convert::ToString(min_y);
		this->ZMinNumericUpDown->Text = Convert::ToString(min_z);
	}
}

System::Void sqr_tools::CBoneBoxEditCtrl::BoneListTreeView_AfterSelect( System::Object^ sender, System::Windows::Forms::TreeViewEventArgs^ e )
{
	if (BoneBoxEditCtrlImp)
	{
		std::string strCPlus = MagStringToUnmagString(this->BoneListTreeView->SelectedNode->Text);
		BoneBoxEditCtrlImp->NodeClick(strCPlus);
	}
}

System::Void sqr_tools::CBoneBoxEditCtrl::XMaxNumericUpDown_ValueChanged( System::Object^ sender, System::EventArgs^ e )
{
	if (BoneBoxEditCtrlImp)
	{
		float max_x,max_y,max_z,min_x,min_y,min_z;
		max_x = Convert::ToSingle(this->XMaxNumericUpDown->Value);
		max_y = Convert::ToSingle(this->YMaxNumericUpDown->Value);
		max_z = Convert::ToSingle(this->ZMaxNumericUpDown->Value);
		min_x = Convert::ToSingle(this->XMinNumericUpDown->Value);
		min_y = Convert::ToSingle(this->YMinNumericUpDown->Value);
		min_z = Convert::ToSingle(this->ZMinNumericUpDown->Value);
		BoneBoxEditCtrlImp->SetBoxInfo(max_x, max_y, max_z, min_x, min_y, min_z);
	}
}

System::Void sqr_tools::CBoneBoxEditCtrl::XMinNumericUpDown_ValueChanged( System::Object^ sender, System::EventArgs^ e )
{
	if (BoneBoxEditCtrlImp)
	{
		float max_x,max_y,max_z,min_x,min_y,min_z;
		max_x = Convert::ToSingle(this->XMaxNumericUpDown->Value);
		max_y = Convert::ToSingle(this->YMaxNumericUpDown->Value);
		max_z = Convert::ToSingle(this->ZMaxNumericUpDown->Value);
		min_x = Convert::ToSingle(this->XMinNumericUpDown->Value);
		min_y = Convert::ToSingle(this->YMinNumericUpDown->Value);
		min_z = Convert::ToSingle(this->ZMinNumericUpDown->Value);
		BoneBoxEditCtrlImp->SetBoxInfo(max_x, max_y, max_z, min_x, min_y, min_z);
	}
}

System::Void sqr_tools::CBoneBoxEditCtrl::YMaxNumericUpDown_ValueChanged( System::Object^ sender, System::EventArgs^ e )
{
	if (BoneBoxEditCtrlImp)
	{
		float max_x,max_y,max_z,min_x,min_y,min_z;
		max_x = Convert::ToSingle(this->XMaxNumericUpDown->Value);
		max_y = Convert::ToSingle(this->YMaxNumericUpDown->Value);
		max_z = Convert::ToSingle(this->ZMaxNumericUpDown->Value);
		min_x = Convert::ToSingle(this->XMinNumericUpDown->Value);
		min_y = Convert::ToSingle(this->YMinNumericUpDown->Value);
		min_z = Convert::ToSingle(this->ZMinNumericUpDown->Value);
		BoneBoxEditCtrlImp->SetBoxInfo(max_x, max_y, max_z, min_x, min_y, min_z);
	}
}

System::Void sqr_tools::CBoneBoxEditCtrl::YMinNumericUpDown_ValueChanged( System::Object^ sender, System::EventArgs^ e )
{
	if (BoneBoxEditCtrlImp)
	{
		float max_x,max_y,max_z,min_x,min_y,min_z;
		max_x = Convert::ToSingle(this->XMaxNumericUpDown->Value);
		max_y = Convert::ToSingle(this->YMaxNumericUpDown->Value);
		max_z = Convert::ToSingle(this->ZMaxNumericUpDown->Value);
		min_x = Convert::ToSingle(this->XMinNumericUpDown->Value);
		min_y = Convert::ToSingle(this->YMinNumericUpDown->Value);
		min_z = Convert::ToSingle(this->ZMinNumericUpDown->Value);
		BoneBoxEditCtrlImp->SetBoxInfo(max_x, max_y, max_z, min_x, min_y, min_z);
	}
}

System::Void sqr_tools::CBoneBoxEditCtrl::ZMaxNumericUpDown_ValueChanged( System::Object^ sender, System::EventArgs^ e )
{
	if (BoneBoxEditCtrlImp)
	{
		float max_x,max_y,max_z,min_x,min_y,min_z;
		max_x = Convert::ToSingle(this->XMaxNumericUpDown->Value);
		max_y = Convert::ToSingle(this->YMaxNumericUpDown->Value);
		max_z = Convert::ToSingle(this->ZMaxNumericUpDown->Value);
		min_x = Convert::ToSingle(this->XMinNumericUpDown->Value);
		min_y = Convert::ToSingle(this->YMinNumericUpDown->Value);
		min_z = Convert::ToSingle(this->ZMinNumericUpDown->Value);
		BoneBoxEditCtrlImp->SetBoxInfo(max_x, max_y, max_z, min_x, min_y, min_z);
	}
}

System::Void sqr_tools::CBoneBoxEditCtrl::ZMinNumericUpDown_ValueChanged( System::Object^ sender, System::EventArgs^ e )
{
	if (BoneBoxEditCtrlImp)
	{
		float max_x,max_y,max_z,min_x,min_y,min_z;
		max_x = Convert::ToSingle(this->XMaxNumericUpDown->Value);
		max_y = Convert::ToSingle(this->YMaxNumericUpDown->Value);
		max_z = Convert::ToSingle(this->ZMaxNumericUpDown->Value);
		min_x = Convert::ToSingle(this->XMinNumericUpDown->Value);
		min_y = Convert::ToSingle(this->YMinNumericUpDown->Value);
		min_z = Convert::ToSingle(this->ZMinNumericUpDown->Value);
		BoneBoxEditCtrlImp->SetBoxInfo(max_x, max_y, max_z, min_x, min_y, min_z);
	}
}

System::Void sqr_tools::CBoneBoxEditCtrl::É¾³ýToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
{
	if (BoneBoxEditCtrlImp)
	{
		if (this->BoneListTreeView->SelectedNode == nullptr)
			return;

		std::string strCPlus = MagStringToUnmagString(this->BoneListTreeView->SelectedNode->Text);
		BoneBoxEditCtrlImp->DeleteBox(strCPlus);
		this->BoneListTreeView->Nodes->Remove(this->BoneListTreeView->SelectedNode);
		Update();
	}
}

System::Void sqr_tools::CBoneBoxEditCtrl::showBox_CheckBox_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
	if (BoneBoxEditCtrlImp)
	{
		BoneBoxEditCtrlImp->SetShowBox(this->showBox_CheckBox->Checked);
	}
}

System::Void sqr_tools::CBoneBoxEditCtrl::BoneListTreeView_MouseDown( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
{
	if( e->Button == System::Windows::Forms::MouseButtons::Right )
	{
		TreeNode^ node = this->BoneListTreeView->GetNodeAt( e->X, e->Y );
		if (node)
		{
			this->BoneListTreeView->SelectedNode = node;
		}
	}
}