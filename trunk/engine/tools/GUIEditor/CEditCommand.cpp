#include "stdafx.h"
#include ".\ceditcommand.h"
#include "CWndGroup.h"
#include "CodeCvs.h"


void CEditCommand::WriteCurEditToString( SQRWnd* pWnd, string& str )
{
	struct StrFile
	{
		string&		  m_Str;
		StrFile( string& str ) : m_Str( str ){}
		void write( char* buf, size_t size )
		{
			for( size_t i = 0; i < size; i++ )
				m_Str.push_back( buf[i] );
		}
	};

	StrFile File(str);
	if( pWnd == NULL )
		return;
	CFRect rt;
	pWnd->GetWndRect( rt );

	int		ClassLen		= (int)strlen( pWnd->GetClassName() );
	LPCSTR  szClass			= pWnd->GetClassName();
	int		WndTextLen		= (int)strlen( pWnd->GetWndText() );
	LPCSTR	szWndText		= pWnd->GetWndText();
	DWORD	wsStyle			= pWnd->GetStyle();
	int		x				= int(rt.left);
	int		y				= int(rt.top);
	int		w				= int(rt.Width());
	int		h				= int(rt.Height());
	DWORD	GroupID			= pWnd->GetGroupID();

	int32	nFontID			= pWnd->GetFontID();
	int		nFontSize		= int(pWnd->GetFontSize());
	int32	bBold			= pWnd->IsFontBold();

	DWORD		fontGradualColor= pWnd->GetFontGradualColor();
	DWORD		fontEffectMask	= pWnd->GetFontEffectMask();

	unsigned	uLockFlag       = pWnd->GetLockFlag();
	int			nDescLen		= (int)strlen( pWnd->GetMouseOverDesc() );
	const char*	szDescInfo		= pWnd->GetMouseOverDesc();
	int			nUseClassLen	= (int)m_pWndGroup->m_CreateInfo[pWnd].szClassName.size();
	const char*	szUseClass		= utf16_to_utf8(m_pWndGroup->m_CreateInfo[pWnd].szClassName).c_str();
	int			nUseInfLen		= (int)m_pWndGroup->m_CreateInfo[pWnd].szCreateInfo.size();
	const char*	szUseInf		= utf16_to_utf8(m_pWndGroup->m_CreateInfo[pWnd].szCreateInfo).c_str();
	int			nClickFxLen		= (int)strlen( pWnd->GetMsgFx(eMF_Click) );
	const char*	szClickFxInfo	= pWnd->GetMsgFx(eMF_Click);
	int			nOpenFxLen		= (int)strlen( pWnd->GetMsgFx(eMF_Open) );
	const char*	szOpenFxInfo	= pWnd->GetMsgFx(eMF_Open);
	int			nCloseFxLen		= (int)strlen( pWnd->GetMsgFx(eMF_Close) );
	const char*	szCloseFxInfo	= pWnd->GetMsgFx(eMF_Close);

	File.write( (char*)&ClassLen, sizeof(int) );
	File.write( (char*)szClass, ClassLen );
	File.write( (char*)&WndTextLen, sizeof(int) );
	File.write( (char*)szWndText, WndTextLen );
	File.write( (char*)&wsStyle, sizeof(DWORD) );
	File.write( (char*)&x, sizeof(int) );
	File.write( (char*)&y, sizeof(int) );
	File.write( (char*)&w, sizeof(int) );
	File.write( (char*)&h, sizeof(int) );
	File.write( (char*)&GroupID, sizeof(DWORD) );
	
	File.write( (char*)&nFontID, sizeof(int32) );
	File.write( (char*)&bBold, sizeof(bool) );
	File.write( (char*)&nFontSize, sizeof(int) );

	File.write(	(char*)&fontGradualColor, sizeof(DWORD));		 
	File.write(	(char*)&fontEffectMask, sizeof(DWORD));

	File.write( (char*)&uLockFlag, sizeof(unsigned) );
	File.write( (char*)&nDescLen, sizeof(int) );
	File.write( (char*)szDescInfo, nDescLen );
	File.write( (char*)&nUseClassLen, sizeof(int) );
	File.write( (char*)szUseClass, nUseClassLen );
	File.write( (char*)&nUseInfLen, sizeof(int) );
	File.write( (char*)szUseInf, nUseInfLen );
	File.write( (char*)&nClickFxLen, sizeof(int) );
	File.write( (char*)szClickFxInfo, nClickFxLen );
	File.write( (char*)&nOpenFxLen, sizeof(int) );
	File.write( (char*)szOpenFxInfo, nOpenFxLen );
	File.write( (char*)&nCloseFxLen, sizeof(int) );
	File.write( (char*)szCloseFxInfo, nCloseFxLen );

	char buf[1024];
	memcpy( buf, str.c_str(), str.size() );

	int nChildCount = 0;
	for( int nCtrlMask = 1; ; nCtrlMask++ )
	{
		SQRWnd* pChild = pWnd->GetResWnd( nCtrlMask );
		if( !pChild )
			break;
		nChildCount++;
	}

	File.write( (char*)&nChildCount, sizeof(int) );
	for( int nCtrlMask = 1; ; nCtrlMask++ )
	{
		SQRWnd* pChild = pWnd->GetResWnd( nCtrlMask );
		if( !pChild )
			break;
		if( nCtrlMask > 1 )
		{
			CFRect rt;
			pChild->GetWndRect( rt );
			int		x          = int(rt.left);
			int		y          = int(rt.top);
			int		w          = int(rt.Width());
			int		h          = int(rt.Height());
			File.write( (char*)&x, sizeof(int) );
			File.write( (char*)&y, sizeof(int) );
			File.write( (char*)&w, sizeof(int) );
			File.write( (char*)&h, sizeof(int) );
		}

		int nStateCount = 0;
		IMAGE_PARAM IP ( 1, 0 );
		for( IP.StateMask = IP_MASK; IP.StateMask < IP_COUNT; IP.StateMask++ )
			if( pChild->GetWndBkImage( IP ) )
				nStateCount++;

		File.write( (char*)&nStateCount, sizeof(int) );
		for( IP.StateMask = IP_MASK; IP.StateMask < IP_COUNT; IP.StateMask++ )
		{
			WND_IMAGE_LIST* pWI = pChild->GetWndBkImage( IP );

			if( pWI )
			{
				int nImageCount = pWI->GetImageCount();
				unsigned uStateColor = pChild->GetWndTextColor( IP );
				unsigned uRimColor = pChild->GetWndTextBcColor( IP );
				int nStateMask = IP.StateMask;
				File.write( (char*)&nStateMask, sizeof(int) );
				File.write( (char*)&uStateColor, sizeof(unsigned) );
				File.write(	(char*)&uRimColor, sizeof(unsigned));     // Ð´ÈëÎÄ×Ö±ß¿òÑÕÉ«
				File.write( (char*)&nImageCount, sizeof(int) );

				for( int i = 0; i < nImageCount; i++ )
				{
					IMAGE& Image = pWI->GetImage(i);
					string	TextName  = Image.pTexture ? Image.pTexture->GetName() : "";
					int		Len = (int)TextName.size();
					File.write( (char*)&Len, sizeof(int) );
					File.write( (char*)TextName.c_str(), Len );
					File.write( (char*)&Image.rtTexture, sizeof( CFRect ) );
					File.write( (char*)&Image.posWnd, sizeof( CFPos ) );
					File.write( (char*)&Image.dwColor, sizeof( unsigned ) );
					File.write( (char*)&Image.dwLockFlag, sizeof( unsigned ) );
				}
			}
		}
	}
}


void CEditCommand::ReadFromString( SQRWnd* pWnd, const string& str )
{
	struct StrFile
	{
		size_t        m_nPos;
		const string& m_Str;
		StrFile( const string& str ) : m_Str( str ), m_nPos(0){}
		void read( char* buf, size_t size )
		{
			for( size_t i = m_nPos; i < m_nPos + size; i++ )
				buf[i - m_nPos] = m_Str[i];
			m_nPos += size;
		}
	};

	int S = (int)str.size();
	StrFile File(str);

	char buf[1024];
	memcpy( buf, str.c_str(), min( str.size(), 1024 ) );

	int		ClassLen;
	char	szClass[256];
	int		WndTextLen;
	char	szWndText[256];
	DWORD	wsStyle;
	int		x;
	int		y;
	int		w;
	int		h;
	DWORD	GroupID;
	
	int32	nFontID;
	bool	bBold;
	int		nFontSize;
	
	unsigned  uLockFlag;
	int     nDescLen;
	char    szDescInfo[256];
	int     nUseClassLen;
	char	szUseClass[256];
	int     nUseInfLen;
	char	szUseInf[256];
	int     nClickFxLen;
	char	szClickFxInfo[256];
	int     nOpenFxLen;
	char	szOpenFxInfo[256];
	int     nCloseFxLen;
	char	szCloseFxInfo[256];
	DWORD	uFontGradualColor;
	DWORD	uFontEffectMask;

	memset( szClass, 0, 256 );
	memset( szWndText, 0, 256 );
	memset( szDescInfo, 0, 256 );
	memset( szUseClass, 0, 256 );
	memset( szUseInf, 0, 256 );
	memset( szClickFxInfo, 0, 256 );
	memset( szOpenFxInfo, 0, 256 );
	memset( szCloseFxInfo, 0, 256 );
	File.read( (char*)&ClassLen, sizeof(int) );
	File.read( (char*)szClass, ClassLen );
	File.read( (char*)&WndTextLen, sizeof(int) );
	File.read( (char*)szWndText, WndTextLen );
	File.read( (char*)&wsStyle, sizeof(DWORD) );
	File.read( (char*)&x, sizeof(int) );
	File.read( (char*)&y, sizeof(int) );
	File.read( (char*)&w, sizeof(int) );
	File.read( (char*)&h, sizeof(int) );
	File.read( (char*)&GroupID, sizeof(DWORD) );
	
	File.read( (char*)&nFontID, sizeof(int32) );
	File.read( (char*)&bBold, sizeof(bool) );
	File.read( (char*)&nFontSize, sizeof(int) );

	File.read( (char*)&uFontGradualColor, sizeof(float));
	File.read( (char*)&uFontEffectMask, sizeof(float));

	File.read( (char*)&uLockFlag, sizeof(unsigned) );
	File.read( (char*)&nDescLen, sizeof(int) );
	File.read( (char*)szDescInfo, nDescLen );
	File.read( (char*)&nUseClassLen, sizeof(int) );
	File.read( (char*)szUseClass, nUseClassLen );
	File.read( (char*)&nUseInfLen, sizeof(int) );
	File.read( (char*)szUseInf, nUseInfLen );
	File.read( (char*)&nClickFxLen, sizeof(int) );
	File.read( (char*)szClickFxInfo, nClickFxLen );
	File.read( (char*)&nOpenFxLen, sizeof(int) );
	File.read( (char*)szOpenFxInfo, nOpenFxLen );
	File.read( (char*)&nCloseFxLen, sizeof(int) );
	File.read( (char*)szCloseFxInfo, nCloseFxLen );

	RECT rt_tmp = { x, y, x + w, y + h };
	CFRect rt(rt_tmp);
	pWnd->SetWndText( szWndText );
	pWnd->SetStyle( wsStyle );
	pWnd->SetWndRect( rt );
	pWnd->SetFont( nFontID, bBold, (float)nFontSize );
	pWnd->SetGroupID( GroupID );
	pWnd->SetLockFlag( uLockFlag );
	pWnd->SetMouseOverDesc( szDescInfo );
	pWnd->SetMsgFx( eMF_Click, szClickFxInfo );
	pWnd->SetMsgFx( eMF_Open,  szOpenFxInfo );
	pWnd->SetMsgFx( eMF_Close, szCloseFxInfo );
	pWnd->SetFontGradualColor(uFontGradualColor);
	pWnd->SetFontEffectMask(uFontEffectMask);

	m_pWndGroup->m_CreateInfo[pWnd].szClassName = utf8_to_utf16(szUseClass);
	m_pWndGroup->m_CreateInfo[pWnd].szCreateInfo = utf8_to_utf16(szUseInf);

	int nChildCount = 0;
	File.read( (char*)&nChildCount, sizeof(int) );
	for( int nCtrlMask = 1; ; nCtrlMask++ )
	{
		SQRWnd* pChild = pWnd->GetResWnd( nCtrlMask );
		if( !pChild )
			break;
		if( nCtrlMask > 1 )
		{
			int x,y,w,h;
			File.read( (char*)&x, sizeof(int) );
			File.read( (char*)&y, sizeof(int) );
			File.read( (char*)&w, sizeof(int) );
			File.read( (char*)&h, sizeof(int) );
			RECT rt_tmp = { x, y, x + w, y + h };
			CRect rt(rt_tmp);
			pChild->SetWndRect( rt );
		}

		int nStateCount = 0;
		File.read( (char*)&nStateCount, sizeof(int) );
		for( int nState = 0; nState < nStateCount; nState++ )
		{
			int nImageCount;
			unsigned uStateColor;
			unsigned uRimColor;
			int nStateMask;
			File.read( (char*)&nStateMask, sizeof(int) );
			File.read( (char*)&uStateColor, sizeof(unsigned) );
			File.read( (char*)&uRimColor, sizeof(unsigned) );
			File.read( (char*)&nImageCount, sizeof(int) );

			IMAGE_PARAM IP ( 1, nStateMask );
			WND_IMAGE_LIST* pWI = pChild->GetWndBkImage( IP );
			pChild->SetWndTextColor( IP, uStateColor );
			pChild->SetWndTextBcColor( IP, uRimColor );
			pWI->Release();

			for( int i = 0; i < nImageCount; i++ )
			{
				char TextName[256];
				memset( TextName, 0, 256 );
				int  Len;
				CFRect rt;
				CFPos  Pos;
				unsigned dwColor;
				unsigned dwLockFlag;
				File.read( (char*)&Len, sizeof(int) );
				File.read( TextName, Len );
				File.read( (char*)&rt, sizeof( CFRect ) );
				File.read( (char*)&Pos, sizeof( CFPos ) );
				File.read( (char*)&dwColor, sizeof( unsigned ) );
				File.read( (char*)&dwLockFlag, sizeof( unsigned ) );
				pWI->AddImage( pWnd->GetGraphic(), pWI->GetImageCount(), TextName, &rt, Pos, dwColor, dwLockFlag );

			}
		}
	}
}

CEditCommand::CEditCommand(void)
{
	m_it = m_CommandList.end();
}

CEditCommand::~CEditCommand(void)
{
}

void CEditCommand::Redo()
{
	if( m_it != m_CommandList.end() )
	{
		CommandList& Cur = (*m_it);
		ReadFromString( Cur.pEditWnd, Cur.NewValue );	
		m_it++;
		//m_pWndGroup->UpdateToPropWnd();
	}
}

void CEditCommand::UnDo()
{
	if( m_it != m_CommandList.begin() )
	{
		m_it--;
		CommandList& Cur = (*m_it);
		ReadFromString( Cur.pEditWnd, Cur.NewValue );	
		//m_pWndGroup->UpdateToPropWnd();
	}
	else
	{
		CommandList& Cur = (*m_it);
		ReadFromString( Cur.pEditWnd, Cur.OldValue );	
		//m_pWndGroup->UpdateToPropWnd();
	}
}
