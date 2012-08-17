#pragma once
#include "CRenderPipeline.h"
#include "CEditTexture.h"

namespace sqr
{
	class CEditRenderPipeline : public CRenderPipeline
	{
	public:
		CEditRenderPipeline( CRenderPipeline* pPipeline );
		~CEditRenderPipeline();

		void BeginShader( const SHADER_HANDLE& shader );
		void EndShader();

		static CEditRenderPipeline* GetInst();

	private:
		SHADER_HANDLE m_ShaderHandle;

	public:	
		virtual void SetVertexShaderF( uint id , float Param1 = 0.0f, float Param2 = 0.0f, 
						float Param3 = 0.0f, float Param4 = 0.0f );
		virtual void SetVertexShaderF( uint id, const void* pdata, size_t float4Count );
		
		virtual void GetVertexShaderF( uint id, void* pdata, size_t float4Count );
		
		virtual void SetFragmentShaderF( uint id, float Param1 = 0.0f, float Param2 = 0.0f, 
						float Param3 = 0.0f, float Param4 = 0.0f );
		
		virtual void SetFragmentShaderF( uint id, const void* pdata, size_t float4Count );
		
		virtual void GetFragmentShaderF( uint id, void* pdata, size_t float4Count );

		virtual void _setVertexDeclaration( CVertexDeclaration* decl );
		
		virtual void _setVertexBufferBinding( CVertexBufferBinding* binding );
		
		virtual void SetLight( index_t lightIndex, const LightStruct* light );

		virtual void SetTextureMatrixImmediate( uint32 uTextStage, const CMatrix* pMatrix );
		
		virtual	void InitPipeline(void);
		
		virtual	void ReleasePipeline(void);
		
		virtual	bool Begin(void);
		
		virtual	void End(void);
		
		virtual void ClearDepth( float depth = 1.0f );
		
		virtual void ClearRender( const CColor& color );
		
		virtual void ClearTarget( CRenderTarget* pTarget );
		
		virtual void SetViewport( CIRect& Rect );

		virtual void _SetRenderStyle( const RENDERSTATETYPE& rsType, DWORD fValue );
		
		virtual void _SetSamplerState( uint i, const SAMPLERSTATETYPE& ssType, DWORD fValue );
		
		virtual void _SetTextureStageState( uint i, const TEXTURESTAGESTATETYPE& tsType, DWORD fValue );
		
		virtual void _SetTexture( uint i, ITexture* pText );
		
		virtual void _SetMaterial( const sMaterial& material );
		
		virtual void _SetClipPlane( uint i,const CPlane& Plane );

		virtual void SetCamera( CCamera* pCamera );
		
		virtual void SetProject( const CMatrix& Project );
		
		virtual void SetView( const CMatrix& View );
		
		virtual void SetWorld( const CMatrix& World );

		virtual bool Render( CRenderOperation* op,RenderParam* rp );
		
		virtual bool RenderEx( CRenderOperation* op );

		virtual bool BeginBatch( CRenderOperation* op,UINT I );
		
		virtual bool SetBatchBuffer( UINT Source, BufferBindingInfo* pVBInfo, DWORD Flag );
		
		virtual bool BatchRender(void);
		
		virtual bool EndBatch(void);

		virtual void SetTexture( const STATIC_RS& RenderStyle );
		
		virtual void SetTextureEx( const STATIC_RS& RenderStyle );
		
		virtual void SetRenderStyle( const STATIC_RS& RenderStyle );
		
		virtual void ResetRenderState();
		
		virtual void SetShader( const SHADER_HANDLE& shaderID );

		virtual void SetStencil( const StencilParam& Stencil );
		
		virtual void SetTextureMatrix( uint32 uTextStage, const CMatrix* pMatrix );

		virtual CMatrix* GetTextureMat( uint32 uTextStage );

		virtual bool boxProbe( const CVector3f& Min, const CVector3f& Max, const CVector3f& RayPosition, 
						const CVector3f& RayDirection);

		virtual CMatrix& MatrixInverse( CMatrix& matrix );

		virtual size_t	GetFaceCount(void);
		virtual size_t	GetVertexCount(void);
		virtual size_t	GetPrimitiveCount(void);

		virtual void EnableFog( bool bEnable );
		virtual void SetFog( uint32 Color, float Start, float End );
		virtual void SetFogColor( const uint32 Color );
		virtual void SetLinerFogPara( const float fogstart, const float fogend );
		virtual void SetFogMode( FogModeType fogmode );
		virtual void SetFogDensity( const float density );
		virtual bool IsFogEnabled(void);
		virtual	void UpdateFog();

		virtual	void SetZDelta( float Z );
		virtual void SetAmbient( uint32 Color );
		virtual	void SetAmbientSrcAsMaterial( bool bMaterial );

		virtual void SetLimitFps( float fLFps  );

		virtual void _SetDefaultTexture( ITexture* pDefTex );

		//
		size_t	GetTexCount(void);

	protected:
		virtual bool _BeginBatch(void);
		virtual bool _SetBatchBuffer( UINT Source, BufferBindingInfo* pVBInfo, DWORD Flag );
		virtual bool _EndBatch(void);
		virtual void _UpdateView( void );
		virtual bool _Render( CRenderOperation* op );

	private:
		CEditRenderPipeline( const CEditRenderPipeline& );
		CEditRenderPipeline& operator = ( const CEditRenderPipeline& );

		
		//纹理统计
		bool IsTexCount;
		set<ITexture*> m_pTexSet;
		uint TexNum;
		
		//是否开启像素填充率统计
		bool IsPixelFillCount;
		
		//纹理替换功能
		bool IsInsteadTex;
		ITexture* InsteadTex;

		//基本参数统计 包括点数 面数和DP数
		bool IsBasicParamCount;

		//是否刷新texture factor
		bool bReFreshTexFactor;
	public:
		//基本参数统计开光
		void EnableBasicParamCount();
		void DisableBasicParamCount();
		//像素填充率开关
		void EnablePixelFillCount();
		void DisablePixelFillCount();
		//纹理统计开关
		void EnalbeTexCount();
		void DisableTexCount();
		//纹理替换功能开关
		void EnableTexInstead(ITexture* tex=NULL);
		void DisableTexInstead();
		void UseColorInsteadTex();
		void UseGridInsteadTex();
		bool IsEnableTexInstead();
		void SetReFreshTexFactor(bool b);
		void GetCurFrameSceneTextureList( vector<ITexture *>& texNameList );

	private:
		class CRefRenderPipeline : public CRenderPipeline
		{
			friend class CEditRenderPipeline;
		};

		CRefRenderPipeline* m_pipeline;
	};
}