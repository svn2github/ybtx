#pragma once
#include "CRenderStyle.h"
#include "GraphicBase.h"
#include "CGpuProgramMgr.h"

namespace sqr
{
//便于移植
class CRenderOperation;
class CIndexHardwareBuffer;
class CVertexBufferBinding;
class CVertexDeclaration;
struct BufferBindingInfo;
class CGraphic;

template <typename T, typename U>
T& type_cast(const U& u)
{
	return *((T*)(&u));
}

extern void DebugText(uint color, const char* fmt, ...);

class CRenderPipeline 
	: public Singleton<CRenderPipeline>
	, public CGraphicMallocObject
{
	friend class CGraphic;
public:
	CRenderPipeline();
	virtual ~CRenderPipeline();

public:	
	virtual void SetVertexShaderF( uint id , float Param1 = 0.0f
		, float Param2 = 0.0f , float Param3 = 0.0f , float Param4 = 0.0f )				= 0;
	virtual void SetVertexShaderF(uint id,const void* pdata,size_t float4Count)			= 0;
	virtual void GetVertexShaderF(uint id,void* pdata,size_t float4Count)				= 0;
	virtual void SetFragmentShaderF( uint id , float Param1 = 0.0f
		, float Param2 = 0.0f , float Param3 = 0.0f , float Param4 = 0.0f )				= 0;
	virtual void SetFragmentShaderF(uint id,const void* pdata,size_t float4Count)		= 0;
	virtual void GetFragmentShaderF(uint id,void* pdata,size_t float4Count)				= 0;
	
	virtual void _setVertexDeclaration(CVertexDeclaration* decl)						= 0;
	virtual void _setVertexBufferBinding(CVertexBufferBinding* binding)					= 0;
	virtual void SetLight(index_t lightIndex,const LightStruct* light)					= 0;

	virtual void SetTextureMatrixImmediate( uint32 uTextStage, const CMatrix* pMatrix )	= 0;
	virtual	void InitPipeline(void )												= 0;
	virtual	void ReleasePipeline(void)												= 0;
	virtual	bool Begin(void)														= 0;
	virtual	void End(void)															= 0;
	virtual void ClearDepth(float depth = 1.0f)										= 0;
	virtual void ClearRender( const CColor& color )									= 0;
	virtual void ClearTarget( CRenderTarget* pTarget )								= 0;
	virtual void SetViewport( CIRect& Rect )										= 0;

public:
	virtual void _SetRenderStyle( const RENDERSTATETYPE& rsType, DWORD fValue )		= 0;
	virtual void _SetSamplerState( uint i,const SAMPLERSTATETYPE& ssType, DWORD fValue )	= 0;
	virtual void _SetTextureStageState( uint i,const TEXTURESTAGESTATETYPE& tsType, DWORD fValue )	= 0;
	virtual void _SetTexture( uint i, ITexture* pText )								= 0;
	virtual void _SetMaterial( const sMaterial& material )							= 0;
	virtual void _SetClipPlane( uint i,const CPlane& Plane )						= 0;

public:
	virtual void SetCamera(CCamera* pCamera);
	virtual void SetProject( const CMatrix& Project );
	virtual void SetView( const CMatrix& View );
	virtual void SetWorld( const CMatrix& World );

	virtual bool Render(CRenderOperation* op,RenderParam* rp);
	virtual bool RenderEx(CRenderOperation* op);
	
	virtual bool BeginBatch(CRenderOperation* op,UINT I);
	virtual bool SetBatchBuffer(UINT Source, BufferBindingInfo* pVBInfo, DWORD Flag);
	virtual bool BatchRender(void);
	virtual bool EndBatch(void);
	
	virtual void SetTexture( const STATIC_RS& RenderStyle );
	virtual void SetTextureEx( const STATIC_RS& RenderStyle );
	virtual void SetRenderStyle(const STATIC_RS& RenderStyle);
	virtual void ResetRenderState();
	virtual void SetShader(const SHADER_HANDLE& shaderID);

	virtual void SetStencil( const StencilParam& Stencil );
	virtual void SetTextureMatrix( uint32 uTextStage, const CMatrix* pMatrix );
	
	virtual CMatrix* GetTextureMat(uint32 uTextStage);

	virtual void _SetDefaultTexture(ITexture* pDefTex);
protected:
	virtual bool _BeginBatch(void)					= 0;
	virtual bool _SetBatchBuffer(UINT Source, BufferBindingInfo* pVBInfo, DWORD Flag) = 0;
	virtual bool _EndBatch(void)					= 0;
	virtual void _UpdateView( void )				= 0;
	virtual bool _Render( CRenderOperation* op )	= 0;
public:
	//-----临时函数-----
	//virtual void makePerspectiveMatrix( CMatrix& projectMat, float fovy, float aspect, float nearPlane, float farPlane ) = 0;
	//virtual void makeOrthoMatrix( CMatrix& projectMat,  float width, float height, float nearPlane, float farPlane ) = 0;
	virtual bool boxProbe( const CVector3f& Min, const CVector3f& Max, const CVector3f& RayPosition, const CVector3f& RayDirection) = 0;
	
	// 不知道D3D的inverse咋写的, 自己写的就是跟它的结果不一样
	virtual CMatrix& MatrixInverse(CMatrix& matrix) = 0;
	
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
	virtual	void UpdateFog()	= 0;

	virtual	void SetZDelta( float Z )						= 0;
	virtual void SetAmbient( uint32 Color )					= 0;
	virtual	void SetAmbientSrcAsMaterial( bool bMaterial )	= 0;

	//inline void ShowFPS(bool isShow)
	//{
	//	m_isShowFps = isShow;
	//}
	virtual void SetLimitFps( float fLFps  )
	{
		m_fLimitFps = fLFps;
	}

protected:
	//以下四个矩阵相对位置不能随意改变
	CMatrix				m_CurProj;
	CMatrix				m_CurView;
	CMatrix				m_CurWorld;
	CMatrix				m_CurViewProj;

	float				m_fZDelta;
	CGpuProgram*		m_pCurGpuPro;

protected:
	FogModeType				m_FogMode;       ///雾的计算模式
	bool					m_FogEnable;
	uint32					m_FogColor;      ///雾效颜色
	float					m_fFogDensity;  ///雾的浓度
	float					m_fFogSatrt;	 ///雾起始点
	float					m_fFogEnd;		 ///雾终止点
	StencilParam			m_Stencil;
	//bool					m_isShowFps;
	bool					m_isNeedUpdateWorld;

protected:
	CRenderOperation*		m_ActiveRenderOp;
	UINT					m_BatchNum;

	uint32 m_Ambient;
protected:
	//数据统计
	size_t	m_sFaceCount; //利用我绘制了多少面？
	size_t	m_sVertexCount; //利用我绘制了多少点？
	size_t	m_sDrawPrimitiveCount; //提交次数
	size_t	m_sRenderVal; //本模型面数

protected:
	CGraphic*	m_pGraphic;
	CGpuProgramMgr*	m_pGpuMgr;
	CVertexDeclaration*		m_pVectexDec;
	CIndexHardwareBuffer*	m_pIndexBuffer;
	CVertexBufferBinding*	m_pBinding;
	CVertexHardwareBuffer*	m_pMainBuffer;
	STATIC_RS				m_GraphicRenderStyle;
	StencilParam			m_GraphicStencilParam;

	//纹理旋转矩阵
	typedef GVector<CMatrix>	MatrixVector;
	typedef GVector<bool>		BoolVector;
	typedef GVector<size_t>		SizeVector;

	MatrixVector			m_TextureMatrix;
	BoolVector				m_bTexMatEnable;
	MatrixVector			m_TextBufMatrix;
	SizeVector				m_BufStart;
	BoolVector				m_BufEnable;

	float					m_fLimitFps;
	ITexture*				m_pDefaultTex;
};
}