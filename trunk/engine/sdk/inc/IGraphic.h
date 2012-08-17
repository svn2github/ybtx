#pragma once

#include "GraphicBase.h"
#include "CPos.h"
#include "CRectangle.h"
#include "CRenderTexture.h"
#include "CGraphicExp.h"
namespace sqr
{
	/// 图形引擎
	class IGraphic : public CGraphicMallocObject
	{
	public:
		virtual void			SetAsynSwitch(bool bEnable) = 0;
		virtual void			EnableAsynRead(bool bEnable) = 0;
		virtual HRESULT			CreateTexture( int32 nWidth, int32 nHeight, eTextureFormat eFormat, ITexture** pText ) = 0;

		virtual void			SwapBegin(void) = 0; //这两个东西很玄妙 不要经常用
		virtual void			SwapEnd(void) = 0; //这两个东西很玄妙 不要经常用

		//CreateEmptyTexture以后需要合并为CreateTexture
		virtual HRESULT			CreateEmptyTexture( int32 nWidth, int32 nHeight, uint32 nLevel, eTextureFormat eFormat, ITexture** pText ) = 0;
		virtual CRenderTexture*	CreateRenderTarget(void) = 0;
		virtual HRESULT			CreateTexture( const wchar_t* alias, const char* szName, ITexture** pText ) = 0;
		virtual HRESULT			CreateFont( int32 id, bool bBold, float fSize, CRenderFont** pFont ) = 0;
		virtual HRESULT			CreatePieceGroup( const char* szName, CPieceGroup** pPieceGroup ) = 0;
		//virtual HRESULT			CreateAnimationsRes( const char* szName, CAnimationsRes** pAnimationGroup ) = 0;

		virtual HRESULT			CreateAnimationGroup( const char* szName, CAnimationGroup** pAnimationGroup ) = 0;

		virtual void			DrawRect( const CFRect& rt, uint32 Color, ITexture* pTexture, const CFRect* rtText = NULL, ERenderStyle rs = RS_GUI, float z = MAX_DEPTH ) = 0;
		virtual void			DrawRect( const CFRect& rt, uint32 Color, ITexture* pTexture, const CPosRect& rtText, ERenderStyle rs = RS_GUI, float z = MAX_DEPTH ) = 0;
		virtual void			DrawSkyBoard( const CFRect& rt, uint32 Color, ITexture* pTexture, const float& uoffset, const float& voffset, ERenderStyle rs = RS_GUI, float z = MAX_DEPTH ) = 0;
		virtual CMatrix*		GetRenderStack( ERenderStyle RenderStyle,
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
			DWORD ShaderID = 0) = 0;
		virtual CMatrix*		GetRenderStack( ERenderStyle RenderStyle,
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
			DWORD ShaderID = 0) = 0;
		virtual CMatrix*		GetRenderThreeLayerStack( ERenderStyle RenderStyle,
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
			DWORD ShaderID = 0 ) = 0;
		virtual void			Draw3DPlane(ITexture* pTexture, CVector3f *vec3d, DWORD Color, ERenderStyle rs, CVector2f *tex_uv) = 0;

		virtual void			DrawRectGroup( ITexture* pTexture, ERenderStyle rs, const RectGroup* rt, int32 nNum ) = 0;
		virtual void            DrawLine( const PointGroup* pPointGroup, int32 nNum, bool b2D = true, ERenderStyle rs = RS_GUI ) = 0;
		/// 绘制几何图形
		virtual void			DrawPrimitive(EPrimitiveType type, const VerColor3D* points, int32 numPoints) {};
	};

};
