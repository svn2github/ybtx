// DDSConvertDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TextureEditor.h"
#include "DDSConvertDlg.h"
#include "FileTree.h"

// CDDSConvertDlg dialog

DWORD WINAPI	DDSTranThread( LPVOID lpParameter )
{
	CDDSConvertDlg*	pDlg	= (CDDSConvertDlg*)lpParameter;
	pDlg->Run();
	return 0;
}

IMPLEMENT_DYNAMIC(CDDSConvertDlg, CDialog)

CDDSConvertDlg::CDDSConvertDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDDSConvertDlg::IDD, pParent)
	, m_DDSTransFlag(DDSTF_KEEPORIGIN)
{
	m_vecSupportExt.push_back(".tex");
	/*m_vecSupportExt.push_back(".bmp");
	m_vecSupportExt.push_back(".tga");
	m_vecSupportExt.push_back(".jpg");
	m_vecSupportExt.push_back(".dds");*/

	m_strTransFailedDir	= "";
	m_nTransFailedNum	= 0;
}

CDDSConvertDlg::~CDDSConvertDlg()
{
}

void CDDSConvertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OLDPICPATH, m_PicPath);
	DDX_Control(pDX, IDC_DDSTRANSINFO, m_ddsTransInfo);
}


BEGIN_MESSAGE_MAP(CDDSConvertDlg, CDialog)
	ON_BN_CLICKED(IDC_OLDPICBROWN, &CDDSConvertDlg::OnBnClickedOldPicBrown)
	ON_BN_CLICKED(IDOK, &CDDSConvertDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDDSConvertDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RADIOCOMPRESS, &CDDSConvertDlg::OnBnClickedRadiocompress)
	ON_BN_CLICKED(IDC_RADIOKEEPORIGIN, &CDDSConvertDlg::OnBnClickedRadioKeepOrigin)
END_MESSAGE_MAP()


// CDDSConvertDlg message handlers

void CDDSConvertDlg::OnBnClickedOldPicBrown()
{
	UpdateData();
	CDirSelectWnd Dlg;
	m_PicPath.m_Path = Dlg.GetDirector( "" );
	m_PicPath.SetWindowText(m_PicPath.m_Path);
	UpdateData(FALSE);
}

void CDDSConvertDlg::OnBnClickedOk()
{
	UpdateData();
	(GetDlgItem(IDOK))->EnableWindow(FALSE);
	DWORD	id;
	CreateThread( NULL, 0, DDSTranThread, this, 0, &id );
}

void CDDSConvertDlg::OnBnClickedCancel()
{
	OnCancel();
}

int CDDSConvertDlg::Run()
{
	if ( m_PicPath.m_Path.IsEmpty() )
	{
		MessageBox( "请输入图片目录，谢谢。", "错误" );
		(GetDlgItem(IDOK))->EnableWindow(TRUE);
		return 0;
	}
	if ( m_DDSTransFlag	== DDSTF_UNCHOOSE )
	{
		MessageBox( "请选择转换类型，谢谢。", "提示" );
		(GetDlgItem(IDOK))->EnableWindow(TRUE);
		return 0;
	}

	m_nTransFailedNum	= 0;
	m_strTransFailedDir	= m_PicPath.m_Path + "\\" + "没有用2^尺寸的纹理" + "\\" ;
	CreateDirectory( m_strTransFailedDir, NULL );
	m_ddsTransInfo.SetWindowText( "等待转换" );
	ConvertAllPicToDDS( m_PicPath.m_Path );

	CString	transInfo;
	transInfo.Format( "转换结束\n转换失败文件数：%d\n这些文件保存在", m_nTransFailedNum );
	transInfo += m_strTransFailedDir;
	m_ddsTransInfo.SetWindowText( transInfo );
	
	m_PicPath.m_Path	= "";
	m_PicPath.SetWindowText("");
	(GetDlgItem(IDOK))->EnableWindow(TRUE);

	return 0;
}

void CDDSConvertDlg::ConvertAllPicToDDS( CString strPicDir )
{
	const CString	CURR_PATH	= ".";
	const CString	PARENT_PATH	= "..";
	WIN32_FIND_DATA	fd;
	
	CString		strFullPath;
	strFullPath	= strPicDir + "\\*";
	HANDLE	h	= FindFirstFile( strFullPath, &fd );
	if ( h != INVALID_HANDLE_VALUE )
	{
		BOOL	bMoreFile	= 1;
		while ( bMoreFile )
		{
			if ( CURR_PATH != fd.cFileName && PARENT_PATH != fd.cFileName )
			{
				if ( fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY )
				{
					if ( fd.cFileName[0] != '.' )
					{
						ConvertAllPicToDDS( strPicDir + "\\" + fd.cFileName );
					}
				}
				else
				{
					int		nDotPos=0;
					BOOL	bCanConvert		= FALSE;
					CStringVec::iterator	it	= m_vecSupportExt.begin();
					for ( ; it != m_vecSupportExt.end() ; it++ )
					{
						if ( (*it) == (TCHAR*)(fd.cFileName) + strlen(fd.cFileName) - 4 )
						{
							CTexture	tex;
							CString		strOldFile	= strPicDir + "\\" + fd.cFileName;
							
							// 真正开始转换文件
							ConvertAndCompress( strOldFile, tex );
						}
					}
				}
			}
			bMoreFile	= FindNextFile( h, &fd ); 
		}
		FindClose(h);
	}
}

void CDDSConvertDlg::ConvertAndCompress( CString strOldFile, CTexture& tex )
{
	tex.SetUseOriginSize(FALSE);
	tex.LoadPic( m_d3d->GetDevice(), strOldFile );

	if ( tex.GetFrameCount() != 1 )
	{
		return;
	}

	m_ddsTransInfo.SetWindowText( CString() + "正在转换: " + strOldFile );

	CString	strExt = strOldFile.Right(4);
	CString strNewFile, strTemp;
	strTemp		= strOldFile; 
	strTemp.Delete( strOldFile.ReverseFind('.'), 260 );
	strNewFile	= strTemp + ".dds";

	int		transResult		= 0;
	if ( m_DDSTransFlag == DDSTF_COMPRESS )
	{
		if ( strExt == ".bmp" )
		{
			transResult	= tex.SaveTEX( m_d3d->GetDevice(), strNewFile, 
									   TF_DXT1, tex.GetOriginMipMapCount(), 0 );
		}
		else
		{
			transResult	= tex.SaveTEX( m_d3d->GetDevice(), strNewFile, 
									   TF_DXT5, tex.GetOriginMipMapCount(), 0 );
		}
	}
	else if ( m_DDSTransFlag == DDSTF_KEEPORIGIN )
	{
		transResult	= tex.SaveTEX( m_d3d->GetDevice(), strNewFile, 
								   tex.GetOriginFormat(), tex.GetOriginMipMapCount(), 0 );
	}
	
	// 若转换失败，则将文件转存到m_strTransFailedDir制定的目录
	if ( 0 != transResult )
	{
		CString	fileName;
		fileName	= strOldFile.Right( strOldFile.GetLength() - strOldFile.ReverseFind('\\') -1 );
		m_ddsTransInfo.SetWindowText( CString() + "转换失败: " + strOldFile );
		CopyFile( strOldFile, m_strTransFailedDir + fileName, FALSE );
		m_nTransFailedNum ++;
	}
}

INT_PTR CDDSConvertDlg::DoModal( CD3d* d3d )
{
	m_d3d	= d3d;
	return CDialog::DoModal();
}

void CDDSConvertDlg::OnBnClickedRadiocompress()
{
	m_DDSTransFlag	= DDSTF_COMPRESS;
}

void CDDSConvertDlg::OnBnClickedRadioKeepOrigin()
{
	m_DDSTransFlag	= DDSTF_KEEPORIGIN;
}

BOOL CDDSConvertDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CButton*)GetDlgItem(IDC_RADIOKEEPORIGIN))->SetCheck(true);
	return true;
}