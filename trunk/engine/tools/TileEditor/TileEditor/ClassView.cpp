// ClassView.cpp : 实现文件
//

#include "stdafx.h"
#include "MapEditor.h"
#include "ClassView.h"
#include ".\classview.h"
#include "Property.h"
#include "ServerObj.h"
#include "Brain.h"
#include "MainFrm.h"

// CClassView

CClassView::CClassView()
	: CFormView(CClassView::IDD)
{
}

CClassView::~CClassView()
{
}

void CClassView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLASSVIEW, m_classTree);
}

BEGIN_MESSAGE_MAP(CClassView, CFormView)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(TVN_SELCHANGED, IDC_CLASSVIEW, OnTvnSelchangedClassview)
END_MESSAGE_MAP()

// CClassView 诊断

#ifdef _DEBUG
void CClassView::AssertValid() const
{
	CFormView::AssertValid();
}

void CClassView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

// CClassView 消息处理程序

void CClassView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CFormView::OnShowWindow(bShow, nStatus);
	static BOOL IsValid = FALSE;
	if( bShow && !IsValid )
	{
		IsValid = TRUE;
		
		m_client.Load("GacShell.dll");
		m_server.Load("GasShell.dll");
		
		m_Image.Create(16,16,ILC_COLOR16|ILC_MASK,0,1);
		m_Image.Add( ::AfxGetApp()->LoadIcon( IDI_EMPTY ) );
		m_Image.Add( ::AfxGetApp()->LoadIcon( IDI_SERVER ) );
		m_Image.Add( ::AfxGetApp()->LoadIcon( IDI_CLIENT ) );
		m_Image.Add( ::AfxGetApp()->LoadIcon( IDI_SERVERCLASS ) );
		m_Image.Add( ::AfxGetApp()->LoadIcon( IDI_CLIENTCLASS ) );
		m_Image.Add( ::AfxGetApp()->LoadIcon( IDI_OBJECT ) );
		m_classTree.SetImageList(&m_Image, TVSIL_NORMAL);
		
		HTREEITEM hItem = m_classTree.GetChildItem( TVI_ROOT );
		
		TVINSERTSTRUCT Item;
		Item.hParent = TVI_ROOT;
		Item.hInsertAfter = TVI_LAST;
		Item.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_TEXT|TVIF_SELECTEDIMAGE;
		Item.item.iImage = 0;
		Item.item.iSelectedImage = 0;
		Item.item.lParam = 0; //(LPARAM)m_curSelResSet;
		Item.item.pszText = "游戏逻辑类"; // (LPSTR)m_curSelResSet->m_resSetName.c_str();		
		HTREEITEM hSetItem = m_classTree.InsertItem( &Item );
		
		Item.hParent = hSetItem;
		Item.item.iImage = 1;
		Item.item.iSelectedImage = 1;
		Item.item.lParam = 0; // (LPARAM)&(m_curSelResSet->m_2DResSet);
		Item.item.pszText = "服务器端逻辑类";	
		ResetServer(m_classTree.InsertItem( &Item ));
		
		Item.hParent = hSetItem;
		Item.item.iImage = 2;
		Item.item.iSelectedImage = 2;
		Item.item.lParam = 0; // (LPARAM)&(m_curSelResSet->m_3DResSet);
		Item.item.pszText = "客户端逻辑类";	
		ResetClient(m_classTree.InsertItem( &Item ));

		m_classTree.Expand( hSetItem, TVE_EXPAND );
	}
}

void CClassView::ResetServer(HTREEITEM hSetItem)
{
	m_hServer = hSetItem;
	
	for ( int i = 0; i < m_server.GetNumClassDefs(); i++ )
	{
		std::string s = m_server.GetClassDefs()[i]->m_ClassName;
		
		if ( !(m_server.GetClassDefs()[i]->m_ClassFlags & CF_HIDDEN))
		{
			TVINSERTSTRUCT Item;
			Item.hParent = hSetItem;
			Item.hInsertAfter = TVI_LAST;
			Item.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_TEXT|TVIF_SELECTEDIMAGE;
			Item.item.iImage = 3;
			Item.item.iSelectedImage = 3;
			Item.item.lParam = 0;
			Item.item.pszText = m_server.GetClassDefs()[i]->m_ClassName;
			m_classTree.InsertItem( &Item );
		}
	}
}

void CClassView::ResetClient(HTREEITEM hSetItem)
{
	m_hClient = hSetItem;
	
	for ( int i = 0; i < m_client.GetNumClassDefs(); i++ )
	{
		std::string s = m_client.GetClassDefs()[i]->m_ClassName;
		if ( !(m_client.GetClassDefs()[i]->m_ClassFlags & CF_HIDDEN) )
		{
			TVINSERTSTRUCT Item;
			Item.hParent = hSetItem;
			Item.hInsertAfter = TVI_LAST;
			Item.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_TEXT|TVIF_SELECTEDIMAGE;
			Item.item.iImage = 4;
			Item.item.iSelectedImage = 4;
			Item.item.lParam = 0;
			Item.item.pszText = m_client.GetClassDefs()[i]->m_ClassName;
			m_classTree.InsertItem( &Item );
		}
	}
}

void CClassView::AddObject(HTREEITEM hSetItem, void * p)
{
	TVINSERTSTRUCT Item;
	Item.hParent = hSetItem;
	Item.hInsertAfter = TVI_LAST;
	Item.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_TEXT|TVIF_SELECTEDIMAGE;
	Item.item.iImage = 5;
	Item.item.iSelectedImage = 5;
	Item.item.lParam = (LPARAM)p;
	Item.item.pszText = "对象实例";
	m_classTree.InsertItem( &Item );
}

void CClassView::OnTvnSelchangedClassview(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	HTREEITEM CurSel = m_classTree.GetSelectedItem();
	if( !CurSel )
		return;
	TVITEM TItem;
	TItem.hItem = CurSel;
	char s[64];
	TItem.pszText = s;
	TItem.cchTextMax = 64;
	TItem.mask = TVIF_TEXT|TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE;
	m_classTree.GetItem( &TItem );
	
	if ( TItem.iImage == 3 )
	{
		if ( GetAsyncKeyState(VK_LCONTROL) & 0x8000 )
		{
			void * p = sc::CXFile::GetInstance()->GetOperator()->BindServerBrainToFace();
			if (!p) return; 
			CBrain * pBrain = new CBrain(p);
			pBrain->Init(TItem.pszText,&m_server);
			this->AddObject(CurSel,(void*)pBrain);
		}
	}
	
	if ( TItem.iImage == 4 )
	{
		if ( GetAsyncKeyState(VK_LCONTROL) & 0x8000 )
		{
			void * p = sc::CXFile::GetInstance()->GetOperator()->BindClientBrainToFace();
			if (!p) return; 
			CBrain * pBrain = new CBrain(p);
			pBrain->Init(TItem.pszText,&m_client);
			this->AddObject(CurSel,(void*)pBrain);
		}
	}
	
	if ( TItem.iImage == 5 )
	{
		CBrain * pBrain = (CBrain*)TItem.lParam;
		sc::CTerrainMesh::CObject * pObject = (sc::CTerrainMesh::CObject*)pBrain->GetHook();
		
		if ( GetAsyncKeyState(VK_LCONTROL) & 0x8000 )
		{
			sc::CXFile::GetInstance()->GetOperator()->MoveTo(pObject);
		}
		
		((CMainFrame*)AfxGetMainWnd())->SetActiveBrain(pBrain);
	}
		
	*pResult = 0;
}

BOOL CClassView::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	switch ( message ) 
	{
	case WM_CLASSVIEWSAVEASSERVER:
		{
			this->SaveMeAsServer((FILE*)wParam,(int)lParam);
		}
	break;

	case WM_CLASSVIEWSAVEASCLIENT:
		{
			this->SaveMeAsClient((FILE*)wParam,(int)lParam);
		}
	break;

	case WM_CLASSVIEWSAVEASLOCAL:
		{
			this->SaveMeAsLocal((FILE*)wParam);
		}
	break;

	//case WM_POSCHANGE:
	//	{
	//		this->SaveMe((FILE*)wParam,(int)lParam);
	//	}
	//break;
	}

	return CFormView::OnWndMsg(message, wParam, lParam, pResult);
}

void CClassView::SaveMeAsServer(FILE * fp, int nRgnIndex)
{
	long lObjectCntPt = ftell(fp); 
	DWORD dwPad = 0;
	fwrite(&dwPad,sizeof(DWORD),1,fp);
	
	int nCnt = 0;

	//////////////////////////////////////////////////////////////////////////

	HTREEITEM hCurrentClass = m_classTree.GetNextItem(m_hServer, TVGN_CHILD);
	
	while ( hCurrentClass != NULL )
	{
		HTREEITEM hCurrentInstance = m_classTree.GetNextItem(hCurrentClass, TVGN_CHILD);

		while ( hCurrentInstance != NULL )
		{
			TVITEM TItem;
			TItem.hItem = hCurrentInstance;
			TItem.mask = TVIF_IMAGE|TVIF_PARAM;
			bool b = m_classTree.GetItem( &TItem );
			
			if ( b )
			{
				if ( TItem.iImage == 5  )
				{
					CBrain * pBrain = (CBrain*)TItem.lParam;
					
					sc::CTerrainMesh::CObject * pObject = (sc::CTerrainMesh::CObject*)pBrain->GetHook();
					
					if ( sc::CXFile::GetInstance()->GetTerrainMesh()->InWhichRegion(pObject->GetCenterGridIndex()) == nRgnIndex )
					{
						nCnt++;
						
						char s[64];
						TVITEM TItemClass;
						TItemClass.pszText = s;
						TItemClass.cchTextMax = 64;
						TItemClass.hItem = hCurrentClass;
						TItemClass.mask = TVIF_TEXT|TVIF_PARAM;
						m_classTree.GetItem( &TItemClass );
						
						// class name
						int nLength = strlen(TItemClass.pszText);
						fwrite(&nLength,sizeof(int),1,fp);
						fwrite(TItemClass.pszText,nLength,1,fp);
						
						// prop count
						int nPropCnt = pBrain->GetPropCnt();
						fwrite(&nPropCnt,sizeof(int),1,fp);
						
						// for each prop
						for ( int i = 0; i< nPropCnt; i++ )
						{
							CBaseProp * pProp = pBrain->GetProp(i);
							
							// prop name
							std::string str = pProp->GetPropName();
							int nPropLength = str.length();
							fwrite(&nPropLength,sizeof(int),1,fp);
							fwrite(str.c_str(),nPropLength,1,fp);
							
							// prop type
							int nPropType = pProp->GetType();
							fwrite(&nPropType,sizeof(int),1,fp);

							// prop size and value
							pProp->SaveMe(fp,pBrain);
						}
					}
				}
			}
			hCurrentInstance = m_classTree.GetNextItem(hCurrentInstance, TVGN_NEXT);
		}
		hCurrentClass = m_classTree.GetNextItem(hCurrentClass, TVGN_NEXT);
	}

	// backfill object cnt
	long l = ftell(fp);
	fseek(fp,lObjectCntPt,SEEK_SET);
	fwrite(&nCnt,sizeof(long),1,fp); // size
	fseek(fp,l,SEEK_SET);
	

	//对象的个数
	//	对于每个对象：
	//{
	//	对象相关的类的名字
	//		属性的个数
	//		对于每个属性：
	//	{
	//		属性的名字
	//			属性的类型
	//			属性的大小
	//			属性的值
	//	}
	//}


}

void CClassView::SaveMeAsClient(FILE * fp, int nRgnIndex)
{
	long lObjectCntPt = ftell(fp); 
	DWORD dwPad = 0;
	fwrite(&dwPad,sizeof(DWORD),1,fp);
	
	int nCnt = 0;
	
	//////////////////////////////////////////////////////////////////////////
	
	HTREEITEM hCurrentClass = m_classTree.GetNextItem(m_hClient, TVGN_CHILD);
	
	while ( hCurrentClass != NULL )
	{
		HTREEITEM hCurrentInstance = m_classTree.GetNextItem(hCurrentClass, TVGN_CHILD);
	
		while ( hCurrentInstance != NULL )
		{
			TVITEM TItem;
			TItem.hItem = hCurrentInstance;
			TItem.mask = TVIF_IMAGE|TVIF_PARAM;
			bool b = m_classTree.GetItem( &TItem );

			if ( b )
			{
				if ( TItem.iImage == 5  )
				{
					CBrain * pBrain = (CBrain*)TItem.lParam;

					sc::CTerrainMesh::CObject * pObject = (sc::CTerrainMesh::CObject*)pBrain->GetHook();
			
					if ( sc::CXFile::GetInstance()->GetTerrainMesh()->InWhichRegion(pObject->GetCenterGridIndex()) == nRgnIndex )
					{
						nCnt++;

						char s[64];
						TVITEM TItemClass;
						TItemClass.pszText = s;
						TItemClass.cchTextMax = 64;
						TItemClass.hItem = hCurrentClass;
						TItemClass.mask = TVIF_TEXT|TVIF_PARAM;
						m_classTree.GetItem( &TItemClass );

						// class name
						int nLength = strlen(TItemClass.pszText);
						fwrite(&nLength,sizeof(int),1,fp);
						fwrite(TItemClass.pszText,nLength,1,fp);

						// prop count
						int nPropCnt = pBrain->GetPropCnt();
						fwrite(&nPropCnt,sizeof(int),1,fp);

						// for each prop
						for ( int i = 0; i< nPropCnt; i++ )
						{
							CBaseProp * pProp = pBrain->GetProp(i);

							// prop name
							std::string str = pProp->GetPropName();
							int nPropLength = str.length();
							fwrite(&nPropLength,sizeof(int),1,fp);
							fwrite(str.c_str(),nPropLength,1,fp);

							// prop type
							int nPropType = pProp->GetType();
							fwrite(&nPropType,sizeof(int),1,fp);

							// prop size and value
							pProp->SaveMe(fp,pBrain);
						}
					}
				}
			}
			hCurrentInstance = m_classTree.GetNextItem(hCurrentInstance, TVGN_NEXT);
		}
		hCurrentClass = m_classTree.GetNextItem(hCurrentClass, TVGN_NEXT);
	}
	
	// backfill object cnt
	long l = ftell(fp);
	fseek(fp,lObjectCntPt,SEEK_SET);
	fwrite(&nCnt,sizeof(long),1,fp); // size
	fseek(fp,l,SEEK_SET);
}

void CClassView::SaveMeAsLocal(FILE * fp)
{
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	
	// client objects
	
	long lClientObjectCntPt = ftell(fp); 
	DWORD dwPad = 0;
	fwrite(&dwPad,sizeof(DWORD),1,fp);
	
	int nCnt = 0;
	
	//////////////////////////////////////////////////////////////////////////
	
	HTREEITEM hCurrentClass = m_classTree.GetNextItem(m_hClient, TVGN_CHILD);
	
	while ( hCurrentClass != NULL )
	{
		HTREEITEM hCurrentInstance = m_classTree.GetNextItem(hCurrentClass, TVGN_CHILD);

		while ( hCurrentInstance != NULL )
		{
			TVITEM TItem;
			TItem.hItem = hCurrentInstance;
			TItem.mask = TVIF_IMAGE|TVIF_PARAM;
			bool b = m_classTree.GetItem( &TItem );

			if ( b )
			{
				if ( TItem.iImage == 5  )
				{
					CBrain * pBrain = (CBrain*)TItem.lParam;

					sc::CTerrainMesh::CObject * pObject = (sc::CTerrainMesh::CObject*)pBrain->GetHook();

					nCnt++;

					char s[64];
					TVITEM TItemClass;
					TItemClass.pszText = s;
					TItemClass.cchTextMax = 64;
					TItemClass.hItem = hCurrentClass;
					TItemClass.mask = TVIF_TEXT|TVIF_PARAM;
					m_classTree.GetItem( &TItemClass );

					// class name
					int nLength = strlen(TItemClass.pszText);
					fwrite(&nLength,sizeof(int),1,fp);
					fwrite(TItemClass.pszText,nLength,1,fp);

					// prop count
					int nPropCnt = pBrain->GetPropCnt();
					fwrite(&nPropCnt,sizeof(int),1,fp);

					// for each prop
					for ( int i = 0; i< nPropCnt; i++ )
					{
						CBaseProp * pProp = pBrain->GetProp(i);

						// prop name
						std::string str = pProp->GetPropName();
						int nPropLength = str.length();
						fwrite(&nPropLength,sizeof(int),1,fp);
						fwrite(str.c_str(),nPropLength,1,fp);

						// prop type
						int nPropType = pProp->GetType();
						fwrite(&nPropType,sizeof(int),1,fp);

						// prop size and value
						pProp->SaveMe(fp,pBrain);
					}
				}
			}
			
			hCurrentInstance = m_classTree.GetNextItem(hCurrentInstance, TVGN_NEXT);
		}
		
		hCurrentClass = m_classTree.GetNextItem(hCurrentClass, TVGN_NEXT);
	}

	// backfill object cnt
	long l = ftell(fp);
	fseek(fp,lClientObjectCntPt,SEEK_SET);
	fwrite(&nCnt,sizeof(long),1,fp); // size
	fseek(fp,l,SEEK_SET);

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	
	// server objects
	
	long lServerObjectCntPt = ftell(fp); 
	fwrite(&dwPad,sizeof(DWORD),1,fp);
	
	nCnt = 0;
	
	//////////////////////////////////////////////////////////////////////////
	
	hCurrentClass = m_classTree.GetNextItem(m_hServer, TVGN_CHILD);
	
	while ( hCurrentClass != NULL )
	{
		HTREEITEM hCurrentInstance = m_classTree.GetNextItem(hCurrentClass, TVGN_CHILD);

		while ( hCurrentInstance != NULL )
		{
			TVITEM TItem;
			TItem.hItem = hCurrentInstance;
			TItem.mask = TVIF_IMAGE|TVIF_PARAM;
			bool b = m_classTree.GetItem( &TItem );

			if ( b )
			{
				if ( TItem.iImage == 5  )
				{
					CBrain * pBrain = (CBrain*)TItem.lParam;

					sc::CTerrainMesh::CObject * pObject = (sc::CTerrainMesh::CObject*)pBrain->GetHook();

					nCnt++;

					char s[64];
					TVITEM TItemClass;
					TItemClass.pszText = s;
					TItemClass.cchTextMax = 64;
					TItemClass.hItem = hCurrentClass;
					TItemClass.mask = TVIF_TEXT|TVIF_PARAM;
					m_classTree.GetItem( &TItemClass );

					// class name
					int nLength = strlen(TItemClass.pszText);
					fwrite(&nLength,sizeof(int),1,fp);
					fwrite(TItemClass.pszText,nLength,1,fp);

					// prop count
					int nPropCnt = pBrain->GetPropCnt();
					fwrite(&nPropCnt,sizeof(int),1,fp);

					// for each prop
					for ( int i = 0; i< nPropCnt; i++ )
					{
						CBaseProp * pProp = pBrain->GetProp(i);
						
						// prop name
						std::string str = pProp->GetPropName();
						int nPropLength = str.length();
						fwrite(&nPropLength,sizeof(int),1,fp);
						fwrite(str.c_str(),nPropLength,1,fp);
						
						// prop type
						int nPropType = pProp->GetType();
						fwrite(&nPropType,sizeof(int),1,fp);
						
						// prop size and value
						pProp->SaveMe(fp,pBrain);
					}
				}
			}
			
			hCurrentInstance = m_classTree.GetNextItem(hCurrentInstance, TVGN_NEXT);
		}
		
		hCurrentClass = m_classTree.GetNextItem(hCurrentClass, TVGN_NEXT);
	}
	
	// backfill object cnt
	l = ftell(fp);
	fseek(fp,lServerObjectCntPt,SEEK_SET);
	fwrite(&nCnt,sizeof(long),1,fp); // size
	fseek(fp,l,SEEK_SET);
}