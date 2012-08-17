// SetView.cpp : implementation file
//

#include "stdafx.h"
#include "MapEditor.h"
#include "SetView.h"
#include ".\setview.h"
#include <SharedTexture.h>
#include <CTileEditApp.h>
#include "Engine.h"
#include <CDomXml.h>
#include <fstream>

using namespace sqr;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CResSetView

//IMPLEMENT_DYNCREATE(CResSetView, CFormView)

// fix string.find bug
// 2007.4.10
int EFind( CString & a, std::string & f )
{
	std::string t;
	int i=0;
	for(;i<a.GetLength();++i)
	{
		if( a.GetAt(i) == '\\' || a.GetAt(i) == '/')
		{
			if( !t.empty())
			{
				if( t == f ) return i - f.size();
			}
			t = "";
		} else
		{
			t = t + a.GetAt(i);
		}
	}
	if( !t.empty())
	{
		if( t == f ) return i - f.size() + 1;
	}
	return -1;
}
int EFind2( std::string & a, std::string & f )
{
	std::string t;
	size_t i=0;
	for(;i<a.size();++i)
	{
		if( a[i] == '\\' || a[i] == '/')
		{
			if( !t.empty())
			{
				if( t == f ) return i - f.size();
			}
			t = "";
		} else
		{
			t = t + a[i];
		}
	}
	if( !t.empty())
	{
		if( t == f ) return i - f.size() + 1;
	}
	return -1;
}

void FindSubString(string& mainstr, const string& substr)
{
	string::iterator iter = search(mainstr.begin( ), mainstr.end( ), substr.begin( ), substr.end( ));

	int npos = iter - mainstr.begin();

	mainstr = mainstr.substr(npos + substr.length() + 1, mainstr.length());
}

CResSetView::CResSetView()
	: CFormView(CResSetView::IDD)
	, m_groupName(_T(""))
	, m_resSetName(_T(""))
{
	m_curSelResSet = NULL;
	//m_eEditState = COperator::EEDIT_STATE::EES_NONE;
	m_pActiveUnit = NULL;
	m_pActive2DUnit = NULL;
}

CResSetView::~CResSetView()
{
	m_curSelResSet = NULL;
	m_pActiveUnit = NULL;
	m_pActive2DUnit = NULL;
}

void CResSetView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SETVIEW, m_SetView);
	DDX_Text(pDX, IDC_GROUPNAME, m_groupName);
	DDX_Text(pDX, IDC_RESSETNAME, m_resSetName);
	DDX_Control(pDX, IDC_RESSETLIST, m_listResSet);
	DDX_Control(pDX, IDC_EFFECTVIEW, m_effectView);
}

BEGIN_MESSAGE_MAP(CResSetView, CFormView)
	ON_BN_CLICKED(IDC_CREATEGROUP, OnBnClickedCreategroup)
	ON_BN_CLICKED(IDC_CREATEUNIT, OnBnClickedCreateSingleUnit)
	ON_BN_CLICKED(IDC_DELETEGROUP, OnBnClickedDeletegroup)
	ON_BN_CLICKED(IDC_DELETEUNIT, OnBnClickedDeleteunit)
	ON_NOTIFY(TVN_SELCHANGED, IDC_SETVIEW, OnTvnSelchangedSetview)
	ON_WM_DESTROY()
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_SETVIEW, OnTvnItemexpandedSetview)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CREATESUBUNIT, OnBnClickedCreateComboUnit)
	ON_NOTIFY(NM_CLICK, IDC_SETVIEW, OnNMClickSetview)
	ON_BN_CLICKED(IDC_RADIO5, OnBnClickedCenter)
	ON_BN_CLICKED(IDC_RADIO3, OnBnClickedCorner)
	ON_BN_CLICKED(IDC_RADIO4, OnBnClickedXMidWay)
//	ON_NOTIFY(BCN_HOTITEMCHANGE, IDC_RADIO6, OnBnHotItemChangeRadio6)
ON_BN_CLICKED(IDC_RADIO6, OnBnClickedZMidWay)
ON_BN_CLICKED(IDC_CREATERESSET, OnBnClickedCreateresset)
ON_BN_CLICKED(IDC_DELETERESSET, OnBnClickedDeleteresset)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_RESSETLIST, OnLvnItemchangedRessetlist)
ON_COMMAND(ID_SV, OnSv)
ON_BN_CLICKED(IDC_BINDANI, OnBnClickedBindani)
ON_BN_CLICKED(IDC_TRANSVERTEX, OnBnClickedTransvertex)
ON_BN_CLICKED(IDC_TRANSTEXTURE, OnBnClickedTranstexture)
ON_NOTIFY(NM_CLICK, IDC_EFFECTVIEW, OnNMClickEffectview)
ON_BN_CLICKED(IDC_BINDEFFECT, OnBnClickedBindeffect)
//ON_BN_CLICKED(IDC_SHADOW, OnBnClickedShadow)
ON_BN_CLICKED(IDC_LOADRESSET, OnBnClickedLoadresset)
END_MESSAGE_MAP()

// CResSetView message handlers

void CResSetView::ResetResSetView()
{
	m_pActiveUnit = NULL;
	m_pActive2DUnit = NULL;
	
	if ( CTileEditApp::GetInstance()->GetOperator() )
		CTileEditApp::GetInstance()->GetOperator()->SetActiveObject(NULL);
	
	CTerrainMesh * pTerrain = CTileEditApp::GetInstance()->GetTerrainMesh();
	
	if ( pTerrain )
	{
	
		for ( size_t i = 0; i < pTerrain->GetGridCount(); ++i )
		{
			pTerrain->SetGridTextureHandle(0,i,0);
		}
	
		pTerrain->RebuildRenderTree();
	}
	
	CheckDlgButton(IDC_RADIO5,false);
	CheckDlgButton(IDC_RADIO4,false);
	CheckDlgButton(IDC_RADIO3,false);
	CheckDlgButton(IDC_RADIO6,false);

	CheckDlgButton(IDC_RADIO1,true);

	CheckDlgButton(IDC_LOWBLOCK,false);
	CheckDlgButton(IDC_MIDBLOCK,false);
	CheckDlgButton(IDC_HIGHBLOCK,false);

	CheckDlgButton(IDC_BLOCKMASK,false);
	CheckDlgButton(IDC_BLOCKTYPE,false);
	CheckDlgButton(IDC_HEIGHT,false);

	CheckDlgButton(IDC_TRANSLUCENT,false);
	CheckDlgButton(IDC_SOLID,false);

	HTREEITEM hItem = m_SetView.GetChildItem( TVI_ROOT );
	
	if( m_curSelResSet )
	{
		TVINSERTSTRUCT Item;
		Item.hParent = TVI_ROOT;
		Item.hInsertAfter = TVI_LAST;
		Item.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_TEXT|TVIF_SELECTEDIMAGE;
		Item.item.iImage = 0;
		Item.item.iSelectedImage = 0;
		Item.item.lParam = (LPARAM)m_curSelResSet;
		Item.item.pszText = (LPSTR)m_curSelResSet->m_resSetName.c_str();		
		HTREEITEM hSetRoot = m_SetView.InsertItem( &Item );

		//////////////////////////////////////////////////////////////////////////
		
		Item.hParent = hSetRoot;
		Item.item.iImage = 5;
		Item.item.iSelectedImage = 6;
		Item.item.lParam = (LPARAM)&(m_curSelResSet->m_2DResSet);
		Item.item.pszText = "场景地表";	
		SetResSetToTree2D(m_SetView.InsertItem(&Item));


		Item.hParent = hSetRoot;
		Item.item.iImage = 7;
		Item.item.iSelectedImage = 8;
		Item.item.lParam = (LPARAM)&(m_curSelResSet->m_3DResSet);
		Item.item.pszText = "场景物件";	
		SetResSetToTree3D( m_SetView.InsertItem( &Item ) );
		
		m_SetView.Expand(hSetRoot, TVE_EXPAND);
	}
	
	if ( hItem )
		m_SetView.DeleteItem( hItem );
}

void CResSetView::SetResSetToTree2D( HTREEITEM h2DSetItem )
{
	CDimSet* Set = (CDimSet*)m_SetView.GetItemData( h2DSetItem );
	
	for( int i = 0; i < (int)Set->m_resSetGroups.size(); i++ )
	{
		TVINSERTSTRUCT Item;
		Item.hParent = h2DSetItem;
		Item.hInsertAfter = TVI_LAST;
		Item.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_TEXT|TVIF_SELECTEDIMAGE;
		Item.item.iImage = 1;
		Item.item.iSelectedImage = 2;
		Item.item.lParam = (LPARAM)Set->m_resSetGroups[i];
		Item.item.pszText = (LPSTR)Set->m_resSetGroups[i]->m_groupName.c_str();		
		SetResSetUnitToTree2D( m_SetView.InsertItem( &Item ) );
	}
}

void CResSetView::SetResSetToTree3D( HTREEITEM h3DSetItem )
{
	CDimSet * Set = (CDimSet*)(m_SetView.GetItemData( h3DSetItem ));
	
	for( int i = 0; i < (int)Set->m_resSetGroups.size(); i++ )
	{
		TVINSERTSTRUCT Item;
		Item.hParent = h3DSetItem;
		Item.hInsertAfter = TVI_LAST;
		Item.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_TEXT|TVIF_SELECTEDIMAGE;
		Item.item.iImage = 1;
		Item.item.iSelectedImage = 2;
		Item.item.lParam = (LPARAM)Set->m_resSetGroups[i];
		Item.item.pszText = (LPSTR)Set->m_resSetGroups[i]->m_groupName.c_str();		
		SetResSetUnitToTree3D( m_SetView.InsertItem( &Item ) );
	}
}

void CResSetView::SetResSetUnitToTree2D( HTREEITEM hGroupItem )
{
	CResSetGroup * pGroup = (CResSetGroup*)m_SetView.GetItemData( hGroupItem );

	for( int i = 0; i < (int)pGroup->m_resSetUnits.size(); i++ )
	{
		TVINSERTSTRUCT Item;
		Item.hParent = hGroupItem;
		Item.hInsertAfter = TVI_LAST;
		Item.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_TEXT|TVIF_SELECTEDIMAGE;

		if ( !((CResSetUnit*)(pGroup->m_resSetUnits[i]))->GetIsEdited() )
		{
			Item.item.iImage = 9;
			Item.item.iSelectedImage = 9;
		}
		else
		{
			Item.item.iImage = 10;
			Item.item.iSelectedImage = 10;
		}

		Item.item.lParam = (LPARAM)pGroup->m_resSetUnits[i];

		string str =  pGroup->m_resSetUnits[i]->GetNameByIndex(0);

		char s[256];
		strcpy(s,str.c_str());
		char * pEndPos = strrchr(s,'.');
		*((char*)(pEndPos)) = '\0';
		char * pStartPos;
		if ( ( pStartPos = strrchr(s,'\\')) == NULL )
			pStartPos = strrchr(s,'/');
		Item.item.pszText = pStartPos+1;
		
		m_SetView.InsertItem( &Item );
	}
}

void CResSetView::SetResSetUnitToTree3D( HTREEITEM hGroupItem )
{
	CResSetGroup * pGroup = (CResSetGroup*)m_SetView.GetItemData( hGroupItem );
	
	for( int i = 0; i < (int)pGroup->m_resSetUnits.size(); i++ )
	{
		TVINSERTSTRUCT Item;
		Item.hParent = hGroupItem;
		Item.hInsertAfter = TVI_LAST;
		Item.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_TEXT|TVIF_SELECTEDIMAGE;
		
		if ( !((CResSetUnit*)(pGroup->m_resSetUnits[i]))->GetIsEdited() )
		{
			Item.item.iImage = 3;
			Item.item.iSelectedImage = 3;
		}
		else
		{
			Item.item.iImage = 4;
			Item.item.iSelectedImage = 4;
		}

		Item.item.lParam = (LPARAM)pGroup->m_resSetUnits[i];
		
		string str =  pGroup->m_resSetUnits[i]->GetNameByIndex(0);
		
		if ( pGroup->m_resSetUnits[i]->GetIsSubUnit() )
		{
			char s[128];
			strcpy(s,str.c_str());
			char * pEndPos = strrchr(s,'.');
			*((char*)(pEndPos-1)) = '\0';
			char * pStartPos;
			if ( ( pStartPos = strrchr(s,'\\')) == NULL )
				pStartPos = strrchr(s,'/');
			Item.item.pszText = pStartPos+1;
		}
		else
		{
			char s[128];
			strcpy(s,str.c_str());
			char * pEndPos = strrchr(s,'.');
			*((char*)(pEndPos)) = '\0';
			char * pStartPos;
			if ( ( pStartPos = strrchr(s,'\\')) == NULL )
				pStartPos = strrchr(s,'/');
			Item.item.pszText = pStartPos+1;
		}
		
		m_SetView.InsertItem( &Item );
	}
}



void CResSetView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CFormView::OnShowWindow(bShow, nStatus);
	
	static BOOL IsValid = FALSE;
	
	if( bShow && !IsValid )
	{
		IsValid = TRUE;
		
		m_Image.Create(16,16,ILC_COLOR16|ILC_MASK,0,1);
		m_Image.Add( ::AfxGetApp()->LoadIcon( IDR_SETICO ) );
		m_Image.Add( ::AfxGetApp()->LoadIcon( IDR_GROUP ) );
		m_Image.Add( ::AfxGetApp()->LoadIcon( IDR_GROUPSEL ) );
		m_Image.Add( ::AfxGetApp()->LoadIcon( IDR_UNEDITED3D ) );
		m_Image.Add( ::AfxGetApp()->LoadIcon( IDI_EDITED3D ) );
		
		m_Image.Add( ::AfxGetApp()->LoadIcon( IDI_2DSET ) );
		m_Image.Add( ::AfxGetApp()->LoadIcon( IDI_2DSETSEL ) );
		m_Image.Add( ::AfxGetApp()->LoadIcon( IDI_3DSET ) );
		m_Image.Add( ::AfxGetApp()->LoadIcon( IDI_3DSETSEL ) );

		m_Image.Add( ::AfxGetApp()->LoadIcon( IDI_UNEDITED2D ) );  //9
		m_Image.Add( ::AfxGetApp()->LoadIcon( IDI_EDITED2D ) );  // 10

		m_SetView.SetImageList( &m_Image, TVSIL_NORMAL );

		m_ImageEffect.Create(16,16,ILC_COLOR16|ILC_MASK,0,1);

		m_ImageEffect.Add( ::AfxGetApp()->LoadIcon( IDR_SETICO ) ); // 0
		
		m_ImageEffect.Add( ::AfxGetApp()->LoadIcon( IDR_GROUP ) ); // 1
		m_ImageEffect.Add( ::AfxGetApp()->LoadIcon( IDR_GROUPSEL ) ); // 2
		
		m_ImageEffect.Add( ::AfxGetApp()->LoadIcon( IDI_FXUNIT ) );	// 3
		m_ImageEffect.Add( ::AfxGetApp()->LoadIcon( IDI_FXUNITSEL ) ); // 4
		
		m_effectView.SetImageList( &m_ImageEffect, TVSIL_NORMAL );
				
		CRect rt;
		m_listResSet.GetClientRect( &rt );
		m_listResSet.InsertColumn( 0, "", LVCFMT_LEFT, rt.Width() );
		
		CheckDlgButton(IDC_RADIO5,false);
		CheckDlgButton(IDC_RADIO4,false);
		CheckDlgButton(IDC_RADIO3,false);
		CheckDlgButton(IDC_RADIO6,false);

		CheckDlgButton(IDC_RADIO1,true);

		CheckDlgButton(IDC_LOWBLOCK,false);
		CheckDlgButton(IDC_MIDBLOCK,false);
		CheckDlgButton(IDC_HIGHBLOCK,false);

		CheckDlgButton(IDC_BLOCKMASK,false);
		CheckDlgButton(IDC_BLOCKTYPE,false);
		CheckDlgButton(IDC_HEIGHT,false);

		CheckDlgButton(IDC_TRANSLUCENT,false);
		CheckDlgButton(IDC_SOLID,false);

		CheckDlgButton(IDC_SHADOW,true);
	}
	
	/*if ( CTileEditApp::GetInstance()->GetOperator() && bShow ) 
	{
		CTileEditApp::GetInstance()->GetOperator()->SetEditState(COperator::EES_SET_BLOCK_MASK);
	}*/
}
//////
//////void CResSetView::ResetResSetList()
//////{
//////	m_listResSet.DeleteAllItems();
//////
//////	// read in all reset files
//////
//////	WIN32_FIND_DATA	  FindFileData;
//////	HANDLE            hFind;
//////
//////	//////////////////////////////////////////////////////////////////////////
//////
//////	// the first
//////	hFind = FindFirstFile("..\\Tools\\TileEditor\\ResourceSet\\*.ars", &FindFileData );
//////
//////	do
//////	{
//////		if ( hFind != INVALID_HANDLE_VALUE )
//////		{
//////			CHAR t[MAX_PATH] = "..\\Tools\\TileEditor\\ResourceSet\\";
//////	
//////			strcat(t, FindFileData.cFileName);
//////	
//////			CResSet * pResSet = new CResSet;
//////
//////			int pos = strlen(FindFileData.cFileName) - 4;
//////
//////			FindFileData.cFileName[pos] = '\0';
//////
//////			pResSet->m_resSetName = FindFileData.cFileName;
//////			
//////			m_resSets.push_back(pResSet);
//////
//////			//////////////////////////////////////////////////////////////////////////
//////
//////			FindFileData.cFileName[pos] = '.';
//////			
//////			CDomXmlDocument doc("x");
//////			ifstream in(t);
//////			in >> doc;
//////			
//////			//////////////////////////////////////////////////////////////////////////
//////			
//////			int cnt = doc("俑兵天下图素集3D").Count();
//////			
//////			for (int i = 0; i < cnt ; i++)
//////			{
////// 				CResSetGroup * pGroup = new CResSetGroup;
//////
//////				pGroup->m_groupName = doc("俑兵天下图素集3D")(i)["ResSetGroupName"].As<std::string>();
//////
//////				int cnt2  = doc("俑兵天下图素集3D")(i).Count();
//////
//////				for ( int j = 0; j < cnt2; j++ )
//////				{
//////					C3DResSetUnit * pResSetUnit  = new C3DResSetUnit;
//////
//////					int cnt3 = doc("俑兵天下图素集3D")(i)(j)["ResSetUnitArpCount"].As<int>();
//////					
//////					pResSetUnit->SetTotalBlockSize(cnt3);
//////					pResSetUnit->SetTotalHeightSize(cnt3);
//////					
//////					for ( int k = 0; k < cnt3; ++k )
//////					{
//////						char s[64];
//////						sprintf(s,"%dName",k);
//////						std::string str = doc("俑兵天下图素集3D")(i)(j)[s].As<std::string>();
//////						pResSetUnit->AddName(str);
//////						
//////						sprintf(s,"%dBlockSize",k);
//////						int nBlockSize = doc("俑兵天下图素集3D")(i)(j)[s].As<int>();
//////						
//////						pResSetUnit->SetModelBlockSize(k,nBlockSize);
//////						
//////						for ( int l = 0; l < nBlockSize; ++l )
//////						{
//////							// block mask
//////							sprintf(s,"%d-%dBlockMask",k,l);
//////							int nBlockMask = doc("俑兵天下图素集3D")(i)(j)[s].As<int>();
//////							pResSetUnit->SetModelBlockMask(k,l,nBlockMask);
//////
//////							// block type
//////							sprintf(s,"%d-%dBlockType",k,l);
//////							int nBlockType = doc("俑兵天下图素集3D")(i)(j)[s].As<int>();
//////							pResSetUnit->SetModelBlockType(k,l,nBlockType);
//////
//////							// block logic height
////////							sprintf(s,"%d-%dBlockLogicHeight",k,l);
////////							float fBlockLogicHeight = doc("俑兵天下图素集3D")(i)(j)[s].As<float>();
////////							pResSetUnit->SetModelLogicHeight(k,l,fBlockLogicHeight);
//////						}
//////					}
//////					
//////					pResSetUnit->SetLift(doc("俑兵天下图素集3D")(i)(j)["Lift"].As<float>());
//////					pResSetUnit->SetShiftX(doc("俑兵天下图素集3D")(i)(j)["ShiftX"].As<float>());
//////					pResSetUnit->SetShiftZ(doc("俑兵天下图素集3D")(i)(j)["ShiftZ"].As<float>());
//////					pResSetUnit->SetIsSubUnit(doc("俑兵天下图素集3D")(i)(j)["IsSubUnit"].As<bool>());
//////					pResSetUnit->SetCenterType((CTileEditorModel::ECenter_Type)doc("俑兵天下图素集3D")(i)(j)["CenterType"].As<int>());
//////					pResSetUnit->SetIsEdited(doc("俑兵天下图素集3D")(i)(j)["IsEdited"].As<bool>());
//////					pResSetUnit->SetTransType((CFace::ETrans_Type)doc("俑兵天下图素集3D")(i)(j)["TransType"].As<int>());
//////					pResSetUnit->SetAnimationByAraFileName(doc("俑兵天下图素集3D")(i)(j)["AraFileName"].As<std::string>());
//////					
//////					pGroup->m_resSetUnits.push_back(pResSetUnit);
//////				}
//////				
//////				pResSet->m_3DResSet.m_resSetGroups.push_back(pGroup);
//////			}
//////
//////			//////////////////////////////////////////////////////////////////////////
//////			
//////			cnt = doc("俑兵天下图素集2D").Count();
//////
//////			for (int i = 0; i < cnt ; i++)
//////			{
//////				CResSetGroup * pGroup = new CResSetGroup;
//////
//////				pGroup->m_groupName = doc("俑兵天下图素集2D")(i)["ResSetGroupName"].As<std::string>();
//////
//////				int cnt2  = doc("俑兵天下图素集2D")(i).Count();
//////
//////				for ( int j = 0; j < cnt2; j++ )
//////				{
//////					C2DResSetUnit * pResSetUnit  = new C2DResSetUnit;
//////
//////					std::string str = doc("俑兵天下图素集2D")(i)(j)["TextureFileName"].As<std::string>();
//////					
//////					pResSetUnit->AddName(str);
//////
//////					pResSetUnit->SetIsSubUnit(doc("俑兵天下图素集2D")(i)(j)["IsSubUnit"].As<bool>());
//////					
//////					pResSetUnit->SetIsEdited(doc("俑兵天下图素集2D")(i)(j)["IsEdited"].As<bool>());
//////
//////					pResSetUnit->SetTransType((CTerrainMesh::CTextureNode::ETRANS_TYPE)doc("俑兵天下图素集2D")(i)(j)["TransType"].As<int>());
//////
//////					pGroup->m_resSetUnits.push_back(pResSetUnit);
//////				}
//////
//////				pResSet->m_2DResSet.m_resSetGroups.push_back(pGroup);
//////			}
//////		}
//////	}
//////	while ( FindNextFile( hFind, &FindFileData ) && hFind != INVALID_HANDLE_VALUE );
//////
//////	FindClose ( hFind );
//////	
//////	//////////////////////////////////////////////////////////////////////////
//////	
//////	for( int i = 0; i< (int)m_resSets.size(); i++ )
//////	{
//////		int Index = m_listResSet.InsertItem( m_listResSet.GetItemCount(), m_resSets[i]->m_resSetName.c_str() );
//////		m_listResSet.SetItemData( Index, (LPARAM)m_resSets[i] );
//////	}
//////	
//////	ResetResSetView();
//////}

void CResSetView::ResetResSetList()
{
	m_listResSet.DeleteAllItems();
	
	// read in all reset files

	WIN32_FIND_DATA	  FindFileData;
	HANDLE            hFind;

	//////////////////////////////////////////////////////////////////////////

	// the first
	hFind = FindFirstFile("..\\Tools\\TileEditor\\ResourceSet\\*.ars", &FindFileData );
	
	do
	{
		if ( hFind != INVALID_HANDLE_VALUE )
		{
			CHAR t[MAX_PATH] = "..\\Tools\\TileEditor\\ResourceSet\\";

			strcat(t, FindFileData.cFileName);

			CResSet * pResSet = new CResSet;

			int pos = strlen(FindFileData.cFileName) - 4;

			FindFileData.cFileName[pos] = '\0';

			pResSet->m_resSetName = FindFileData.cFileName;

			m_resSets.push_back(pResSet);

			//////////////////////////////////////////////////////////////////////////

			FindFileData.cFileName[pos] = '.';

			CDomXmlDocument doc("x");
			ifstream in(t);
			in >> doc;
			
			CDomXmlDocument* p3d=doc.GetFirstChild();

			for (CDomXmlDocument * pi=p3d->GetFirstChild(); pi ; pi=pi->GetNextSibling())
			{
				CResSetGroup * pGroup = new CResSetGroup;

				pGroup->m_groupName = (*pi)["ResSetGroupName"].As<std::string>();

				for (CDomXmlDocument * pj=pi->GetFirstChild(); pj; pj=pj->GetNextSibling() )
				{
					C3DResSetUnit * pResSetUnit  = new C3DResSetUnit;
					
					CDomXmlAttribute* pk=pj->GetFirstAttribute();
					
					int cnt3 = pk->As<int>();
					
					pResSetUnit->SetTotalBlockSize(cnt3);
					pResSetUnit->SetTotalHeightSize(cnt3);
					
					for ( int k = 0; k < cnt3; ++k )
					{
						pk=pk->GetNextSibling();
						std::string str = pk->As<std::string>();
						pResSetUnit->AddName(str);

						pk=pk->GetNextSibling();
						int nBlockSize = pk->As<int>();
						pResSetUnit->SetModelBlockSize(k,nBlockSize);

						for ( int l = 0; l < nBlockSize; ++l )
						{
							// block mask
							pk=pk->GetNextSibling();
							int nBlockMask = pk->As<int>();
							pResSetUnit->SetModelBlockMask(k,l,nBlockMask);

							// block type
							pk=pk->GetNextSibling();
							int nBlockType = pk->As<int>();
							pResSetUnit->SetModelBlockType(k,l,nBlockType);
						}
						
						// height
						pk=pk->GetNextSibling();
						int nHeightSize = pk->As<int>();
						pResSetUnit->SetModelHeightSize(k,nHeightSize);
						
						for ( int l = 0; l < nHeightSize; ++l )
						{
							// block logic height
							pk=pk->GetNextSibling();
							float fBlockLogicHeight = pk->As<float>();
							pResSetUnit->SetModelLogicHeight(k,l,fBlockLogicHeight);
						}
					}
					
					pk=pk->GetNextSibling();
					pResSetUnit->SetLift(pk->As<float>());
					pk=pk->GetNextSibling();
					pResSetUnit->SetShiftX(pk->As<float>());
					pk=pk->GetNextSibling();
					pResSetUnit->SetShiftZ(pk->As<float>());
					pk=pk->GetNextSibling();
					pResSetUnit->SetIsSubUnit(pk->As<bool>());
					pk=pk->GetNextSibling();
					pResSetUnit->SetCenterType((CTileEditorModel::ECenter_Type)pk->As<int>());
					pk=pk->GetNextSibling();
					pResSetUnit->SetIsEdited(pk->As<bool>());
					pk=pk->GetNextSibling();
					pResSetUnit->SetTransType((CFace::ETrans_Type)pk->As<int>());

					pk=pk->GetNextSibling();
					pResSetUnit->SetShadow(pk->As<bool>());

					pk=pk->GetNextSibling();
					pResSetUnit->SetAnimationByAraFileName(pk->As<std::string>());
					
					pk=pk->GetNextSibling();
					if ( pk->As<bool>() )
					{
						pk=pk->GetNextSibling();
						pResSetUnit->m_strEffectName = pk->As<std::string>();
						pResSetUnit->m_pEffectGroup = m_curFXSet->m_pEffectGroup;
					}
					
					pGroup->m_resSetUnits.push_back(pResSetUnit);
				}
				
				pResSet->m_3DResSet.m_resSetGroups.push_back(pGroup);
			}
			
			//////////////////////////////////////////////////////////////////////////
			
			int cnt = doc("俑兵天下图素集2D").Count();
			
			for (int i = 0; i < cnt ; i++)
			{
				CResSetGroup * pGroup = new CResSetGroup;
				
				pGroup->m_groupName = doc("俑兵天下图素集2D")(i)["ResSetGroupName"].As<std::string>();

				int cnt2  = doc("俑兵天下图素集2D")(i).Count();

				for ( int j = 0; j < cnt2; j++ )
				{
					C2DResSetUnit * pResSetUnit  = new C2DResSetUnit;

					std::string str = doc("俑兵天下图素集2D")(i)(j)["TextureFileName"].As<std::string>();

					pResSetUnit->AddName(str);

					pResSetUnit->SetIsSubUnit(doc("俑兵天下图素集2D")(i)(j)["IsSubUnit"].As<bool>());

					pResSetUnit->SetIsEdited(doc("俑兵天下图素集2D")(i)(j)["IsEdited"].As<bool>());

					pResSetUnit->SetTransType((CTerrainMesh::CTextureNode::ETRANS_TYPE)doc("俑兵天下图素集2D")(i)(j)["TransType"].As<int>());

					pGroup->m_resSetUnits.push_back(pResSetUnit);
				}

				pResSet->m_2DResSet.m_resSetGroups.push_back(pGroup);
			}
		}
	}
	while ( FindNextFile( hFind, &FindFileData ) && hFind != INVALID_HANDLE_VALUE );

	FindClose ( hFind );

	//////////////////////////////////////////////////////////////////////////

	for( int i = 0; i< (int)m_resSets.size(); i++ )
	{
		int Index = m_listResSet.InsertItem( m_listResSet.GetItemCount(), m_resSets[i]->m_resSetName.c_str() );
		m_listResSet.SetItemData( Index, (LPARAM)m_resSets[i] );
	}

	ResetResSetView();
}

void CResSetView::AddResSetFromFile(std::string strResSetFileName)
{
	// read in all reset files
	m_listResSet.DeleteAllItems();

	int i = strResSetFileName.find_last_of('.');
	
	int nPos = strResSetFileName.find("ResourceSet");	
	
	CResSet * pResSet = new CResSet;
	
	pResSet->m_resSetName =	strResSetFileName.substr(nPos+12,i-nPos-12);
	
	m_resSets.push_back(pResSet);
		
	//////////////////////////////////////////////////////////////////////////
	
	CDomXmlDocument doc("x");
	ifstream in(strResSetFileName.c_str());
	in >> doc;
	
	CDomXmlDocument* p3d=doc.GetFirstChild();
	
	for (CDomXmlDocument * pi=p3d->GetFirstChild(); pi ; pi=pi->GetNextSibling())
	{
		CResSetGroup * pGroup = new CResSetGroup;

		pGroup->m_groupName = (*pi)["ResSetGroupName"].As<std::string>();
		
		for (CDomXmlDocument * pj=pi->GetFirstChild(); pj; pj=pj->GetNextSibling() )
		{
			C3DResSetUnit * pResSetUnit  = new C3DResSetUnit;

			CDomXmlAttribute* pk=pj->GetFirstAttribute();

			int cnt3 = pk->As<int>();

			pResSetUnit->SetTotalBlockSize(cnt3);
			pResSetUnit->SetTotalHeightSize(cnt3);

			for ( int k = 0; k < cnt3; ++k )
			{
				pk=pk->GetNextSibling();
				std::string str = pk->As<std::string>();
				pResSetUnit->AddName(str);

				pk=pk->GetNextSibling();
				int nBlockSize = pk->As<int>();
				pResSetUnit->SetModelBlockSize(k,nBlockSize);

				for ( int l = 0; l < nBlockSize; ++l )
				{
					// block mask
					pk=pk->GetNextSibling();
					int nBlockMask = pk->As<int>();
					pResSetUnit->SetModelBlockMask(k,l,nBlockMask);

					// block type
					pk=pk->GetNextSibling();
					int nBlockType = pk->As<int>();
					pResSetUnit->SetModelBlockType(k,l,nBlockType);
				}

				// height
				pk=pk->GetNextSibling();
				int nHeightSize = pk->As<int>();
				pResSetUnit->SetModelHeightSize(k,nHeightSize);

				for ( int l = 0; l < nHeightSize; ++l )
				{
					// block logic height
					pk=pk->GetNextSibling();
					float fBlockLogicHeight = pk->As<float>();
					pResSetUnit->SetModelLogicHeight(k,l,fBlockLogicHeight);
				}
			}

			pk=pk->GetNextSibling();
			pResSetUnit->SetLift(pk->As<float>());
			pk=pk->GetNextSibling();
			pResSetUnit->SetShiftX(pk->As<float>());
			pk=pk->GetNextSibling();
			pResSetUnit->SetShiftZ(pk->As<float>());
			pk=pk->GetNextSibling();
			pResSetUnit->SetIsSubUnit(pk->As<bool>());
			pk=pk->GetNextSibling();
			pResSetUnit->SetCenterType((CTileEditorModel::ECenter_Type)pk->As<int>());
			pk=pk->GetNextSibling();
			pResSetUnit->SetIsEdited(pk->As<bool>());
			pk=pk->GetNextSibling();
			pResSetUnit->SetTransType((CFace::ETrans_Type)pk->As<int>());

			pk=pk->GetNextSibling();
			pResSetUnit->SetShadow(pk->As<bool>());
			
			pk=pk->GetNextSibling();
			pResSetUnit->SetAnimationByAraFileName(pk->As<std::string>());
			
			pk=pk->GetNextSibling();
			if ( pk->As<bool>() )
			{
				pk=pk->GetNextSibling();
				pResSetUnit->m_strEffectName = pk->As<std::string>();
				pResSetUnit->m_pEffectGroup = m_curFXSet->m_pEffectGroup;
			}

			pGroup->m_resSetUnits.push_back(pResSetUnit);
		}

		pResSet->m_3DResSet.m_resSetGroups.push_back(pGroup);
	}

	//////////////////////////////////////////////////////////////////////////

	int cnt = doc("俑兵天下图素集2D").Count();

	for (int i = 0; i < cnt ; i++)
	{
		CResSetGroup * pGroup = new CResSetGroup;

		pGroup->m_groupName = doc("俑兵天下图素集2D")(i)["ResSetGroupName"].As<std::string>();

		int cnt2  = doc("俑兵天下图素集2D")(i).Count();

		for ( int j = 0; j < cnt2; j++ )
		{
			C2DResSetUnit * pResSetUnit  = new C2DResSetUnit;

			std::string str = doc("俑兵天下图素集2D")(i)(j)["TextureFileName"].As<std::string>();

			pResSetUnit->AddName(str);

			pResSetUnit->SetIsSubUnit(doc("俑兵天下图素集2D")(i)(j)["IsSubUnit"].As<bool>());

			pResSetUnit->SetIsEdited(doc("俑兵天下图素集2D")(i)(j)["IsEdited"].As<bool>());

			pResSetUnit->SetTransType((CTerrainMesh::CTextureNode::ETRANS_TYPE)doc("俑兵天下图素集2D")(i)(j)["TransType"].As<int>());

			pGroup->m_resSetUnits.push_back(pResSetUnit);
		}

		pResSet->m_2DResSet.m_resSetGroups.push_back(pGroup);
	}
	
	//////////////////////////////////////////////////////////////////////////
	
	for( int i = 0; i< (int)m_resSets.size(); i++ )
	{
		int Index = m_listResSet.InsertItem( m_listResSet.GetItemCount(), m_resSets[i]->m_resSetName.c_str() );
		m_listResSet.SetItemData( Index, (LPARAM)m_resSets[i] );
	}
}

void CResSetView::OnBnClickedCreategroup()
{
	// TODO: Add your control notification handler code here

	if ( m_curSelResSet == NULL )
	{
		MessageBox( "请先创建或选择编辑图素包", "错误" );
		return;
	}

	UpdateData();

	if( m_groupName.IsEmpty() )
	{
		MessageBox( "新创建图素组名字不能为空", "错误" );
		return;
	}
	
	HTREEITEM CurSel = m_SetView.GetSelectedItem();
	if( !CurSel )
		return;
	TVITEM TItem;
	TItem.hItem = CurSel;
	TItem.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE;
	m_SetView.GetItem( &TItem );
	if( TItem.iImage != 5 && TItem.iImage != 7 )
		return;

	CDimSet* Set = (CDimSet*)TItem.lParam;
	
	CResSetGroup * Group = new CResSetGroup;
	Group->m_groupName = (LPCTSTR)m_groupName;
	Set->m_resSetGroups.push_back( Group );
	
	TVINSERTSTRUCT Item;
	Item.hParent = CurSel;
	Item.hInsertAfter = TVI_LAST;
	Item.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_TEXT|TVIF_SELECTEDIMAGE;
	Item.item.iImage = 1;
	Item.item.iSelectedImage = 2;
	Item.item.lParam = (LPARAM)Group;
	Item.item.pszText = (LPSTR)Group->m_groupName.c_str();		
	m_SetView.InsertItem( &Item );
	m_SetView.Expand( CurSel, TVE_EXPAND );
}

void CResSetView::AddSingleUnitForSelectedGroup( TVITEM & TItem)
{
	CResSetGroup * pGroup = (CResSetGroup*)TItem.lParam;
	TVINSERTSTRUCT Item;
	Item.hParent = TItem.hItem;
	Item.hInsertAfter = TVI_LAST;
	Item.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_TEXT|TVIF_SELECTEDIMAGE;

	//////////////////////////////////////////////////////////////////////////

	int Image, SelImage;
	m_SetView.GetItemImage( m_SetView.GetParentItem( TItem.hItem ), Image, SelImage );
	
	if ( Image == 7 )
	{
		Item.item.iImage = 3;
		Item.item.iSelectedImage = 3;

		static char Filter[] = "Static Model Files (*.arp)|*.arp||";

		CFileDialog Dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT|OFN_NOCHANGEDIR, Filter );

		if ( Dlg.DoModal() == IDOK )
		{
			POSITION Pos = Dlg.GetStartPosition();
			while ( Pos )
			{
				CString str = Dlg.GetNextPathName( Pos );
				str.MakeUpper();
				int nPos = EFind( str, string("DATA"));
				int nPad = 0;

				int i = str.GetLength();
				for ( ; i > 0; i-- )
				{
					if ( str[ i - 1 ] == '.' )
					{
						nPad = i - 1;
					}

					if ( str[ i - 1 ] == '/' || str[ i - 1 ] == '\\' )
					{
						break;
					}
				}

				//////////////////////////////////////////////////////////////////////////

				C3DResSetUnit * pUnit = new C3DResSetUnit();
				pUnit->SetTotalBlockSize(1);
				pUnit->SetTotalHeightSize(1);
				pUnit->SetIsSubUnit(false);
				//pUnit->AddName(str.GetBuffer()+nPos+5);

				//////////////////////////////////////////////////////////////////////////
				//得到相对名
				string modelname = str;
				FindSubString(modelname, "DATA");
				//////////////////////////////////////////////////////////////////////////
				
				//pUnit->AddName(str.GetBuffer());
				pUnit->AddName(modelname.c_str());
				pUnit->SetCenterType(0); // center
				pGroup->m_resSetUnits.push_back( pUnit );

				str.SetAt(nPad,'\0');

				char str2[64];

				strcpy(str2,str.GetBuffer()+i);

				Item.item.lParam = (LPARAM)pUnit;
				Item.item.pszText = str2;	
				m_SetView.InsertItem( &Item );
			}
			m_SetView.Expand( TItem.hItem, TVE_EXPAND );
		}
	}
	
	if( Image == 5 )
	{
		Item.item.iImage = 9;
		Item.item.iSelectedImage = 9;
		
		static char Filter[] = "地表纹理贴图 (*.tga;*.bmp)|*.tga;*.bmp||";
		
		CFileDialog Dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT|OFN_NOCHANGEDIR, Filter );
		
		if ( Dlg.DoModal() == IDOK )
		{
			POSITION Pos = Dlg.GetStartPosition();
			while ( Pos )
			{
				CString str = Dlg.GetNextPathName( Pos );
				str.MakeUpper();
				// int nPos = str.Find("DATA");
				int nPos = EFind( str, string("DATA"));
				int nPad = 0;

				int i = str.GetLength();
				for ( ; i > 0; i-- )
				{
					if ( str[ i - 1 ] == '.' )
					{
						nPad = i - 1;
					}

					if ( str[ i - 1 ] == '/' || str[ i - 1 ] == '\\' )
					{
						break;
					}
				}

				//////////////////////////////////////////////////////////////////////////

				C2DResSetUnit * pUnit = new C2DResSetUnit();
				pUnit->SetIsSubUnit(false);
				//pUnit->AddName(str.GetBuffer()+nPos+5);

				//////////////////////////////////////////////////////////////////////////
				//得到相对名
				string texname = str;
				FindSubString(texname, "DATA");
				//////////////////////////////////////////////////////////////////////////

				//pUnit->AddName(str.GetBuffer());
				pUnit->AddName(texname.c_str());
				pGroup->m_resSetUnits.push_back( pUnit );
				
				str.SetAt(nPad,'\0');

				char str2[64];

				strcpy(str2,str.GetBuffer()+i);
				
				Item.item.lParam = (LPARAM)pUnit;
				Item.item.pszText = str2;	
				m_SetView.InsertItem( &Item );
			}
			m_SetView.Expand( TItem.hItem, TVE_EXPAND );
		}
	}
}

void CResSetView::OnBnClickedCreateSingleUnit()
{
	// TODO: Add your control notification handler code here
	
	HTREEITEM CurSel = m_SetView.GetSelectedItem();
	if( !CurSel )
		return;
	TVITEM TItem;
	TItem.hItem = CurSel;
	TItem.mask = TVIF_IMAGE|TVIF_PARAM;
	m_SetView.GetItem( &TItem );
	
	if( TItem.iImage != 1 )
		return;
	
	AddSingleUnitForSelectedGroup(TItem);
}

void CResSetView::OnBnClickedDeletegroup()
{
	// TODO: Add your control notification handler code here
	HTREEITEM CurSel = m_SetView.GetSelectedItem();
	if( !CurSel )
		return;
	TVITEM TItem;
	TItem.hItem = CurSel;
	TItem.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE;
	m_SetView.GetItem( &TItem );
	if( TItem.iImage != 1 )
		return;

	HTREEITEM Parent = m_SetView.GetParentItem( CurSel );
	CDimSet* DimSet = (CDimSet*)m_SetView.GetItemData( Parent );
	m_SetView.DeleteItem( CurSel );
	
	for( int i = 0; i < (int)DimSet->m_resSetGroups.size(); i++ )
	{
		if ( TItem.lParam == (LPARAM)DimSet->m_resSetGroups[i] )
		{
			DimSet->m_resSetGroups.erase( DimSet->m_resSetGroups.begin() + i );
			break;
		}
	}
}

void CResSetView::OnBnClickedDeleteunit()
{
	HTREEITEM CurSel = m_SetView.GetSelectedItem();
	if( !CurSel )
		return;
	TVITEM TItem;
	TItem.hItem = CurSel;
	TItem.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE;
	m_SetView.GetItem( &TItem );
	if( TItem.iImage != 3 && TItem.iImage != 4 && TItem.iImage != 9 && TItem.iImage != 10  )
		return;
	HTREEITEM Parent = m_SetView.GetParentItem( CurSel );
	CResSetGroup * Group = (CResSetGroup*)m_SetView.GetItemData( Parent );
	m_SetView.DeleteItem( CurSel );
	
	for ( int i = 0; i < (int)Group->m_resSetUnits.size(); i++ )
	{
		if ( TItem.lParam == (LPARAM)Group->m_resSetUnits[i] )
		{
			Group->m_resSetUnits.erase( Group->m_resSetUnits.begin() + i );
			break;
		}
	}
}

void CResSetView::OnTvnSelchangedSetview(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	//HTREEITEM CurSel = m_SetView.GetSelectedItem();
	//if( !CurSel )
	//	return;
	//TVITEM TItem;
	//TItem.hItem = CurSel;
	//TItem.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE;
	//m_SetView.GetItem( &TItem );
	//
	////if( TItem.iImage == 3 )
	////{
	////	CTileEditApp::GetInstance()->GetOperator()->SetEditState(COperator::EES_SET_TILE_FACE);
	////	m_eEditState = COperator::EES_SET_TILE_FACE;
	////}

	//if( TItem.iImage == 8 )
	//{
	//	CTileEditApp::GetInstance()->GetOperator()->SetEditState(COperator::EES_SET_TILE_FACE);
	//	m_eEditState = COperator::EES_SET_TILE_FACE;

	//	if ( GetAsyncKeyState(VK_LCONTROL) & 0x8000 )
	//	{

	//		HTREEITEM Parent = m_SetView.GetParentItem( CurSel );
	//		CResSetGroup * pGroup = (CResSetGroup*)m_SetView.GetItemData( Parent );
	//		
	//		CResSetUnit * pUnit;

	//		for( int i = 0; i < (int)pGroup->m_resSetUnits.size(); i++ )
	//		{
	//			if( TItem.lParam == (LPARAM)pGroup->m_resSetUnits[i] )
	//			{
	//				pUnit = pGroup->m_resSetUnits[i];
	//				break;
	//			}
	//		}
	//
	//		CTileEditApp::GetInstance()->GetOperator()->CreateActiveObject(pUnit->GetName(),pUnit->GetIsSubUnit());
	//	}
	//}
	
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
//
//void CResSetView::InvalidResTree()
//{
//	m_SetView.Select(0,TVGN_CARET);
//}

BOOL CResSetView::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此添加专用代码和/或调用基类
	
	int i = (int)wParam;
	
	switch ( message ) 
	{
	case WM_LIFT:
		{
			m_pActiveUnit->SetLift(float(i));
		}
	break;
	
	case WM_SHIFTX:
		{
			m_pActiveUnit->SetShiftX(float(i));
		}
	break;
	
	case WM_SHIFTZ:
		{
			m_pActiveUnit->SetShiftZ(float(i));
		}
	break;

	case WM_SETHEIGHTSIZE:
		{
			if ( m_pActiveUnit->GetModelHeightSize(wParam) == 0 )
				m_pActiveUnit->SetModelHeightSize(wParam,lParam);
		}
	break;

	case WM_SETBLOCKSIZE:
		{
			if ( m_pActiveUnit->GetModelBlockSize(wParam) == 0 )
				m_pActiveUnit->SetModelBlockSize(wParam,lParam);
		}
	break;

	case WM_BLOCKMASK_SETBLOCKMASK:
		{
			m_pActiveUnit->FlipModelBlockMask(wParam,lParam);
		}
	break;

	case WM_BLOCKTYPE_SETBLOCKTYPE:
		{
			CToolSetBlockType::SBlockTypeExInfo * pInfo = (CToolSetBlockType::SBlockTypeExInfo*)(wParam);
			m_pActiveUnit->SetModelBlockType(pInfo->nModelIndex,pInfo->nGridIndexIndex,pInfo->nBlockType);
		}
	break;
	
	case WM_BLOCKLOGICHEIGHT_SETBLOCKLOGICHEIGHT:
		{
			CToolSetBlockLogicHeight::SBlockLogicHeightExInfo * pInfo = (CToolSetBlockLogicHeight::SBlockLogicHeightExInfo*)(wParam);
			m_pActiveUnit->SetModelLogicHeight(pInfo->nModelIndex,pInfo->nVertexIndexIndex,pInfo->fLogicHeight);
		}
	break;

	case WM_BLOCKTYPE_LOCKGRIDS:
		{
			CheckDlgButton(IDC_LOWBLOCK,false);
			CheckDlgButton(IDC_MIDBLOCK,false);
			CheckDlgButton(IDC_HIGHBLOCK,false);

			DWORD dwBlockType = wParam;

			if ( dwBlockType == 0xff )
			{
				break;
			}
			else
			{	
				if ( dwBlockType == 1 )
					CheckDlgButton(IDC_LOWBLOCK,true);
				else if ( dwBlockType == 2 )
					CheckDlgButton(IDC_MIDBLOCK,true);
				else if ( dwBlockType == 3 )
					CheckDlgButton(IDC_HIGHBLOCK,true);
			}
		}
	break;

	case WM_EDITMODECHANGE:
		{
			/*m_eEditState = (COperator::EEDIT_STATE)wParam;
			
			switch( m_eEditState )
			{
			
			case COperator::EES_SET_BLOCK_TYPE:
			{
				
				GetDlgItem(IDC_LOWBLOCK)->EnableWindow(true);
				GetDlgItem(IDC_MIDBLOCK)->EnableWindow(true);
				GetDlgItem(IDC_HIGHBLOCK)->EnableWindow(true);
				CheckDlgButton(IDC_BLOCKMASK,false);
				CheckDlgButton(IDC_BLOCKTYPE,true);
				CheckDlgButton(IDC_HEIGHT,false);
			}
			break;*/

			/*case COperator::EES_SET_BLOCK_MASK:
				{
					GetDlgItem(IDC_LOWBLOCK)->EnableWindow(false);
					GetDlgItem(IDC_MIDBLOCK)->EnableWindow(false);
					GetDlgItem(IDC_HIGHBLOCK)->EnableWindow(false);
					CheckDlgButton(IDC_BLOCKMASK,true);
					CheckDlgButton(IDC_BLOCKTYPE,false);
					CheckDlgButton(IDC_HEIGHT,false);
				}
			break;*/

		/*	case COperator::EES_SET_BLOCK_LOGIC_HEIGHT:
				{
					GetDlgItem(IDC_LOWBLOCK)->EnableWindow(false);
					GetDlgItem(IDC_MIDBLOCK)->EnableWindow(false);
					GetDlgItem(IDC_HIGHBLOCK)->EnableWindow(false);
					CheckDlgButton(IDC_BLOCKMASK,false);
					CheckDlgButton(IDC_BLOCKTYPE,false);
					CheckDlgButton(IDC_HEIGHT,true);
				}
			break;*/

			/*case COperator::EES_NONE:
				{
					GetDlgItem(IDC_LOWBLOCK)->EnableWindow(false);
					GetDlgItem(IDC_MIDBLOCK)->EnableWindow(false);
					GetDlgItem(IDC_HIGHBLOCK)->EnableWindow(false);
					CheckDlgButton(IDC_BLOCKMASK,false);
					CheckDlgButton(IDC_BLOCKTYPE,false);
					CheckDlgButton(IDC_HEIGHT,false);
				}
			break;*/
			//}
		}
	break;
	
	}

	return CFormView::OnWndMsg(message, wParam, lParam, pResult);
}

void CResSetView::OnDestroy()
{
	CFormView::OnDestroy();
	
	//if ( m_curSelResSet )
	//{
	//	CDomXmlDocument doc;
	//	ofstream out((m_curSelResSet->m_resSetName + ".arr").c_str(),ios::binary);
	//	
	//	doc.InsertNodeFirst("俑兵天下图素集");
	//			
	//	for ( int i = 0; i < m_curSelResSet->m_3DResSet.m_resSetGroups.size(); i++)
	//	{
	//		CResSetGroup * pResSetGroup = m_curSelResSet->m_3DResSet.m_resSetGroups[i];
	//		
	//		doc("俑兵天下图素集").InsertNodeLast("ResSetGroup");
	//		
	//		doc("俑兵天下图素集")(i).InsertAttributeLast("ResSetGroupName",pResSetGroup->m_groupName);

	//		for ( int j = 0; j < pResSetGroup->m_resSetUnits.size(); j++)
	//		{
	//			C3DResSetUnit * pResSetUnit = (C3DResSetUnit*)(pResSetGroup->m_resSetUnits[j]);
	//			doc("俑兵天下图素集")(i).InsertNodeLast("ResSetUnit");
	//			doc("俑兵天下图素集")(i)(j).InsertAttributeLast("ResSetUnitArpCount",pResSetUnit->GetNameSize());

	//			for ( int k = 0; k < pResSetUnit->GetNameSize(); ++k ) 
	//			{
	//				char s[16];
	//				string str = pResSetUnit->GetNameByIndex(k);
	//				sprintf(s,"%d",k);
	//				doc("俑兵天下图素集")(i)(j).InsertAttributeLast(s,str);
	//			}
	//			
	//			doc("俑兵天下图素集")(i)(j).InsertAttributeLast("IsSubUnit",pResSetUnit->GetIsSubUnit());
	//			doc("俑兵天下图素集")(i)(j).InsertAttributeLast("BlockType",pResSetUnit->GetBlockType());
	//		}
	//	}
	//	out << doc;
	//}
}

void CResSetView::OnTvnItemexpandedSetview(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	// TODO: 在此添加控件通知处理程序代码
	
	//if ( pNMTreeView->itemNew.iImage == 5 )
	//{
	//	HTREEITEM Parent = m_SetView.GetParentItem(pNMTreeView->itemNew.hItem);
	//
	//	TVITEM TItem;
	//	TItem.hItem = Parent;
	//	TItem.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE;
	//	m_SetView.GetItem( &TItem );

	//	if ( TItem.iImage == 3 )
	//	{
	//		CTileEditApp::GetInstance()->GetOperator()->SetEditState(COperator::EES_SET_TILE_FACE);
	//		m_eEditState = COperator::EES_SET_TILE_FACE;
	//	}
	//}
	//
	//if ( pNMTreeView->itemNew.iImage == 3 )
	//{
	//	CTileEditApp::GetInstance()->GetOperator()->SetEditState(COperator::EES_SET_TILE_FACE);
	//	m_eEditState = COperator::EES_SET_TILE_FACE;
	//}

	*pResult = 0;
}

void CResSetView::OnBnClickedCreateComboUnit()
{
	HTREEITEM CurSel = m_SetView.GetSelectedItem();
	if( !CurSel )
		return;
	TVITEM TItem;
	TItem.hItem = CurSel;
	TItem.mask = TVIF_IMAGE|TVIF_PARAM;
	m_SetView.GetItem( &TItem );
	if( TItem.iImage != 1 )
		return;
	
	int Image, SelImage;
	m_SetView.GetItemImage( m_SetView.GetParentItem( TItem.hItem ), Image, SelImage );
	
	if ( Image == 5 )
	{
		return;
	}
	
	AddComboUnitForSelectedGroup(TItem);
}

void CResSetView::OnResetActiveObject()
{
	CTileEditorModel::ECenter_Type eCenterType;
	
	if ( m_pActiveUnit->GetCenterType() == CTileEditorModel::ECT_NONE )
	{
		eCenterType = CTileEditorModel::ECT_CENTER;
	}
	else
	{
		eCenterType = CTileEditorModel::ECenter_Type(m_pActiveUnit->GetCenterType());
	}

	CTileEditApp::GetInstance()->GetOperator()->CreateActiveObject(m_pActiveUnit->GetName(),m_pActiveUnit->GetTotalBlock(), m_pActiveUnit->GetTotalHeight(), m_pActiveUnit->GetIsSubUnit(),eCenterType, m_pActiveUnit->GetLift(), m_pActiveUnit->GetShiftX(), m_pActiveUnit->GetShiftZ());

	std::string strAni = m_pActiveUnit->GetAraFileName();

	if ( !strAni.empty() )
	{
		CTileEditApp::GetInstance()->GetOperator()->GetActiveObject()->GetFace()->SetAnimationByAraFileName(strAni);
	}

	if ( m_pActiveUnit->m_pEffectGroup )
	{
		CTileEditApp::GetInstance()->GetOperator()->ToolSetModel_LinkEffect(m_pActiveUnit->m_strEffectName,m_pActiveUnit->m_pEffectGroup);
	}

	//CTileEditApp::GetInstance()->GetOperator()->SetEditState(m_eEditState);

	switch ( eCenterType )
	{
	case CTileEditorModel::ECT_CENTER :
		{
			CheckDlgButton(IDC_RADIO5,true);
			CheckDlgButton(IDC_RADIO4,false);
			CheckDlgButton(IDC_RADIO3,false);
			CheckDlgButton(IDC_RADIO6,false);
		}
		break;

	case CTileEditorModel::ECT_CORNER :
		{
			CheckDlgButton(IDC_RADIO3,true);
			CheckDlgButton(IDC_RADIO4,false);
			CheckDlgButton(IDC_RADIO5,false);
			CheckDlgButton(IDC_RADIO6,false);
		}
		break;

	case CTileEditorModel::ECT_XMIDWAY :
		{
			CheckDlgButton(IDC_RADIO4,true);
			CheckDlgButton(IDC_RADIO5,false);
			CheckDlgButton(IDC_RADIO3,false);
			CheckDlgButton(IDC_RADIO6,false);
		}
		break;

	case CTileEditorModel::ECT_ZMIDWAY :
		{
			CheckDlgButton(IDC_RADIO6,true);
			CheckDlgButton(IDC_RADIO4,false);
			CheckDlgButton(IDC_RADIO3,false);
			CheckDlgButton(IDC_RADIO5,false);
		}
		break;
	}

	/*CFace::ETrans_Type eTransType = m_pActiveUnit->GetTransType();

	switch( eTransType )
	{
	case CFace::ETT_TRANSLUCENT:
		{
			CheckDlgButton(IDC_TRANSLUCENT,true);
			CheckDlgButton(IDC_SOLID,false);
		}
		break;

	case CFace::ETT_SOLID:
		{
			CheckDlgButton(IDC_TRANSLUCENT,false);
			CheckDlgButton(IDC_SOLID,true);
		}
	break;
	}*/
}

void CResSetView::OnNMClickSetview(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint myPoint;
	
	::GetCursorPos(&myPoint); 
	m_SetView.ScreenToClient(&myPoint);

	//// select the item that is at the point myPoint.
	UINT uFlags;
	HTREEITEM CurSel = m_SetView.HitTest(myPoint, &uFlags);

	if( !CurSel )
		return;
	TVITEM TItem;
	TItem.hItem = CurSel;
	TItem.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE;
	m_SetView.GetItem( &TItem );

	if ( TItem.iImage == 3 || TItem.iImage == 4 )
	{
		if ( GetAsyncKeyState(VK_LCONTROL) & 0x8000 )
		{		
			eg::CEngine::GetInstance()->GetActiveCamera()->MoveToRTSByView(D3DXVECTOR3(REGION_SPACE/2.0f*64.0f,0.0f,REGION_SPACE/2.0f*64.0f));
			
			m_pActiveUnit = (C3DResSetUnit*)TItem.lParam;

			// change the icon
			if ( !m_pActiveUnit->GetIsEdited() )
			{
				m_SetView.SetItem(CurSel,TVIF_IMAGE|TVIF_SELECTEDIMAGE,NULL,4,4,0,0,0);
				m_pActiveUnit->SetIsEdited(true);
			}		
			
			CTileEditorModel::ECenter_Type eCenterType;
			
			if ( m_pActiveUnit->GetCenterType() == CTileEditorModel::ECT_NONE )
			{
				eCenterType = CTileEditorModel::ECT_CENTER;
			}
			else
			{
				eCenterType = CTileEditorModel::ECenter_Type(m_pActiveUnit->GetCenterType());
			}
			
			CTileEditApp::GetInstance()->GetOperator()->CreateActiveObject(m_pActiveUnit->GetName(),m_pActiveUnit->GetTotalBlock(), m_pActiveUnit->GetTotalHeight(), m_pActiveUnit->GetIsSubUnit(),eCenterType, m_pActiveUnit->GetLift(), m_pActiveUnit->GetShiftX(), m_pActiveUnit->GetShiftZ());
			
			std::string strAni = m_pActiveUnit->GetAraFileName();
			
			if ( !strAni.empty() )
			{
				CTileEditApp::GetInstance()->GetOperator()->GetActiveObject()->GetFace()->SetAnimationByAraFileName(strAni);
			}
			
			if ( m_pActiveUnit->m_pEffectGroup )
			{
				CTileEditApp::GetInstance()->GetOperator()->ToolSetModel_LinkEffect(m_pActiveUnit->m_strEffectName,m_pActiveUnit->m_pEffectGroup);
			}
			
			//CTileEditApp::GetInstance()->GetOperator()->SetEditState(m_eEditState);
			
			switch ( eCenterType )
			{
			case CTileEditorModel::ECT_CENTER :
				{
					CheckDlgButton(IDC_RADIO5,true);
					CheckDlgButton(IDC_RADIO4,false);
					CheckDlgButton(IDC_RADIO3,false);
					CheckDlgButton(IDC_RADIO6,false);
				}
				break;
			
			case CTileEditorModel::ECT_CORNER :
				{
					CheckDlgButton(IDC_RADIO3,true);
					CheckDlgButton(IDC_RADIO4,false);
					CheckDlgButton(IDC_RADIO5,false);
					CheckDlgButton(IDC_RADIO6,false);
				}
				break;
			
			case CTileEditorModel::ECT_XMIDWAY :
				{
					CheckDlgButton(IDC_RADIO4,true);
					CheckDlgButton(IDC_RADIO5,false);
					CheckDlgButton(IDC_RADIO3,false);
					CheckDlgButton(IDC_RADIO6,false);
				}
				break;
			
			case CTileEditorModel::ECT_ZMIDWAY :
				{
					CheckDlgButton(IDC_RADIO6,true);
					CheckDlgButton(IDC_RADIO4,false);
					CheckDlgButton(IDC_RADIO3,false);
					CheckDlgButton(IDC_RADIO5,false);
				}
			break;
			}
			
			/*CFace::ETrans_Type eTransType = m_pActiveUnit->GetTransType();

			switch( eTransType )
			{
			case CFace::ETT_TRANSLUCENT:
				{
					CheckDlgButton(IDC_TRANSLUCENT,true);
					CheckDlgButton(IDC_SOLID,false);
				}
			break;
			
			case CFace::ETT_SOLID:
				{
					CheckDlgButton(IDC_TRANSLUCENT,false);
					CheckDlgButton(IDC_SOLID,true);
				}
			break;
			}*/
			
			bool bShadow = m_pActiveUnit->GetShadow();
			CheckDlgButton(IDC_SHADOW,bShadow);
		}
	}
		
	//////////////////////////////////////////////////////////////////////////

	if ( TItem.iImage == 9 || TItem.iImage == 10 )
	{
		if ( GetAsyncKeyState(VK_LCONTROL) & 0x8000 )
		{	
			m_pActive2DUnit = (C2DResSetUnit*)TItem.lParam;

			//GetDlgItem(IDC_TRANSVERTEX)->EnableWindow(true);
			//GetDlgItem(IDC_TRANSTEXTURE)->EnableWindow(true);
					
			int nTextureHandle = CSharedTexture::GetTexture(m_pActive2DUnit->GetNameByIndex(0))->GetTextureHandle();
			
			CTerrainMesh * pTerrain = CTileEditApp::GetInstance()->GetTerrainMesh();
			
			for ( size_t i = 0; i < pTerrain->GetGridCount(); ++i )
			{
				pTerrain->SetGridTextureHandle(0,i,nTextureHandle);
			}

			pTerrain->RebuildRenderTree();

			//if ( m_pActive2DUnit->GetTransType() == CTerrainMesh::CTextureNode::ETT_VERTEX )
			//{
			//	CheckDlgButton(IDC_TRANSVERTEX,true);
			//	CheckDlgButton(IDC_TRANSTEXTURE,false);
			//}
			//else
			//{
			//	CheckDlgButton(IDC_TRANSVERTEX,false);
			//	CheckDlgButton(IDC_TRANSTEXTURE,true);
			//}
		}
	}
	
	*pResult = 0;
}

void CResSetView::AddComboUnitForSelectedGroup( TVITEM& TItem )
{
	CResSetGroup * pGroup = (CResSetGroup*)TItem.lParam;
	TVINSERTSTRUCT Item;
	Item.hParent = TItem.hItem;
	Item.hInsertAfter = TVI_LAST;
	Item.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_TEXT|TVIF_SELECTEDIMAGE;
	Item.item.iImage = 3;
	Item.item.iSelectedImage = 3;

	static char Filter[] = "Static Model Files (*.arp)|*.arp||";

	C3DResSetUnit * pUnit = new C3DResSetUnit();

	CFileDialog Dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT|OFN_NOCHANGEDIR, Filter );
	
	char s[1024];
	memset(s,' ',1024);
	s[1023] = 0;
	Dlg.GetOFN().nMaxFile = 1024;
	Dlg.GetOFN().lpstrFile = s;
	
	if ( Dlg.DoModal() == IDOK )
	{
		POSITION Pos = Dlg.GetStartPosition();
		
		while ( Pos )
		{
			CString str = Dlg.GetNextPathName( Pos );
			str.MakeUpper();
			// int nPos = str.Find("DATA");
			int nPos = EFind( str, string("DATA"));
			for( int i = str.GetLength(); i > 0; i-- )
				if( str[ i - 1 ] == '/' || str[ i - 1 ] == '\\' )
					break;

			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			//得到相对名
			string modelname = str;
			FindSubString(modelname, "DATA");
			pUnit->AddName(modelname.c_str());
			//////////////////////////////////////////////////////////////////////////
		
			//pUnit->AddName(str.GetBuffer()+nPos+5);
			//pUnit->AddName(str.GetBuffer());
		}
		
		pUnit->SetTotalBlockSize(pUnit->GetName().size());
		pUnit->SetTotalHeightSize(pUnit->GetName().size());
		pUnit->SetIsSubUnit(true);
		pUnit->SetCenterType(0); // center
		pGroup->m_resSetUnits.push_back( pUnit );
		
		Item.item.lParam = (LPARAM)pUnit;
		char str[64];
		strcpy(str,pUnit->GetNameByIndex(0).c_str());
		char * pEndPos = strrchr(str,'.');
		*((char*)(pEndPos-1)) = '\0';
		char * pStartPos;
		if ( ( pStartPos = strrchr(str,'\\')) == NULL )
			pStartPos = strrchr(str,'/');
		Item.item.pszText = pStartPos+1;
		m_SetView.InsertItem( &Item );
				
		m_SetView.Expand( TItem.hItem, TVE_EXPAND );
	}

}

void CResSetView::OnBnClickedCenter()
{
	CTileEditApp::GetInstance()->GetOperator()->SetCenterType(CTileEditorModel::ECT_CENTER);
	
	if ( m_pActiveUnit )
	{
		m_pActiveUnit->SetCenterType(CTileEditorModel::ECT_CENTER);
	}
}

void CResSetView::OnBnClickedCorner()
{
	CTileEditApp::GetInstance()->GetOperator()->SetCenterType(CTileEditorModel::ECT_CORNER);

	if ( m_pActiveUnit )
	{
		m_pActiveUnit->SetCenterType(CTileEditorModel::ECT_CORNER);
	}
}

void CResSetView::OnBnClickedXMidWay()
{
	CTileEditApp::GetInstance()->GetOperator()->SetCenterType(CTileEditorModel::ECT_XMIDWAY);

	if ( m_pActiveUnit )
	{
		m_pActiveUnit->SetCenterType(CTileEditorModel::ECT_XMIDWAY);
	}
}

void CResSetView::OnBnClickedZMidWay()
{
	CTileEditApp::GetInstance()->GetOperator()->SetCenterType(CTileEditorModel::ECT_ZMIDWAY);

	if ( m_pActiveUnit )
	{
		m_pActiveUnit->SetCenterType(CTileEditorModel::ECT_ZMIDWAY);
	}
}

void CResSetView::OnBnClickedCreateresset()
{
	UpdateData();
	
	if ( m_resSetName.IsEmpty() )
	{
		MessageBox("新创建图素包名字不能为空", "错误");
		return;
	}
	
	m_curSelResSet = new CResSet;
	m_curSelResSet->m_resSetName = m_resSetName.GetBuffer();
	m_resSets.push_back( m_curSelResSet );
	
	int index = m_listResSet.InsertItem( m_listResSet.GetItemCount(), m_curSelResSet->m_resSetName.c_str() );
	m_listResSet.SetItemData(index, (LPARAM)m_curSelResSet);
}

void CResSetView::OnBnClickedDeleteresset()
{
	std::string str = "..\\Tools\\TileEditor\\ResourceSet\\" + m_curSelResSet->m_resSetName + ".ars";

	::DeleteFile(str.c_str());
	
	for( int i = 0; i < (int)m_resSets.size(); i++ )
		if( m_curSelResSet == m_resSets[i] )
			m_resSets.erase( m_resSets.begin() + i );
	
	for( int i = 0; i < m_listResSet.GetItemCount(); i++ )
	{
		if( m_curSelResSet == (CResSet*)m_listResSet.GetItemData( i ) )
		{
			m_listResSet.SetItemData( i, 0 );
			m_listResSet.DeleteItem( i );
			return;
		}
	}
}

void CResSetView::OnLvnItemchangedRessetlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	*pResult = 0;
	CResSet * Temp = (CResSet*)m_listResSet.GetItemData( pNMLV->iItem );
	m_curSelResSet = Temp;
	ResetResSetView();
}

void CResSetView::OnSv()
{
	//for ( int m = 0; m < m_resSets.size(); ++m )
	//{
	//		std::string str = "..\\Tools\\TileEditor\\ResourceSet\\" + m_resSets[m]->m_resSetName + ".ars";
	//		
	//		//////////////////////////////////////////////////////////////////////////
	//		
	//		ofstream out(str.c_str(),ios::binary);

	//		CSaxXmlWriter Writer(&out);		 

	//		Writer.ElementBegin("x");

	//		Writer.ElementBegin("俑兵天下图素集3D");

	//		for ( int i = 0; i < m_resSets[m]->m_3DResSet.m_resSetGroups.size(); i++)
	//		{
	//			CResSetGroup * pResSetGroup = m_resSets[m]->m_3DResSet.m_resSetGroups[i];

	//			Writer.ElementBegin("ResSetGroup");

	//			Writer.Attribute("ResSetGroupName",pResSetGroup->m_groupName);
	//			//doc("俑兵天下图素集3D")(i).InsertAttributeLast("ResSetGroupName",pResSetGroup->m_groupName);

	//			for ( int j = 0; j < pResSetGroup->m_resSetUnits.size(); j++)
	//			{
	//				C3DResSetUnit * pResSetUnit = (C3DResSetUnit*)(pResSetGroup->m_resSetUnits[j]);

	//				Writer.ElementBegin("ResSetUnit");

	//				Writer.Attribute("ResSetUnitArpCount",pResSetUnit->GetNameSize());

	//				// arps
	//				for ( int k = 0; k < pResSetUnit->GetNameSize(); ++k ) 
	//				{
	//					char s[64];
	//					string str = pResSetUnit->GetNameByIndex(k);
	//					sprintf(s,"%dName",k);
	//					Writer.Attribute(s,str);

	//					//////////////////////////////////////////////////////////////////////////

	//					// block

	//					std::vector<CTileEditorModel::SBlockInfo> vecBlock = pResSetUnit->GetModelBlock(k);
	//					sprintf(s,"%dBlockSize",k);
	//					Writer.Attribute(s,vecBlock.size());

	//					std::vector<CTileEditorModel::SBlockInfo>::iterator iter = vecBlock.begin();

	//					for ( int l = 0 ; iter != vecBlock.end(); ++iter, ++l )
	//					{
	//						// block mask
	//						sprintf(s,"%d-%dBlockMask",k,l);
	//						Writer.Attribute(s,(*iter).nBlockMask);

	//						// block type
	//						sprintf(s,"%d-%dBlockType",k,l);
	//						Writer.Attribute(s,(*iter).nBlockType);
	//					}

	//					//////////////////////////////////////////////////////////////////////////

	//					// height

	//					std::vector<CTileEditorModel::SLogicHeightInfo> vecHeight = pResSetUnit->GetModelHeight(k);
	//					sprintf(s,"%dHeightSize",k);
	//					Writer.Attribute(s,vecHeight.size());

	//					std::vector<CTileEditorModel::SLogicHeightInfo>::iterator iterH = vecHeight.begin();

	//					for ( int l = 0 ; iterH != vecHeight.end(); ++iterH, ++l )
	//					{
	//						// block logic height
	//						sprintf(s,"%d-%dLogicHeight",k,l);
	//						Writer.Attribute(s,(*iterH).fLogicHeight);
	//					}

	//				}

	//				// offset delta
	//				Writer.Attribute("Lift",pResSetUnit->GetLift());
	//				Writer.Attribute("ShiftX",pResSetUnit->GetShiftX());
	//				Writer.Attribute("ShiftZ",pResSetUnit->GetShiftZ());

	//				// is sub unit
	//				Writer.Attribute("IsSubUnit",pResSetUnit->GetIsSubUnit());

	//				// block type
	//				Writer.Attribute("CenterType",pResSetUnit->GetCenterType());

	//				// is edited
	//				Writer.Attribute("IsEdited",pResSetUnit->GetIsEdited());

	//				// trans type
	//				Writer.Attribute("TransType",pResSetUnit->GetTransType());

	//				// shadow type
	//				Writer.Attribute("Shadow",pResSetUnit->GetShadow());

	//				// ani file name
	//				Writer.Attribute("AraFileName",pResSetUnit->GetAraFileName());

	//				// effect file name
	//				if ( pResSetUnit->m_pEffectGroup )
	//				{
	//					Writer.Attribute("EffectBound",true);
	//					Writer.Attribute("EffectName",pResSetUnit->m_strEffectName);
	//				}
	//				else
	//					Writer.Attribute("EffectBound",false);

	//				Writer.ElementEnd();
	//			}

	//			Writer.ElementEnd();
	//		}

	//		Writer.ElementEnd();

	//		//////////////////////////////////////////////////////////////////////////

	//		Writer.ElementBegin("俑兵天下图素集2D");

	//		for ( int i = 0; i < m_resSets[m]->m_2DResSet.m_resSetGroups.size(); i++)
	//		{
	//			CResSetGroup * pResSetGroup = m_resSets[m]->m_2DResSet.m_resSetGroups[i];

	//			Writer.ElementBegin("ResSetGroup");

	//			Writer.Attribute("ResSetGroupName",pResSetGroup->m_groupName);

	//			for ( int j = 0; j < pResSetGroup->m_resSetUnits.size(); j++)
	//			{
	//				C2DResSetUnit * pResSetUnit = (C2DResSetUnit*)(pResSetGroup->m_resSetUnits[j]);

	//				Writer.ElementBegin("ResSetUnit");

	//				string str = pResSetUnit->GetNameByIndex(0);

	//				Writer.Attribute("TextureFileName",str);

	//				// is sub unit
	//				Writer.Attribute("IsSubUnit",pResSetUnit->GetIsSubUnit());

	//				// is edited
	//				Writer.Attribute("IsEdited",pResSetUnit->GetIsEdited());

	//				// trans type
	//				Writer.Attribute("TransType",pResSetUnit->GetTransType());

	//				Writer.ElementEnd();
	//			}

	//			Writer.ElementEnd();
	//		}

	//		Writer.ElementEnd();

	//		Writer.ElementEnd();
	//}
	//	
	//	
	//	
	//	
		
	if ( m_curSelResSet )
	{
		std::string strSave = "..\\Tools\\TileEditor\\ResourceSet\\log.log";
		std::string str = "..\\Tools\\TileEditor\\ResourceSet\\" + m_curSelResSet->m_resSetName + ".ars";
				
		ofstream log(strSave.c_str(),ios::binary);
		
		CSaxXmlWriter LogWriter(&log);	
		
		LogWriter.ElementBegin("log");
		LogWriter.Attribute("LastOne",str);
		LogWriter.ElementEnd();
		
		//////////////////////////////////////////////////////////////////////////
		
		ofstream out(str.c_str(),ios::binary);
		
		CSaxXmlWriter Writer(&out);		 

		Writer.ElementBegin("x");

		Writer.ElementBegin("俑兵天下图素集3D");
		//doc.InsertNodeFirst("俑兵天下图素集3D");

		for ( size_t i = 0; i < m_curSelResSet->m_3DResSet.m_resSetGroups.size(); i++)
		{
			CResSetGroup * pResSetGroup = m_curSelResSet->m_3DResSet.m_resSetGroups[i];

			Writer.ElementBegin("ResSetGroup");

			Writer.Attribute("ResSetGroupName",pResSetGroup->m_groupName);
			//doc("俑兵天下图素集3D")(i).InsertAttributeLast("ResSetGroupName",pResSetGroup->m_groupName);

			for ( size_t j = 0; j < pResSetGroup->m_resSetUnits.size(); j++)
			{
				C3DResSetUnit * pResSetUnit = (C3DResSetUnit*)(pResSetGroup->m_resSetUnits[j]);

				Writer.ElementBegin("ResSetUnit");

				Writer.Attribute("ResSetUnitArpCount",pResSetUnit->GetNameSize());
				
				// arps
				for ( int k = 0; k < pResSetUnit->GetNameSize(); ++k ) 
				{
					char s[128];
					string str = pResSetUnit->GetNameByIndex(k);
					sprintf(s,"%dName",k);
					Writer.Attribute(s,str);

					//////////////////////////////////////////////////////////////////////////

					// block

					std::vector<CTileEditorModel::SBlockInfo> vecBlock = pResSetUnit->GetModelBlock(k);
					sprintf(s,"%dBlockSize",k);
					Writer.Attribute(s,vecBlock.size());

					std::vector<CTileEditorModel::SBlockInfo>::iterator iter = vecBlock.begin();

					for ( int l = 0 ; iter != vecBlock.end(); ++iter, ++l )
					{
						// block mask
						sprintf(s,"%d-%dBlockMask",k,l);
						Writer.Attribute(s,(*iter).nBlockMask);

						// block type
						sprintf(s,"%d-%dBlockType",k,l);
						Writer.Attribute(s,(*iter).nBlockType);
					}

					//////////////////////////////////////////////////////////////////////////

					// height

					std::vector<CTileEditorModel::SLogicHeightInfo> vecHeight = pResSetUnit->GetModelHeight(k);
					sprintf(s,"%dHeightSize",k);
					Writer.Attribute(s,vecHeight.size());

					std::vector<CTileEditorModel::SLogicHeightInfo>::iterator iterH = vecHeight.begin();

					for ( int l = 0 ; iterH != vecHeight.end(); ++iterH, ++l )
					{
						// block logic height
						sprintf(s,"%d-%dLogicHeight",k,l);
						Writer.Attribute(s,(*iterH).fLogicHeight);
					}

				}

				// offset delta
				Writer.Attribute("Lift",pResSetUnit->GetLift());
				Writer.Attribute("ShiftX",pResSetUnit->GetShiftX());
				Writer.Attribute("ShiftZ",pResSetUnit->GetShiftZ());

				// is sub unit
				Writer.Attribute("IsSubUnit",pResSetUnit->GetIsSubUnit());

				// block type
				Writer.Attribute("CenterType",pResSetUnit->GetCenterType());

				// is edited
				Writer.Attribute("IsEdited",pResSetUnit->GetIsEdited());

				// trans type
				Writer.Attribute("TransType",pResSetUnit->GetTransType());
				
				// shadow type
				Writer.Attribute("Shadow",pResSetUnit->GetShadow());
								
				// ani file name
				Writer.Attribute("AraFileName",pResSetUnit->GetAraFileName());
				
				// effect file name
				if ( pResSetUnit->m_pEffectGroup )
				{
					Writer.Attribute("EffectBound",true);
					Writer.Attribute("EffectName",pResSetUnit->m_strEffectName);
				}
				else
					Writer.Attribute("EffectBound",false);
				
				Writer.ElementEnd();
			}

			Writer.ElementEnd();
		}

		Writer.ElementEnd();
		
		//////////////////////////////////////////////////////////////////////////

		Writer.ElementBegin("俑兵天下图素集2D");

		for ( size_t i = 0; i < m_curSelResSet->m_2DResSet.m_resSetGroups.size(); i++)
		{
			CResSetGroup * pResSetGroup = m_curSelResSet->m_2DResSet.m_resSetGroups[i];

			Writer.ElementBegin("ResSetGroup");

			Writer.Attribute("ResSetGroupName",pResSetGroup->m_groupName);

			for ( size_t j = 0; j < pResSetGroup->m_resSetUnits.size(); j++)
			{
				C2DResSetUnit * pResSetUnit = (C2DResSetUnit*)(pResSetGroup->m_resSetUnits[j]);

				Writer.ElementBegin("ResSetUnit");

				string str = pResSetUnit->GetNameByIndex(0);

				Writer.Attribute("TextureFileName",str);

				// is sub unit
				Writer.Attribute("IsSubUnit",pResSetUnit->GetIsSubUnit());

				// is edited
				Writer.Attribute("IsEdited",pResSetUnit->GetIsEdited());

				// trans type
				Writer.Attribute("TransType",pResSetUnit->GetTransType());

				Writer.ElementEnd();
			}
			
			Writer.ElementEnd();
		}
		
		Writer.ElementEnd();
		
		Writer.ElementEnd();
	}
	
	std::string strSrc =  "..\\Tools\\TileEditor\\ResourceSet\\" + m_curSelResSet->m_resSetName + ".ars";
	std::string str = "..\\Tools\\TileEditor\\ResourceSet\\" + m_curSelResSet->m_resSetName + ".backars";
	CopyFile(strSrc.c_str(),str.c_str(),false);
}

void CResSetView::OnBnClickedBindani()
{
	HTREEITEM CurSel = m_SetView.GetSelectedItem();
	if( !CurSel )
		return;
	TVITEM TItem;
	TItem.hItem = CurSel;
	TItem.mask = TVIF_IMAGE|TVIF_PARAM;
	m_SetView.GetItem( &TItem );

	if ( TItem.iImage != 3 && TItem.iImage != 4 )
		return;

	if ( m_pActiveUnit )
	{
		static char Filter[] = " Model Animation Files (*.ara)|*.ara||";
		
		CFileDialog Dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_NOCHANGEDIR, Filter );
		
		if ( Dlg.DoModal() == IDOK )
		{
			POSITION Pos = Dlg.GetStartPosition();
			
			while ( Pos )
			{
				CString str = Dlg.GetNextPathName( Pos );
				str.MakeUpper();
				int nPos = EFind( str, string("DATA") );
				// int nPos = str.Find("DATA");				
				m_pActiveUnit->SetAnimationByAraFileName(str.GetBuffer() + nPos + 5);
			}
		}

		std::string strAni = m_pActiveUnit->GetAraFileName();

		if ( !strAni.empty() )
		{
			CTileEditApp::GetInstance()->GetOperator()->GetActiveObject()->GetFace()->SetAnimationByAraFileName(strAni);
		}
	}
}

void CResSetView::OnBnClickedTransvertex()
{
	if ( m_pActive2DUnit ) 
		m_pActive2DUnit->SetTransType(CTerrainMesh::CTextureNode::ETT_VERTEX);

	CheckDlgButton(IDC_TRANSVERTEX,true);
	CheckDlgButton(IDC_TRANSTEXTURE,false);
}

void CResSetView::OnBnClickedTranstexture()
{
	if ( m_pActive2DUnit )
		m_pActive2DUnit->SetTransType(CTerrainMesh::CTextureNode::ETT_TEXTURE);

	CheckDlgButton(IDC_TRANSVERTEX,false);
	CheckDlgButton(IDC_TRANSTEXTURE,true);
}

void CResSetView::OnNMClickEffectview(NMHDR *pNMHDR, LRESULT *pResult)
{
	if ( !m_pActiveUnit ) return;

	CPoint myPoint;

	::GetCursorPos(&myPoint); 
	m_effectView.ScreenToClient(&myPoint);

	// select the item that is at the point myPoint.
	UINT uFlags;
	HTREEITEM CurSel = m_effectView.HitTest(myPoint, &uFlags);

	if( !CurSel ) return;

	char s[128];
	TVITEM TItem;
	TItem.hItem = CurSel;
	TItem.pszText = s;
	TItem.cchTextMax = 64;
	TItem.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_TEXT;
	m_effectView.GetItem( &TItem );
	
	if ( TItem.iImage == 3 )
	{
		HTREEITEM Parent = m_effectView.GetParentItem( CurSel );
		CResSetGroup * pGroup = (CResSetGroup*)m_effectView.GetItemData( Parent );
		CResSetUnit * pUnit = (CResSetUnit*)TItem.lParam;
		
		HTREEITEM Parent2 = m_effectView.GetParentItem( Parent );
		CResSet * pResSet = (CResSet*)m_effectView.GetItemData(Parent2);
		
		//////////////////////////////////////////////////////////////////////////
		CTileEditApp::GetInstance()->GetOperator()->ToolSetModel_LinkEffect(pGroup->m_groupName+'\\'+pUnit->GetItemName(),pResSet->m_pEffectGroup);
						
		m_pActiveUnit->m_strEffectName = pGroup->m_groupName+'\\'+pUnit->GetItemName();
		m_pActiveUnit->m_pEffectGroup = pResSet->m_pEffectGroup;
	}
	
	*pResult = 0;
}

// now used to clear effect
void CResSetView::OnBnClickedBindeffect()
{
	if ( m_pActiveUnit )
	{
		m_pActiveUnit->m_pEffectGroup = NULL;
		this->OnResetActiveObject();
	}

	//static char Filter[] = " Effect Files (*.are)|*.are||";
	//
	//CFileDialog Dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT|OFN_NOCHANGEDIR, Filter );
	//
	//if ( Dlg.DoModal() == IDOK )
	//{
	//	POSITION Pos = Dlg.GetStartPosition();

	//	while ( Pos )
	//	{
	//		CString str = Dlg.GetNextPathName( Pos );
	//	
	//		//////////////////////////////////////////////////////////////////////////
	//		
	//		AddEffectSetFromFile(str.GetBuffer());
	//	}
	//}
}

void CResSetView::LoadLog()
{	std::string strSave = "..\\Tools\\TileEditor\\ResourceSet\\log.log";
	
	CDomXmlDocument doc("log");
	ifstream in(strSave.c_str());
	
	if ( !in ) return;
	
	in >> doc;
	
	this->AddResSetFromFile(doc["LastOne"].As<std::string>());
}

void CResSetView::AddEffectSetFromFile(std::string strFXFileName)
{
	CResSet * pFXSet = new CResSet;
	
	int i = strFXFileName.find_last_of('.');
	
	int nPos = strFXFileName.find("setting");
	
	pFXSet->m_resSetName = strFXFileName.substr(nPos+8,i-nPos-8);
	
	m_vecFXSets.push_back(pFXSet);
	
	//nPos = strFXFileName.find("data");
	  nPos = EFind2( strFXFileName , string("data") );
	
	if ( nPos != - 1 ) 
		strFXFileName = strFXFileName.substr(nPos+5);
	
	std::map< std::string, std::vector< std::string > > mapFXNames;
	
	CTileEditApp::GetInstance()->GetOperator()->GetFXNames(strFXFileName, mapFXNames, &pFXSet->m_pEffectGroup);
	
	std::map< std::string, std::vector< std::string > >::iterator iterGroup = mapFXNames.begin(); 
	
	for (; iterGroup != mapFXNames.end(); ++iterGroup)
	{
		CResSetGroup * pGroup = new CResSetGroup;
	
		pGroup->m_groupName = iterGroup->first;
	
		std::vector<std::string>::iterator iterFXItem = iterGroup->second.begin();
		
		for ( ; iterFXItem != iterGroup->second.end(); ++iterFXItem )
		{
			CResSetUnit * pResSetUnit  = new CResSetUnit;
	
			std::string str = *iterFXItem;
	
			pResSetUnit->SetItemName(str);
	
			pGroup->m_resSetUnits.push_back(pResSetUnit);
		}
	
		pFXSet->m_FXResSet.m_resSetGroups.push_back(pGroup);
	}
	
	AddFXSetTo3DTree(pFXSet);
	
	m_curFXSet = pFXSet;
}

void CResSetView::AddFXSetTo3DTree(CResSet * pResSet)
{
	if ( pResSet )
	{
		TVINSERTSTRUCT Item;
		Item.hParent = TVI_ROOT;
		Item.hInsertAfter = TVI_LAST;
		Item.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_TEXT|TVIF_SELECTEDIMAGE;
		Item.item.iImage = 0;
		Item.item.iSelectedImage = 0;
		Item.item.lParam = (LPARAM)(pResSet);
		Item.item.pszText = (LPSTR)pResSet->m_resSetName.c_str();		
		HTREEITEM hSetItem = m_effectView.InsertItem( &Item );
		
		AddFXGroupTo3DTree(hSetItem);
	}
}

void CResSetView::AddFXGroupTo3DTree(HTREEITEM hItem)
{			   
	CDimSet * Set = &((CResSet*)m_effectView.GetItemData(hItem))->m_FXResSet;
	
	for( int i = 0; i < (int)Set->m_resSetGroups.size(); i++ )
	{
		TVINSERTSTRUCT Item;
		Item.hParent = hItem;
		Item.hInsertAfter = TVI_LAST;
		Item.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_TEXT|TVIF_SELECTEDIMAGE;
		Item.item.iImage = 1;
		Item.item.iSelectedImage = 2;
		Item.item.lParam = (LPARAM)Set->m_resSetGroups[i];
		Item.item.pszText = (LPSTR)Set->m_resSetGroups[i]->m_groupName.c_str();
		AddFXUnitTo3DTree(m_effectView.InsertItem(&Item));
	}
}

void CResSetView::AddFXUnitTo3DTree(HTREEITEM hItem)
{
	CResSetGroup * pGroup = (CResSetGroup*)m_effectView.GetItemData( hItem );
	
	for ( int i = 0; i < (int)pGroup->m_resSetUnits.size(); i++ )
	{
		TVINSERTSTRUCT Item;
		Item.hParent = hItem;
		Item.hInsertAfter = TVI_LAST;
		Item.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_TEXT|TVIF_SELECTEDIMAGE;
	
		Item.item.iImage = 3;
		Item.item.iSelectedImage = 4;

		Item.item.lParam = (LPARAM)pGroup->m_resSetUnits[i];

		char s[256];

		string str =  pGroup->m_resSetUnits[i]->GetItemName();
		
		strcpy(s,str.c_str());
		Item.item.pszText = s;
		
		HTREEITEM h = m_effectView.InsertItem( &Item );
		pGroup->m_resSetUnits[i]->h = h;
	}
}

//void CResSetView::OnBnClickedShadow()
//{
//	if ( m_pActiveUnit )
//		m_pActiveUnit->SetShadow(IsDlgButtonChecked(IDC_SHADOW));
//}

void CResSetView::OnBnClickedLoadresset()
{
	static char Filter[] = " Resource Files (*.ars)|*.ars||";
	
	CFileDialog Dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT|OFN_NOCHANGEDIR, Filter );
	
	if ( Dlg.DoModal() == IDOK )
	{
		POSITION Pos = Dlg.GetStartPosition();
		
		while ( Pos )
		{
			CString str = Dlg.GetNextPathName( Pos );
			
			//////////////////////////////////////////////////////////////////////////
			
			AddResSetFromFile(str.GetBuffer());
		}
	}
}
