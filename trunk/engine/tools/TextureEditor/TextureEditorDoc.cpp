// TextureEditorDoc.cpp :  CTextureEditorDoc 类的实现
//

#include "stdafx.h"
#include "TextureEditor.h"

#include "TextureEditorDoc.h"
#include ".\textureeditordoc.h"
#include "FormatSelectDlg.h"
#include "AutoBatDlg.h"
#include "DDSConvertDlg.h"
#include "GetInputStr.h"
#include <set>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTextureEditorDoc

IMPLEMENT_DYNCREATE(CTextureEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CTextureEditorDoc, CDocument)
	ON_COMMAND(ID_FILE_AUTOBAT, OnFileAutobat)
	ON_COMMAND(ID_FILE_MAPTRANS, OnFileMaptrans)
	ON_COMMAND(ID_EDIT_ADDFRAME, OnEditAddframe)
	ON_COMMAND(ID_EXPORT_TGA, OnExportTga)
	ON_COMMAND(ID_DDSCONVERT, &CTextureEditorDoc::OnDDSConvert)
END_MESSAGE_MAP()


// CTextureEditorDoc 构造/析构

CTextureEditorDoc::CTextureEditorDoc()
{
	// TODO: 在此添加一次性构造代码
	m_Scale=1.0f;
}

CTextureEditorDoc::~CTextureEditorDoc()
{
}

BOOL CTextureEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}

//打开文档
BOOL CTextureEditorDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	CString Str=lpszPathName;
	if(m_PathStr==Str)
		return 0;
	m_PathStr=Str;
	Str.MakeLower();

	m_Text.LoadPic( m_D3d->GetDevice(), lpszPathName );

	TCHAR* p = (TCHAR*)(LPCTSTR)Str;
	for( int i = Str.GetLength(); i > 0; i-- )
	{
		if( p[i] == '.' )
		{
			p[i] = 0;
			break;
		}
	}
	SetPathName( p );
	// TODO:  Add your specialized creation code here

	return TRUE;
}

void CTextureEditorDoc::SetTitle(LPCTSTR lpszTitle)
{
	CDocument::SetTitle( lpszTitle + CString("(") + GetFormat() + CString(")") );
}

// CTextureEditorDoc 命令
CPoint CTextureEditorDoc::SetTexture()
{
	m_D3d->SetTexture( 0, m_Text.GetD3DTexture() );
	return CPoint(m_Text.GetWidth(),m_Text.GetHeight());
}

CString CTextureEditorDoc::GetFormat()
{
	static CString sFormat[8] = { "TF_DXT1", "TF_DXT5", "TF_RGB24", "TF_ARGB32", "TF_R5G6B5", "TF_A4R4G4B4", "TF_A8","TF_ACF" };
	if(m_Text.IsValid())
		return sFormat[m_Text.GetFormat()];
	else
		return "NONE";
}

BOOL CTextureEditorDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: Add your specialized code here and/or call the base class

	CString FileName = lpszPathName;
	m_PathStr = FileName;
	FileName.MakeLower();
	// if( FileName.Find( ".tex", max( FileName.GetLength() - 4, 0 ) ) < 0 )
	//	 FileName += ".tex";

	CFormatSelectDlg Dlg;
	eTextureFormat Format = m_Text.GetFormat();
	BOOL		  MipMap = m_Text.GetMipMapCount() == 0;
	int			  FrameTime = m_Text.GetFrameTime();

	if( Dlg.DoModal( Format, MipMap, FrameTime ) == IDOK )
		m_Text.SaveTEX( m_D3d->GetDevice(), FileName, Format, MipMap ? 0 : 1, FrameTime );

	return 1;
}

void CTextureEditorDoc::OnFileAutobat()
{
	// TODO: Add your command handler code here
	CAutoBatDlg Dlg;
	Dlg.DoModal(m_D3d);
}

void CTextureEditorDoc::OnDDSConvert()
{
	// TODO: Add your command handler code here
	CDDSConvertDlg Dlg;
	Dlg.DoModal(m_D3d);
}

void CTextureEditorDoc::OnEditAddframe()
{
	// TODO: Add your command handler code here
	static int First = 0;
	static CString Files[5] = 
	{ 
		"DDS Files (*.dds)|*.dds|",
		"TGA Files (*.tga)|*.tga|",
		"JPG Files (*.jpg)|*.jpg|",
		"BMP Files (*.bmp)|*.bmp|",
		" Texture (*.tex)|*.tex|",
	};

	int i,pre = 0;
	CString Filter = Files[First];
	for( i = 4; i > 0; i-- )
	{
		if( i == First )
			pre = 1;
		Files[i] = Files[ i - pre ];
	}
	Files[0] = Filter;

	for( i = 1; i < 5; i++ )
	{
		Filter += Files[i];
	}
	Filter += "|";

	set<CString> FileNames;
	CFileDialog Dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT|OFN_NOCHANGEDIR, Filter );
	TCHAR		tempBuffer[8000]	= {0};			// 需要选择多个文件，默认缓冲区不够大，故重新定义
	Dlg.m_ofn.lpstrFile				= tempBuffer;
	Dlg.GetOFN().nMaxFile			= 8000;
	if ( Dlg.DoModal() == IDCANCEL )
	{
		return;
	}

	POSITION Pos = Dlg.GetStartPosition();
	while( Pos )
	{
		FileNames.insert( Dlg.GetNextPathName( Pos ) );
	}
	if( FileNames.size() == 0 )
	{	
		return;
	}

	CTexture Texture;
	set<CString>::iterator it = FileNames.begin();
	Texture.LoadPic( m_D3d->GetDevice(), (*it) );
	it++;
	for( ; it != FileNames.end(); it++ )
	{
		Texture.AddExFrame( (*it) );
	}

	static TCHAR TexFilter[] = "ARkTexture Files (*.tex)|*.tex||";
	CFileDialog SaveDlg( FALSE, NULL, NULL, OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT|OFN_NOCHANGEDIR, TexFilter );
	if ( SaveDlg.DoModal() == IDCANCEL )
	{
		return;
	}

	CString FileName = SaveDlg.GetPathName();
	if( FileName.IsEmpty() )
	{
		return;
	}
	if( FileName.Find( ".tex", max( FileName.GetLength() - 4, 0 ) ) < 0 )
	{
		FileName += ".tex";
	}

	CFormatSelectDlg FmtDlg;
	eTextureFormat	 Format		= Texture.GetFormat();
	BOOL			 MipMap		= (Texture.GetMipMapCount() == 0);
	int				 FrameTime	= Texture.GetFrameTime();

	if( FmtDlg.DoModal( Format, MipMap, FrameTime ) == IDOK )
	{
		Texture.SaveTEX( m_D3d->GetDevice(), FileName, Format, 1, FrameTime );
	}
}

void CTextureEditorDoc::OnFileMaptrans()
{
	static TCHAR Filter[] = "Mipmap Files (*.bmp)|*.bmp||";
	
	CFileDialog Dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT|OFN_NOCHANGEDIR, Filter );
	
	if ( Dlg.DoModal() == IDOK )
	{
		POSITION Pos = Dlg.GetStartPosition();
				
		CString str = Dlg.GetNextPathName( Pos );
		
		std::string strSTL = str.GetBuffer();

		std::locale loc;
		use_facet< ctype<TCHAR> >( loc ).tolower
			( &strSTL[0], &strSTL[strSTL.length()] );
		
		FILE* fp = fopen(strSTL.c_str(), "rb");
		
		int nWidth;
		int nHeight;

		fseek(fp, 18, SEEK_SET);
		fread(&nWidth, 4, 1, fp);
		fread(&nHeight, 4, 1, fp);

		int nSize = nWidth*nHeight/2;
		
		fclose(fp);

		LPDIRDEV pDevice = m_D3d->GetDevice();
		
		IDirect3DSurface9 * pMiniMapSurface; 
        
		HRESULT hr = pDevice->CreateOffscreenPlainSurface( 
			128,
			128,
			D3DFMT_A8R8G8B8, //D3DFMT_DXT1, 
			D3DPOOL_DEFAULT, 
			&pMiniMapSurface,NULL );
		
		if ( FAILED(hr) )
		{
			::MessageBox(NULL,"创建表面失败","错误！",MB_OK);		
		}
		
		if ( D3D_OK != D3DXLoadSurfaceFromFile(pMiniMapSurface,NULL,NULL,strSTL.c_str(),NULL,D3DX_DEFAULT,NULL,NULL) )
		{
			::MessageBox(NULL,"导入数据失败","错误！",MB_OK);		
		}
		
		// save compressed texture
		int nPos = strSTL.find(".bmp");
		std::string strMinimap = strSTL.substr(0,nPos) + ".amm";
		
		fp = fopen(strMinimap.c_str(), "wb");
		
		fwrite(&nWidth,4,1,fp);
		fwrite(&nHeight,4,1,fp);
				
		D3DLOCKED_RECT LockedRect;
		pMiniMapSurface->LockRect(&LockedRect,NULL,NULL);
		
		fwrite(LockedRect.pBits,nSize,1,fp);
		
		pMiniMapSurface->UnlockRect();

		fclose(fp);
	}
}

void CTextureEditorDoc::OnExportTga()
{
	// TODO: Add your command handler code here
	static TCHAR TexFilter[] = "TGA Files (*.tga)|*.tga||";
	CFileDialog Dlg( FALSE, NULL, NULL, OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT|OFN_NOCHANGEDIR, TexFilter );

	Dlg.DoModal();

	m_Text.SaveAsTga( Dlg.GetPathName() );
}
