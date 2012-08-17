#include "stdafx.h"
#include "Render.h"
#include "CGraphic.h"
#include "BaseHelper.h"
#include "CTerrainMgr.h"
#include "CWindowTarget.h"

namespace sqr
{
	// ----------------------------------------------------------------------------
	
	CRenderDevice::CRenderDevice()
	{
		m_dwTextureCnt = 0;
		m_iTextureCount = 0;;
	}

	CRenderDevice::~CRenderDevice() 
	{ 

	}

	bool CRenderDevice::Init(/*HWND hWnd, RECT rc, const TCHAR * pszConfigFile*/)
	{
		//设置渲染队列顺序
		RenderQueueFlag SceneFlag;
		SceneFlag
			<<ID_SKYBOX_FILTER
			<<ID_SHADOW_FILTER
			<<ID_NPC_FILTER
			<<ID_SCENE_FILTER
			<<ID_TERRAIN_FILTER
			<<ID_WATER_FILTER		
			<<ID_ALPHA_NPC_FILTER
			<<ID_ALPHA_SCENE_FILTER
			<<ID_NPC_EFFECT_FILTER
			<<ID_SCENE_EFFECT_FILTER
			<<ID_SCENE_SHADOWRECEIVET_FILTER;

		CMainWindowTarget::GetInst()->GetGroup()->SetRenderQueueFlag(SceneFlag);

		return true;
	}

	void CRenderDevice::Draw3DLines(const CVector3f& start, const CVector3f& end, DWORD color)
	{
 		PointGroup PG[2];
 		PG[0].uColor = PG[1].uColor = color;
	 
 		PG[0].vPoint = CVector3f(start.x, start.y, start.z);
 		PG[1].vPoint = CVector3f(end.x, end.y, end.z);

		int nNum = 2;
		VerColor3D* pVB;
		RenderState *pRS;

		CGraphic::GetInst()->GetRenderStack(RS_DEFAULT, ROS_LIGHTMAT1, NULL, NULL, PT_LINESTRIP, nNum, nNum-1,
			VerColor3D::Format, sizeof(VerColor3D), (void**)&pVB, NULL, (void**)&pRS )->Identity();
		pRS->m_LightEnable = false;
		pRS->m_ZTestEnable = true;
		pRS->m_ZWrite = true;
		pRS->m_Cull = CULLT_CCW;

		memcpy( pVB, PG, sizeof(PointGroup)*nNum );
	}

	void CRenderDevice::DrawLayer2DWireQuad(int iX, int iY, int iWidth, int iHeight, DWORD color)
	{
		PointGroup PG[5];
		PG[0].uColor = 
		PG[1].uColor = 
		PG[2].uColor = 
		PG[3].uColor = 
		PG[4].uColor = color;

		float ix = float(iX), iy = float(iY), width = float(iWidth), height = float(iHeight);
		PG[0].vPoint = CVector3f(ix, iy, 0.0f);
		PG[1].vPoint = CVector3f(ix + width, iy, 0.0f);
		PG[2].vPoint = CVector3f(ix + width, iy + height, 0.0f);
		PG[3].vPoint = CVector3f(ix, iy + height, 0.0f);
		PG[4].vPoint = CVector3f(ix, iy, 0.0f);

		CGraphic::GetInst()->DrawLine( PG, 5, true );
	}

	void CRenderDevice::DrawLayer2DSolidQuad(RenderState ** pRS, int iX, int iY, int iWidth, int iHeight, DWORD color, ITexture *tex)
	{
		float ix = float(iX), iy = float(iY), width = float(iWidth), height = float(iHeight);

		Vertex2D vs[4];
		vs[0] = Vertex2D( ix,          iy + height, 0.0, color, 0, 1);
		vs[1] = Vertex2D( ix,          iy,           0.0, color, 0, 0);
		vs[2] = Vertex2D( ix + width,  iy + height, 0.0, color, 1, 1);
		vs[3] = Vertex2D( ix + width,  iy,           0.0, color, 1, 0);

		Vertex2D* pVB;

		CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, tex, NULL, PT_TRIANGLESTRIP, 4, 2, 
			Vertex2D::Format , sizeof(Vertex2D), (void**)&pVB, NULL, (void**)pRS)->Identity();

		memcpy(pVB,vs,sizeof(vs));
	}

	void CRenderDevice::DrawLayer2DSolidQuad(RenderState ** pRS, int iX, int iY, int iWidth, int iHeight, DWORD* color, ITexture *tex0, ITexture *tex1)
	{
		float ix = float(iX), iy = float(iY), width = float(iWidth), height = float(iHeight);

		Vertex2D vs[4];
		vs[0] = Vertex2D( ix,          iy + height, 0.0, color[0], 0, 1);
		vs[1] = Vertex2D( ix,          iy,           0.0, color[1], 0, 0);
		vs[2] = Vertex2D( ix + width,  iy + height, 0.0, color[2], 1, 1);
		vs[3] = Vertex2D( ix + width,  iy,           0.0, color[3], 1, 0);

		Vertex2D* pVB;

		CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, tex0, tex1, PT_TRIANGLESTRIP, 4, 2, 
			Vertex2D::Format , sizeof(Vertex2D), (void**)&pVB, NULL, (void**)pRS)->Identity();

		memcpy(pVB,vs,sizeof(vs));
	}

	void CRenderDevice::DrawLayer2DSolidQuad(int iX, int iY, int iWidth, int iHeight, uint32 Color, string name)
	{
		SQR_TRY
		{
			float ix = float(iX), iy = float(iY), width = float(iWidth), height = float(iHeight);

			ITexture *tex = NULL;

			if( name == "" )
				tex = NULL;
			else
				tex = GetTexture(name);

			if( tex == NULL )
				return;

			CFRect rect;

			rect.left = ix;
			rect.bottom = iy + height;
			rect.right = ix + width;
			rect.top = iy;

			CGraphic::GetInst()->DrawRect( rect, Color, tex );
		}
		SQR_CATCH(exp)
		{
			MessageBox(NULL, exp.ErrorMsg(), "DrawLayer2DSolidQuad错误", MB_OK);

			return;
		}
		SQR_TRY_END;
	}

	void CRenderDevice::DrawLayer2DSolidQuad(int iX, int iY, int iWidth, int iHeight, uint32 Color, ITexture *tex,CFRect* rc)
	{
		if( NULL == rc )
		{
			CFRect rect;
			float ix = float(iX), iy = float(iY), width = float(iWidth), height = float(iHeight);

			rect.left = ix;
			rect.bottom = iy + height;
			rect.right = ix + width;
			rect.top = iy;

			CGraphic::GetInst()->DrawRect( rect, Color, tex, rc );
		}
		else
		{
			float ix = float(iX), iy = float(iY), width = float(iWidth), height = float(iHeight);
			RenderState * pRS = NULL;
			Vertex2D vs[4];
			vs[0] = Vertex2D( ix,          iy + height, 0.0, Color, rc->left, rc->bottom);
			vs[1] = Vertex2D( ix,          iy,           0.0, Color, rc->left, rc->top);
			vs[2] = Vertex2D( ix + width,  iy + height, 0.0, Color, rc->right, rc->bottom);
			vs[3] = Vertex2D( ix + width,  iy,           0.0, Color, rc->right, rc->top);

			Vertex2D* pVB;

			CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, tex, NULL, PT_TRIANGLESTRIP, 4, 2, 
				Vertex2D::Format , sizeof(Vertex2D), (void**)&pVB, NULL, (void**)pRS)->Identity();

			memcpy(pVB,vs,sizeof(vs));
		}
	}

	void CRenderDevice::DrawLayer3DWireQuad(CVector3f* vertex, DWORD* color)
	{
		VerColorTex vs[4] = {
			VerColorTex( vertex[0].x, vertex[0].y, vertex[0].z, color[0], 0, 0),
			VerColorTex( vertex[1].x, vertex[1].y, vertex[1].z, color[1], 0, 1),
			VerColorTex( vertex[2].x, vertex[2].y, vertex[2].z, color[2], 1, 1),
			VerColorTex( vertex[3].x, vertex[3].y, vertex[3].z, color[3], 1, 0),
		};

		VerColorTex* pVB;
		RenderState * pRS;

		CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, NULL, NULL, PT_TRIANGLESTRIP, 4, 2, 
			VerColorTex::Format , sizeof(VerColorTex), (void**)&pVB, NULL, (void**)&pRS )->Identity();

		pRS->m_FillMode = FILL_WIREFRAME;

		memcpy(pVB,vs,sizeof(vs));
	}

	void CRenderDevice::DrawLayer3DWireQuad(CVector3f* vertex, DWORD color)
	{
		VerColorTex vs[4] = {
			VerColorTex( vertex[0].x, vertex[0].y, vertex[0].z, color, 0, 0),
			VerColorTex( vertex[1].x, vertex[1].y, vertex[1].z, color, 0, 1),
			VerColorTex( vertex[2].x, vertex[2].y, vertex[2].z, color, 1, 1),
			VerColorTex( vertex[3].x, vertex[3].y, vertex[3].z, color, 1, 0),
		};

		VerColorTex* pVB;
		RenderState * pRS;

		CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, NULL, NULL, PT_TRIANGLESTRIP, 4, 2, 
			VerColorTex::Format , sizeof(VerColorTex), (void**)&pVB, NULL, (void**)&pRS )->Identity();

		pRS->m_FillMode = FILL_WIREFRAME;

		memcpy(pVB,vs,sizeof(vs));
	}

	void CRenderDevice::DrawLayer3DWireQuad(VerColorTex* vertex,  int count)
	{
		VerColorTex* pVB;
		RenderState * pRS;

		CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, NULL, NULL, PT_TRIANGLELIST, count * 6, count * 2, 
			VerColorTex::Format , sizeof(VerColorTex), (void**)&pVB, NULL, (void**)&pRS )->Identity();

		pRS->m_FillMode = FILL_WIREFRAME;

		memcpy(pVB,vertex,sizeof(VerColorTex) * count * 6);
	}

	void CRenderDevice::DrawLayer3DWireQuad(RenderState ** pRS, VerColorTex* vertex,  int count)
	{
		VerColorTex* pVB;

		CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, NULL, NULL, PT_TRIANGLELIST, count * 6, count * 2, 
			VerColorTex::Format , sizeof(VerColorTex), (void**)&pVB, NULL, (void**)pRS )->Identity();

		(*pRS)->m_FillMode = FILL_WIREFRAME;

		memcpy(pVB,vertex,sizeof(VerColorTex) * count * 6);
	}

	void CRenderDevice::DrawLayer3DWireQuad(RenderState ** pRS, VerColorTex* vertex,  WORD* pIB, int count)
	{
		VerColorTex* pVB;
		WORD* ppIB;

		CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, NULL, NULL, PT_TRIANGLELIST, count * 4, count * 2, 
			VerColorTex::Format , sizeof(VerColorTex), (void**)&pVB, (void**)&ppIB, (void**)pRS )->Identity();

		(*pRS)->m_FillMode = FILL_WIREFRAME;

		memcpy(pVB,vertex,sizeof(VerColorTex) * count * 4);
		memcpy(ppIB,pIB,sizeof(WORD) * count * 6);
	}

	void CRenderDevice::DrawLayer3DSolidQuad(CVector3f* vertex, DWORD* color, ITexture* tex)
	{
		VerColorTex vs[4] = {
			VerColorTex( vertex[0].x, vertex[0].y, vertex[0].z, color[0], 0, 1),
			VerColorTex( vertex[1].x, vertex[1].y, vertex[1].z, color[1], 0, 0),
			VerColorTex( vertex[2].x, vertex[2].y, vertex[2].z, color[2], 1, 1),
			VerColorTex( vertex[3].x, vertex[3].y, vertex[3].z, color[3], 1, 0),
		};

		VerColorTex* pVB;
		RenderState * pRS;

		CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, tex, NULL, PT_TRIANGLESTRIP, 4, 2, 
			VerColorTex::Format , sizeof(VerColorTex), (void**)&pVB, NULL, (void**)&pRS )->Identity();

		memcpy(pVB,vs,sizeof(vs));
	}

	void CRenderDevice::DrawLayer3DSolidQuad(CVector3f* vertex, CVector2f *uv, DWORD* color, ITexture* tex)
	{
		VerColorTex vs[4] = {
			VerColorTex( vertex[0].x, vertex[0].y, vertex[0].z, color[0], uv[0].x, uv[0].y),
			VerColorTex( vertex[1].x, vertex[1].y, vertex[1].z, color[1], uv[1].x, uv[1].y),
			VerColorTex( vertex[2].x, vertex[2].y, vertex[2].z, color[2], uv[2].x, uv[2].y),
			VerColorTex( vertex[3].x, vertex[3].y, vertex[3].z, color[3], uv[3].x, uv[3].y),
		};

		VerColorTex* pVB;
		RenderState * pRS;

		CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, tex, NULL, PT_TRIANGLESTRIP, 4, 2, 
			VerColorTex::Format , sizeof(VerColorTex), (void**)&pVB, NULL, (void**)&pRS )->Identity();

		memcpy(pVB,vs,sizeof(vs));
	}

	void CRenderDevice::DrawLayer3DSolidQuad(VerColorTex* vertex, ITexture* tex, int count)
	{
		VerColorTex* pVB;
		RenderState * pRS;

		CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, tex, NULL, PT_TRIANGLELIST, count * 6, count * 2, 
			VerColorTex::Format , sizeof(VerColorTex), (void**)&pVB, NULL, (void**)&pRS )->Identity();
			
		memcpy(pVB,vertex,sizeof(VerColorTex) * count * 6);

	}

	void CRenderDevice::DrawLayer3DSolidQuad(RenderState ** pRS, VerNCT* vertex, ITexture* tex, int count)
	{
		VerNCT* pVB;
		CGraphic::GetInst()->GetRenderStack( RS_DEFAULT,
			ROS_LIGHTMAT2, tex, NULL, PT_TRIANGLELIST, count * 6, count * 2, 
			VerNCT::Format , sizeof(VerNCT), (void**)&pVB, NULL, (void**)pRS)->Identity();

		memcpy(pVB,vertex,sizeof(VerNCT) * count * 6);	
	}

	void CRenderDevice::DrawLayer3DSolidQuad(RenderState ** pRS, VerColorTex* vertex, ITexture* tex, int count)
	{
		VerColorTex* pVB;

		CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, tex, NULL, PT_TRIANGLELIST, count * 6, count * 2, 
			VerColorTex::Format , sizeof(VerColorTex), (void**)&pVB, NULL, (void**)pRS)->Identity();
			
		memcpy(pVB,vertex,sizeof(VerColorTex) * count * 6);
	}

	void CRenderDevice::DrawLayer3DSolidQuad(RenderState ** pRS, VerNCT* vertex, ITexture* tex0, ITexture* tex1, int count)
	{
		VerNCT* pVB;

		CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, tex0, tex1, PT_TRIANGLELIST, count * 6, count * 2, 
			VerNCT::Format , sizeof(VerNCT), (void**)&pVB, NULL, (void**)pRS)->Identity();
			
		memcpy(pVB,vertex,sizeof(VerNCT) * count * 6);
	}

	// 3 layer texture
	void CRenderDevice::DrawLayer3DSolidQuad(RenderState ** pRS, VerNCT* vertex, ITexture* tex0, ITexture* tex1, ITexture* tex2, int count)
	{
		VerNCT* pVB;

		CGraphic::GetInst()->GetRenderThreeLayerStack( RS_DEFAULT, tex0, tex1, tex2, PT_TRIANGLELIST, count * 6, count * 2, 
			VerNCT::Format , sizeof(VerNCT), (void**)&pVB, NULL, (void**)pRS)->Identity();

		memcpy(pVB,vertex,sizeof(VerNCT) * count * 6);
	}

	// 4 layer texture
	void CRenderDevice::DrawLayer3DSolidQuad(RenderState ** pRS, VerNCT* vertex, ITexture* tex0, ITexture* tex1, ITexture* tex2, ITexture* tex3, int count)
	{
		VerNCT* pVB;

		CGraphic::GetInst()->GetRenderFourLayerStack( RS_DEFAULT, tex0, tex1, tex2, tex3, PT_TRIANGLELIST, count * 6, count * 2, 
			VerNCT::Format , sizeof(VerNCT), (void**)&pVB, NULL, (void**)pRS)->Identity();

		memcpy(pVB,vertex,sizeof(VerNCT) * count * 6);
	}

	// 地编shader接口
	void CRenderDevice::DrawLayer3DSolidQuad(RenderState ** pRS, VerNCT1* vertex, ITexture* tex0, ITexture* tex1, ITexture* tex2, ITexture* tex3, int count, int shaderType)
	{
		VerNCT1* pVB;
		CGraphic::GetInst()->GetRenderFourLayerStack( RS_DEFAULT, tex0, tex1, tex2, tex3, PT_TRIANGLELIST, count * 6, count * 2, 
				VerNCT1::Format , sizeof(VerNCT1), (void**)&pVB, NULL, (void**)pRS,shaderType)->Identity();
		memcpy(pVB,vertex,sizeof(VerNCT1) * count * 6);
	}

	void CRenderDevice::DrawLayer3DSolidQuad(RenderState ** pRS, VerNCST* vertex, ITexture* tex0, ITexture* tex1, ITexture* tex2, ITexture* tex3, int count, int shaderType)
	{
		VerNCST* pVB;
		CGraphic::GetInst()->GetRenderFourLayerStack( RS_DEFAULT, tex0, tex1, tex2, tex3, PT_TRIANGLELIST, count * 6, count * 2, 
				VerNCST::Format , sizeof(VerNCST), (void**)&pVB, NULL, (void**)pRS,shaderType)->Identity();
		memcpy(pVB,vertex,sizeof(VerNCST) * count * 6);
	}

	void CRenderDevice::DrawLayer3DSolidQuad(RenderState ** pRS, VerNCST1* vertex, ITexture* tex0, ITexture* tex1, ITexture* tex2, ITexture* tex3, int count, int shaderType)
	{
		VerNCST1* pVB;
		CGraphic::GetInst()->GetRenderFourLayerStack( RS_DEFAULT, tex0, tex1, tex2, tex3, PT_TRIANGLELIST, count * 6, count * 2, 
			VerNCST1::Format , sizeof(VerNCST1), (void**)&pVB, NULL, (void**)pRS,shaderType)->Identity();
		memcpy(pVB,vertex,sizeof(VerNCST1) * count * 6);
	}

	void CRenderDevice::DrawLayer3DSolidQuad(RenderState ** pRS, VerColorTex* vertex, ITexture* tex0, ITexture* tex1, ITexture* tex2, ITexture* tex3, int count, int shaderType)
	{
		VerColorTex* pVB;
		CGraphic::GetInst()->GetRenderFourLayerStack( RS_DEFAULT, tex0, tex1, tex2, tex3, PT_TRIANGLELIST, count * 6, count * 2, 
				VerColorTex::Format , sizeof(VerColorTex), (void**)&pVB, NULL, (void**)pRS,shaderType)->Identity();
		memcpy(pVB,vertex,sizeof(VerColorTex) * count * 6);
	}

	void CRenderDevice::DrawLayer3DWireQuad(RenderState ** pRS, VerColorTex* vertex,  int count, int shaderType)
	{
		VerColorTex* pVB;
		CGraphic::GetInst()->GetRenderFourLayerStack( RS_DEFAULT, NULL, NULL, NULL, NULL, PT_TRIANGLELIST, count * 6, count * 2, 
				VerColorTex::Format , sizeof(VerColorTex), (void**)&pVB, NULL, (void**)pRS,shaderType )->Identity();
		memcpy(pVB,vertex,sizeof(VerColorTex) * count * 6);
	}

	void CRenderDevice::DrawLayer3DSolidQuad(RenderState ** pRS, CVector3f* vertex, CVector2f *uv, DWORD* color, ITexture* tex0, CMatrix matrix, int shaderType)
	{
		VerColorTex vs[4] = {
			VerColorTex( vertex[0].x, vertex[0].y, vertex[0].z, color[0], uv[0].x, uv[0].y),
			VerColorTex( vertex[1].x, vertex[1].y, vertex[1].z, color[1], uv[1].x, uv[1].y),
			VerColorTex( vertex[2].x, vertex[2].y, vertex[2].z, color[2], uv[2].x, uv[2].y),
			VerColorTex( vertex[3].x, vertex[3].y, vertex[3].z, color[3], uv[3].x, uv[3].y),
		};

		CMatrix *pcm;
		VerColorTex* pVB;
		pcm =CGraphic::GetInst()->GetRenderFourLayerStack( RS_DEFAULT, tex0, NULL, NULL, NULL, PT_TRIANGLESTRIP, 4, 2, 
				VerColorTex::Format , sizeof(VerColorTex), (void**)&pVB, NULL, (void**)pRS,shaderType );
		memcpy(pVB,vs,sizeof(vs));
		*pcm = matrix;
	}

	void CRenderDevice::DrawLayer2DWireQuad(int iX, int iY, int iWidth, int iHeight, DWORD color, int shaderType)
	{
		float ix = float(iX), iy = float(iY), width = float(iWidth), height = float(iHeight);

		Vertex2D vs[4];
		vs[0] = Vertex2D( ix,          iy + height, 0.0, color, 0, 1);
		vs[1] = Vertex2D( ix,          iy,           0.0, color, 0, 0);
		vs[2] = Vertex2D( ix + width,  iy + height, 0.0, color, 1, 1);
		vs[3] = Vertex2D( ix + width,  iy,           0.0, color, 1, 0);

		Vertex2D* pVB;
		RenderState * pRS = NULL;
		CGraphic::GetInst()->GetRenderFourLayerStack( RS_DEFAULT, NULL, NULL, NULL, NULL, PT_TRIANGLESTRIP, 4, 2, 
				Vertex2D::Format , sizeof(Vertex2D), (void**)&pVB, NULL, (void**)pRS,shaderType)->Identity();
		memcpy(pVB,vs,sizeof(vs));
	}

	void CRenderDevice::DrawLayer2DSolidQuad(int iX, int iY, int iWidth, int iHeight, uint32 Color, ITexture *tex, int shaderType)
	{
		float ix = float(iX), iy = float(iY), width = float(iWidth), height = float(iHeight);
		RenderState * pRS = NULL;
		Vertex2D vs[4];
		vs[0] = Vertex2D( ix,          iy + height, 0.0, Color, 0, 1);
		vs[1] = Vertex2D( ix,          iy,           0.0, Color, 0, 0);
		vs[2] = Vertex2D( ix + width,  iy + height, 0.0, Color, 1, 1);
		vs[3] = Vertex2D( ix + width,  iy,           0.0, Color, 1, 0);

		Vertex2D* pVB;
		CGraphic::GetInst()->GetRenderFourLayerStack( RS_DEFAULT, tex, NULL, NULL, NULL, PT_TRIANGLESTRIP, 4, 2, 
				Vertex2D::Format , sizeof(Vertex2D), (void**)&pVB, NULL, (void**)pRS,shaderType)->Identity();

		memcpy(pVB,vs,sizeof(vs));
	}

	// ----------------------------------------------------------------------------
	
	void CRenderDevice::DrawLayer3DSolidQuad(RenderState ** pRS, CVector3f* vertex, CVector2f *uv, DWORD* color, ITexture* tex, CMatrix matrix)
	{
		VerColorTex vs[4] = {
			VerColorTex( vertex[0].x, vertex[0].y, vertex[0].z, color[0], uv[0].x, uv[0].y),
			VerColorTex( vertex[1].x, vertex[1].y, vertex[1].z, color[1], uv[1].x, uv[1].y),
			VerColorTex( vertex[2].x, vertex[2].y, vertex[2].z, color[2], uv[2].x, uv[2].y),
			VerColorTex( vertex[3].x, vertex[3].y, vertex[3].z, color[3], uv[3].x, uv[3].y),
		};

		CMatrix *pcm;
		VerColorTex* pVB;

		pcm =CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, tex, NULL, PT_TRIANGLESTRIP, 4, 2, 
			VerColorTex::Format , sizeof(VerColorTex), (void**)&pVB, NULL, (void**)pRS );

		memcpy(pVB,vs,sizeof(vs));
		*pcm = matrix;
	}

	void CRenderDevice::DrawLayer3DSolidQuad(RenderState ** pRS, CVector3f* vertex, CVector2f *uv, DWORD* color, ITexture* tex)
	{
		VerColorTex vs[4] = {
			VerColorTex( vertex[0].x, vertex[0].y, vertex[0].z, color[0], uv[0].x, uv[0].y),
			VerColorTex( vertex[1].x, vertex[1].y, vertex[1].z, color[1], uv[1].x, uv[1].y),
			VerColorTex( vertex[2].x, vertex[2].y, vertex[2].z, color[2], uv[2].x, uv[2].y),
			VerColorTex( vertex[3].x, vertex[3].y, vertex[3].z, color[3], uv[3].x, uv[3].y),
		};

		VerColorTex* pVB;

		CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, tex, NULL, PT_TRIANGLESTRIP, 4, 2, 
			VerColorTex::Format , sizeof(VerColorTex), (void**)&pVB, NULL, (void**)pRS )->Identity();

		memcpy(pVB,vs,sizeof(vs));
	}

	void CRenderDevice::DrawLayer3DSolidQuad(RenderState ** pRS, CVector3f* vertex, CVector2f *uv, DWORD* color, ITexture* tex0, ITexture* tex1)
	{
		VerColorTex vs[4] = {
			VerColorTex( vertex[0].x, vertex[0].y, vertex[0].z, color[0], uv[0].x, uv[0].y),
			VerColorTex( vertex[1].x, vertex[1].y, vertex[1].z, color[1], uv[1].x, uv[1].y),
			VerColorTex( vertex[2].x, vertex[2].y, vertex[2].z, color[2], uv[2].x, uv[2].y),
			VerColorTex( vertex[3].x, vertex[3].y, vertex[3].z, color[3], uv[3].x, uv[3].y),
		};

		VerColorTex* pVB;

		CGraphic::GetInst()->GetRenderStack( RS_GUI, tex0, tex1, PT_TRIANGLESTRIP, 4, 2, 
			VerColorTex::Format , sizeof(VerColorTex), (void**)&pVB, NULL, (void**)pRS )->Identity();

		memcpy(pVB,vs,sizeof(vs));
	}

	void CRenderDevice::DrawLayer3DSolidQuadTex2(RenderState ** pRS, CVector3f* vertex, CVector2f *uv0, CVector2f *uv1, DWORD* color, ITexture* tex0, ITexture* tex1)
	{
		Vertex2DAlpha vs[4] = {
			Vertex2DAlpha( vertex[0].x, vertex[0].y, vertex[0].z, color[0], uv0[0].x, uv0[0].y, uv1[0].x, uv1[0].y),
			Vertex2DAlpha( vertex[1].x, vertex[1].y, vertex[1].z, color[1], uv0[1].x, uv0[1].y, uv1[1].x, uv1[1].y),
			Vertex2DAlpha( vertex[2].x, vertex[2].y, vertex[2].z, color[2], uv0[2].x, uv0[2].y, uv1[2].x, uv1[2].y),
			Vertex2DAlpha( vertex[3].x, vertex[3].y, vertex[3].z, color[3], uv0[3].x, uv0[3].y, uv1[3].x, uv1[3].y),
		};

		Vertex2DAlpha* pVB;

		CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, tex0, tex1, PT_TRIANGLESTRIP, 4, 2, 
			Vertex2DAlpha::Format , sizeof(Vertex2DAlpha), (void**)&pVB, NULL, (void**)pRS )->Identity();

		memcpy(pVB,vs,sizeof(vs));
	}

	ITexture * CRenderDevice::GetTexture(string texname)
	{
		Ast(!texname.empty());

		static bool first = true;

		ITexture *temp = NULL;
		map<string, ITexture*>::iterator iter;

		iter = m_pTextureList.find(texname);
		if(iter == m_pTextureList.end())
		{
// 			if( m_TerrainTexIndexs.size() > 256  )
// 			{
// 				MessageBox(NULL, "载入纹理数已到达最大值", "", MB_OK);
// 				return NULL;
// 			}

			SQR_TRY
			{
				CPkgFile::EnableAsyncRead(false);
				locale loc = locale::global(locale(""));
				if( FAILED(CGraphic::GetInst()->CreateTexture(L"", texname.c_str(), &temp) ) )
				{
					MessageBox(NULL, texname.c_str(), "找不到", MB_OK);
				}
				locale::global(loc);
				CSyncSystem::BeginSyncFun();
			}
			SQR_CATCH(exp)
			{
				if( first )
				{
					first = false;

					MessageBox(0, exp.ErrorMsg(), "提示", 0);

					return NULL;
				}
			}
			SQR_TRY_END;

			if( temp != NULL )
			{
				first = true;
				m_pTextureList.insert( (make_pair(texname, temp) ) );

				++m_iTextureCount;
				return temp;
			}
			else
			{
				string log = texname;
				log += "创建失败";
				MessageBox(0, log.c_str(), "提示", 0);

				return NULL;
			}
		}
		else
			return iter->second;
	}

	bool CRenderDevice::DeleteTexture(string texname)
	{
		map<string, ITexture*>::iterator iter = m_pTextureList.find(texname);

		if( iter == m_pTextureList.end() )
			return true;
		else
		{
			ITexture* tex = NULL;
			tex = iter->second;
			if( tex )
			{
				// 从系统中彻底删掉
				CGraphic::GetInst()->DeleteTexture(texname.c_str());

				tex->Release();
				m_pTextureList.erase(iter);

				--m_iTextureCount;

				return true;
			}
			else
				return false;
		}
	}

	void CRenderDevice::ClearTextures()
	{
		map<string, ITexture*>::iterator iter = m_pTextureList.begin();

		for( iter; iter != m_pTextureList.end(); ++iter )
		{
			ITexture* tex = NULL;
			tex = iter->second;
			if( tex )
				SafeDelete(tex);
		}
		m_pTextureList.clear();
	}

	/*void CRenderDevice::RemoveSwapChain()
	{
		SafeRelease(m_pSwapChain);
	}

	void CRenderDevice::ReCreateSwapChain()
	{
		Ast(NULL == m_pSwapChain);
		RECT rect;
		::GetClientRect(m_hSwapChainWnd, &rect);
		this->AddSwapChain(rect.right, rect.bottom, m_hSwapChainWnd);
	}*/
}
