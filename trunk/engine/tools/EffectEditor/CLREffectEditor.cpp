#include "stdafx.h"
#include "CLREffectEditor.h"
#include "IApplication.h"
#include "EditorMainForm.h"
#include "TSqrAllocator.inl"

using namespace System;
using namespace System::Threading;
using namespace EffectEditor;

[STAThreadAttribute]
int WINAPI WinMain(HINSTANCE hinst,HINSTANCE hpre, LPSTR lpCmd, int nShowCmd)
{
	IApplication::InitApp();
	CUIBase::Initialize(IApplication::GetInst()->GetCtrlImpFactory());
	//创建主窗口
	EditorMainForm^ mainForm = gcnew EditorMainForm();
	mainForm->Show();
	IApplication::GetInst()->Run();
	return 0;
}
