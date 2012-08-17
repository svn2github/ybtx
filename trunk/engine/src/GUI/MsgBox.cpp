#include "stdafx.h"
#include "MsgBox.h"
#include "SQRGUIManager.h"
#include "SQRDialog.h"
#include "CScript.h"
#include "GUISize.h"
#include "CodeCvs.h"
#include "TSqrAllocator.inl"
#include "BaseHelper.h"


// ----------------------------------------------------------------------------------------
// MessageBox
struct CHILD_INFO
{
	UIWString szText;
	UIWString szInfo;
	UIWString szBtn[2];
};

class CMessageInfo : public SQRRichText
{
	
public:
	CMessageInfo();
	~CMessageInfo();
	CFRect					m_DrawRect;
	//画窗口文本
	virtual void			DrawWndText();
	//设置文本
	Size					SetInfo( const UIWString& str, const TCHAR* sImageStr );
	void					SetImageInfo(const TCHAR* sImageStr);
	void					SetAutoResize(bool au);
private:
	bool					m_beAutoResize;
};

CMessageInfo::CMessageInfo()
: m_beAutoResize(false)
{
}

CMessageInfo::~CMessageInfo()
{
}

void CMessageInfo::SetAutoResize(bool au)
{
	m_beAutoResize = au;
}

void CMessageInfo::SetImageInfo(const TCHAR* sImageStr)
{
	UIString str = sImageStr;
	size_t strLength  = str.size();
	bool bInsertIndexStr  = false;
	UIString tempIndexStr = "";
	UIString tempImageStr = "";
	bool bInsertImageStr = false;
	for (uint i=0; i< strLength; i++)
	{
		if ( str[i] == '#')
		{
			bInsertIndexStr = true;
			if (i>0)
			{
				WND_IMAGE_LIST image;
				int32 nImageIndex=  atoi(tempIndexStr.c_str());
				image.AddImage( GetGraphic(), -1,  tempImageStr.c_str(), NULL, CFPos(0.0f, 0.0f), 0xffffffff, 0 );
				InsertImage(nImageIndex, image);
				bInsertImageStr = false;
				tempImageStr = "";
				tempIndexStr = "";
			}
			i++;
		}
		if ( str[i] == ':')
		{
			bInsertIndexStr = false;
			bInsertImageStr = true;
			i++;
		}
		if ( bInsertIndexStr )
			tempIndexStr += str[i];

		if (bInsertImageStr)
		{
			tempImageStr += str[i];
		}	
	}
	WND_IMAGE_LIST image;
	int32 nImageIndex=  atoi(tempIndexStr.c_str());
	image.AddImage( GetGraphic(), -1,  tempImageStr.c_str(), NULL, CFPos(0.0f, 0.0f), 0xffffffff, 0 );
	InsertImage(nImageIndex, image);
}

Size CMessageInfo::SetInfo( const UIWString& str, const TCHAR* sImageStr )
{
	UIWString szInfo = str;

	Size tSize(0, 0);
	
	UIString utf8_str = utf16_to_utf8( szInfo.c_str() ).c_str();
	SetImageInfo(sImageStr);
	SetWndText(utf8_str.c_str());

	if (m_beAutoResize)
	{
		// 重置窗体大小
		CFRect rt;
		CFRect rtRoot;
		GetWndRect( rt );
		GetRootWnd()->GetWndRect( rtRoot );
		float curWidth	= GetRichStringInfo()->GetMaxWidth() + GetXBorder( GetStyle() )*2;
		float curHeight  = GetRichStringInfo()->GetCurHeight() + GetYBorder( GetStyle() )*2;


		if( curWidth > rt.right - rt.left )
		{
			tSize.width = curWidth - rt.right + rt.left;
			m_DrawRect.left = -tSize.width/2;
			m_DrawRect.right = m_DrawRect.left + curWidth;
		}
		else
		{
			m_DrawRect.left = ( rt.right - rt.left - curWidth )/2;
			m_DrawRect.right = m_DrawRect.left + curWidth;
		}

		if( curHeight > rt.bottom - rt.top )
		{
			tSize.height = curHeight - rt.bottom + rt.top;
			m_DrawRect.top = -tSize.height/2 ;
			m_DrawRect.bottom =  m_DrawRect.top + curHeight;
		}
		else
		{
			m_DrawRect.top = ( rt.bottom - rt.top - curHeight )/2;
			m_DrawRect.bottom = m_DrawRect.top + curHeight;
		}

		WndToScreen(m_DrawRect);
		SetWndRect(m_DrawRect);

		SetWndText(utf8_str.c_str());
	}

	return tSize;
}

void CMessageInfo::DrawWndText()
{
	CFRect rt;
	GetWndRect(rt);
	ScreenToWnd(rt);
	SQRRichText::DrawWndText();
	//CColor cl = IsEnable() ? m_hWnd->m_EnableColor : m_hWnd->m_DisableColor;
	//CColor cB = IsEnable() ? m_hWnd->m_EnableBcColor : m_hWnd->m_DisableBcColor;
	//SQRWnd::DrawText( GetWndTextW(), 0, 0, cl, cB, true, &rt );
}

class SQRMessageWnd : public SQRDialog
{
	DYNAMIC_DECLARE_WND( SQRMessageWnd )
public:
	SQRMessageWnd(){}
	SQRMessageWnd( uint32 nStyle, SQRHolderHandler* pHolder, void* pParams );
	~SQRMessageWnd();

	//窗口创建
	int32		Create( const WndCreateParam& param );
	//初始化子窗口
	void		InitChildWnd( const CHILD_INFO& ChildInfo, const TCHAR* sImageStr );
	//响应子窗口
	void		OnCtrlmsg( SQRWnd* pChild, uint32 uMsgID, uint32 uParam1, uint32 uParam2 );
	//
	bool		IsInWndArea( float x, float y  );
	//键盘按下消息
	void		OnKeyDown( uint32 nChar, uint32 nFlags );
	//左键点击消息
	void		OnLButtonDown( uint32 nFlags, int32 x, int32 y );
	void		SetAutoResize(bool au);

private:
	enum EMaxBtnNum{eMaxBtnNum=2};
	uint32          m_nStyle;
	CMessageInfo	m_WndInfo;
	SQRWnd*			m_pBtn[eMaxBtnNum];
	SQRHolderHandler*	m_pHolder;
	void*			m_pParams;
};

DYNAMIC_CREATE_WND( SQRMessageWnd, SQRWnd );

namespace sqr
{
	
	void RegistMsgBox( 
		uint32			nStyle, 
		SQRWnd*			pRoot, 
		const char*		szResName,
		const char*		szInfo,
		const char*		szBtnOK,
		const char*		szBtnCnl
		)
	{
		if( !pRoot || !pRoot->IsCreated() )
			return;
		SQRGUIManager* pGUI = SQRGUIManager::GetGUI( pRoot );
		UIVector<UIWString>& Res = pGUI->GetMsgResMap()[nStyle];

		Res.clear();
		wstring utf16_str;
		Res.push_back( szResName ? utf8_to_utf16(szResName).c_str()	: L"");
		Res.push_back( szInfo	 ? utf8_to_utf16(szInfo).c_str()	: L"" );
		Res.push_back( szBtnOK   ? utf8_to_utf16(szBtnOK).c_str()	: L"" );
		Res.push_back( szBtnCnl  ? utf8_to_utf16(szBtnCnl).c_str()	: L"" );
	}

	SQRWnd* MsgBox( 
		uint32			nStyle, 
		SQRHolderHandler*	pHolder,
		SQRWnd*			pParent, 
		const char*	sText, 
		uint32			dwBtnStyle,
		void*			pParam,
		const char*		sImageStr,
		bool			autoresize
		)
	{
		if( !pParent || !pParent->IsCreated() )
			return NULL;
		
		SQRGUIManager* pGUI = SQRGUIManager::GetGUI( pParent );
		MsgResMap::iterator it = pGUI->GetMsgResMap().find( nStyle );
		if( it == pGUI->GetMsgResMap().end() )
			return NULL;

		wstring utf16_str;
		utf8_to_utf16(utf16_str, sText);
		CHILD_INFO Info = {	utf16_str.c_str(), it->second[1], it->second[2], it->second[3] };

		SQRMessageWnd* pMsgWnd = new SQRMessageWnd( dwBtnStyle, pHolder, pParam );
		pMsgWnd->SetAutoResize(autoresize);

		UIString utf8_str = utf16_to_utf8( it->second[0].c_str() ).c_str();
		pMsgWnd->SQRDialog::CreateFromRes( utf8_str.c_str(), pGUI->GetRootWnd() );
		pMsgWnd->InitChildWnd( Info , sImageStr);
		pMsgWnd->GetWndHandle()->m_bNewByGUISys = true;

		pMsgWnd->SetParent( pParent );
		pMsgWnd->ShowWnd( true );
		return pMsgWnd;
	}

	void CloseAllMsgBox( SQRWnd* pWnd )
	{
		if( !pWnd || !pWnd->IsCreated() )
			return;
		SQRGUIManager* pGUI = SQRGUIManager::GetGUI( pWnd );

		SQRWnd *pModule=pGUI->GetModuleWnd();
		SQRWnd **ppWnd=&pModule;

		while( *ppWnd )
		{
			if( (*ppWnd)->GetClassName() == SQRMessageWnd::classSQRMessageWnd.m_ClassName )
			{
				SQRWnd* pNext = *ppWnd;
				*ppWnd = pNext->GetWndHandle()->m_pNextModule;
				delete pNext;
			}
			else
			{
				ppWnd = &((*ppWnd)->GetWndHandle()->m_pNextModule);
			}
		}
	}

}
//========================================
// CGameMsgWnd
//========================================
SQRMessageWnd::SQRMessageWnd( uint32 nStyle, SQRHolderHandler*	pHolder, void * pParams )
{
	m_nStyle		= nStyle;
	m_pHolder		= pHolder;
	m_pParams		= pParams;
	StyleSQRMessageWnd = SQRDialog::StyleSQRDialog;
}

SQRMessageWnd::~SQRMessageWnd()
{
	if( m_pHolder )
	{
		m_pHolder->CallBack( MB_BtnCancel, m_pParams );
		m_pHolder->Release();
	}
}

void SQRMessageWnd::SetAutoResize(bool au)
{
	m_WndInfo.SetAutoResize(au);
}

int32 SQRMessageWnd::Create( const WndCreateParam& param )
{
	WndCreateParam newParam = param;
	newParam.uStyle |= WS_MODULE;
	return SQRWnd::Create( newParam );
}

//初始化子窗口
void SQRMessageWnd::InitChildWnd( const CHILD_INFO& ChildInfo, const TCHAR* sImageStr )
{
	AssignDlgChildToWnd( m_WndInfo, ChildInfo.szInfo.c_str() );
	Size tSize = m_WndInfo.SetInfo( ChildInfo.szText , sImageStr);

	CFRect rt;
	GetWndRect( rt );
	rt.left		-= tSize.width/2;
	rt.right	+= tSize.width/2;
	rt.top		-= tSize.height/2;
	rt.bottom	+= tSize.height/2;
	SetWndRect( rt );

	static uint32 nMask[2] = { MB_BtnOK,  MB_BtnCancel };
	CFRect Scale( FLOAT_MAX_VALUE, FLOAT_MAX_VALUE, FLOAT_MIN_VALUE, FLOAT_MIN_VALUE );

	int32 n = 0;
	SQRWnd* pBtn[4];
	for( int32 i = 0; i < eMaxBtnNum; i++ )
	{
		m_pBtn[i] = GetDlgChild( utf16_to_utf8(ChildInfo.szBtn[i].c_str()).c_str() );

		if( m_pBtn[i] )
		{
			CFRect rc;
			m_pBtn[i]->GetWndRect( rc );
			float xCen = ( rc.left + rc.right )/2;
			float yCen = ( rc.top + rc.bottom )/2;

			Scale.left		= min( Scale.left,   xCen );
			Scale.top		= min( Scale.top,    yCen );
			Scale.right		= max( Scale.right,  xCen );
			Scale.bottom	= max( Scale.bottom, yCen );

			if( nMask[i]&m_nStyle )
			{
				pBtn[n] = m_pBtn[i];
				n++;
				continue;
			}
			m_pBtn[i]->ShowWnd( false );
			m_pBtn[i] = NULL;
		}
	}

	float nWidth   = Scale.right - Scale.left;
	Scale.left   = ( rt.right + rt.left )/2 - nWidth/2;
	Scale.right  = ( rt.right + rt.left )/2 + nWidth/2;;
	for( int32 i = 0; i < n; i++ )
	{
		float x = Scale.left + ( ( n == 1 ) ? ( Scale.right - Scale.left )/2 : ( Scale.right - Scale.left )*i/( n - 1 ) );	
		float y = Scale.top + ( ( n == 1 ) ? ( Scale.bottom - Scale.top )/2 : ( Scale.bottom - Scale.top )*i/( n - 1 ) );	
		float w = pBtn[i]->GetWndWidth();
		float h = pBtn[i]->GetWndHeight();
		CFRect rt(x - w/2, y - h/2, x + w/2, y + h/2);
		pBtn[i]->SetWndRect( rt );
	}
}

void SQRMessageWnd::OnCtrlmsg( SQRWnd* pChild, uint32 uMsgID, uint32 uParam1, uint32 uParam2 )
{
	if( uMsgID != BUTTON_LCLICK )
		return;

	static int32  nRet[2] = { MB_BtnOK,  MB_BtnCancel };

	for( int32 i = 0; i < eMaxBtnNum; i++ )
	{
		if( m_pBtn[i] && m_pBtn[i] == pChild )
		{
			if( !m_pHolder || m_pHolder->CallBack( nRet[i],m_pParams ) )
			{
				SafeRelease( m_pHolder );
				delete this;
			}
			return;
		}
	}
}

bool SQRMessageWnd::IsInWndArea( float x, float y )
{
	return true;
}

//键盘按下消息
void SQRMessageWnd::OnKeyDown( uint32 nChar, uint32 nFlags )
{
	bool bRet = false;

	if( m_pHolder )
	{
		if( nChar == VK_RETURN )
		{
			if( MB_BtnNone == m_nStyle )
				bRet = m_pHolder->CallBack( MB_BtnNone,m_pParams );
			if( MB_BtnOK&m_nStyle )
				bRet = m_pHolder->CallBack( MB_BtnOK,m_pParams );
		}
		else if( nChar == VK_ESCAPE )
		{
			if( MB_BtnNone == m_nStyle )
				bRet = m_pHolder->CallBack( MB_BtnNone,m_pParams );
			if( MB_BtnCancel&m_nStyle )
				bRet = m_pHolder->CallBack( MB_BtnCancel,m_pParams );
		}
	}
	else
		bRet = true;

	if( bRet )
	{
		SafeRelease( m_pHolder );
		delete this;
	}
}

//左键点击消息
void SQRMessageWnd::OnLButtonDown( uint32 nFlags, int32 x, int32 y )
{
	if( MB_BtnNone == m_nStyle )
	{
		if( x < 0 || x > GetWndWidth() || y < 0 || y > GetWndHeight() )
		{
			if( !m_pHolder || m_pHolder->CallBack( MB_BtnNone,m_pParams ) )
			{
				SafeRelease( m_pHolder );
				delete this;
			}
		}
	}
}

SQRMsgBoxHandler::~SQRMsgBoxHandler()
{
	if(m_pHolder)m_pHolder->SetHandler(NULL);
}

void SQRMsgBoxHandler::SetHandler(SQRMsgBoxHandlerHolder* pHolder)
{
	m_pHolder=pHolder;
}

SQRMsgBoxHandlerHolder::SQRMsgBoxHandlerHolder( SQRMsgBoxHandler* pHandler )
: m_pHandler( pHandler )
{
	AdvAddRef( m_pHandler );
	m_pHandler->SetHandler(this);
}

SQRMsgBoxHandlerHolder::~SQRMsgBoxHandlerHolder()
{
	if(m_pHandler)m_pHandler->SetHandler(NULL);
}
bool SQRMsgBoxHandlerHolder::CallBack( int nIndex, void* pParam )
{
	return m_pHandler?m_pHandler->CallBack( nIndex, pParam ):false;
}

void SQRMsgBoxHandlerHolder::Release()
{
	AdvDelRef( m_pHandler );
	delete this;
}

void SQRMsgBoxHandlerHolder::SetHandler(SQRMsgBoxHandler* pHandler)
{
	m_pHandler=pHandler;
}
