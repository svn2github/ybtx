#include "stdafx.h"
#include "GUILayoutHandler.h"
#include "SQRGUIManager.h"
#include "StringHelper.h"
#include "CFontCfg.h"


namespace sqr
{
	/************************************************************************/
	/*       应用相关常量                                                   */
	/************************************************************************/
	// default property
	const uint32 GUILayoutHandler::DefFontSize				= 14;
	// Tag use
	const UIString GUILayoutHandler::GUILayoutElement			= "GUILayout";
	const UIString GUILayoutHandler::WindowElement				= "Window";
	const UIString GUILayoutHandler::PropertyElement			= "Property";
	const UIString GUILayoutHandler::ChildWinElement			= "ChildWin";
	const UIString GUILayoutHandler::StateElement				= "State";
	const UIString GUILayoutHandler::ImageElement				= "Image";
	const UIString GUILayoutHandler::MainItemElement			= "MainItem";
	// Property use
	const UIString GUILayoutHandler::WindowTypeAttr				= "Type";
	const UIString GUILayoutHandler::WindowNameAttr				= "Name";
	const UIString GUILayoutHandler::PropertyNameAttr			= "Name";
	const UIString GUILayoutHandler::PropertyValueAttr			= "Value";
	const UIString GUILayoutHandler::StateTypeAttr				= "Type";
	const UIString GUILayoutHandler::LayoutVersionAttr			= "Version";
	// Property name use
	const UIString GUILayoutHandler::PropertyNameClick			= "SClick";
	const UIString GUILayoutHandler::PropertyNameOpen			= "SOpen";
	const UIString GUILayoutHandler::PropertyNameClose			= "SClose";
	const UIString GUILayoutHandler::PropertyNameMouseOver		= "SMouseOver";
	const UIString GUILayoutHandler::PropertyNameWinName		= "WinName";
	const UIString GUILayoutHandler::PropertyNameWinType		= "WinType";

	const UIString GUILayoutHandler::PropertyNameWinText		= "WinText";
	const UIString GUILayoutHandler::PropertyNameWinRect		= "WinRect";
	const UIString GUILayoutHandler::PropertyNameGroupId		= "GroupId";
	const UIString GUILayoutHandler::PropertyNameFont			= "FontID";
	const UIString GUILayoutHandler::PropertyNameFontSize		= "FontSize";
	const UIString GUILayoutHandler::PropertyNameFontGCol		= "FontGColor";
	const UIString GUILayoutHandler::PropertyNameFontEffe		= "FontEffect";

	const UIString GUILayoutHandler::PropertyNameBGShow			= "BGShow";
	const UIString GUILayoutHandler::PropertyNameWinSty			= "WinStyle";
	const UIString GUILayoutHandler::PropertyNameWinTexture		= "WinTexture";
	const UIString GUILayoutHandler::PropertyNameLockFlag		= "LockFlag";
	const UIString GUILayoutHandler::PropertyNameDescInfo		= "DescInfo";
	const UIString GUILayoutHandler::PropertyNameUseClass		= "UseClass";
	const UIString GUILayoutHandler::PropertyNameCreatInfo		= "CreatInfo";
	const UIString GUILayoutHandler::PropertyNameClickFx		= "ClickFx";
	const UIString GUILayoutHandler::PropertyNameOpenFx			= "OpenFx";
	const UIString GUILayoutHandler::PropertyNameCloseFx		= "CloseFx";
	//const UIString GUILayoutHandler::PropertyNameVisable		= "Visable";//add by guo
	const UIString GUILayoutHandler::PropertyNameSource			= "Source";//add by guo

	const UIString GUILayoutHandler::PropertyNameFlsIP_MASK					= "FlsMask";
	const UIString GUILayoutHandler::PropertyNameFlsIP_MOUSEDESC			= "FlsMsDesc";
	const UIString GUILayoutHandler::PropertyNameFlsIP_ENABLE				= "FlsEnable";
	const UIString GUILayoutHandler::PropertyNameFlsIP_DISABLE				= "FlsDisable";
	const UIString GUILayoutHandler::PropertyNameFlsIP_MOUSEOVER			= "FlsMsOver";
	const UIString GUILayoutHandler::PropertyNameFlsIP_CLICKDOWN			= "FlsClickdown";
	const UIString GUILayoutHandler::PropertyNameFlsIP_UNCHECK_ENABLE		= "FlsUncheckEnable";
	const UIString GUILayoutHandler::PropertyNameFlsIP_UNCHECK_DISABLE		= "FlsUncheckDisable";
	const UIString GUILayoutHandler::PropertyNameFlsIP_UNCHECK_MOUSEOVER	= "FlsUncheckMsOver";
	const UIString GUILayoutHandler::PropertyNameFlsIP_UNCHECK_CLICKDOWN	= "FlsUncheckClickdown";
	const UIString GUILayoutHandler::PropertyNameFlsIP_FILL					= "FlsFill";
	const UIString GUILayoutHandler::PropertyNameFlsIP_TR_ENABLE			= "FlsEnable";
	const UIString GUILayoutHandler::PropertyNameFlsIP_TR_DISABLE			= "FlsDisable";
	const UIString GUILayoutHandler::PropertyNameFlsIP_LS_SEL				= "FlsLsSel";
	const UIString GUILayoutHandler::PropertyNameFlsIP_LS_UNSEL				= "FlsLsUnSel";
	const UIString GUILayoutHandler::PropertyNameFlsIP_FILL_FULL			= "FlsFillFull";
	const UIString GUILayoutHandler::PropertyNameFlsIP_LBTN_CLICK			= "FlsLbtnClick";
	const UIString GUILayoutHandler::PropertyNameFlsIP_RBTN_CLICK			= "FlsRbtnClick";

	const UIString GUILayoutHandler::PropertyNameWinRect0				= "WinRect0";
	const UIString GUILayoutHandler::PropertyStateColor					= "StateColor";
	const UIString GUILayoutHandler::PropertyRimColor					= "RimColor";
	const UIString GUILayoutHandler::PropertyTextureName				= "TextureName";
	const UIString GUILayoutHandler::PropertyTextureRect				= "TextureRect";
	const UIString GUILayoutHandler::PropertyTexturePos					= "TexturePos";
	const UIString GUILayoutHandler::PropertydwColor					= "dwColor";
	const UIString GUILayoutHandler::PropertydwLock						= "dwLock";


	GUILayoutHandler::GUILayoutHandler()
	{

	}
	GUILayoutHandler::~GUILayoutHandler()
	{

	}

	// 元素开始
	void GUILayoutHandler::ElementStart(const char* element, TiXmlElement* ElementPtr)
	{
		// 新窗体
		if ((UIString)element == WindowElement)
		{
			ElementWinStart( ElementPtr );
		}
		// 子窗体
		else if ((UIString)element == ChildWinElement)
		{
			ElementChildWinStart(ElementPtr);
		}
		// 状态
		else if ((UIString)element == StateElement)
		{
			ElementStateStart(ElementPtr);
		}
		// 图片类表资源
		else if ((UIString)element == ImageElement)
		{
			ElementImageStart(ElementPtr);
		}
		// 属性
		else if ((UIString)element == PropertyElement)
		{
			const char* ProName = ElementPtr->Attribute(PropertyNameAttr.c_str());
			ElementPropertyStart(ProName, ElementPtr);
		}
	}

	// 元素结束
	void GUILayoutHandler::ElementEnd(const char* element)
	{
		if ((UIString)element == WindowElement)
		{
			ElementWinEnd();
		}
		// 子窗体
		else if ((UIString)element == ChildWinElement)
		{
			ElementChildWinEnd();
		}
		// 状态
		else if ((UIString)element == StateElement)
		{
			ElementStateEnd();
		}
		// 图片类表资源
		else if ((UIString)element == ImageElement)
		{
			ElementImageEnd();
		}
		// 属性
		else if ((UIString)element == PropertyElement)
		{
			ElementPropertyEnd();
		}
	}

	// 新窗体函数开始
	void GUILayoutHandler::ElementWinStart(TiXmlElement* ElementPtr)
	{
		if ( m_wndResStatck.size() == 0 )
		{
			// 根窗体
			m_wndResStatck.push_back(m_root);
			m_curWndRes = m_root;
		}
		else
		{
			// 新创建的子窗体
			UIString WinName = ElementPtr->Attribute( WindowNameAttr.c_str() );

			UIMap<UIString, WndResParam*>::iterator it = m_curWndRes->m_Child.find(WinName);
			if (it!=m_curWndRes->m_Child.end())
				delete it->second;

			WndResParam& ChildWndParam = *( m_curWndRes->m_Child[WinName] = new WndResParam );
			ChildWndParam.m_beAucRes = false;

			m_wndResStatck.push_back(&ChildWndParam);
			m_curWndRes = &ChildWndParam;
		}
		// 填充窗体名称属性,不需要填充
		/*std::string winName = ElementPtr->Attribute(WindowNameAttr.c_str());
		memcpy(m_curWndRes->m_ClassName, winName.c_str(), winName.length());*/

		// 添加一个图片列表单元
		m_curWndRes->m_ImageList.resize( m_curWndRes->m_ImageList.size() + 1 );
		m_curWinState = &(m_curWndRes->m_ImageList.back());
	}
	inline void GUILayoutHandler::ElementWinEnd()
	{
		if ( m_wndResStatck.size() == 1)
		{
			// 结束了该文件的读取
			CleanUpDataLoaded();
		}
		else
		{
			m_wndResStatck.pop_back();
			m_curWndRes = m_wndResStatck.back();
		}
		m_curWinState = NULL;
	}

	// 子窗体函数
	void GUILayoutHandler::ElementChildWinStart(TiXmlElement* ElementPtr)
	{
		// 添加一个图片列表单元
		m_curWndRes->m_ImageList.resize( m_curWndRes->m_ImageList.size() + 1 );
		m_curWinState = &(m_curWndRes->m_ImageList.back());
	}
	inline void GUILayoutHandler::ElementChildWinEnd()
	{
		m_curWinState = NULL;
	}

	// 状态函数
	void GUILayoutHandler::ElementStateStart(TiXmlElement* ElementPtr)
	{
		if ( m_curWinState )
		{
			m_curWinState->ChildBkList.resize( m_curWinState->ChildBkList.size() + 1 );
			m_curImgList = &(m_curWinState->ChildBkList.back());

			integer(ElementPtr->Attribute( StateTypeAttr.c_str() ), m_curImgList->nStateMask);
		}
	}
	inline void GUILayoutHandler::ElementStateEnd()
	{
		m_curImgList = NULL;
	}

	
	// 图片函数
	void GUILayoutHandler::ElementImageStart(TiXmlElement* ElementPtr)
	{
		if ( m_curImgList )
		{
			m_curImgList->ImageList.resize( m_curImgList->ImageList.size() + 1 );
			m_curImg = &(m_curImgList->ImageList.back());
		}
	}
	inline void GUILayoutHandler::ElementImageEnd()
	{
		m_curImg = NULL;
	}

	// 属性相关
	void GUILayoutHandler::ElementPropertyStart(const char* element, TiXmlElement* ElementPtr)
	{
		if ( (UIString)element == PropertyNameClick )
		{
			UIString soundName = ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_MsgSound[eMS_Click] = soundName;
		}
		else if ( (UIString)element == PropertyNameOpen )
		{
			UIString soundName = ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_MsgSound[eMS_Open] = soundName;
		}
		else if ( (UIString)element == PropertyNameClose )
		{
			UIString soundName = ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_MsgSound[eMS_Close] = soundName;
		}
		else if ( (UIString)element == PropertyNameMouseOver )
		{
			UIString soundName = ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_MsgSound[eMS_MouseOver] = soundName;
		}
		else if ( (UIString)element == PropertyNameWinName )
		{
			std::string winName = ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_ClassName = new char[winName.length() + 1];
			memset(m_curWndRes->m_ClassName, 0, winName.length() + 1);
			memcpy(m_curWndRes->m_ClassName, winName.c_str(), winName.length());
		}
		else if ( (UIString)element == PropertyNameWinType )
		{
			std::string winType = ElementPtr->Attribute(PropertyValueAttr.c_str());
			uint32 tempType;
			uinteger(winType, tempType);
			m_curWndRes->m_WndStyle = (uint32)tempType;
		}
		else if ( (UIString)element == PropertyNameWinText )
		{
			std::string winTitle = ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_WindowTitle = new char[winTitle.length() + 1];
			memset(m_curWndRes->m_WindowTitle, 0, winTitle.length() + 1);
			memcpy(m_curWndRes->m_WindowTitle, winTitle.c_str(), winTitle.length());
		}
		else if ( (UIString)element == PropertyNameWinRect )
		{
			std::string winRect = ElementPtr->Attribute(PropertyValueAttr.c_str());
			CIRect rt;
			StringToRect(winRect, rt, true);
			
			m_curWndRes->m_x				= rt.left;
			m_curWndRes->m_y				= rt.top;
			m_curWndRes->m_Width			= rt.right - rt.left;
			m_curWndRes->m_Height		= rt.bottom - rt.top;
		}
		else if ( (UIString)element == PropertyNameGroupId )
		{
			std::string winGroup = ElementPtr->Attribute(PropertyValueAttr.c_str());
			int32 groupid;
			integer(winGroup,groupid);
			m_curWndRes->m_GroupID			= groupid;
		}
		// font
		else if ( (UIString)element == PropertyNameFont )
		{
			std::string winFontId = ElementPtr->Attribute(PropertyValueAttr.c_str());
			int32 fontid;
			integer(winFontId,fontid);
			m_curWndRes->m_nFontID			= fontid;
		}
		else if ( (UIString)element == PropertyNameFontSize )
		{
			std::string winFontSize = ElementPtr->Attribute(PropertyValueAttr.c_str());
			uint32 FontSize;
			uinteger(winFontSize,FontSize);
			m_curWndRes->m_FontSize = uint32(FontSize);
		}
		else if ( (UIString)element == PropertyNameFontGCol )
		{
			// 字体颜色
			std::string winFGCol = ElementPtr->Attribute(PropertyValueAttr.c_str());
			uint32 tempCol;
			uinteger(winFGCol, tempCol);
			m_curWndRes->m_uFontGradualColor = (uint32)tempCol;
		}
		else if ( (UIString)element == PropertyNameFontEffe )
		{
			std::string winFGEff = ElementPtr->Attribute(PropertyValueAttr.c_str());
			uint32 tempeff;
			uinteger(winFGEff, tempeff);
			m_curWndRes->m_uFontEffectMask = (uint32)tempeff;
		}
		else if ( (UIString)element == PropertyNameBGShow )
		{
			std::string BGShow = ElementPtr->Attribute(PropertyValueAttr.c_str());
			int32 tempBG;
			integer(BGShow, tempBG);
			m_curWndRes->m_isShowBackground = (tempBG == 1) ? true : false ;
		}
		else if ( (UIString)element == PropertyNameWinSty )
		{
			// 窗口类型，这时候是两种情况，一种是当前窗体的，一种是组件窗体的
			std::string winSty = ElementPtr->Attribute(PropertyValueAttr.c_str());
			uint32 tempSty;
			uinteger(winSty, tempSty);
			if (m_curWndRes->m_ImageList.size() == 1)
			{
				// 添加到 m_curWndRes
				m_curWndRes->m_uStyleIndex = (uint32)tempSty;
			}
			else
			{
				// 添加到组件窗体部分
				if (m_curWinState)
				{
					m_curWinState->m_uStyleIndex = (uint32)tempSty;
				}
			}
		}
		else if ( (UIString)element == PropertyNameWinTexture )
		{
			std::string winTexture = ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_TransTexture = new char[winTexture.length() + 1];
			memset(m_curWndRes->m_TransTexture, 0, winTexture.length() + 1);
			memcpy(m_curWndRes->m_TransTexture, winTexture.c_str(), winTexture.length());
		}
		else if ( (UIString)element == PropertyNameLockFlag )
		{
			std::string winLockFlag = ElementPtr->Attribute(PropertyValueAttr.c_str());
			uint32 LockFlag;
			uinteger(winLockFlag, LockFlag);
			if (m_curWndRes->m_ImageList.size() == 1)
			{
				// 添加到 m_curWndRes
				m_curWndRes->m_uLockFlage = (uint32)LockFlag;
			}
			else
			{
				// 添加到组件窗体部分
				if (m_curWinState)
				{
					m_curWinState->m_uLockFlag = (uint32)LockFlag;
				}
			}
		}
		else if ( (UIString)element == PropertyNameDescInfo )
		{
			std::string winDescInfo= ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_szDesc = new char[winDescInfo.length() + 1];
			memset(m_curWndRes->m_szDesc, 0, winDescInfo.length() + 1);
			memcpy(m_curWndRes->m_szDesc, winDescInfo.c_str(), winDescInfo.length());
		}
		else if ( (UIString)element == PropertyNameUseClass )
		{
			std::string winUseClass= ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_UseClassName = new char[winUseClass.length() + 1];
			memset(m_curWndRes->m_UseClassName, 0, winUseClass.length() + 1);
			memcpy(m_curWndRes->m_UseClassName, winUseClass.c_str(), winUseClass.length());
		}
		else if ( (UIString)element == PropertyNameCreatInfo )
		{
			std::string winCreatInfo= ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_UseInfo = new char[winCreatInfo.length() + 1];
			memset(m_curWndRes->m_UseInfo, 0, winCreatInfo.length() + 1);
			memcpy(m_curWndRes->m_UseInfo, winCreatInfo.c_str(), winCreatInfo.length());
		}
		else if ( (UIString)element == PropertyNameClickFx )
		{
			std::string winfx= ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_MsgFx[eMF_Click] = new char[winfx.length() + 1];
			memset(m_curWndRes->m_MsgFx[eMF_Click], 0, winfx.length() + 1);
			memcpy(m_curWndRes->m_MsgFx[eMF_Click], winfx.c_str(), winfx.length());
		}
		else if ( (UIString)element == PropertyNameOpenFx )
		{
			std::string winfx= ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_MsgFx[eMF_Open] = new char[winfx.length() + 1];
			memset(m_curWndRes->m_MsgFx[eMF_Open], 0, winfx.length() + 1);
			memcpy(m_curWndRes->m_MsgFx[eMF_Open], winfx.c_str(), winfx.length());
		}
		else if ( (UIString)element == PropertyNameCloseFx )
		{
			std::string winfx= ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_MsgFx[eMF_Close] = new char[winfx.length() + 1];
			memset(m_curWndRes->m_MsgFx[eMF_Close], 0, winfx.length()  +1);
			memcpy(m_curWndRes->m_MsgFx[eMF_Close], winfx.c_str(), winfx.length());
		}
		else if ( (UIString)element == PropertyNameSource )//add by guo
		{
			std::string sourceName = ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_SourceName = new char[sourceName.length() + 1];
			memset(m_curWndRes->m_SourceName, 0, sourceName.length() + 1);
			memcpy(m_curWndRes->m_SourceName, sourceName.c_str(), sourceName.length());
		}
		else if ((UIString)element == PropertyNameFlsIP_MASK)
		{
			UIString winfls= ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_FlashWndStr.insert(pair<EventStateMask, UIString>(IP_MASK, winfls));
		}
		else if ((UIString)element == PropertyNameFlsIP_MOUSEDESC)
		{
			UIString winfls= ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_FlashWndStr.insert(pair<EventStateMask, UIString>(IP_MOUSEDESC, winfls));
		}
		else if ((UIString)element == PropertyNameFlsIP_ENABLE)
		{
			UIString winfls= ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_FlashWndStr.insert(pair<EventStateMask, UIString>(IP_ENABLE, winfls));
		}
		else if ((UIString)element == PropertyNameFlsIP_DISABLE)
		{
			UIString winfls= ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_FlashWndStr.insert(pair<EventStateMask, UIString>(IP_DISABLE, winfls));
		}
		else if ((UIString)element == PropertyNameFlsIP_MOUSEOVER)
		{
			UIString winfls= ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_FlashWndStr.insert(pair<EventStateMask, UIString>(IP_MOUSEOVER, winfls));
		}
		else if ((UIString)element == PropertyNameFlsIP_CLICKDOWN)
		{
			UIString winfls= ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_FlashWndStr.insert(pair<EventStateMask, UIString>(IP_CLICKDOWN, winfls));
		}
		else if ((UIString)element == PropertyNameFlsIP_UNCHECK_ENABLE)
		{
			UIString winfls= ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_FlashWndStr.insert(pair<EventStateMask, UIString>(IP_UNCHECK_ENABLE, winfls));
		}
		else if ((UIString)element == PropertyNameFlsIP_UNCHECK_DISABLE)
		{
			UIString winfls= ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_FlashWndStr.insert(pair<EventStateMask, UIString>(IP_UNCHECK_DISABLE, winfls));
		}
		else if ((UIString)element == PropertyNameFlsIP_UNCHECK_MOUSEOVER)
		{
			UIString winfls= ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_FlashWndStr.insert(pair<EventStateMask, UIString>(IP_UNCHECK_MOUSEOVER, winfls));
		}
		else if ((UIString)element == PropertyNameFlsIP_UNCHECK_CLICKDOWN)
		{
			UIString winfls= ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_FlashWndStr.insert(pair<EventStateMask, UIString>(IP_UNCHECK_CLICKDOWN, winfls));
		}
		else if ((UIString)element == PropertyNameFlsIP_FILL)
		{
			UIString winfls= ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_FlashWndStr.insert(pair<EventStateMask, UIString>(IP_FILL, winfls));
		}
		else if ((UIString)element == PropertyNameFlsIP_TR_ENABLE)
		{
			UIString winfls= ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_FlashWndStr.insert(pair<EventStateMask, UIString>(IP_TR_ENABLE, winfls));
		}
		else if ((UIString)element == PropertyNameFlsIP_TR_DISABLE)
		{
			UIString winfls= ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_FlashWndStr.insert(pair<EventStateMask, UIString>(IP_TR_DISABLE, winfls));
		}
		else if ((UIString)element == PropertyNameFlsIP_LS_SEL)
		{
			UIString winfls= ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_FlashWndStr.insert(pair<EventStateMask, UIString>(IP_LS_SEL, winfls));
		}
		else if ((UIString)element == PropertyNameFlsIP_LS_UNSEL)
		{
			UIString winfls= ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_FlashWndStr.insert(pair<EventStateMask, UIString>(IP_LS_UNSEL, winfls));
		}
		else if ((UIString)element == PropertyNameFlsIP_FILL_FULL)
		{
			UIString winfls= ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_FlashWndStr.insert(pair<EventStateMask, UIString>(IP_FILL_FULL, winfls));
		}
		else if ((UIString)element == PropertyNameFlsIP_LBTN_CLICK)
		{
			UIString winfls= ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_FlashWndStr.insert(pair<EventStateMask, UIString>(IP_LBTNCLICK_ACTION, winfls));
		}
		else if ((UIString)element == PropertyNameFlsIP_RBTN_CLICK)
		{
			UIString winfls= ElementPtr->Attribute(PropertyValueAttr.c_str());
			m_curWndRes->m_FlashWndStr.insert(pair<EventStateMask, UIString>(IP_RBTNCLICK_ACTION, winfls));
		}
		else if ( (UIString)element == PropertyNameWinRect0 )
		{
			std::string winRect0 = ElementPtr->Attribute(PropertyValueAttr.c_str());
			CIRect rt;
			StringToRect(winRect0, rt, true);

			m_curWinState->rtWnd = CFRect( float(rt.left)
										 , float(rt.top)
										 , float( rt.right)
										 , float( rt.bottom) 
										 );
		}
		else if ( (UIString)element == PropertyStateColor )
		{
			std::string sStateCol = ElementPtr->Attribute(PropertyValueAttr.c_str());
			uint32 iStateCol;
			uinteger(sStateCol, iStateCol);
			if (m_curImgList)
			{
				// 添加到 m_curWndRes
				m_curImgList->uTextColor = (uint32)iStateCol;
			}
		}
		else if ( (UIString)element == PropertyRimColor )
		{
			std::string sRimCol = ElementPtr->Attribute(PropertyValueAttr.c_str());
			uint32 iRimCol;
			uinteger(sRimCol, iRimCol);
			if (m_curImgList)
			{
				// 添加到 m_curWndRes
				m_curImgList->uRimColor = (uint32)iRimCol;
			}
		}
		else if ( (UIString)element == PropertyTextureName )
		{
			std::string texName= ElementPtr->Attribute(PropertyValueAttr.c_str());
			if (m_curImg)
			{
				m_curImg->szTexture = texName.c_str();
			}
		}
		else if ( (UIString)element == PropertyTextureRect )
		{
			std::string TexRect = ElementPtr->Attribute(PropertyValueAttr.c_str());
			CIRect rt;
			StringToRect(TexRect, rt);
			if (m_curImg)
			{
				m_curImg->rtScale.top		= rt.top;
				m_curImg->rtScale.bottom	= rt.bottom;
				m_curImg->rtScale.left		= rt.left;
				m_curImg->rtScale.right		= rt.right;
			}
		}
		else if ( (UIString)element == PropertyTexturePos )
		{
			std::string TexPos = ElementPtr->Attribute(PropertyValueAttr.c_str());
			int x,y;
			
			int pos;
			string sub, subend;
			pos		= TexPos.find_first_of(',');
			sub		= TexPos.substr(0,pos);
			subend	= TexPos.substr(pos + 1, TexPos.length() - pos - 1);
			integer(sub, x);
			integer(subend, y);

			if (m_curImg)
			{
				m_curImg->posWnd.x = (float)x;
				m_curImg->posWnd.y = (float)y;
			}
		}
		else if ( (UIString)element == PropertydwColor )
		{
			std::string sdwCol = ElementPtr->Attribute(PropertyValueAttr.c_str());
			uint32 idwCol;
			uinteger(sdwCol, idwCol);
			if (m_curImg)
			{
				// 添加到 m_curWndRes
				m_curImg->dwColor = (uint32)idwCol;
			}
		}
		else if ( (UIString)element == PropertydwLock )
		{
			std::string sdwLock = ElementPtr->Attribute(PropertyValueAttr.c_str());
			uint32 idwLock;
			uinteger(sdwLock, idwLock);
			if (m_curImg)
			{
				// 添加到 m_curWndRes
				m_curImg->dwLockFlage = (uint32)idwLock;
			}
		}

	}
	inline void GUILayoutHandler::ElementPropertyEnd()
	{
		return;
	}

	void GUILayoutHandler::StringToRect(string& str, CIRect& re, bool beChange)
	{
		int pos;
		string sub, subend;
		pos = str.find_first_of(',');
		sub = str.substr(0,pos);
		subend = str.substr(pos + 1, str.length() - pos - 1);
		if (beChange)
		{
			integer(sub, re.left);
		}
		else
		{
			integer(sub, re.top);
		}
		

		pos = subend.find_first_of(',');
		sub = subend.substr(0,pos);
		subend = subend.substr(pos + 1, subend.length() - pos - 1);
		if (beChange)
		{
			integer(sub, re.top);
		}
		else
		{
			integer(sub, re.bottom);
		}

		pos = subend.find_first_of(',');
		sub = subend.substr(0,pos);
		subend = subend.substr(pos + 1, subend.length() - pos - 1);
		int w;
		integer(sub, w);
		if (beChange)
		{
			re.right = re.left + w;
		}
		else
		{
			re.left = w;
		}
		

		int h;
		integer(subend, h);
		if (beChange)
		{
			re.bottom = re.top + h;
		}
		else
		{
			re.right = h;
		}
		
		return;
	}




	// 设置根参数
	void GUILayoutHandler::SetRootWndResParam(WndResParam* WndParam)
	{
		CleanUpDataLoaded();
		m_root = WndParam;
		return;
	}

	// 清空当前数据
	void GUILayoutHandler::CleanUpDataLoaded(void)
	{
		m_root = NULL;
		m_curWndRes = m_root;
		m_wndResStatck.clear();
		m_curWinState = NULL;
		m_curImgList = NULL;
		m_curImg = NULL;
	}


	// SQRWnd 到 Element 的转换
	void GUILayoutHandler::CWnd2Element(SQRWnd* pWnd, TiXmlElement* element, string& classname, string& creatinfo)
	{
		string tempStr;

		// save name property
		string szClass( pWnd->GetClassName() ) ;
		if ( (int)strlen( pWnd->GetClassName() ) == 0 )
		{
			// no name, return
			return;
		}
		AddPropertyElement(PropertyNameWinName.c_str(), szClass.c_str(), element);

		// save sound property
		for( int i = 0; i < eMS_Count; ++i )
		{
			ESoundMsg eSoundType = static_cast<ESoundMsg>(i);
			AddSoundPro(eSoundType, pWnd, element);
		}

		// save text property if have
		int		WndTextLen		= (int)strlen( pWnd->GetWndText() );
		string szWndText = string( pWnd->GetWndText() );
		if (WndTextLen != 0)
		{
			AddPropertyElement(PropertyNameWinText.c_str(), szWndText.c_str(), element);
		}

		// save style property
		uint32	wsStyle			= (uint32)(pWnd->GetStyle());
		tostring(tempStr, wsStyle);
		ReBuiltU32Str( tempStr );
		AddPropertyElement(PropertyNameWinType.c_str(), tempStr.c_str(), element);

		// save the rect
		AddRectPro(pWnd, element);

		// save group id
		int32	GroupID			= pWnd->GetGroupID();
		if (GroupID != -1)
		{
			tostring(tempStr, GroupID);
			AddPropertyElement(PropertyNameGroupId.c_str(), tempStr.c_str(), element);
		}

		// save font id
		int32     nFontId		= pWnd->GetFontID();
		tostring(tempStr, nFontId);
		ReBuiltU32Str( tempStr );
		AddPropertyElement(PropertyNameFont.c_str(), tempStr.c_str(), element);

		// save font size
		int		nFontSize		= (int)(pWnd->GetFontSize()/CFontCfg::GetInst()->GetFontZoom() + 0.5f);
		if (nFontSize != DefFontSize)
		{
			tostring(tempStr, nFontSize);
			ReBuiltU32Str( tempStr );
			AddPropertyElement(PropertyNameFontSize.c_str(), tempStr.c_str(), element);
		}

		// save font gradual
		uint32 fontGradualColor	= (uint32)(pWnd->GetFontGradualColor());
		if (fontGradualColor != 0)
		{
			tostring(tempStr, fontGradualColor);
			ReBuiltU32Str( tempStr );
			AddPropertyElement(PropertyNameFontGCol.c_str(), tempStr.c_str(), element);
		}

		// save font effect
		uint32 fontEffectMask	= (uint32)(pWnd->GetFontEffectMask());
		tostring(tempStr, fontEffectMask);
		ReBuiltU32Str( tempStr );
		AddPropertyElement(PropertyNameFontEffe.c_str(), tempStr.c_str(), element);


		// save the win bg is show or nor
		bool  bShow				= pWnd->IsDrawBackground();
		if ( !bShow )
		{
			tempStr = "0";
			AddPropertyElement(PropertyNameBGShow.c_str(), tempStr.c_str(), element);
		}
		
		// save the win style index
		uint32 StyleIndex		= (uint32)(pWnd->GetStyleIndex());
		if (StyleIndex != 0)
		{
			tostring(tempStr, StyleIndex);
			ReBuiltU32Str( tempStr );
			AddPropertyElement(PropertyNameWinSty.c_str(), tempStr.c_str(), element);
		}

		// save texture
		if (pWnd->GetWndHandle()->m_strTransName.size() > 0)
		{
			tempStr = pWnd->GetWndHandle()->m_strTransName.c_str();
			AddPropertyElement(PropertyNameWinTexture.c_str(), tempStr.c_str(), element);
			AddResData(eRes_T_GUI, tempStr);
		}

		// save lock flag 
		uint32  uLockFlag     = (uint32)(pWnd->GetLockFlag());
		tostring(tempStr, uLockFlag);
		ReBuiltU32Str( tempStr );
		AddPropertyElement(PropertyNameLockFlag.c_str(), tempStr.c_str(), element);

		// save desc info
		int     nDescLen		= (int)strlen( pWnd->GetMouseOverDesc() );
		string szDescInfo( pWnd->GetMouseOverDesc() );
		if (nDescLen != 0)
		{
			AddPropertyElement(PropertyNameDescInfo.c_str(), szDescInfo.c_str(), element);
		}

		AddPropertyElement(PropertyNameUseClass.c_str(), classname.c_str(), element);
		if (creatinfo.length())
		{
			AddPropertyElement(PropertyNameCreatInfo.c_str(), creatinfo.c_str(), element);
		}

		// save fx
		int     nClickFxLen		= (int)strlen( pWnd->GetMsgFx(eMF_Click) );
		string szClickFxInfo( pWnd->GetMsgFx(eMF_Click) );
		if (nClickFxLen != 0)
		{
			AddPropertyElement(PropertyNameClickFx.c_str(), szClickFxInfo.c_str(), element);
		}

		int     nOpenFxLen		= (int)strlen( pWnd->GetMsgFx(eMF_Open) );
		string szOpenFxInfo( pWnd->GetMsgFx(eMF_Open) );
		if (nOpenFxLen != 0)
		{
			AddPropertyElement(PropertyNameOpenFx.c_str(), szOpenFxInfo.c_str(), element);
		}

		int     nCloseFxLen		= (int)strlen( pWnd->GetMsgFx(eMF_Close) );
		string szCloseFxInfo( pWnd->GetMsgFx(eMF_Close) );
		if (nCloseFxLen != 0)
		{
			AddPropertyElement(PropertyNameCloseFx.c_str(), szCloseFxInfo.c_str(), element);
		}

		int     nFlashlLen		= (int)strlen( pWnd->GetFlashInfo(IP_MASK) );
		string szFlashInfo( pWnd->GetFlashInfo(IP_MASK) );
		if (nFlashlLen != 0)
		{
			AddPropertyElement(PropertyNameFlsIP_MASK.c_str(), szFlashInfo.c_str(), element);
		}

		nFlashlLen		= (int)strlen( pWnd->GetFlashInfo(IP_MOUSEDESC) );
		szFlashInfo = pWnd->GetFlashInfo(IP_MOUSEDESC);
		if (nFlashlLen != 0)
		{
			AddPropertyElement(PropertyNameFlsIP_MOUSEDESC.c_str(), szFlashInfo.c_str(), element);
		}

		nFlashlLen		= (int)strlen( pWnd->GetFlashInfo(IP_ENABLE) );
		szFlashInfo = pWnd->GetFlashInfo(IP_ENABLE);
		if (nFlashlLen != 0)
		{
			AddPropertyElement(PropertyNameFlsIP_ENABLE.c_str(), szFlashInfo.c_str(), element);
		}

		nFlashlLen		= (int)strlen( pWnd->GetFlashInfo(IP_DISABLE) );
		szFlashInfo = pWnd->GetFlashInfo(IP_DISABLE);
		if (nFlashlLen != 0)
		{
			AddPropertyElement(PropertyNameFlsIP_DISABLE.c_str(), szFlashInfo.c_str(), element);
		}

		nFlashlLen		= (int)strlen( pWnd->GetFlashInfo(IP_MOUSEOVER) );
		szFlashInfo = pWnd->GetFlashInfo(IP_MOUSEOVER);
		if (nFlashlLen != 0)
		{
			AddPropertyElement(PropertyNameFlsIP_MOUSEOVER.c_str(), szFlashInfo.c_str(), element);
		}

		nFlashlLen		= (int)strlen( pWnd->GetFlashInfo(IP_CLICKDOWN) );
		szFlashInfo = pWnd->GetFlashInfo(IP_CLICKDOWN);
		if (nFlashlLen != 0)
		{
			AddPropertyElement(PropertyNameFlsIP_CLICKDOWN.c_str(), szFlashInfo.c_str(), element);
		}

		nFlashlLen		= (int)strlen( pWnd->GetFlashInfo(IP_UNCHECK_ENABLE) );
		szFlashInfo = pWnd->GetFlashInfo(IP_UNCHECK_ENABLE);
		if (nFlashlLen != 0)
		{
			AddPropertyElement(PropertyNameFlsIP_UNCHECK_ENABLE.c_str(), szFlashInfo.c_str(), element);
		}

		nFlashlLen		= (int)strlen( pWnd->GetFlashInfo(IP_UNCHECK_DISABLE) );
		szFlashInfo = pWnd->GetFlashInfo(IP_UNCHECK_DISABLE);
		if (nFlashlLen != 0)
		{
			AddPropertyElement(PropertyNameFlsIP_UNCHECK_DISABLE.c_str(), szFlashInfo.c_str(), element);
		}

		nFlashlLen		= (int)strlen( pWnd->GetFlashInfo(IP_UNCHECK_MOUSEOVER) );
		szFlashInfo = pWnd->GetFlashInfo(IP_UNCHECK_MOUSEOVER);
		if (nFlashlLen != 0)
		{
			AddPropertyElement(PropertyNameFlsIP_UNCHECK_MOUSEOVER.c_str(), szFlashInfo.c_str(), element);
		}

		nFlashlLen		= (int)strlen( pWnd->GetFlashInfo(IP_UNCHECK_CLICKDOWN) );
		szFlashInfo = pWnd->GetFlashInfo(IP_UNCHECK_CLICKDOWN);
		if (nFlashlLen != 0)
		{
			AddPropertyElement(PropertyNameFlsIP_UNCHECK_CLICKDOWN.c_str(), szFlashInfo.c_str(), element);
		}

		nFlashlLen		= (int)strlen( pWnd->GetFlashInfo(IP_FILL) );
		szFlashInfo = pWnd->GetFlashInfo(IP_FILL);
		if (nFlashlLen != 0)
		{
			AddPropertyElement(PropertyNameFlsIP_FILL.c_str(), szFlashInfo.c_str(), element);
		}

		nFlashlLen		= (int)strlen( pWnd->GetFlashInfo(IP_TR_ENABLE) );
		szFlashInfo = pWnd->GetFlashInfo(IP_TR_ENABLE);
		if (nFlashlLen != 0)
		{
			AddPropertyElement(PropertyNameFlsIP_TR_ENABLE.c_str(), szFlashInfo.c_str(), element);
		}

		nFlashlLen		= (int)strlen( pWnd->GetFlashInfo(IP_TR_DISABLE) );
		szFlashInfo = pWnd->GetFlashInfo(IP_TR_DISABLE);
		if (nFlashlLen != 0)
		{
			AddPropertyElement(PropertyNameFlsIP_TR_DISABLE.c_str(), szFlashInfo.c_str(), element);
		}

		nFlashlLen		= (int)strlen( pWnd->GetFlashInfo(IP_LS_SEL) );
		szFlashInfo = pWnd->GetFlashInfo(IP_LS_SEL);
		if (nFlashlLen != 0)
		{
			AddPropertyElement(PropertyNameFlsIP_LS_SEL.c_str(), szFlashInfo.c_str(), element);
		}

		nFlashlLen		= (int)strlen( pWnd->GetFlashInfo(IP_LS_UNSEL) );
		szFlashInfo = pWnd->GetFlashInfo(IP_LS_UNSEL);
		if (nFlashlLen != 0)
		{
			AddPropertyElement(PropertyNameFlsIP_LS_UNSEL.c_str(), szFlashInfo.c_str(), element);
		}

		nFlashlLen		= (int)strlen( pWnd->GetFlashInfo(IP_FILL_FULL) );
		szFlashInfo = pWnd->GetFlashInfo(IP_FILL_FULL);
		if (nFlashlLen != 0)
		{
			AddPropertyElement(PropertyNameFlsIP_FILL_FULL.c_str(), szFlashInfo.c_str(), element);
		}
		
		nFlashlLen		= (int)strlen( pWnd->GetFlashInfo(IP_LBTNCLICK_ACTION) );
		szFlashInfo = pWnd->GetFlashInfo(IP_LBTNCLICK_ACTION);
		if (nFlashlLen != 0)
		{
			AddPropertyElement(PropertyNameFlsIP_LBTN_CLICK.c_str(), szFlashInfo.c_str(), element);
		}

		nFlashlLen		= (int)strlen( pWnd->GetFlashInfo(IP_RBTNCLICK_ACTION) );
		szFlashInfo = pWnd->GetFlashInfo(IP_RBTNCLICK_ACTION);
		if (nFlashlLen != 0)
		{
			AddPropertyElement(PropertyNameFlsIP_RBTN_CLICK.c_str(), szFlashInfo.c_str(), element);
		}

		// save source property if have
		int	SourceLen		= (int)strlen( pWnd->GetszSourceName() );//add by guo
		string szSource = string( pWnd->GetszSourceName() );
		if (SourceLen != 0)
		{
			AddPropertyElement(PropertyNameSource.c_str(), szSource.c_str(), element);
		}

		// 保存主窗体和子窗体信息
		for( int nCtrlMask = 1; ; nCtrlMask++ )
		{
			::sqr::SQRWnd* pChild = pWnd->GetResWnd( nCtrlMask );
			if( !pChild )
				break;

			TiXmlElement* temp_element = element;
			if( nCtrlMask > 1 )
			{
				// 有子窗体，创建节点
				TiXmlElement Elem( ChildWinElement.c_str() );
				element->InsertEndChild(Elem);

				temp_element = element->LastChild()->ToElement();

				AddRectPro(pChild, temp_element, true);



				// save the win style index
				uint32 ChildStyle	 = (uint32)(pChild->GetStyleIndex()); 
				tostring(tempStr, ChildStyle);
				ReBuiltU32Str( tempStr );
				AddPropertyElement(PropertyNameWinSty.c_str(), tempStr.c_str(), temp_element);

				// save lock flag 
				uint32 LockFlag		 = (uint32)(pChild->GetLockFlag());
				tostring(tempStr, LockFlag);
				ReBuiltU32Str( tempStr );
				AddPropertyElement(PropertyNameLockFlag.c_str(), tempStr.c_str(), temp_element);
			}

			//不同状态XML记录
			IMAGE_PARAM IP ( 1, 0 );
			for( IP.StateMask = IP_MASK; IP.StateMask < IP_COUNT; IP.StateMask++ )
			{
				WND_IMAGE_LIST* pWI = pChild->GetWndBkImage( IP );
				if( pWI )
				{
					// 创建该状态
					TiXmlElement Elem( StateElement.c_str() );

					int nStateMask = IP.StateMask;
					tostring(tempStr, nStateMask);
					ReBuiltU32Str( tempStr );
					Elem.SetAttribute(StateTypeAttr.c_str(), tempStr.c_str());


					int nImageCount = pWI->GetImageCount();
					uint32 nStateColor = (uint32)(pChild->GetWndTextColor( IP ));
					uint32 uRimColor = (uint32)(pChild->GetWndTextBcColor( IP ));
					

					// 状态颜色
					if (nStateColor != 0)
					{
						tostring(tempStr, nStateColor);
						ReBuiltU32Str( tempStr );
						AddPropertyElement(PropertyStateColor.c_str(), tempStr.c_str(), &Elem);
					}

					// 文字边框颜色
					if (uRimColor != 0)
					{
						tostring(tempStr, uRimColor);
						ReBuiltU32Str( tempStr );
						AddPropertyElement(PropertyRimColor.c_str(), tempStr.c_str(), &Elem);	
					}

					for( int i = 0; i < nImageCount; i++ )
					{
						TiXmlElement ImgElem( ImageElement.c_str() );

						IMAGE& Image = pWI->GetImage(i);
						string	TextName  = Image.GetTexName();
						slash_to_backslash(TextName);

						if (TextName.length() != 0)
						{
							AddPropertyElement(PropertyTextureName.c_str(), TextName.c_str(), &ImgElem);
							AddResData(eRes_T_GUI, TextName);
						}

						AddPropTextureRect(Image, ImgElem);
						AddPropTexturePos(Image, ImgElem);

						tostring(tempStr, Image.dwColor);
						ReBuiltU32Str( tempStr );
						AddPropertyElement(PropertydwColor.c_str(), tempStr.c_str(), &ImgElem);

						if (Image.dwLockFlag != 0)
						{
							tostring(tempStr, Image.dwLockFlag);
							ReBuiltU32Str( tempStr );
							AddPropertyElement(PropertydwLock.c_str(), tempStr.c_str(), &ImgElem);
						}


						// 添加图片信息节点
						Elem.InsertEndChild(ImgElem);
					}
					// 添加子信息节点
					temp_element->InsertEndChild(Elem);
				}
			}
		}


	}

	void GUILayoutHandler::AddPropTexturePos(IMAGE& Image, TiXmlElement& ImgElem)
	{
		int32 x = (int32)(Image.posWnd.x > 0.0f ? Image.posWnd.x + 0.5f : Image.posWnd.x - 0.5f);
		int32 y = (int32)(Image.posWnd.y > 0.0f ? Image.posWnd.y + 0.5f : Image.posWnd.y - 0.5f);

		if ( !(x == 0 && y == 0) )
		{
			string tempstr,val;

			tostring(tempstr, x);
			ReBuiltU32Str( tempstr );
			val = tempstr + ",";
			tostring(tempstr, y);
			ReBuiltU32Str( tempstr );
			val += tempstr;
			AddPropertyElement(PropertyTexturePos.c_str(), val.c_str(), &ImgElem);
		}
		
	}


	void GUILayoutHandler::AddPropTextureRect(IMAGE& Image, TiXmlElement& ImgElem)
	{
		int32 t = (int32)(Image.rtTexture.top > 0.0f ? Image.rtTexture.top + 0.5f : Image.rtTexture.top - 0.5f);
		int32 b = (int32)(Image.rtTexture.bottom > 0.0f ? Image.rtTexture.bottom + 0.5f : Image.rtTexture.bottom - 0.5f);
		int32 l = (int32)(Image.rtTexture.left > 0.0f ? Image.rtTexture.left + 0.5f : Image.rtTexture.left - 0.5f);
		int32 r = (int32)(Image.rtTexture.right > 0.0f ? Image.rtTexture.right + 0.5f : Image.rtTexture.right - 0.5f);

		string tempstr,val;

		tostring(tempstr, t);
		ReBuiltU32Str( tempstr );
		val = tempstr + ",";
		tostring(tempstr, b);
		ReBuiltU32Str( tempstr );
		val += tempstr;		val += ",";
		tostring(tempstr, l);
		ReBuiltU32Str( tempstr );
		val += tempstr;		val += ",";
		tostring(tempstr, r);
		ReBuiltU32Str( tempstr );
		val += tempstr;

		AddPropertyElement(PropertyTextureRect.c_str(), val.c_str(), &ImgElem);

	}

	void GUILayoutHandler::ReBuiltU32Str(string& val)
	{
		size_t pos = val.find_first_of(',');
		while( pos != string::npos)
		{
			val.erase(pos,1);
			pos = val.find_first_of(',');
		}
	}

	void GUILayoutHandler::AddPropertyElement(const char* name,const char* val,TiXmlElement* parent)
	{
		TiXmlElement Elem( PropertyElement.c_str() );
		
		// 设置属性
		Elem.SetAttribute(PropertyNameAttr.c_str(), name);
		Elem.SetAttribute(PropertyValueAttr.c_str(), val);

		parent->InsertEndChild(Elem);
	}

	void GUILayoutHandler::AddRectPro(SQRWnd* pWnd, TiXmlElement* element, bool be_zero)
	{
		string tempstr,val;
		CFRect rt;
		pWnd->GetWndRect( rt );
		if (pWnd->GetParent() && !be_zero)
		{
			pWnd->GetParent()->ScreenToWnd( rt );
		}

		int		x				= (int)(rt.left > 0.0f ? rt.left + 0.5f : rt.left - 0.5f);
		int		y				= (int)(rt.top > 0.0f ? rt.top + 0.5f : rt.top - 0.5f);
		int		w				= (int)(rt.Width() > 0.0f ? rt.Width() + 0.5f : rt.Width() - 0.5f);
		int		h				= (int)(rt.Height() > 0.0f ? rt.Height() + 0.5f : rt.Height() - 0.5f);	

		tostring(tempstr, x);
		ReBuiltU32Str( tempstr );
		val = tempstr + ",";
		tostring(tempstr, y);
		ReBuiltU32Str( tempstr );
		val += tempstr;		val += ",";
		tostring(tempstr, w);
		ReBuiltU32Str( tempstr );
		val += tempstr;		val += ",";
		tostring(tempstr, h);
		ReBuiltU32Str( tempstr );
		val += tempstr;

		if (be_zero)
		{
			AddPropertyElement(PropertyNameWinRect0.c_str(), val.c_str(), element);
		}
		else
		{
			AddPropertyElement(PropertyNameWinRect.c_str(), val.c_str(), element);
		}
		
	}

	void GUILayoutHandler::AddSoundPro(ESoundMsg ES, SQRWnd* pWnd, TiXmlElement* element)
	{
		string szSoundInfo( pWnd->GetMsgSound(ES) );
		int nSoundLen = szSoundInfo.length();
		
		switch ( ES )
		{
		case eMS_Click:
			if (nSoundLen != 0)
			{
				AddPropertyElement(PropertyNameClick.c_str(), szSoundInfo.c_str(), element);
			}
			break;
		case eMS_Open:
			if (nSoundLen != 0)
			{
				AddPropertyElement(PropertyNameOpen.c_str(), szSoundInfo.c_str(), element);
			}
			break;
		case eMS_Close:
			if (nSoundLen != 0)
			{
				AddPropertyElement(PropertyNameClose.c_str(), szSoundInfo.c_str(), element);
			}
			break;
		case eMS_MouseOver:
			if (nSoundLen != 0)
			{
				AddPropertyElement(PropertyNameMouseOver.c_str(), szSoundInfo.c_str(), element);
			}
			break;
		default:
			break;
		}

	}

}
