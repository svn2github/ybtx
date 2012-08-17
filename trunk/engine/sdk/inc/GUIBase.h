#pragma once
#include "CDynamicObject.h"
#include "IGraphic.h"
#include "CallBackX.h"
#include "SQRGuiMallocObject.h"
#include "GUITextureHelper.h"

namespace sqr
{

#ifdef _USRDLL
#ifdef GUI_EXPORTS
#define GUI_API __declspec(dllexport)
#else
#define GUI_API __declspec(dllimport)
#endif
#else
#define GUI_API
#endif

#ifdef _STINT_MODE
#define TEXTURE_STINT_MODE
#endif
#define UI_HIDE_MODE
#define GUITEX_LOAD_ASYN

#define RENDERDLOG_FLUSH_MODE
#define DEF_MAX_BLOCK_NUM 32
enum
{
	AUF_VERSION = 10005,	//没改格式的auf版本号
	AUF_ADD_AUDIO,			//增加音效名
	AUF_ADD_MULTIFONT,
};

enum
{
	EDLG_FLUSH_TEX	,
	EDLG_FLUSH_FLASH,
	EDLG_FLUSH_FX	,
	EDLG_FLUSH_COUNT
};

class SQRWnd;
class SQRRichString;

	// ----------------------------------------------------------------------------------------
	// 常量定义
	// ----------------------------------------------------------------------------------------
	extern const float FLOAT_MAX_VALUE;
	extern const float FLOAT_MIN_VALUE;
	extern const float FLOAT_EPSILON;

	// ----------------------------------------------------------------------------------------
	// 宏定义
	// ----------------------------------------------------------------------------------------
	
	/**
	\brief
		浮点坐标对其到像素	
	*/
#define PixelAligned(x) ( (float)(int)(( x ) + (( x ) > 0.0f ? 0.5f : -0.5f)) )

	// ----------------------------------------------------------------------------------------
	// 窗口类型
	// ----------------------------------------------------------------------------------------

	//窗口主类型
#define WS_MODULE			0x80000000L		//模态
#define WS_CHILD			0x40000000L		//子窗口
#define WS_MSGTRANSPT		0x20000000L		//消息透明窗口
#define WS_AREAUSETEXRECT	0x10000000L		//有效区域使用贴图范围
#define WS_MULLINETEXT		0x0f000000L		//标题多行
#define WS_TEXTYB			0x08000000L		//窗口文字y居下
#define WS_TEXTYC			0x04000000L		//窗口文字y居中
#define WS_TEXTXR			0x02000000L		//窗口文字x居右
#define WS_TEXTXC			0x01000000L		//窗口文字x居中
#define WS_TOPMOST			0x00800000L		//最顶窗口
#define WS_IGNOREWHEEL		0x00400000L		//忽略滚轮消息
#define WS_HORSCROLL		0x00200000L		//有水平滚动条
#define WS_VERSCROLL		0x00100000L		//有垂直滚动条
#define WS_CUTBYPARENT		0x00080000L		//被父窗口剪切
#define WS_NOTIFY			0x00040000L		//对子窗口WND_NOTIFY感兴趣,要求子窗口在响应系统消息时发送WND_NOTIFY消息
#define WS_TINYTEXTBACK		0x00020000L		//文字边框为阴影形态
#define WS_ITEM				0x00010000L     //Item控件(避免渲染顺序改变) ---- add by lx
#define WS_NSITEM			0x00001000L     //子窗口不显示 ---- add by guo

	// ----------------------------------------------------------------------------------------
	// 控件自定义类型 
	//

	// EDIT类型
#define	ECS_MULTILINE		0x0001L			//EDIT控件多行
#define ECS_PASSWORD		0x0002L			//EDIT控件为密码输入
#define ECS_ENABLECLIPBD	0x0004L			//EDIT控件接收剪切板内容
#define ECS_DISABLEIME		0x0008L			//禁止输入法窗口
#define ECS_YBORDER			0x00F0L			//EDIT纵向边界宽度
#define ECS_XBORDER			0x0F00L			//EDIT横向边界宽度
#define ECS_DIGITAL			0x1000L			//EDIT控件为数字输入
#define ECS_STICK_BOTTOM	0x2000L			//EDIT控件垂直滚动条滑块向下粘滞
#define ECS_UNSIGNEDINT		0x4000L			//EDIT控件为正数输入

	inline float GetXBorder( uint32 n )
	{
		return float( (n&ECS_XBORDER)>>8 );
	}
	inline float GetYBorder( uint32 n )
	{
		return float( (n&ECS_YBORDER)>>4 );
	}

	// ----------------------------------------------------------------------------
	// SCROLL类型
	//
#define SS_HSCROLL			0x4000L			//Scroll控件水平式样
#define AUTODOWN			0x0000000AL     //直接滑条是在底部的

	// ----------------------------------------------------------------------------
	// SPIN类型
	//
#define PS_HSCROLL			0x4000L			//Spin控件水平式样

	// ----------------------------------------------------------------------------
	// CheckButton类型
	//
#define BTN_CHECK			0x0001L
#define BTN_RADIO			0x0002L

#define BTN_XOF				0xF000L			// button和check button 的title的X偏移
#define BTN_YOF				0x0F00L			// button和check button 的title的Y偏移
	inline float GetXOF( uint32 n )
	{
		return float((n&BTN_XOF)>>12);
	}
	inline float GetYOF( uint32 n )
	{
		return float((n&BTN_YOF)>>8);
	}
	//-----------------------------------------------------------------------------------------
	// STATIC类型
	//
#define STA_VERTICAL		0x0001L
	// ----------------------------------------------------------------------------------------
	// SQRProgress类型
	//
#define PRG_FILLFIRST		0x0001L

	// ----------------------------------------------------------------------------
	// ListCtrl类型
	//
#define LCTL_SCLSMOOTH		0x0002L
#define LCTL_SHOWSCLAUTO	0x0004L
#define LCTL_MULTSEL		0x0008L
#define LCTL_CLICKSEL		0x0010L
#define LCTL_SELFULLLINE	0x0020L
#define LCTL_SHOWGRID		0x0040L

	// ----------------------------------------------------------------------------
	// ComboCtrl类型
	//
#define COMBO_DISEDIT		0x0001L

	// ----------------------------------------------------------------------------
	// TreeCtrl类型
	//
#define TREE_FULLSELECT		0x0001L
#define TREE_HASLINE		0x0002L

	// ----------------------------------------------------------------------------
	// SQRRichText类型
	//
#define RCHW_AUTORESETH			0x0001L
#define RCHW_STICK_BOTTOM		0x0002L					//垂直滚动条滑块向下粘滞
#define RCHW_ENABLE_SELECT		0x0004L					//允许鼠标进行选择
#define RCHW_AUTOSETRECT_DOWN	0x0008L					//SQRRichText 根据文字数量向下伸展

	// ----------------------------------------------------------------------------
	// 窗口消息
	//

	//控件自定义消息
#define WND_NOTIFY					0x0001			//子控件标准消息
#define	EDIT_RETURN					0x0101			//EDIT控件回车消息
#define	EDIT_CTRLUP					0x0102			//EDIT控件CTRL+UP消息
#define	EDIT_CTRLDOWN				0x0103			//EDIT控件CTRL+DOWN消息
#define EDIT_CTRLRETURN				0x0104			//EDIT控件CTRL+回车消息
#define EDIT_ONCHAR					0x0105			//EDIT控件输入字符消息
#define EDIT_BACK					0x0106			//EDIT控件倒退键消息
#define EDIT_DELETE					0x0107			//EDIT控件删除键消息
#define EDIT_LOSEFOCUS				0x0108			//EDIT控件失去编辑焦点消息
#define	BUTTON_CLICKUP				0x0201			//BUTTON控件点击消息
#define	BUTTON_RIGHTCLICKUP			0x0202			//BUTTON控件点击消息
#define	BUTTON_CLICKCONT			0x0203			//BUTTON控件点击消息
#define	BUTTON_DRAG					0x0204			//BUTTON控件点击消息
#define	BUTTON_CLICKDOWN			0x0205			//BUTTON控件点击消息
#define	BUTTON_RIGHTCLICKDOWN		0x0206			//BUTTON控件点击消息
#define BUTTON_MOUSEMOVE			0x0207			//BUTTON上鼠标移动
#define BUTTON_LCLICK				0x0209			//BUTTON一次左键按下抬起
#define BUTTON_RCLICK				0x0210			//BUTTON一次右键按下抬起
#define SCROLL_MOVE					0x0301			//SCROLL控件移动
#define SCROLL_RELEASE				0x0302			//SCROLL控件释放按键
#define SPIN_DELTA					0x0401			//SCROLL控件移动
#define ITEM_SELECT_CHANGED			0x0501			//Item鼠标位置移动到另一个Item上
#define ITEM_RBUTTONUP			    0x0502			//Item选择改变
#define ITEM_SHIFTDOWN				0x0503			//Item选择改变
#define ITEM_SHIFTRDOWN				0x0504			//Item选择改变
#define ITEM_LBUTTONUP				0x0505			//Item上鼠标移动
#define ITEM_CTRLDOWN				0x0506			//Item上Ctrl+鼠标左键
#define ITEM_LBUTTONDBLCLICK		0x0507			//Item上双击左键
#define ITEM_LBUTTONCLICK			0x0508			//Item上一次左键按下抬起
#define ITEM_RBUTTONCLICK			0x0509			//Item上一次右键按下抬起
#define RICH_CLICK					0x0601			//点击Rich控件
#define RICH_SCROLL					0x0602			//点击Rich控件
#define RICH_CHOOSE					0x0603			//点击Rich控件
#define RICH_RCLICK					0x0605			//点击Rich控件
#define RICHWND_LCLICKUP			0x0606			//点击Rich控件
#define RICHWND_LCLICKDOWN			0x0607			//点击Rich控件
#define RICHWND_RCLICKUP			0x0608			//点击Rich控件
#define RICHWND_RCLICKDOWN			0x0609			//点击Rich控件
#define RICHWND_MOUSEOVER			0x0610			//点击Rich控件
#define RICHWND_MOUSEOVERDOWN		0x0611			//点击Rich控件
#define CILCK_WNDER					0x0701			//点击高亮控件的消息
#define TREECTRL_RIGHTCLICK			0x0801			//右键单击TreeCtrl消息
#define LISTCTRL_LEFTCLICKUP		0x0901			//ListCtrl左键释放
#define LISTCTRL_RIGHTCLICKUP		0x0902			//ListCtrl右键释放
#define LISTCTRL_LEFTCLICK			0x0903			//ListCtrl一次左键按下抬起
#define LISTCTRL_RIGHTCLICK			0x0904			//ListCtrl一次右键按下抬起
#define ICON_LBUTTONDBLCLICK		0x0a01			//Icon双击

#define WEBBROWSER_FORWARD			0x0b01
#define WEBBROWSER_LOADCOMPLETE		0x0b02
#define	WEBBROWSER_DOCCOMPLETE		0x0b03

	// 测试区域颜色定义
#define MASKCOLOR_TITLE				0xff000000		//标题颜色定义
#define MASKCOLOR_AREA				0xffffffff		//区域颜色定义

	//Base: SQRWnd,SQRButton,CheckButton,
#define SM_BS_BK					0x0001		//背景贴图

	//ScrollBar
#define SM_SC_BK					0x0001		//背景贴图
#define SM_SC_P1					0x0002		//按键1
#define SM_SC_P2			        0x0003		//按键2
#define SM_SC_BAR					0x0004		//滑块

	//Spin
#define SM_SP_P1					0x0001		//按键1
#define SM_SP_P2			        0x0002		//按键2

	//ComboBox
#define SM_CB_BK					0x0001		//Edit背景贴图
#define SM_CB_BT					0x0002		//Button背景贴图
#define SM_CB_LB_BK					0x0003		//ListBox背景贴图
#define SM_CB_SC_BK					0x0004		//背景贴图
#define SM_CB_SC_P1					0x0005		//按键1
#define SM_CB_SC_P2					0x0006		//按键2
#define SM_CB_SC_BAR			    0x0007		//滑块

	//ListCtrl
#define SM_LB_BK					0x0001		//ListBox背景贴图
#define SM_LB_SCH_BK				0x0002		//水平滚动条背景贴图
#define SM_LB_SCH_P1				0x0003		//水平滚动条按键1
#define SM_LB_SCH_P2			    0x0004		//水平滚动条按键2
#define SM_LB_SCH_BAR				0x0005		//水平滚动条滑块
#define SM_LB_SCV_BK				0x0006		//垂直滚动条背景贴图
#define SM_LB_SCV_P1				0x0007		//垂直滚动条按键1
#define SM_LB_SCV_P2			    0x0008		//垂直滚动条按键2
#define SM_LB_SCV_BAR				0x0009		//垂直滚动条滑块

	//RichWnd
#define SM_RW_BK					0x0001		//ListBox背景贴图
#define SM_RW_SCV_BK				0x0002		//垂直滚动条背景贴图
#define SM_RW_SCV_P1				0x0003		//垂直滚动条按键1
#define SM_RW_SCV_P2			    0x0004		//垂直滚动条按键2
#define SM_RW_SCV_BAR				0x0005		//垂直滚动条滑块


	//TreeCtrl
#define SM_TC_BK					0x0001		//TreeCtrl背景贴图
#define SM_TC_SCV_BK				0x0002		//垂直滚动条背景贴图
#define SM_TC_SCV_P1				0x0003		//垂直滚动条按键1
#define SM_TC_SCV_P2			    0x0004		//垂直滚动条按键2
#define SM_TC_SCV_BAR				0x0005		//垂直滚动条滑块

	//SQRClock
#define SM_CK_BK					0x0001      //SQRClock背景贴图
#define SM_CK_PG0_Bk				0x0002		//Progress[0]背景贴图
#define SM_CK_PG1_Bk				0x0003		//Progress[1]背景贴图
#define SM_CK_PG2_Bk				0x0004		//Progress[2]背景贴图
#define SM_CK_PG3_Bk				0x0005		//Progress[3]背景贴图

	//SQREdit
#define SM_CE_BK					0x0001		//SQREdit背景贴图
#define SM_CE_SCV_BK				0x0002		//垂直滚动条背景贴图
#define SM_CE_SCV_P1				0x0003		//垂直滚动条按键1
#define SM_CE_SCV_P2			    0x0004		//垂直滚动条按键2
#define SM_CE_SCV_BAR				0x0005		//垂直滚动条滑块

	//SQRProgress
#define SM_PG_BK					0x0001		//SQRProgress背景贴图
#define SM_PG_PD1					0x0002		//显示块1背景贴图
#define SM_PG_BD2					0x0003		//显示块2背景贴图

	//SQRVideo
#define SM_VO_BK					0x0001      //SQRClock背景贴图
#define SM_VO_B1					0x0002		//按键播放
#define SM_VO_B2					0x0003		//按键暂停
#define SM_VO_B3					0x0004		//按键重播

	//////////////////////////////////////////////////////////////////////////
	// CheckButton状态
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// MessageBox
	//////////////////////////////////////////////////////////////////////////
	//式样
#define     MB_BtnNone              0x0000
#define     MB_BtnOK                0x0001
#define     MB_BtnCancel            0x0002

	//////////////////////////////////////////////////////////////////////////
	// 文本对齐方式
	//////////////////////////////////////////////////////////////////////////
#define _TEXT_FMT_LEFT				0x00003001L
#define _TEXT_FMT_RIGHT				0x00003002L
#define _TEXT_FMT_CENTER			0x00003003L

	//////////////////////////////////////////////////////////////////////////
	// 光标状态
	//////////////////////////////////////////////////////////////////////////
#define _CURSOR_NORMAL				0x00000000L
#define _CURSOR_GLOBAL				0x00000001L
#define _CURSOR_EXCLUSIVE			0x00000002L


	// 锁定标志
#define		LOCK_LEFT				0x0001L		//靠左
#define		LOCK_RIGHT				0x0002L		//靠右
#define		LOCK_TOP				0x0004L		//靠上
#define		LOCK_BOTTOM				0x0008L		//靠下
#define		LOCK_ALL				0x000FL		//四边锁定
#define		LOCK_UNZOOM				0x0010L		//非缩放

	// ----------------------------------------------------------------------------------------
	// GUI基本结构
	// ----------------------------------------------------------------------------------------

	// ----------------------------------------------------------------------------
	// StateMask
	//
	enum EventStateMask
	{
		IP_MASK					= 0,	//
		IP_MOUSEDESC			= 1,	//0x0001	//点击测试区域
		IP_ENABLE				= 2,	//0x0002	//背景贴图
		IP_DISABLE				= 3,    //0x0003	//背景贴图
		IP_MOUSEOVER			= 4,    //0x0004	//BUTTON控件MOUSEOVER贴图
		IP_CLICKDOWN			= 5,    //0x0005	//BUTTON控件CLICKDOWN贴图
		IP_UNCHECK_ENABLE		= 6,    //0x0006	//BUTTON控件Uncheck Normal贴图
		IP_UNCHECK_DISABLE		= 7,    //0x0007	//BUTTON控件Uncheck Disable贴图
		IP_UNCHECK_MOUSEOVER	= 8,    //0x0008	//BUTTON控件Uncheck MOUSEOVER贴图
		IP_UNCHECK_CLICKDOWN	= 9,    //0x0009    //BUTTON控件Uncheck CLICKDOWN贴图
		IP_FILL					= 10,   //0x000A    //Progress控件的贴图
		IP_TR_ENABLE			= 11,
		IP_TR_DISABLE			= 12,
		IP_LS_SEL				= 13,
		IP_LS_UNSEL				= 14,
		IP_FILL_FULL			= 15,
		IP_LBTNCLICK_ACTION		= 16,
		IP_RBTNCLICK_ACTION		= 17,
		IP_COUNT
	};

	enum GuiBlockMask
	{
		GBM_TOP		= 0,
		GBM_LEFT	= 1,
		GBM_RIGHT	= 2,
		GBM_BOTTOM	= 3,
		GBM_CENTER	= 4,
		GBM_TOP_LEFT	= 5,
		GBM_TOP_RIGHT	= 6,
		GBM_BOTTOM_LEFT	= 7,
		GBM_BOTTOM_RIGHT= 8,
		GBM_COUNT		= 9,
	};

	enum GUIDrawState
	{
		eDraw_No			= 0,
		eDraw_Start			= 1,
		eDraw_End			= 2,
		EDraw_StateCount	= 3,
	};
	// ----------------------------------------------------------------------------
	// CtrlMask
	//
	struct IMAGE_PARAM : public SQRGuiMallocObject
	{
		uint16	CtrlMask;
		uint16	StateMask;
		IMAGE_PARAM():CtrlMask(0),StateMask(0) {}
		IMAGE_PARAM( uint16 CtrlMask, uint16 StateMask ):CtrlMask(CtrlMask),StateMask(StateMask) {}
	};

	///effect message
	enum EMsgFx
	{
		eMF_Click	= 0,			//点击
		eMF_Open	= 1,			//打开
		eMF_Close	= 2,			//关闭

		eMF_Count,
	};

	///sound message
	enum ESoundMsg
	{
		eMS_Click		= 0,			//点击
		eMS_Open		= 1,			//打开
		eMS_Close		= 2,			//关闭
		eMS_MouseOver	= 3,			//Mouse Over

		eMS_Count,
	};

	// effect run kind
	enum EEffectRun
	{
		eEffRun_Normal		= 0,		// 正常播放
		eEffRun_Repeat		= 1,		// 循环播放
		eEffRun_Last		= 2,		// 最后一帧持续
		eEffRun_Count,
	};

	// 一个GUI效果对应一个效果类
	// 同类的效果不可以同时出现
	enum EGUIEffect
	{
		eEff_Shape			= 0,		// 渐隐渐现
		eEff_Transparent	= 1,		// 形变
		
		eEff_Count,
	};

	//struct GUIRenderEvn
	//{
	//	uint32			uAmbient;
	//	float			fFovy;
	//	CVector3f		vecCameraPos;
	//	CVector3f		vecCameraDir;
	//};

	class GUI_EVENT_FONT : public SQRGuiMallocObject
	{
	public: 
		GUI_EVENT_FONT(){ m_FontColor = 0; m_uFontBkColor = 0;}
		uint32			m_FontColor;
		uint32			m_uFontBkColor;
	};
	
	class GUI_FONT : public SQRGuiMallocObject
	{
	public:
		GUI_FONT(){ m_FontSize = 14; m_uFontGradualColor = 0;}
		
		int32			m_nFontID;					// 字体编号
		uint32			m_FontSize;					// 字体大小

		typedef	UIMap	<EventStateMask,GUI_EVENT_FONT> EvtFontVector;
		GUI_EVENT_FONT* GetEventFont(EventStateMask esm);
		EvtFontVector	m_EventFont;
		uint32			m_uFontGradualColor;
		uint32			m_uFontEffectMask;
	};

	class GUI_BLOCK : public SQRGuiMallocObject
	{
	public:
		typedef UIVector<CFRect> BLOCK_RECT;
		BLOCK_RECT	m_BlockRect;
		GUI_BLOCK() { m_BlockRect.resize(GBM_COUNT); };
		inline CFRect& operator [] (UINT i) { return m_BlockRect[i]; }
	};

	class BLOCK_POOL
	{
	public:
		typedef UIVector<GUI_BLOCK*> RectVector;
		RectVector	m_vecRect;
		static const uint32 MAX_BLOCK_NUM;
		BLOCK_POOL();
		~BLOCK_POOL();
		inline GUI_BLOCK* operator[] (UINT i) { return m_vecRect[i-1]; }//1~MAX_BLOCK_NUM
		GUI_BLOCK& InitBlock(UINT i);
	};

	class StyleStruct : public SQRGuiMallocObject
	{
	public:
		StyleStruct();
		~StyleStruct() { Release(); }

		void Release(void);

		typedef	UIMap<EventStateMask,BLOCK_POOL> PoolVector;
		typedef UIMap<int32,GUI_FONT*>	FontVector;
		ITexture*	pTexture;
		UIString	m_TexString;
		DWORD		m_dwColor;
		PoolVector	m_vecPool;
		FontVector	m_vecFont;
	};

	struct IMAGE : public SQRGuiMallocObject
	{
		IMAGE()
			: pTexture(0)
			, dwColor(0)
			, dwLockFlag(0)
			, bWidthNegtvDir(false)
			, bHeightNegtvDir(false)
		{
		}
		UIString		texName;		// 贴图名字
		ITexture*		pTexture;		// 贴图
		CFRect			rtTexture;		// 贴图使用区域
		CFPos			posWnd;			// 贴图使用区域左上角在窗口中的位置
		uint32			dwColor;		// 贴图附加颜色
		uint32			dwLockFlag;		// 贴图停靠锁定的方向
		bool			bWidthNegtvDir;
		bool			bHeightNegtvDir;

		void SetColor(uint32 uColor)
		{
			dwColor = uColor;
		}
		void	SetPosWndX(float nX)
		{
			posWnd.x=nX;
		}
		float	GetPosWndX()const
		{
			return posWnd.x;
		}
		void	SetPosWndY(float nY)
		{
			posWnd.y=nY;
		}
		float	GetPosWndY()const
		{
			return posWnd.y;
		}
		const char* GetTexName()
		{
			/*if(pTexture)
				return pTexture->GetName();
			else
				return "";*/
			return texName.c_str();
		}
	};
	struct VEC_IMAGE
	{
		UIVector<IMAGE> Image;
	};

	class IGUIHandler : public SQRGuiMallocObject
	{
	public:
		typedef pair<SQRWnd*, UIString> EffectKey;
		virtual void	OnEnableIme( bool bEnable ) = 0;
		virtual void	OnPlayFx( SQRWnd* pWnd, const char* szFxName, EEffectRun eEffrun = eEffRun_Normal ) = 0;
		virtual void*	OnPlayFxIme( SQRWnd* pWnd, const char* fxName, EEffectRun eEffrun = eEffRun_Normal ) = 0;
		virtual void	OnPlayFxEx( SQRWnd* pWnd, const char* fxFileName, const char* fxName, EEffectRun eEffrun = eEffRun_Normal ) = 0;
		virtual void*	OnPlayFxImeEx( SQRWnd* pWnd, const char* fxFileName, const char* fxName, EEffectRun eEffrun = eEffRun_Normal ) = 0;
		virtual void	OnClearFx( SQRWnd* pWnd, const char* fxName ) = 0;
		virtual void	OnClearFx( SQRWnd* pWnd ) = 0;
		virtual void	OnClearImeFx( SQRWnd* pWnd, const char* fxName ) = 0;
		virtual void	OnUpdateFx(uint curTime, float zoom) = 0;
		virtual void	OnWndSized(const CFRect& rect) = 0;
		virtual bool	OnRenderFx( EffectKey& key, uint curTime, float zoom ) = 0;
	};

	class IHook	
		: public virtual CDynamicObject
		, public SQRGuiMallocObject
	{
	public:
		IHook();
		~IHook();
		virtual bool OnHook( SQRWnd* pWnd, uint32 message, WPARAM wParam, LPARAM lParam );
	};

	class IAccelerator 
		: public virtual CDynamicObject
		, public SQRGuiMallocObject
	{
	public:
		IAccelerator();
		~IAccelerator();
		virtual bool OnAccelerator( uint32 message, WPARAM wParam, LPARAM lParam )
		{
			CALL_CLASS_CALLBACK_3_RET(bool, message, wParam, lParam)
			return false;
		};
		virtual void OnAcceleratorInEdit() { CALL_CLASS_CALLBACK() };
		virtual void OnAcceleratorIsModule(uint32 message, WPARAM wParam, LPARAM lParam) { CALL_CLASS_CALLBACK_3(message,wParam,lParam) };
	};
	class GUI_API WND_IMAGE_LIST
		: public virtual CDynamicObject
		, public SQRGuiMallocObject
	{
		VEC_IMAGE*			m_pImageList;
		bool				m_IsAsynEnable;

	public:
		WND_IMAGE_LIST( const WND_IMAGE_LIST& ImageList );
		WND_IMAGE_LIST();
		~WND_IMAGE_LIST();

		void			Release();
		void			SetGUIAsynEnable(bool enable);
		bool			IsImageLoadValid();
		int32			AddImage( IGraphic* pGraphic, int32 nImageIndex, const char* szTextName,
			const CFRect* prt, const CFPos& posWnd, uint32 uColor, uint32 uLockFlag = 0 );
		int32			AddImageWin( IGraphic* pGraphic, int32 nImageIndex, const char* szTextName,
			const CFRect* prt, const CFPos& posWnd, uint32 uColor, uint32 uLockFlag = 0 );

		int32			AddImageFromImageList( const WND_IMAGE_LIST & Imagelist, int32 nIndexBegin, int32 nIndexEnd );

		int32			GetImageCount() const;
		IMAGE&			GetImage( int32 nImageIndex )const;
		void			GetImage(int32 nImageIndex, IMAGE& image);
		void			DeleteImage( int32 nImageIndex );
		int32			DetectImage( const CFPos& CursorInWnd );
		void			ClearImage();
		void			Scale( float xDel, float yDel );

		void			ReleaseTex();
		void			CreatTex( IGraphic* pGraphic, GUITextureHelper* texhelp = NULL );
	
		const WND_IMAGE_LIST& operator = ( const WND_IMAGE_LIST& ImageList );
	};

	//class GUI_API WND_RENDER_OBJ 
	//	: public virtual CDynamicObject
	//	, public SQRGuiMallocObject
	//{
	//	HANDLE			m_hRenderObj;

	//public:
	//	WND_RENDER_OBJ( const WND_RENDER_OBJ& RenderObj );
	//	WND_RENDER_OBJ();
	//	~WND_RENDER_OBJ();

	//	void			Release();
	//	bool			IsEmpty() const;
	//	const CVector2f&	GetDir()const;
	//	void			GetDir(CVector2f& vecDir);
	//	void			SetDir( const CVector2f& vecDir );
	//	float			GetScale() const;
	//	void			SetScale( float fScale );
	//	CVector3f		GetOff() const;
	//	void			GetOff(CVector3f& vecOff);
	//	void			SetOff( const CVector3f& vecOff );
	//	void			SetNextAnimation( const char* AniName, const FramePair& FmPair, bool bLoop = true, int32 DelayTime = 200 /*毫秒*/ );
	//	void			AddRenderObj( SQRWnd* pAnyWnd, const char* sArpFile, const char* sClass,
	//		uint32 uRS, const char* sAraFile, const char* sFxFile, const char* sFxName );
	//	AniResult		Render( const CMatrix& matWorld, uint32 uCurTime ) const;

	//	const WND_RENDER_OBJ& operator = ( const WND_RENDER_OBJ& RenderObj );
	//};
}
