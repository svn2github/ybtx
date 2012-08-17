//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////
#ifndef _RENDER_H__
#define _RENDER_H__
#pragma once

#include "BluelyG.h"
#include "ICoreGraphic.h"
#include "ITexture.h"
#include "CGraphic.h"

namespace bg
{
namespace rd
{

struct SLayer2DFVF
{
	D3DXVECTOR3 pos;
	float		rhw;
	D3DCOLOR    color;
	D3DXVECTOR2 uv;
};

enum ELAYER2D_ELEMENT_TYPE
{
	EL2DET_LINES = 0,
	EL2DET_TRISTRIP,
	EL2DET_STRING
};

enum ELAYER3D_MULTISTREAM_TYPE 
{
	EL3DMST_POSCOL = 0,
	EL3DMST_POSCOLTEX0,
	EL3DMST_POSCOLTEX0TEX1,
	EL3DMST_POSNMLTEX0TEX1,
	EL3DMST_POSNMLCOLTEX0,
	EL3DMST_POSNMLCOLTEX0TEX1,
	EL3DMST_ENDOFTYPE
};

enum ELAYER3D_SHADER_TYPE
{
	// solid material with 2 texture layers. The second is blended onto the
	// first using the alpha value of the vertex.
	// used in our game for terrain transition 
	EL3DST_SOLID_2_LAYER = 0,
		
	// this is sooooooooooo common color modulated and alpha from texture
	EL3DST_SOLID_1_LAYER,
		
	
	EL3DST_WIREFRAME_0_LAYER,
		
	//// @ 
	//EL3DST_SOLID_ARKOBJECT
	
	EL3DST_CNT
};

// shader flags
enum ELAYER3D_SHADER_FLAG
{
	// Draw as wireframe or filled triangles? Default: false
	EL3DSF_WIREFRAME = 0,
		
	// Flat or Gouraud shading? Default: true
	EL3DSF_GOURAUD_SHADING,
		
	// Will this material be lighted? Default: true
	EL3DSF_LIGHTING,

	// Is the ZBuffer enabled? Default: true
	EL3DSF_ZBUFFER,

	// May be written to the zbuffer or is it readonly. Default: true
	// This flag is ignored, if the material type is a transparent type.
	EL3DSF_ZWRITE_ENABLE,

	// Is backfaceculling enabled? Default: true
	EL3DSF_BACK_FACE_CULLING,

	// Is bilinear filtering enabled? Default: true
	EL3DSF_BILINEAR_FILTER,
	
	// Is fog enabled? Default: false
	EL3DSF_FOG_ENABLE,



	// This is not a flag, but a value indicating how much flags there are.
	EL3DSF_FLAG_COUNT 
};

#define MAX_SHADER_ARRAY_SIZE				65536
#define MAX_TEXTURE_ARRAY_SIZE				2048
#define MAX_TEXTURE_LAYER					2
//#define FONT_TEXTURE_SIZE					512
#define MAX_LAYER2D_VERTEXBUFFER_SIZE		256
#define MAX_LAYER2D_CACHE_SIZE				1024
#define LAYER3D_VERTEXBUFFER_SIZE			8192

//////////////////////////////////////////////////////////////////////////

class CQuadXZ
{
	D3DXVECTOR3 m_vMin,m_vMax;

	/*float m_fxMin,m_fzMin;
	float m_fxMax,m_fzMax;*/

	D3DXVECTOR3 m_vCorner[4];

public:
	void	Set(float fxMin , float fxMax , float fzMin , float fzMax);
	inline 	const D3DXVECTOR3 * GetCorner() const { return m_vCorner;}
	
	void Get(D3DXVECTOR3 &vMin,D3DXVECTOR3 &vMax);
	
	CQuadXZ(void);
	~CQuadXZ(void);
};

//////////////////////////////////////////////////////////////////////////

class CBoundingBox
{
	D3DXVECTOR3 m_vCenter;
	float m_fOutsideRadius;

	D3DXVECTOR3 m_vMin,m_vMax;
	D3DXVECTOR3 m_vCorner[8];
	D3DXPLANE	m_plane[6];	

public:
	void operator = (const CBoundingBox &BoundingBox);
	void Set(const D3DXVECTOR3 & vMin, const D3DXVECTOR3 & vMax);
	void Set(const D3DXVECTOR3 &vCenter,float fRadius);

	bool TestPointInside(D3DXVECTOR3 * pvPoint);
	bool TestRadialIntersected(D3DXVECTOR3 &vOrigin,D3DXVECTOR3 &vDirection);
	bool TestLineSegmentIntersected(D3DXVECTOR3 vLineSegment[2],D3DXVECTOR3 &vIntersection);
	bool TestTriangleIntersected(D3DXVECTOR3 vTriangle[3]);
	bool TestSphereIntersected(D3DXVECTOR3 &vCenter,float fRadius);

	void _DEBUG_Render(const D3DXMATRIX * pMatWorld,DWORD dwColor);

	void Get(D3DXVECTOR3 &vMin,D3DXVECTOR3 &vMax);
	void GetCenter(D3DXVECTOR3 &vCenter) { vCenter = m_vCenter;}
	inline float GetOutsideRadius() { return m_fOutsideRadius;}
	inline const D3DXVECTOR3 * GetCornerVertex() const { return m_vCorner;}
};

//////////////////////////////////////////////////////////////////////////

class CFrustum
{
	D3DXVECTOR3 m_vFrustum[8];
	D3DXPLANE   m_planeFrustum[6];

public:
	void CalculateFrustum(const D3DXMATRIX *pView,const D3DXMATRIX *pProjection);
	bool BoxIntersectFrustum(const CBoundingBox * pBoundingBox);
	bool BoxIntersectFrustum(const D3DXMATRIX * pWorldMatrix,const CBoundingBox * pBoundingBox);
	bool BoxInFrustum(const CBoundingBox * pBoundingBox);
	bool PointInFrustum(const D3DXVECTOR3 * pPoint);
	bool QuadXZInFrustum(CQuadXZ * pQuadXZ);
	bool QuadXZIntersectFrustum(CQuadXZ * pQuadXZ);

	const D3DXVECTOR3 * GetCornerVertex() const { return m_vFrustum;}

	void _DEBUG_Render();
};	

//////////////////////////////////////////////////////////////////////////

class CTexture
{
	DWORD * m_pBitmap;
	DWORD   m_dwBitmapSize;

	// 16/32 bit texture
	LPDIRECT3DTEXTURE9 m_pD3DTexture;

	DWORD m_dwWidth,m_dwHeight;

	D3DFORMAT m_d3dFormat;

//	bool BitmapReadFilePCX(const char * pszFileName);
	bool BitmapReadFileBMP(const char * pszFilename);
	bool BitmapReadFileTGA(const char * pszFileName);
//	bool BitmapReadFilePSD(const char * pszFileName);
//	bool BitmapReadFilePPM(const char * pszFileName);
//	bool BitmapReadFileJPG(const char * pszFileName);

	bool CreateBitmap(DWORD dwWidth, DWORD dwHeight, DWORD dwColor = 0x00000000);
	void DestroyBitmap();
	void BitmapFlipVertical(void);

public:
	CTexture(void);
	~CTexture(void);

	bool Create(LPDIRECT3DDEVICE9 pDevice,const char * pszFileName,bool bUse32BitTex,DWORD dwMaxTextureWidth ,DWORD dwMaxTextureHeight);
	bool Create(LPDIRECT3DDEVICE9 pDevice,DWORD dwWidth,DWORD dwHeight,DWORD dwColor,bool bUse32BitTex,DWORD dwMaxTextureWidth,DWORD dwMaxTextureHeight);
	bool Create(LPDIRECT3DDEVICE9 pDevice,DWORD dwWidth,DWORD dwHeight,DWORD *pBitmap,bool bUse32BitTex,DWORD dwMaxTextureWidth,DWORD dwMaxTextureHeight);
	void Destroy();

	DWORD GetWidth()  const { return m_dwWidth;}
	DWORD GetHeight() const { return m_dwHeight;}
	DWORD * GetBitmap() const { return m_pBitmap;}

	const LPDIRECT3DTEXTURE9 GetD3DTexture() const { return m_pD3DTexture;}

	bool CreateD3DTexture(LPDIRECT3DDEVICE9 pDevice);
	void DestroyD3DTexture();

	bool CopyBitmapToTexture();
	DWORD GetPixel(int iX, int iY);
	void  SetPixel(int iX, int iY,DWORD dwColor);
};

//////////////////////////////////////////////////////////////////////////

class CRenderDevice // dx9 version
{
public:
	static CRenderDevice * GetInstance() { if ( !one ) one = new CRenderDevice; return one;} 

private:
	CRenderDevice();
	virtual ~CRenderDevice();
	static CRenderDevice * one;

public:
	DWORD GetWindowWidth();
	DWORD GetWindowHeight();
	void AddSwapChain(DWORD dwWidth, DWORD dwHeight, LPDIRECT3DSWAPCHAIN9 * ppSwapChain);

public:

	//////////////////////////////////////////////////////////////////////////
	
	class Layer2D
	{
		static const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	
		struct SLines
		{
			struct SVertex
			{
				D3DXVECTOR3 pos;
				float		rhw;
				D3DCOLOR    color;
				D3DXVECTOR2 reserved;
			} vertices[8];

			DWORD dwCount;				
		};
		
		struct STristrip
		{
			struct SVertex
			{
				D3DXVECTOR3 pos;
				float		rhw;
				D3DCOLOR    color;
				D3DXVECTOR2 uv;
			} vertices[4];
			DWORD dwTextureHandle;
		};

		struct SString
		{
			int iX,iY;
			float fScale;
			char  szString[240];
			DWORD dwColorArray[240];
		};

		struct SElement
		{
			ELAYER2D_ELEMENT_TYPE eType;
			DWORD pDataBuffer[512];
		};

		SElement * m_pElementCache;
		DWORD m_dwElementCacheUsage;

		LPDIRECT3DVERTEXBUFFER9 m_pVB;

		DWORD m_dwFillMode;

		bool CreateVertexBuffer();
		void DestroyVertexBuffer();

	public:

		Layer2D(void);
		~Layer2D(void);

		//////////////////////////////////////////////////////////////////////////
		// cache
		
		void DrawLine(int iStartX,int iStartY,int iEndX,int iEndY,D3DCOLOR color = 0xFFFFFFFF);
		
		void DrawSolidQuad(int iX,int iY,int iWidth,int iHeight,D3DCOLOR color = 0xFFFFFFFF);
		void DrawSolidQuadRect(int iX1,int iY1,int iX2,int iY2,D3DCOLOR color = 0xFFFFFFFF);

		void DrawWireQuad(int iX,int iY,int iWidth,int iHeight,D3DCOLOR color = 0xFFFFFFFF);
		void DrawWireQuadRect(int iX1,int iY1,int iX2,int iY2,D3DCOLOR color = 0xFFFFFFFF);

		void DrawTexture(int iX,int iY,DWORD dwTextureHandle,D3DCOLOR d3dColor = 0xFFFFFFFF);
		void DrawTexture(int iX,int iY,int iWidth,int iHeight,DWORD dwTextureHandle,D3DCOLOR color = 0xFFFFFFFF);
		void DrawTextureSubRect(int iX,int iY,const RECT *pRect,DWORD dwTextureHandle,D3DCOLOR color = 0xFFFFFFFF);
		void DrawTextureRotation(int iX,int iY,float fAngle,DWORD dwTextureHandle,D3DCOLOR color = 0xFFFFFFFF);

		
		void DrawTextureDirectly(int iX,int iY,int iWidth,int iHeight,DWORD dwTextureHandle,D3DCOLOR color = 0xFFFFFFFF);

		bool Create(const char * pszFontFileName);
		void Destroy(void);
		void Render(void);

		void SetFillMode(DWORD dwFillMode) { m_dwFillMode = dwFillMode;}
	};

	//////////////////////////////////////////////////////////////////////////
	
	class Layer3D
	{
	public:
		
		//////////////////////////////////////////////////////////////////////////
		
		struct SShader
		{
			ELAYER3D_SHADER_TYPE eShaderType;
			ELAYER3D_MULTISTREAM_TYPE eStreamType;
			
			// texture layer union.
			union
			{
				struct
				{
					// primary texture layer.
					int iTexHandle0;	

					// secondary texture layer
					int iTexHandle1;	
				};

				int iTexHandles[MAX_TEXTURE_LAYER];
			};

			// other setting
			union
			{
				struct
				{
					// Draw as wireframe or filled triangles? Default: false
					bool bWireframe;

					// Flat or Gouraud shading? Default: true
					bool bGouraudShading;				

					// Will this material be lighted? Default: true
					bool bLighting;						

					// Is the ZBuffer enabled? Default: true
					bool bZBuffer;						

					// May be written to the zbuffer or is it readonly. Default: true
					// This flag is ignored, if the MaterialType is a transparent type.
					bool bZWriteEnable;					

					// Is backfaceculling enabled? Default: true
					bool bBackfaceCulling;				

					// Is bilinear filtering enabled? Default: true
					bool bBilinearFilter;	

					// Is fog enabled? Default: false
					bool bFogEnable;
				};
				bool bFlags[EL3DSF_FLAG_COUNT];
			};

			SShader()
				: iTexHandle0(INVALID), iTexHandle1(INVALID),
				eShaderType(EL3DST_SOLID_2_LAYER), bWireframe(false), bLighting(true),
				bZBuffer(true), bZWriteEnable(true), bBackfaceCulling(true),
				bGouraudShading(true), bBilinearFilter(true),bFogEnable(false)
				{}

			~SShader(void) { }
		};

		//////////////////////////////////////////////////////////////////////////
		
	private:	
		SShader * m_pShaderArray[MAX_SHADER_ARRAY_SIZE];
		DWORD m_dwShaderCnt;
		DWORD m_dwShaderInUse;
		DWORD m_dwShaderLast;

		bool m_b2DState;

		LPDIRECT3DVERTEXBUFFER9 m_pVBpos;
		LPDIRECT3DVERTEXBUFFER9 m_pVBnormal;
		LPDIRECT3DVERTEXBUFFER9 m_pVBcolor;
		LPDIRECT3DVERTEXBUFFER9 m_pVBtex0;
		LPDIRECT3DVERTEXBUFFER9 m_pVBtex1;

		IDirect3DVertexDeclaration9 * m_pVertexDeclarationArray[EL3DMST_ENDOFTYPE];

		bool CreateVertexBuffer();
		void DestroyVertexBuffer();
		void CreateVertexDecl();
		void DestroyVertexDecl();

		void SetTexture();
		void SetStreamType();
		void SetRenderState();
		void SetRenderState2D();

		
		//////////////////////////////////////////////////////////////////////////
		// triangles

		// list
		void DrawTriangleListPCU2(DWORD dwCount, const D3DXVECTOR3 * pPos, const D3DCOLOR * pColor, const D3DXVECTOR2 * pUV0, const D3DXVECTOR2 * pUV1);
		void DrawTriangleListPCU1(DWORD dwCount,const D3DXVECTOR3 * pPos, const D3DCOLOR * pColor, const D3DXVECTOR2 * pUV0);
		void DrawTriangleListPNU2(DWORD dwCount,const D3DXVECTOR3 * pPos, const D3DXVECTOR3 * pNml, const D3DXVECTOR2 * pUV0, const D3DXVECTOR2 * pUV1);
		void DrawTriangleListPNCU1(DWORD dwCount,const D3DXVECTOR3 * pPos, const D3DXVECTOR3 * pNml, const D3DCOLOR * pColor, const D3DXVECTOR2 * pUV0);
		void DrawTriangleListPNCU2(DWORD dwCount,const D3DXVECTOR3 * pPos, const D3DXVECTOR3 * pNml, const D3DCOLOR * pColor, const D3DXVECTOR2 * pUV0, const D3DXVECTOR2 * pUV1);

		// strip
		void DrawTriangleStrip(DWORD dwCount,const D3DXVECTOR3 * pPos, const D3DCOLOR * pColor, const D3DXVECTOR2 * pUV0, bool b2D);

	public:
		// DWORD CreateShaderFromFile(string strFileName);

		DWORD CreateShader(int iTexHandle0, int iTexHandle1,
			ELAYER3D_SHADER_TYPE eShaderType, ELAYER3D_MULTISTREAM_TYPE eStreamType, bool bWireframe, bool bLighting,
			bool bZBuffer, bool bZWriteEnable, bool bBackfaceCulling,
			bool bGouraudShading, bool bBilinearFilter, bool bFogEnable);
		
		DWORD CreateDefaultShader();

		void SetShader(DWORD dwShaderHandle);
		
		// shared get
		SShader * GetShader(DWORD dwShaderHandle);

	public:
		Layer3D();
		~Layer3D();

		bool Create();
		void Destroy();

		void DrawTriangleList(DWORD dwCount, const D3DXVECTOR3 * pPos,const D3DXVECTOR3 * pNml, const D3DCOLOR * pColor, const D3DXVECTOR2 * pUV0,const D3DXVECTOR2 * pUV1);
		void DrawTriangleStrip(DWORD dwCount, const D3DXVECTOR3 * pPos,const D3DXVECTOR3 * pNml, const D3DCOLOR * pColor, const D3DXVECTOR2 * pUV0,const D3DXVECTOR2 * pUV1,bool b2D = false);
		void DrawLineStrip(DWORD dwCount, const D3DXVECTOR3 * pPos, const D3DCOLOR * pClr);

	};
	
	//////////////////////////////////////////////////////////////////////////
	

	// device info 
	struct SDeviceInfo
	{
		D3DADAPTER_IDENTIFIER9 AdapterIdentifier;
		D3DPRESENT_PARAMETERS  d3dpp;			 
		D3DCAPS9			   d3dCaps;			 			
		D3DDISPLAYMODE		   * pModeArray;	 
		int					   iModeCount;		 
	} m_deviceInfo;

	public:
	
	struct SSetting
	{
		char szFont[_MAX_PATH];

		bool   bHVProcess;
			
		
		bool bIsWindow;

				
		int iWndPosX,iWndPosY ; // window mode only
		int iWndHeight,iWndWidth;
		bool bWndAutoStyle;		
		bool bWndAutoCenter;

		// for full screen 
		D3DFORMAT d3dfmtBackBuffer;
	
		// D3DFORMAT d3dfmtStencil;

		int  iTextureMode;

		DWORD dwPresentationInterval;

	} m_setting;

private:
	//sqr::ICoreGraphic *m_pCoreGraphic;
	HWND m_hWnd;
	RECT m_rc;
	LPDIRECT3D9		   m_pD3D;			
	LPDIRECT3DDEVICE9  m_pD3DDevice; 
	
	// texture cache
	CTexture * m_pTextureArray[MAX_TEXTURE_ARRAY_SIZE];
	
	DWORD m_dwTextureCnt;
	
	Layer2D * m_pLayer2D;
	
	Layer3D * m_pLayer3D;

	void DefaultSetting(const char * pszConfigString);

	bool CreateDevice(void);
	void DestroyDevice(void);

	bool CreateDirect3D9(void);
	void DestroyDirect3D9(void);

	bool CheckHardware(void);
	void SetDefaultRenderState();

	bool CreateLayer();
	void DestroyLayer(void);
   
public:
	bool Create(HWND hWnd, RECT rc, const char * pszConfigFile);
	void Destroy();

	LPDIRECT3DDEVICE9 GetRawDevice(void) { return m_pD3DDevice;}

	bool BeginScene();
	void EndScene();
	void Render();
	void Present();
	void Present(HWND hWnd, LPDIRECT3DSWAPCHAIN9 pSwapChain);

	void Clear(D3DCOLOR d3dColor);

	DWORD  CreateTexture(const char * pszFileName,bool bUse32Tex = TRUE);
	DWORD  CreateTexture(DWORD dwWidth,DWORD dwHeight,DWORD *pBitmap,bool bUse32Tex = TRUE);
	DWORD  CreateTexture(DWORD dwWidth,DWORD dwHeight,DWORD dwColor,bool bUse32Tex = TRUE);
	void   DestroyTexture(DWORD dwTextureIndex);
	void   SetTexture(DWORD dwStage,DWORD dwTextureHandle);
	CTexture * GetTexture(DWORD dwTextureHandle);

	int  GetWndWidth()  const { return m_setting.iWndWidth;}
	int  GetWndHeight() const { return m_setting.iWndHeight;}
	HWND GetWndHandle() const { return m_hWnd;}

	void Cull(bool bEnable);
	
	Layer2D * GetLayer2D() { return m_pLayer2D;}
	
	Layer3D * GetLayer3D() { return m_pLayer3D;}
	
	void Dump(void);

	// void ModifySetting(const char * pszConfigString);		
};

}
}

#endif