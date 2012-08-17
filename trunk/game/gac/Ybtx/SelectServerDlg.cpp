// SelectServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Ybtx.h"
#include "YbtxDlg.h"
#include "SelectServerDlg.h"
#include "tinyxml/tinyxml.h"
#include "CodeCvs.h"
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SERVER_SELECTED_LOG L"../../var/gac/SelectedServer.txt"

#define BMP_SELECTDLG					"selectdlg.bmp"
#define BMP_SELECTOK1					"selectok_1.bmp"
#define BMP_SELECTOK2					"selectok_2.bmp"
#define BMP_SELECTOK3					"selectok_3.bmp"
#define BMP_SELECTCANCEL1				"selectcancel_1.bmp"
#define BMP_SELECTCANCEL2				"selectcancel_2.bmp"
#define BMP_SELECTCANCEL3				"selectcancel_3.bmp"
#define BMP_SERVERGROUP2				"servergroup_2.bmp"
#define BMP_SERVERGROUP1				"servergroup_1.bmp"
#define BMP_SERVERGROUPNORMALITEM1		"servergroupnormalitem_1.bmp"
#define BMP_SERVERGROUPSELECTITEM1		"servergroupselectitem_1.bmp"
#define	BMP_SERVERGROUPBKG				"servergroupbkg.bmp"
#define BMP_SERVERNORMALITEM0			"servernormalitem_0.bmp"
#define BMP_SERVERSELECTITEM0			"serverselectitem_0.bmp"
#define BMP_SERVERNORMALITEM1			"servernormalitem_1.bmp"
#define BMP_SERVERSELECTITEM1			"serverselectitem_1.bmp"
#define BMP_SERVERNORMALITEM2			"servernormalitem_2.bmp"
#define BMP_SERVERSELECTITEM2			"serverselectitem_2.bmp"
#define BMP_SERVERNORMALITEM3			"servernormalitem_3.bmp"
#define BMP_SERVERSELECTITEM3			"serverselectitem_3.bmp"
#define BMP_SERVERNORMALITEM4			"servernormalitem_4.bmp"
#define BMP_SERVERSELECTITEM4			"serverselectitem_4.bmp"
#define BMP_SERVERITEMBKG				"serverItembkg.bmp"
#define BMP_THUMB						"thumb.bmp"	
#define BMP_SPAN						"span.bmp"
#define	BMP_THUMBITEM					"thumbitem.bmp"	
#define BMP_SPANITEM					"spanitem.bmp"
// CSelectServerDlg dialog

IMPLEMENT_DYNAMIC(CSelectServerDlg, CDialog)

CSelectServerDlg::CSelectServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectServerDlg::IDD, pParent)
	, m_strSelectedServer(L"")
	, m_strSelectedServerGroup(L"")
	, m_strSelectedServerGroupTemp(L"")
{
}

CSelectServerDlg::~CSelectServerDlg()
{
}

void CSelectServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSelectServerDlg, CDialog)
	ON_WM_PAINT()
	ON_NOTIFY(NM_CLICK, IDC_LIST_SERVERGROUP,	&CSelectServerDlg::OnNMClickListServerGroup)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SERVERITEM,	&CSelectServerDlg::OnNMClickListServer)
	ON_NOTIFY(NM_DBLCLK,IDC_LIST_SERVERITEM,	&CSelectServerDlg::OnNMDBLCLKListServer)

	ON_BN_CLICKED(IDC_BUTTON_SELECTCANCEL,		&CSelectServerDlg::OnBnClickedButtonSelectCancel)
	ON_BN_CLICKED(IDC_BUTTON_SELECTOK,			&CSelectServerDlg::OnBnClickedButtonSelectOK)

	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


// CSelectServerDlg message handlers

BOOL CSelectServerDlg::OnInitDialog()
{
	// TODO:  Add extra initialization here
	CDialog::OnInitDialog();

	CYbtxDlg* pParent = (CYbtxDlg*)GetParent();
	m_BGImage.LoadImage(pParent->GetImagePath(BMP_SELECTDLG).c_str());
	
	m_SelectOKBtn.Create(NULL,WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW,CRect(238,287,238+126,287+23),
		GetDC()->GetWindow(),IDC_BUTTON_SELECTOK);
	m_SelectOKBtn.LoadButtonImage(pParent->GetImagePath(BMP_SELECTOK1));
	m_SelectOKBtn.LoadHoverImage(pParent->GetImagePath(BMP_SELECTOK2));
	m_SelectOKBtn.LoadClickImage(pParent->GetImagePath(BMP_SELECTOK3));

	m_SelectCancelBtn.Create(NULL,WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW,CRect(366,287,366+126,287+23),
		GetDC()->GetWindow(),IDC_BUTTON_SELECTCANCEL);
	m_SelectCancelBtn.LoadButtonImage(pParent->GetImagePath(BMP_SELECTCANCEL1));
	m_SelectCancelBtn.LoadHoverImage(pParent->GetImagePath(BMP_SELECTCANCEL2));
	m_SelectCancelBtn.LoadClickImage(pParent->GetImagePath(BMP_SELECTCANCEL3));  

	m_ServerGroupListCtrl.Create(WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_SHOWSELALWAYS | LVS_OWNERDRAWFIXED | LVS_NOCOLUMNHEADER ,CRect(23,47,23+116,47+207),GetDC()->GetWindow(),IDC_LIST_SERVERGROUP);
	m_ServerGroupListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|m_ServerGroupListCtrl.GetExtendedStyle());
	m_ServerGroupListCtrl.InsertColumn(0,L"大区",LVCFMT_CENTER,120);
	m_ServerGroupListCtrl.LoadNormalImage(pParent->GetImagePath(BMP_SERVERGROUP1));
	m_ServerGroupListCtrl.LoadSelectImage(pParent->GetImagePath(BMP_SERVERGROUP2));
	m_ServerGroupListCtrl.LoadBkImage(pParent->GetImagePath(BMP_SERVERGROUPBKG));
	m_ServerGroupListCtrl.SetItemHeight(28);
	m_ServerGroupListCtrl.SetNormalColor(0x588098);
	m_ServerGroupListCtrl.SetSelectColor( 0x3081e5);
	m_ServerGroupListCtrl.InitScrollbar(CRect(155,57,155+21,57+202),L"",L"",pParent->GetImagePath(BMP_THUMB),pParent->GetImagePath(BMP_SPAN));
	
	map<int, pair<const char*,const char*> >::iterator Iter;
	map<int, pair<const char*,const char*> > ServerGroupMapID;
	ServerGroupMapID[1] = make_pair(BMP_SERVERGROUPNORMALITEM1,BMP_SERVERGROUPSELECTITEM1);
	for ( Iter = ServerGroupMapID.begin();  Iter != ServerGroupMapID.end(); Iter++ )
	{
		CImageManage* pImageNormal = new CImageManage();
		pImageNormal->LoadImage(pParent->GetImagePath(Iter->second.first).c_str());
		CImageManage* pImageSelect = new CImageManage();
		pImageSelect->LoadImage(pParent->GetImagePath(Iter->second.second).c_str());
		m_ServerGroupListCtrl.SetItemImage(Iter->first,pImageNormal,pImageSelect);
	}

	m_ServerItemListCtrl.Create(WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_SHOWSELALWAYS | LVS_OWNERDRAWFIXED | LVS_NOCOLUMNHEADER ,CRect(184,46,184+268,46+209),GetDC()->GetWindow(),IDC_LIST_SERVERITEM);
	m_ServerItemListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|m_ServerItemListCtrl.GetExtendedStyle());
	m_ServerItemListCtrl.InsertColumn(0,L"列表1",LVCFMT_CENTER,134);
	m_ServerItemListCtrl.InsertColumn(1,L"列表2",LVCFMT_CENTER,134);
	m_ServerItemListCtrl.LoadNormalImage(pParent->GetImagePath(BMP_SERVERNORMALITEM0));
	m_ServerItemListCtrl.LoadSelectImage(pParent->GetImagePath(BMP_SERVERSELECTITEM0));
	m_ServerItemListCtrl.LoadBkImage(pParent->GetImagePath(BMP_SERVERITEMBKG));
	m_ServerItemListCtrl.SetNormalColor(0x588ab8);
	m_ServerItemListCtrl.SetSelectColor(0xcfa554);
	m_ServerItemListCtrl.SetItemHeight(25);
	m_ServerItemListCtrl.InitScrollbar(CRect(463,92,463+21,92+171),L"",L"",pParent->GetImagePath(BMP_THUMBITEM),pParent->GetImagePath(BMP_SPANITEM));
	
	map<int, pair<const char*,const char*> > MapID;
	MapID[1] = make_pair(BMP_SERVERNORMALITEM1,BMP_SERVERSELECTITEM1);
	MapID[2] = make_pair(BMP_SERVERNORMALITEM2,BMP_SERVERSELECTITEM2);
	MapID[3] = make_pair(BMP_SERVERNORMALITEM3,BMP_SERVERSELECTITEM3);
	MapID[4] = make_pair(BMP_SERVERNORMALITEM4,BMP_SERVERSELECTITEM4);
	for ( Iter = MapID.begin(); Iter != MapID.end(); Iter++)
	{
		CImageManage* pImageNormal = new CImageManage();
		pImageNormal->LoadImage(pParent->GetImagePath(Iter->second.first).c_str());
		CImageManage* pImageSelect = new CImageManage();
		pImageSelect->LoadImage(pParent->GetImagePath(Iter->second.second).c_str());
		m_ServerItemListCtrl.SetItemImage(Iter->first,pImageNormal,pImageSelect);
	}
	SetWindowPos(NULL, 110,144, 503, 318, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectServerDlg::OnPaint()
{
	CPaintDC dc(this);
	SelectObject(m_BGImage.GetMemDC(),m_BGImage.GetBitmapHandle());
	BitBlt(dc.GetSafeHdc(), 0, 0, m_BGImage.GetBitmapInfo().bmWidth, m_BGImage.GetBitmapInfo().bmHeight,
		m_BGImage.GetMemDC(), 0, 0, SRCCOPY);
}

void CSelectServerDlg::InitServerGroupList(wstring wstrServerListPath)
{
	TiXmlDocument* doc = new TiXmlDocument();
	doc->LoadFile(utf16_to_utf8(wstrServerListPath).c_str());

	int nItemCount = -1;

	TiXmlNode* pXmlDlNode = doc->FirstChild("dl");
	if (pXmlDlNode == NULL)
	{
		MessageBox(L"读取ServerList失败！！");
		return;
	}
	//Init ServerGroup
	for (TiXmlNode* pServerGroup = pXmlDlNode->FirstChild("g");pServerGroup;pServerGroup = pServerGroup->NextSibling())
	{
		const char* charTemp = pServerGroup->ToElement()->Attribute("n");
		wstring wstrTemp = utf8_to_utf16(string(charTemp));
		nItemCount = m_ServerGroupListCtrl.GetItemCount();
		int nRow = m_ServerGroupListCtrl.InsertItem(nItemCount,wstrTemp.c_str());

		int icon = strtol(pServerGroup->ToElement()->Attribute("icon"),NULL,10);
		if (icon != 0 )
			m_ServerGroupListCtrl.SetItemImageID(nRow,0,icon);

		COLORREF color = strtol(pServerGroup->ToElement()->Attribute("fontcolor"),NULL,16);
		if (color != 0 )
			m_ServerGroupListCtrl.SetItemColor(nRow,0,color);

		if (m_strSelectedServerGroupTemp == L"")
			m_strSelectedServerGroupTemp = wstrTemp;
		
		ServerInfoVector serverInfoVector;
		for ( TiXmlNode* pServerItem = pServerGroup->FirstChild("z");pServerItem;pServerItem = pServerItem->NextSibling())
		{
			TiXmlElement * pXmlElement = pServerItem->ToElement();
			wstring serverName = utf8_to_utf16(pXmlElement->Attribute("n"));
			CServerInfo serverInfo;
			serverInfo.name		= pXmlElement->Attribute("n");
			serverInfo.server	= pXmlElement->Attribute("s");
			serverInfo.version	= pXmlElement->Attribute("v");
			serverInfo.param	= pXmlElement->Attribute("param");
			serverInfo.icon		= strtol(pXmlElement->Attribute("icon"),NULL,10);	
			serverInfo.color	= strtol(pXmlElement->Attribute("fontcolor"),NULL,16);
			serverInfo.tip		= utf8_to_utf16(pXmlElement->Attribute("tip"));
			string_replace(serverInfo.tip,L"\\n",L"\n");
			serverInfoVector.push_back(make_pair(serverName,serverInfo));
		}
		m_VectorServerGroupInfo.push_back(make_pair(wstrTemp,serverInfoVector));
	}

	delete doc;
	doc = NULL;
	//DWORD dwResult;
	//TryDeleteFile( wstrServerListPath.c_str(),dwResult);

	ReadSelectedServerStr();
	InitServerList(m_strSelectedServerGroup);
	m_ServerGroupListCtrl.SelectSubItem(m_strSelectedServerGroup);
	m_ServerItemListCtrl.SelectSubItem(m_strSelectedServer);

}

void CSelectServerDlg::InitServerList(const wstring strSelectedServerGroup)
{
	this->LockWindowUpdate();
	m_ServerItemListCtrl.DeleteAllItems();
	
	for (ServerGroupInfoVector::iterator iterServerGroupInfo = m_VectorServerGroupInfo.begin(); iterServerGroupInfo != m_VectorServerGroupInfo.end(); ++iterServerGroupInfo)
	{
		if ( iterServerGroupInfo->first.c_str() == strSelectedServerGroup )
		{
			int nItemCount = 0; 
			ServerInfoVector& serverInfoVector = iterServerGroupInfo->second;
			for ( ServerInfoVector::iterator iterServerInfo = serverInfoVector.begin();iterServerInfo != serverInfoVector.end();++iterServerInfo)
			{
				int nRow = m_ServerItemListCtrl.InsertItem(nItemCount++,iterServerInfo->first.c_str());
				m_ServerItemListCtrl.SetItemToolTipText(nRow,0,iterServerInfo->second.tip.c_str());
				if (iterServerInfo->second.color != 0 )
					m_ServerItemListCtrl.SetItemColor(nRow,0,iterServerInfo->second.color);
				if (iterServerInfo->second.icon != 0 )
					m_ServerItemListCtrl.SetItemImageID(nRow,0,iterServerInfo->second.icon);
				if (++iterServerInfo != serverInfoVector.end())
				{
					m_ServerItemListCtrl.SetItemText(nRow,1,iterServerInfo->first.c_str());
					m_ServerItemListCtrl.SetItemToolTipText(nRow,1,iterServerInfo->second.tip.c_str());
					if (iterServerInfo->second.color != 0 )
						m_ServerItemListCtrl.SetItemColor(nRow,1,iterServerInfo->second.color);
					if (iterServerInfo->second.icon != 0 )
						m_ServerItemListCtrl.SetItemImageID(nRow,1,iterServerInfo->second.icon);
				}
				else
					break;
			}
			this->UnlockWindowUpdate();
			break;
		}	
	}
}

//实现根据服务器名称查找服务区大区功能
void CSelectServerDlg::ReviseServerName()
{
	ServerGroupInfoVector::iterator iterServerGroupInfo = m_VectorServerGroupInfo.begin();
	for (; iterServerGroupInfo != m_VectorServerGroupInfo.end() ;++iterServerGroupInfo )
	{
		if ( iterServerGroupInfo->first.c_str() == m_strSelectedServerGroup )
		{
			for (ServerInfoVector::iterator iterServerInfo = iterServerGroupInfo->second.begin(); iterServerInfo != iterServerGroupInfo->second.end();++iterServerInfo)
			{
				if ( iterServerInfo->first.c_str() == m_strSelectedServer)
				{	
					return;
				}

			}			
		}
	}


	iterServerGroupInfo = m_VectorServerGroupInfo.begin();
	for (; iterServerGroupInfo != m_VectorServerGroupInfo.end() ;++iterServerGroupInfo)
	{
		for (ServerInfoVector::iterator iterServerInfo = iterServerGroupInfo->second.begin(); iterServerInfo != iterServerGroupInfo->second.end();++iterServerInfo)
		{
			if ( iterServerInfo->first.c_str() == m_strSelectedServer)
			{	
				m_strSelectedServerGroup = iterServerGroupInfo->first.c_str();
				m_strSelectedServerGroupTemp = m_strSelectedServerGroup;
				return;	
			}
		}	
	}

}

void CSelectServerDlg::ReadSelectedServerStr()
{
	ifstream ifSelectedServer(SERVER_SELECTED_LOG);
	if(ifSelectedServer)
	{
		char szSelectedServerGroup[MAX_PATH];
		ifSelectedServer.getline(szSelectedServerGroup,MAX_PATH);
		m_strSelectedServerGroup =  utf8_to_utf16(szSelectedServerGroup);
		ServerGroupInfoVector::iterator iterServerGroupInfo = m_VectorServerGroupInfo.begin();
		for (; iterServerGroupInfo != m_VectorServerGroupInfo.end() ;++iterServerGroupInfo )
		{
			if ( iterServerGroupInfo->first.c_str() == m_strSelectedServerGroup )
				break;
		}

		if (iterServerGroupInfo == m_VectorServerGroupInfo.end()) // 读取的是坏数据
			m_strSelectedServerGroup = m_strSelectedServerGroupTemp; 
		else
			m_strSelectedServerGroupTemp = m_strSelectedServerGroup;
		
		char szSelectServer[MAX_PATH];
		ifSelectedServer.getline(szSelectServer,MAX_PATH);
		m_strSelectedServer = utf8_to_utf16(szSelectServer);
		ReviseServerName();
	}
	else
	{
		m_strSelectedServerGroup = m_strSelectedServerGroupTemp; 
	}
}

void CSelectServerDlg::OnNMClickListServerGroup(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMITEMACTIVATE* nm=(NMITEMACTIVATE*)pNMHDR;

	if ( nm->iItem == -1)
		return;

	m_ServerGroupListCtrl.SelectSubItem(nm->iItem, nm->iSubItem);
	m_strSelectedServerGroupTemp = m_ServerGroupListCtrl.GetItemText(nm->iItem, nm->iSubItem).GetBuffer();
	InitServerList(m_strSelectedServerGroupTemp);
	m_ServerItemListCtrl.SelectSubItem(-1, -1);
}

void CSelectServerDlg::OnNMClickListServer(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMITEMACTIVATE* nm=(NMITEMACTIVATE*)pNMHDR;
	if ( nm->iItem == -1)
		return;
	m_ServerItemListCtrl.SelectSubItem(nm->iItem, nm->iSubItem);
	m_strSelectedServer = m_ServerItemListCtrl.GetItemText(nm->iItem, nm->iSubItem).GetBuffer();
	m_strSelectedServerGroup = m_strSelectedServerGroupTemp;
}

const CServerInfo* CSelectServerDlg::GetServerInfo()
{
	for (ServerGroupInfoVector::iterator iterServerGroupInfo = m_VectorServerGroupInfo.begin(); iterServerGroupInfo != m_VectorServerGroupInfo.end() ;++iterServerGroupInfo )
	{
		if ( iterServerGroupInfo->first.c_str() == m_strSelectedServerGroup )
		{
			for (ServerInfoVector::iterator iterServerInfo = iterServerGroupInfo->second.begin(); iterServerInfo != iterServerGroupInfo->second.end();++iterServerInfo)
			{
				if ( iterServerInfo->first.c_str() == m_strSelectedServer)
				{	
					return &iterServerInfo->second;
				}
			}
		}	
	}
	return NULL;
}

void CSelectServerDlg::OnBnClickedButtonSelectCancel()
{
	ShowWindow(SW_HIDE);
	CYbtxDlg *pDlg  = (CYbtxDlg*)GetParent();
	pDlg->ShowSelectServerBtn();	
}

void CSelectServerDlg::OnBnClickedButtonSelectOK()
{
	CYbtxDlg *pDlg  = (CYbtxDlg*)GetParent();
	pDlg->ChangeServer();
	ShowWindow(SW_HIDE);
	pDlg->ShowSelectServerBtn();
}

void CSelectServerDlg::OnNMDBLCLKListServer(NMHDR *pNMHDR, LRESULT *pResult) 
{
	OnNMClickListServer(pNMHDR, pResult);
	OnBnClickedButtonSelectOK();
}

wstring CSelectServerDlg::GetServerName()
{
	return m_strSelectedServer;
}

void CSelectServerDlg::SaveSelectServer()
{
	ofstream ofsSelectedServer(SERVER_SELECTED_LOG);
	if (ofsSelectedServer)
	{
		string strTemp = utf16_to_utf8(m_strSelectedServerGroup);
		ofsSelectedServer << strTemp << "\n";
		strTemp = utf16_to_utf8(m_strSelectedServer);
		ofsSelectedServer << strTemp;
	}
	ofsSelectedServer.close();
}

LRESULT CSelectServerDlg::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	ScreenToClient(&point);
	CRect rc;
	GetClientRect(&rc);
	if (rc.PtInRect(point))
	{
		return HTBORDER;
	}
	else
		return CDialog::OnNcHitTest(point);
}
