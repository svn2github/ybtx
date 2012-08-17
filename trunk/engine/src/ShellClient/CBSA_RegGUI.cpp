#include "stdafx.h"
#include "CBaseScriptApp.h"
#include "ScriptX.h"
#include "SQRWnd.h"
#include "SQRListCtrl.h"
#include "SQRRichText.h"
#include "SQRComboBox.h"
#include "SQRTreeCtrl.h"
#include "SQRSpin.h"
#include "SQRCheckButton.h"
#include "SQRProgress.h"
#include "SQRStatic.h"
#include "SQRClock.h"
#include "SQRControl.h"
#include "SQRChart.h"
#include "SQRIcon.h"
#include "SQRDialog.h"
#include "SQRItemWnd.h"
#include "MsgBox.h"
#include "SQRWndOffSet.h"
#include "SQRRootWnd.h"
#include "TreeNode.h"
#include "SQRToolTip.h"
#include "IRenderTime.h"
#include "TipHandler.h"
#include "SQRRichToolTip.h"
#include "TSqrAllocator.inl"
#include "SQRVideo.h"//add by guo
#include "SQRFlashWnd.h"
#include "SQRWebBrowser.h"
#include "SQRYYCtrl.h"

namespace sqr
{

	SQRWnd* SMsgBox( uint32 nStyle, SQRMsgBoxHandler* pHolder, SQRWnd* pParent, const char* sText, uint32 dwBtnStyle, void* pParam, const char * sImageStr, bool autoresize)
	{
		if( pHolder )
		{	
			return MsgBox( nStyle, new SQRMsgBoxHandlerHolder( pHolder ), pParent, sText, dwBtnStyle, pParam, sImageStr, autoresize );
		}
		else
			return MessageBox( nStyle, MSG_CALL_NULL, pParent, sText, NULL, dwBtnStyle, pParam, autoresize );
	}
}


int32 CBaseScriptApp::RegisterGUI(CScript& Script)
{
	REGIST_ISOLATED_CLASS(Script, IMAGE);
	//REGIST_B_CLASS(Script, IMAGE);
	//REGIST_B_CLASS( Script, IMAGE_PARAM );
	REGIST_ISOLATED_CLASS( Script, IMAGE_PARAM );
	REGIST_B_CLASS( Script, WND_IMAGE_LIST );
	REGIST_ISOLATED_CLASS( Script, FramePair );
	REGIST_B_CLASS( Script, SQRMsgBoxHandler );
	REGIST_B_CLASS( Script, IHook );
	REGIST_B_CLASS( Script, IAccelerator );
	REGIST_B_CLASS( Script, SQRRichToolTip );
	REGIST_B_CLASS( Script, CYYManager );

	REGIST_B_CLASS( Script, SQRWnd );
	REGIST_D_CLASS( Script, SQRControl, SQRWnd );
	REGIST_D_CLASS( Script, SQRDialog, SQRWnd );
	REGIST_D_CLASS( Script, SQREdit, SQRControl );
	REGIST_D_CLASS( Script, SQRWndOffSet, SQRControl );
	REGIST_D_CLASS( Script, SQRComboBox, SQREdit );
	REGIST_D_CLASS( Script, SQRListCtrl, SQRControl );
	REGIST_D_CLASS( Script, SQRRichText, SQRControl );
	REGIST_D_CLASS( Script, SQRButton, SQRControl );
	REGIST_D_CLASS( Script, SQRCheckButton, SQRButton );
	REGIST_D_CLASS( Script, SQRItemWnd, SQRButton );
	REGIST_D_CLASS( Script, SQRTreeCtrl, SQRControl );
	REGIST_D_CLASS( Script, SQRScrollBar, SQRControl );
	REGIST_D_CLASS( Script, SQRProgress, SQRControl );
	REGIST_D_CLASS( Script, SQRSpin, SQRControl );
	REGIST_D_CLASS( Script, SQRClock, SQRControl );
	REGIST_D_CLASS( Script, SQRStatic, SQRControl );
	REGIST_D_CLASS( Script, SQRVideo, SQRControl );//add by guo
	REGIST_D_CLASS( Script, SQRFlashWnd, SQRControl );
	REGIST_D_CLASS( Script, SQRWebBrowser, SQRControl );
	REGIST_D_CLASS( Script, SQRChart, SQRControl );
	REGIST_D_CLASS( Script, SQRYYCtrl, SQRControl )
	REGIST_D_CLASS( Script, SQRIcon, SQRButton );
	REGIST_D_CLASS( Script, SQRRootWnd, SQRWnd );

	REGIST_B_CLASS( Script, SQRRichString);
	REGIST_D_CLASS( Script,SQRToolTip,SQRRichString);
	REGIST_D_CLASS_2( Script, CCppMouseHook, SQRDialog, IHook );

	REGIST_ISOLATED_CLASS( Script, WndCreateParam );
	REGIST_CLASSMEMBER( Script, WndCreateParam, szTitle, "s" );
	REGIST_CLASSMEMBER( Script, WndCreateParam, uStyle, "I" );
	REGIST_CLASSMEMBER( Script, WndCreateParam, x, "f" );
	REGIST_CLASSMEMBER( Script, WndCreateParam, y, "f" );
	REGIST_CLASSMEMBER( Script, WndCreateParam, width, "f" );
	REGIST_CLASSMEMBER( Script, WndCreateParam, height, "f" );
	//REGIST_CLASSMEMBER( Script, WndCreateParam, pParentWnd, "(SQRWnd)" );
	REGIST_CLASSMEMBER( Script, WndCreateParam, nFontID, "i" );
	REGIST_CLASSMEMBER( Script, WndCreateParam, font_size, "f" );
	REGIST_CLASSMEMBER( Script, WndCreateParam, uGroupID, "I" );
	REGIST_CLASSMEMBER( Script, WndCreateParam, szSource, "s" );//add by guo
	REGIST_CLASSFUNCTION( Script, WndCreateParam, SetParentWnd, "(SQRWnd)");
	REGIST_CLASSFUNCTION( Script, WndCreateParam, GetParentWnd, ":(SQRWnd)");

	REGIST_STATICFUNCTION( Script, CYYManager, CreateYYMgr, "" );
	REGIST_STATICFUNCTION( Script, CYYManager, DestroyYYMgr, "" );


	REGIST_CALLBACKFUNCTION( Script, SQRWnd, WndProc, "III:i" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnActive, "b(SQRWnd)" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnCreated, "" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnDestroy, "" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnResCreated, "s" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnChildCreated, "" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnParentWndMoved, "ff" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnWndMoved, "ff" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnWndShow, "b" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnPlayWndFx, "i" );
	REGIST_CALLBACKFUNCTION_OVERLOAD( Script, SQRWnd, void(SQRWnd::*)(float, float, float, float), OnDrag, OnDrag, "ffff");
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnMouseDescBegin, ":b");
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnMouseDescEnd, "");
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnSized, "");

	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnCtrlmsg, "(SQRWnd)III" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnChar, "II" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnImeChar, "II" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnKeyDown, "II" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnKeyUp, "II" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnLButtonDblClk, "Iii" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnLButtonDown, "Iii" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnLButtonUp, "Iii" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnLButtonClick, "Iii" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnShiftLButtonUp, "Iii" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnShiftLButtonDown, "Iii" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnMButtonDblClk, "Iii" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnMButtonDown, "Iii" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnMButtonUp, "Iii" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnMouseMove, "Iii" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnMouseWheel, "Ihii" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnRButtonDblClk, "Iii" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnRButtonDown, "Iii" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnRButtonUp, "Iii" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnRButtonClick, "Iii" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnCommand, "II:i" );
	REGIST_CALLBACKFUNCTION( Script, SQRWnd, OnShapeEffectEnded, "" )

	//======================= SQRWnd =======================
	//REGIST_CLASSFUNCTION( Script, SQRWnd, setBillboard, "b" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, RegisterAccelKeyCommand, "bbbCII" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, UnRegisterAccelKeyCommand, "bbbC" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, DestroyWnd, ":i" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, Create, "(WndCreateParam):i" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, ReCreateRes, "s:i" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SendMsg, "III:i" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, ReDrawChildren, "" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetGraphic, ":t" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetRootWnd, ":(SQRRootWnd)" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetRootZoomSize, ":f" );	
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetModuleWnd, ":(SQRWnd)" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetStyle, ":I" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetStyle, "I" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetParent, ":(SQRWnd)" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetToolTipRichText,":(SQRToolTip)");
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetRichToolTip,":(SQRRichToolTip)");
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetNewRichToolTip,":(SQRRichToolTip)");
	REGIST_CLASSFUNCTION( Script, SQRWnd, DelRichToolTip,"");
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetFontSize, ":f" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetFontSize, "f" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetTipFontSize,"f");
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetGroupID, ":i" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetGroupID, "i" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetMouseOverDesc, ":s");
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetMouseOverDesc, "s" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetMouseOverDescAfter, "s" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetMouseOverRichTextImageForAllWnd, "i(WND_IMAGE_LIST)" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetMsgFx, "I:s" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetMsgFx, "Is" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetLockFlag, ":I" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetLockFlag, "I" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, IsDrawBackground, ":b" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetIsDrawBackground, "b" );
	//REGIST_CLASSFUNCTION( Script, SQRWnd, WndToScreen, "(CIRect):(CIRect)" );
	//REGIST_CLASSFUNCTION( Script, SQRWnd, ScreenToWnd, "(CIRect):(CIRect)" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, SQRWnd, void(SQRWnd::*)(CFRect&, CFRect&), WndToScreen, WndToScreen, "(CFRect)(CFRect)" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, SQRWnd, void(SQRWnd::*)(CFRect&, CFRect&), ScreenToWnd, ScreenToWnd, "(CFRect)(CFRect)" );
	//REGIST_CLASSFUNCTION( Script, SQRWnd, GetCursorPos, ":(CPos)" ); //CPos
	REGIST_CLASSFUNCTION_OVERLOAD( Script, SQRWnd, void(SQRWnd::*)(CFPos&), GetCursorPos, GetCursorPos, "(CFPos)" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, SQRWnd, void(SQRWnd::*)(CFPos&), GetLogicCursorPos, GetLogicCursorPos, "(CFPos)" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetCurMessage, ":I" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, FindNextWnd, ":(SQRWnd)" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, FindPreWnd, ":(SQRWnd)" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, FindFirstChild, ":(SQRWnd)" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, FindLastChild, ":(SQRWnd)" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetTopWnd, ":(SQRWnd)" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetZOrder, ":s" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetDlgChild, "s:(SQRWnd)" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetWndData, "I" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetWndData, ":I" );
	REGIST_STATICFUNCTION( Script, SQRWnd, RegisterHook, "(IHook)I" ); //IHook
	REGIST_STATICFUNCTION( Script, SQRWnd, UnRegisterHook, "(IHook)" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, AddHookMsg, "(IHook)I" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, TestHook, "(IHook):b" );
	REGIST_STATICFUNCTION( Script, SQRWnd, RegisterAccelerator, "(IAccelerator)I" ); //IAccelerator
	REGIST_STATICFUNCTION( Script, SQRWnd, UnRegisterAccelerator, "(IAccelerator)" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, EnableWnd, "b" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, ShowWnd, "b" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, ShowWndIgnTex, "b" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetWndRect, "(CFRect)");
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetWndRect, "(CFRect)");
	REGIST_CLASSFUNCTION( Script, SQRWnd, ResizeRect, "(CFRect)b");
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetShapeEffect, "i(CFRect)(CFRect)");
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetWndRectBak, "(CFRect)");
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetLogicRect, "(CFRect)");
	REGIST_CLASSFUNCTION( Script, SQRWnd, RefreshWndDis, "f:f");
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetLogicRect, "(CFRect)");
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetWndWidth, ":f" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetWndOrgHeight, ":f" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetWndOrgWidth, ":f" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetWndHeight, ":f" )
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetWndWidth, "f" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetWndHeight, "f" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetActive, "" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetFocus, "" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetTransparent, "f" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetDepth, "f" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetDepth, ":f" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetTransparentObj, "ib" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, EndTransparent, "b");
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetTransparent, ":f" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetFont, "ibf:b" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetParent, "(SQRWnd):(SQRWnd)" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, AddChildWnd, "(SQRWnd)" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetWndName, ":s" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetWndText, ":s" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetWndText, "s" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, CopyToClipboard, "s" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetWndBkImage, "(IMAGE_PARAM)(WND_IMAGE_LIST)" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetWndBkImage, "(IMAGE_PARAM):(WND_IMAGE_LIST)" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetWndTextColor, "(IMAGE_PARAM)I" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetWndTextColor, "(IMAGE_PARAM):I" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetWndTextBcColor, "(IMAGE_PARAM)I" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetWndTextBcColor, "(IMAGE_PARAM):I" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetResWnd, "i:(SQRWnd)" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetWndCursor, "s" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetWndCursor, ":s" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetGlobCursor, "s" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetGlobCursor, ":s" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, UpdateGlobCursor, "(SQRWnd)" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, IsChild, "(SQRWnd):b" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, IsHeld, ":b" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetDownTooltips,"b");
	REGIST_CLASSFUNCTION( Script, SQRWnd, IsActive, ":b" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, IsFocus, ":b" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, IsEnable, ":b" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, IsEnableTest, ":b" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, IsShow, ":b" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, IsCreated, ":b" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, IsLastMouseMsgWnd, ":b" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetLastMouseMsgWnd, ":(SQRWnd)" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, IsInWndArea, "ff:b" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, IsInTitleArea, "ff:b" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetClassName, ":s");
	REGIST_CLASSFUNCTION( Script, SQRWnd, FindParent, "(SQRWnd):b");
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetRotation, "f");
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetIsBottom, "b" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetHaveEditActive, ":b" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetDetectMouseOverWnd, "b" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, IsDetectMouseOverWnd, ":b" );
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetMouseWheel, "b")
	REGIST_CLASSFUNCTION( Script, SQRWnd, InserBefore, "(SQRWnd):(SQRWnd)")
	REGIST_CLASSFUNCTION( Script, SQRWnd, PlayWndFx, "s")
	REGIST_CLASSFUNCTION( Script, SQRWnd, PlayScreenFx, "s")
	REGIST_CLASSFUNCTION( Script, SQRWnd, PlayWndFxEx, "si")
	REGIST_CLASSFUNCTION( Script, SQRWnd, PlayScreenFxEx, "si")
	REGIST_CLASSFUNCTION( Script, SQRWnd, PlayScreenPathFx, "ssi")
	REGIST_CLASSFUNCTION( Script, SQRWnd, PlayWndPathFx, "ssi")
	REGIST_CLASSFUNCTION( Script, SQRWnd, PlayWndImePathFx, "ssi")
	REGIST_CLASSFUNCTION( Script, SQRWnd, DelWndFx, "s")
	REGIST_CLASSFUNCTION( Script, SQRWnd, DelWndFxEx, "")
	REGIST_CLASSFUNCTION( Script, SQRWnd, DelScreenFx, "s")
	REGIST_CLASSFUNCTION( Script, SQRWnd, PlayWndImeFx, "si")
	REGIST_CLASSFUNCTION( Script, SQRWnd, DelWndImeFx, "s")
	REGIST_CLASSFUNCTION( Script, SQRWnd, ResizeRectOneChild,"(CFRect)(SQRWnd)")
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetToAutoFormat, "b");
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetCutWnd, "(SQRWnd)");
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetWndZoom, "f");
	//======================= SQRWnd-Flash =======================
	REGIST_CLASSFUNCTION( Script, SQRWnd, SetFlashInfo, "Is");
	REGIST_CLASSFUNCTION( Script, SQRWnd, GetFlashInfo, "I:s");
	REGIST_CLASSFUNCTION( Script, SQRWnd, DelFlashInfo, "I");
	REGIST_CLASSFUNCTION( Script, SQRWnd, AddFlashAttentionTex, "s(CFRect)(SQRWnd)Ifffff");
	REGIST_CLASSFUNCTION( Script, SQRWnd, DelFlashAttention, "");
	REGIST_CLASSFUNCTION( Script, SQRWnd, AddFlashInfoByName, "s");
	REGIST_CLASSFUNCTION( Script, SQRWnd, AddFlashInfoByNameImp, "s");
	REGIST_CLASSFUNCTION( Script, SQRWnd, AddFlashInfoByNameEx, "sb");
	REGIST_CLASSFUNCTION( Script, SQRWnd, DelFlashAttentionSpe, "(SQRWnd)s");
	
	
	//======================= SQRControl =======================//
	REGIST_CLASSFUNCTION( Script, SQRControl, SendCtrlMsg, "III:i" );
	
	//====================== IRenderTime =====================//
	REGIST_ABSTRACT_B_CLASS( Script, IRenderTime);
	
	//======================= SQRClock =======================//
	REGIST_CLASSFUNCTION( Script, SQRClock, setTime, "i" );
	REGIST_CLASSFUNCTION( Script, SQRClock, setRunedTime, "i" );
	REGIST_CLASSFUNCTION( Script, SQRClock, SetDirection, "b" );
	REGIST_CLASSFUNCTION( Script, SQRClock, SetBackImageColor, "I" );
	REGIST_CLASSFUNCTION( Script, SQRClock, SetBeFrontImageColor, "b" );
	REGIST_CLASSFUNCTION( Script, SQRClock, SetPos, "i" );
	REGIST_CLASSFUNCTION( Script, SQRClock, SetRange, "i" );
	REGIST_CLASSFUNCTION( Script, SQRClock, setTimeEx, "i(IRenderTime)" );

	//======================= SQRDialog =======================//
	REGIST_CLASSFUNCTION( Script, SQRDialog, CreateFromRes, "s(SQRWnd):i" );
	REGIST_CLASSFUNCTION( Script, SQRDialog, CreateFromResEx, "s(SQRWnd)bb:i" );

	//======================= SQRWndOffSet =====================//
	REGIST_CLASSFUNCTION( Script, SQRWndOffSet, SetStep, "if" );

	//======================= SQRButton =======================
	REGIST_CLASSFUNCTION( Script, SQRButton, SetFlashTime, "Ii" );
	REGIST_CLASSFUNCTION( Script, SQRButton, SetBackImageColor, "I" );

	//======================= SQRCheckButton =======================
	REGIST_CALLBACKFUNCTION( Script, SQRCheckButton, OnCheck, "b" );
	REGIST_CLASSFUNCTION( Script, SQRCheckButton, SetCheck, "b" );
	REGIST_CLASSFUNCTION( Script, SQRCheckButton, GetCheck, ":b" );
	REGIST_CLASSFUNCTION( Script, SQRCheckButton, GetFirstBrother, ":(SQRWnd)" );
	REGIST_CLASSFUNCTION( Script, SQRCheckButton, GetLastBrother, ":(SQRWnd)" );
	REGIST_CLASSFUNCTION( Script, SQRButton, SetTextOffsetX, "f" );
	REGIST_CLASSFUNCTION( Script, SQRButton, SetTextOffsetY, "f" );

	//======================= SQRComboBox =======================
	REGIST_CLASSFUNCTION( Script, SQRComboBox, InsertItem, "sif:i" );
	REGIST_CLASSFUNCTION( Script, SQRComboBox, GetItem, "i:(SQRWnd)" );
	REGIST_CLASSFUNCTION( Script, SQRComboBox, RemoveAllItem, "" );
	REGIST_CLASSFUNCTION( Script, SQRComboBox, RemoveItem, "i" );
	REGIST_CLASSFUNCTION( Script, SQRComboBox, RemoveItemByText, "s" );
	REGIST_CLASSFUNCTION( Script, SQRComboBox, SetItemHeight, "fi" );
	REGIST_CLASSFUNCTION( Script, SQRComboBox, GetCurrentSel, ":i" );
	REGIST_CLASSFUNCTION( Script, SQRComboBox, SetSel, "i" );
	REGIST_CLASSFUNCTION( Script, SQRComboBox, GetSelText, ":s" );
	REGIST_CLASSFUNCTION( Script, SQRComboBox, GetItemText, "i:s" );
	REGIST_CLASSFUNCTION( Script, SQRComboBox, GetItemCount, ":i");
	REGIST_CLASSFUNCTION( Script, SQRComboBox, InsertColumn, "if:i");
	REGIST_CLASSFUNCTION( Script, SQRComboBox, InsertDefaultColumn, "");

	//======================= SQREdit =======================
	REGIST_CLASSFUNCTION( Script, SQREdit, SetMaxTextLenght, "i" );
	REGIST_CLASSFUNCTION( Script, SQREdit, GetMaxTextLenght, ":i" );
	REGIST_CLASSFUNCTION( Script, SQREdit, InsertStr, "s" );
	REGIST_CLASSFUNCTION( Script, SQREdit, SetSelection, "II" );
	REGIST_CLASSFUNCTION( Script, SQREdit, DisableIme, "b" );
	REGIST_CLASSFUNCTION( Script, SQREdit, setCaratIndex, "I");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, SQREdit, int32 (SQREdit::*)(), GetLineCount, GetLineCount, ":i" );
	REGIST_CLASSFUNCTION( Script, SQREdit, SetIsSendMsgToParent, "b" );
	REGIST_CLASSFUNCTION( Script, SQREdit, SetIsOnCommand, "b" );
	REGIST_CLASSFUNCTION( Script, SQREdit, SetSendType, "b" );
	REGIST_CLASSFUNCTION( Script, SQREdit, GetSendType, ":b" );

	//======================= SQRItemWnd =======================
	REGIST_CLASSFUNCTION( Script, SQRItemWnd, GetRenderObjAni, "(IMAGE_PARAM):s" );
	REGIST_CLASSFUNCTION( Script, SQRItemWnd, SetRenderObjAni, "(IMAGE_PARAM)s" );
	REGIST_CLASSFUNCTION( Script, SQRItemWnd, SetSelected, "b" );
	REGIST_CLASSFUNCTION( Script, SQRItemWnd, IsSelected, ":b" );

	//======================= SQRListCtrl =======================
	REGIST_CLASSFUNCTION( Script, SQRListCtrl, InsertColumn, "if:i" );
	REGIST_CLASSFUNCTION( Script, SQRListCtrl, InsertItem, "if:i" );
	REGIST_CLASSFUNCTION( Script, SQRListCtrl, InsertItemFromRes, "is:i" );
	REGIST_CLASSFUNCTION( Script, SQRListCtrl, SetItemHighLight, "i" );
	REGIST_CLASSFUNCTION( Script, SQRListCtrl, SetItemHeight, "fi" );
	REGIST_CLASSFUNCTION( Script, SQRListCtrl, GetSubItem, "ii:(SQRWnd)" );
	REGIST_CLASSFUNCTION( Script, SQRListCtrl, GetHighLightIndex, ":i" );
	REGIST_CLASSFUNCTION( Script, SQRListCtrl, GetSelectItem, "i:i" );
	REGIST_CLASSFUNCTION( Script, SQRListCtrl, GetSelectSubItem, "ii:i" );
	REGIST_CLASSFUNCTION( Script, SQRListCtrl, DeleteAllItem, "" );
	REGIST_CLASSFUNCTION( Script, SQRListCtrl, DeleteItem, "i" );
	REGIST_CLASSFUNCTION( Script, SQRListCtrl, GetItemCount, ":i" );
	REGIST_CLASSFUNCTION( Script, SQRListCtrl, GetSubItemCount, ":i" );
	REGIST_CLASSFUNCTION( Script, SQRListCtrl, SelectItem, "i" );
	REGIST_CLASSFUNCTION( Script, SQRListCtrl, ResetItem, "");
	REGIST_CLASSFUNCTION( Script, SQRListCtrl, GetVerticalScroll, ":(SQRScrollBar)");
	REGIST_CLASSFUNCTION( Script, SQRListCtrl, SetVerticalPos, "i");
	REGIST_CLASSFUNCTION( Script, SQRListCtrl, GetVerticalPos, ":i");
	REGIST_CLASSFUNCTION( Script, SQRListCtrl, GetWorkAreaHeight, ":f");
	REGIST_CLASSFUNCTION( Script, SQRListCtrl, GetWorkAreaWidth, ":f");
	REGIST_CLASSFUNCTION( Script, SQRListCtrl, GetWorkAreaRectEx, "(CFRect)");
	REGIST_CLASSFUNCTION( Script, SQRListCtrl, GetWorkAreaOrgHeight, ":f");
	REGIST_CLASSFUNCTION( Script, SQRListCtrl, GetWorkAreaOrgWidth, ":f");
	REGIST_CLASSFUNCTION( Script, SQRListCtrl, GetWorkAreaOrgRectEx, "(CFRect)");
	
	
	//======================= SQRProgress =======================
	REGIST_CLASSFUNCTION( Script, SQRProgress, GetPos, ":i" );
	REGIST_CLASSFUNCTION( Script, SQRProgress, GetRange, ":i" );
	REGIST_CLASSFUNCTION( Script, SQRProgress, GetProgressMode, ":i" );
	REGIST_CLASSFUNCTION( Script, SQRProgress, SetPos, "i" );
	REGIST_CLASSFUNCTION( Script, SQRProgress, SetRange, "i" );
	REGIST_CLASSFUNCTION( Script, SQRProgress, SetProgressMode, "i" );
	REGIST_CLASSFUNCTION( Script, SQRProgress, SetFillColor, "I" );
	REGIST_CLASSFUNCTION( Script, SQRProgress, SetChangeDiff, "iI:(SQRWnd)" );
	REGIST_CLASSFUNCTION( Script, SQRProgress, EndChangeDiff, "" );
	REGIST_CLASSFUNCTION( Script, SQRProgress, GetPreDifWnd, ":(SQRWnd)" );
	REGIST_CLASSFUNCTION( Script, SQRProgress, GetBakDifWnd, ":(SQRWnd)" );
	REGIST_CLASSFUNCTION( Script, SQRProgress, GetCurDifWnd, ":(SQRWnd)" );

	//======================= SQRScrollBar =======================
	REGIST_CLASSFUNCTION( Script, SQRScrollBar, SetRange, "i" );
	REGIST_CLASSFUNCTION( Script, SQRScrollBar, SetPage, "i" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, SQRScrollBar, void(SQRScrollBar::*)(int32), SetPos, SetPos, "i" );
	REGIST_CLASSFUNCTION( Script, SQRScrollBar, GetRange, ":i" );
	REGIST_CLASSFUNCTION( Script, SQRScrollBar, GetPage, ":i" );
	REGIST_CLASSFUNCTION( Script, SQRScrollBar, GetPos, ":i" );
	REGIST_CLASSFUNCTION( Script, SQRScrollBar, Istop, ":b" );
	REGIST_CLASSFUNCTION( Script, SQRScrollBar, Isbottom, ":b" );
	REGIST_CLASSFUNCTION( Script, SQRScrollBar, SetCutWnd, "(SQRWnd)" );

	//======================= SQRTreeCtrl =======================
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, GetCurrentNode, ":t" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, GetShowNodeCount, ":i" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, InsertNode, "ts(WND_IMAGE_LIST)(WND_IMAGE_LIST)II:t" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, DeleteNode, "t" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, DeleteAll, "" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, ExpandNode, "tb" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, IsExpand, "t:b" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, GroupNode, "tb" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, IsGroup, "t:b" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, SelectNode, "tb" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, IsSelect, "t:b" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, ScrollToNode, "t" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, SetNodeText, "ts" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, SetNodeImage, "t(WND_IMAGE_LIST)(WND_IMAGE_LIST)" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, SetNodeAscendData, "tI" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, SetNodeParam, "tI" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, GetNodeText, "t:s" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, GetNodeImage, "t(WND_IMAGE_LIST)(WND_IMAGE_LIST)" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, GetNodeAscendData, "t:I" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, GetNodeParam, "t:I" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, GetNodeParent, "t:t" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, GetChildNodeCount, "t:I" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, GetFirstChildNode, "t:t" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, GetNextChildNode, "t:t" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, GetNodeByText, "st:t" );
	REGIST_CLASSFUNCTION( Script, SQRTreeCtrl, Sort, "t" );

	//======================= SQRRichText =======================
	REGIST_CLASSFUNCTION( Script, SQRRichText, AddWndText, "s" );
	REGIST_CLASSFUNCTION( Script, SQRRichText, AddNewLineText, "s" );
	REGIST_CLASSFUNCTION( Script, SQRRichText, InsertImage, "i(WND_IMAGE_LIST)" );
	REGIST_CLASSFUNCTION( Script, SQRRichText, Updata, "" );
	REGIST_CLASSFUNCTION( Script, SQRRichText, Isbottom, ":b" );
	REGIST_CLASSFUNCTION( Script, SQRRichText, GetChooseStr, ":s" );
	REGIST_CLASSFUNCTION( Script, SQRRichText, GetNameWnd, "s:(SQRWnd)" );
	REGIST_CLASSFUNCTION( Script, SQRRichText, GetChooseStrAttr, ":s" ); 
	REGIST_CLASSFUNCTION( Script, SQRRichText, GetChooseStr, ":s" );
	REGIST_CLASSFUNCTION( Script, SQRRichText, GetNameWnd, "s:(SQRWnd)" );
	REGIST_CLASSFUNCTION( Script, SQRRichText, SetShowScrollBar, "b" );
	REGIST_CLASSFUNCTION( Script, SQRRichText, SetAutoWidth, "ff" );
	REGIST_CLASSFUNCTION( Script, SQRRichText, SetMinHeight, "f" );
	REGIST_CLASSFUNCTION( Script, SQRRichText, SetRowDist, "f" );
	REGIST_CLASSFUNCTION( Script, SQRRichText, SetPageDown, "");
	REGIST_CLASSFUNCTION( Script, SQRRichText, SetPageUp, "");
	REGIST_CLASSFUNCTION( Script, SQRRichText, SetPageBottom, "");
	REGIST_CLASSFUNCTION( Script, SQRRichText, SetWndTextXR,"b");

	//======================= SQRChart =======================
	REGIST_ABSTRACT_B_CLASS( Script, SQRChartGraphicItem );
	REGIST_D_CLASS( Script, SQRChartLineGraph, SQRChartGraphicItem );
	REGIST_D_CLASS( Script, SQRChartHistogram, SQRChartGraphicItem );

	//======================= SQRVideo =======================
	REGIST_CLASSFUNCTION( Script, SQRVideo, Load, "s" );
	REGIST_CLASSFUNCTION( Script, SQRVideo, Play, "" );
	REGIST_CLASSFUNCTION( Script, SQRVideo, Pause, "" );
	REGIST_CLASSFUNCTION( Script, SQRVideo, Rewind, "" );

	//======================= SQRYYCtrl =======================
	REGIST_CALLBACKFUNCTION( Script, SQRYYCtrl, CallBackChar, "is:i" );
	REGIST_CALLBACKFUNCTION( Script, SQRYYCtrl, CallBackInt, "iI:i" );
	REGIST_CLASSFUNCTION( Script, SQRYYCtrl, LoginSucceed, ":i" );
	REGIST_CLASSFUNCTION( Script, SQRYYCtrl, LoginFailure, "s:i" );
	REGIST_CLASSFUNCTION( Script, SQRYYCtrl, SetLoginKEY, "s:i" );
	REGIST_CLASSFUNCTION( Script, SQRYYCtrl, DestroyYY, "" );
	REGIST_CLASSFUNCTION( Script, SQRYYCtrl, SetLocationStyle, "I" );
	
	//======================= SQRWebBrowser =======================
	REGIST_CLASSFUNCTION( Script, SQRWebBrowser, SetszSourceName, "s");
	REGIST_CLASSFUNCTION( Script, SQRWebBrowser, GetSrcCode, ":s");

	//======================= SQRToolTip =======================
	REGIST_CLASSFUNCTION( Script, SQRToolTip, AddChildToolTip, ":b");
	REGIST_CLASSFUNCTION( Script, SQRToolTip, ClearChildToolTip, "");
	REGIST_CLASSFUNCTION( Script, SQRToolTip, GetChildToolTip, ":(SQRToolTip)");
	REGIST_CLASSFUNCTION( Script, SQRToolTip, SetToolTipDesc, "s");
	REGIST_CLASSFUNCTION( Script, SQRToolTip, SetToolTipStyle,"I");

	REGIST_CLASSFUNCTION( Script, SQRChart, Clear, "" );
	REGIST_CLASSFUNCTION( Script, SQRChart, Count, ":I" );
	REGIST_CLASSFUNCTION( Script, SQRChart, AppendGraphicItem, "I:(SQRChartGraphicItem)" );
	REGIST_CLASSFUNCTION( Script, SQRChart, GetGraphicItem, "i:(SQRChartGraphicItem)" );
	
	REGIST_CLASSFUNCTION( Script, SQRChartGraphicItem, Count, ":I" );
	REGIST_CLASSFUNCTION( Script, SQRChartGraphicItem, GetVisible, ":b" );
	REGIST_CLASSFUNCTION( Script, SQRChartGraphicItem, SetVisible, "b" );
	REGIST_CLASSFUNCTION( Script, SQRChartGraphicItem, Clear, "" );
	REGIST_CLASSFUNCTION( Script, SQRChartGraphicItem, GetHost, ":(SQRChart)" );
	
	REGIST_CLASSFUNCTION( Script, SQRChartLineGraph, AppendData, "f" );
	REGIST_CLASSFUNCTION( Script, SQRChartLineGraph, GetColor, ":I" );
	REGIST_CLASSFUNCTION( Script, SQRChartLineGraph, SetColor, "I" );
	REGIST_CLASSFUNCTION( Script, SQRChartLineGraph, GetZoomY, ":f" );
	REGIST_CLASSFUNCTION( Script, SQRChartLineGraph, SetZoomY, "f" );
	REGIST_CLASSFUNCTION( Script, SQRChartLineGraph, GetDeltaY, ":f" );
	REGIST_CLASSFUNCTION( Script, SQRChartLineGraph, SetDeltaY, "f" )
	REGIST_CLASSFUNCTION( Script, SQRChartLineGraph, GetDeltaX, ":f" );
	REGIST_CLASSFUNCTION( Script, SQRChartLineGraph, SetDeltaX, "f" )
	REGIST_CLASSFUNCTION( Script, SQRChartLineGraph, GetSpanX, ":f" );
	REGIST_CLASSFUNCTION( Script, SQRChartLineGraph, SetSpanX, "f" );
	REGIST_CALLBACKFUNCTION( Script, SQRChartLineGraph, OnHitPoint, "iiiif" );

	REGIST_CLASSFUNCTION( Script, SQRChartHistogram, AppendData, "fII" );
	REGIST_CLASSFUNCTION( Script, SQRChartHistogram, GetZoomY, ":f" );
	REGIST_CLASSFUNCTION( Script, SQRChartHistogram, SetZoomY, "f" );
	REGIST_CLASSFUNCTION( Script, SQRChartHistogram, GetItemWidth, ":f" );
	REGIST_CLASSFUNCTION( Script, SQRChartHistogram, SetItemWidth, "f" )
	REGIST_CLASSFUNCTION( Script, SQRChartHistogram, GetSpanX, ":f" );
	REGIST_CLASSFUNCTION( Script, SQRChartHistogram, SetSpanX, "f" );

	//======================= SQRRootWnd =======================
	REGIST_CLASSFUNCTION( Script, SQRRootWnd, GetSysCursorPos, "(CFPos)" );
	REGIST_CLASSFUNCTION( Script, SQRRootWnd, SetSysCursorPos, "ii" );
	REGIST_CLASSFUNCTION( Script, SQRRootWnd, ShowCursor, "b" );
	REGIST_CLASSFUNCTION( Script, SQRRootWnd, SetLockCursorMessage, "b" );
	REGIST_CLASSFUNCTION( Script, SQRRootWnd, GetClientRect, "(CIRect)" );
	REGIST_CLASSFUNCTION( Script, SQRRootWnd, ClipCursor, "(CIRect)" );
	REGIST_CLASSFUNCTION( Script, SQRRootWnd, SetCapture, "");
	REGIST_CLASSFUNCTION( Script, SQRRootWnd, ReleaseCapture, "");
	REGIST_CLASSFUNCTION( Script, SQRRootWnd, SetLimitedState, "b");

	//======================= TipHandler =========================
	REGIST_B_CLASS( Script, TipHandler );
	REGIST_CALLBACKFUNCTION( Script, TipHandler, OnToolTipShow, "(SQRRichToolTip)" );

	//======================= SQRRichToolTip ===============================
	REGIST_CLASSFUNCTION( Script, SQRRichToolTip, AddChildToolTip, ":b");
	REGIST_CLASSFUNCTION( Script, SQRRichToolTip, ClearChildToolTip, "");
	REGIST_CLASSFUNCTION( Script, SQRRichToolTip, GetChildToolTip, ":(SQRRichToolTip)");
	REGIST_CLASSFUNCTION( Script, SQRRichToolTip, SetToolTipStyle,"I");
	REGIST_CLASSFUNCTION( Script, SQRRichToolTip, PushTipsWnd,"s:(SQRWnd)");
	REGIST_CLASSFUNCTION( Script, SQRRichToolTip, GetParentWnd,":(SQRWnd)");
	REGIST_CLASSFUNCTION( Script, SQRRichToolTip, SetTipHandler,"(TipHandler)");


	//======================= WND_IMAGE_LIST =======================
	REGIST_CLASSMEMBER( Script, IMAGE, dwColor, "i" );
	REGIST_CLASSMEMBER( Script, IMAGE, dwLockFlag, "i" );
	REGIST_CLASSFUNCTION( Script, IMAGE, GetPosWndX, ":f" );
	REGIST_CLASSFUNCTION( Script, IMAGE, GetPosWndY, ":f" );
	REGIST_CLASSFUNCTION( Script, IMAGE, SetPosWndX, "f" );
	REGIST_CLASSFUNCTION( Script, IMAGE, SetPosWndY, "f" );


	//======================= WND_IMAGE_LIST =======================
	REGIST_CLASSFUNCTION( Script, WND_IMAGE_LIST, Release, "" );
	REGIST_CLASSFUNCTION( Script, WND_IMAGE_LIST, AddImage, "tis(CFRect)(CFPos)II:i" );
	REGIST_CLASSFUNCTION( Script, WND_IMAGE_LIST, AddImageWin, "tis(CFRect)(CFPos)II:i" );
	REGIST_CLASSFUNCTION( Script, WND_IMAGE_LIST, SetGUIAsynEnable, "b" );
	REGIST_CLASSFUNCTION( Script, WND_IMAGE_LIST, AddImageFromImageList, "(WND_IMAGE_LIST)ii:i" );
	REGIST_CLASSFUNCTION( Script, WND_IMAGE_LIST, ClearImage, "" );
	REGIST_CLASSFUNCTION( Script, WND_IMAGE_LIST, GetImageCount, ":i" );
	//REGIST_CLASSFUNCTION( Script, WND_IMAGE_LIST, GetImage, "i:(IMAGE)" );
	REGIST_CLASSFUNCTION_OVERLOAD(Script, WND_IMAGE_LIST, void(WND_IMAGE_LIST::*)(int32, IMAGE&), GetImage, GetImage, "i(IMAGE)");
	REGIST_CLASSFUNCTION( Script, WND_IMAGE_LIST, DeleteImage, "i" );
	REGIST_CLASSFUNCTION( Script, WND_IMAGE_LIST, DetectImage, "(CFPos):i" );

	//====================== SQRMsgBoxHandler =========================
	REGIST_CALLBACKFUNCTION( Script, SQRMsgBoxHandler, CallBack, "it:b" );

	//====================== CMsgBox =========================
	REGIST_GLOBALFUNCTION( Script, RegistMsgBox, "I(SQRWnd)ssss" );
	REGIST_GLOBALFUNCTION( Script, SMsgBox, "I(SQRMsgBoxHandler)(SQRWnd)sItsb:(SQRWnd)" );
	REGIST_GLOBALFUNCTION( Script, CloseAllMsgBox, "(SQRWnd)" );

	//======================== IHook ================================
	REGIST_CALLBACKFUNCTION( Script, IHook, OnHook, "(SQRWnd)III:b" );

	//======================== IAccelerator ========================
	REGIST_CALLBACKFUNCTION( Script, IAccelerator, OnAccelerator, "III:b" );
	REGIST_CALLBACKFUNCTION( Script, IAccelerator, OnAcceleratorInEdit, "" );
	REGIST_CALLBACKFUNCTION( Script, IAccelerator, OnAcceleratorIsModule, "III" );

	//======================== FramePair ================================
	REGIST_CLASSMEMBER( Script, FramePair, nStart, "h" );
	REGIST_CLASSMEMBER( Script, FramePair, nEnd, "h" );

	//======================== IMAGE_PARAM ================================
	REGIST_CLASSMEMBER( Script, IMAGE_PARAM, CtrlMask, "H" );
	REGIST_CLASSMEMBER( Script, IMAGE_PARAM, StateMask, "H" );


	//======================== 基本类型 ========================
	REGIST_CONSTANT( Script, WS_MODULE );
	REGIST_CONSTANT( Script, WS_CHILD );
	REGIST_CONSTANT( Script, WS_MSGTRANSPT );
	REGIST_CONSTANT( Script, WS_AREAUSETEXRECT );
	REGIST_CONSTANT( Script, WS_MULLINETEXT );
	REGIST_CONSTANT( Script, WS_TEXTYB );
	REGIST_CONSTANT( Script, WS_TEXTYC );
	REGIST_CONSTANT( Script, WS_TEXTXR );
	REGIST_CONSTANT( Script, WS_TEXTXC );
	REGIST_CONSTANT( Script, WS_TOPMOST );
	REGIST_CONSTANT( Script, WS_IGNOREWHEEL );
	REGIST_CONSTANT( Script, WS_HORSCROLL );
	REGIST_CONSTANT( Script, WS_VERSCROLL );

	//======================== 控件类型 ========================
	//======================== EDIT类型 ========================
	REGIST_CONSTANT( Script, ECS_MULTILINE );
	REGIST_CONSTANT( Script, ECS_PASSWORD );
	REGIST_CONSTANT( Script, ECS_ENABLECLIPBD );
	REGIST_CONSTANT( Script, ECS_DISABLEIME );
	REGIST_CONSTANT( Script, ECS_YBORDER );
	REGIST_CONSTANT( Script, ECS_XBORDER );

	//======================== SCROLL类型 ========================
	REGIST_CONSTANT( Script, SS_HSCROLL );

	//======================== SPIN类型 ========================
	REGIST_CONSTANT( Script, PS_HSCROLL );

	//======================== CheckButton类型 ========================
	REGIST_CONSTANT( Script, BTN_CHECK );
	REGIST_CONSTANT( Script, BTN_RADIO );
	REGIST_CONSTANT( Script, BTN_XOF );
	REGIST_CONSTANT( Script, BTN_YOF );

	//======================== SQRProgress类型 ========================
	REGIST_CONSTANT( Script, PRG_FILLFIRST );

	//======================== ListCtrl类型 ========================
	REGIST_CONSTANT( Script, LCTL_SHOWSCLAUTO );
	REGIST_CONSTANT( Script, LCTL_MULTSEL );
	REGIST_CONSTANT( Script, LCTL_CLICKSEL );
	REGIST_CONSTANT( Script, LCTL_SELFULLLINE );
	REGIST_CONSTANT( Script, LCTL_SHOWGRID );

	//======================== ComboCtrl类型 ========================
	REGIST_CONSTANT( Script, COMBO_DISEDIT );

	//======================== TreeCtrl类型 ========================
	REGIST_CONSTANT( Script, TREE_FULLSELECT );
	REGIST_CONSTANT( Script, TREE_HASLINE );

	//======================== 标准Windows消息 ========================
	REGIST_CONSTANT( Script, WM_KEYDOWN );
	REGIST_CONSTANT( Script, WM_KEYUP );
	REGIST_CONSTANT( Script, WM_CHAR );
	REGIST_CONSTANT( Script, WM_SYSKEYDOWN );
	REGIST_CONSTANT( Script, WM_SYSKEYUP );
	REGIST_CONSTANT( Script, WM_IME_CHAR );
	REGIST_CONSTANT( Script, WM_MOUSEMOVE );
	REGIST_CONSTANT( Script, WM_LBUTTONDOWN );
	REGIST_CONSTANT( Script, WM_LBUTTONUP );
	REGIST_CONSTANT( Script, WM_LBUTTONDBLCLK );
	REGIST_CONSTANT( Script, WM_RBUTTONDOWN );
	REGIST_CONSTANT( Script, WM_RBUTTONUP );
	REGIST_CONSTANT( Script, WM_RBUTTONDBLCLK );
	REGIST_CONSTANT( Script, WM_MBUTTONDOWN );
	REGIST_CONSTANT( Script, WM_MBUTTONUP );
	REGIST_CONSTANT( Script, WM_MBUTTONDBLCLK );
	REGIST_CONSTANT( Script, WM_MOUSEWHEEL );
	REGIST_CONSTANT( Script, WM_ACTIVATE);
	REGIST_CONSTANT( Script, WM_MOUSELEAVE);

	//======================== 虚拟键 ========================
	REGIST_CONSTANT( Script, VK_LBUTTON );
	REGIST_CONSTANT( Script, VK_RBUTTON );
	REGIST_CONSTANT( Script, VK_CANCEL );
	REGIST_CONSTANT( Script, VK_MBUTTON );
	REGIST_CONSTANT( Script, VK_BACK );
	REGIST_CONSTANT( Script, VK_TAB );
	REGIST_CONSTANT( Script, VK_CLEAR );
	REGIST_CONSTANT( Script, VK_RETURN );
	REGIST_CONSTANT( Script, VK_SHIFT );
	REGIST_CONSTANT( Script, VK_CONTROL );
	REGIST_CONSTANT( Script, VK_MENU );
	REGIST_CONSTANT( Script, VK_PAUSE );
	REGIST_CONSTANT( Script, VK_CAPITAL );
	REGIST_CONSTANT( Script, VK_KANA );
	REGIST_CONSTANT( Script, VK_JUNJA );
	REGIST_CONSTANT( Script, VK_FINAL );
	REGIST_CONSTANT( Script, VK_HANJA );
	REGIST_CONSTANT( Script, VK_ESCAPE );
	REGIST_CONSTANT( Script, VK_CONVERT );
	REGIST_CONSTANT( Script, VK_NONCONVERT );
	REGIST_CONSTANT( Script, VK_ACCEPT );
	REGIST_CONSTANT( Script, VK_MODECHANGE );
	REGIST_CONSTANT( Script, VK_SPACE );
	REGIST_CONSTANT( Script, VK_PRIOR );
	REGIST_CONSTANT( Script, VK_NEXT );
	REGIST_CONSTANT( Script, VK_END );
	REGIST_CONSTANT( Script, VK_HOME );
	REGIST_CONSTANT( Script, VK_LEFT );
	REGIST_CONSTANT( Script, VK_UP );
	REGIST_CONSTANT( Script, VK_RIGHT );
	REGIST_CONSTANT( Script, VK_DOWN );
	REGIST_CONSTANT( Script, VK_SELECT );
	REGIST_CONSTANT( Script, VK_PRINT );
	REGIST_CONSTANT( Script, VK_EXECUTE );
	REGIST_CONSTANT( Script, VK_SNAPSHOT );
	REGIST_CONSTANT( Script, VK_INSERT );
	REGIST_CONSTANT( Script, VK_DELETE );
	REGIST_CONSTANT( Script, VK_HELP );
	REGIST_CONSTANT( Script, VK_LWIN );
	REGIST_CONSTANT( Script, VK_RWIN );
	REGIST_CONSTANT( Script, VK_APPS );
	REGIST_CONSTANT( Script, VK_SLEEP );
	REGIST_CONSTANT( Script, VK_NUMPAD0 );
	REGIST_CONSTANT( Script, VK_NUMPAD1 );
	REGIST_CONSTANT( Script, VK_NUMPAD2 );
	REGIST_CONSTANT( Script, VK_NUMPAD3 );
	REGIST_CONSTANT( Script, VK_NUMPAD4 );
	REGIST_CONSTANT( Script, VK_NUMPAD5 );
	REGIST_CONSTANT( Script, VK_NUMPAD6 );
	REGIST_CONSTANT( Script, VK_NUMPAD7 );
	REGIST_CONSTANT( Script, VK_NUMPAD8 );
	REGIST_CONSTANT( Script, VK_NUMPAD9 );
	REGIST_CONSTANT( Script, VK_MULTIPLY );
	REGIST_CONSTANT( Script, VK_ADD );
	REGIST_CONSTANT( Script, VK_SEPARATOR );
	REGIST_CONSTANT( Script, VK_SUBTRACT );
	REGIST_CONSTANT( Script, VK_DECIMAL );
	REGIST_CONSTANT( Script, VK_DIVIDE );
	REGIST_CONSTANT( Script, VK_F1 );
	REGIST_CONSTANT( Script, VK_F2 );
	REGIST_CONSTANT( Script, VK_F3 );
	REGIST_CONSTANT( Script, VK_F4 );
	REGIST_CONSTANT( Script, VK_F5 );
	REGIST_CONSTANT( Script, VK_F6 );
	REGIST_CONSTANT( Script, VK_F7 );
	REGIST_CONSTANT( Script, VK_F8 );
	REGIST_CONSTANT( Script, VK_F9 );
	REGIST_CONSTANT( Script, VK_F10 );
	REGIST_CONSTANT( Script, VK_F11 );
	REGIST_CONSTANT( Script, VK_F12 );
	REGIST_CONSTANT( Script, VK_F13 );
	REGIST_CONSTANT( Script, VK_F14 );
	REGIST_CONSTANT( Script, VK_F15 );
	REGIST_CONSTANT( Script, VK_F16 );
	REGIST_CONSTANT( Script, VK_F17 );
	REGIST_CONSTANT( Script, VK_F18 );
	REGIST_CONSTANT( Script, VK_F19 );
	REGIST_CONSTANT( Script, VK_F20 );
	REGIST_CONSTANT( Script, VK_F21 );
	REGIST_CONSTANT( Script, VK_F22 );
	REGIST_CONSTANT( Script, VK_F23 );
	REGIST_CONSTANT( Script, VK_F24 );
	REGIST_CONSTANT( Script, VK_NUMLOCK );
	REGIST_CONSTANT( Script, VK_SCROLL );
	REGIST_CONSTANT( Script, VK_OEM_NEC_EQUAL );
	REGIST_CONSTANT( Script, VK_OEM_FJ_JISHO );
	REGIST_CONSTANT( Script, VK_OEM_FJ_MASSHOU );
	REGIST_CONSTANT( Script, VK_OEM_FJ_TOUROKU );
	REGIST_CONSTANT( Script, VK_OEM_FJ_LOYA );
	REGIST_CONSTANT( Script, VK_OEM_FJ_ROYA );
	REGIST_CONSTANT( Script, VK_LSHIFT );
	REGIST_CONSTANT( Script, VK_RSHIFT );
	REGIST_CONSTANT( Script, VK_LCONTROL );
	REGIST_CONSTANT( Script, VK_RCONTROL );
	REGIST_CONSTANT( Script, VK_LMENU );
	REGIST_CONSTANT( Script, VK_RMENU );
	REGIST_CONSTANT( Script, VK_OEM_1 );
	REGIST_CONSTANT( Script, VK_OEM_PLUS );
	REGIST_CONSTANT( Script, VK_OEM_COMMA );
	REGIST_CONSTANT( Script, VK_OEM_MINUS );
	REGIST_CONSTANT( Script, VK_OEM_PERIOD );
	REGIST_CONSTANT( Script, VK_OEM_2 );
	REGIST_CONSTANT( Script, VK_OEM_3 );
	REGIST_CONSTANT( Script, VK_OEM_4 );
	REGIST_CONSTANT( Script, VK_OEM_5 );
	REGIST_CONSTANT( Script, VK_OEM_6 );
	REGIST_CONSTANT( Script, VK_OEM_7 );
	REGIST_CONSTANT( Script, VK_OEM_8 );
	REGIST_CONSTANT( Script, VK_OEM_AX );
	REGIST_CONSTANT( Script, VK_OEM_102 );
	REGIST_CONSTANT( Script, VK_ICO_HELP );
	REGIST_CONSTANT( Script, VK_ICO_00 );
	REGIST_CONSTANT( Script, VK_PROCESSKEY );
	REGIST_CONSTANT( Script, VK_ICO_CLEAR );
	REGIST_CONSTANT( Script, VK_OEM_RESET );
	REGIST_CONSTANT( Script, VK_OEM_JUMP );
	REGIST_CONSTANT( Script, VK_OEM_PA1 );
	REGIST_CONSTANT( Script, VK_OEM_PA2 );
	REGIST_CONSTANT( Script, VK_OEM_PA3 );
	REGIST_CONSTANT( Script, VK_OEM_WSCTRL );
	REGIST_CONSTANT( Script, VK_OEM_CUSEL );
	REGIST_CONSTANT( Script, VK_OEM_ATTN );
	REGIST_CONSTANT( Script, VK_OEM_FINISH );
	REGIST_CONSTANT( Script, VK_OEM_COPY );
	REGIST_CONSTANT( Script, VK_OEM_AUTO );
	REGIST_CONSTANT( Script, VK_OEM_ENLW );
	REGIST_CONSTANT( Script, VK_OEM_BACKTAB );
	REGIST_CONSTANT( Script, VK_ATTN );
	REGIST_CONSTANT( Script, VK_CRSEL );
	REGIST_CONSTANT( Script, VK_EXSEL );
	REGIST_CONSTANT( Script, VK_EREOF );
	REGIST_CONSTANT( Script, VK_PLAY );
	REGIST_CONSTANT( Script, VK_ZOOM );
	REGIST_CONSTANT( Script, VK_NONAME );
	REGIST_CONSTANT( Script, VK_PA1 );
	REGIST_CONSTANT( Script, VK_OEM_CLEAR );

	//======================== 控件消息 ========================
	REGIST_CONSTANT( Script, WND_NOTIFY );
	REGIST_CONSTANT( Script, EDIT_RETURN );
	REGIST_CONSTANT( Script, EDIT_CTRLUP );
	REGIST_CONSTANT( Script, EDIT_CTRLDOWN );
	REGIST_CONSTANT( Script, EDIT_CTRLRETURN );
	REGIST_CONSTANT( Script, EDIT_ONCHAR );
	REGIST_CONSTANT( Script, BUTTON_CLICKUP );
	REGIST_CONSTANT( Script, BUTTON_LCLICK );
	REGIST_CONSTANT( Script, BUTTON_RCLICK );
	REGIST_CONSTANT( Script, EDIT_BACK );
	REGIST_CONSTANT( Script, EDIT_DELETE );
	REGIST_CONSTANT( Script, EDIT_LOSEFOCUS );
	REGIST_CONSTANT( Script, BUTTON_RIGHTCLICKUP );
	REGIST_CONSTANT( Script, BUTTON_RIGHTCLICKDOWN );
	REGIST_CONSTANT( Script, BUTTON_CLICKCONT );
	REGIST_CONSTANT( Script, BUTTON_DRAG );
	REGIST_CONSTANT( Script, BUTTON_CLICKDOWN );
	REGIST_CONSTANT( Script, BUTTON_MOUSEMOVE );
	REGIST_CONSTANT( Script, SCROLL_MOVE );
	REGIST_CONSTANT( Script, SCROLL_RELEASE );
	REGIST_CONSTANT( Script, SPIN_DELTA );
	REGIST_CONSTANT( Script, ITEM_SELECT_CHANGED );
	REGIST_CONSTANT( Script, ITEM_RBUTTONUP);
	REGIST_CONSTANT( Script, ITEM_SHIFTDOWN);
	REGIST_CONSTANT( Script, ITEM_SHIFTRDOWN);
	REGIST_CONSTANT( Script, ITEM_LBUTTONUP);
	REGIST_CONSTANT( Script, ITEM_CTRLDOWN);
	REGIST_CONSTANT( Script, ITEM_LBUTTONDBLCLICK);
	REGIST_CONSTANT( Script, ITEM_LBUTTONCLICK);
	REGIST_CONSTANT( Script, ITEM_RBUTTONCLICK);
	REGIST_CONSTANT( Script, RICH_CLICK );
	REGIST_CONSTANT( Script, RICH_RCLICK );
	REGIST_CONSTANT( Script, RICH_SCROLL );
	REGIST_CONSTANT( Script, RICH_CHOOSE );
	REGIST_CONSTANT( Script, RICHWND_LCLICKUP );
	REGIST_CONSTANT( Script, RICHWND_LCLICKDOWN );
	REGIST_CONSTANT( Script, RICHWND_RCLICKUP );
	REGIST_CONSTANT( Script, RICHWND_RCLICKDOWN );
	REGIST_CONSTANT( Script, TREECTRL_RIGHTCLICK );
	REGIST_CONSTANT( Script, LISTCTRL_LEFTCLICKUP );
	REGIST_CONSTANT( Script, LISTCTRL_RIGHTCLICKUP );
	REGIST_CONSTANT( Script, LISTCTRL_LEFTCLICK );
	REGIST_CONSTANT( Script, LISTCTRL_RIGHTCLICK );
	REGIST_CONSTANT( Script, ICON_LBUTTONDBLCLICK );
	REGIST_CONSTANT( Script, WEBBROWSER_FORWARD );
	REGIST_CONSTANT( Script, WEBBROWSER_LOADCOMPLETE );
	REGIST_CONSTANT( Script, WEBBROWSER_DOCCOMPLETE );
	
	//======================== MsgBox类型 ========================
	REGIST_CONSTANT( Script, MB_BtnNone );
	REGIST_CONSTANT( Script, MB_BtnOK );
	REGIST_CONSTANT( Script, MB_BtnCancel );

	//======================== 控件Mask ========================
	REGIST_CONSTANT( Script, SM_BS_BK );

	//======================== ScrollBar ========================
	REGIST_CONSTANT( Script, SM_SC_BK );
	REGIST_CONSTANT( Script, SM_SC_P1 );
	REGIST_CONSTANT( Script, SM_SC_P2 );
	REGIST_CONSTANT( Script, SM_SC_BAR );

	//======================== Spin ========================
	REGIST_CONSTANT( Script, SM_SP_P1 );
	REGIST_CONSTANT( Script, SM_SP_P2 );

	//======================== ComboBox ========================
	REGIST_CONSTANT( Script, SM_CB_BK );
	REGIST_CONSTANT( Script, SM_CB_BT );
	REGIST_CONSTANT( Script, SM_CB_LB_BK );
	REGIST_CONSTANT( Script, SM_CB_SC_BK );
	REGIST_CONSTANT( Script, SM_CB_SC_P1 );
	REGIST_CONSTANT( Script, SM_CB_SC_P2 );
	REGIST_CONSTANT( Script, SM_CB_SC_BAR );

	//======================== ListCtrl ========================
	REGIST_CONSTANT( Script, SM_LB_BK );
	REGIST_CONSTANT( Script, SM_LB_SCH_BK );
	REGIST_CONSTANT( Script, SM_LB_SCH_P1 );
	REGIST_CONSTANT( Script, SM_LB_SCH_P2 );
	REGIST_CONSTANT( Script, SM_LB_SCH_BAR );
	REGIST_CONSTANT( Script, SM_LB_SCV_BK );
	REGIST_CONSTANT( Script, SM_LB_SCV_P1 );
	REGIST_CONSTANT( Script, SM_LB_SCV_P2 );
	REGIST_CONSTANT( Script, SM_LB_SCV_BAR );

	//======================== RichWnd ========================
	REGIST_CONSTANT( Script, SM_RW_BK );
	REGIST_CONSTANT( Script, SM_RW_SCV_BK );
	REGIST_CONSTANT( Script, SM_RW_SCV_P1 );
	REGIST_CONSTANT( Script, SM_RW_SCV_P2 );
	REGIST_CONSTANT( Script, SM_RW_SCV_BAR );

	//======================== TreeCtrl ========================
	REGIST_CONSTANT( Script, SM_TC_BK );
	REGIST_CONSTANT( Script, SM_TC_SCV_BK );
	REGIST_CONSTANT( Script, SM_TC_SCV_P1 );
	REGIST_CONSTANT( Script, SM_TC_SCV_P2 );
	REGIST_CONSTANT( Script, SM_TC_SCV_BAR );

	//======================== 状态Mask ========================
	REGIST_CONSTANT( Script, IP_MASK );
	REGIST_CONSTANT( Script, IP_MOUSEDESC );
	REGIST_CONSTANT( Script, IP_ENABLE );
	REGIST_CONSTANT( Script, IP_DISABLE );
	REGIST_CONSTANT( Script, IP_MOUSEOVER );
	REGIST_CONSTANT( Script, IP_CLICKDOWN );
	REGIST_CONSTANT( Script, IP_UNCHECK_ENABLE );
	REGIST_CONSTANT( Script, IP_UNCHECK_DISABLE );
	REGIST_CONSTANT( Script, IP_UNCHECK_MOUSEOVER );
	REGIST_CONSTANT( Script, IP_UNCHECK_CLICKDOWN );
	REGIST_CONSTANT( Script, IP_FILL );
	REGIST_CONSTANT( Script, IP_TR_ENABLE );
	REGIST_CONSTANT( Script, IP_TR_DISABLE );
	REGIST_CONSTANT( Script, IP_LS_SEL );
	REGIST_CONSTANT( Script, IP_LS_UNSEL );

	//======================== 锁定标志 ========================
	REGIST_CONSTANT( Script, LOCK_LEFT );
	REGIST_CONSTANT( Script, LOCK_RIGHT );
	REGIST_CONSTANT( Script, LOCK_TOP );
	REGIST_CONSTANT( Script, LOCK_BOTTOM );
	REGIST_CONSTANT( Script, LOCK_UNZOOM );

	//======================== 按键状态 ========================
	REGIST_CONSTANT( Script, MK_LBUTTON );
	REGIST_CONSTANT( Script, MK_RBUTTON );

	return 1;
}
