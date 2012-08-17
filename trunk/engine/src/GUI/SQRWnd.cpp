#include "stdafx.h"
#include "MathAlgorithm.h"
#include "SQRWnd.h"
#include "CTexture.h"
#include "SQRGUIManager.h"
#include "SQRRootWnd.h"
#include "BaseHelper.h"
#ifdef _DEBUG
#include "ExpHelper.h"
#endif
#include "CPkgFile.h"
#include "CodeCvs.h"
#include "IAudioModule.h"
#include "SQRToolTip.h"
#include "CodeCvs.h"
#include "TSqrAllocator.inl"
#include "CFontMgr.h"
#include "CTextureFontMgr.h"
#include "SQRRichToolTip.h"
#include "CGraphic.h"
#include "SQRDialog.h"
#include "FlashDataManager.h"
#include "SQRGUIEffectHandler.h"
#include "CColor.inl"
#include "CFontCfg.h"

#define MAX_SCROLL_SIZE 65536.0f

DYNAMIC_CREATE_WNDBASE( SQRWnd )//创建SQRWnd对象、返回
//////////////////////////////////////////////////////////////////////////
SQRWnd::SQRWnd(void)
		: bIsBottom(false)
		, bDetectMouseOverWnd(false)
		, m_itX(0)
		, m_itY(0)
		, m_WndZoom(1.0f)
		, m_bAlignToPixel(true)
		, m_FlushDlg(NULL)
		, m_ESMask(IP_COUNT)
		, bNotModule(true)
		, m_effHld(NULL)
		, m_bInShape(false)
		, m_pCutWnd(NULL)
		, m_beBGDrawed(false)
		, m_bCutFlash(false)
		, m_bIgnTex(false)
{
	m_hWnd		 = new WndHandle;
	offset[0]	 = 0.0f;
	offset[1]	 = 0.0f;
	offset[2]	 = 0.0f;
	offset[3]	 = 0.0f;
	offset[4]	 = 0.0f;
	offset[5]	 = 0.0f;
	offset[6]	 = -1.0f;
	m_Effect.clear();
}

SQRWnd::~SQRWnd(void)
{
	DestroyWnd();
	delete m_hWnd;
}

void SQRWnd::OnProcess()
{
	if ( m_effHld )
	{
		// 处理所有GUI效果
		// 包括渐隐渐显，形变
		m_effHld->ExecuteEffects();
	}
}

void SQRWnd::EndTransparent( bool bvalue )
{
	if ( m_effHld )
	{
		SQRGUIEffectBase* temp_eff = m_effHld->GetEffect(eEff_Transparent);
		if (temp_eff && temp_eff->GetEffectType() == eEff_Transparent)
		{
			SQRGUITransparentEffect* T_eff = static_cast<SQRGUITransparentEffect*>(temp_eff);
			T_eff->EndTransparent(bvalue);
		}
	}
}

void SQRWnd::SetRotationOrigin( FLOAT offsetx, FLOAT offsety )
{
}

void SQRWnd::SetTransparentObj(int32 runtime, bool bvalue)
{
	if ( !m_effHld )
		m_effHld = new SQRGUIEffectHandler();
	SQRGUITransparentEffect* T_eff = NULL;
	SQRGUIEffectBase* temp_eff = m_effHld->GetEffect(eEff_Transparent);
	if ( !temp_eff )
	{
		T_eff = new SQRGUITransparentEffect(this);
		T_eff->SetTransparentObj(runtime, bvalue);
		m_effHld->AddGUIEffect(this, T_eff);
	}
	else if (temp_eff->GetEffectType() == eEff_Transparent)
	{
		T_eff = static_cast<SQRGUITransparentEffect*>(temp_eff);
		T_eff->SetTransparentObj(runtime, bvalue);
	}
}

void SQRWnd::OnMouseWheel( uint32 nFlags, short zDelta, int32 x, int32 y )
{
	CALL_CLASS_CALLBACK_4(nFlags, zDelta, x, y)

	if ( GetParent() && (GetStyle() & WS_IGNOREWHEEL) )
			GetParent()->OnMouseWheel(nFlags, zDelta, x, y);
}

////////////////////////////////////////////////////////////////////////
// 删除关联flash
void SQRWnd::DestroyFlashAtt()
{
	if (m_FlushDlg)
		if(m_FlushDlg->GetFlashRenderTask())
			m_FlushDlg->GetFlashRenderTask()->DelFlashTaskAll(this);
}

SQRDialog* SQRWnd::GetFlushDialog()
{
	SQRWnd* parentwnd = this;
	while( parentwnd && !(parentwnd->IsFlushDialog()) )
		parentwnd = parentwnd->m_hWnd->m_ParentWnd;
	if ( !parentwnd )
		return NULL;
	return static_cast<SQRDialog*>(parentwnd);
}

bool SQRWnd::IsFlushDialog()
{
	return false;
}

// 此函数只给最后一个接收信息的窗体调用，只需要检测当前窗体的状态，其他不需要关心
void SQRWnd::AddFlashInfoShow()
{
	if ( m_FlushDlg == NULL )
	{
		m_FlushDlg = GetFlushDialog();
		if ( m_FlushDlg == NULL ) return;
	}
	// 判断当前窗体的状态，选择更新指示信息状态
	EventStateMask fs = GetEventStateMask();
	if (m_hWnd->m_FlashInfo.find(fs) == m_hWnd->m_FlashInfo.end())
		AddFlashInfoShow(NULL, fs);
	else
	{
		FlashWndInfo* info = m_hWnd->m_FlashInfo[fs]; 
		AddFlashInfoShow(info, fs);
	}
}

void SQRWnd::AddFlashInfoShow(FlashWndInfo* info, uint32 fs)
{
	if ( m_FlushDlg == NULL )
	{
		m_FlushDlg = GetFlushDialog();
		if ( m_FlushDlg == NULL ) return;
	}

	if (fs >= IP_COUNT)
		m_FlushDlg->GetFlashRenderTask(true)->AddFlashTask(info, this, (uint32)0);
	else
		m_FlushDlg->GetFlashRenderTask(true)->AddFlashTask(info, this, (uint32)1);
}

void SQRWnd::AddFlashInfoByName(const char* info)
{
	DelFlashAttention();
	AddFlashInfoByNameImp(info);
}

void SQRWnd::AddFlashInfoByNameImp(const char* info)
{
	FlashWndInfo* finfo = FlashDataManager::Inst()->GetFlashInfo(info);
	if (!finfo)
		return;
	if ( m_FlushDlg == NULL )
	{
		m_FlushDlg = GetFlushDialog();
		if ( m_FlushDlg == NULL ) return;
	}
	m_FlushDlg->GetFlashRenderTask(true)->AddFlashTask(finfo, this, (uint32)0);
}

void SQRWnd::AddFlashInfoByNameEx(const char* info, bool cut)
{
	m_bCutFlash = cut;
	AddFlashInfoByNameImp(info);
}

void SQRWnd::AddFlashAttentionTex( char* flashTex, const CFRect& rt, SQRWnd* pChildWnd, uint32 ct, float v, 
								float lx, float ty ,float rx, float by)
{
	if ( m_FlushDlg == NULL )
	{
		m_FlushDlg = GetFlushDialog();
		if ( m_FlushDlg == NULL ) return;
	}
	m_FlushDlg->GetFlashRenderTask(true)->AddFlashTask(flashTex, rt, pChildWnd, ct, v, lx, ty , rx, by);
}

void SQRWnd::DelFlashAttention()
{
	if ( m_FlushDlg == NULL )
	{
		m_FlushDlg = GetFlushDialog();
		if ( m_FlushDlg == NULL ) return;
	}
	if(m_FlushDlg->GetFlashRenderTask())
		m_FlushDlg->GetFlashRenderTask()->DelFlashTaskAll(this);
}

void SQRWnd::DelFlashAttentionSpe( SQRWnd* pChildWnd, const char* sTex )
{
	if ( m_FlushDlg == NULL )
	{
		m_FlushDlg = GetFlushDialog();
		if ( m_FlushDlg == NULL ) return;
	}
	if(m_FlushDlg->GetFlashRenderTask())
		m_FlushDlg->GetFlashRenderTask()->DelFlashTask(pChildWnd, sTex);
}

// 设置wnd flash info
void SQRWnd::SetFlashInfo(EventStateMask fs, const char* name)
{
	if (fs >= IP_COUNT)
		return;
	FlashWndInfo* finfo = FlashDataManager::Inst()->GetFlashInfo(name);
	if (!finfo)
	{
		DelFlashInfo(fs);
		return;
	}

	if ( m_hWnd->m_FlashInfo.find(fs) == m_hWnd->m_FlashInfo.end() )
		m_hWnd->m_FlashInfo.insert(pair<EventStateMask, FlashWndInfo*>(fs, finfo));
	else
		m_hWnd->m_FlashInfo[fs] = finfo;
}

// 设置wnd flash info
void SQRWnd::DelFlashInfo(EventStateMask fs)
{
	if ( m_hWnd->m_FlashInfo.find(fs) != m_hWnd->m_FlashInfo.end() )
		m_hWnd->m_FlashInfo.erase(m_hWnd->m_FlashInfo.find(fs));
}

const char*	SQRWnd::GetFlashInfo(EventStateMask fs)
{
	if ( m_hWnd->m_FlashInfo.find(fs) != m_hWnd->m_FlashInfo.end() && m_hWnd->m_FlashInfo[fs] )
		return m_hWnd->m_FlashInfo[fs]->m_FlashName.c_str();
	return "";
}

UIMap<EventStateMask, FlashWndInfo*>& SQRWnd::GetFlashInfoMap()
{
	return m_hWnd->m_FlashInfo;
}

void SQRWnd::SetEventStateMask(EventStateMask es)
{
	if (   es == IP_FILL_FULL 
		|| es ==  IP_LBTNCLICK_ACTION 
		|| es == IP_RBTNCLICK_ACTION )		// 事件驱动处理
	{
		if (m_hWnd->m_FlashInfo.find(es) != m_hWnd->m_FlashInfo.end())
		{
			FlashWndInfo* info = m_hWnd->m_FlashInfo[es]; 
			AddFlashInfoShow(info, es);
		}
	}
	else if(m_ESMask != es)
	{
		m_ESMask = es;
		AddFlashInfoShow();
	}
}

////////////////////////////////////////////////////////////////////////
// 功能：注销窗口
// 参数：无
// 返回：>=0 成功，<0 失败
////////////////////////////////////////////////////////////////////////

int32 SQRWnd::DestroyWnd()
{
	if ( !m_hWnd->m_pGUI )
		return 0;
	// 删除关联flash
	DestroyFlashAtt();

	if (m_effHld)
	{
		delete m_effHld;
		m_effHld = NULL;
	}

	m_hWnd->m_pGUI->ClearFontCatch(m_hWnd->m_WndFont);
	m_hWnd->m_pGUI->OnTipHandlerDel(this);
	OnDestroy();
	OnWndShow( false );

	DelRichToolTip();
	ClearImeFx();
	DelWndFxEx();
	OnPlayWndSound(eMS_Close); 

	m_hWnd->m_pGUI->UnregistWnd( this );

	//删除所有子窗口
	while ( m_hWnd->m_FirstChild )
	{
		SQRWnd* pChildWnd = m_hWnd->m_FirstChild;
		bool bNewByGUISys  = pChildWnd->m_hWnd->m_bNewByGUISys;
		pChildWnd->DestroyWnd();
		if ( bNewByGUISys )
			delete pChildWnd;
	}

	if ( IsHeld() )
		m_hWnd->m_pGUI->SetHeldWindow( NULL );
	if ( IsFocus() && GetParent() )
		GetParent()->SetFocus();

	if ( m_hWnd->m_WndStyle & WS_MODULE )
		m_hWnd->m_pGUI->DeleteModuleWnd( this );

	if ( !m_hWnd->m_ParentWnd )
	{
		delete m_hWnd->m_pGUI;
	}
	else
	{
		//本窗口和父窗口、兄弟窗口脱离关系
		if ( m_hWnd->m_ParentWnd->m_hWnd->m_FirstChild == this )
			m_hWnd->m_ParentWnd->m_hWnd->m_FirstChild = m_hWnd->m_NextBrother;
		if ( m_hWnd->m_ParentWnd->m_hWnd->m_LastChild == this )
			m_hWnd->m_ParentWnd->m_hWnd->m_LastChild = m_hWnd->m_PreBrother;

		if ( m_hWnd->m_NextBrother )
			m_hWnd->m_NextBrother->m_hWnd->m_PreBrother = m_hWnd->m_PreBrother;
		if ( m_hWnd->m_PreBrother )
			m_hWnd->m_PreBrother->m_hWnd->m_NextBrother = m_hWnd->m_NextBrother;
	}

	SafeRelease( m_hWnd->m_WndFont );
	SafeRelease( m_hWnd->m_pTransparenTex );
	m_hWnd->m_Title.Release();
	m_hWnd->m_Enable.Release();
	m_hWnd->m_Disable.Release();
	GfkSafeDelete(m_hWnd->m_DescRichText);
	*GetWndHandle() = WndHandle();

	return 0;
}


////////////////////////////////////////////////////////////////////////
// 功能：登记快捷键
// 参数：bool bCtrl   Ctrl键是否按下,
//       bool bShift  Shift键是否按下,
//		 bool bAlt    Alt键是否按下,
//		 BYTE VirKey  其他虚拟键,
//		 WORD Command 对应的命令
// 返回：无
////////////////////////////////////////////////////////////////////////
void SQRWnd::RegisterAccelKeyCommand( bool bCtrl, bool bShift, bool bAlt, uint8 VirKey, uint32 wParam, uint32 lParam )
{
	m_hWnd->m_pGUI->RegisterAccelKeyCommand( this, bCtrl, bShift, bAlt, VirKey, wParam, lParam );
}


void SQRWnd::UnRegisterAccelKeyCommand( bool bCtrl, bool bShift, bool bAlt, uint8 VirKey )
{
	m_hWnd->m_pGUI->UnRegisterAccelKeyCommand( bCtrl, bShift, bAlt, VirKey );
}

void SQRWnd::AddChildWnd( SQRWnd *pWnd )
{
	if ( m_hWnd->m_pGUI )
		return ;

	m_hWnd->m_pGUI->RegistWnd( pWnd );

	//如果创建需要的贴图失败，返回
	pWnd->m_hWnd->m_xScreen			= GetWndLeft() + pWnd->GetWndLeft();
	pWnd->m_hWnd->m_yScreen			= GetWndTop() + pWnd->GetWndTop();
	pWnd->m_hWnd->m_xScreenbak		= pWnd->GetWndLeft();
	pWnd->m_hWnd->m_yScreenbak		= pWnd->GetWndTop();
	pWnd->m_hWnd->m_ParentWnd			= this;
	pWnd->m_hWnd->m_bNewByGUISys      = true;
}

////////////////////////////////////////////////////////////////////////
// 功能：创建窗口
// 返回：>=0 成功，<0 失败
////////////////////////////////////////////////////////////////////////
int32 SQRWnd::Create( const WndCreateParam& param )
{
	if ( !param.pParentWnd || !param.pParentWnd->m_hWnd->m_pGUI )
		return GERROR;
	if ( m_hWnd->m_pGUI )
		return GERROR;

	*GetWndHandle() = WndHandle();
	m_hWnd->m_pGUI = param.pParentWnd->m_hWnd->m_pGUI;
	m_hWnd->m_pGUI->RegistWnd( this );

	//如果创建需要的贴图失败，返回
	m_hWnd->m_WndStyle		= param.uStyle;
	if ( m_hWnd->m_WndStyle & WS_MODULE )
		bNotModule = false;
	m_hWnd->m_WindowTitle	= param.szTitle ? param.szTitle : "";
	m_hWnd->m_xScreen		= param.pParentWnd->GetWndLeft() + param.x;
	m_hWnd->m_yScreen		= param.pParentWnd->GetWndTop() + param.y;
	m_hWnd->m_Width			= max(param.width, 0);
	m_hWnd->m_Height		= max(param.height, 0);

	m_hWnd->m_xScreenbak	= GetWndLeft();
	m_hWnd->m_yScreenbak	= GetWndTop();
	m_hWnd->m_Widthbak		= m_hWnd->m_Width;
	m_hWnd->m_Heightbak		= m_hWnd->m_Height;

	m_hWnd->m_xInitial		= param.pParentWnd->m_hWnd->m_xInitial + param.x;
	m_hWnd->m_yInitial		= param.pParentWnd->m_hWnd->m_yInitial + param.y;
	m_hWnd->m_WidthInitial	= param.width;
	m_hWnd->m_HeightInitial	= param.height;

	m_hWnd->m_GroupID		= param.uGroupID;
	m_hWnd->m_ParentWnd		= param.pParentWnd;
	m_hWnd->m_WindowName	= param.szTitle;
	m_hWnd->m_szSource		= param.szSource ? param.szSource : "";//add by guo
	m_hWnd->m_DescRichText  = new SQRToolTip();
		
	// 生成m_WindowTitile对应的宽字符串
	m_hWnd->m_WindowTitleW	= utf8_to_utf16(m_hWnd->m_WindowTitle.c_str()).c_str();
	
	CFRect rt( 0.0f, 0.0f, m_hWnd->m_Width,m_hWnd->m_Height );
	uint32 uEnableColor  = CColor::RandDark();
	uint32 uDisableColor = CColor::RandDark();
	m_hWnd->m_Title.AddImage( GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), MASKCOLOR_TITLE );
	m_hWnd->m_Enable.AddImage( GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), uEnableColor );
	m_hWnd->m_Disable.AddImage( GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), uDisableColor );
	m_hWnd->m_MouseDesc.AddImage( GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), 0xCC000000, LOCK_LEFT|LOCK_RIGHT|LOCK_TOP|LOCK_BOTTOM );

	InserBefore( param.pParentWnd->FindFirstChild() );

	if ( !SetFont( param.nFontID, param.bBold, param.font_size*CFontCfg::GetInst()->GetFontZoom() ) )
	{
		DestroyWnd();
		return GERROR;
	}

	if ( param.uStyle & WS_MODULE )
		m_hWnd->m_pGUI->SetModuleWnd( this );

	m_hWnd->m_WndFont->UpdateCharInfo(m_hWnd->m_WindowTitleW.c_str());

	OnCreated();
	OnWndShow( true );

	// 创建wnd时候，如果有open effect，则播放该open effect
	OnPlayWndFx( eMF_Open );
	// 如果当前焦点在CEidt上，则不重新设置焦点
	if ( m_hWnd->m_pGUI -> GetIsEditActive() )		
		return 0;
	if ( m_hWnd->m_ParentWnd->IsActive() || (param.uStyle & WS_MODULE) )
		SetActive();

	return 0;
}

GUI_FONT* SQRWnd::GetFontInfo(uint32 idx)
{
	StyleStruct* pStyle = GetStyleStruct();
	if (pStyle)
	{
		if ( idx > 0 && idx < BLOCK_POOL::MAX_BLOCK_NUM )
		{
			int32 ix = (int32)idx;
			if (pStyle->m_vecFont.find(ix) != pStyle->m_vecFont.end())
			{
				GUI_FONT* pFont = pStyle->m_vecFont[ix];
				return pFont;
			}
		}
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// 功能：从资源中重新创建子窗口
// 参数：const char* szChildName 子窗口名字
// 返回：略
////////////////////////////////////////////////////////////////////////
int32 SQRWnd::ReCreateRes( const char* szChildName )
{
	SQRGUIManager* pGUI = m_hWnd->m_pGUI;
	UIMap<UIString, WndResParam*>::iterator it = m_hWnd->m_pResParam->m_Child.find( szChildName );
	if ( it == m_hWnd->m_pResParam->m_Child.end() )
		return GERROR;

	for ( SQRWnd* pWnd = m_hWnd->m_FirstChild; pWnd; pWnd = pWnd->FindNextWnd() )
	{
		if ( szChildName == pWnd->m_hWnd->m_WindowName )
			return GERROR;
	}

	WndResParam& CP = *it->second;
	SQRWnd* pChild = SQRWndClass::CreateWndObject( CP.m_UseClassName );
	if ( !pChild )
		pChild = SQRWndClass::CreateWndObject( CP.m_ClassName );
	if ( pChild )
	{
		WndCreateParam param;
		param.szTitle	 = CP.m_WindowTitle;
		param.uStyle	 = CP.m_WndStyle;
		param.x			 = (float)CP.m_x;
		param.y			 = (float)CP.m_y;
		param.width		 = (float)CP.m_Width;
		param.height	 = (float)CP.m_Height;
		param.pParentWnd = this;

		GUI_FONT* pFont = pChild->GetFontInfo(CP.m_uStyleIndex);
		if (pFont)
		{
			param.nFontID	 = pFont->m_nFontID;
			param.font_size  = (float)pFont->m_FontSize;
		}
		else
		{
			param.nFontID	 = CP.m_nFontID;
			param.font_size  = (float)CP.m_FontSize;
		}
		
		param.uGroupID   = CP.m_GroupID;
		if ( FAILED( pChild->Create( param ) ) )
		{
			delete pChild;
			return GERROR;
		}
		else
		{
			pChild->m_hWnd->m_bNewByGUISys = true;
			pChild->SetLockFlag( CP.m_uLockFlage );
			if (pFont)
			{
				pChild->SetFontGradualColor( pFont->m_uFontGradualColor );
				pChild->SetFontEffectMask( pFont->m_uFontEffectMask );
			}
			else
			{
				pChild->SetFontGradualColor( CP.m_uFontGradualColor );
				pChild->SetFontEffectMask( CP.m_uFontEffectMask );
			}
			
			pChild->SetIsDrawBackground(  CP.m_isShowBackground );
			pChild->SetStyleIndex( CP.m_uStyleIndex );
			pChild->SetMouseOverDesc( CP.m_szDesc );
			
			//effect
			pChild->SetMsgFx( eMF_Click,	CP.m_MsgFx[eMF_Click] );
			pChild->SetMsgFx( eMF_Open,		CP.m_MsgFx[eMF_Open] );
			pChild->SetMsgFx( eMF_Close,	CP.m_MsgFx[eMF_Close] );

			//sound
			pChild->SetMsgSound( eMS_Click, CP.m_MsgSound[eMS_Click] );
			pChild->SetMsgSound( eMS_Open,  CP.m_MsgSound[eMS_Open] );
			pChild->SetMsgSound( eMS_Close, CP.m_MsgSound[eMS_Close] );

			pChild->m_hWnd->m_pResParam = &CP;
			pChild->m_hWnd->m_WindowName = it->first;
			pChild->m_hWnd->m_strTransName = CP.m_TransTexture;
			pChild->m_hWnd->m_bShow			= false;


			//if(pChild->m_hWnd->m_strTransName.size()>0)
			//{
			//	bool bEnableASynRead = CPkgFile::IsEnableAsyncRead();
			//	CPkgFile::EnableAsyncRead( false );
			//	pGUI->GetGraphic()->CreateTexture( GUI_PATH_ALIAS_GUITEX.c_str(), (pChild->m_hWnd->m_strTransName).c_str(), &(pChild->m_hWnd->m_pTransparenTex) ); // szTextName
			//	CPkgFile::EnableAsyncRead( bEnableASynRead );
			//}

			for ( int32 i = 0; i < (int32)CP.m_ImageList.size(); i++ )
			{
				ChildWndInfo& Info = CP.m_ImageList[i];
				if ( i )
				{
					CFRect rt = Info.rtWnd;
					rt.left   += GetWndLeft() - m_hWnd->m_pResParam->m_x;
					rt.right  += GetWndLeft() - m_hWnd->m_pResParam->m_x;
					rt.top    += GetWndTop() - m_hWnd->m_pResParam->m_y;
					rt.bottom += GetWndTop() - m_hWnd->m_pResParam->m_y;
					pChild->GetResWnd( i + 1 )->SetStyleIndex(Info.m_uStyleIndex);
					pChild->GetResWnd( i + 1 )->ResizeRect( rt );
				}
				for ( int32 j = 0; j < (int32)Info.ChildBkList.size(); j++ )
				{
					IMAGE_PARAM IP;
					IP.CtrlMask= WORD( i + 1 );
					IP.StateMask= WORD( Info.ChildBkList[j].nStateMask );
					WND_IMAGE_LIST* pImage = pChild->GetWndBkImage( IP );

					if (pFont)
					{
						pChild->SetWndTextColor( IP, pFont->m_EventFont[(EventStateMask)(IP.StateMask)].m_FontColor );
						pChild->SetWndTextBcColor( IP, pFont->m_EventFont[(EventStateMask)(IP.StateMask)].m_uFontBkColor );
					}
					else
					{
						pChild->SetWndTextColor( IP, Info.ChildBkList[j].uTextColor );
						pChild->SetWndTextBcColor( IP, Info.ChildBkList[j].uRimColor );
					}

					if ( pImage )
						*pImage = m_hWnd->m_pGUI->LoadBKRes( Info.ChildBkList[j].ImageList );
				}
			}
			pChild->OnResCreated( pChild->m_hWnd->m_pResParam->m_UseInfo );
		}
	}

	return GERROR;
}

////////////////////////////////////////////////////////////////////////
// 功能：返回鼠标信息显示时间
// 参数：无
// 返回：鼠标信息显示时间
////////////////////////////////////////////////////////////////////////
uint32 SQRWnd::OnPreShowMouseDesc()
{
	return 100;
}

////////////////////////////////////////////////////////////////////////
//鼠标Tooltip提示的RichText有效
////////////////////////////////////////////////////////////////////////
void SQRWnd::PrepareDefaultDesc()
{
	if ( GetRichToolTip() )
		SQRRichToolTip::PrepareToolTip(this);
	else
		SQRToolTip::PrepareToolTip(this);
}

void SQRWnd::PushTipData()
{
	if ( GetRichToolTip() )
		GetRichToolTip()->PushData();
}

void SQRWnd::CopyToClipboard( const char* str )
{
	if ( OpenClipboard( NULL ) )
	{
		EmptyClipboard();

		wstring utf16_str = utf8_to_utf16(str); 
		int32	nStrLen = utf16_str.length()*2 + 2;
		char*	pStrBuf = (char*)VirtualAlloc(0, nStrLen, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
		if ( pStrBuf == 0 )
		{
			CloseClipboard();
			return;
		}

		CharToOemBuffW( utf16_str.c_str(), pStrBuf, nStrLen );

		if ( strlen(pStrBuf) != 0 && pStrBuf[nStrLen-1] == '\n' )
			pStrBuf[nStrLen-1] = '\0';

		HANDLE hMem = GlobalAlloc( GMEM_MOVEABLE|GMEM_DDESHARE, nStrLen );
		char* Buf = (char*)GlobalLock( hMem );
		memset(Buf, 0, nStrLen);
		memcpy( Buf, pStrBuf, nStrLen);
		GlobalUnlock( hMem );
		SetClipboardData( CF_TEXT, hMem );
		CloseClipboard();
		GlobalFree( hMem );

		VirtualFree(pStrBuf, 0, MEM_RELEASE);
	}
}

////////////////////////////////////////////////////////////////////////
// 功能：x,y位置是否在窗口里面
// 参数：int32 x, int32 y  位置
// 返回：略
////////////////////////////////////////////////////////////////////////
bool SQRWnd::IsInWndArea( float x, float y )
{
	uint32 uStyle = GetStyle();
	if ( uStyle&WS_CUTBYPARENT )
	{
		if ( GetParent() && !GetParent()->IsInWndArea( x, y ) )
			return false;
	}

	if ( ( WS_MSGTRANSPT&uStyle ) == 0 )
	{
		float xMe = x - GetWndLeft();
		float yMe = y - GetWndTop();

		if ( WS_AREAUSETEXRECT&uStyle )
		{
			if (m_hWnd->m_pTransparenTex)
			{
				if ( xMe >= 0.0f && yMe >= 0.0f && xMe < m_hWnd->m_Width && yMe < m_hWnd->m_Height 
					&& !m_hWnd->m_pTransparenTex->IsTransparent(
					xMe/(m_hWnd->m_Width),
					yMe/(m_hWnd->m_Height)) )
					return true;
				else return false;
			}
			else
			{
				WND_IMAGE_LIST& ImageList = m_hWnd->m_Title;
				for ( int32 i = 0; i < ImageList.GetImageCount(); i++ )
				{
					IMAGE& Image = ImageList.GetImage(i);
					if ( Image.dwColor == MASKCOLOR_AREA )
					{
						if ( xMe >= Image.posWnd.x && xMe < Image.posWnd.x + abs(Image.rtTexture.Width()) &&
							yMe >= Image.posWnd.y && yMe < Image.posWnd.y + abs(Image.rtTexture.Height()) )
						{
							return true;
						}
					}
				}
			}
		}
		else
		{
			if ( xMe >= 0.0f && yMe >= 0.0f && xMe < m_hWnd->m_Width && yMe < m_hWnd->m_Height )
			{
				if (m_hWnd->m_pTransparenTex)
				{
					if (!m_hWnd->m_pTransparenTex->IsTransparent(
						xMe/(m_hWnd->m_Width),
						yMe/(m_hWnd->m_Height)) )
						return true;
					else return false;
				}
				else
					return true;
			}
		}
	}

	return false;
}

bool SQRWnd::IsInFrameArea( float x, float y )
{
	float xMe = x - GetWndLeft();
	float yMe = y - GetWndTop();
	if ( xMe >= 0.0f && yMe >= 0.0f && xMe < m_hWnd->m_Width && yMe < m_hWnd->m_Height )
		return true;
	return false;
}

float SQRWnd::GetWndLeft()
{
	return m_hWnd->m_xScreen; 
}

float SQRWnd::GetWndTop()
{
	return m_hWnd->m_yScreen;
}

////////////////////////////////////////////////////////////////////////
// 功能：x,y位置是否在窗口可拖动区域里面
// 参数：int32 x, int32 y  位置
// 返回：略
////////////////////////////////////////////////////////////////////////
bool SQRWnd::IsInTitleArea( float x, float y )
{
	if ( x < GetWndLeft() || y < GetWndTop() ||
	        x >= GetWndLeft() + m_hWnd->m_Width || y >= GetWndTop() + m_hWnd->m_Height )
		return false;

	x -= GetWndLeft();
	y -= GetWndTop();

	WND_IMAGE_LIST& ImageList = m_hWnd->m_Title;
	for ( int32 i = 0; i < ImageList.GetImageCount(); i++ )
	{
		IMAGE& Image = ImageList.GetImage(i);
		if ( Image.dwColor == MASKCOLOR_TITLE )
		{
			if (( x < Image.posWnd.x || x >= Image.posWnd.x + Image.rtTexture.Width() ) &&
			     ( y < Image.posWnd.y || y >= Image.posWnd.y + Image.rtTexture.Height() ))
				return false;
		}
	}
	return true;
}


////////////////////////////////////////////////////////////////////////
// 功能：发送消息
// 参数：uint32 message, WPARAM wParam, LPARAM lParam 消息内容
// 返回：略
////////////////////////////////////////////////////////////////////////
LRESULT SQRWnd::SendMsg( uint32 message, WPARAM wParam, LPARAM lParam )
{
	return IsCreated() ? WndProc( message, wParam, lParam ) : 0;
}

////////////////////////////////////////////////////////////////////////
// 功能：窗口处理的消息
// 参数：uint32 message, WPARAM wParam, LPARAM lParam 消息内容
// 返回：略
////////////////////////////////////////////////////////////////////////
LRESULT SQRWnd::WndProc( uint32 message, WPARAM wParam, LPARAM lParam )
{
	CALL_CLASS_CALLBACK_3_RET(LRESULT, message, wParam, lParam);
	SQR_TRY
	{
		switch ( message )
		{
		case WM_HOTKEY:
			break;
		case WM_CHAR:
			OnChar( (char)wParam, lParam );
			break;
		case WM_IME_CHAR:
			OnImeChar( wParam, lParam );
			break;
		case WM_KEYDOWN:
			OnKeyDown( wParam, lParam );
			break;
		case WM_KEYUP:
			OnKeyUp( wParam, lParam );
			break;
		case WM_LBUTTONDBLCLK:
			OnLButtonDblClk( wParam, LOWORD(lParam), HIWORD(lParam) );
			break;
		case WM_LBUTTONDOWN:
			if(GetKeyState(VK_SHIFT) & 0x8000)
				OnShiftLButtonDown( wParam, LOWORD(lParam), HIWORD(lParam) );
			else
				OnLButtonDown( wParam, LOWORD(lParam), HIWORD(lParam) );
			break;
		case WM_LBUTTONUP:
			if(GetKeyState(VK_SHIFT) & 0x8000)
				OnShiftLButtonUp( wParam, LOWORD(lParam), HIWORD(lParam) );
			else
				if ( this == m_hWnd->m_pGUI->GetLastLButtonDownWnd() )
					OnLButtonClick( wParam, LOWORD(lParam), HIWORD(lParam) );
				else
					OnLButtonUp( wParam, LOWORD(lParam), HIWORD(lParam) );
			break;
		case WM_MBUTTONDBLCLK:
			OnMButtonDblClk( wParam, LOWORD(lParam), HIWORD(lParam) );
			break;
		case WM_MBUTTONDOWN:
			OnMButtonDown( wParam, LOWORD(lParam), HIWORD(lParam) );
			break;
		case WM_MBUTTONUP:
			OnMButtonUp( wParam, LOWORD(lParam), HIWORD(lParam) );
			break;
		case WM_MOUSEMOVE:
			OnMouseMove( wParam, LOWORD(lParam), HIWORD(lParam) );
			break;
		case WM_MOUSEWHEEL:
			OnMouseWheel( LOWORD(wParam), (short) HIWORD(wParam), LOWORD(lParam), HIWORD(lParam) );
			break;
		case WM_RBUTTONDBLCLK:
			OnRButtonDblClk( wParam, LOWORD(lParam), HIWORD(lParam) );
			break;
		case WM_RBUTTONDOWN:
			OnRButtonDown( wParam, LOWORD(lParam), HIWORD(lParam) );
			break;
		case WM_RBUTTONUP:
			if ( this == m_hWnd->m_pGUI->GetLastRButtonDownWnd() )
				OnRButtonClick( wParam, LOWORD(lParam), HIWORD(lParam) );
			else
				OnRButtonUp( wParam, LOWORD(lParam), HIWORD(lParam) );
			break;
		case WM_SETCURSOR:
			return OnSetCursor();
		default:
			return 0;
		}
		return 0;
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;
	
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// 功能：播放窗口特效
// 参数：EMsgFx eFx 特效类型
// 返回：略
////////////////////////////////////////////////////////////////////////
void SQRWnd::OnPlayWndFx( EMsgFx eFx )
{
	//CALL_CLASS_CALLBACK_1(eFx)
	if ( !IsCreated() )
		return;

	UIString fx = GetMsgFx( eFx );
	if (fx.size() == 0 || 0 == fx[0])
		return;

	if (eFx == eMF_Click || eFx == eMF_Open)
	{
		void* ptr = SQRGUIManager::GetGUI( this )->PlayFxIme( this, fx.c_str() );
		if (ptr)
			m_Effect.push_back( fx );
	}
	else
	{
		// distribute the wnd fx
		SQRGUIManager::GetGUI( this )->PlayFx( this, fx.c_str() );
	}
}

//活动状态改变
void    SQRWnd::OnActive( bool bActive, SQRWnd* Wnd ) 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_2(bActive, Wnd) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
}

//窗口创建
void    SQRWnd::OnCreated() 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK() 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
}

//窗口创建
void    SQRWnd::OnDestroy() 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK() 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
}

//窗口创建
void    SQRWnd::OnResCreated( const char* szResInfo ) 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_1(szResInfo) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
}

//子窗口创建
void    SQRWnd::OnChildCreated() 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK() 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
}

//父窗口移动
void	SQRWnd::OnParentWndMoved( float dx, float dy ) 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_2(dx, dy) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
}

//窗口移动前
void	SQRWnd::OnWndMoved( float dx, float dy ) 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_2(dx, dy) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
}

//窗口显示
void	SQRWnd::OnWndShow( bool bShow ) 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_1(bShow) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
}


void	SQRWnd::OnMouseDescEnd() 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK() 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
}

//--------------------标准Windows消息--------------------
//字符消息
void	SQRWnd::OnChar( uint32 nChar, uint32 nFlags ) 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_2(nChar, nFlags) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
	
}
//Ime字符消息
void	SQRWnd::OnImeChar( uint32 nChar, uint32 nFlags ) 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_2(nChar, nFlags) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
	
}
//键盘按下消息
void	SQRWnd::OnKeyDown( uint32 nChar, uint32 nFlags ) 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_2(nChar, nFlags) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
	
}
//键盘弹起消息
void	SQRWnd::OnKeyUp( uint32 nChar, uint32 nFlags ) 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_2(nChar, nFlags) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
	
}
//左键双击消息
void	SQRWnd::OnLButtonDblClk( uint32 nFlags, int32 x, int32 y ) 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_3(nFlags, x, y) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
	
}
//左键点击消息
void	SQRWnd::OnLButtonDown( uint32 nFlags, int32 x, int32 y ) 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_3(nFlags, x, y) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
	
}
//左键释放消息
void	SQRWnd::OnLButtonUp( uint32 nFlags, int32 x, int32 y ) 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_3(nFlags, x, y) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
	
}
//一次鼠标左键按下抬起操作
void	SQRWnd::OnLButtonClick( uint32 nFlags, int32 x, int32 y ) 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_3(nFlags, x, y) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
	
}
//按住shift时候左键释放消息
void	SQRWnd::OnShiftLButtonUp( uint32 nFlags, int32 x, int32 y ) 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_3(nFlags, x, y) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
	
}
//按住shift时候左键释放消息
void	SQRWnd::OnShiftLButtonDown( uint32 nFlags, int32 x, int32 y ) 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_3(nFlags, x, y) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
	
}
//中键双击消息
void	SQRWnd::OnMButtonDblClk( uint32 nFlags, int32 x, int32 y ) 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_3(nFlags, x, y) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
	
}
//中键点击消息
void	SQRWnd::OnMButtonDown( uint32 nFlags, int32 x, int32 y ) 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_3(nFlags, x, y) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
	
}
//中键弹起消息
void	SQRWnd::OnMButtonUp( uint32 nFlags, int32 x, int32 y ) 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_3(nFlags, x, y) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
	
}
//鼠标移动消息
void	SQRWnd::OnMouseMove( uint32 nFlags, int32 x, int32 y ) 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_3(nFlags, x, y) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
	
}
//右键双击消息
void	SQRWnd::OnRButtonDblClk( uint32 nFlags, int32 x, int32 y ) 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_3(nFlags, x, y) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
	
}
//右键点击消息
void	SQRWnd::OnRButtonDown( uint32 nFlags, int32 x, int32 y ) 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_3(nFlags, x, y) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
	
}
//右键释放消息
void	SQRWnd::OnRButtonUp( uint32 nFlags, int32 x, int32 y ) 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_3(nFlags, x, y) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
	
}
//一次鼠标右键按下抬起操作
void	SQRWnd::OnRButtonClick( uint32 nFlags, int32 x, int32 y ) 
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_3(nFlags, x, y) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
	
}


void SQRWnd::OnCtrlmsg( SQRWnd* pChild, uint32 uMsgID, uint32 uParam1, uint32 uParam2 ) 
{
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_4(pChild, uMsgID, uParam1, uParam2) 
	}
	SQR_CATCH(exp) 
	{
		string errmsg = GetWndHandle()->m_WindowName.c_str();
		exp.AppendMsg(errmsg.c_str());
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
}

LRESULT SQRWnd::OnCommand( WPARAM wParam, LPARAM lParam )
{
	SQR_TRY
	{
		CALL_CLASS_CALLBACK_2_RET(LRESULT, wParam, lParam) 
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
	return 0;
}

void SQRWnd::OnSized()
{ 
	SQR_TRY
	{
		CALL_CLASS_CALLBACK()
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
}

void SQRWnd::OnShapeEffectEnded()
{
	SQR_TRY
	{
		CALL_CLASS_CALLBACK()
	}
	SQR_CATCH(exp) 
	{
		GfkLogErrOnce(exp.ErrorTitle(), exp.ErrorMsg());
	}
	SQR_TRY_END;
}

////////////////////////////////////////////////////////////////////////
// 功能：播放窗口音效
// 参数：ESoundMsg eSound 音效消息类型
// 返回：略
////////////////////////////////////////////////////////////////////////
void SQRWnd::OnPlayWndSound( ESoundMsg eSound )
{	
	UIString strSoundName = GetMsgSound( eSound );
	if( strSoundName != "" )
		PlayCue( GetMsgSound( eSound ) );
}

void SQRWnd::PlayScreenPathFx(const char* fxFile, const char* fxName, EEffectRun eEffrun)
{
	SQRGUIManager::GetGUI( this )->PlayFxEx( NULL, fxFile, fxName, eEffrun );
}

void SQRWnd::PlayWndPathFx(const char* fxFile, const char* fxName, EEffectRun eEffrun)
{
	if ( !IsCreated() )
		return;
	SQRGUIManager::GetGUI( this )->PlayFxEx( this, fxFile, fxName, eEffrun );
}

void SQRWnd::PlayWndImePathFx(const char* fxFile, const char* fxName, EEffectRun eEffrun)
{
	UIString fx = fxName;
	void* ptr = SQRGUIManager::GetGUI( this )->PlayFxImeEx( this, fxFile, fx.c_str(), eEffrun);
	if (ptr)
		m_Effect.push_back( fx );
}

//------------------------------------------------------------------------------
void SQRWnd::PlayWndFx(const char* fxName)
{
	PlayWndFxEx( fxName );
}

void SQRWnd::PlayScreenFx( const char* fxName )
{
	PlayScreenFxEx(fxName);
}

void SQRWnd::PlayWndFxEx(const char* fxName, EEffectRun eEffrun)
{
	if ( !IsCreated() )
		return;
	SQRGUIManager::GetGUI( this )->PlayFx( this, fxName, eEffrun );
}

void SQRWnd::PlayScreenFxEx( const char* fxName, EEffectRun eEffrun)
{
	SQRGUIManager::GetGUI( this )->PlayFx( NULL, fxName, eEffrun );
}

void SQRWnd::DelWndFx(const char* fxName)
{
	SQRGUIManager::GetGUI( this )->ClearFx( this, fxName);
}

void SQRWnd::DelWndFxEx()
{
	SQRGUIManager::GetGUI( this )->ClearFx( this );
}

void SQRWnd::DelScreenFx(const char* fxName)
{
	SQRGUIManager::GetGUI( this )->ClearFx( NULL, fxName);
}

void SQRWnd::PlayWndImeFx(const char* fxName, EEffectRun eEffrun)
{
	UIString fx = fxName;
	void* ptr = SQRGUIManager::GetGUI( this )->PlayFxIme( this, fx.c_str(), eEffrun);
	if (ptr)
		m_Effect.push_back( fx );
}

void SQRWnd::DelWndImeFx(const char* fxName)
{
	UIString fx = fxName;
	EffectVec::iterator itr = m_Effect.begin();
	EffectVec::iterator eitr = m_Effect.end();
	while (itr != eitr)
	{
		if ( *itr == fx)
		{
			m_Effect.erase(itr);
			break;
		}
		++itr;
	}
}

void SQRWnd::DrawWndBackground()
{
	m_beBGDrawed = true;
	if (m_hWnd->m_bIsShowBackground)
	{
		bool isEnable = IsEnable();
		SetEventStateMask(isEnable ? IP_ENABLE : IP_DISABLE);
		DrawRect( isEnable ? m_hWnd->m_Enable : m_hWnd->m_Disable );
	}
}

void SQRWnd::DrawWndArea()
{
	CFRect rtWnd =  CFRect( 0, 0, (float)GetWndWidth(), (float)GetWndHeight() );
	if ( GetStyle()&WS_CUTBYPARENT )
	{
		CFRect rtRootCutWnd = GetParentCutRect();
		ScreenToWnd( rtRootCutWnd );
		rtWnd = rtWnd.Intersection(rtRootCutWnd);
	}
	if ( m_pCutWnd )
	{
		CFRect rtCutWnd;
		m_pCutWnd->GetCutAreaRect(rtCutWnd);
		ScreenToWnd( rtCutWnd );
		rtWnd = rtWnd.Intersection(rtCutWnd);
	}


	ITexture* pTexture = m_hWnd->m_pTransparenTex;
	if(pTexture)
	{
		CFRect rtScreen = rtWnd;
		CFRect rtText( 0, 0, (float)pTexture->GetWidth(), (float)pTexture->GetHeight() );

		float rootx = GetRootWnd()->GetWndHandle()->m_xScreen + GetWndLeft() ;
		float rooty = GetRootWnd()->GetWndHandle()->m_yScreen + GetWndTop();

		rtScreen.left		+= rootx;
		rtScreen.right		+= rootx;
		rtScreen.top		+= rooty;
		rtScreen.bottom		+= rooty;

#ifdef UI_HIDE_MODE
		if (m_hWnd->m_pGUI->GetHidenMode() && !IsModleState() )
		{
			return;
		}
#endif

		m_hWnd->m_pGUI->DrawRect( rtScreen, 0xFFFFFFFF, m_hWnd->m_pTransparenTex, &rtText, RS_FONT, m_hWnd->m_fZ );
	}
	else
		DrawRect(0xFF0000FF);
}

bool SQRWnd::IsModleState()
{
	return ( GetStyle()&WS_MODULE || (GetParent() && GetParent()->GetStyle()&WS_MODULE) );
}

void SQRWnd::DrawAfterChildren()
{
}

void SQRWnd::DrawAfterAllEyeCandy()
{
}

void SQRWnd::DrawWndText()
{
	if (m_hWnd->m_WndFont)
		m_hWnd->m_WndFont->SetAlignToPixel(m_bAlignToPixel);
	
	DrawText( m_hWnd->m_WindowTitleW.c_str(),
	          IsEnable() ? m_hWnd->m_EnableColor : m_hWnd->m_DisableColor,
	          IsEnable() ? m_hWnd->m_EnableBcColor : m_hWnd->m_DisableBcColor );
}

void SQRWnd::DrawText( const wstring& text, uint32 textColor, uint32 textBcColor )
{
	if (NULL == m_hWnd->m_WndFont || text.empty())
		return;

	if ( (WS_MULLINETEXT&GetStyle()) == WS_MULLINETEXT )
	{
		DrawText( text, m_itX, m_itY, textColor, textBcColor, true, NULL);
	}
	else
	{
		float x = m_itX;
		float y = m_itY;

		// 需要根据缩放比例提前设置字体大小
		float	f_size = m_hWnd->m_FontSize;
		f_size *= m_hWnd->m_pGUI->GetZoomSize();
		f_size *= m_WndZoom;
		m_hWnd->m_WndFont->Resize(f_size);
		
		CVector2f	size	= GetFont()->CalculateSize(text.c_str());
		float		Width	= size.x;
		float		Height	= size.y;

		if ( GetStyle() & WS_TEXTXC )
			x = (m_hWnd->m_Width - Width)/2.0f;
		else if ( GetStyle() & WS_TEXTXR )
			x = m_hWnd->m_Width - Width - m_itX;

		if ( GetStyle() & WS_TEXTYC )
			y = (m_hWnd->m_Height - Height)/2.0f;
		else if ( GetStyle() & WS_TEXTYB )
			y = m_hWnd->m_Height - Height - m_itY;

		DrawText( text, x, y, textColor, textBcColor, false, NULL );
	}
}

//------------------------------------------------------------------------------
void
SQRWnd::DrawText(const wstring& text, float x, float y, CColor color, CColor bkColor, bool multiline, CFRect* rect)
{
	DrawText( text, x, y, color, bkColor, multiline, rect, false );
}

//------------------------------------------------------------------------------
void
SQRWnd::DrawText(const wstring& text, float x, float y, CColor color, CColor bkColor, bool multiline, CFRect* rect, bool vertical)
{
	if (NULL == m_hWnd->m_WndFont || text.empty())
		return;

	

	CFRect rtWnd = (NULL != rect) ? *rect : CFRect(0, 0, GetWndWidth(), GetWndHeight());
	if (GetStyle() & WS_CUTBYPARENT)
	{
		WndToScreen(rtWnd);
		x += rtWnd.left;
		y += rtWnd.top;
		CFRect rtParent = GetParentCutRect();
		rtWnd = rtWnd.Intersection(rtParent);
		x -= rtWnd.left;
		y -= rtWnd.top;
		rect = &rtWnd;
		ScreenToWnd(rtWnd);
	}
	if ( m_pCutWnd )
	{
		CFRect rtCutWnd;
		m_pCutWnd->GetCutAreaRect(rtCutWnd);
		ScreenToWnd( rtCutWnd );
		rtWnd = rtWnd.Intersection(rtCutWnd);
	}

	CColor gradual = GetFontGradualColor();
	color.A = uint8(color.A * m_hWnd->m_fTransparent);
	bkColor.A = uint8(bkColor.A * m_hWnd->m_fTransparent);
	gradual.A = uint8(gradual.A * m_hWnd->m_fTransparent);

	FontEffect::Mask mask = GetFontEffectMask();
	if (multiline)
		mask |= FontEffect::Multiline;
	if (vertical)
		mask |= FontEffect::Vertical;

	WndToScreen(rtWnd);

#ifdef UI_HIDE_MODE
	if (m_hWnd->m_pGUI->GetHidenMode() && !IsModleState() )
	{
		return;
	}
#endif

	m_hWnd->m_pGUI->DrawText(m_hWnd->m_WndFont, m_hWnd->m_FontSize*m_WndZoom,
	                                 text, color, bkColor, gradual,
	                                 rtWnd, x, y, 0.0f,
	                                 mask, 0);
}

void SQRWnd::DrawMouseOver()
{
	if ( GetRichToolTip() )
		SQRRichToolTip::DrawToolTip(this);
	else
		SQRToolTip::DrawToolTip(this);
}

void SQRWnd::DrawRect( ITexture* pTexture, const CFRect& rt, uint32 uColor, const CFRect* pTextRect, bool bClip )
{
	CColor cl = uColor;
	cl.A = static_cast<uint8>( cl.A*m_hWnd->m_fTransparent );
	uColor = cl;

	if ( !bClip )
	{
		CFRect rtScreen = rt;
		WndToScreen( rtScreen );
#ifdef UI_HIDE_MODE
		if (m_hWnd->m_pGUI->GetHidenMode() && !IsModleState() )
		{
			return;
		}
#endif
		m_hWnd->m_pGUI->DrawRect( rtScreen, uColor, pTexture?pTexture:CGraphic::GetInst()->GetSystemTexture(eSTT_WHITE), pTextRect, m_hWnd->m_eRS, m_hWnd->m_fZ, offset );
	}
	else
	{
		CFRect rtText( 0, 0, 0, 0 );
		if ( pTexture )
		{
			if ( pTextRect )
				rtText = *pTextRect;
			else
			{
				rtText.right = (float)pTexture->GetWidth();
				rtText.bottom = (float)pTexture->GetHeight();
			}
		}
		else
		{
			rtText = CFRect( 0.0f, 0.0f, m_hWnd->m_Width, m_hWnd->m_Height );
		}

		if (GetStyle() & WS_CUTBYPARENT)
		{
			WndToScreen(rtText);
			CFRect rtParent = GetParentCutRect() ;
			rtText = rtText.Intersection(rtParent);
			ScreenToWnd(rtText);
		}
		if ( m_pCutWnd )
		{
			CFRect rtCutWnd;
			m_pCutWnd->GetCutAreaRect(rtCutWnd);
			ScreenToWnd( rtCutWnd );
			rtText = rtText.Intersection(rtCutWnd);
		}

		CFRect rtDraw = rt.Intersection(rtText);
		if (rtDraw.IsEmpty())
			return;

		rtText = rtText + rtDraw - rt;
		rtDraw = WndToScreen( rtDraw );
#ifdef UI_HIDE_MODE
		if (m_hWnd->m_pGUI->GetHidenMode() && !IsModleState() )
		{
			return;
		}
#endif
		m_hWnd->m_pGUI->DrawRect( rtDraw, uColor, pTexture?pTexture:CGraphic::GetInst()->GetSystemTexture(eSTT_WHITE), &rtText, m_hWnd->m_eRS, m_hWnd->m_fZ );
	}
}

void SQRWnd::DrawRect( const CVector2f* rt, uint32 uColor, ITexture* pTexture0, ITexture* pTexture1, const CFRect* rtText0,const CVector2f* rtText1, ERenderStyle rs, float z )
{
#ifdef UI_HIDE_MODE
	if (m_hWnd->m_pGUI->GetHidenMode() && !IsModleState() )
	{
		return;
	}
#endif
	m_hWnd->m_pGUI->DrawRect( rt, uColor, pTexture0 ,pTexture1,rtText0,rtText1,rs,z );
}

void SQRWnd::DrawRect( const CVector2f* rt, uint32 uColor, ITexture* pTexture0, ITexture* pTexture1, ITexture* pTexture2,ERenderStyle rs, float z )
{
#ifdef UI_HIDE_MODE
	if (m_hWnd->m_pGUI->GetHidenMode() && !IsModleState() )
	{
		return;
	}
#endif
	m_hWnd->m_pGUI->DrawRect( rt, uColor, pTexture0 ,pTexture1,pTexture2,rs,z );
}

void SQRWnd::DrawRect( ITexture* pTexture, const CFRect& rt, uint32 uColor, const CPosRect& TextRect )
{
	CColor cl = uColor;
	cl.A = static_cast<uint8>( cl.A*m_hWnd->m_fTransparent );
	uColor = cl;

	CFRect rtScreen = rt;
	rtScreen.left		+= GetWndLeft();
	rtScreen.right		+= GetWndLeft();
	rtScreen.top		+= GetWndTop();
	rtScreen.bottom		+= GetWndTop();

#ifdef UI_HIDE_MODE
	if (m_hWnd->m_pGUI->GetHidenMode() && !IsModleState() )
	{
		return;
	}
#endif

	m_hWnd->m_pGUI->DrawRect( rtScreen, uColor, pTexture, TextRect, m_hWnd->m_eRS, m_hWnd->m_fZ );
}

struct CalCoord
{
	CVector2f m_posStart;
	CVector2f m_posEnd;
	CVector2f m_texStart;
	CVector2f m_texEnd;
	float	  m_fLength;
	CalCoord( CVector2f posStart, CVector2f posEnd, CVector2f texStart, CVector2f texEnd ) :
			m_posStart( posStart ), m_posEnd( posEnd ), m_texStart( texStart ), m_texEnd( texEnd ),
			m_fLength( ( posEnd - posStart ).Mag() ) {}
	CVector2f GetTextCoord( CVector2f pos )
	{
		return m_fLength > 0.000001f ? ( pos - m_posStart ).Mag()*( m_texEnd - m_texStart )/m_fLength + m_texStart : m_texStart;
	}
};

void SQRWnd::DrawRect( const CVector2f* rt, uint32 uColor, ITexture* pTexture, const CVector2f* rtText, bool bClip )
{
	CColor cl = uColor;
	cl.A = static_cast<uint8>( cl.A*m_hWnd->m_fTransparent );
	uColor = cl;

	if ( bClip )
	{
		bClip = false;
		CFRect rtWnd = CFRect( 0, 0, (float)GetWndWidth(), (float)GetWndHeight() );
		for ( int32 i = 0; i < 4; i++ )
		{
			if ( rt[i].x < 0 || rt[i].x	>= rtWnd.right || rt[i].y < 0 || rt[i].y >= rtWnd.bottom )
			{
				bClip = true;
				break;
			}
		}

		if ( bClip )
		{
			int32 n[4] = { 0, 1, 3, 2 };
			UIVector<CVector2f> Point;
			UIVector<CVector2f> Texture;

			CVector2f tex[4] = { CVector2f( 0, 0 ), CVector2f( 0, 0 ), CVector2f( 0, 0 ), CVector2f( 0, 0 )	};
			if ( !rtText )
			{
				if ( pTexture )
				{
					ITexture* pText = (ITexture*)pTexture;
					tex[0] = CVector2f( 0,						(float)pText->GetHeight() );
					tex[1] = CVector2f( 0,						0 );
					tex[2] = CVector2f( (float)pText->GetWidth(),	(float)pText->GetHeight() );
					tex[3] = CVector2f( (float)pText->GetWidth(),	0 );
				}
				rtText = tex;
			}

			for ( int32 i = 0; i < 4; i++ )
			{
				CalCoord Cal( rt[n[i]], rt[n[(i+1)&3]], rtText[n[i]], rtText[n[(i+1)&3]] );
				CVector2f vStart = Cal.m_posStart;
				CVector2f vEnd   = Cal.m_posEnd;
				if ( Cut2DLine( rtWnd, vStart, vEnd ) )
				{
					if ( Point.empty() || Point[Point.size()-1] != vStart )
					{
						Point.push_back( vStart );
						Texture.push_back( Cal.GetTextCoord( vStart ) );
					}

					if ( Point[Point.size()-1] != vEnd )
					{
						Point.push_back( vEnd );
						Texture.push_back( Cal.GetTextCoord( vEnd ) );
					}
				}
			}

			size_t nSize = Point.size();
			if ( !nSize )
				return;
			Point.push_back( Point[0] );
			Texture.push_back( Texture[0] );

			CVector2f vStart( (float)GetWndLeft(), (float)GetWndTop() );
#ifdef UI_HIDE_MODE
			if (m_hWnd->m_pGUI->GetHidenMode() && !IsModleState() )
			{
				return;
			}
#endif
			for ( size_t i = 1; i < nSize - 1; i += 2 )
			{
				CVector2f rtScreen[4]  = { Point[0] + vStart, Point[i] + vStart, Point[i+2] + vStart, Point[i+1] + vStart };
				CVector2f rtTexture[4] = { Texture[0], Texture[i], Texture[i+2], Texture[i+1] };
				m_hWnd->m_pGUI->DrawRect( rtScreen, uColor, pTexture, rtTexture, m_hWnd->m_eRS, m_hWnd->m_fZ );
			}

			return;
		}
	}

	CVector2f vStart( (float)GetWndLeft(), (float)GetWndTop() );
	CVector2f rtScreen[4] = { rt[0] + vStart, rt[1] + vStart, rt[2] + vStart, rt[3] + vStart };
#ifdef UI_HIDE_MODE
	if (m_hWnd->m_pGUI->GetHidenMode() && !IsModleState() )
	{
		return;
	}
#endif
	m_hWnd->m_pGUI->DrawRect( rtScreen, uColor, pTexture, rtText, m_hWnd->m_eRS, m_hWnd->m_fZ );
}

void SQRWnd::DrawRect( WND_IMAGE_LIST& ImageList, const CFRect* rt )
{
	DrawRect(ImageList, rt, 0.0f, 0.0f);
}
void SQRWnd::DrawRect( WND_IMAGE_LIST& ImageList, const CFRect* rt, float offsetX, float offsetY )
{
	OnProcess();
	if ( !(m_hWnd->m_bShow) )
	{
		return;
	}

	CFRect rtWnd = rt ? *rt : CFRect( 0, 0, (float)GetWndWidth(), (float)GetWndHeight() );
	CFRect rtWndBak = rtWnd;
	if ( GetStyle()&WS_CUTBYPARENT )
	{
		CFRect rtRootCutWnd = GetParentCutRect();
		ScreenToWnd( rtRootCutWnd );
		rtWnd = rtWnd.Intersection(rtRootCutWnd);
	}
	if ( m_pCutWnd )
	{
		CFRect rtCutWnd;
		m_pCutWnd->GetCutAreaRect(rtCutWnd);
		ScreenToWnd( rtCutWnd );
		rtWnd = rtWnd.Intersection(rtCutWnd);
	}
	if(rtWnd.IsEmpty())
	{
		return;
	}
	StyleStruct* pStyle = GetStyleStruct();
	CFRect	TempWnd;
	CFRect	TempTexture;
	float Zoom = m_hWnd->m_pGUI->GetZoomSize() /** m_WndZoom*/;
	uint32 Style = m_hWnd->m_StyleIndex;


	GUI_BLOCK* pBlock = NULL;
	if ( Style>0 && NULL != pStyle )
	{
		pBlock = pStyle->m_vecPool[m_ESMask][Style];
		if(pBlock == NULL)
			pBlock = pStyle->m_vecPool[m_ESMask][1];
	}

	if( Style == 0 || pBlock==NULL)
	{
		for ( int32 i = 0; i < ImageList.GetImageCount(); i++ )
		{
			IMAGE& image = ImageList.GetImage(i);
			CFRect rtImage((float)image.posWnd.x, (float)image.posWnd.y, image.posWnd.x + abs(image.rtTexture.Width()), image.posWnd.y + abs(image.rtTexture.Height()));
			rtImage.top		*= (Zoom * m_WndZoom);
			rtImage.left	*= (Zoom * m_WndZoom);
			rtImage.bottom	*= (Zoom * m_WndZoom);
			rtImage.right	*= (Zoom * m_WndZoom);
			CFRect rtDrawPic = rtWnd.Intersection(rtImage).Offset(offsetX, offsetY);
			if (rtImage.Width() == 0.0f || rtImage.Height() == 0.0f)
				return;
			CFRect rtText(
				( rtDrawPic.left   - rtImage.left )*image.rtTexture.Width() /rtImage.Width()  + image.rtTexture.left,
				( rtDrawPic.top    -  rtImage.top )*image.rtTexture.Height()/rtImage.Height() + image.rtTexture.top,
				( rtDrawPic.right  - rtImage.left )*image.rtTexture.Width() /rtImage.Width()  + image.rtTexture.left,
				( rtDrawPic.bottom -  rtImage.top )*image.rtTexture.Height()/rtImage.Height() + image.rtTexture.top);
			DrawRect(image.pTexture, rtDrawPic, image.dwColor, &rtText, false);
		}
		m_itX = 0;
		m_itY = 0;
	}
	else
	{
		//---------------------九宫格---------------------------------
		GUI_BLOCK& Block = *pBlock;

		float TopLeftH		= Block[GBM_TOP_LEFT].Height()* Zoom;
		float TopLeftW		= Block[GBM_TOP_LEFT].Width() * Zoom;
		float BottomRightW	= Block[GBM_BOTTOM_RIGHT].Width() * Zoom;
		float BottomRightH	= Block[GBM_BOTTOM_RIGHT].Height()* Zoom;
		float TopRightW		= Block[GBM_TOP_RIGHT].Width() * Zoom;
		float BottomLeftW	= Block[GBM_BOTTOM_LEFT].Width() * Zoom;
		TempTexture = Block[GBM_CENTER];
		TempWnd.top		= rtWndBak.top		+ TopLeftH;
		TempWnd.left	= rtWndBak.left	+ TopLeftW;
		TempWnd.right	= rtWndBak.right	- BottomRightW;
		TempWnd.bottom	= rtWndBak.bottom	- BottomRightH;
		TempWnd = TempWnd.Intersection(rtWnd);
		if ( !(TempWnd.IsEmpty()) )
			DrawRect(pStyle->pTexture, TempWnd, pStyle->m_dwColor, &TempTexture	, false);


		TempTexture = Block[GBM_TOP];
		TempWnd.top		= rtWndBak.top;
		TempWnd.bottom	= rtWndBak.top		+ TempTexture.Height() * Zoom;
		TempWnd.left	= rtWndBak.left	+ TopLeftW;
		TempWnd.right	= rtWndBak.right	- TopRightW;	
		TempWnd = TempWnd.Intersection(rtWnd);
		if ( !(TempWnd.IsEmpty()) )
			DrawRect(pStyle->pTexture, TempWnd, pStyle->m_dwColor, &TempTexture	, false);


		TempTexture = Block[GBM_BOTTOM];
		TempWnd.top		= rtWndBak.bottom	- TempTexture.Height() * Zoom;;
		TempWnd.bottom	= rtWndBak.bottom;
		TempWnd.left	= rtWndBak.left	+ BottomLeftW;
		TempWnd.right	= rtWndBak.right	- BottomRightW;
		TempWnd = TempWnd.Intersection(rtWnd);
		if ( !(TempWnd.IsEmpty()) )
			DrawRect(pStyle->pTexture, TempWnd, pStyle->m_dwColor, &TempTexture	, false);		

		TempTexture = Block[GBM_LEFT];
		TempWnd.top		= rtWndBak.top		+ TopLeftH;
		TempWnd.left	= rtWndBak.left;
		TempWnd.right	= rtWndBak.left	+ TempTexture.Width() * Zoom;;
		TempWnd.bottom	= rtWndBak.bottom	- BottomRightH;
		TempWnd = TempWnd.Intersection(rtWnd);
		if ( !(TempWnd.IsEmpty()) )
			DrawRect(pStyle->pTexture, TempWnd, pStyle->m_dwColor, &TempTexture	, false);

		TempTexture = Block[GBM_RIGHT];
		TempWnd.top		= rtWndBak.top		+ TopLeftH;
		TempWnd.left	= rtWndBak.right	- TempTexture.Width() * Zoom;;
		TempWnd.right	= rtWndBak.right;
		TempWnd.bottom	= rtWndBak.bottom	- BottomRightH;
		TempWnd = TempWnd.Intersection(rtWnd);
		if ( !(TempWnd.IsEmpty()) )
			DrawRect(pStyle->pTexture, TempWnd, pStyle->m_dwColor, &TempTexture	, false);

		TempTexture = Block[GBM_TOP_LEFT];
		TempWnd.top		= rtWndBak.top;
		TempWnd.bottom	= rtWndBak.top		+ TopLeftH;//TempTexture.Height();
		TempWnd.left	= rtWndBak.left;
		TempWnd.right	= rtWndBak.left	+ TopLeftW;//TempTexture.Width();
		TempWnd = TempWnd.Intersection(rtWnd);
		if ( !(TempWnd.IsEmpty()) )
			DrawRect(pStyle->pTexture, TempWnd, pStyle->m_dwColor, &TempTexture	, false);
		m_itX = TempTexture.Width();
		m_itY = TempTexture.Height();

		TempTexture = Block[GBM_TOP_RIGHT];
		TempWnd.top		= rtWndBak.top;
		TempWnd.bottom	= rtWndBak.top		+ TempTexture.Height() * Zoom;
		TempWnd.left	= rtWndBak.right	- TopRightW;//TempTexture.Width();
		TempWnd.right	= rtWndBak.right;	
		TempWnd = TempWnd.Intersection(rtWnd);
		if ( !(TempWnd.IsEmpty()) )
			DrawRect(pStyle->pTexture, TempWnd, pStyle->m_dwColor, &TempTexture	, false);

		TempTexture = Block[GBM_BOTTOM_LEFT];
		TempWnd.top		= rtWndBak.bottom	- TempTexture.Height() * Zoom;;
		TempWnd.bottom	= rtWndBak.bottom;
		TempWnd.left	= rtWndBak.left;
		TempWnd.right	= rtWndBak.left	+ BottomLeftW;//TempTexture.Width();
		TempWnd = TempWnd.Intersection(rtWnd);
		if ( !(TempWnd.IsEmpty()) )
			DrawRect(pStyle->pTexture, TempWnd, pStyle->m_dwColor, &TempTexture	, false);

		TempTexture = Block[GBM_BOTTOM_RIGHT];
		TempWnd.top		= rtWndBak.bottom	- BottomRightH;//TempTexture.Height();
		TempWnd.bottom	= rtWndBak.bottom;
		TempWnd.left	= rtWndBak.right	- BottomRightW;//TempTexture.Width();
		TempWnd.right	= rtWndBak.right;
		TempWnd = TempWnd.Intersection(rtWnd);
		if ( !(TempWnd.IsEmpty()) )
			DrawRect(pStyle->pTexture, TempWnd, pStyle->m_dwColor, &TempTexture	, false);				
	}	
}


void sqr::SQRWnd::DrawRect( const CFRect& rt, uint32 color, bool screenPos/* = false*/ )
{
	PointGroup point[5];
	CFRect rect = rt;
	if (screenPos == false) //将相对坐标转换为绝对坐标
	{
		rect.left	+= GetWndLeft();
		rect.right	+= GetWndLeft();
		rect.top	+= GetWndTop();
		rect.bottom += GetWndTop();
	}

	point[0].uColor = point[1].uColor = point[2].uColor = point[3].uColor = point[4].uColor = color;

	point[0].vPoint = CVector3f(rect.left, rect.top, 0);
	point[1].vPoint = CVector3f(rect.left, rect.bottom, 0);
	point[2].vPoint = CVector3f(rect.right, rect.bottom, 0);
	point[3].vPoint = CVector3f(rect.right, rect.top, 0);
	point[4].vPoint = CVector3f(rect.left, rect.top, 0);
	GetGraphic()->DrawLine( point, 5, true, m_hWnd->m_eRS );
}

void sqr::SQRWnd::DrawRect(CColor color /*= Color::Red*/)
{
	CFRect rect;
	GetWndRect(rect);
	float rootx = GetRootWnd()->GetWndHandle()->m_xScreen;
	float rooty = GetRootWnd()->GetWndHandle()->m_yScreen;
	rect.left		+= rootx;
	rect.right		+= rootx;
	rect.top		+= rooty;
	rect.bottom		+= rooty;
	this->DrawRect(rect, color, true);
}

void SQRWnd::DrawLine( const PointGroup* pPointGroup, int32 nNum, bool bScreenPos )
{
	FlushDraw();
	CFRect rcWnd;
	GetWndRect( rcWnd );
	CFRect rtScreen = rcWnd;
	const float zoomSize = m_hWnd->m_pGUI->GetZoomSize();
	PointGroup Points[2];
	for ( int32 i = 0; i < nNum - 1;  i++)
	{
		Points[0] = pPointGroup[i];
		Points[1] = pPointGroup[i+1];

		if ( !bScreenPos )
		{
			Points[0].vPoint.x += (float)rtScreen.left;
			Points[0].vPoint.x *= zoomSize;
			Points[0].vPoint.y += (float)rtScreen.top;
			Points[0].vPoint.y *= zoomSize;

			Points[1].vPoint.x += (float)rtScreen.left;
			Points[1].vPoint.x *= zoomSize;
			Points[1].vPoint.y += (float)rtScreen.top;
			Points[1].vPoint.y *= zoomSize;
		}

		if ( Cut2DLine( rtScreen, Points[0].vPoint, Points[1].vPoint ) )
		{
			GetGraphic()->DrawLine( Points, 2, true, m_hWnd->m_eRS );
		}
	}
}

void SQRWnd::ReDrawChildren()
{
	SQRWnd* ChildWnd = m_hWnd->m_LastChild;
	while ( ChildWnd )
	{
		ChildWnd->Draw();
		ChildWnd = ChildWnd->m_hWnd->m_PreBrother;
	}
}

void SQRWnd::DrawImeFx( uint curTime, float zoom )
{
	for (int32 i = 0; i < (int32)(m_Effect.size()); i++)
	{
		if ( m_FlushDlg == NULL )
			m_FlushDlg = GetFlushDialog();
		if ( m_FlushDlg )
			m_FlushDlg->GetFxRenderTask(true)->PushRenderStack(this, curTime, zoom);
		else
			FlashImeFx(curTime, zoom);
	}
}

void SQRWnd::FlashImeFx(uint curTime, float zoom)
{
	FlushDraw();
	for (int32 i = 0; i < (int32)(m_Effect.size()); i++)
	{
		UIString fx = m_Effect[i];
		bool succ = SQRGUIManager::GetGUI( this )->DrawImeFx( this, fx.c_str(), curTime, zoom );
		if (!succ)
		{
			EffectVec::iterator itr = m_Effect.begin();
			itr = itr + i;
			m_Effect.erase(itr);
			i--;
		}
	}
}

void SQRWnd::ShowWnd( bool bShow )
{
	struct _Show
	{
		_Show( SQRWnd* pWnd, bool bShow, bool igntex )
		{
			if ( pWnd->GetStyle() & WS_MODULE )
			{
				if ( bShow && pWnd->m_hWnd->m_bShow )
				{
					pWnd->m_hWnd->m_pGUI->SetModuleWnd( pWnd );
					pWnd->SetFocus();
				}
				else
				{
					pWnd->m_hWnd->m_pGUI->DeleteModuleWnd( pWnd );
				}
			}
			if( pWnd->GetClassName() == UIString( "SQREdit" ) && bShow == false && pWnd->m_hWnd->m_pGUI->IsFocus(pWnd))
				pWnd->SetActive();
			
#ifdef TEXTURE_STINT_MODE
			// 清空和创建材质
			if ( bShow && pWnd->m_hWnd->m_bShow )
				pWnd->CreatWndTex();
			else if ( !igntex )
				pWnd->ReleaseWndTex();
#endif
			if (pWnd->m_hWnd->m_bShow == true && bShow)
				pWnd->OnPlayWndFx( eMF_Open );
			else if (!bShow && pWnd->m_hWnd->m_bShow == true )
				pWnd->ClearImeFx();

			pWnd = pWnd->FindLastChild();
			while ( pWnd )
			{
				_Show( pWnd, bShow, igntex );
				pWnd = pWnd->FindPreWnd();
			}
		}
	};

	if ( m_hWnd->m_bShow != bShow )
	{
		m_hWnd->m_bShow = bShow;
		_Show( this, bShow, m_bIgnTex );
		OnWndShow( bShow );
		if (!bShow)
			ClearImeFx();

		///open or close effect is decided by bShow parameter
		OnPlayWndFx( bShow ? eMF_Open : eMF_Close );
		OnPlayWndSound( bShow ? eMS_Open : eMS_Close );
	}
}

void SQRWnd::ClearImeFx()
{
	// 将分层管理的特效转到全局特效区域
	for (size_t i = 0; i < m_Effect.size(); i++)
	{
		UIString fx = m_Effect[i];
		SQRGUIManager::GetGUI( this )->ClearImeFx( this, fx.c_str() );
	}
	m_Effect.clear();
}

void SQRWnd::ShowWndIgnTex( bool bShow )
{
	struct _Show
	{
		_Show( SQRWnd* pWnd, bool bShow )
		{
			if ( pWnd->GetStyle() & WS_MODULE )
			{
				if ( bShow && pWnd->m_hWnd->m_bShow )
				{
					pWnd->m_hWnd->m_pGUI->SetModuleWnd( pWnd );
					pWnd->SetFocus();
				}
				else
				{
					pWnd->m_hWnd->m_pGUI->DeleteModuleWnd( pWnd );
				}
			}

			if( pWnd->GetClassName() == UIString( "SQREdit" ) && bShow == false )
			{
				pWnd->SetActive();
			}
#ifdef TEXTURE_STINT_MODE
			pWnd->CreatWndTex();
#endif
			pWnd = pWnd->FindLastChild();
			while ( pWnd )
			{
				_Show( pWnd, bShow );
				pWnd = pWnd->FindPreWnd();
			}
		}
	};
	m_bIgnTex = true;
	if ( m_hWnd->m_bShow != bShow )
	{
		m_hWnd->m_bShow = bShow;
		_Show( this, bShow );
		OnWndShow( bShow );

		///open or close effect is decided by bShow parameter
		OnPlayWndFx( bShow ? eMF_Open : eMF_Close );
		OnPlayWndSound( bShow ? eMS_Open : eMS_Close );
	}
}

void  SQRWnd::SetTransparent( float fTrans )
{
	limit( fTrans, 0.0f, 1.0f );
	m_hWnd->m_fTransparent = fTrans;

	SQRWnd* pChildWnd = FindFirstChild();
	while ( pChildWnd != NULL )
	{
		pChildWnd->SetTransparent( fTrans );
		pChildWnd = pChildWnd->FindNextWnd();
	}
}

void SQRWnd::ReleaseWndTex()
{
	// 释放可以释放的UI贴图资源
	m_hWnd->m_Title.ReleaseTex();
	m_hWnd->m_Enable.ReleaseTex();
	m_hWnd->m_Disable.ReleaseTex();
	m_hWnd->m_MouseDesc.ReleaseTex();

	SafeRelease( m_hWnd->m_pTransparenTex );
}
void SQRWnd::CreatWndTex()
{
	// 显示之前申请资源
	if ( m_FlushDlg == NULL )
	{
		m_FlushDlg = GetFlushDialog();
		if ( m_FlushDlg == NULL ) return;
	}
	m_hWnd->m_Title.CreatTex( GetGraphic(), m_FlushDlg );
	m_hWnd->m_Enable.CreatTex( GetGraphic(), m_FlushDlg );
	m_hWnd->m_Disable.CreatTex( GetGraphic(), m_FlushDlg );
	m_hWnd->m_MouseDesc.CreatTex( GetGraphic(), m_FlushDlg );

	if(m_hWnd->m_strTransName.size()>0 && !(m_hWnd->m_pTransparenTex))
	{
		bool bEnableASynRead = CPkgFile::IsEnableAsyncRead();
		CPkgFile::EnableAsyncRead( false );
		GetGraphic()->CreateTexture( GUI_PATH_ALIAS_GUITEX.c_str(), (m_hWnd->m_strTransName).c_str(), &(m_hWnd->m_pTransparenTex) );
		CPkgFile::EnableAsyncRead( bEnableASynRead );
	}
}

SQRRichToolTip* SQRWnd::SetNewRichToolTip()
{
	SQRRichToolTip* ptempToolTip = GetRichToolTip();
	if ( ptempToolTip )
	{
		// 清理资源
		ptempToolTip->ClearChildToolTip();
		ptempToolTip->OnTipHide();
	}
	else
	{
		// 创建重量级的tooltip
		ptempToolTip = new SQRRichToolTip();
		ptempToolTip->SetParentWnd( this );
		m_hWnd->m_pRichToolTip = ptempToolTip;
	}
	RefreshToolTip();
	return ptempToolTip;
}

void SQRWnd::DelRichToolTip()
{
	GfkSafeDelete( m_hWnd->m_pRichToolTip );
}

void SQRWnd::SetWndText( const char* str )
{
	if (str == NULL) return;
	m_hWnd->m_WindowTitle = str;
	m_hWnd->m_WindowTitleW = utf8_to_utf16(m_hWnd->m_WindowTitle.c_str()).c_str();

	GetFont()->UpdateCharInfo(m_hWnd->m_WindowTitleW.c_str());
}

const char* SQRWnd::GetWndText()
{
	m_hWnd->m_WindowTitle = utf16_to_utf8(m_hWnd->m_WindowTitleW.c_str()).c_str();
	return m_hWnd->m_WindowTitle.c_str();
}

const char* SQRWnd::AppendWndText(const string& str)
{
	m_hWnd->m_WindowTitle.append(str.c_str());
	m_hWnd->m_WindowTitleW = utf8_to_utf16(m_hWnd->m_WindowTitle.c_str()).c_str();
	return m_hWnd->m_WindowTitle.c_str();
}
void SQRWnd::SetIsBottom(bool bIsBottom)
{
	this->bIsBottom = bIsBottom;
	SQRWnd* pChildWnd = m_hWnd->m_FirstChild;
	while ( pChildWnd )
	{
		pChildWnd->SetIsBottom( bIsBottom );
		pChildWnd = pChildWnd->m_hWnd->m_NextBrother;
	}
}

void SQRWnd::RefreshToolTip()
{
	m_hWnd->m_pGUI->SetRefreshToolTip();
}

void SQRWnd::SetLogicRect( const CFRect& rt )
{
	if (!m_hWnd->m_pGUI)
		return;
	CFRect srt = rt;
	float Zoom = m_hWnd->m_pGUI->GetZoomSize(); 
	srt.top		= srt.top		* Zoom;
	srt.bottom	= srt.bottom	* Zoom;
	srt.left	= srt.left		* Zoom;
	srt.right	= srt.right		* Zoom;
	SetWndRect( srt );
}

float SQRWnd::RefreshWndDis( float val )
{
	if (!m_hWnd->m_pGUI)
		return 0.0f;
	val = val * m_hWnd->m_pGUI->GetZoomSize();
	return val;
}

void SQRWnd::GetLogicRect( CFRect& rt )
{
	if (!m_hWnd->m_pGUI)
		return;
	GetWndRect(rt);
	float Zoom = m_hWnd->m_pGUI->GetZoomSize(); 
	float InvZoom = 1.0f/Zoom;
	rt.top		= (rt.top) * InvZoom;
	rt.bottom	= (rt.bottom) * InvZoom;
	rt.left		= (rt.left) * InvZoom;
	rt.right	= (rt.right) * InvZoom;
}

void SQRWnd::SetWndZoom( float zoom )
{
	m_WndZoom = zoom;
	SQRWnd* ChildWnd = m_hWnd->m_LastChild;
	while ( ChildWnd )
	{
		ChildWnd->SetWndZoom(zoom);
		ChildWnd = ChildWnd->m_hWnd->m_PreBrother;
	}
}

float SQRWnd::GetWndZoom( void )
{
	return m_WndZoom;
}

void SQRWnd::OnScreenPosChanged()
{
	WndHandle* pWndH = GetWndHandle();
	for ( SQRWnd* pChildWnd = pWndH->m_FirstChild
		; pChildWnd != NULL
		; pChildWnd = pChildWnd->m_hWnd->m_NextBrother)
	{
		pChildWnd->OnScreenPosChanged();
	}
}

void SQRWnd::SetWndRect( const CFRect& rt )
{
	if (m_bInShape)
		DelShapeEffect( this );
	_SetWndRect(rt);
}

void SQRWnd::_SetWndRect( const CFRect& rt )
{
	WndHandle* pWndH = GetWndHandle();
	if (!IsCreated())
		return;

	CFRect rtt = rt;
	RejustRect(rtt);

	float Zoom		= m_hWnd->m_pGUI->GetZoomSize() * m_WndZoom;
	CTextureFontMgr::Inst()->SetImgScale(Zoom);
	float InvZoom	= 1.0f/Zoom;
	
	float DeltaX =  rtt.left - pWndH->m_xScreenbak;
	float DeltaY =  rtt.top  - pWndH->m_yScreenbak;

	float DeltaW =  rtt.right - pWndH->m_Widthbak  - rtt.left;
	float DeltaH =  rtt.bottom - pWndH->m_Heightbak - rtt.top;

	FLOAT Ini_X = DeltaX * InvZoom;
	FLOAT Ini_Y = DeltaY * InvZoom;
	FLOAT Ini_W = DeltaW * InvZoom;
	FLOAT Ini_H = DeltaH * InvZoom;

	pWndH->m_xScreen = rtt.left;
	pWndH->m_yScreen = rtt.top;
	pWndH->m_Width	 = rtt.right - rtt.left;
	pWndH->m_Height	 = rtt.bottom - rtt.top;

	pWndH->m_xScreenbak = pWndH->m_xScreen;
	pWndH->m_yScreenbak = pWndH->m_yScreen;
	pWndH->m_Widthbak	= pWndH->m_Width;
	pWndH->m_Heightbak	= pWndH->m_Height;

	for (SQRWnd* pChildWnd = pWndH->m_FirstChild; pChildWnd != NULL; pChildWnd = pChildWnd->m_hWnd->m_NextBrother)
	{
		CFRect rect;
		Relock(rect, pWndH, pChildWnd, rtt, Zoom, false);
		pChildWnd->_SetWndRect(rect);
	}

	pWndH->m_xInitial		+= Ini_X;
	pWndH->m_yInitial		+= Ini_Y;
	pWndH->m_WidthInitial	+= Ini_W;
	pWndH->m_HeightInitial	+= Ini_H;
}

void SQRWnd::ResetWndPos(float x, float y)
{
	if ( m_bInShape )
		DelShapeEffect( this );
	_ResetWndPos(x, y);
}

void SQRWnd::_ResetWndPos(float x, float y)
{
	WndHandle* pWndH = GetWndHandle();

	CFRect rt;
	float disx = x - pWndH->m_xScreen;
	float disy = y - pWndH->m_yScreen;

	pWndH->m_xScreen = x;
	pWndH->m_yScreen = y;

	for ( SQRWnd* pChildWnd = pWndH->m_FirstChild
		; pChildWnd != NULL
		; pChildWnd = pChildWnd->m_hWnd->m_NextBrother)
	{
		WndHandle* ChildWndH = pChildWnd->GetWndHandle();
		float lx = ChildWndH->m_xScreen + disx;
		float ry = ChildWndH->m_yScreen + disy;

		pChildWnd->_ResetWndPos(lx, ry);
	}
}

void SQRWnd::ResizeRect( const CFRect& rt, bool refreshbak )
{
	if ( refreshbak && m_bInShape )
		DelShapeEffect( this );
	_ResizeRect( rt, refreshbak );
}

void SQRWnd::_ResizeRect( const CFRect& rt, bool refreshbak )
{
	WndHandle* pWndH = GetWndHandle();
	if (!IsCreated())
	{
		return;
	}
	
	CFRect rtt = rt;
	RejustRect(rtt);
	
	float Zoom = pWndH->m_pGUI->GetZoomSize() * m_WndZoom;

	pWndH->m_xScreen = rtt.left;
	pWndH->m_yScreen = rtt.top;
	pWndH->m_Width = rtt.right - rtt.left;
	pWndH->m_Height = rtt.bottom - rtt.top;

	if ( refreshbak )
	{
		pWndH->m_xScreenbak = pWndH->m_xScreen;
		pWndH->m_yScreenbak = pWndH->m_yScreen;
		pWndH->m_Widthbak = pWndH->m_Width;
		pWndH->m_Heightbak = pWndH->m_Height;
	}

	for (SQRWnd* pChildWnd = pWndH->m_FirstChild; pChildWnd != NULL; pChildWnd = pChildWnd->m_hWnd->m_NextBrother)
	{
		CFRect rect;
		Relock(rect, pWndH, pChildWnd, rtt, Zoom, true);
		pChildWnd->_ResizeRect(rect, refreshbak);
	}

	OnSized();
}

void SQRWnd::SetShapeEffect(int32 runtime, CFRect& src, CFRect& des)
{
	if ( !m_effHld )
		m_effHld = new SQRGUIEffectHandler();
	SQRGUIShapeEffect* T_eff = NULL;
	SQRGUIEffectBase* temp_eff = m_effHld->GetEffect(eEff_Shape);
	if ( !temp_eff )
	{
		T_eff = new SQRGUIShapeEffect(this);
		T_eff->SetShapeEffect(runtime, src, des);
		m_effHld->AddGUIEffect(this, T_eff);
	}
	else if (temp_eff->GetEffectType() == eEff_Shape)
	{
		T_eff = static_cast<SQRGUIShapeEffect*>(temp_eff);
		T_eff->SetShapeEffect(runtime, src, des);
	}
	SetInShape();
}

void SQRWnd::SetInShape()
{
	m_bInShape = true;
	
	SQRWnd* ParentWnd = m_hWnd->m_ParentWnd;
	while (ParentWnd)
	{
		ParentWnd->m_bInShape = true;
		ParentWnd = ParentWnd->m_hWnd->m_ParentWnd;
	}
}


void SQRWnd::DelShapeEffect( SQRWnd* Wnd )
{
	// 删除自己和子的形变效果
	if ( Wnd && Wnd->m_effHld )
			Wnd->m_effHld->DelGUIEffect(eEff_Shape);

	SQRWnd* ChildWnd = Wnd->m_hWnd->m_LastChild;
	while ( ChildWnd )
	{
		if ( ChildWnd->m_bInShape )
			DelShapeEffect(ChildWnd);
		ChildWnd = ChildWnd->m_hWnd->m_PreBrother;
	}
	Wnd->m_bInShape = false;
}

void SQRWnd::ResizeRectOneChild( const CFRect& rt, SQRWnd* pChild )
{
	WndHandle* pWndH = GetWndHandle();
	if (m_bInShape)
		DelShapeEffect( this );

	pWndH->m_xScreen = rt.left;
	pWndH->m_yScreen = rt.top;
	pWndH->m_Width	 = rt.right - rt.left;
	pWndH->m_Height  = rt.bottom - rt.top;

	pWndH->m_xScreenbak = rt.left;
	pWndH->m_yScreenbak = rt.top;
	pWndH->m_Widthbak	 = rt.right - rt.left;
	pWndH->m_Heightbak  = rt.bottom - rt.top;

	if (!pChild)
		return;

	float Zoom = m_hWnd->m_pGUI->GetZoomSize() * m_WndZoom;

	SQRWnd* pChildWnd = pChild;
	CFRect rect;
	Relock(rect, pWndH, pChildWnd, rt, Zoom, false);
	pChildWnd->_ResizeRect(rect);
}

void SQRWnd::GetWndRect( CFRect& rt )
{
	rt.left = GetWndLeft();
	rt.top = GetWndTop();	
	rt.right = GetWndLeft() + m_hWnd->m_Width;
	rt.bottom = GetWndTop() + m_hWnd->m_Height;
}

void SQRWnd::GetWndRectBak( CFRect& rt )	// 获取形变窗体实际大小
{
	rt.left = m_hWnd->m_xScreenbak;
	rt.top = m_hWnd->m_yScreenbak;
	rt.right = m_hWnd->m_xScreenbak + m_hWnd->m_Widthbak;
	rt.bottom = m_hWnd->m_yScreenbak + m_hWnd->m_Heightbak;
}

const CFRect& SQRWnd::WndToScreen( CFRect& rt )
{
	rt.left		+= GetWndLeft();
	rt.right	+= GetWndLeft();
	rt.top		+= GetWndTop();
	rt.bottom	+= GetWndTop();

	return rt;
}

const CFRect& SQRWnd::ScreenToWnd( CFRect& rt )
{
	rt.left		-= GetWndLeft();
	rt.right	-= GetWndLeft();
	rt.top		-= GetWndTop();
	rt.bottom	-= GetWndTop();

	return rt;
}

void SQRWnd::SetMouseOverRichTextImageForAllWnd( int32 nIndex, WND_IMAGE_LIST& ImageList )
{
	m_hWnd->m_DescRichText->InsertImage( nIndex, ImageList );
}

bool SQRWnd::FindParent( SQRWnd* pParent )
{
	SQRWnd* pCurParent = GetParent();
	while ( pCurParent )
	{
		if ( pCurParent == pParent )
			return true;
		pCurParent = pCurParent->GetParent();
	}
	return false;
}

SQRWnd* SQRWnd::InserBefore( SQRWnd* pWnd )
{
	SQRWnd* pParent = GetParent();
	if ( !pParent || pWnd == this )
		return NULL;
	if ( pWnd && pWnd == FindNextWnd() )
		return NULL;

	//先脱离关系
	if ( this == pParent->m_hWnd->m_LastChild )
		pParent->m_hWnd->m_LastChild = m_hWnd->m_PreBrother;
	else if ( m_hWnd->m_NextBrother )
		m_hWnd->m_NextBrother->m_hWnd->m_PreBrother = m_hWnd->m_PreBrother;

	if ( this == pParent->m_hWnd->m_FirstChild )
		pParent->m_hWnd->m_FirstChild = m_hWnd->m_NextBrother;
	else if ( m_hWnd->m_PreBrother )
		m_hWnd->m_PreBrother->m_hWnd->m_NextBrother = m_hWnd->m_NextBrother;

	//加入
	m_hWnd->m_ParentWnd = pParent = pWnd ? pWnd->GetParent() : pParent;

	//寻找合适的插入点
	if ( !( GetStyle()&WS_TOPMOST ) )
		while ( pWnd && pWnd != this && ( pWnd->GetStyle()&WS_TOPMOST ) )
			pWnd = pWnd->m_hWnd->m_NextBrother;

	m_hWnd->m_PreBrother = pWnd ? pWnd->m_hWnd->m_PreBrother : pParent->m_hWnd->m_LastChild;
	m_hWnd->m_NextBrother = pWnd;

	if ( m_hWnd->m_PreBrother )
		m_hWnd->m_PreBrother->m_hWnd->m_NextBrother = this;
	else
		pParent->m_hWnd->m_FirstChild = this;

	if ( !pWnd )
		pParent->m_hWnd->m_LastChild = this;
	else
		pWnd->m_hWnd->m_PreBrother = this;

	return m_hWnd->m_PreBrother;
}

SQRWnd* SQRWnd::GetTopWnd()
{
	return m_hWnd->m_pGUI->GetTopWnd();
}

const char* SQRWnd::GetZOrder()
{
	static UIString StackValue;
	static UIString ReturnValue;

	StackValue.clear();
	ReturnValue.clear();
	SQRWnd* pWnd = this;
	while ( pWnd )
	{
		StackValue.push_back( (uint8)GetWndZOrder( pWnd ) );
		pWnd = pWnd->GetParent();
	}

	for ( int32 i = StackValue.size() - 1; i >= 0; i-- )
		ReturnValue.push_back( StackValue[i] );

	return ReturnValue.c_str();
}

void SQRWnd::RegisterHook( IHook* pHook, uint32 uWndMsg )
{
	return SQRGUIManager::RegisterHook( pHook, uWndMsg );
}

void SQRWnd::UnRegisterHook( IHook* pHook )
{
	return SQRGUIManager::UnRegisterHook( pHook );
}
void SQRWnd::RegisterAccelerator( IAccelerator* pAccelerator, uint32 uWndMsg )
{
	return SQRGUIManager::RegisterAccelerator( pAccelerator, uWndMsg );
}

void SQRWnd::UnRegisterAccelerator( IAccelerator* pAccelerator )
{
	return SQRGUIManager::UnRegisterAccelerator( pAccelerator );
}
void SQRWnd::AddHookMsg( IHook* pHook, uint32 uWndMsg )
{
	return m_hWnd->m_pGUI->AddHookMsg( pHook, uWndMsg );
}

bool SQRWnd::TestHook( IHook* pHook )
{
	return false;
}

void SQRWnd::SetWndDebugInfo(const char* info)
{
	debugWndInfo = info;
}

void SQRWnd::GetWndDebugInfo(string& str)
{
	if (debugWndInfo.empty())
	{
		SQRWnd* pParendWnd = m_hWnd->m_ParentWnd;
		if ( pParendWnd )
		{
			string info;
			pParendWnd->GetWndDebugInfo(info);
			if (info.empty())
			{
				if ( pParendWnd->GetWndHandle() )
					info = pParendWnd->GetWndHandle()->m_WindowName.c_str();
			}
			info = "Parent info:" + info;
			debugWndInfo = info.c_str();
		}
		debugWndInfo += "\nWndInfo:";
		debugWndInfo +=  GetWndHandle()->m_WindowName;
	}
	str = debugWndInfo.c_str();
}

SQRWnd* SQRWnd::GetDlgChild( const char* szChildName )
{
	
	SQR_TRY
	{
		SQRWnd* pChildWnd = m_hWnd->m_FirstChild;
		while ( pChildWnd )
		{

			UIString sWndName = pChildWnd->m_hWnd->m_WindowName;
			if (  pChildWnd->m_hWnd->m_WindowName == szChildName )
			{
				return pChildWnd;
			}
			pChildWnd = pChildWnd->m_hWnd->m_NextBrother;
		}

		return NULL;
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;
	
	return NULL;
}

bool SQRWnd::AssignDlgChildToWnd( SQRWnd& Wnd, const wchar_t* szChildName )
{
	if ( Wnd.IsCreated() )
		return false;

	SQRWnd* pWnd = NULL;

	if ( !pWnd )
	{
		SQRWnd* pChildWnd = m_hWnd->m_FirstChild;
		while ( pChildWnd )
		{
			if (  pChildWnd->m_hWnd->m_WindowNameW == szChildName )
			{
				pWnd = pChildWnd;
				break;
			}
			pChildWnd = pChildWnd->m_hWnd->m_NextBrother;
		}
	}

	if ( pWnd == NULL || Wnd.FindParentClass( pWnd->GetClassName() ) == FALSE )
		return false;

	CFRect rt;
	pWnd->GetWndRect( rt );
	ScreenToWnd( rt );
	float Zoom = m_hWnd->m_pGUI->GetZoomSize(); 
	float InvZoom = 1.0f/Zoom;
	rt = rt * InvZoom;

	WndCreateParam param;
	param.szTitle = pWnd->GetWndText();
	param.uStyle = pWnd->GetStyle();
	param.x = rt.left;
	param.y = rt.top;
	param.width = rt.right - rt.left;
	param.height = rt.bottom - rt.top;
	param.pParentWnd = this;
	param.nFontID = pWnd->GetFontID();
	param.font_size = pWnd->GetFontSize()/CFontCfg::GetInst()->GetFontZoom();
	param.uGroupID = pWnd->GetGroupID();
	if ( FAILED( Wnd.Create( param ) ) )
		return false;

	IMAGE_PARAM IP;
	for ( IP.CtrlMask = 1; ; IP.CtrlMask++ )
	{
		if ( pWnd->GetResWnd( IP.CtrlMask ) == NULL )
			break;

		if ( IP.CtrlMask > 1 )
		{
			CFRect rt;
			pWnd->GetResWnd( IP.CtrlMask )->GetWndRect( rt );
			Wnd.GetResWnd( IP.CtrlMask )->SetWndRect( rt );
		}

		for ( IP.StateMask = IP_ENABLE; IP.StateMask < IP_COUNT; IP.StateMask++ )
		{
			WND_IMAGE_LIST* pImage = pWnd->GetWndBkImage( IP );
			Wnd.SetWndTextColor( IP, pWnd->GetWndTextColor( IP ) );
			Wnd.SetWndTextBcColor( IP, pWnd->GetWndTextBcColor( IP ) );
			if ( pImage )
				Wnd.SetWndBkImage( IP, *pImage );
		}
	}

	Wnd.SetLockFlag( pWnd->GetLockFlag() );
	Wnd.SetFontGradualColor( pWnd->GetFontGradualColor() );
	Wnd.SetFontEffectMask( pWnd->GetFontEffectMask() );
	Wnd.SetIsDrawBackground( pWnd->IsDrawBackground() );

	Wnd.SetMouseOverDesc( pWnd->GetMouseOverDesc() );
	Wnd.m_hWnd->m_WindowNameW = szChildName;
	Wnd.m_hWnd->m_WindowName = utf16_to_utf8(szChildName).c_str();
	Wnd.m_hWnd->m_pResParam = pWnd->m_hWnd->m_pResParam;
	if ( Wnd.m_hWnd->m_pResParam )
		Wnd.OnResCreated( Wnd.m_hWnd->m_pResParam->m_UseInfo );

	CFRect rtParentWnd;
	Wnd.m_hWnd->m_ParentWnd->GetWndRect(rtParentWnd);
	Wnd.m_hWnd->m_ParentWnd->ResizeRectOneChild(rtParentWnd,&Wnd);

	if ( (pWnd)->m_hWnd->m_bNewByGUISys )
		delete pWnd;
	else
		pWnd->DestroyWnd();

	return true;
}

bool SQRWnd::IsChild( SQRWnd* pWnd )
{
	if ( !m_hWnd->m_pGUI )
		return false;
	SQRWnd* ChildWnd = m_hWnd->m_LastChild;
	while ( ChildWnd )
	{
		if ( ChildWnd == pWnd )
			return true;
		ChildWnd =ChildWnd->m_hWnd->m_PreBrother;
	}
	return false;
}

SQRWnd* SQRWnd::GetLastMouseMsgWnd()
{
	if ( m_hWnd->m_pGUI != NULL )
	{
		SQRWnd* pWnd = m_hWnd->m_pGUI->GetLastMouseMsgWnd();
		if ( m_hWnd->m_pGUI->FindWnd( pWnd ) )
			return pWnd;
	}
	return NULL;
}

void SQRWnd::SetStyle( uint32 Style )
{
	if ( ( Style & WS_MODULE ) && ( m_hWnd->m_WndStyle & WS_MODULE ) == 0 )
		m_hWnd->m_pGUI->SetModuleWnd( this );
	else if ( ( m_hWnd->m_WndStyle & WS_MODULE ) && ( Style & WS_MODULE ) == 0 )
		m_hWnd->m_pGUI->DeleteModuleWnd( this );

	m_hWnd->m_WndStyle = Style;

	if (m_hWnd->m_WndStyle & WS_MODULE)
		bNotModule = false;
	else
		bNotModule = true;
}

void SQRWnd::SetWndBkImage( const IMAGE_PARAM& ImageFlag, const WND_IMAGE_LIST& Image )
{
	WND_IMAGE_LIST* pImage = GetWndBkImage( ImageFlag );

	if ( !pImage )
		return;

	*pImage = Image;
}

void SQRWnd::SetWndTextColor( const IMAGE_PARAM& ImageFlag, uint32 uColor )
{
	if ( IP_ENABLE == ImageFlag.StateMask )
		m_hWnd->m_EnableColor = uColor;
	else if ( IP_DISABLE == ImageFlag.StateMask )
		m_hWnd->m_DisableColor = uColor;
}

void SQRWnd::SetWndTextBcColor( const IMAGE_PARAM& ImageFlag, uint32 uColor )
{
	if ( IP_ENABLE == ImageFlag.StateMask )
		m_hWnd->m_EnableBcColor = uColor;
	else if ( IP_DISABLE == ImageFlag.StateMask )
		m_hWnd->m_DisableBcColor = uColor;
}

bool SQRWnd::SetFont(int32 nFontID, bool bBold, float fSize )
{
	if ( nFontID == CFontMgr::INVALID_FONT_ID )
		nFontID = 0;
	if (!m_hWnd->m_pGUI)
		return false;
	CRenderFont* pFont = NULL;
	if ( (fSize != 0.0f) && FAILED( GetGraphic()->CreateFont( nFontID, bBold, fSize, &pFont ) ) )
		return false;
	m_hWnd->m_pGUI->ClearFontCatch(m_hWnd->m_WndFont);
	SafeRelease( m_hWnd->m_WndFont );
	m_hWnd->m_WndFont = pFont;
	m_hWnd->m_FontSize = fSize;

	return true;
}

SQRWnd* SQRWnd::SetParent( SQRWnd* pParent )
{
	SQRWnd* pPreParent = m_hWnd->m_ParentWnd;
	if ( pPreParent == pParent )
		return pParent;

	if ( pParent->FindFirstChild() )
	{
		InserBefore( pParent->FindFirstChild() );
	}
	else
	{
		if ( pPreParent ) //先脱离关系
		{
			if ( this == pPreParent->m_hWnd->m_LastChild )
				pPreParent->m_hWnd->m_LastChild = m_hWnd->m_PreBrother;
			else if ( m_hWnd->m_NextBrother )
				m_hWnd->m_NextBrother->m_hWnd->m_PreBrother = m_hWnd->m_PreBrother;

			if ( this == pPreParent->m_hWnd->m_FirstChild )
				pPreParent->m_hWnd->m_FirstChild = m_hWnd->m_NextBrother;
			else if ( m_hWnd->m_PreBrother )
				m_hWnd->m_PreBrother->m_hWnd->m_NextBrother = m_hWnd->m_NextBrother;
		}

		pParent->m_hWnd->m_FirstChild = this;
		pParent->m_hWnd->m_LastChild = this;
		m_hWnd->m_PreBrother = NULL;
		m_hWnd->m_NextBrother = NULL;
		m_hWnd->m_ParentWnd = pParent;
	}

	m_FlushDlg = GetFlushDialog();
	return pPreParent;
}

bool SQRWnd::IsHasEventState(EventStateMask Mask)
{
	if(	IP_MASK		== Mask ||
		IP_ENABLE	== Mask ||
		IP_DISABLE	== Mask ||
		IP_MOUSEDESC == Mask )
		return true;
	return false;
}

bool SQRWnd::ObjHasEventState(EventStateMask Mask)
{
	return SQRWnd::IsHasEventState(Mask);
}

WND_IMAGE_LIST* SQRWnd::GetWndBkImage(const IMAGE_PARAM& ImageFlag)
{
	if ( ImageFlag.CtrlMask != SM_BS_BK )
		return NULL;
	else if ( IP_MASK == ImageFlag.StateMask )
		return &m_hWnd->m_Title;
	else if ( IP_ENABLE == ImageFlag.StateMask )
		return &m_hWnd->m_Enable;
	else if ( IP_DISABLE == ImageFlag.StateMask )
		return &m_hWnd->m_Disable;
	else if ( IP_MOUSEDESC == ImageFlag.StateMask )
		return &m_hWnd->m_MouseDesc;
	else
		return NULL;
}

uint32 SQRWnd::GetWndTextColor(const IMAGE_PARAM& ImageFlag)
{
	if ( ImageFlag.CtrlMask != SM_BS_BK )
		return 0;
	else if ( IP_MASK == ImageFlag.StateMask )
		return 0;
	else if ( IP_ENABLE == ImageFlag.StateMask )
		return m_hWnd->m_EnableColor;
	else if ( IP_DISABLE == ImageFlag.StateMask )
		return m_hWnd->m_DisableColor;
	else
		return 0;
}

uint32 SQRWnd::GetWndTextBcColor( const IMAGE_PARAM& ImageFlag )
{
	if ( ImageFlag.CtrlMask != SM_BS_BK )
		return 0;
	else if ( IP_MASK == ImageFlag.StateMask )
		return 0;
	else if ( IP_ENABLE == ImageFlag.StateMask )
		return m_hWnd->m_EnableBcColor;
	else if ( IP_DISABLE == ImageFlag.StateMask )
		return m_hWnd->m_DisableBcColor;
	else
		return 0;
}

void SQRWnd::SetWndCursor( const char* szCursorFile )
{
	m_hWnd->m_hCursor = m_hWnd->m_pGUI->LoadCursorRes( szCursorFile );
}

const char* SQRWnd::GetWndCursor()
{
	return m_hWnd->m_pGUI->GetCursorFileName( m_hWnd->m_hCursor );
}

void SQRWnd::SetGlobCursor( const char* szCursorFile )
{
	m_hWnd->m_pGUI->SetGlobalCursor( m_hWnd->m_pGUI->LoadCursorRes( szCursorFile ) );
}

const char* SQRWnd::GetGlobCursor()
{
	return m_hWnd->m_pGUI->GetCursorFileName( m_hWnd->m_pGUI->GetGlobalCursor() );
}

void SQRWnd::UpdateGlobCursor(SQRWnd &Wnd)
{
	m_hWnd->m_pGUI->UpdateCursor( Wnd);
}


CFRect SQRWnd::GetParentCutRect()
{
	SQRWnd* pParent = GetParent();
	CFRect parentRect;
	pParent->GetCutAreaRect(parentRect);
	CFRect rt(parentRect);

	if ( ( pParent->GetStyle() ) & WS_CUTBYPARENT )
	{
		rt = rt.Intersection(pParent->GetParentCutRect());
		return rt;
	}
	else
		return parentRect;
}

void SQRWnd::GetCutAreaRect( CFRect& rt )
{
	GetWndRect(rt);
}


const CFPos& SQRWnd::GetCursorPos()
{
	return m_hWnd->m_pGUI->GetCursorPos();
}

const CFPos& SQRWnd::GetLogicCursorPos()
{
	return m_hWnd->m_pGUI->GetLogicCursorPos();
}

IGraphic* SQRWnd::GetGraphic()
{
	return CGraphic::GetInst();
}

IEffectManager* SQRWnd::GetEffectManager()
{
	return m_hWnd->m_pGUI->GetEffectManager();
}

SQRRootWnd* SQRWnd::GetRootWnd()
{
	if (!(m_hWnd->m_pGUI))
		return NULL;
	return m_hWnd->m_pGUI->GetRootWnd();
}

SQRWnd* SQRWnd::GetModuleWnd()
{
	if (!(m_hWnd->m_pGUI))
		return NULL;
	return m_hWnd->m_pGUI->GetModuleWnd();
}

bool SQRWnd::IsFocus()
{
	return m_hWnd->m_pGUI->IsFocus( this );
}

bool SQRWnd::IsLastMouseMsgWnd()
{
	return ( m_hWnd->m_pGUI != NULL && m_hWnd->m_pGUI->GetLastMouseMsgWnd() ==this );
}

uint32 SQRWnd::GetCurMessage()
{
	return m_hWnd->m_pGUI->GetCurMessage();
}

const bool SQRWnd::GetHaveEditActive()
{
	return m_hWnd->m_pGUI->GetIsEditActive();
}

void SQRWnd::SetActive()
{
	m_hWnd->m_pGUI->SetActive( *this, false );
}

void SQRWnd::SetFocus()
{
	m_hWnd->m_pGUI->SetActive( *this, true );
}

void SQRWnd::FlushRect()
{
	m_hWnd->m_pGUI->FlushRect();
}

void SQRWnd::FlushDraw()
{
	m_hWnd->m_pGUI->FlushDraw();
}

void SQRWnd::Draw()
{
	if ( m_hWnd->m_bShow )
	{
#ifdef TEXTURE_STINT_MODE
		if (!(m_hWnd->m_bTexInit))
		{
			m_hWnd->m_bTexInit = true;
			CreatWndTex();
		}
#endif

		DrawWndBackground();
		DrawWndText();

		if (m_hWnd->m_bShow)
		{
			uint32 tProTime = uint32(GetProcessTime());
			DrawImeFx(tProTime, GetRootZoomSize()*m_WndZoom);

			SQRWnd* ChildWnd = m_hWnd->m_LastChild;
			while ( ChildWnd )
			{
				if ( ChildWnd->bNotModule )
					ChildWnd->Draw();
				ChildWnd = ChildWnd->m_hWnd->m_PreBrother;
			}
			DrawAfterChildren();
		}
	}
}


uint32	SQRWnd::GetStyleIndex()
{
	return m_hWnd->m_StyleIndex;
}

void SQRWnd::SetStyleIndex( uint32 index )
{
	m_hWnd->m_StyleIndex = index;
}

const char* SQRWnd::GetszSourceName()
{
	return m_hWnd->m_szSource.c_str();
}

void SQRWnd::SetszSourceName( const char* szDesc )
{
	m_hWnd->m_szSource = szDesc ? szDesc : "";
}

float SQRWnd::GetRootZoomSize(void)
{
	return m_hWnd->m_pGUI->GetZoomSize();
}

const char* SQRWnd::GetszClassName()
{
	return m_hWnd->m_szClassName.c_str();
}

void SQRWnd::SetszClassName( const char* szDesc )
{
	m_hWnd->m_szClassName = szDesc ? szDesc : "";
}

const char* SQRWnd::GetszCreateInfo()
{
	return m_hWnd->m_szCreateInfo.c_str();
}

const char* SQRWnd::GetszWindowName()
{
	return m_hWnd->m_szWindowName.c_str();
}

void SQRWnd::SetszCreateInfo( const char* szDesc )
{
	m_hWnd->m_szCreateInfo = szDesc ? szDesc : "";
}

void SQRWnd::SetszWindowName( const char* szDesc )
{
	m_hWnd->m_szWindowName = szDesc ? szDesc : "";
}

const char* SQRWnd::GetMouseOverDesc()
{
	m_hWnd->m_DescRichText->m_szDesc = utf16_to_utf8(m_hWnd->m_DescRichText->m_szDescW.c_str()).c_str();
	return m_hWnd->m_DescRichText->m_szDesc.c_str();
}

void SQRWnd::SetMouseOverDesc( const char* szDesc )
{
	if (!m_hWnd->m_DescRichText)
		return;
	m_hWnd->m_DescRichText->m_szDesc = szDesc ? szDesc : "";
	m_hWnd->m_DescRichText->m_szDescW = utf8_to_utf16(m_hWnd->m_DescRichText->m_szDesc.c_str()).c_str();
	m_hWnd->m_DescRichText->ClearChildToolTip();
	DelRichToolTip();
}

//此函數用來設置tooltips，如果調用此函數，那么時時檢測到的窗口是mouse所在位置的窗口
void SQRWnd::SetMouseOverDescAfter( const char* szDesc )
{
	if(!m_hWnd->m_DescRichText)
		return;
	string errmsg;
	SQR_TRY
	{
		errmsg = szDesc ? szDesc : "";
		errmsg += "\n";
		errmsg += m_hWnd->m_WindowName.c_str();
		m_hWnd->m_DescRichText->m_szDesc = szDesc ? szDesc : "";
		m_hWnd->m_DescRichText->m_szDescW = utf8_to_utf16(m_hWnd->m_DescRichText->m_szDesc.c_str()).c_str();
		m_hWnd->m_DescRichText->ClearChildToolTip();
		SetDetectMouseOverWnd( true );
		DelRichToolTip();
	}
	SQR_CATCH(exp)
	{
		exp.AppendMsg(errmsg);
		GfkLogExp(exp);
	}
	SQR_TRY_END
}

void SQRWnd::SetAlignToPixel(bool bAlignToPixel)
{
	m_bAlignToPixel = bAlignToPixel;
}

int32 SQRWnd::GetFontID()
{
	return m_hWnd->m_WndFont ? m_hWnd->m_WndFont->GetFontID() : CFontMgr::INVALID_FONT_ID;
}

void SQRWnd::RejustRect(CFRect& rt)
{
	limit(rt.left, -MAX_SCROLL_SIZE, MAX_SCROLL_SIZE-1);
	limit(rt.right, -MAX_SCROLL_SIZE+1, MAX_SCROLL_SIZE);
	limit(rt.top, -MAX_SCROLL_SIZE, MAX_SCROLL_SIZE-1);
	limit(rt.bottom, -MAX_SCROLL_SIZE+1, MAX_SCROLL_SIZE);
}

void SQRWnd::Relock(CFRect& rect, WndHandle* pWndH, SQRWnd* pChildWnd, const CFRect& rt, float Zoom,bool bTestIsGUITool)
{
	WndHandle* ChildWndH = pChildWnd->GetWndHandle();
	const uint32 lockFlag = ChildWndH->m_uLockFlage;
	// 水平位置计算
	if ((lockFlag & LOCK_UNZOOM) && (lockFlag & LOCK_RIGHT) && (lockFlag & LOCK_LEFT))
	{
		float fLeft		= GetWndLeft() + ((ChildWndH->m_xInitial - pWndH->m_xInitial) * Zoom);
		float fRight	= rt.right - ((pWndH->m_xInitial + pWndH->m_WidthInitial - ChildWndH->m_xInitial - ChildWndH->m_WidthInitial)* Zoom);
		float fMid		= (fLeft + fRight)/2;
		float fWidth	= ChildWndH->m_WidthInitial* Zoom;
		rect.left		= fMid - fWidth/2;
		rect.right		= fMid + fWidth/2;
	}
	else if ((lockFlag & LOCK_RIGHT) && (lockFlag & LOCK_LEFT))
	{
		rect.left	= GetWndLeft() + ((ChildWndH->m_xInitial - pWndH->m_xInitial) * Zoom);
		rect.right	= rt.right - ((pWndH->m_xInitial + pWndH->m_WidthInitial - ChildWndH->m_xInitial - ChildWndH->m_WidthInitial)* Zoom);
	}
	else if (lockFlag & LOCK_LEFT)
	{
		rect.left	= GetWndLeft() + ((ChildWndH->m_xInitial - pWndH->m_xInitial) * Zoom);
		rect.right	= rect.left + (ChildWndH->m_WidthInitial* Zoom);
	}
	else if (lockFlag & LOCK_RIGHT)
	{
		rect.right	=  rt.right - ((pWndH->m_xInitial + pWndH->m_WidthInitial - ChildWndH->m_xInitial - ChildWndH->m_WidthInitial)* Zoom);
		rect.left	= rect.right - (ChildWndH->m_WidthInitial * Zoom);
	}
	else
	{
		if (bTestIsGUITool && pWndH->m_pGUI->GetBeGUITool())
		{
			rect.left	= ChildWndH->m_xInitial;
			rect.right	= ChildWndH->m_xInitial + ChildWndH->m_WidthInitial;
		}
		else
		{
			rect.left	= GetWndLeft() + (pWndH->m_Width - (ChildWndH->m_WidthInitial * Zoom))/2.0f;
			rect.right	= rect.left + (ChildWndH->m_WidthInitial * Zoom);
		}
	}

	// 垂直位置计算
	if ((lockFlag & LOCK_UNZOOM) && (lockFlag & LOCK_TOP) && (lockFlag & LOCK_BOTTOM))
	{
		float fTop		= GetWndTop() + ((ChildWndH->m_yInitial - pWndH->m_yInitial) * Zoom);
		float fBottom	= rt.bottom -  ((pWndH->m_yInitial + pWndH->m_HeightInitial - ChildWndH->m_yInitial - ChildWndH->m_HeightInitial)* Zoom);
		float fMid		= (fTop + fBottom)/2;
		float fHeight	= ChildWndH->m_HeightInitial* Zoom;
		rect.top		= fMid - fHeight/2;
		rect.bottom		= fMid + fHeight/2;
	}
	else if ((lockFlag & LOCK_TOP) && (lockFlag & LOCK_BOTTOM))
	{
		rect.top	= GetWndTop() + ((ChildWndH->m_yInitial - pWndH->m_yInitial) * Zoom);
		rect.bottom = rt.bottom -  ((pWndH->m_yInitial + pWndH->m_HeightInitial - ChildWndH->m_yInitial - ChildWndH->m_HeightInitial)* Zoom);
	}
	else if (lockFlag & LOCK_TOP)
	{
		rect.top	= GetWndTop() + ((ChildWndH->m_yInitial - pWndH->m_yInitial) * Zoom);
		rect.bottom = rect.top + (ChildWndH->m_HeightInitial* Zoom);
	}
	else if (lockFlag & LOCK_BOTTOM)
	{
		rect.bottom = rt.bottom - ((pWndH->m_yInitial + pWndH->m_HeightInitial - ChildWndH->m_yInitial - ChildWndH->m_HeightInitial) * Zoom);
		rect.top	= rect.bottom - (ChildWndH->m_HeightInitial * Zoom);
	}
	else
	{
		if (bTestIsGUITool && pWndH->m_pGUI->GetBeGUITool())
		{
			rect.top	= ChildWndH->m_yInitial;
			rect.bottom = ChildWndH->m_yInitial + ChildWndH->m_HeightInitial;
		}
		else
		{
			rect.top	= GetWndTop() + (pWndH->m_Height - (ChildWndH->m_HeightInitial* Zoom))/2.0f;
			rect.bottom = rect.top + (ChildWndH->m_HeightInitial * Zoom);
		}
	}
}
