#pragma once
#include "IEffectGraphic.h"
#include "GraphicCommon.h"
#include "RenderSettings.h"
#include "FilterDefine.h"
#include "CParamsBufferMgr.h"
#include "CDynamicVB.h"
#include "GraphicUtil.h"
#include "SyncSystem.h"
#include "CGeometryInstance.h"
#include "CDynamicPool.h"

namespace sqr
{
	extern void	DebugText( uint color, const char* fmt, ... );//一定要干掉 
	enum ECreateType
	{
		eRCT_DX9				= 0,
		eRCT_DX10				= 1,
		eRCT_DX11				= 2,
		eRCT_OPENGL				= 3,
		eRCT_OTHER				= 4,
	};

	enum ESysTexType
	{
		eSTT_BLACK				= 0,
		eSTT_GRAY				= 1,
		eSTT_WHITE				= 2,
		esTT_COUNT,
	};

	class CGraphic : public IEffectGraphic , public Singleton<CGraphic>
	{
	public:
		CGraphic(ECreateType eCT);
		virtual ~CGraphic();
	
	public:
		inline	void		ClearRecycle(void) { m_ResMgr.Clear(); }
				bool		Initialize(void);
		CRenderTarget*		GetActiveRenderTarget(void);
		bool			IsGroupOptimiz(void);
		void			BeginModel(CModel* pModel);
		void			EndModel(void);

		///设置shader配置文件路径
				void		SetShaderConfigPath( const wstring& sPath );
		const	wchar_t*	GetShaderConfigPath() const;

				void		SetEffectLevel(float level);
				float		GetEffectLevel(void);
				void		SetEffectCallBack(SetFun sfun , GetFun gfun);

				void		SetCameraShake(bool b);
				bool		GetCameraShake(void);
				void		SetCameraShakeCallBack( SetboolFun sfun , GetboolFun gfun );

				void		UpdateCamera(void);//过渡期函数，现在新老结构交替时使用
				void		RenderGroup(void);

		uint32				GetFrameCount();
		void				ClearFrameCount();
		FilterNode*			Insert(FilterNode* fn);
		void				InsertGeometry(CGeometryInst* pInst, ERenderObjStyle eObjStyle);

		void				UpdateRenderTarget(void);
		void				ActiveRenderTarget(CRenderTarget* pTarget);
		bool				DeleteWindowTarget(CWindowTarget* pTarget);

		CDynamicVB&			GetDynamicVB();
		STATIC_RS&			GetStaticRS( ERenderStyle rs );

		bool			DrawImmediate();
		void			DrawStack();
		void			ClearStack();
		uint32			GetMaxSystemBuf();
		uint32			GetCurSystemBuf();
		uint32			GetMaxVideoBuf();
		uint32			GetCurVideoBuf();
		ECreateType		GetCreateType(void);


		RenderParam& LockRenderStack( EPrimitiveType PrimitiveType,
			uint32 VertexNum,
			uint32 PrimitiveCount,
			uint32 VerFormat,
			uint32 VerStride,
			void** ppVB,
			void** ppIB );
		void UnlockRenderStack( RenderParam& RP );



		CMatrix*	GetRenderStack( ERenderStyle RenderStyle,
			ITexture* Text0,
			ITexture* Text1,
			EPrimitiveType PrimitiveType,
			uint32 VertexNum,
			uint32 PrimitiveCount,
			uint32 VerFormat,
			uint32 VerStride,
			void** ppVB,
			void** ppIB,
			void** ppRS = NULL,
			DWORD ShaderID = 0);

		CMatrix*    GetRenderStack( ERenderStyle RenderStyle,
			ERenderObjStyle RenderObjStyle,
			ITexture* Text0,
			ITexture* Text1,
			EPrimitiveType PrimitiveType,
			uint32 VertexNum,
			uint32 PrimitiveCount,
			uint32 VerFormat,
			uint32 VerStride,
			void** ppVB,
			void** ppIB,
			void** ppRS = NULL,
			DWORD ShaderID = 0);

		//3 layer texture
		CMatrix*		GetRenderThreeLayerStack( ERenderStyle RenderStyle,
			ITexture* Text0,
			ITexture* Text1,
			ITexture* Text2,
			EPrimitiveType PrimitiveType,
			uint32 VertexNum,
			uint32 PrimitiveCount,
			uint32 VerFormat,
			uint32 VerStride,
			void** ppVB,
			void** ppIB,
			void** ppRS = NULL,
			DWORD ShaderID = 0 );

		//4 layer texutre
		CMatrix*		GetRenderFourLayerStack( ERenderStyle RenderStyle,
			ITexture* Text0,
			ITexture* Text1,
			ITexture* Text2,
			ITexture* Text3,
			EPrimitiveType PrimitiveType,
			uint32 VertexNum,
			uint32 PrimitiveCount,
			uint32 VerFormat,
			uint32 VerStride,
			void** ppVB,
			void** ppIB,
			void** ppRS = NULL,
			DWORD ShaderID = 0 );
		//void			OptimizeDP(ERenderObjStyle RenderObjStyle);
		CDynamicPool*	GetDynamicPool(void);
		void			ResetRenderStack();

		void			DrawRect( const CFRect& rt, uint32 Color, ITexture* pTexture, const CFRect* rtText = NULL, ERenderStyle rs = RS_GUI, float z = MAX_DEPTH );
		void			DrawRect( const CFRect& rt, uint32 Color, ITexture* pTexture, const CPosRect& rtText, ERenderStyle rs = RS_GUI, float z = MAX_DEPTH );
		void			DrawSkyBoard( const CFRect& rt, uint32 Color, ITexture* pTexture, const float& uoffset, const float& voffset, ERenderStyle rs = RS_GUI, float z = MAX_DEPTH );
		void			Draw3DPlane(ITexture* pTexture, CVector3f *vec3d, DWORD Color, ERenderStyle rs, CVector2f *tex_uv);
		void			DrawRectGroup( ITexture* pTexture, ERenderStyle rs, const RectGroup* rt, int nNum );
		void            DrawLine( const PointGroup* pPointGroup, int nNum, bool b2D = true, ERenderStyle rs = RS_GUI );
		/// 绘制几何图形
		void			DrawPrimitive(EPrimitiveType type, const VerColor3D* points, int32 numPoints);

		bool			DestroyDepthTexture(CRenderTexture* tex);
		void			ReloadTexture( LPCSTR szName );

		///以下是虚函数
		CRenderTexture*	CreateDepthTexture(uint width, uint height);
		CRenderTexture*	CreateRenderTarget(void);
		HRESULT			CreateTexture(  const wchar_t* alias, const char* szName, ITexture** pText );
		HRESULT			CGraphic::CreateTexture( int32 nWidth, int32 nHeight, eTextureFormat eFormat, ITexture** pText );
		HRESULT			CreateEmptyTexture( int32 nWidth, int32 nHeight, uint32 nLevel, eTextureFormat eFormat, ITexture** pText );
		HRESULT			CreateTexture(  const wchar_t* alias, const char* szName, uint Width, uint Height, ITexture** pText
			, uint MinMap = 3, eTextureFormat eFormat = TF_RGB24, DWORD Filter = TEXF_LINEAR,	DWORD MipFilter = TEXF_LINEAR, bool UseResRecycle = true );
		
		//////////////////////////////////////////////////////////////////////////
		//资源创建才是我的本职工作
		HRESULT					CreateRenderStyle( const char* szName, CRenderStyle** pRS );
		HRESULT					CreateFont( int32 id, bool bBold, float fSize, CRenderFont** pFont );
		HRESULT					CreatePieceGroup( const char* szName, CPieceGroup** pPieceGroup );
		//HRESULT					CreateAnimationsRes( const char* szName, CAnimationsRes** pAnimationGroup );
		HRESULT					CreateAnimationGroup( const char* szName, CAnimationGroup** pAnimationGroup );

		//delete a texure
		void					DeleteTexture( const char* szName );

		//新的渲染方式
		CWindowTarget*			CreateNewWindow(const RenderSettings& settings) ;
		bool					InitShader(const wstring& path, ShaderVer maxVer = ShaderVerCount);
		bool					RefreshShader(void);
		
		void					SetAsynSwitch(bool bEnable);
		void					EnableAsynRead(bool bEnable);
		void					SwapBegin(void); //这两个东西很玄妙 不要经常用
		void					SwapEnd(void); //这两个东西很玄妙 不要经常用

		//void					Render(void);

		ITexture*				GetSystemTexture( ESysTexType eStt );
		void					RegisterGfkManageTex( ITexture* pTex );
		void					RemoveGfkManageTex( ITexture* pTex );

	public:
		CDynamicVB				m_DynamicVB;
		CDynamicVB				m_EffectNpcVB;
		CDynamicVB				m_EffectSceneVB;
		CDynamicVB				m_TempVB;
		CDynamicVB*				m_pActiveVertexBuffer;
	protected:		
		CRenderTarget*			m_pActiveTarget;
		CParamsBufferMgr		m_ParamsBuffer;
		RECT					m_SourceRect;

		GWString				m_strConfigShaderPath;
		uint32					m_uFrameCount;
		SetFun					m_pSetEffect;
		GetFun					m_pGetEffect;
		SetboolFun				m_pSetCameraShake;
		GetboolFun				m_pGetCameraShake;
		ECreateType				m_CreateType;
		//----------------------------------------------
		CResMgr					m_ResMgr;
		CDynamicPool*			m_pDynamicPool;
		STATIC_RS				m_RS[RS_COUNT];
		ITexture*				m_pSysTexture[esTT_COUNT];
		
		typedef	GSet<CWindowTarget*>	TARGET_LIST;
		typedef GSet<CRenderTexture*>	DepthTextureList;
		typedef GSet<ITexture*>			GraphicManageTexList;

		GraphicManageTexList	m_gfkManageTexList;

	public:
		TARGET_LIST						m_RenderWindowList;
		DepthTextureList				m_depthTexList;	///< depth stencil texture list
	};

	inline void CGraphic::SetShaderConfigPath(const wstring &sPath)
	{
		m_strConfigShaderPath = sPath.c_str();
	}

	inline const wchar_t*  CGraphic::GetShaderConfigPath() const
	{
		return m_strConfigShaderPath.c_str();
	}

	//------------------------------------------------------------------------------
	inline CRenderTarget* CGraphic::GetActiveRenderTarget( void )
	{
		return m_pActiveTarget;
	}

	//------------------------------------------------------------------------------
	inline uint32 CGraphic::GetFrameCount()
	{
		return m_uFrameCount;
	}


	//------------------------------------------------------------------------------
	inline CDynamicVB& CGraphic::GetDynamicVB()
	{
		return *m_pActiveVertexBuffer;
	}

	//------------------------------------------------------------------------------
	inline STATIC_RS& CGraphic::GetStaticRS( ERenderStyle rs )
	{
		return m_RS[rs];
	}

	//------------------------------------------------------------------------------
	inline void CGraphic::ResetRenderStack()
	{
		m_DynamicVB.ResetStack();
	}

	//------------------------------------------------------------------------------
	inline RenderParam& CGraphic::LockRenderStack( EPrimitiveType PrimitiveType, uint32 VertexNum, uint32 PrimitiveCount, uint32 VerFormat, uint32 VerStride, void** ppVB, void** ppIB )
	{
		return m_pActiveVertexBuffer->GetRenderStack( PrimitiveType, VertexNum, PrimitiveCount, VerFormat, VerStride, ppVB, ppIB );
	}

	inline void CGraphic::UnlockRenderStack( RenderParam& RP )
	{
		if (RP.m_RS.m_Texture_S0)RP.m_RS.m_Texture_S0->AddRef();
		if (RP.m_RS.m_Texture_S1)RP.m_RS.m_Texture_S1->AddRef();
		if (RP.m_RS.m_Texture_S2)RP.m_RS.m_Texture_S2->AddRef();
		if (RP.m_RS.m_Texture_S3)RP.m_RS.m_Texture_S3->AddRef();
	}

	inline CDynamicPool*	CGraphic::GetDynamicPool(void)
	{
		return m_pDynamicPool;
	}

	inline ITexture*		CGraphic::GetSystemTexture( ESysTexType eStt )
	{
		return m_pSysTexture[eStt];
	}
}