// PathBrowser.cpp : implementation file
//

#include "stdafx.h"
#include "PathBrowser.h"
#include "DirectoryDialog.h"
#include "CPkgFile.h"
#include "CodeCvs.h"

extern const wstring PATH_ALIAS_GUIEDITOR;

// CPathBrowser dialog

CPathBrowser::PathList	CPathBrowser::st_lstPath;	
string					CPathBrowser::st_szCtrlType = "PathBrowser";
//TiXmlNode*				CPathBrowser::pConfig_xml = NULL;

IMPLEMENT_DYNAMIC(CPathBrowser, CDialog)

CPathBrowser::CPathBrowser(::CWnd* pParent /*=NULL*/)
	: CDialog(CPathBrowser::IDD, pParent)
	, m_Index(-1)
{

}

CPathBrowser::~CPathBrowser()
{
}

bool CPathBrowser::Initialize(void)
{
	st_lstPath.clear();
	CPkgFile::ClearSearchPath();

	TiXmlNode* pConfig_xml = CEditorConfig::GetInst()->GetEditorConfig(st_szCtrlType);
	pConfig_xml = CEditorConfig::GetInst()->GetEditorConfig(st_szCtrlType);

	for( TiXmlNode* pNode = pConfig_xml->FirstChild("Path"); pNode; pNode = pNode->NextSibling("Path") )
	{
		TiXmlElement* pElem  = pNode->ToElement();
		if(pElem && pElem->GetText())
		{
			st_lstPath.push_back(gbk_to_utf16(pElem->GetText()));
			CPkgFile::AddLoadPath(NULL, gbk_to_utf16(pElem->GetText()).c_str());
		}
	}

	return true;
}

void CPathBrowser::OnBnClickedSave()
{
	OnLbnSetfocusDirList();

	st_lstPath.clear();
	TiXmlNode* pConfig_xml = CEditorConfig::GetInst()->GetEditorConfig(st_szCtrlType);
	pConfig_xml->Clear();
	CPkgFile::ClearSearchPath();

	for(int i = 0; i<m_DirList.GetCount(); ++i)
	{
		CString Path;
		m_DirList.GetText(i,Path);
		st_lstPath.push_back(Path.GetBuffer());
		TiXmlElement PathNode("Path");
		TiXmlText	PathText(utf16_to_gbk(Path.GetBuffer()).c_str());
		PathNode.InsertEndChild(PathText);
		pConfig_xml->InsertEndChild(PathNode);
		CPkgFile::AddLoadPath(NULL, Path.GetBuffer());
	}
	CEditorConfig::GetInst()->SaveEditorConfig();
	return;
}

BOOL CPathBrowser::OnInitDialog()
{
	BOOL ret = CDialog::OnInitDialog();
	CButton* pButton = NULL;
	m_ToolTips.Create(this);
	RECT rect;
	pButton = (::CButton*)GetDlgItem( IDC_PathBrowser_DOWN );
	pButton->SetIcon( AfxGetApp()->LoadIcon( IDI_PathBrowser_DOWN_ICON ) );
	pButton->GetClientRect(&rect);
	m_ToolTips.AddTool(pButton,L"下移");//,&rect,IDC_PathBrowser_DOWN);

	pButton = (::CButton*)GetDlgItem( IDC_PathBrowser_UP );
	pButton->SetIcon( AfxGetApp()->LoadIcon( IDI_PathBrowser_UP_ICON ) );
	pButton->GetClientRect(&rect);
	m_ToolTips.AddTool(pButton,L"上移");//,&rect,IDC_PathBrowser_UP);
	
	pButton = (::CButton*)GetDlgItem( IDC_PathBrowser_NEW );
	pButton->SetIcon( AfxGetApp()->LoadIcon( IDI_PathBrowser_ADD_ICON ) );
	pButton->GetClientRect(&rect);
	m_ToolTips.AddTool(pButton,L"新建");//,&rect,IDC_PathBrowser_NEW);

	pButton = (::CButton*)GetDlgItem( IDC_PathBrowser_DELETE );
	pButton->SetIcon( AfxGetApp()->LoadIcon( IDI_PathBrowser_DEL_ICON ) );
	pButton->GetClientRect(&rect);
	m_ToolTips.AddTool(pButton,L"删除");//,&rect,IDC_PathBrowser_DELETE);

	m_ToolTips.SetDelayTime(0);
	m_ToolTips.Activate(TRUE);
	m_DirList.SetItemHeight(0,15);

	PathList::iterator it,eit = st_lstPath.end();
	m_DirList.ResetContent();
	for( it = st_lstPath.begin(); it!=eit; ++it )
		m_DirList.AddString((*it).c_str());
		
	m_SelPathButton.ShowWindow(SW_HIDE);
	m_PathEdit.ShowWindow(SW_HIDE);	

	m_PathEdit.SetParent(&m_DirList);
	m_SelPathButton.SetParent(&m_DirList);
	m_SelPathButton.Init(this);
	return ret;
}
BOOL CPathBrowser::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_LBUTTONUP || pMsg->message == WM_MOUSEMOVE )
	{
		m_ToolTips.Activate(TRUE);
		m_ToolTips.RelayEvent(pMsg);
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CPathBrowser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control( pDX, IDC_PathBrowser_DIR_LIST,	m_DirList );
	DDX_Control( pDX, IDC_PathBrowser_SETDIR,	m_SelPathButton );
	DDX_Control( pDX, IDC_PathBrowser_PATHEDIT,m_PathEdit );
	//DDX_Control( pDX, IDC_PathBrowser_PARENT_WND,m_ParentWnd );
}


BEGIN_MESSAGE_MAP(CPathBrowser, CDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_PathBrowser_SETDIR, &CPathBrowser::OnBnClickedSelDir)
	ON_BN_CLICKED(IDC_PathBrowser_DOWN, &CPathBrowser::OnBnClickedDown)
	ON_BN_CLICKED(IDC_PathBrowser_DELETE, &CPathBrowser::OnBnClickedDelete)
	ON_BN_CLICKED(IDC_PathBrowser_NEW, &CPathBrowser::OnBnClickedNew)
	ON_BN_CLICKED(IDC_PathBrowser_UP, &CPathBrowser::OnBnClickedUp)
	ON_BN_CLICKED(IDC_PathBrowser_OK, &CPathBrowser::OnBnClickedOk)
	ON_BN_CLICKED(IDC_PathBrowser_Cancel, &CPathBrowser::OnBnClickedCancel)
	ON_LBN_DBLCLK(IDC_PathBrowser_DIR_LIST, &CPathBrowser::OnLbnDblclkDirList)
	ON_BN_CLICKED(IDC_PathBrowser_Save, &CPathBrowser::OnBnClickedSave)
	ON_LBN_SELCHANGE(IDC_PathBrowser_DIR_LIST, &CPathBrowser::OnLbnSelchangeDirList)
	ON_LBN_SETFOCUS(IDC_PathBrowser_DIR_LIST, &CPathBrowser::OnLbnSetfocusDirList)
END_MESSAGE_MAP()

void CPathBrowser::OnBnClickedDown()
{
	OnLbnSetfocusDirList();

	int Index = m_DirList.GetCurSel();
	if( Index<0 || ++Index >= m_DirList.GetCount() )
		return;

	CString str;
	m_DirList.GetText(Index-1, str);	
	m_DirList.DeleteString(Index-1);
	m_DirList.InsertString( Index,str.GetBuffer() );
	m_DirList.SetCurSel(Index);
}

void CPathBrowser::OnBnClickedDelete()
{
	OnLbnSetfocusDirList();
	int Index = m_DirList.GetCurSel();
	if( Index < 0 )
		return;
	m_DirList.DeleteString(Index);
}

void CPathBrowser::OnBnClickedNew()
{
	OnLbnSetfocusDirList();
	int Index = m_DirList.GetCurSel();
	if( Index < 0 )
	{
		m_DirList.AddString(L"");
		Index = m_DirList.GetCount();
		--Index;
	}
	else
		m_DirList.InsertString( ++Index,L"");
	m_DirList.SetCurSel(Index);
	OnLbnDblclkDirList();
}

void CPathBrowser::OnBnClickedUp()
{
	OnLbnSetfocusDirList();
	int Index = m_DirList.GetCurSel();
	if( --Index<0 )
		return;

	CString str;
	m_DirList.GetText(Index+1, str);	
	m_DirList.DeleteString(Index+1);
	m_DirList.InsertString( Index,str.GetBuffer() );
	m_DirList.SetCurSel(Index);
}

void CPathBrowser::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnBnClickedSave();
	CDialog::OnOK();
}

void CPathBrowser::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}

void CPathBrowser::OnLbnDblclkDirList()
{
	// TODO: Add your control notification handler code here
	m_Index = m_DirList.GetCurSel();
	if( m_Index < 0 )
		return;

	CString str;
	int Height;
	RECT rect;
	RECT rectB;
	long Width;
	m_DirList.GetText(m_Index, str);
	m_DirList.GetClientRect(&rect);
	m_SelPathButton.GetClientRect(&rectB);

	Width = rectB.right - rectB.left;

	
	Height = m_DirList.GetItemHeight(m_Index);
	m_PathEdit.SetWindowText(str);
	rect.top += Height * m_Index;
	rect.bottom = rect.top + Height;
	rect.left++;
	rect.right--;

	
	//m_ParentWnd.MoveWindow(&rect);
	//m_ParentWnd.GetClientRect(&rect);
	rect.right -= Width;
	m_PathEdit.MoveWindow(&rect);
	
	rectB.top = rect.top;
	rectB.bottom = rect.bottom;
	rectB.left = rect.right ;
	rectB.right = rect.right + Width;
	m_SelPathButton.MoveWindow(&rectB);

	m_SelPathButton.ShowWindow(SW_SHOW);
	m_PathEdit.ShowWindow(SW_SHOW);	
	//m_ParentWnd.ShowWindow(SW_SHOW);
}	

void CPathBrowser::OnBnClickedSelDir()
{
	// TODO: Add your control notification handler code here
	CFileDialog Files( TRUE,NULL,NULL,0,NULL,this);
	Files.DoModal();
	OnLbnSetfocusDirList();
}

void CPathBrowser::OnLbnSelchangeDirList()
{
	OnLbnSetfocusDirList();
	// TODO: Add your control notification handler code here
}

void CPathBrowser::OnLbnSetfocusDirList()
{
	m_SelPathButton.ShowWindow(SW_HIDE);
	m_PathEdit.ShowWindow(SW_HIDE);	
	//m_ParentWnd.ShowWindow(SW_HIDE);

	if( m_Index>=0 )
	{
		CString str;
		m_PathEdit.GetWindowText(str);
		m_DirList.InsertString(m_Index,str.GetBuffer());
		m_DirList.DeleteString(++m_Index);
		m_Index = -1;
	}
	// TODO: Add your control notification handler code here
}

void  CPathBrowser::SelectDir(void)
{
	CDirectoryDialog Files(this);
	CString path;
	m_PathEdit.GetWindowText(path);
	
	if( IDOK == Files.DoModal(path) )
	{
		m_PathEdit.SetWindowText(path.GetBuffer());
	}
	//OnLbnSetfocusDirList();
}
bool	CPathBrowser::GetResFileName(wstring& str)
{
	CString tStr = str.c_str();
	tStr.MakeLower();
	str = tStr.GetBuffer();
	PathList::iterator it,eit = st_lstPath.end();
	for( it = st_lstPath.begin(); it!=eit; ++it )
	{
		CString tmp = (*it).c_str();
		if(tmp[tmp.GetLength()-1]!=L'\\')
			tmp.Append(L"\\");

		tmp.MakeLower();
		size_t pos = str.find(tmp.GetBuffer());
		
		if(pos != -1)
		{
			str = str.substr((*it).length(),str.length() - (*it).length());
			if( str[0] == '/' || str[0] == '\\')
				str = str.substr(1,str.length() - 1);
			return true;
		}
	}
	return false;
}
bool	CPathBrowser::GetFirstPath(wstring& str)
{
	CString tStr = str.c_str();
	tStr.MakeLower();
	str = tStr.GetBuffer();
	PathList::iterator it,eit = st_lstPath.end();
	for( it = st_lstPath.begin(); it!=eit; ++it )
	{
		CString tmp = (*it).c_str();
		tmp.MakeLower();
		str = tmp.GetBuffer();
		return true;
	}
	return false;
}

bool	CPathBrowser::GetPathByIndex(wstring& str,size_t i)
{
	CString tStr = str.c_str();
	tStr.MakeLower();
	str = tStr.GetBuffer();
	PathList::iterator it,eit = st_lstPath.end();
	it = st_lstPath.begin();
	for( size_t t = 0; t <= i; ++t )
	{
		if (it == eit)
		{
			return false;
		}
		if (t == i)
		{
			CString tmp = (*it).c_str();
			tmp.MakeLower();
			str = tmp.GetBuffer();
			return true;
		}
		else
		{
			++it;
		}
		
	}
	return false;
}

IMPLEMENT_DYNAMIC(CPathOpen, CButton)
BEGIN_MESSAGE_MAP(CPathOpen, CButton)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()
CPathOpen::CPathOpen(){}
CPathOpen::~CPathOpen(){}

void	 CPathOpen::Init(CPathBrowser* pParent)
{  m_pParent = pParent; }

void CPathOpen::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	m_pParent->SelectDir();
	CButton::OnLButtonDblClk(nFlags, point);
}
