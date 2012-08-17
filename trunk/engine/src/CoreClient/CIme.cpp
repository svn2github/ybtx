#include "stdafx.h"
#include "CIme.h"
#include "CRenderSystem.h"
#include "CGraphic.h"

CIme::CIme( HWND hWnd )
: m_hWnd( hWnd )
, m_uCharBuf(0)
, m_bEnableIme(true)
, m_hCurKL( NULL )
, m_hImc( NULL )
{
	m_hCurKL	= GetKeyboardLayout( 0 );
	m_hImc		= ImmGetContext( m_hWnd );
	//SetImeIcon();
}

CIme::~CIme(void)
{
	if( m_hImc )
		ImmReleaseContext( m_hWnd, m_hImc );
	m_hImc = NULL;
}

void CIme::InitIme( HWND hWnd )
{ 
	GetIme() = new CIme( hWnd ); 
}

void CIme::UnitIme()
{ 
	delete GetIme(); 
	GetIme() = NULL;
}

CIme*& CIme::GetIme()
{ 
	static CIme* _pIme = NULL; 
	return _pIme;
}

bool CIme::CorrectCharMsg( UINT& uMsg, WPARAM& wParam )
{
	if( uMsg == WM_CHAR )
	{
		if( m_uCharBuf )
		{
			uMsg = WM_IME_CHAR;
			wParam |= m_uCharBuf<<8;
			m_uCharBuf = 0;
		}
		else if( (wchar_t)wParam < 0 )
		{
			m_uCharBuf = wParam;
			return false;
		}
	}

	return true;
}

//启用、关闭输入法
void CIme::EnableIme( bool bEnable )
{ 
	if( m_bEnableIme == bEnable )
		return;

	ImmAssociateContext( m_hWnd, bEnable ? m_hImc : NULL );
	m_bEnableIme = bEnable;
}

int32 CALLBACK EnumResNameProc( HINSTANCE hModule, const char* lpszType, const char* lpszName, HICON* pIcon )
{
	HRSRC hRs;
	HGLOBAL hGl;
	void* pBuf;

	if( !( hRs = FindResource( hModule, lpszName, lpszType ) ) )
		return true;
	if( !( hGl = LoadResource( hModule, hRs ) ) )
		return true;
	if( !( pBuf = LockResource( hGl ) ) )
		return true;
	lpszName = MAKEINTRESOURCE( LookupIconIdFromDirectoryEx( (BYTE*)pBuf, TRUE, 16, 16, LR_DEFAULTCOLOR	) );

	if( !( hRs = FindResource( hModule, lpszName, RT_ICON ) ) )
		return true;
	if( !( hGl = LoadResource( hModule, hRs ) ) )
		return true;
	if( !( pBuf = LockResource( hGl ) ) )
		return true;
	*pIcon = CreateIconFromResourceEx( (PBYTE)pBuf, SizeofResource( hModule, hRs ), TRUE, 0x00030000, 16, 16, LR_DEFAULTCOLOR );

	return !( *pIcon );
}

void CIme::SetImeIcon()
{
	HICON hIcon = NULL;
	if( ImmIsIME( m_hCurKL ) )
	{
		char buf[256];
		ImmGetIMEFileName( m_hCurKL, buf, 256 );
		HMODULE hDll = LoadLibrary( buf );
		EnumResourceNames( hDll, RT_GROUP_ICON, (ENUMRESNAMEPROC)EnumResNameProc, (int32)(void*)&hIcon );
		FreeLibrary( hDll );
	}

	m_ImeImage.Release();
	if( hIcon )
	{
		IGraphic* pGraphic = CGraphic::GetInst();

		CFRect rt = CFRect( 0, 0, 16, 16 );
		m_ImeImage.AddImage( pGraphic, -1, NULL, &rt, CFPos(0.0f, 0.0f), 0xffffffff );
		pGraphic->CreateTexture( 16, 16, TF_UNKNOW, &m_ImeImage.GetImage(0).pTexture );

		ICONINFO Info; 
		GetIconInfo( hIcon, &Info );
		m_ImeImage.GetImage(0).pTexture->FillFromHBitmap( Info.hbmColor, Info.hbmMask );
		::DestroyIcon( hIcon );
	}

	OnImeChanged();
}

LRESULT CIme::ImeProc( uint32 message, WPARAM wParam, LPARAM lParam )
{    
	HKL hCurKL = GetKeyboardLayout( 0 );
	if( m_hCurKL == hCurKL )
		return DefWindowProcW( m_hWnd, message, wParam, lParam );
	m_hCurKL = hCurKL;

	SetImeIcon();
	ImmAssociateContext( m_hWnd, m_hImc );
	LRESULT re = DefWindowProcW( m_hWnd, message, wParam, lParam );
	if( !m_bEnableIme )
		ImmAssociateContext( m_hWnd, NULL );

	m_uCharBuf = 0;
	return re;
}

const WND_IMAGE_LIST* CIme::GetGuiImeImage()
{ 
	return m_ImeImage.GetImageCount() ? &m_ImeImage : NULL; 
};