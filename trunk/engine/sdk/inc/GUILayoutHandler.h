#pragma once

#include "stdafx.h"
#include "GUIXmlHandler.h"
#include "GUIXmlResSaver.h"

namespace sqr
{
	class GUI_API GUILayoutHandler 
		: public XMLHandler
		, public GUIXmlResSaverClass
	{
		// 构造与析构函数
	public:
		GUILayoutHandler();
		virtual ~GUILayoutHandler(void);

		/************************************************************************/
		/*       应用相关常量                                                   */
		/************************************************************************/
		// UI default property
		static const uint32 DefFontSize;				// default property of font size
		// Tag use
		static const UIString GUILayoutElement;			// Tag for Layout elements.
		static const UIString WindowElement;			// Tag for Window elements.
		static const UIString PropertyElement;			// Tag for Property elements.
		static const UIString ChildWinElement;			// Tag for Child win elements.
		static const UIString StateElement;				// Tag for State elements.
		static const UIString ImageElement;				// Tag for Image elements.
		static const UIString MainItemElement;			// Tag for Main item elements.
		// Property use
		static const UIString WindowTypeAttr;				// Attribute for Window type.
		static const UIString WindowNameAttr;				// Attribute for Window name.
		static const UIString PropertyNameAttr;			// Attribute for Property name.
		static const UIString PropertyValueAttr;			// Attribute for Property value.
		static const UIString StateTypeAttr;				// Attribute for state type.
		static const UIString LayoutVersionAttr;			// Attribute for layout version.
		// Property name use
		static const UIString PropertyNameClick;			// Attribute for Property name click sound.
		static const UIString PropertyNameOpen;			// Attribute for Property name open sound.
		static const UIString PropertyNameClose;			// Attribute for Property name close sound.
		static const UIString PropertyNameMouseOver;		// Attribute for Property name mouse over sound.
		static const UIString PropertyNameWinName;		// Attribute for Property name WinName.
		static const UIString PropertyNameWinType;		// Attribute for Property name WinType.

		static const UIString PropertyNameWinText;		// Attribute for Property name WinText.
		static const UIString PropertyNameWinRect;		// Attribute for Property name WinRect.
		static const UIString PropertyNameGroupId;		// Attribute for Property name GroupId.
		static const UIString PropertyNameFont;			// Attribute for Property name Font.
		static const UIString PropertyNameFontSize;		// Attribute for Property name FontSize.
		static const UIString PropertyNameFontGCol;		// Attribute for Property name FontGCol.
		static const UIString PropertyNameFontEffe;		// Attribute for Property name FontEffe.

		static const UIString PropertyNameBGShow;			// Attribute for Property name BGShow.
		static const UIString PropertyNameWinSty;			// Attribute for Property name WinSty.
		static const UIString PropertyNameWinTexture;		// Attribute for Property name WinTexture.
		static const UIString PropertyNameLockFlag;		// Attribute for Property name LockFlag.
		static const UIString PropertyNameDescInfo;		// Attribute for Property name DescInfo.
		static const UIString PropertyNameUseClass;		// Attribute for Property name UseClass.
		static const UIString PropertyNameCreatInfo;		// Attribute for Property name CreatInfo.
		static const UIString PropertyNameClickFx;		// Attribute for Property name ClickFx.
		static const UIString PropertyNameOpenFx;			// Attribute for Property name OpenFx.
		static const UIString PropertyNameCloseFx;		// Attribute for Property name CloseFx.
		//static const UIString PropertyNameVisable;
		static const UIString PropertyNameSource;		// Attribute for Property name Source.//add by guo

		static const UIString PropertyNameFlsIP_MASK;					// Attribute
		static const UIString PropertyNameFlsIP_MOUSEDESC;			// Attribute
		static const UIString PropertyNameFlsIP_ENABLE;				// Attribute
		static const UIString PropertyNameFlsIP_DISABLE;				// Attribute
		static const UIString PropertyNameFlsIP_MOUSEOVER;			// Attribute
		static const UIString PropertyNameFlsIP_CLICKDOWN;			// Attribute
		static const UIString PropertyNameFlsIP_UNCHECK_ENABLE;		// Attribute
		static const UIString PropertyNameFlsIP_UNCHECK_DISABLE;		// Attribute
		static const UIString PropertyNameFlsIP_UNCHECK_MOUSEOVER;	// Attribute
		static const UIString PropertyNameFlsIP_UNCHECK_CLICKDOWN;	// Attribute
		static const UIString PropertyNameFlsIP_FILL;					// Attribute
		static const UIString PropertyNameFlsIP_TR_ENABLE;			// Attribute
		static const UIString PropertyNameFlsIP_TR_DISABLE;			// Attribute
		static const UIString PropertyNameFlsIP_LS_SEL;				// Attribute
		static const UIString PropertyNameFlsIP_LS_UNSEL;				// Attribute
		static const UIString PropertyNameFlsIP_FILL_FULL;			// Attribute
		static const UIString PropertyNameFlsIP_LBTN_CLICK;			// Attribute
		static const UIString PropertyNameFlsIP_RBTN_CLICK;			// Attribute

		static const UIString PropertyNameWinRect0;		// Attribute for Property name NameWinRect0.

		static const UIString PropertyStateColor;			// Attribute for Property name StateColor.
		static const UIString PropertyRimColor;			// Attribute for Property name RimColor.
		static const UIString PropertyTextureName;		// Attribute for Property name TextureName.
		static const UIString PropertyTextureRect;		// Attribute for Property name TextureRect.
		static const UIString PropertyTexturePos;			// Attribute for Property name TexturePos.
		static const UIString PropertydwColor;			// Attribute for Property name dwColor.
		static const UIString PropertydwLock;				// Attribute for Property name dwLock.

		//////////////////////////////////////////////////////////////////////////
		// 解析相关
	public:
		virtual void ElementStart(const char* element, TiXmlElement* ElementPtr);
		virtual void ElementEnd	 (const char* element);
		// 设置根参数
		void SetRootWndResParam(WndResParam* WndParam);


	private:
		// 清空当前数据
		void CleanUpDataLoaded(void);
		// 新窗体函数
		void ElementWinStart(TiXmlElement* ElementPtr);
		void ElementWinEnd();
		// 子窗体函数
		void ElementChildWinStart(TiXmlElement* ElementPtr);
		void ElementChildWinEnd();
		// 状态函数
		void ElementStateStart(TiXmlElement* ElementPtr);
		void ElementStateEnd();
		// 图片函数
		void ElementImageStart(TiXmlElement* ElementPtr);
		void ElementImageEnd();
		// 属性相关
		void ElementPropertyStart(const char* element, TiXmlElement* ElementPtr);
		void ElementPropertyEnd();

		// 字符串到Rect的转换
		void StringToRect(string& str, CIRect& re, bool beChange = false);
		

	private:

		// 设置当前操作所需数据
		typedef UIVector<WndResParam*>		WindowResParamStack;

		WndResParam*		m_root;				// xml 根对象
		WndResParam*		m_curWndRes;		// 当前操作的对象
		WindowResParamStack m_wndResStatck;		// 操作对象堆栈
		ChildWndInfo*		m_curWinState;		// 当前对象状态堆栈
		ChildImageList*		m_curImgList;		// 当前状态图片信息堆栈
		WindowImage*		m_curImg;			// 当前图片指针
		//////////////////////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////////////////////
		// 存储相关
	public:
		// SQRWnd 到 Element 的转换
		void CWnd2Element(SQRWnd* pWnd, TiXmlElement* element, string& classname, string& creatinfo);

	private:
		// 保存相关函数
		void AddPropertyElement(const char* name,const char* val,TiXmlElement* parent);
		// 声音资源保存
		void AddSoundPro(ESoundMsg ES, SQRWnd* pWnd, TiXmlElement* element);
		// 保存位置信息
		void AddRectPro(SQRWnd* pWnd, TiXmlElement* element, bool be_zero = false);
		// 保存材质位置信息
		void AddPropTexturePos(IMAGE& Image, TiXmlElement& ImgElem);
		// 保存使用位置信息
		void AddPropTextureRect(IMAGE& Image, TiXmlElement& ImgElem);
		// 整理无符号整形转换后的字符串
		void ReBuiltU32Str(string& val);
		//////////////////////////////////////////////////////////////////////////
	};
}
