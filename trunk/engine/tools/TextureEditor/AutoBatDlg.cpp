// CAutoBatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TextureEditor.h"
#include "AutoBatDlg.h"
#include ".\AutoBatDlg.h"
#include "FileTree.h"
#include <fstream>
using namespace std;

// CAutoBatDlg dialog

DWORD WINAPI TranThread(LPVOID lpParameter)
{
	CAutoBatDlg* bat=(CAutoBatDlg*)lpParameter;
	bat->Run();
	return 0;
}

void FindFileNum( CString& Path, CString& SubStr, vector< CString >& Out )
{
	static CString CurPath = ".", ParentPath = "..";
	WIN32_FIND_DATA fd;

	CString Full = Path + "\\*";
	HANDLE h = FindFirstFile( Full, &fd );
	if( h != INVALID_HANDLE_VALUE )
	{
		int re=1;
		while(re)
		{
			if( CurPath != fd.cFileName && ParentPath != fd.cFileName )
			{
				if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				{
					if( fd.cFileName[0] != '.' )
                        FindFileNum( Path + "\\" + fd.cFileName, SubStr, Out );
				}
				else
				{
					int i = strlen( fd.cFileName );
					for( ; i > 0; i-- )
					{
						if( fd.cFileName[i] == '.' )
							break;
					}
					CString str = fd.cFileName + i + 1;
					if( str.CompareNoCase( SubStr )  == 0 )
					{
						Out.push_back( Path + "\\" + fd.cFileName );
					}
				}
			}

			re = FindNextFile(h,&fd);
		}
		FindClose(h);
	}
}

IMPLEMENT_DYNAMIC(CAutoBatDlg, CDialog)
CAutoBatDlg::CAutoBatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoBatDlg::IDD, pParent)
{
	m_IDAlpha = IDC_ATDXT5;
	m_IDNoAlpha = IDC_ATDXT1;
	m_bMipMap = TRUE;
	m_PicPath.GetMsgNum(1);
	m_SavePath.GetMsgNum(2);
	m_SaveType	= CTexture::eSTY_DDS;
}

CAutoBatDlg::~CAutoBatDlg()
{
}

void CAutoBatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	CDialog::CheckRadioButton( IDC_ATDXT5,IDC_ATARGB32, m_IDAlpha );
	CDialog::CheckRadioButton( IDC_ATDXT1,IDC_ATRGB24, m_IDNoAlpha );
	CDialog::CheckDlgButton( IDC_ENABLEMIP, m_bMipMap );
	DDX_Control(pDX, IDC_SAVEPROC, m_FileSaveProc);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_PICPATH, m_PicPath);
	DDX_Control(pDX, IDC_SAVEPATH, m_SavePath);
	DDX_Control(pDX, IDC_COMBOSAVETYPE, m_comboSaveType);
}

BOOL CAutoBatDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 初始化存储类型下拉列表框选项

	m_comboSaveType.AddString( "dds 文件格式" );

	m_comboSaveType.SetCurSel(0);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CAutoBatDlg, CDialog)
//	ON_COMMAND(ID_FILE_AUTOBAT, OnFileAutobat)
ON_MESSAGE(WM_DROP_MESSAGE, OnDropMessage)
ON_MESSAGE(WM_DROP_MESSAGE1, OnDropMessage1)
ON_BN_CLICKED(IDC_ATARGB32, OnBnClickedAtargb32)
ON_BN_CLICKED(IDC_ATDXT5, OnBnClickedAtdx5)
ON_BN_CLICKED(IDC_ATDXT1, OnBnClickedAtdxt1)
ON_BN_CLICKED(IDC_ATRGB24, OnBnClickedAtrgb24)
ON_BN_CLICKED(IDC_PICBROWN, OnBnClickedPicbrown)
ON_BN_CLICKED(IDC_SAVEBROWN, OnBnClickedSavebrown)
ON_BN_CLICKED(IDC_ENABLEMIP, OnBnClickedEnablemip)
ON_BN_CLICKED(IDC_ATR5G6B5, OnBnClickedAtr5g6b5)
ON_BN_CLICKED(IDC_ATA4R4G4B4, OnBnClickedAta4r4g4b4)
ON_WM_CREATE()
ON_CBN_SELCHANGE(IDC_COMBOSAVETYPE, &CAutoBatDlg::OnCbnSelchangeCombosavetype)
ON_BN_CLICKED(IDOK, &CAutoBatDlg::OnBnClickedOk)
END_MESSAGE_MAP()

LRESULT CAutoBatDlg::OnDropMessage(WPARAM wParam, LPARAM lParam)
{
	m_CurPath = m_PicPath.m_Path;
	m_PicPath.SetWindowText(m_PicPath.m_Path);
	UpdateData(FALSE);
	return 0;
}

LRESULT CAutoBatDlg::OnDropMessage1(WPARAM wParam, LPARAM lParam)
{
	m_CurPath = m_SavePath.m_Path;
	m_SavePath.SetWindowText(m_SavePath.m_Path);
	UpdateData(FALSE);
	return 0;
}
// CAutoBatDlg message handlers

void CAutoBatDlg::OnBnClickedAtargb32()
{
	m_IDAlpha = IDC_ATARGB32;
}

void CAutoBatDlg::OnBnClickedAtdx5()
{
	m_IDAlpha = IDC_ATDXT5;
}

void CAutoBatDlg::OnBnClickedAta4r4g4b4()
{
	m_IDAlpha = IDC_ATA4R4G4B4;
}

void CAutoBatDlg::OnBnClickedAtdxt1()
{
	m_IDNoAlpha = IDC_ATDXT1;
}

void CAutoBatDlg::OnBnClickedAtrgb24()
{
	m_IDNoAlpha = IDC_ATRGB24;
}

void CAutoBatDlg::OnBnClickedAtr5g6b5()
{
	m_IDNoAlpha = IDC_ATR5G6B5;
}

void CAutoBatDlg::OnBnClickedEnablemip()
{
	m_bMipMap = !m_bMipMap;
}

INT_PTR CAutoBatDlg::DoModal(CD3d* d3d)
{
	m_D3d=d3d;
	return CDialog::DoModal();
}

void CAutoBatDlg::OnBnClickedPicbrown()
{
	UpdateData();
	CDirSelectWnd Dlg;
	m_CurPath = m_PicPath.m_Path = Dlg.GetDirector( m_CurPath );
	m_PicPath.SetWindowText(m_PicPath.m_Path);
	UpdateData(FALSE);
}

void CAutoBatDlg::OnBnClickedSavebrown()
{
	UpdateData();
	CDirSelectWnd Dlg;
	m_CurPath = m_SavePath.m_Path = Dlg.GetDirector( m_CurPath );
	m_SavePath.SetWindowText(m_SavePath.m_Path);
	UpdateData(FALSE);
}

void CAutoBatDlg::OnOK()
{
	UpdateData();
	(GetDlgItem(IDOK))->EnableWindow(0);
	m_IsCancel = FALSE;
	DWORD id;
	CreateThread(NULL,0,TranThread,this,0,&id);
}

void CAutoBatDlg::OnCancel()
{
	if(m_IsCancel)
		CDialog::OnCancel();
	else
		m_IsCancel = TRUE;
}

int CAutoBatDlg::Run()
{
	//const int  ExtNum  = 5;
	//static CString Ext[ExtNum]={ "tex","tga","jpg","dds","bmp" };
	const int  ExtNum  = 4;
	static CString Ext[ExtNum]={ "tex","tga","jpg","bmp" };
	vector<CString> FileName[ExtNum];

	int Num = 0;
	int i;
	
	// 检测用户是否没有输入批量转换路径

	if ( m_PicPath.m_Path.IsEmpty() )
	{
		::MessageBox( NULL, "请输入源路径，谢谢。", "提示", MB_OK);
		m_IsCancel=TRUE;
		(GetDlgItem(IDOK))->EnableWindow(1);
		return 0;
	}
	if ( m_SavePath.m_Path.IsEmpty() )
	{
		::MessageBox( NULL, "请输入目标路径，谢谢。", "提示", MB_OK );
		m_IsCancel=TRUE;
		(GetDlgItem(IDOK))->EnableWindow(1);
		return 0;
	}

	for( i=0; i<ExtNum; i++ )
	{
		FindFileNum( m_PicPath.m_Path, Ext[i], FileName[i] );
		Num += FileName[i].size();
	}

	int Cur = 0;
	for( i=0; i<ExtNum; i++ )
	{
		for( int j = 0; j < (int)FileName[i].size(); j++ )
		{
			LPCTSTR pFile  = ( (LPCTSTR)FileName[i][j] ) + m_PicPath.m_Path.GetLength();
			CString& Pic   = FileName[i][j];
			CString  Save  = m_SavePath.m_Path  + pFile;

			// 先去掉原文件扩展名，之后根据选择的存储格式加上对应扩展名
			int		nDotPos		= Save.ReverseFind('.');
			if ( nDotPos > 0 )
			{
				Save.Delete(nDotPos, 128);
			}
			if ( m_SaveType == CTexture::eSTY_TEX )
			{
				Save	+= ".tex";
			}
			else if ( m_SaveType == CTexture::eSTY_DDS )
			{
				Save	+= ".dds";
			}

			CTexture Text;
			int hT = Text.LoadPic( m_D3d->GetDevice(), Pic );

			if( SUCCEEDED( hT ) )
			{
				eTextureFormat TextFormat;
				if( i == 0  || i == 1 || i == 2 || i == 3 )
				{
					if( m_IDAlpha == IDC_ATARGB32 )
						TextFormat = TF_ARGB32;
					else if( m_IDAlpha == IDC_ATA4R4G4B4 )
						TextFormat = TF_A4R4G4B4;
					else if( m_IDAlpha == IDC_ATDXT5 )
						TextFormat = TF_DXT5;
				}
				else
				{
					if( m_IDNoAlpha == IDC_ATRGB24 )
						TextFormat = TF_RGB24;
					else if( m_IDNoAlpha == IDC_ATR5G6B5 )
						TextFormat = TF_R5G6B5;
					else if( m_IDNoAlpha == IDC_ATDXT1 )
						TextFormat = TF_DXT1;
				}
				
				ifstream temp(Save.GetString());
				if ( temp.good() )
				{
					Cur++;
					continue;
				}

				INT		ret;
				ret = Text.SaveTEX( m_D3d->GetDevice(), Save, 
									TextFormat, Text.GetMipMapCount(), Text.GetFrameTime() ) ;
				if ( ret == GERROR )
				{
					CString		str		= Save;
					str			+=  "\n转换失败，请使用2的整数次幂的纹理。谢谢。";
					::MessageBox( NULL, str.GetBuffer(), "错误", MB_OK );
				}
			}
			Cur++;
			m_Progress.SetPos(Cur*100/Num);
			m_FileSaveProc.SetWindowText( pFile+1 );
		}
	}

	::Sleep(300);
	m_FileSaveProc.SetWindowText( "转换完成" );
	m_Progress.SetPos(0);
	m_IsCancel=TRUE;
	(GetDlgItem(IDOK))->EnableWindow(1);

	return 0;
}

void CAutoBatDlg::OnCbnSelchangeCombosavetype()
{
	// TODO: Add your control notification handler code here
	m_SaveType	= static_cast<CTexture::ESaveType>( m_comboSaveType.GetCurSel() + 1 );
}

void CAutoBatDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
