//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#include "SkyDome.h"
#include "Render.h"

#if defined(_DEBUG) && defined(_WIN32)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

namespace bg
{
namespace sc
{

CSkyDome::CSkyDome(ISceneNode * pParent) : ISceneNode(pParent),m_pPosBuffer(NULL),m_pUVBuffer(NULL),m_pColBuffer(NULL) { }

CSkyDome::~CSkyDome()
{
	SAFE_DELETE_ARRAY(m_pPosBuffer);
	SAFE_DELETE_ARRAY(m_pUVBuffer);
	SAFE_DELETE_ARRAY(m_pColBuffer);
}

void CSkyDome::Create(DWORD dwTextureHandle,int iDivisions, float fPlanetRadius, float fAtmosphereRadius, float fHTile, float fVTile, float fOffset)
{
	m_fOffset = fOffset;
	
	// set the number of divisions into a valid range
	int divs = iDivisions;
	if ( iDivisions < 1) 
		divs = 1;

	if ( iDivisions > 256) 
		divs = 256; 

	m_fRadius = fPlanetRadius;

	// initialize the Vertex and indices arrays
	m_dwPlaneVertexCnt = (divs + 1) * (divs + 1);   // 1 division would give 4 verts
	m_dwIndexCnt  = divs * divs * 2 * 3;       // 1 division would give 6 indices for 2 tris

	m_pPosBuffer = new D3DXVECTOR3[m_dwPlaneVertexCnt];
	m_pUVBuffer = new D3DXVECTOR2[m_dwPlaneVertexCnt];
	m_pColBuffer = new D3DCOLOR[m_dwPlaneVertexCnt];

	m_pdwIndices = new DWORD[m_dwIndexCnt];
	ZeroMemory(m_pdwIndices, sizeof(WORD)*m_dwIndexCnt);

	// calculate some values
	float plane_size = 2.0f * (float)sqrtf(fAtmosphereRadius*fAtmosphereRadius - fPlanetRadius*fPlanetRadius);
	float delta = plane_size/(float)divs;
	float tex_delta = 2.0f/(float)divs;

	// variables we'll use during the dome's generation
	float x_dist   = 0.0f;
	float z_dist   = 0.0f;
	float x_height = 0.0f;
	float z_height = 0.0f;
	float height = 0.0f;

	int count = 0;

	D3DXVECTOR3 pos;
	D3DXVECTOR2 uv;
	D3DCOLOR    col;

	for ( int i = 0; i <= divs;i++ )
	{
		for ( int j = 0; j <= divs; j++ )
		{
			x_dist = (-0.5f * plane_size) + ((float)j*delta);
			z_dist = (-0.5f * plane_size) + ((float)i*delta);

			x_height = (x_dist*x_dist) / fAtmosphereRadius;
			z_height = (z_dist*z_dist) / fAtmosphereRadius;
			height = x_height + z_height;

			pos.x = x_dist;
			pos.y = fPlanetRadius - height;
			pos.z = z_dist;

			// calculate the texture coordinates
			uv.x = fHTile*((float)j * tex_delta*0.5f);
			uv.y = fVTile*(1.0f - (float)i * tex_delta*0.5f);
			col = 0xffffffff;
			m_pPosBuffer[i*(divs+1)+j] = pos;
			m_pUVBuffer[i*(divs+1)+j] = uv;
			m_pColBuffer[i*(divs+1)+j] = col;
		}
	}
	
	// calculate the indices
	int index = 0;
	for ( int i = 0; i < divs; i++)
	{
		for ( int j = 0; j < divs; j++ )
		{
			int startvert = (i*(divs+1) + j);

			// tri 1
			m_pdwIndices[index++] = startvert;
			m_pdwIndices[index++] = startvert+1;
			m_pdwIndices[index++] = startvert+divs+1;

			// tri 2
			m_pdwIndices[index++] = startvert+1;
			m_pdwIndices[index++] = startvert+divs+2;
			m_pdwIndices[index++] = startvert+divs+1;
		}
	}

	rd::CRenderDevice * pRender = rd::CRenderDevice::GetInstance();
	rd::CRenderDevice::Layer3D * pLayer3d = pRender->GetLayer3D();

	// create shader here...
	m_dwShaderHandle = pLayer3d->CreateShader(dwTextureHandle,INVALID,rd::EL3DST_SOLID_1_LAYER,rd::EL3DMST_POSCOLTEX0,false,false,true,true,false,true,false,false);
}

void CSkyDome::Render()
{
	rd::CRenderDevice * pRender = rd::CRenderDevice::GetInstance();
	rd::CRenderDevice::Layer3D * pLayer3d = pRender->GetLayer3D();

	// self specific transform
	pRender->GetRawDevice()->SetTransform(D3DTS_WORLD,&this->GetAbsolute());

	pLayer3d->SetShader(m_dwShaderHandle);

	pRender->GetRawDevice()->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

	D3DXVECTOR3 pos[3];
	D3DXVECTOR2 uv[3];
	D3DCOLOR color[3];

	for ( int i = 0; i < m_dwIndexCnt/3 ; i++ )
	{
		uv[0] = m_pUVBuffer[m_pdwIndices[i * 3]];
		uv[1] = m_pUVBuffer[m_pdwIndices[i * 3 + 1]];
		uv[2] = m_pUVBuffer[m_pdwIndices[i * 3 + 2]];


		pos[0] = m_pPosBuffer[m_pdwIndices[i * 3]];
		pos[1] = m_pPosBuffer[m_pdwIndices[i * 3 + 1]];
		pos[2] = m_pPosBuffer[m_pdwIndices[i * 3 + 2]];

		color[0] = m_pColBuffer[m_pdwIndices[i * 3]];
		color[1] = m_pColBuffer[m_pdwIndices[i * 3 + 1]];
		color[2] = m_pColBuffer[m_pdwIndices[i * 3 + 2]];

		pLayer3d->DrawTriangleList(1,pos,NULL,color,uv,NULL);
	}
	return;
}

void CSkyDome::Update(ip::CInputDevice::SEvent & event)
{
	for ( DWORD n = 0; n < m_dwPlaneVertexCnt; n++)
	{
		m_pUVBuffer[n].x += m_fOffset;
	}
	ISceneNode::Update(event);
}

}
}