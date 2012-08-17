#include "StdAfx.h"
#include "ModelEditorForm.h"
#include "IApplication.h"
#include "CtrlImpHelper.h"
#include "CCoreViewImp.h"
#include "CEditContext.h"
#include "TSqrAllocator.inl"
#include "ContextCtrl\CContextCtrl.h"

namespace ModelEditor
{
	System::Void ModelEditorForm::ModelEditorForm_FormClosed( System::Object^ sender, System::Windows::Forms::FormClosedEventArgs^ e )
	{
		IApplication::GetInst()->Close();
	}

	System::Void ModelEditorForm::ModelEditorForm_Load( System::Object^ sender, System::EventArgs^ e )
	{
	} 

	System::Void ModelEditorForm::打开模型ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		this->cPieceGroupEditCtrl->AddMesh();
	}

	System::Void ModelEditorForm::保存模型ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		this->cPieceGroupEditCtrl->SaveMesh();
	}

	System::Void ModelEditorForm::保存模型为ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		this->cPieceGroupEditCtrl->SaveMeshAs();
	}

	System::Void ModelEditorForm::保存全部ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		this->cPieceGroupEditCtrl->SaveAllMesh();
	}

	System::Void ModelEditorForm::关闭所有模型文件ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		this->cPieceGroupEditCtrl->ClearAllMesh();
	}

	System::Void ModelEditorForm::工作路径ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
	{
		CPathSetDlg^ pathDlg = gcnew CPathSetDlg;
		pathDlg->ShowDialog();
	}
}
