#include "StdAfx.h"
#include "CPathSetCtrl.h"
#include "PathManager/CPathManager.h"
#include "OpenDirectoryDialog/OpenDirectoryDialog.h"
#include <string>

void CPathSetCtrl::Update(void)
{
	if( (!PList) || (!m_pCtrl))
		return ;
	PList->Items->Clear();
	CPathSetImp* pPathImp = (CPathSetImp*)m_pCtrl;
	unsigned int num = pPathImp->Count();
	for(unsigned int i=0;i<num;i++)
	{
		std::string str = pPathImp->GetPath(i);
		System::String^ path=UnmagStringToMagString((char*)(str.c_str()));
		PList->Items->Add(path);
	}
}

void CPathSetCtrl::ResetPath()
{
	Update();
}

void CPathSetCtrl::AppPath()
{
	if(m_pCtrl)
	{
		CPathSetImp* pPathImp = (CPathSetImp*)m_pCtrl;
		pPathImp->Clear();
		for(Int32 i = 0; i < PList->Items->Count; ++i)
		{
			pPathImp->AddPath(MagStringToUnmagString(PList->Items[i]->ToString()));
		}
		pPathImp->SaveConfigPath();
		Update();
	}
}

void CPathSetCtrl::HideButton()
{
	PathApp->Visible=false;
	PathReset->Visible=false;
}

System::Void CPathSetCtrl::PathAdd_Click(System::Object^  sender, System::EventArgs^  e) 
{
	PList->SelectedIndex = PList->Items->Add("");
	OnSetPath(sender,e);
}

System::Void CPathSetCtrl::PathDel_Click(System::Object^  sender, System::EventArgs^  e) 
{
	if(PList->Items->Count == 0)
		return;
	_ApplyEdit();
	if(m_ActiveIndex>-1)
		PList->Items->RemoveAt(m_ActiveIndex);
}

System::Void CPathSetCtrl::PathUp_Click(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_ActiveIndex < 1)
		return;
	_ApplyEdit();
	String^ tmp = PList->Items[m_ActiveIndex]->ToString();
	PList->Items[m_ActiveIndex] = PList->Items[m_ActiveIndex-1];
	PList->Items[m_ActiveIndex-1] = tmp;
	PList->SetSelected(m_ActiveIndex-1,true);
}

System::Void CPathSetCtrl::PathDown_Click(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_ActiveIndex >= PList->Items->Count - 1 ||  m_ActiveIndex < 0)
		return;
	_ApplyEdit();
	String^ tmp = PList->Items[m_ActiveIndex]->ToString();
	PList->Items[m_ActiveIndex] = PList->Items[m_ActiveIndex + 1];
	PList->Items[m_ActiveIndex + 1] = tmp;
	PList->SetSelected(m_ActiveIndex + 1,true);
}

System::Void CPathSetCtrl::PathApp_Click(System::Object^  sender, System::EventArgs^  e) 
{
	_ApplyEdit();
	AppPath();
}

System::Void CPathSetCtrl::OnSetPath(System::Object^  sender, System::EventArgs^  e)
{
	if( PList->Items->Count <= 0 )
		return;

	EditPanel->Parent = PList;
	System::Drawing::Rectangle rect = PList->GetItemRectangle(m_ActiveIndex);

	rect.Y = PList->ItemHeight * m_ActiveIndex - 1;
	rect.Height = PList->ItemHeight + 4;
	rect.X += 1;
	rect.Width-=2;
	
	EditPanel->Bounds = rect;
	PathEdit->Text = PList->SelectedItem->ToString();
	EditPanel->Visible = true;
	PathEdit->Select();
}

System::Void CPathSetCtrl::PathEdit_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) 
{
	if(!m_pCtrl)
		return ;
	if(e->KeyCode == System::Windows::Forms::Keys::Return)
	{
		_ApplyEdit();
	}
}

System::Void CPathSetCtrl::OnFileBrowserClick(System::Object^  sender, System::EventArgs^  e) 
{
	if(!m_pCtrl)
		return ;
	OpenDirectoryDialog dlg;
	dlg->InitialDirectory = PList->Items[m_ActiveIndex]->ToString();
	if( dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK )
	{
		PathEdit->Text = dlg->FileName;
	}
}

char* sqr_tools::CPathSetCtrl::GetImpTypeName( void )
{
	return GET_IMP_NAME(CPathSetImp);
}

System::Void sqr_tools::CPathSetCtrl::_InitCtrl( void )
{
	Update();
	m_ActiveIndex = -1;
}

System::Void sqr_tools::CPathSetCtrl::PList_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
{
	_ApplyEdit();
	m_ActiveIndex = this->PList->SelectedIndex;
}

System::Void sqr_tools::CPathSetCtrl::_ApplyEdit( void )
{
	if(EditPanel->Visible)
	{
		this->EditPanel->Visible = false;
		this->PList->Items[m_ActiveIndex]=this->PathEdit->Text;
		this->PathEdit->Text="";
	}
}