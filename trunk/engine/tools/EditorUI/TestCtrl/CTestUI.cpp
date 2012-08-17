#include "StdAfx.h"
#include "CTestUI.h"

void CTestUI::Init()
{
	lightProp=gcnew sqr_tools::CLightProp(this->m_pCtrl);
	sceneProp=gcnew sqr_tools::CSceneProp(this->m_pCtrl);
	this->propertyGrid1->SelectedObject = sceneProp;
}