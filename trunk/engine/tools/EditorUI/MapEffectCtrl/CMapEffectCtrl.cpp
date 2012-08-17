#include "StdAfx.h"
#include "CMapEffectCtrl.h"
#include "MapEffectCtrl/MapEffectCtrlImp.h"

#include "dthelp.h"

System::Void CMapEffectCtrl::CMapEffectCtrl_Load( System::Object^ sender, System::EventArgs^ e )
{
	CMapEffectCtrlImp *pImp = NULL;
	pImp = (CMapEffectCtrlImp*)m_pCtrl;
	if( pImp )
	{
		pImp->LoadMapLocalEffect();


		if( !this->bOnlyShowLocalEffect )
		{
			pImp->LoadMapAmbientEffect();
		}

		this->InsertLocalEffectToTreeView();
	}
}

void CMapEffectCtrl::Update( void )
{

}

char* CMapEffectCtrl::GetImpTypeName( void )
{
	return GET_IMP_NAME(CMapEffectCtrlImp);
}

void* CMapEffectCtrl::GetImpParam( void )
{
	return this->Handle.ToPointer();
}

void CMapEffectCtrl::SetIsOnlyShowLocalEffect( bool b )
{
	this->bOnlyShowLocalEffect = b;
}

void CMapEffectCtrl::InsertLocalEffectToTreeView( )
{
	CMapEffectCtrlImp *pImp = NULL;
	pImp = (CMapEffectCtrlImp*)m_pCtrl;
	if( !pImp )
		return;

	TreeNode^ pParentNode = gcnew TreeNode;
	pParentNode->Text = gcnew String("mapfx_local_yb");
	pParentNode->ImageIndex = 0;
	this->effect_treeView->Nodes->Add(pParentNode);

	string groupname = "", effectname = "";
	int nGroupCount = pImp->GetLocalEffectGroupCount();
	for ( int m = 0; m < nGroupCount; ++m )
	{
		pImp->GetLocalEffectGroupName(m, groupname);
 		TreeNode^ pGroupNode = gcnew TreeNode;
 		pGroupNode->Text = gcnew String(groupname.c_str());
 		pGroupNode->ImageIndex = 1;
 		pParentNode->Nodes->Add(pGroupNode);

		int nGroupEffectCount = pImp->GetLocalEffectUnitCount(groupname);
		for ( int n = 0; n < nGroupEffectCount; ++n )
		{
 			pImp->GetLocalEffectUnitName(n, groupname, effectname);
 
 			TreeNode^ pEffectNode = gcnew TreeNode;
 			pEffectNode->Text = gcnew String(effectname.c_str());
 			pEffectNode->ImageIndex = 2;
 			pGroupNode->Nodes->Add(pEffectNode);
		}
	}

	pParentNode->Expand();
}

void CMapEffectCtrl::InsertAmbientEffectToTreeView( )
{
	CMapEffectCtrlImp *pImp = NULL;
	pImp = (CMapEffectCtrlImp*)m_pCtrl;
	if( !pImp )
		return;

	TreeNode^ pParentNode = gcnew TreeNode;
	pParentNode->Text = gcnew String("mapfx_ambient_yb");
	pParentNode->ImageIndex = 0;
	this->effect_treeView->Nodes->Add(pParentNode);

	string groupname = "", effectname = "";
	int nGroupCount = pImp->GetAmbientEffectGroupCount();
	for ( int m = 0; m < nGroupCount; ++m )
	{
		pImp->GetAmbientEffectGroupName(m, groupname);
		TreeNode^ pGroupNode = gcnew TreeNode;
		pGroupNode->Text = gcnew String(groupname.c_str());
		pGroupNode->ImageIndex = 1;
		pParentNode->Nodes->Add(pGroupNode);

		int nGroupEffectCount = pImp->GetAmbientEffectUnitCount(groupname);
		for ( int n = 0; n < nGroupEffectCount; ++n )
		{
			pImp->GetAmbientEffectUnitName(n, groupname, effectname);

			TreeNode^ pEffectNode = gcnew TreeNode;
			pEffectNode->Text = gcnew String(effectname.c_str());
			pEffectNode->ImageIndex = 3;
			pEffectNode->SelectedImageIndex = 4;
			pGroupNode->Nodes->Add(pEffectNode);
		}
	}

	pParentNode->Expand();
}

System::Void CMapEffectCtrl::effect_treeView_KeyDown( System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e )
{
	if(e->KeyCode == System::Windows::Forms::Keys::Control)
	{
		//show scene the same effect
		TreeNode ^seleceNode = this->effect_treeView->SelectedNode;
		if( seleceNode)
		{
			TreeNode^ parentNode = seleceNode->Parent;

			String^clrGroupName = parentNode->Text;
			string strGroupName = MagStringToUnmagString(clrGroupName);

			String^clrEffectName = seleceNode->Text;
			string strEffectName =	MagStringToUnmagString(clrEffectName);

			string m_strSelecetLocalEffectName = strGroupName + '\\' + strEffectName;
			int tt = 0;
		}
	}
}

String^ CMapEffectCtrl::GetActiveEffectName()
{
	return this->bActiveEffectName;
}

System::Void CMapEffectCtrl::effect_treeView_MouseUp( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
{
	TreeNode ^mouseUpNode = this->effect_treeView->GetNodeAt(e->X, e->Y);
	if( mouseUpNode )
	{
		TreeNode ^seleceNode = this->effect_treeView->SelectedNode;
		if( seleceNode )
		{
			int nSelectImageIndex = seleceNode->ImageIndex;
			if( nSelectImageIndex == 2 )
			{
				CMapEffectCtrlImp *pImp = NULL;
				pImp = (CMapEffectCtrlImp*)m_pCtrl;
				if( !pImp )
					return;

				//local effect
				TreeNode ^parentNode = seleceNode->Parent;
				String^  clrGroupName = parentNode->Text;
				string   strGroupName = MagStringToUnmagString(clrGroupName);
				String^  clrEffectUnitName = seleceNode->Text;
				string   strEffectUnitName = MagStringToUnmagString(clrEffectUnitName);
				strEffectUnitName = strGroupName + "/" + strEffectUnitName;

				pImp->SetActiveEffectName(strEffectUnitName);
			}
			else
			{
				if( nSelectImageIndex == 3 )
				{

				}
				else
				{
					if( nSelectImageIndex == 4 )
					{

					}
				}
			}
		}
	}
	else
	{
		//¿Ø¼þ¿Õ°×Çø
		return;
	}
}