#pragma once
#include "TSingleton.h"
#include "CEventOwner.h"
#include "CVector3.h"

namespace sqr
{
	#define USE_OLD_RESOURCEDIR_NOTCODER //用旧的资源路径以及不用coder读取保存文件
	#define USE_OLD_LIGHT_MODEL          //用旧的光照模型，也即场景点光源默认不影响地表，顶点色（也即新的模式下的材质色）不影响物件
	class CEditStateOpertiaonMgr: public Singleton<CEditStateOpertiaonMgr>
	{
	public:

		// 游戏视角.
		enum EDefViewState
		{
			YBTX,		// 佣兵天下
			YBTX_GAME,
		};

		enum EEDIT_STATE
		{
			EES_SET_TILE_HEIGHT = 0, EES_SET_TILE_TEXTURE, EES_SUBDIVIDE_GRID,
			EES_VERTEX, EES_REGION, EES_SET_TILE_FACE, EES_SET_TILE_EFFECT, EES_SET_TILE_BLOCK,
			EES_SET_TILE_TITLE, EES_SET_TILE_LOGIN, EES_PROPS, EES_RECORD,
			EES_SET_PARAMS, EES_SET_EXTRA_PARAMS, EES_RUN, EES_AUDIO, EES_SET_TILE_REGIOON, EES_SET_LOGIC_HEIGHT, EES_CNT, EES_NONE
		};

	private:
		EEDIT_STATE    m_eEditState;		// 标志当前active面板
		EDefViewState  m_eDefViewState;
		bool		   m_bGameView;

		
		HWND		   m_hMainFrame;
		HWND		   m_hParamsView;
		HWND 		   m_hTextureView;
		HWND		   m_hVertexView;
		HWND 		   m_hMeshView;
		HWND 		   m_hResSetView;

		DWORD		   m_dwHotGridIndex;
 
		bool		   m_bLogicShowLine;	
		bool		   m_bIsCalObjectWorOffset;///在载入地图时，场景模型的世界位置偏移是否需要进行包围盒计算
		bool		   m_bHideLayerOneTex;
		bool		   m_bHideLayerTwoTex;
		bool		   m_bHideVertexColor;
	public:
		CEditStateOpertiaonMgr();
		~CEditStateOpertiaonMgr();

		void SetEditState(EEDIT_STATE e);

		inline EEDIT_STATE GetEditState()
		{
			return m_eEditState;
		}

		void Initialize(bool bFirst, HWND hhMainFrame, HWND hParamsView, HWND hTextureView, HWND hVertexView, HWND hMeshView, HWND hResSetView, HWND hLoginSetView, HWND hRegionView, HWND hTileRegionView);
		void CreateSingle();
		void DestroySingle();

		void OnEvent(const CMsg& msg);
		void UpdateFPSLimited();
		void Update();

		void DrawStateBrush();
		void  RenderMainPlayer();
		//------------------------------------------------------------------------------

		///envent
		// 改变各个面板画刷大小
		void  AmplifyBrushX();
		void  ShrinkBrushX();
		void  AmplifyBrushZ();
		void  ShrinkBrushZ();

		void  HideLayerOneTexture();
		void  HideLayerTwoTexture();
		void  HideVertexColor();

		void  DeleteBlock();

		// 相同快捷键的处理
		//+
		void  PlusEvent();
		//-
		void  SubtractionEvent();

		void  F1Event();
		void  F2Event();
		void  F4Event();
		void  F8Event();
		void  DeleteEvent();
		void  PageUpEvent();
		void  PageDownEvent();
		void  HomeEvent();
		void  EndEvent();

		// 游戏视角
		void  ShowGameView();
		// 地编视角
		void  ShowMapEditorView();
		// 垂直视角
		void  ShowOrthogonalView();
		// 顶视视角
		void  ShowTopView();
		void  ShowMainPlayer();
		CVector3f  GetCurPlayerPosition() const;

		bool GetDoesShowLogicHeight();
		bool GetDoesShowBlock();

		inline void SetHotGridIndex(const DWORD dwGridIndex)
		{
			m_dwHotGridIndex = dwGridIndex;
		}

		inline DWORD GetHotGridIndex() const
		{
			return m_dwHotGridIndex;
		}

		inline bool GetIsGameView() const
		{
			return m_bGameView;
		}

		inline void SetIsLineShowLogic(const bool b) 
		{
			m_bLogicShowLine = b;
		}

		inline bool GetIsLineShowLogic() const
		{
			return m_bLogicShowLine;
		}

		inline void SetIsCalObjectWorOffset(const bool b) 
		{
			m_bIsCalObjectWorOffset = b;
		}

		inline bool GetIsCalObjectWorOffset() const
		{
			return m_bIsCalObjectWorOffset;
		}

		void SetMeshLogicHeight(const float fHeight);
		float GetMeshLogicHeight();

		inline bool GetHideLayerOneTexture()
		{
			return m_bHideLayerOneTex;
		}

		inline bool GetHideLayerTwoTexture()
		{
			return m_bHideLayerTwoTex;
		}

		inline bool GetHideVertexColor()
		{
			return m_bHideVertexColor;
		}

	private:
		void SwtichShowBlock();
		void SmallKeyBoardEvent(MSG_KEY_INF msg);
		void ToolStateEvent(const CMsg& msg);
		void SingleKeyDownEvent(MSG_KEY_INF key);
	};
}
