// ModelEditor.cpp : main project file.

#include "stdafx.h"
#include "ModelEditorForm.h"
#include "IApplication.h"

using namespace sqr;
[STAThreadAttribute]
int WINAPI WinMain(HINSTANCE hinst,HINSTANCE hpre, LPSTR lpCmd, int nShowCmd)
{
	IApplication::InitApp();
	CUIBase::Initialize(IApplication::GetInst()->GetCtrlImpFactory());
	ModelEditor::ModelEditorForm^ mainForm = gcnew ModelEditor::ModelEditorForm();
	mainForm->Show();
	IApplication::GetInst()->Run();
	return 0;
}

