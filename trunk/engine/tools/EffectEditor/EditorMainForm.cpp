#include "stdafx.h"
#include "EditorMainForm.h"
#include "IApplication.h"
#include "EditorMainFormImp.h"

namespace EffectEditor
{
	System::Void EditorMainForm::InitMainFormImp()
	{
		this->Load += gcnew System::EventHandler(this, &EditorMainForm::_Load);
		m_pMainFormImp = new EditorMainFormImp();
	}

	System::Void EditorMainForm::_Load(System::Object^  sender, System::EventArgs^  e)
	{
		if (m_pMainFormImp)
			m_pMainFormImp->ChangeToEditEfxManager();
	}

	System::Void EditorMainForm::DestroyMainFormImp()
	{
		delete m_pMainFormImp;
	}

	System::Void EditorMainForm::ExitToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		this->Close();
	}

	System::Void EditorMainForm::EditorMainForm_FormClosed( System::Object^ sender, System::Windows::Forms::FormClosedEventArgs^ e )
	{
		IApplication::GetInst()->Close();
	}
}
