#include "stdafx.h"
#include "SQRGUIManager.h"
#include "SQRDialog.h"
#include "CPkgFile.h"
#include "CodeCvs.h"
#include "TSqrAllocator.inl"
#include "SQRFlashLayer.h"


DYNAMIC_CREATE_WND( SQRDialog, SQRWnd )
SQRDialog::SQRDialog()
: m_bWaitHide(false)
, m_bOptimize(true)
, m_bFlashed(false)
, m_bAutoFlushWnd(false)
, m_bAsynLoad(false)
, m_bValid(true)
, m_bCanDraw(true)
{
	for (size_t i = 0; i < EDLG_FLUSH_COUNT; i++)
		m_DlgRenderTask[i] = NULL;
}

SQRDialog::~SQRDialog()
{
	for (size_t i = 0; i < EDLG_FLUSH_COUNT; i++)
	{
		if (m_DlgRenderTask[i] != NULL) delete m_DlgRenderTask[i];
		m_DlgRenderTask[i] = NULL;
	}
}

int32	SQRDialog::DestroyWnd()
{
	TexPool::iterator itr = m_TexMap.begin();
	TexPool::iterator itrend = m_TexMap.end();

	while(itr != itrend)
	{
		SafeRelease(*itr);
		++itr;
	}
	return SQRWnd::DestroyWnd();
}

////////////////////////////////////////////////////////////////////////
// 功能：创建窗口
// 参数：const char* szWindowNameInResFile 资源文件中窗口名字
// 返回：>=0 成功，<0 失败
////////////////////////////////////////////////////////////////////////

void  SQRDialog::ResizeRect( const CFRect& rt, bool refreshbak  )
{
	SQRWnd::ResizeRect(rt,refreshbak);
}

void  SQRDialog::SetWndRect( const CFRect& rt )
{
	SQRWnd::SetWndRect(rt);
}

void  SQRDialog::_ResizeRect( const CFRect& rt, bool refreshbak  )
{
	SQRWnd::_ResizeRect(rt,refreshbak);
}

void  SQRDialog::_SetWndRect( const CFRect& rt )
{
	SQRWnd::_SetWndRect(rt);
}

int32 SQRDialog::CreateFromRes( const char* szWindowNameInResFile, SQRWnd* ParentWnd )
{
	m_bOptimize = true;
	m_bAsynLoad = true;
	if (m_DlgRenderTask[EDLG_FLUSH_TEX] == NULL)
		m_DlgRenderTask[EDLG_FLUSH_TEX] = new TextRenderTask();
	TextRenderTask* pTextRT = static_cast<TextRenderTask*>(m_DlgRenderTask[EDLG_FLUSH_TEX]);
	
	if (!ParentWnd)
		return -1;
	pTextRT->SetFlushDlg( this );
	return _CreateFromRes(szWindowNameInResFile, ParentWnd);
}

int32 SQRDialog::CreateFromResEx( const char* szWindowNameInResFile, SQRWnd* ParentWnd, bool merged, bool bAsyn )
{
	m_bOptimize = !merged;
	m_bAsynLoad = bAsyn;
	if ( m_bOptimize )
	{
		if (m_DlgRenderTask[EDLG_FLUSH_TEX] == NULL)
			m_DlgRenderTask[EDLG_FLUSH_TEX] = new TextRenderTask();
		TextRenderTask* pTextRT = static_cast<TextRenderTask*>(m_DlgRenderTask[EDLG_FLUSH_TEX]);
		pTextRT->SetFlushDlg( this );
	}

	if (!ParentWnd)
		return -1;
	return _CreateFromRes(szWindowNameInResFile, ParentWnd);
}

int32 SQRDialog::_CreateFromRes( const char* szWindowNameInResFile, SQRWnd* ParentWnd )
{
	SQR_TRY
	{
		debugWndInfo = szWindowNameInResFile;
		if ( !ParentWnd || !ParentWnd->m_hWnd->m_pGUI )
			return GERROR;
		SQRGUIManager* pGUI = ParentWnd->m_hWnd->m_pGUI;
		WndResParam* pParam = pGUI->GetWndRes( szWindowNameInResFile );
		if ( !pParam )//|| !FindParentClass( pParam->m_ClassName ) )
			return GERROR;
		CFRect rtRootWnd;
		CFRect rtParentWnd;
		//m_isRootDialog = ParentWnd==pGUI->GetRootWnd()||ParentWnd->GetParent()==pGUI->GetRootWnd();
		ParentWnd->GetRootWnd()->GetWndRect( rtRootWnd );
		ParentWnd->GetWndRect( rtParentWnd );
		ParentWnd->GetRootWnd()->ResizeRectOneChild( ParentWnd->GetRootWnd()->GetOrgRect(), NULL );

		if (!(ParentWnd==pGUI->GetRootWnd()))
		{
			CFRect rtParent;
			rtParent.left	= ParentWnd->m_hWnd->m_xInitial;
			rtParent.top	= ParentWnd->m_hWnd->m_yInitial;
			rtParent.right	= ParentWnd->m_hWnd->m_WidthInitial + rtParent.left;
			rtParent.bottom	= ParentWnd->m_hWnd->m_HeightInitial + rtParent.top;
			ParentWnd->ResizeRectOneChild( rtParent, NULL );
		}

		WndCreateParam param;
#if 1
		ResInfo* res_info = pGUI->GetResInfo( (UIString)szWindowNameInResFile );
		if ( res_info )
		{
			param.szTitle	= ( res_info->szTitle.c_str() ) ? res_info->szTitle.c_str() : "";
			param.szDesc	= ( res_info->szDesc.c_str() ) ? res_info->szDesc.c_str() : "";
		}
#else
		param.szTitle	 = pParam->m_WindowTitle;
		param.szDesc	 = pParam->m_szDesc;
#endif
		param.uStyle	 = pParam->m_WndStyle;
		param.x			 = (float)pParam->m_x;
		param.y			 = (float)pParam->m_y;
		param.width		 = (float)pParam->m_Width;
		param.height	 = (float)pParam->m_Height;
		param.pParentWnd = ParentWnd;

		GUI_FONT* pFont = SQRWnd::GetFontInfo(pParam->m_uStyleIndex);
		if (pFont)
		{
			param.nFontID	 = pFont->m_nFontID;
			param.font_size  = (float)pFont->m_FontSize;
		}
		else
		{
			param.nFontID	 = pParam->m_nFontID;
			param.font_size  = (float)pParam->m_FontSize;
		}
		
		param.uGroupID   = pParam->m_GroupID;
		int32 re = Create( param );

		if ( param.uStyle & WS_MODULE )
			m_hWnd->m_pGUI->DeleteModuleWnd( this );

		if ( re == GERROR )
		{
			GetRootWnd()->ResizeRect( rtRootWnd );
			return re;
		}

		SetLockFlag( pParam->m_uLockFlage );
		if (pFont)
		{
			SetFontGradualColor( pFont->m_uFontGradualColor );
			SetFontEffectMask( pFont->m_uFontEffectMask );
		}
		else
		{
			SetFontGradualColor( pParam->m_uFontGradualColor );
			SetFontEffectMask( pParam->m_uFontEffectMask );
		}
		SetIsDrawBackground( pParam->m_isShowBackground );
		SetStyleIndex( pParam->m_uStyleIndex );
		SetMouseOverDesc( param.szDesc );

		///effect
		SetMsgFx( eMF_Click,	pParam->m_MsgFx[eMF_Click] );
		SetMsgFx( eMF_Open,		pParam->m_MsgFx[eMF_Open] );
		SetMsgFx( eMF_Close,	pParam->m_MsgFx[eMF_Close] );

		///sound
		SetMsgSound( eMS_Click, pParam->m_MsgSound[eMS_Click] );
		SetMsgSound( eMS_Open,  pParam->m_MsgSound[eMS_Open] );
		SetMsgSound( eMS_Close, pParam->m_MsgSound[eMS_Close] );

		///flash info
		if (pParam->m_FlashWndStr.size())
		{
			UIMap<EventStateMask, UIString>::iterator itr = pParam->m_FlashWndStr.begin();
			UIMap<EventStateMask, UIString>::iterator itrend = pParam->m_FlashWndStr.end();
			while ( itr != itrend )
			{
				if (itr->first < IP_COUNT)
					SetFlashInfo( itr->first, itr->second.c_str() );
				++itr;
			}
		}

		m_hWnd->m_WindowName	= szWindowNameInResFile;
		m_hWnd->m_WindowNameW	= utf8_to_utf16(m_hWnd->m_WindowName.c_str()).c_str();
		m_hWnd->m_pResParam		= pParam;
		m_hWnd->m_strTransName	= pParam->m_TransTexture;

		m_hWnd->m_bShow			= false;


		//if(m_hWnd->m_strTransName.size()>0)
		//{
		//	bool bEnableASynRead = CPkgFile::IsEnableAsyncRead();
		//	CPkgFile::EnableAsyncRead( false );
		//	pGUI->GetGraphic()->CreateTexture( GUI_PATH_ALIAS_GUITEX.c_str(), (m_hWnd->m_strTransName).c_str(), &(m_hWnd->m_pTransparenTex) ); // szTextName
		//	CPkgFile::EnableAsyncRead( bEnableASynRead );
		//}

		CFPos posParent( ParentWnd->GetWndLeft(),  ParentWnd->GetWndTop() );

		for ( int32 i = 0; i < (int32)pParam->m_ImageList.size(); i++ )
		{
			ChildWndInfo& Info = pParam->m_ImageList[i];
			if ( i )
			{
				GetResWnd( i + 1 )->ResizeRect( Info.rtWnd + posParent );
				GetResWnd( i + 1 )->SetStyleIndex(Info.m_uStyleIndex);
			}
			for ( int32 j = 0; j < (int32)Info.ChildBkList.size(); j++ )
			{
				IMAGE_PARAM IP;
				IP.CtrlMask= WORD( i + 1 );
				IP.StateMask= WORD( Info.ChildBkList[j].nStateMask );
				WND_IMAGE_LIST* pImage = GetWndBkImage( IP );
				if (pFont)
				{
					SetWndTextColor( IP, pFont->m_EventFont[(EventStateMask)(IP.StateMask)].m_FontColor );
					SetWndTextBcColor( IP, pFont->m_EventFont[(EventStateMask)(IP.StateMask)].m_uFontBkColor );
				}
				else
				{
					SetWndTextColor( IP, Info.ChildBkList[j].uTextColor );
					SetWndTextBcColor( IP, Info.ChildBkList[j].uRimColor );
				}
				
				if ( pImage )
					*pImage = m_hWnd->m_pGUI->LoadBKRes( Info.ChildBkList[j].ImageList );
			}
		}

#ifdef _DEBUG
		if ( ParentWnd->GetParent() == GetRootWnd())
		{
			CFRect rtScreen;
			CFRect rtMyself;
			this->GetWndRect(rtMyself);
			GetRootWnd()->GetWndRect( rtScreen );
			CFRect rtAnd = rtScreen.Intersection(rtMyself);
			if ( rtAnd.IsEmpty())
			{
				printf("%s窗口位置超出游戏主画面!\n", szWindowNameInResFile);
			}
		}
#endif

		UIVector<SQRWnd*> ChildList;
		UIMap<UIString, WndResParam*>::iterator it = pParam->m_Child.begin();
		for ( ; it != pParam->m_Child.end(); it++ )
		{
			WndResParam& CP = *it->second;

			

			SQRWnd* pChild = SQRWndClass::CreateWndObject( CP.m_UseClassName );
			if ( !pChild )
				pChild = SQRWndClass::CreateWndObject( CP.m_ClassName );
			if ( pChild )
			{
				{
					ostringstream strm;	
					strm << "WndDebugInfo: " 
						<< "\n--UserClassName:" << CP.m_UseClassName
						<< "\n--ClassName:" << CP.m_ClassName
						<< "\n--WndParentName:" << szWindowNameInResFile
						<< "\n--WndName:" << it->first
						<< endl;
					pChild->debugWndInfo = strm.str().c_str();
				}
				WndCreateParam childParam;
#if 1
				if ( res_info )
				{
					UIMap<const UIString, ChildResInfo>* pChildInfo = &(res_info->mapChildInfo);
					UIMap<const UIString, ChildResInfo> ::iterator itChildInfo = pChildInfo->find( (UIString)it->first );
					if ( !( itChildInfo == pChildInfo->end() ) )
					{
						ChildResInfo* child_res_info = &(itChildInfo->second);
						childParam.szTitle	= ( child_res_info->szTitle.c_str() ) ? child_res_info->szTitle.c_str() : "";
						childParam.szDesc	= ( child_res_info->szDesc.c_str() ) ? child_res_info->szDesc.c_str() : "";
					}
				}
#else
				childParam.szTitle	= CP.m_WindowTitle;
				childParam.szDesc	= CP.m_szDesc;
#endif

				childParam.uStyle	  = CP.m_WndStyle;
				childParam.x		  = (float)CP.m_x;
				childParam.y		  = (float)CP.m_y;
				childParam.width	  = (float)CP.m_Width;
				childParam.height	  = (float)CP.m_Height;
				childParam.szSource   = CP.m_SourceName;//add by guo
				childParam.pParentWnd = this;
				GUI_FONT* pCFont = pChild->GetFontInfo(CP.m_uStyleIndex);
				if (pCFont)
				{
					childParam.nFontID	 = pCFont->m_nFontID;
					childParam.font_size  = (float)pCFont->m_FontSize;
				}
				else
				{
					childParam.nFontID = CP.m_nFontID;
					childParam.font_size  = (float)CP.m_FontSize;
				}
				
				childParam.uGroupID   = CP.m_GroupID;

				re = pChild->Create( childParam );
				if ( childParam.uStyle & WS_MODULE )
					pChild->m_hWnd->m_pGUI->DeleteModuleWnd( pChild );

				if ( re == GERROR )
					delete pChild;
				else
				{
					pChild->m_hWnd->m_bNewByGUISys = true;
					ChildList.push_back( pChild );
					pChild->SetLockFlag( CP.m_uLockFlage );
					if (pCFont)
					{
						pChild->SetFontGradualColor( pCFont->m_uFontGradualColor );
						pChild->SetFontEffectMask( pCFont->m_uFontEffectMask );
					}
					else
					{
						pChild->SetFontGradualColor( CP.m_uFontGradualColor );
						pChild->SetFontEffectMask( CP.m_uFontEffectMask );
					}
					pChild->SetIsDrawBackground(  CP.m_isShowBackground );
					pChild->SetStyleIndex( CP.m_uStyleIndex );
					pChild->SetMouseOverDesc( childParam.szDesc );

					//effect
					pChild->SetMsgFx( eMF_Click,	CP.m_MsgFx[eMF_Click] );
					pChild->SetMsgFx( eMF_Open,		CP.m_MsgFx[eMF_Open] );
					pChild->SetMsgFx( eMF_Close,	CP.m_MsgFx[eMF_Close] );

					//sound
					pChild->SetMsgSound( eMS_Click, CP.m_MsgSound[eMS_Click] );
					pChild->SetMsgSound( eMS_Open,  CP.m_MsgSound[eMS_Open] );
					pChild->SetMsgSound( eMS_Close, CP.m_MsgSound[eMS_Close] );

					///flash info
					if ( CP.m_FlashWndStr.size() )
					{
						UIMap<EventStateMask, UIString>::iterator citr = CP.m_FlashWndStr.begin();
						UIMap<EventStateMask, UIString>::iterator citrend = CP.m_FlashWndStr.end();
						while ( citr != citrend )
						{
							if (citr->first < IP_COUNT)
								pChild->SetFlashInfo( citr->first, citr->second.c_str() );
							++citr;
						}
					}

					pChild->m_hWnd->m_pResParam		= &CP;
					pChild->m_hWnd->m_WindowName	= it->first;
					pChild->m_hWnd->m_WindowNameW	= utf8_to_utf16(pChild->m_hWnd->m_WindowName.c_str()).c_str();
					pChild->m_hWnd->m_strTransName  = CP.m_TransTexture;

					for ( int32 i = 0; i < (int32)CP.m_ImageList.size(); i++ )
					{
						ChildWndInfo& Info = CP.m_ImageList[i];
						if ( i )
						{
							// the gui zoom should be 1.0f
							float temp_zoom = pChild->GetWndHandle()->m_pGUI->GetZoomSize();
							pChild->GetWndHandle()->m_pGUI->SetZoomSize(1.0f);
							pChild->GetResWnd( i + 1 )->SetWndRect( Info.rtWnd + posParent );
							pChild->GetWndHandle()->m_pGUI->SetZoomSize(temp_zoom);

							pChild->GetResWnd( i + 1 )->SetStyleIndex(Info.m_uStyleIndex);
							pChild->GetResWnd( i + 1 )->SetLockFlag(Info.m_uLockFlag);
						}
						for ( int32 j = 0; j < (int32)Info.ChildBkList.size(); j++ )
						{
							IMAGE_PARAM IP;
							IP.CtrlMask=WORD( i + 1 );
							IP.StateMask=WORD( Info.ChildBkList[j].nStateMask );
							WND_IMAGE_LIST* pImage = pChild->GetWndBkImage( IP );

							if (pCFont)
							{
								pChild->SetWndTextColor( IP, pCFont->m_EventFont[(EventStateMask)(IP.StateMask)].m_FontColor );
								pChild->SetWndTextBcColor( IP, pCFont->m_EventFont[(EventStateMask)(IP.StateMask)].m_uFontBkColor );
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

					pChild->OnCreatedAfter();
				}
			}
		}



		if (ParentWnd->GetRootWnd() == ParentWnd)
		{
			ParentWnd->ResizeRectOneChild( rtRootWnd, this );
		}
		else if (ParentWnd->GetRootWnd() == ParentWnd->m_hWnd->m_ParentWnd)
		{
			GetRootWnd()->ResizeRectOneChild( rtRootWnd, NULL );
			ParentWnd->ResizeRectOneChild( rtRootWnd, this );
		}
		else
		{
			GetRootWnd()->ResizeRectOneChild( rtRootWnd, NULL );
			ParentWnd->ResizeRectOneChild( rtParentWnd, this );
		}

		OnResCreated( pParam->m_UseInfo );
		for ( int32 i = 0; i < (int32)ChildList.size(); i++ )
			ChildList[i]->OnResCreated( ChildList[i]->m_hWnd->m_pResParam->m_UseInfo );

		OnChildCreated();
		return 0;
	}
	SQR_CATCH(exp)
	{
		string errMsg = szWindowNameInResFile;
		errMsg = errMsg + debugWndInfo.c_str();
		exp.AppendMsg(errMsg);
		GfkLogExp(exp);
	}
	SQR_TRY_END;

	return NULL;
}

bool SQRDialog::IsFlushDialog()
{
	return m_bOptimize;
}

void SQRDialog::DrawAfterChildren()
{
	// 完成画整个以DIALOG为单位的批量内容
	for (size_t i = 0; i < EDLG_FLUSH_COUNT; i++)
		if (m_DlgRenderTask[i] != NULL)
			m_DlgRenderTask[i]->RenderTask();

}

void SQRDialog::ShowWnd( bool bShow )
{
	if ( !bShow )
		m_bCanDraw = false;
	if ( !bShow && m_hWnd->m_bShow != bShow)
	{
		// 检测子窗体是否有闪烁内容
		// 决定是否隐藏或延迟隐藏
		if (m_DlgRenderTask[EDLG_FLUSH_FLASH] == NULL)
		{
			SQRWnd::ShowWnd( bShow );
			return;
		}
		FLASHRenderTask* pFlashRT = static_cast<FLASHRenderTask*>(m_DlgRenderTask[EDLG_FLUSH_FLASH]);
		if (pFlashRT->HasCountFlash())
		{
			m_bWaitHide = true;
		}
		else
		{
			m_bWaitHide = false;
			SQRWnd::ShowWnd( bShow );
		}
	}
	else
	{
		m_bWaitHide = false;
		SQRWnd::ShowWnd( bShow );
	}

}

void SQRDialog::DrawWndBackground()
{
	if ( m_bOptimize )
	{
		SQRWnd* parentwnd = m_hWnd->m_ParentWnd;
		if ( parentwnd )
			parentwnd = parentwnd->GetFlushDialog();
		if ( parentwnd )
		{
			parentwnd->DrawAfterChildren();
		}
	}
	m_beBGDrawed = true;
	SQRWnd::DrawWndBackground();
}


void SQRDialog::OnProcess()
{
	if ( m_bWaitHide )
	{
		if (m_DlgRenderTask[EDLG_FLUSH_FLASH] == NULL)
			return;
		FLASHRenderTask* pFlashRT = static_cast<FLASHRenderTask*>(m_DlgRenderTask[EDLG_FLUSH_FLASH]);
		if ( !(pFlashRT->HasCountFlash()) )
		{
			m_bWaitHide = false;
			SQRWnd::ShowWnd( false );
		}
	}
	SQRWnd::OnProcess();
}

FLASHRenderTask* SQRDialog::GetFlashRenderTask(bool toNew)
{
	GUIRenderTask* pRt = _GetRenderTask( EDLG_FLUSH_FLASH );
	if (pRt == NULL && toNew)
	{
		m_DlgRenderTask[EDLG_FLUSH_FLASH] = new FLASHRenderTask();
		pRt = m_DlgRenderTask[EDLG_FLUSH_FLASH];
	}
	else if (pRt == NULL)
		return NULL;
	FLASHRenderTask* pFlashRT = static_cast<FLASHRenderTask*>(pRt);
	return pFlashRT;
}

TextRenderTask* SQRDialog::GetTextRenderTask(bool toNew)
{
	GUIRenderTask* pRt = _GetRenderTask( EDLG_FLUSH_TEX );
	if (pRt == NULL && toNew)
	{
		m_DlgRenderTask[EDLG_FLUSH_TEX] = new TextRenderTask();
		pRt = m_DlgRenderTask[EDLG_FLUSH_TEX];
	}
	else if (pRt == NULL)
		return NULL;
	TextRenderTask* pTextRT = static_cast<TextRenderTask*>(pRt);
	pTextRT->SetFlushDlg( this );
	return pTextRT;
}

FxRenderTask* SQRDialog::GetFxRenderTask(bool toNew)
{
	GUIRenderTask* pRt = _GetRenderTask( EDLG_FLUSH_FX );
	if (pRt == NULL && toNew)
	{
		m_DlgRenderTask[EDLG_FLUSH_FX] = new FxRenderTask();
		pRt = m_DlgRenderTask[EDLG_FLUSH_FX];
	}
	else if (pRt == NULL)
		return NULL;
	FxRenderTask* pFxRT = static_cast<FxRenderTask*>(pRt);
	return pFxRT;
}

GUIRenderTask* SQRDialog::_GetRenderTask( uint32 et )
{
	if (et >= 0 && et < EDLG_FLUSH_COUNT)
		return m_DlgRenderTask[et];
	else 
		return NULL;
}

void SQRDialog::InsertTexture(ITexture* pTexture)
{
	if (!pTexture || pTexture->IsValid())
		return;

	TexPool::iterator itr = m_TexMap.find(pTexture);
	if ( itr == m_TexMap.end())
	{
		pTexture->AddRef();
		m_TexMap.insert(pTexture);
		m_bValid = false;
	}
}

bool SQRDialog::IsAsynLoad()
{
	return m_bAsynLoad && !m_bCanDraw;
}

void SQRDialog::Draw()
{
	if ( m_hWnd->m_bShow )
	{
		if ( !m_bValid )
		{
			TexPool::iterator itr = m_TexMap.begin();
			TexPool::iterator itrend = m_TexMap.end();

			while(itr != itrend)
			{
				if ( (*itr)->IsValid() )
				{
					SafeRelease((*itr));
					m_TexMap.erase(itr++);
				}
				else
					++itr;
			}
			if (m_TexMap.size() == 0)
				m_bValid = true;
		}

		if ( m_bValid )
		{
			SQRWnd::Draw();
			m_bCanDraw = true;
			return;
		}
	}
	m_bCanDraw = false;
}

///////////////////////////////////////////////////////////////////////////////////
// CCppMouseHook
///////////////////////////////////////////////////////////////////////////////////
CCppMouseHook::CCppMouseHook()
{
}
CCppMouseHook::~CCppMouseHook()
{
}
