// CWndImageList.cpp : implementation file
//

#include "stdafx.h"
#include "GUIEditor.h"
#include "CWndImageList.h"
#include ".\CWndImageList.h"
#include "CWndGroup.h"
#include "CWndImageProp.h"
#include "BaseHelper.h"
#include "CodeCvs.h"

#include "../src/gui/SQRGUIManager.h"

#define WND_IMAGE_WIDTH 60

// CWndImageList

IMPLEMENT_DYNAMIC(CWndImageList, CListCtrl)
CWndImageList::CWndImageList() : m_nImageType( IP_ENABLE ), m_ImageCopy( *(new WND_IMAGE_LIST) )
{
}

CWndImageList::~CWndImageList()
{
}



BEGIN_MESSAGE_MAP(CWndImageList, CListCtrl)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnLvnItemchanged)
	ON_EN_KILLFOCUS( 1000, OnEnKillfocusEdit)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnLvnBeginlabeledit)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnLvnEndlabeledit)
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnLvnKeydown)
END_MESSAGE_MAP()


void CWndImageList::Init( CWndGroup* pWndGroup, CWndImageProp* pImageProp )
{
	m_pWndGroup = pWndGroup;
	m_ImageProp = pImageProp;
}

void CWndImageList::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	CRect rt;
	GetClientRect( &rt );
	InsertColumn( 0, L"Data", LVCFMT_LEFT, rt.Width() - 20 );
	SetExtendedStyle( GetExtendedStyle()|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT );

	rt.left = 16;
	rt.top = 0;
	rt.bottom = 18;
	rt.right -= 20;
	m_FileLoader.Create( 0, rt, this, 1000, true );
	m_FileLoader.SetFont( GetFont() );
	m_FileLoader.SetFilter( L"GUI Texture (*.tex)|*.tex||" );

	m_ImageList.Create( 16, 16, ILC_COLOR16|ILC_MASK, 0, 1 );
	SetImageList( &m_ImageList, LVSIL_SMALL );
	m_ImageList.Add( LoadIcon( ::AfxGetInstanceHandle(), MAKEINTRESOURCE( IDI_TEXTURE ) ) );
	m_ImageList.Add( LoadIcon( ::AfxGetInstanceHandle(), MAKEINTRESOURCE( IDI_TEXTUREEDIT ) ) );

	CListCtrl::PreSubclassWindow();
}

void CWndImageList::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	if( pNMLV->uChanged != LVIF_STATE )
		return;
	if( ( pNMLV->uNewState & LVIS_SELECTED ) == 0 )
		return;

	CString str;
	if( m_FileLoader.IsWindowVisible() )
	{
		m_FileLoader.GetWindowText( str );
		SetItemText( m_nEditItem, 0, str );
	}	
	m_ImageProp->UpdateProp();

	CRect rt;
	GetItemRect( pNMLV->iItem, &rt, LVIR_LABEL );
	m_FileLoader.SetWindowPos( NULL, 18, rt.top - 1, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW );
	m_FileLoader.SetWindowText( GetItemText( pNMLV->iItem, 0 ) );
	m_FileLoader.ShowWindow( SW_SHOW );
	m_nEditItem = pNMLV->iItem;
	SQRWnd* pEditWnd = m_pWndGroup->GetEditWnd();
	if( pEditWnd == NULL )
		return;

	IMAGE_PARAM IP ( SM_BS_BK, m_nImageType );	
	WND_IMAGE_LIST* pImageList = pEditWnd->GetWndBkImage( IP );
	IMAGE& Image = pImageList->GetImage( m_nEditItem );
	m_ImageProp->ResetImageProp( &Image, m_pWndGroup );
	UpdateImageList();
	Invalidate();

	if (GetKeyState( VK_CONTROL )&0x8000)
	{
		for (int i = 0; i<int(m_vecSelectItem.size()); i++)
		{
			if (m_vecSelectItem[i] ==  m_nEditItem)
			{
				return;
			}
		}
		m_vecSelectItem.push_back(m_nEditItem);
	}
	else if(!(GetKeyState( VK_SPACE )&0x8000) &&
		    !(GetKeyState( VK_LEFT )&0x8000)  &&
			!(GetKeyState( VK_RIGHT )&0x8000) &&
			!(GetKeyState( VK_UP )&0x8000)    &&
		    !(GetKeyState( VK_DOWN )&0x8000) )
	{
		m_vecSelectItem.clear();
		m_vecSelectItem.push_back(m_nEditItem);
	}

	m_ImageProp->SetImageList(pImageList);
	m_ImageProp->SetChooseImgList(m_vecSelectItem);
}

void CWndImageList::OnEnKillfocusEdit()
{
	// TODO: Add your control notification handler code here
	CString str;
	if( m_FileLoader.IsWindowVisible() )
	{
		m_FileLoader.GetWindowText( str );
		SetItemText( m_nEditItem, 0, str );
		UpdateImageList();
		SetItemState( m_nEditItem, LVIS_SELECTED, 0 );
	}

	if( GetFocus() == &m_FileLoader || GetFocus() == this )
		return;
	m_FileLoader.ShowWindow( SW_HIDE );
}

// CWndImageList uMessage handlers


void CWndImageList::OnLvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	m_FileLoader.ShowWindow( SW_SHOW );
}

void CWndImageList::OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	if( pDispInfo->item.pszText != NULL && pDispInfo->item.pszText[0] )
		SetItemText( pDispInfo->item.iItem, 0, pDispInfo->item.pszText );
}

void CWndImageList::OnLvnKeydown(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	m_FileLoader.ShowWindow( SW_HIDE );
	SQRWnd* pEditWnd = m_pWndGroup->GetEditWnd();
	if( pEditWnd == NULL )
		return;
	IMAGE_PARAM IP ( SM_BS_BK, m_nImageType );	
	WND_IMAGE_LIST* pImageList = pEditWnd->GetWndBkImage( IP );

	if( pLVKeyDow->wVKey == VK_INSERT )
	{	
		string szTextName = "";
		CFRect rt( 0.0f, 0.0f, float(pEditWnd->GetWndWidth()), float(pEditWnd->GetWndHeight()) );
		CFPos pos(0.0f, 0.0f);
		unsigned uColor = 0xffffffff;
		unsigned uLockFlag = 0;
		if( m_nEditItem < pImageList->GetImageCount() )
		{
			IMAGE& Image = pImageList->GetImage( m_nEditItem );
			szTextName	 = Image.pTexture ? Image.pTexture->GetName() : "";
			rt			 = Image.rtTexture;
			pos			 = Image.posWnd;
			uColor		 = Image.dwColor;
			uLockFlag	 = Image.dwLockFlag;
		}
		InsertItem( m_nEditItem, utf8_to_utf16(szTextName).c_str(), 0 );
		pImageList->AddImage( pEditWnd->GetGraphic(), m_nEditItem, szTextName.c_str(), &rt, pos, uColor, uLockFlag );
	}

	if( pLVKeyDow->wVKey == VK_ADD )
	{
		m_nEditItem = GetItemCount();
		InsertItem( m_nEditItem, L"", 0 );
		CFRect rt( 0.0f, 0.0f, float(pEditWnd->GetWndWidth()), float(pEditWnd->GetWndHeight()) );
		pImageList->AddImage( pEditWnd->GetGraphic(), m_nEditItem, "", &rt, CFPos(0.0f, 0.0f), 0xffffffff, 0 );
	}

	//解决无法删除最后一张贴图和删除列表最下面的贴图宕掉的问题(方法有待再考虑)
	if( pLVKeyDow->wVKey == VK_DELETE )
	{
		if (m_nEditItem != pImageList->GetImageCount()-1)
		{
			pImageList->DeleteImage( m_nEditItem );
			DeleteItem( m_nEditItem );
			m_nEditItem = 0;
		} 
		else
		{
			string szTextName = "";
			CFRect rt( 0.0f, 0.0f, float(pEditWnd->GetWndWidth()), float(pEditWnd->GetWndHeight()) );
			CFPos pos(0.0f, 0.0f);
			unsigned uColor = 0xffffffff;
			unsigned uLockFlag = 0;
			if( m_nEditItem < pImageList->GetImageCount() )
			{
				IMAGE& Image = pImageList->GetImage( m_nEditItem );
				szTextName	 = "";
				rt			 = Image.rtTexture;
				pos			 = Image.posWnd;
				uColor		 = 0x00ffffff;
				uLockFlag	 = Image.dwLockFlag;
			}
			InsertItem( m_nEditItem, utf8_to_utf16(szTextName).c_str(), 0 );
			pImageList->AddImage( pEditWnd->GetGraphic(), m_nEditItem, szTextName.c_str(), &rt, pos, uColor, uLockFlag );

			pImageList->DeleteImage( m_nEditItem + 1 );
			DeleteItem(  m_nEditItem + 1 );
			m_nEditItem = 0;
		}
	}

	wchar_t Buf[256];
	//按住空格键向左移动贴图
	if( pLVKeyDow->wVKey == VK_LEFT && (GetKeyState( VK_SPACE )&0x8000) )
	{
		if(!m_vecSelectItem.empty())
		{	
			for(int i = 0; i< int(m_vecSelectItem.size()); i++)
			{
				int tmp = m_vecSelectItem[i];
				pImageList->GetImage(tmp).posWnd.x -= 1;
				SetItemText( 0, 1, _itow( int32(pImageList->GetImage(tmp).posWnd.x), Buf, 10 ) );
			}
		}
	}
	//按住空格键向右移动贴图
	if( pLVKeyDow->wVKey == VK_RIGHT && (GetKeyState( VK_SPACE )&0x8000) )
	{
		if(!m_vecSelectItem.empty())
		{
			for(int i = 0; i< int(m_vecSelectItem.size()); i++)
			{
				int tmp = m_vecSelectItem[i];
				pImageList->GetImage(tmp).posWnd.x += 1;
				SetItemText( 0, 1, _itow( int32(pImageList->GetImage(tmp).posWnd.x), Buf, 10 ) );
			}
		}
	}

	//按住空格键向上移动贴图
    if( pLVKeyDow->wVKey == VK_UP && (GetKeyState( VK_SPACE )&0x8000) )
	{
		if(!m_vecSelectItem.empty())
		{
			for(int i = 0; i< int(m_vecSelectItem.size()); i++)
			{
				int tmp = m_vecSelectItem[i];
				pImageList->GetImage(tmp).posWnd.y -= 1;
				SetItemText( 1, 1, _itow( int32(pImageList->GetImage(tmp).posWnd.y), Buf, 10 ) );
			}
		}
	}
	//不按空格键移动贴图图层
	else if( pLVKeyDow->wVKey == VK_UP && m_nEditItem > 0 )
	{
		if( 1 != int(m_vecSelectItem.size()))
			return;

		m_vecSelectItem[0]--;//这里是用来控制贴图选择框的渲染
		
		IMAGE Image = pImageList->GetImage( m_nEditItem - 1 );
		pImageList->GetImage( m_nEditItem - 1 ) = pImageList->GetImage( m_nEditItem );
		pImageList->GetImage( m_nEditItem ) = Image;
		SetItemText( m_nEditItem, 0, pImageList->GetImage( m_nEditItem ).pTexture ? utf8_to_utf16(pImageList->GetImage( m_nEditItem ).pTexture->GetName()).c_str() : L"" );
		SetItemText( m_nEditItem - 1, 0, pImageList->GetImage( m_nEditItem - 1 ).pTexture ? utf8_to_utf16(pImageList->GetImage( m_nEditItem - 1 ).pTexture->GetName()).c_str() : L"" );
		m_nEditItem--;
	}

	//按住空格键向下移动贴图
	if( pLVKeyDow->wVKey == VK_DOWN && (GetKeyState( VK_SPACE )&0x8000) )
	{
		if(!m_vecSelectItem.empty())
		{
			for(int i = 0; i< int(m_vecSelectItem.size()); i++)
			{
				int tmp = m_vecSelectItem[i];
				pImageList->GetImage(tmp).posWnd.y += 1;
				SetItemText( 1, 1, _itow( int32(pImageList->GetImage(tmp).posWnd.y), Buf, 10 ) );
			}
		}
	}
	//不按空格键移动贴图图层
	else if( pLVKeyDow->wVKey == VK_DOWN && m_nEditItem < pImageList->GetImageCount() - 1 )
	{
		if( 1 != int(m_vecSelectItem.size()))
			return;

		m_vecSelectItem[0]++;//这里是用来控制贴图选择框的渲染

		IMAGE Image = pImageList->GetImage( m_nEditItem + 1 );
		pImageList->GetImage( m_nEditItem + 1 ) = pImageList->GetImage( m_nEditItem );
		pImageList->GetImage( m_nEditItem ) = Image;
		SetItemText( m_nEditItem, 0, pImageList->GetImage( m_nEditItem ).pTexture ? utf8_to_utf16(pImageList->GetImage( m_nEditItem ).pTexture->GetName()).c_str() : L"" );
		SetItemText( m_nEditItem + 1, 0, pImageList->GetImage( m_nEditItem + 1 ).pTexture ? utf8_to_utf16(pImageList->GetImage( m_nEditItem + 1 ).pTexture->GetName()).c_str() : L"" );
		m_nEditItem++;
	}
    
	if( pLVKeyDow->wVKey == 'C' && ( GetKeyState( VK_CONTROL )&0x8000 ) )
	{
		if (!m_vecSelectItem.empty())
		{
			for (int i = 0; i < int(m_vecSelectItem.size()); i++)
			{
				m_ImageCopy.AddImageFromImageList(*pImageList,m_vecSelectItem[i],m_vecSelectItem[i]+1);
			}
		}
		m_vecSelectItem.clear();
		pEditWnd->GetWndRect( m_rtWndCopy );
		m_nEditItem = 0;
	}
	if( pLVKeyDow->wVKey == 'V' && ( GetKeyState( VK_CONTROL )&0x8000 ) )
	{
		if (m_ImageCopy.GetImageCount() == 0 )
		{
			return;
		}
		CFRect rtPre;
		pEditWnd->GetWndRect( rtPre );
		pEditWnd->SetWndRect( m_rtWndCopy );
		if (pImageList->GetImageCount() != 0 )
		{
			int a = pImageList->GetImageCount();
			pImageList->AddImageFromImageList(m_ImageCopy,0,m_ImageCopy.GetImageCount());
		}
		pEditWnd->SetWndRect( rtPre );
		IMAGE& Image = pImageList->GetImage( m_nEditItem );
		m_ImageProp->ResetImageProp( &Image, m_pWndGroup );
		SetEditImageType(m_nImageType);
		m_ImageCopy.ClearImage();
	}
	if( pLVKeyDow->wVKey == 'D' && ( GetKeyState( VK_CONTROL )&0x8000 ) )
	{
		CFRect rtPre;
		pEditWnd->GetWndRect( rtPre );
		pEditWnd->SetWndRect( m_rtWndCopy );
		for( int i = 0; i < m_ImageCopy.GetImageCount(); i++ )
		{
			IMAGE& Image = m_ImageCopy.GetImage(i);
			pImageList->AddImage( pEditWnd->GetGraphic(), -1, Image.pTexture ? Image.pTexture->GetName() : "", 
				&Image.rtTexture, Image.posWnd, Image.dwColor, Image.dwLockFlag );
		}
		pEditWnd->SetWndRect( rtPre );
		IMAGE& Image = pImageList->GetImage( m_nEditItem );
		m_ImageProp->ResetImageProp( &Image, m_pWndGroup );
		SetEditImageType(m_nImageType);
	}

	IMAGE& Image = pImageList->GetImage( m_nEditItem );
	m_ImageProp->ResetImageProp( &Image, m_pWndGroup );
}

void CWndImageList::SetEditImageType( int nImageType, bool bSavePreWnd )
{
	m_nImageType = -1; 
	DeleteAllItems();
	if( bSavePreWnd )
		m_ImageProp->UpdateProp();

	SQRWnd* pEditWnd = m_pWndGroup->GetEditWnd();
	if( pEditWnd == NULL )
		return;

	m_nImageType = nImageType; 
	IMAGE_PARAM IP ( SM_BS_BK, m_nImageType );	
	WND_IMAGE_LIST* pImageList = pEditWnd->GetWndBkImage( IP );

	if( pImageList == NULL )
		return;

	for( int i = 0; i < pImageList->GetImageCount(); i++ )
	{
		IMAGE& Image = pImageList->GetImage(i);
		int nItem = InsertItem( i, Image.pTexture ? utf8_to_utf16(pImageList->GetImage(i).pTexture->GetName()).c_str() : L"", 0 );
	}

	m_nEditItem = 0;
	CRect rt;
	GetItemRect( m_nEditItem, &rt, LVIR_LABEL );
	m_FileLoader.SetWindowPos( NULL, 18, rt.top - 1, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW );
	m_FileLoader.SetWindowText( GetItemText( m_nEditItem, 0 ) );
	m_FileLoader.ShowWindow( SW_HIDE );

	IMAGE& Image = pImageList->GetImage( m_nEditItem );
	m_ImageProp->ResetImageProp( &Image, m_pWndGroup );
}

void CWndImageList::UpdateImageList()
{
	SQRWnd* pEditWnd = m_pWndGroup->GetEditWnd();
	if( pEditWnd == NULL )
		return;

	IMAGE_PARAM IP ( SM_BS_BK, m_nImageType );	
	WND_IMAGE_LIST* pImageList = pEditWnd->GetWndBkImage( IP );
	if( pImageList == NULL )
		return;

	m_pWndGroup->Check();
	int n1 = GetItemCount();
	int n2 = pImageList->GetImageCount();
	for( int i = 0; i < GetItemCount(); i++ )
	{
		IMAGE& Image = pImageList->GetImage( i );
		CString szImageName = Image.pTexture ? gbk_to_utf16(Image.pTexture->GetName()).c_str() : L"";
		CString itemText = GetItemText(i, 0);
		if(itemText == szImageName )
			continue;
		Image.dwColor = 0xffffffff;
		SafeRelease( Image.pTexture );
		HRESULT hr = pEditWnd->GetGraphic()->CreateTexture( L"", utf16_to_gbk(itemText.GetString() ).c_str(), &Image.pTexture);
		if (FAILED(hr))
			OutputDebugString(itemText);

		if ( Image.pTexture )
		{
			Image.texName = utf16_to_gbk(itemText.GetString() ).c_str();
		}
		// 对于 tree 和 list 需要即时更新
		if ( pEditWnd->GetClassName() == string( "CListCtrl" ))
		{
			SQRListCtrl* pListCtrl = (SQRListCtrl*)pEditWnd;
			pListCtrl->UpdateItemImage();
		}
	}
	IMAGE& Image = pImageList->GetImage( m_nEditItem );
	m_ImageProp->ResetImageProp( &Image, m_pWndGroup );
	m_pWndGroup->ReCheck();
}

void CWndImageList::DetectImage( UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	if( uMessage != WM_RBUTTONDOWN )
		return;

	SQRWnd* pEditWnd = m_pWndGroup->GetEditWnd();
	if( pEditWnd == NULL )
		return;

	IMAGE_PARAM IP ( SM_BS_BK, m_nImageType );	
	WND_IMAGE_LIST* pImageList = pEditWnd->GetWndBkImage( IP );
	if( pImageList == NULL )
		return;

	CFRect rt;
	pEditWnd->GetWndRect( rt );
	int n = pImageList->DetectImage( CFPos( float(LOWORD(lParam)) - rt.left, float(HIWORD(lParam)) - rt.top ) );
	if( n < 0 )
		return;
	SetItemState( m_nEditItem, LVIS_SELECTED, 0 );
	SetItemState( n, LVIS_SELECTED, LVIS_SELECTED );
}

void CWndImageList::DrawEditLine()
{
	if( IDC_RADIO_TEX == m_pWndGroup->m_EditID )
	{
		SQRWnd* pEditWnd = m_pWndGroup->GetEditWnd();
		if( pEditWnd == NULL )
			return;

		IMAGE_PARAM IP ( SM_BS_BK, m_nImageType );	
		WND_IMAGE_LIST* pImageList = pEditWnd->GetWndBkImage( IP );
		if( pImageList == NULL )
			return;

		if( m_nImageType != IP_MOUSEDESC )
			pEditWnd->DrawRect( *pImageList );
		else
		{
			CFRect rt;
			pEditWnd->GetRootWnd()->GetWndRect( rt );
			pEditWnd->ScreenToWnd( rt );
			pEditWnd->DrawRect( *pImageList, &rt );
		}
	}
}