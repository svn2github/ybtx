// FileTree.cpp : implementation file
//

#include "stdafx.h"
#include "FileTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////////////////////
CFileList::CFileList()
{
	m_bInit=0;
}

CFileList::~CFileList()
{
}


BEGIN_MESSAGE_MAP(CFileList, CListCtrl)
	//{{AFX_MSG_MAP(CFileList)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileList message handlers

BOOL CFileList::Create(CWnd* parent)
{	
	if(!CListCtrl::Create(LVS_REPORT|LVS_SINGLESEL|LVS_SHOWSELALWAYS|LVS_SORTASCENDING| 
		LVS_SHAREIMAGELISTS|LVS_EDITLABELS|WS_TABSTOP,CRect(0,0,0,0),parent,1000))
		return 0;;
	RECT rect;
	GetClientRect(&rect);

	int n=(rect.right-rect.left)/5;
	InsertColumn(0,"文件",LVCFMT_LEFT,n);
	InsertColumn(1,"大小",LVCFMT_LEFT,n);
	InsertColumn(2,"创建日期",LVCFMT_LEFT,n+n/2);
	InsertColumn(3,"修改日期",LVCFMT_LEFT,n+n/2);
	m_Image.Create(16,16,ILC_COLOR16|ILC_MASK,0,1);

	SetImageList(&m_Image,LVSIL_SMALL);
	SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	
	return 1;
}

BOOL CFileList::SetFileIcon( LPCTSTR extname )
{
	string str = "temp.";
	str += extname;

	SHFILEINFO shfi;
	memset( &shfi,0,sizeof(shfi) );
	SHGetFileInfo( str.c_str(), FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(shfi), 
		SHGFI_SMALLICON|SHGFI_ICON|SHGFI_USEFILEATTRIBUTES );
	m_FileExt[extname] = m_Image.Add( shfi.hIcon );

	return 1;
}

BOOL CFileList::ResetList(LPCTSTR path)
{
	this->DeleteAllItems();
	WIN32_FIND_DATA fd;
	TCHAR buf[1500];

	sprintf(m_Path,path);

	for( map< string, UINT >::iterator it = m_FileExt.begin(); it != m_FileExt.end(); it++ )
	{
		sprintf(buf,"%s",path);
		size_t n=strlen(buf);
		if(buf[n-1]=='\\'||buf[n-1]=='/')
			buf[n-1]=0;
		strcat(buf,"\\*.");
		strcat( buf, it->first.c_str() );
		HANDLE h=FindFirstFile( buf, &fd );
		if(h!=INVALID_HANDLE_VALUE)
		{
			int re=1;
			while(re)
			{
				LVITEM	lvIt;
				WORD date,time;
				
				lvIt.mask=LVIF_TEXT|LVIF_IMAGE;
				lvIt.iImage = it->second;
				lvIt.pszText = fd.cFileName;
				lvIt.iSubItem = 0;
				lvIt.iItem = GetItemCount();
				int iPos = InsertItem(&lvIt);
				
				lvIt.mask=LVIF_TEXT;
				lvIt.iItem = iPos;
				
				sprintf(buf,"%d",fd.nFileSizeLow);
				lvIt.pszText = buf;
				lvIt.iSubItem = 1;
				SetItem(&lvIt);
				
				FileTimeToDosDateTime(&fd.ftCreationTime,&date,&time);
				sprintf(buf,"%d-%02d-%02d %02d:%02d:%02d",1980+(date>>9),
					(date&0x1e0)>>5,date&0x1f,time>>11,(time&0x7e0)>>5,time&0x1f);
				lvIt.pszText = buf;
				lvIt.iSubItem = 2;
				SetItem(&lvIt);
				
				FileTimeToDosDateTime(&fd.ftLastWriteTime,&date,&time);
				sprintf(buf,"%d-%02d-%02d %02d:%02d:%02d",1980+(date>>9),
					(date&0x1e0)>>5,date&0x1f,time>>11,(time&0x7e0)>>5,time&0x1f);
				lvIt.pszText = buf;
				lvIt.iSubItem = 3;
				SetItem(&lvIt);

				re = FindNextFile(h,&fd);
			}
			FindClose(h);
		}
	}

	return 0;
}

void CFileList::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	CString name=GetItemText(pDispInfo->item.iItem,pDispInfo->item.iSubItem);
	if(pDispInfo->item.pszText==NULL||strlen(pDispInfo->item.pszText)==0)
		return;
	TCHAR Old[1500];
	TCHAR New[1500];
	sprintf(Old,"%s\\%s",m_Path,name);
	sprintf(New,"%s\\%s",m_Path,pDispInfo->item.pszText);
	int err=rename(Old,New);
	if(err)
	{		
		MessageBox("文件名非法或文件不可写");
		return;	
	}

	SetItemText(pDispInfo->item.iItem,
		pDispInfo->item.iSubItem,pDispInfo->item.pszText);

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
// CFileTree

CFileTree::CFileTree()
{
	m_SelectFloder=NULL;
	m_FileList=NULL;
}

CFileTree::~CFileTree()
{
}


BEGIN_MESSAGE_MAP(CFileTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CFileTree)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemexpanding)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileTree message handlers

BOOL CFileTree::InitChild(FLODER& floder,int order)
{
	LPSHELLFOLDER child=NULL;
	LPEXTRACTICON icon=NULL;
	LPENUMIDLIST elist=NULL;
	LPITEMIDLIST plist=NULL;
	DWORD num;
	TCHAR buf[256];
	UINT flag;
	int ico1,ico2;
	HICON ico=NULL;

	if(floder.pFloder==NULL)
		return 0;
	//删除旧的目录
	if(floder.pChild)
	{
		for(int i=0;i<floder.ChildNum;i++)
		{
			DeleteItem(floder.pChild[i]->Item);
			delete floder.pChild[i];
		}
		delete [floder.ChildNum]floder.pChild;
		floder.pChild=NULL;
		floder.ChildNum=0;
	}

	//枚举当前目录下的所有目录
	floder.pFloder->EnumObjects(m_hWnd,32,&elist);
	if(elist==NULL)
		return 0;

	while(elist->Next(1,&plist,&num)==S_OK)
	{
		STRRET dis,path;
		CString diss,paths;
		floder.pFloder->BindToObject(plist,0,IID_IShellFolder,(void**)&child);
		if(!child)
			continue;

		//得到目录信息
		floder.pFloder->GetDisplayNameOf(plist,SHGDN_FORPARSING,&path);
		//得到要显示的名字信息
		floder.pFloder->GetDisplayNameOf(plist,SHGDN_FORADDRESSBAR|SHGDN_INFOLDER,&dis);

		//根据类型获得相应字符串
		if(dis.uType==STRRET_CSTR)
			diss=CString(dis.cStr);
		else if(dis.uType==STRRET_OFFSET)
			diss=CString(((TCHAR*)plist)+dis.uOffset);
		else if(dis.uType==STRRET_WSTR)
			diss=CString(dis.pOleStr);

		if(path.uType==STRRET_CSTR)
			paths=CString(path.cStr);
		else if(path.uType==STRRET_OFFSET)
			paths=CString(((TCHAR*)plist)+path.uOffset);
		else if(path.uType==STRRET_WSTR)
			paths=CString(path.pOleStr);

		/*
		//忽略下面的“目录”
		if(!strcmp(diss,"Internet Explorer")||!strcmp(diss,"回收站")||
			!strcmp(diss,"打印机")||!strcmp(diss,"控制面板")||
			!strcmp(diss,"拨号网络")||!strcmp(diss,"计划任务"))
			continue;*/

		//在根目录下增加一个子节点
		FLODER **f=new FLODER*[floder.ChildNum+1];
		int i=0;
		for(;i<floder.ChildNum;i++)
			f[i]=floder.pChild[i];
		f[i]=new FLODER;
		f[i]->pFloder=child;
		floder.pFloder->GetUIObjectOf(m_hWnd,1,(const struct _ITEMIDLIST **)&plist,
			IID_IExtractIcon,NULL,(void**)(&icon));
		if(!icon)
			continue;

		//得到打开状态和平常状态的目录图标
		icon->GetIconLocation(GIL_FORSHELL,buf,256,&ico1,&flag);
		ExtractIconEx(buf,ico1,NULL,&ico,1);
		ico1=m_Image.Add(ico);
		icon->GetIconLocation(GIL_OPENICON,buf,256,&ico2,&flag);
		ExtractIconEx(buf,ico2,NULL,&ico,1);
		ico2=m_Image.Add(ico);
		HTREEITEM pos=TVI_LAST;
		if(order)
		{
			for(int j=0;j<floder.ChildNum;j++)
			{
				CString s=GetItemText(f[j]->Item);
				if(s.CompareNoCase(diss)>0)
				{
					FLODER *temp=f[floder.ChildNum];
					for(int k=floder.ChildNum;k>j;k--)
						f[k]=f[k-1];
					f[j]=temp;
					i=j;
					if(j>0)
						pos=f[j-1]->Item;
					else
						pos=TVI_FIRST;
					break;
				}
			}
		}

		TVINSERTSTRUCT Item;
		Item.hInsertAfter        = pos;
		Item.hParent             = floder.Item;
		Item.item.mask           = TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_PARAM;
		Item.item.pszText        = (LPSTR)(LPCTSTR)diss;
		Item.item.iImage         = ico1;
		Item.item.iSelectedImage = ico2;
		Item.item.lParam         = (LPARAM)f[i];

		f[i]->Item=InsertItem(&Item);

		f[i]->pChild=new FLODER*[1];
		f[i]->pChild[0]=new FLODER;
		f[i]->ChildNum=1;
		f[i]->pChild[0]->Item=InsertItem("",f[i]->Item);
		sprintf(f[i]->Path,"%s",paths);

		if(floder.pChild)
			delete []floder.pChild;
		floder.pChild=f;
		floder.ChildNum++;
	}
	floder.pFloder->Release();
	floder.pFloder=NULL;
	Expand(floder.Item,TVE_EXPAND);

	return 1;
}

BOOL CFileTree::Create(CWnd* parent)
{	
	if(!CTreeCtrl::Create(TVS_HASBUTTONS|TVS_HASLINES|
		TVS_LINESATROOT|TVS_SHOWSELALWAYS|WS_TABSTOP,CRect(0,0,0,0),parent,1000))
		return 0;

	//初始化桌面根目录
	if(m_desktop.pChild==NULL)
	{
		m_Image.Create(16,16,ILC_COLOR16|ILC_MASK,0,1000);
		SetImageList(&m_Image,TVSIL_NORMAL);
		
		SHGetDesktopFolder(&m_desktop.pFloder);
		m_desktop.Item=TVI_ROOT;
		InitChild(m_desktop,0);
		for(int i=0;i<m_desktop.ChildNum;i++)
			if(!strcmp(GetItemText(m_desktop.pChild[i]->Item),"我的电脑"))
				InitChild(*m_desktop.pChild[i],0);		
	}
	
	return 1;
}

LPCTSTR CFileTree::GetSelectedPath()
{
	if(m_SelectFloder)
		return m_SelectFloder->Path;
	return NULL;
}

BOOL CFileTree::SetCurPath( LPCTSTR Path )
{
	FLODER* Cur = m_desktop.pChild[0];
	TCHAR SrcPath[256];
	TCHAR DestPath[256];
	int i,j,k=0;
	LPCTSTR StartPath = Path;
	while(1)
	{
		//C:\\WINNT\\ -> C:
		for( j = 0; Path[j] !='\\' && Path[j] != 0; j++ )
			SrcPath[j] = Path[j];
		if( j == 0 )
			break;

		SrcPath[j] = 0;
		Path += j;
		for( j = 0; Path[j]=='\\'; j++ )
			Path++;

		for( i = 0; i < Cur->ChildNum; i++ )
		{
			for( j = 0; Cur->pChild[i]->Path[k+j] != '\\' &&  Cur->pChild[i]->Path[ k + j ] != 0; j++ )
				DestPath[j] = Cur->pChild[i]->Path[ k + j ];
			DestPath[j] = 0;
			if( j && _stricmp( DestPath, SrcPath ) )
				continue;
			k = (int)( Path - StartPath )/sizeof(*Path);
			Cur = Cur->pChild[i];
			InitChild(*Cur,0);
			break;
		}
		if( i== Cur->ChildNum )
			break;
	}

	SelectItem(Cur->Item);

	return 1;
}

void CFileTree::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	HTREEITEM item=GetSelectedItem();
	if(item)
	{
		FLODER* floder=(FLODER*)GetItemData(item);
		if(floder)
			m_SelectFloder=floder;
		if(m_FileList)
			m_FileList->ResetList(GetSelectedPath());
	}

	*pResult = 0;
}

void CFileTree::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	HTREEITEM item=pNMTreeView->itemNew.hItem;
	if(item)
	{
		FLODER* floder=(FLODER*)GetItemData(item);
		if(floder)
		{
			InitChild(*floder);
		}
	}

	*pResult = 0;
}



/////////////////////////////////////////////////////////////////////////////
// CFileTreeView

IMPLEMENT_DYNCREATE(CFileTreeView, CView)

CFileTreeView::CFileTreeView()
{
//	MessageBox("dsgsdg");
}

CFileTreeView::~CFileTreeView()
{
}


BEGIN_MESSAGE_MAP(CFileTreeView, CView)
	//{{AFX_MSG_MAP(CFileTreeView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileTreeView message handlers

int CFileTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	RECT rect={0,0,0,0};
	//this->GetClientRect(&rect);
	// TODO: Add your specialized creation code here
	m_pathtree.Create(this);
	m_pathtree.ShowWindow(SW_SHOW);
	return 0;
}

void CFileTreeView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	m_pathtree.SetWindowPos(NULL,0,0,cx,cy,SWP_NOZORDER);
	// TODO: Add your message handler code here	
}

/////////////////////////////////////////////////////////////////////////////
// CFileListView

IMPLEMENT_DYNCREATE(CFileListView, CView)

CFileListView::CFileListView()
{
}

CFileListView::~CFileListView()
{
}


BEGIN_MESSAGE_MAP(CFileListView, CView)
	//{{AFX_MSG_MAP(CFileListView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMCHANGED, 1000, OnItemchangedList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileListView message handlers

int CFileListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	m_filelist.Create(this);
	m_filelist.ShowWindow(SW_SHOW);
	// TODO: Add your specialized creation code here
	return 0;
}

void CFileListView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	m_filelist.SetWindowPos(NULL,0,0,cx,cy,SWP_NOZORDER);

	int n=(cx-10)/5;
	m_filelist.SetColumnWidth(0,n);
	m_filelist.SetColumnWidth(1,n);
	m_filelist.SetColumnWidth(2,n+n/2);
	m_filelist.SetColumnWidth(3,n+n/2);
	// TODO: Add your message handler code here	
}

//当列表选择改变后要调用打开文档函数
void CFileListView::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	CString str=m_filelist.GetItemText(pNMListView->iItem,pNMListView->iSubItem);
	CString path=m_filelist.GetPath();
	TCHAR ch=path.GetAt(path.GetLength()-1);
	if(ch=='\\'||ch=='//')
		path.SetAt(path.GetLength()-1,0);
	GetDocument()->OnOpenDocument(path+"\\"+str);

	*pResult = 0;
}


BEGIN_MESSAGE_MAP(CDirSelectWnd, CDialog)
END_MESSAGE_MAP()

CString CDirSelectWnd::GetDirector( const CString InitDir, CString Title )
{
	static CString sInitPath;
	if( !InitDir.IsEmpty() )
		sInitPath = InitDir;
	m_ReturnID = IDCANCEL;
	m_CurPath  = sInitPath;	

	BYTE Buf[256];
	memset( Buf, 0, 256 );
	DWORD* dwBuf = (DWORD*)Buf;
	dwBuf[0] = 0xffff0001;
	dwBuf[1] = 0;
	dwBuf[2] = 0;
	dwBuf[3] = DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU;

	WORD* wBuf = (WORD*)( Buf + 16 );
	wBuf[0] = 0;
	wBuf[1] = 0;
	wBuf[2] = 0;
	wBuf[3] = 180;
	wBuf[4] = 200;
	wBuf[5] = 0;
	wBuf[6] = 0;
	wBuf[7] = 0;

	CWnd* pWnd = AfxGetMainWnd();
	InitModalIndirect( (DLGTEMPLATE*)Buf );
	
	DoModal();
	sInitPath = m_CurPath;
	return m_CurPath;
}

BOOL CDirSelectWnd::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	m_Tree.Create( this );
	SetWindowLong( m_Tree.m_hWnd, GWL_EXSTYLE, WS_EX_CLIENTEDGE );
	m_Tree.SetWindowPos( NULL, 10, 10, 250, 250, SWP_SHOWWINDOW );
	m_Tree.SetCurPath( m_CurPath );

	m_Font.CreateFont( 12, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, PROOF_QUALITY, 0, "MS Shell Dlg" );
	m_OK.Create( "确定", WS_CHILD, CRect( 100, 270, 170, 292 ), this, IDOK );
	m_Cancel.Create( "取消", WS_CHILD, CRect( 185, 270, 255, 292 ), this, IDCANCEL );

	m_OK.SetFont( &m_Font );
	m_Cancel.SetFont( &m_Font );
	m_OK.ShowWindow( SW_SHOW );
	m_Cancel.ShowWindow( SW_SHOW );

	m_Tree.SetFocus();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDirSelectWnd::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	m_CurPath = m_Tree.GetSelectedPath();
	CDialog::OnOK();
}

void CDirSelectWnd::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	CDialog::OnCancel();
}
