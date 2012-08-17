#pragma once
#include "SQRGuiMallocObject.h"
#include "CPos.h"
#include "CRectangle.h"
#include "GUIBase.h"
#include "SQRRichToolTip.h"

namespace sqr
{
	class SQRGUIManager;
	class SQRToolTip;
	class SQRWnd;
	class SQRRichToolTip;

	struct WindowImage : public SQRGuiMallocObject
	{
		WindowImage();// : dwLockFlage(0), dwColor(0) {}
		UIString	szTexture;
		RECT		rtScale;		// 贴图使用区域
		CFPos		posWnd;			// 贴图使用区域左上角在窗口中的位置
		uint32		dwColor;		// 贴图附加颜色
		uint32		dwLockFlage;	// 贴图锁定标志
	};

	struct ChildImageList : public SQRGuiMallocObject
	{
		ChildImageList();
		int32						nStateMask;
		uint32						uTextColor;
		uint32                      uRimColor;			// 字体边框颜色
		UIVector<WindowImage>	ImageList;
	};

	struct ChildWndInfo : public SQRGuiMallocObject
	{
		ChildWndInfo();// : m_uStyleIndex(0),m_uLockFlag(LOCK_LEFT|LOCK_TOP ){};
		CFRect						rtWnd;
		uint32						m_uStyleIndex;
		uint32						m_uLockFlag;
		UIVector<ChildImageList>	ChildBkList;
	};

	// 该结构可以在程序中创建，但主要是在资源中加载
	struct FlashWndInfo : public SQRGuiMallocObject
	{
		FlashWndInfo();
		~FlashWndInfo();
		bool Init(IGraphic* pGraphic, UIString flashname, UIString texname, CFRect Texpos, uint32 time,
				  uint32 ct, float amin, float amax, CFRect repos);
		void Destroy();
		void CreateTexture();
		void ReleaseTexture();

		UIString				m_FlashName;		// 名字
		IMAGE					m_image;			// 贴图信息
		uint32					m_count;			// 闪烁次数，0为一直闪烁
		uint32					m_time;				// 单程时间, 豪秒

		// 相等为不闪烁
		float					m_Amin;				// 最大透明度 最小为0F
		float					m_Amax;				// 最小透明度 最大为1F
		CFRect					m_repos;			// 校正位置
		bool					m_bAdd;				// 透明度是否增加

		int32					m_iRef;				// 使用次数
	};

	struct WndCreateParam : public SQRGuiMallocObject
	{
		const char*		szTitle;
		uint32			uStyle;
		float			x;
		float			y;
		float			width;
		float			height;
		SQRWnd*			pParentWnd;
		int32			nFontID;
		bool			bBold;
		float			font_size;
		uint32			uGroupID;
		const char*		szDesc;
		const char*		szSource;//add by guo

		WndCreateParam()
			: szTitle("")
			, uStyle(0)
			, x(0.0f)
			, y(0)
			, width(0.0f)
			, height(0.0f)
			, pParentWnd(NULL)
			, nFontID(-1)
			, bBold(false)
			, font_size(0.0f)
			, uGroupID(-1)
			, szDesc("")
			, szSource("")
		{}

		void SetParentWnd(SQRWnd* pWnd){ pParentWnd = pWnd; }
		SQRWnd* GetParentWnd() const { return pParentWnd; }
	};



	struct WndResParam : public SQRGuiMallocObject
	{
		char*									m_ClassName;
		char*									m_WindowTitle;              //窗口标题
		uint32									m_WndStyle;                 //窗口类型
		int32									m_x;						//窗口在父窗口的水平坐标
		int32									m_y;						//窗口在父窗口的垂直坐标
		int32									m_Width;                    //窗口的宽度
		int32									m_Height;                   //窗口的高度
		int32									m_nFontID;					//字体编号
		uint32									m_FontSize;					//字体大小
		bool									m_bBold;					//字体是否加粗
		uint32									m_uFontGradualColor;
		uint32									m_uFontEffectMask;
		bool									m_isShowBackground;
		uint32									m_uStyleIndex;
		char*									m_TransTexture;
		uint32									m_uLockFlage;				//窗口锁定标志
		uint32									m_GroupID;					//组ID
		char*									m_szDesc;					//鼠标移过窗口时的信息显示
		char*									m_MsgFx[eMF_Count];			//消息特效字符串
		UIString								m_MsgSound[eMS_Count];		//消息音效字符串
		UIMap<EventStateMask, UIString>			m_FlashWndStr;				//闪烁窗体资源不同状态
		char*									m_UseClassName;				//自定义类
		char*									m_UseInfo;					//自定义信息
		char*									m_WindowName;
		char*									m_SourceName;				//add by Guo
		UIVector<ChildWndInfo>					m_ImageList;				//图片列表
		UIMap<UIString, WndResParam*>			m_Child;
		bool									m_beAucRes;					//从AUC资源创建

		WndResParam() 
			: m_ClassName(NULL)
			, m_isShowBackground(true)
			, m_uStyleIndex(0)
			, m_FontSize(14)
			, m_beAucRes(true)
			, m_uFontGradualColor(0)
		{
			InitTChar();
		}
		~WndResParam();

		void InitTChar();
		void UninitTChar();

	private:
		const WndResParam& operator= ( const WndResParam& )
		{
			return *this;
		}
		WndResParam( const WndResParam& ) {}
		void clearTChar( char* str);
	};

	struct WndHandle : public SQRGuiMallocObject
	{
	public:
		WndHandle();
	public:
		SQRGUIManager*				m_pGUI;						//窗口管理器
		uint32						m_WndStyle;                 //窗口类型
		UIString					m_WindowTitle;              //窗口标题
		UIString					m_WindowName;				//窗口名字，来源于资源或者窗口标题
		UIWString					m_WindowTitleW;
		UIWString					m_WindowNameW;
		float						m_xScreen;                  //窗口在根窗口的水平坐标
		float						m_yScreen;                  //窗口在根窗口的垂直坐标
		float						m_Width;                    //窗口的宽度
		float						m_Height;                   //窗口的高度

		float						m_xScreenbak;               //窗口在根窗口的水平坐标
		float						m_yScreenbak;               //窗口在根窗口的垂直坐标
		float						m_Widthbak;                 //窗口的宽度
		float						m_Heightbak;                //窗口的高度


		float						m_xInitial;                 //窗口初始的水平坐标
		float						m_yInitial;                 //窗口初始的的垂直坐标
		float						m_WidthInitial;             //窗口初始的的宽度
		float						m_HeightInitial;            //窗口初始的的高度
		int32						m_GroupID;					//
		uint32						m_uLockFlage;				//窗口锁定标志
		float                       m_fTransparent;             //窗口透明度
		uint32						m_StyleIndex;				//风格索引
		UIString					m_szSource;					//资源名称（现对视频控件可用）//add by guo

		bool						m_bIsShowBackground;		//窗口背景是否显示
		bool						m_bEnable;					//窗口是否有效
		bool						m_bShow;					//窗口是否显示
		bool						m_bIsHeld;					//窗口是否被点住
		bool						m_IsActive;					//窗口是否活动状态
		bool						m_bNewByGUISys;				//由GUI系统动态创建，也就是由资源创建的窗口
		bool						m_bGetMouseWheel;			//窗口得到鼠标滚轮，快捷键忽略滚轮消息
		bool						m_bIsDownTooltips;			//tooltips是否在鼠标下方显示

		float						m_FontSize;					//窗口字体大小
		CRenderFont*				m_WndFont;                  //窗口字体

		float						m_TipFontSize;				//窗口ToolTips字体大小

		ITexture*					m_pTransparenTex;			//窗体消息透明区域
		UIString					m_strTransName;				//窗体消息透明纹理路径

		float						m_fZ;						//渲染的深度
		ERenderStyle				m_eRS;						//渲染类型

		uint32						m_dwHeldTime;
		CFPos						m_CurHeldPos;				//窗口被点住的位置

		WND_IMAGE_LIST				m_Title;					//标题头位置
		WND_IMAGE_LIST				m_Enable;					//背景贴图
		WND_IMAGE_LIST				m_Disable;					//背景贴图

		uint32						m_EnableColor;              //窗口活动时字体颜色
		uint32						m_DisableColor;				//窗口非活动时字体颜色

		uint32						m_EnableBcColor;            //窗口活动时字体边框颜色
		uint32						m_DisableBcColor;           //窗口活动时字体边框颜色

		uint32						m_FontGradualColor;			///< 字体渐变色
		uint32						m_FontEffectMask;			///< 字体效果

		CFPos						m_posMouseOver;
		WND_IMAGE_LIST				m_MouseDesc;				//Desc显示窗口资源
		SQRToolTip*					m_DescRichText;				//鼠标移过窗口时的信息显示
		SQRRichToolTip*				m_pRichToolTip;				//重量级的tooltip
		UIWString					m_szDescW;
		UIString					m_MsgFx[eMF_Count];			//消息特效
		UIString					m_MsgSound[eMS_Count];		//消息音效
		UIMap<EventStateMask, FlashWndInfo*>	m_FlashInfo;	//闪烁信息

		SQRWnd*						m_ParentWnd;                //父窗口
		SQRWnd*						m_FirstChild;				//活动子窗口
		SQRWnd*						m_LastChild;                //最底子窗口
		SQRWnd*						m_PreBrother;               //同级前窗口
		SQRWnd*						m_NextBrother;              //同级后窗口

		SQRWnd*						m_pNextModule;				//下一个模态窗口
		HCURSOR						m_hCursor;                  //光标

		uint32						m_dwOtherData;              //关联的其他数据
		WndResParam*				m_pResParam;				//资源窗口名字
		bool						m_bTexInit;					//材质资源是否首次初始化

		UIString					m_szClassName;
		UIString					m_szCreateInfo;
		UIString					m_szWindowName;
	};

	struct ChildResInfo : public SQRGuiMallocObject
	{
		ChildResInfo(const UIString& strTitle, const UIString& strDesc)
			: szTitle(strTitle), szDesc(strDesc)
		{}
		UIString	szTitle;
		UIString	szDesc;
	};

	struct ResInfo : public SQRGuiMallocObject
	{
		typedef UIMap<const UIString, ChildResInfo> ChildInfoMap;
		
		ResInfo(const UIString& strTitle, const UIString& strDesc, const ChildInfoMap& Map)
			: szTitle(strTitle), szDesc(strDesc), mapChildInfo(Map)
		{}
		UIString		szTitle;
		UIString		szDesc;
		ChildInfoMap	mapChildInfo;
	};

}// namespace sqr
