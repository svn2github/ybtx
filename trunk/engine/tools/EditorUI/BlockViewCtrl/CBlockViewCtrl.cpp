#include "StdAfx.h"
#include "CBlockViewCtrl.h"
#include "BlockViewCtrl/BlockViewCtrlImp.h"

System::Void CBlockViewCtrl::normalBlock_radioButton1_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
	// normal brush
	CBlockViewCtrlImp *pImp = NULL;
	pImp = (CBlockViewCtrlImp*)m_pCtrl;
	if( pImp )
		pImp->SetEditBrushType(0);
}

System::Void CBlockViewCtrl::exactBlock_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
	// excat brush
	CBlockViewCtrlImp *pImp = NULL;
	pImp = (CBlockViewCtrlImp*)m_pCtrl;
	if( pImp )
		pImp->SetEditBrushType(1);
}

System::Void CBlockViewCtrl::addBlock_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
	// write
	CBlockViewCtrlImp *pImp = NULL;
	pImp = (CBlockViewCtrlImp*)m_pCtrl;
	if( pImp )
		pImp->SetEditType(0);
		
}

System::Void CBlockViewCtrl::eraseBlock_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
	// erase
	CBlockViewCtrlImp *pImp = NULL;
	pImp = (CBlockViewCtrlImp*)m_pCtrl;
	if( pImp )
		pImp->SetEditType(1);
}

System::Void CBlockViewCtrl::lowBlock_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
	// low
	CBlockViewCtrlImp *pImp = NULL;
	pImp = (CBlockViewCtrlImp*)m_pCtrl;
	if( pImp )
		pImp->SetBlockType(1);
}

System::Void CBlockViewCtrl::midleBlock_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
	// mid
	CBlockViewCtrlImp *pImp = NULL;
	pImp = (CBlockViewCtrlImp*)m_pCtrl;
	if( pImp )
		pImp->SetBlockType(2);
}

System::Void CBlockViewCtrl::highBlock_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
	// high
	CBlockViewCtrlImp *pImp = NULL;
	pImp = (CBlockViewCtrlImp*)m_pCtrl;
	if( pImp )
		pImp->SetBlockType(3);
}

System::Void CBlockViewCtrl::block_button_Click( System::Object^ sender, System::EventArgs^ e )
{
// 		if ( !CMapGUIManager::HasInst() )
// 			return;
// 
// 		CMapGUIManager::GetInst()->GetMiniMap()->RefreshMiniMap(1);
}

System::Void CBlockViewCtrl::object_button_Click( System::Object^ sender, System::EventArgs^ e )
{
// 		if ( !CMapGUIManager::HasInst() )
// 			return;
// 
// 		CMapGUIManager::GetInst()->GetMiniMap()->RefreshMiniMap( 3);
}

System::Void CBlockViewCtrl::effect_button_Click( System::Object^ sender, System::EventArgs^ e )
{
// 		if ( !CMapGUIManager::HasInst() )
// 			return;
// 
// 		CMapGUIManager::GetInst()->GetMiniMap()->RefreshMiniMap( 2);
}

System::Void CBlockViewCtrl::sceneLight_button_Click( System::Object^ sender, System::EventArgs^ e )
{
// 		if ( !CMapGUIManager::HasInst() )
// 			return;
// 
// 		CMapGUIManager::GetInst()->GetMiniMap()->RefreshMiniMap( 5);
}

System::Void CBlockViewCtrl::clear_button_Click( System::Object^ sender, System::EventArgs^ e )
{
// 		if ( !CMapGUIManager::HasInst() )
// 			return;
// 
// 		CMapGUIManager::GetInst()->GetMiniMap()->RefreshMiniMap( 4);
}

System::Void CBlockViewCtrl::all_button_Click( System::Object^ sender, System::EventArgs^ e )
{
// 		if ( !CMapGUIManager::HasInst() )
// 			return;
// 
// 		CMapGUIManager::GetInst()->GetMiniMap()->RefreshMiniMap( 0);
}


void CBlockViewCtrl::Update( void )
{
	CBlockViewCtrlImp *pImp = NULL;
	pImp = (CBlockViewCtrlImp*)m_pCtrl;
	if( pImp )
	{
	}
}

char* CBlockViewCtrl::GetImpTypeName( void )
{
	return GET_IMP_NAME(CBlockViewCtrlImp);
}

void* CBlockViewCtrl::GetImpParam( void )
{
	return this->Handle.ToPointer();
}