// NewCharacterBrowser.cpp : main project file.

#include "stdafx.h"
#include "CharacterBrowserForm.h"
#include "CharacterBrowserBriage.h"
#include "IApplication.h"
using namespace CharacterBrowser;

[STAThreadAttribute]
int WINAPI WinMain(HINSTANCE hinst,HINSTANCE hpre, LPSTR lpCmd, int nShowCmd)
{
	IApplication::InitApp();

 	//////////////////////////////////////////////////////////////////////////
 	if( !CharacterBrowserBriage::HasInst() )
 		CharacterBrowserBriage::Create();
	
	CharacterBrowserBriage::GetInst()->InitEnviroment();
	//////////////////////////////////////////////////////////////////////////

	CUIBase::Initialize(IApplication::GetInst()->GetCtrlImpFactory());
	CharacterBrowserForm^ mainForm = gcnew CharacterBrowserForm();
	mainForm->Show();
	IApplication::GetInst()->Run();

 	if( CharacterBrowserBriage::HasInst() )
 		CharacterBrowserBriage::Destroy();

	return 0;
}
