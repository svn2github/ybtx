#pragma once
#include "GraphicBase.h"
#include "TSingleton.h"
#include "ExsertStructDB.h"

namespace sqr
{
	class ITexture;
	class CGraphic;
	template<class T> struct TRect;
	typedef TRect<float>	CFRect;

	#define MAX_TEXTURE_ARRAY_SIZE 2048

	class CRenderDevice : public Singleton<CRenderDevice>
	{
	private:
		friend class Singleton<CRenderDevice>;
		CRenderDevice();
		virtual ~CRenderDevice();

	private:
		DWORD m_dwTextureCnt;
		map<string, ITexture*> m_pTextureList;
		int m_iTextureCount;

	public:
		map<string, DWORD> m_TerrainTexIndexs; //地表贴图索引名

		void SetDefaultRenderState();
		bool Init(void);

		// 清空纹理队列
		ITexture*	GetTexture(string texname);
		bool		DeleteTexture(string texname);
		void		ClearTextures();

		// 2D Line
		void Draw3DLines(const CVector3f& start, const CVector3f& end, DWORD color);

		// 2D Quad
		void DrawLayer2DWireQuad(int iX, int iY, int iWidth, int iHeight, DWORD color);
		void DrawLayer2DSolidQuad(int iX, int iY, int iWidth, int iHeight, uint32 Color, string name);
		void DrawLayer2DSolidQuad(int iX, int iY, int iWidth, int iHeight, uint32 Color, ITexture *tex, CFRect* rc = NULL);
		void DrawLayer2DSolidQuad(RenderState ** pRS, int iX, int iY, int iWidth, int iHeight, DWORD color, ITexture *tex);
		void DrawLayer2DSolidQuad(RenderState ** pRS, int iX, int iY, int iWidth, int iHeight, DWORD* color, ITexture *tex0, ITexture *tex1);

		// 3D
		void DrawLayer3DWireQuad(CVector3f* vertex, DWORD* color);
		void DrawLayer3DWireQuad(VerColorTex* vertex,  int count);
		void DrawLayer3DWireQuad(RenderState ** pRS, VerColorTex* vertex,  int count);
		void DrawLayer3DWireQuad(RenderState ** pRS, VerColorTex* vertex,  WORD* pIB, int count);
		void DrawLayer3DWireQuad(CVector3f* vertex, DWORD color);
		void DrawLayer3DSolidQuad(CVector3f* vertex, DWORD* color, ITexture* tex);
		void DrawLayer3DSolidQuad(CVector3f* vertex, CVector2f *uv, DWORD* color, ITexture* tex);
		void DrawLayer3DSolidQuad(RenderState ** pRS, CVector3f* vertex, CVector2f *uv, DWORD* color, ITexture* tex, CMatrix matrix);
		void DrawLayer3DSolidQuad(RenderState ** pRS, CVector3f* vertex, CVector2f *uv, DWORD* color, ITexture* tex);
		void DrawLayer3DSolidQuad(VerColorTex* vertex, ITexture* tex, int count);
		void DrawLayer3DSolidQuad(RenderState ** pRS, VerNCT* vertex, ITexture* tex, int count);
		void DrawLayer3DSolidQuad(RenderState ** pRS, VerColorTex* vertex, ITexture* tex, int count);
		void DrawLayer3DSolidQuad(RenderState ** pRS, VerNCT* vertex, ITexture* tex0, ITexture* tex1, int count);
		// 3 layer texture
		void DrawLayer3DSolidQuad(RenderState ** pRS, VerNCT* vertex, ITexture* tex0, ITexture* tex1, ITexture* tex2, int count);
		// 4 layer texture
		void DrawLayer3DSolidQuad(RenderState ** pRS, VerNCT* vertex, ITexture* tex0, ITexture* tex1, ITexture* tex2, ITexture* tex3, int count);

		// shaer
		void DrawLayer3DSolidQuad(RenderState ** pRS, CVector3f* vertex, CVector2f *uv, DWORD* color, ITexture* tex0, CMatrix matrix, int shaderType);
		void DrawLayer3DSolidQuad(RenderState ** pRS, VerNCT1* vertex, ITexture* tex0, ITexture* tex1, ITexture* tex2, ITexture* tex3, int count, int shaderType);
		// VerNCST
		void DrawLayer3DSolidQuad(RenderState ** pRS, VerNCST* vertex, ITexture* tex0, ITexture* tex1, ITexture* tex2, ITexture* tex3, int count, int shaderType);
		// VerNCST1
		void DrawLayer3DSolidQuad(RenderState ** pRS, VerNCST1* vertex, ITexture* tex0, ITexture* tex1, ITexture* tex2, ITexture* tex3, int count, int shaderType);
		void DrawLayer3DSolidQuad(RenderState ** pRS, VerColorTex* vertex, ITexture* tex0, ITexture* tex1, ITexture* tex2, ITexture* tex3, int count, int shaderType);
		void DrawLayer3DWireQuad(RenderState ** pRS, VerColorTex* vertex,  int count, int shaderType);

		void DrawLayer2DWireQuad(int iX, int iY, int iWidth, int iHeight, DWORD color, int shaderType);
		void DrawLayer2DSolidQuad(int iX, int iY, int iWidth, int iHeight, uint32 Color, ITexture *tex, int shaderType);

		// ----------------------------------------------------------------------------

		void DrawLayer3DSolidQuad(RenderState ** pRS, CVector3f* vertex, CVector2f *uv, DWORD* color, ITexture* tex0, ITexture* tex1);
		void DrawLayer3DSolidQuadTex2(RenderState ** pRS, CVector3f* vertex, CVector2f *uv0, CVector2f *uv1, DWORD* color, ITexture* tex0, ITexture* tex1);
	};
}
