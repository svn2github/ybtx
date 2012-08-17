#include "stdafx.h"
#include "ScriptX.h"
#include "CBaseScriptApp.h"
#include "GraphicBase.h"
#include "TSqrAllocator.inl"
#include "CTextureFontMgr.h"
#include "CRenderSpace.h"
#include "CRenderSpaceNode.h"
#include "IModelHandler.h"
#include "CModel.h"
#include "RenderSettings.h"



int32 CBaseScriptApp::RegisterGraphic(CScript& Script)
{
	REGIST_ENUMERATION( Script, eLinkType, LT_CENTER );
	REGIST_ENUMERATION( Script, eLinkType, LT_SOCKET );
	REGIST_ENUMERATION( Script, eLinkType, LT_SKELETAL );
	REGIST_ENUMERATION( Script, eLinkType, LT_SKELETAL_OFFSET );
	REGIST_ENUMERATION( Script, eLinkType, LT_UNKNOW );


	//×¢²áRenderSettings
	REGIST_ISOLATED_CLASS( Script, RenderSettings );

	//×¢²áRenderSettings Ïà¹Øº¯Êý
	REGIST_CLASSFUNCTION( Script, RenderSettings, IsWindowed, ":b" );
	REGIST_CLASSFUNCTION( Script, RenderSettings, SetWindowed, "b" );
	REGIST_CLASSFUNCTION( Script, RenderSettings, GetWidth, ":I" );
	REGIST_CLASSFUNCTION( Script, RenderSettings, SetWidth, "I" );
	REGIST_CLASSFUNCTION( Script, RenderSettings, GetHeight, ":I" );
	REGIST_CLASSFUNCTION( Script, RenderSettings, SetHeight, "I" );
	REGIST_CLASSFUNCTION( Script, RenderSettings, GetUseDef, ":b");
	REGIST_CLASSFUNCTION( Script, RenderSettings, SetUseDef, "b");
	REGIST_CLASSFUNCTION( Script, RenderSettings, GetWindowWidth, ":I" );
	REGIST_CLASSFUNCTION( Script, RenderSettings, GetWindowHeight, ":I" );
	REGIST_CLASSFUNCTION( Script, RenderSettings, GetViewRatio, ":f" );
	REGIST_CLASSFUNCTION( Script, RenderSettings, GetAntiAliasQuality, ":I" );
	REGIST_CLASSFUNCTION( Script, RenderSettings, SetAntiAliasQuality, "I" );
	REGIST_CLASSFUNCTION( Script, RenderSettings, IsVSync, ":b" );
	REGIST_CLASSFUNCTION( Script, RenderSettings, SetVSync, "b" );
	REGIST_CLASSFUNCTION( Script, RenderSettings, GetColorDepth, ":I" );
	REGIST_CLASSFUNCTION( Script, RenderSettings, SetColorDepth, "I" );

	


	//×¢²áCTextureFontMgr
	REGIST_B_CLASS( Script, CTextureFontMgr );

	// ×¢²áCTextureFontMgr
	REGIST_STATICFUNCTION( Script, CTextureFontMgr, Inst, ":(CTextureFontMgr)" );
	REGIST_CLASSFUNCTION( Script, CTextureFontMgr, GetEmtStrByIdx, "s:s" );


	//×¢²áCRenderSpace
	REGIST_ABSTRACT_B_CLASS( Script, CRenderSpace );
	//×¢²áCRenderSpaceNode
	REGIST_B_CLASS( Script, CRenderSpaceNode );
	
	//×¢²áCRenderSpaceNode
	REGIST_ABSTRACT_B_CLASS( Script, CCtrlNode );
	REGIST_ABSTRACT_D_CLASS_2( Script, CRenderNode	, CCtrlNode  ,CRenderSpaceNode);
	REGIST_ABSTRACT_D_CLASS( Script, CBaseModel		, CRenderNode );
	REGIST_ABSTRACT_D_CLASS( Script, CModel			, CBaseModel );
	//REGIST_ABSTRACT_D_CLASS( Script, CModel			, CModel);



	//#endif
	//RenderParent
	REGIST_CLASSFUNCTION( Script, CRenderSpace, AddChild, "(CRenderSpaceNode)Is" );
	REGIST_CLASSFUNCTION( Script, CRenderSpace, DelChild, "(CRenderSpaceNode)" );

	//RenderChild
	REGIST_CLASSFUNCTION( Script, CRenderSpaceNode, GetAlphaValue, ":C" );
	REGIST_CLASSFUNCTION( Script, CRenderSpaceNode, SetAlphaValue, "C" );
	REGIST_CLASSFUNCTION( Script, CRenderSpaceNode, SetDiffuse, "I" );
	REGIST_CLASSFUNCTION( Script, CRenderSpaceNode, SetAmbient, "I" );

	// ×¢²áISpaceNodeHandler
	REGIST_B_CLASS( Script, ISpaceNodeHandler );
	REGIST_CALLBACKFUNCTION( Script, ISpaceNodeHandler, OnDestroyed, "" );

	// ×¢²áIModelHandler
	REGIST_B_CLASS( Script, IModelHandler );
	REGIST_CALLBACKFUNCTION( Script, IModelHandler, OnSetVisable, "b" );
	REGIST_CALLBACKFUNCTION( Script, IModelHandler, OnAnimationEnd, "s" );
	REGIST_CALLBACKFUNCTION( Script, IModelHandler, OnLinkAnimationEnd, "s" );
	REGIST_CALLBACKFUNCTION( Script, IModelHandler, OnSepAnimationEnd, "s" );
	REGIST_CALLBACKFUNCTION( Script, IModelHandler, OnKeyFrame, "s" );
	REGIST_CALLBACKFUNCTION( Script, IModelHandler, OnSepKeyFrame, "s" );
	REGIST_CALLBACKFUNCTION( Script, IModelHandler, OnResLoaded, "" );
	REGIST_CALLBACKFUNCTION( Script, IModelHandler, BeSelected, "" );

	return 1;
}
