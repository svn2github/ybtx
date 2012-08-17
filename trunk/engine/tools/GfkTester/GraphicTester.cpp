// GraphicTester.cpp : main project file.
#include "stdafx.h"
#include "MainForm.h"
#include "IApplication.h"
#include "TSqrAllocator.inl"
using namespace GraphicTester;
//using namespace sqr;
using namespace System::Threading;

[STAThreadAttribute]
int WINAPI WinMain(HINSTANCE hinst,HINSTANCE hpre, LPSTR lpCmd, int nShowCmd)
{
	IApplication::InitApp();
	CUIBase::Initialize(IApplication::GetInst()->GetCtrlImpFactory());
	//创建主窗口
	MainForm^ mainForm = gcnew MainForm();
	//CParamsForm^ mainForm = gcnew CParamsForm();
	mainForm->Show();
	IApplication::GetInst()->Run();
	return 0;
}
