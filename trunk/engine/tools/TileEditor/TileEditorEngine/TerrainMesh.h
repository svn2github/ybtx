//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "BluelyG.h"
#include "Scene.h"
#include "Render.h"
#include "CTileEditorModel.h"
#include "Face.h"

namespace bg
{
namespace sc
{

class CQTNode;
class CVisibleSet;

class CTerrainMesh : public ISceneNode
{
public:
	int InWhichRegion(int nGridIndex)
	{
		int nX = ( nGridIndex % this->GetWidth() ) / REGION_SPACE;
		int nY = ( nGridIndex / this->GetWidth() ) / REGION_SPACE;
		return nY * this->GetWidthInRegion() + nX;
	}
	
	struct STextureNodeInfo
	{
		STextureNodeInfo() 
		{
			dwTextureHandles[0] = dwTextureHandles[1] = dwTextureHandles[2] = INVALID;
		}
		
		DWORD dwTextureHandles[3];
		DWORD dwGridIndex;
	};
	
	struct SVertex
	{
		DWORD       dwVertexIndex;
		D3DXVECTOR3  vPosition;
		D3DXVECTOR3  vNormal;
		D3DCOLOR	 cColor;
		DWORD       dwUsedCnt;
		DWORD       dwGridsBelongto[4];
	};
	
	class CBrain : public CRootObject
	{
	};
	
	// warning : 
	class CObject : public CRootObject
	{
		CFace * m_pFace;
		bool m_bServerBrainBound;
		bool m_bClientBrainBound;
		int m_nCenterGridIndex; // the grid the  object belongs to...
		
	public:
		CObject() : m_nCenterGridIndex(INVALID), m_pFace(NULL), m_bServerBrainBound(false), m_bClientBrainBound(false)
		{
		
		}
		
		~CObject() 
		{
//			LogConsole( this->GetFace()->GetSelfPresentation()->GetName().c_str() << "are being deleted");
			SAFE_DELETE(m_pFace); 

		}
		
		void SetCenterGridIndex(int n) 
		{ 
			m_nCenterGridIndex = n;
			m_pFace->SetCenterGridIndex(n);
		}
		
		int GetCenterGridIndex() { return m_nCenterGridIndex; }
		
		void SetFace(CFace * p) { m_pFace = p; }
		CFace * GetFace() { return m_pFace; }
		
		void SetIsServerBrainBound(bool b) { m_bServerBrainBound = b; }
		bool GetIsServerBrainBound() { return m_bServerBrainBound; }
		
		void SetIsClientBrainBound(bool b) { m_bClientBrainBound = b; }
		bool GetIsClientBrainBound() { return m_bClientBrainBound; }
	};
	
	struct SGrid
	{
		DWORD	dwGridIndex;
		DWORD	dwVertexIndex[4];
		D3DXVECTOR2  uv[2][4];
		std::vector< Ptr<CObject> > vecObjects;
		STextureNodeInfo nodeInfo;
		unsigned char byBlockFlag; // 0 none 1 low 2 mid 3 high
		D3DXVECTOR3 vCenter;
		std::vector< std::pair<float,CObject*> > vecLogicHeights;
		std::vector< Ptr<CObject> > vecCenterObjects; // is this the center gird(in Model way)?
	};
	
	struct SRegion
	{
		std::vector<int> vVertexIndices;
		std::vector<int> vGridIndices;
	};
	
	//////////////////////////////////////////////////////////////////////////
	
	class CTextureNode
	{
	public:
		enum ETRANS_TYPE { ETT_VERTEX = 0, ETT_TEXTURE, ETT_NONE };		

	private:
		DWORD m_dwTextureHandle;
		std::list< CTextureNode* > m_children;
		std::vector<DWORD> m_vGridIndices;
		DWORD m_dwGridCnt;
		DWORD m_dwTextureLayer;
		
		void AddGrid(DWORD dw)
		{
			m_vGridIndices.push_back(dw);
			m_dwGridCnt++;
		}

	public:
		CTextureNode() : m_dwGridCnt(0),m_dwTextureHandle(INVALID),m_dwTextureLayer(INVALID) { }
		~CTextureNode() { }

		DWORD GetGridIndex(DWORD dw){ return m_vGridIndices[dw]; }
		
		DWORD GetGridCnt() { return m_dwGridCnt; }

		void SetTextureLayer(DWORD dw) { m_dwTextureLayer = dw; } 
		DWORD GetTextureLayer() { return m_dwTextureLayer; } 

		DWORD GetTextureHandle() { return m_dwTextureHandle; }
		void SetTextureHandle(DWORD dw) { m_dwTextureHandle = dw; }

		bool Insert(STextureNodeInfo & nodeInfo)
		{
			static int iLevel = -1;
			
			// for the root
			if ( m_dwTextureHandle == INVALID )
			{ 
				// the root ( no texture...so pass to children )
				
				iLevel++;
				
				std::list<CTextureNode*>::iterator it = m_children.begin();

				for (; it != m_children.end(); ++it)
				{
					if ( (*it)->Insert(nodeInfo) )
					{ 
						return true;
					}
				}

				// create new node and attach to it
				CTextureNode * p = new CTextureNode;
				p->AddGrid(nodeInfo.dwGridIndex);
				p->SetTextureHandle(nodeInfo.dwTextureHandles[iLevel]);
				p->SetTextureLayer(iLevel);
				m_children.push_back(p);
				iLevel = -1;
				return true;
			}
			
			if ( nodeInfo.dwTextureHandles[iLevel] == m_dwTextureHandle )
			{
				if ( nodeInfo.dwTextureHandles[iLevel+1] == INVALID )
				{
					// not added yet 
					AddGrid(nodeInfo.dwGridIndex);
					iLevel = -1;
					return true; // then break higher loop
				}
				else
				{
					iLevel++;

					std::list<CTextureNode*>::iterator it = m_children.begin();
					
					for (; it != m_children.end(); ++it)
					{
						if ( (*it)->Insert(nodeInfo) )
						{ 
							return true;
						}
					}

					// create new node and attach to it
					CTextureNode * p = new CTextureNode;
					p->AddGrid(nodeInfo.dwGridIndex);
					p->SetTextureHandle(nodeInfo.dwTextureHandles[iLevel]);
					p->SetTextureLayer(iLevel);
					m_children.push_back(p);
					iLevel = -1;
					return true;
				}
			}
			else
			{
				return false;
			}
		}

		void Render(DWORD dwShaderHandle, SGrid * pGrids, SVertex * pVertices)
		{
			rd::CRenderDevice * pRender = rd::CRenderDevice::GetInstance();
			LPDIRECT3DDEVICE9 pDevice = pRender->GetRawDevice();
			rd::CRenderDevice::Layer3D * pLayer3d = pRender->GetLayer3D();
			
          	DWORD dwTextureLayer = this->GetTextureLayer();
			DWORD dwTextureHandle = this->GetTextureHandle();
			
			if ( dwTextureHandle != INVALID )
			{
				if ( dwTextureLayer == 0 )
				{
					pLayer3d->GetShader(dwShaderHandle)->eShaderType = rd::EL3DST_SOLID_1_LAYER;
					pLayer3d->GetShader(dwShaderHandle)->eStreamType = rd::EL3DMST_POSNMLCOLTEX0;
				}
				
				if ( dwTextureLayer == 1 )
				{
					pLayer3d->GetShader(dwShaderHandle)->eShaderType = rd::EL3DST_SOLID_2_LAYER;
					pLayer3d->GetShader(dwShaderHandle)->eStreamType = rd::EL3DMST_POSNMLCOLTEX0TEX1;
				}
				
				//////////////////////////////////////////////////////////////////////////
				
				pLayer3d->GetShader(dwShaderHandle)->iTexHandles[dwTextureLayer] = dwTextureHandle;
				
				if ( dwTextureLayer == 0 )
					pLayer3d->GetShader(dwShaderHandle)->iTexHandles[dwTextureLayer+1] = INVALID;

				//////////////////////////////////////////////////////////////////////////
				
				
				//////////////////////////////////////////////////////////////////////////
				
				//if ( dwTextureLayer == 0 )
				//{
				//	pLayer3d->GetShader(dwShaderHandle)->iTexHandles[dwTextureLayer] = dwTextureHandle;
				//	pLayer3d->GetShader(dwShaderHandle)->iTexHandles[dwTextureLayer+1] = INVALID;
				//}

				//if ( dwTextureLayer == 1 )
				//{
				//	int n = pLayer3d->GetShader(dwShaderHandle)->iTexHandles[0];
				//	pLayer3d->GetShader(dwShaderHandle)->iTexHandles[0] = dwTextureHandle;
				//	pLayer3d->GetShader(dwShaderHandle)->iTexHandles[1] = n;
				//}

				//////////////////////////////////////////////////////////////////////////
												
				static D3DXVECTOR3 vertex[VERTEX_BATCH_SIZE];
				static D3DXVECTOR2 uv0[VERTEX_BATCH_SIZE];
				static D3DXVECTOR2 uv1[VERTEX_BATCH_SIZE];
				static DWORD color[VERTEX_BATCH_SIZE];
				static D3DXVECTOR3 nml[VERTEX_BATCH_SIZE];
				
				DWORD cnt = this->GetGridCnt();

				DWORD drawnCnt = 0;
				for ( DWORD n = 0, drawnCnt = 0; n < cnt; n++, drawnCnt++ )
				{	
					if ( drawnCnt && !((drawnCnt) % GRID_BATCH_SIZE) )
					{	
						pLayer3d->DrawTriangleList(TRIANGLE_BATCH_SIZE, vertex, nml, color, uv0 ,uv1);
						drawnCnt = 0;
					}
									
					DWORD index = m_vGridIndices[n];
					
					SGrid & grid = pGrids[index];
					
					vertex[6*drawnCnt+0] = pVertices[grid.dwVertexIndex[0]].vPosition;
					vertex[6*drawnCnt+1] = pVertices[grid.dwVertexIndex[3]].vPosition;
					vertex[6*drawnCnt+2] = pVertices[grid.dwVertexIndex[1]].vPosition;
					
					nml[6*drawnCnt+0] = pVertices[grid.dwVertexIndex[0]].vNormal;
					nml[6*drawnCnt+1] = pVertices[grid.dwVertexIndex[3]].vNormal;
					nml[6*drawnCnt+2] = pVertices[grid.dwVertexIndex[1]].vNormal;
					
					color[6*drawnCnt+0] = pVertices[grid.dwVertexIndex[0]].cColor;
					color[6*drawnCnt+1] = pVertices[grid.dwVertexIndex[3]].cColor;
					color[6*drawnCnt+2] = pVertices[grid.dwVertexIndex[1]].cColor;
									
					uv0[6*drawnCnt+0] = grid.uv[0][0];
					uv0[6*drawnCnt+1] = grid.uv[0][3];
					uv0[6*drawnCnt+2] = grid.uv[0][1];
					
					if ( dwTextureLayer == 1 )
					{
						uv1[6*drawnCnt+0] = grid.uv[1][0];
						uv1[6*drawnCnt+1] = grid.uv[1][3];
						uv1[6*drawnCnt+2] = grid.uv[1][1];
					}
					
					vertex[6*drawnCnt+3] = pVertices[grid.dwVertexIndex[3]].vPosition;
					vertex[6*drawnCnt+4] = pVertices[grid.dwVertexIndex[2]].vPosition;
					vertex[6*drawnCnt+5] = pVertices[grid.dwVertexIndex[1]].vPosition;
					
					nml[6*drawnCnt+3] = pVertices[grid.dwVertexIndex[3]].vNormal;
					nml[6*drawnCnt+4] = pVertices[grid.dwVertexIndex[2]].vNormal;
					nml[6*drawnCnt+5] = pVertices[grid.dwVertexIndex[1]].vNormal;
					
					color[6*drawnCnt+3] = pVertices[grid.dwVertexIndex[3]].cColor;
					color[6*drawnCnt+4] = pVertices[grid.dwVertexIndex[2]].cColor;
					color[6*drawnCnt+5] = pVertices[grid.dwVertexIndex[1]].cColor;

					uv0[6*drawnCnt+3] = grid.uv[0][3];
					uv0[6*drawnCnt+4] = grid.uv[0][2];
					uv0[6*drawnCnt+5] = grid.uv[0][1];

					if ( dwTextureLayer == 1 )
					{
						uv1[6*drawnCnt+3] = grid.uv[1][3];
						uv1[6*drawnCnt+4] = grid.uv[1][2];
						uv1[6*drawnCnt+5] = grid.uv[1][1];
					}
				}

				pLayer3d->DrawTriangleList( (drawnCnt)*2, vertex, nml, color, uv0, uv1);
			}

			//////////////////////////////////////////////////////////////////////////
			
			//// restore the shader order !!! once a big bug
			//if ( dwTextureLayer == 1 )
			//{
			//	int n = pLayer3d->GetShader(dwShaderHandle)->iTexHandles[1];
			//	pLayer3d->GetShader(dwShaderHandle)->iTexHandles[0] = n;
			//	pLayer3d->GetShader(dwShaderHandle)->iTexHandles[1] = dwTextureHandle;
			//}

			//////////////////////////////////////////////////////////////////////////
						
			std::list<CTextureNode*>::iterator it = m_children.begin();

			for (; it != m_children.end(); ++it)
			{
				(*it)->Render(dwShaderHandle, pGrids, pVertices);
			}
		}

		void Clear()
		{
			m_vGridIndices.clear();
			m_dwGridCnt = 0;
			std::list<CTextureNode*>::iterator it = m_children.begin();

			for (; it != m_children.end(); ++it)
			{
				(*it)->Clear();
			}
		}

	};

	//////////////////////////////////////////////////////////////////////////
	
	static void SetClassView(HWND hwnd) 
	{
		m_hClassView = hwnd;
	}

private:
	static HWND m_hClassView;
	float m_fHeight;
	float m_fAlpha;
	
	CQTNode * m_pQTRoot;
	CVisibleSet * m_pVisibleSet;
	
	SVertex * m_pVertices;	
	DWORD     m_dwVertexCnt;
	
	SGrid * m_pGrids;            
	DWORD   m_dwGridCnt;         
	
	DWORD   m_dwWidth;          
	DWORD   m_dwDepth;

	DWORD m_dwWidthInRegion;
	DWORD m_dwDepthInRegion;

	SRegion * m_pRegions;
	DWORD m_dwRegionCnt;
	
	DWORD   m_dwTextureHandleDefault;
	
	CTextureNode * m_pTTRoot; // texture tree root

public:
	CTerrainMesh(ISceneNode * pParent);
	
	~CTerrainMesh();
	
	CVisibleSet * GetVisibleSet() { return m_pVisibleSet; }
	
	bool Create(DWORD dwWidth, DWORD dwDepth, DWORD dwTextureHandleDefault);
		
	void Render();
	void SubRenderObjects();
	void Update(ip::CInputDevice::SEvent & event);
	
	void ResetVS();
	void RebuildRenderTree();

	void ReCalculateGridsHeight(DWORD dwVertexIndex);
	
	DWORD GetWidth() 
	{ 
		return m_dwWidth; 
	}
	DWORD GetDepth() { return m_dwDepth; }

	DWORD GetWidthInRegion() { return m_dwWidthInRegion; }
	DWORD GetDepthInRegion() { return m_dwDepthInRegion; }

	int GetRegionIndexByGridIndex(int nGridIndex);
	
	DWORD GetRegionCount() { return m_dwRegionCnt; }

	DWORD GetGridCount() { return m_dwWidth*m_dwDepth;}
	SGrid & GetGrid(DWORD dwGridIndex);
	SVertex & GetVertex(DWORD dwVertexIndex);

	SGrid * GetGrids()	{ return m_pGrids; }
	SVertex * GetVertices()	{ return m_pVertices; }

	SRegion & GetRegion(int nRegionIndex); 

	DWORD GetVertexCount() { return m_dwVertexCnt; }
	SVertex & GetVertexHot();
	SGrid & GetGridHot();
	DWORD GetVertexIndexHot();
	void SetWireFrame(bool b);
	void SetTextureHandleDefault(DWORD dwTextureHandleDefault);
	bool IsValid(D3DXVECTOR3 v);
	bool IsValid(int nGridIndex);

	void SetHeightStrength(float f)
	{
		m_fHeight = f;
	}

	void SetAlphaStrength(float f)
	{
		m_fAlpha = f;
	}
	
	//////////////////////////////////////////////////////////////////////////
	// texture manipulation
	
	// do & undo
	void SetGridTextureHandle(DWORD dwLayer, DWORD dwGridIndex, DWORD dwTextureHandle);
	
	//////////////////////////////////////////////////////////////////////////
	// alpha manipulataion
	
	// do
	void CutVertexAlpha(DWORD dwVertexIndex);
	void IncLinearVertexAlphaX(DWORD dwVertexIndex);
	void IncLinearVertexAlphaZ(DWORD dwVertexIndex);
	void IncLinearVertexAlphaB(DWORD dwVertexIndex, DWORD dwVertexIndexHot);
	
	void DecLinearVertexAlphaX(DWORD dwVertexIndex);
	void DecLinearVertexAlphaZ(DWORD dwVertexIndex);
	void DecLinearVertexAlphaB(DWORD dwVertexIndex);
	
	// undo
	void SetVertexAlpha(DWORD dwVertexIndex, D3DCOLOR cColor);
	
	//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////
	// height manipulataion
	
	// do
	void CutVertexHeight(DWORD dwVertexIndex);
	void IncLinearVertexHeightX(DWORD dwVertexIndex, DWORD dwGridIndexHot);
	void IncLinearVertexHeightZ(DWORD dwVertexIndex,  DWORD dwGridIndexHot);
	void IncLinearVertexHeightB(DWORD dwVertexIndex,  DWORD dwGridIndexHot);
	void IncGaussVertexHeight(DWORD dwVertexIndex, DWORD dwBrushWidth, DWORD dwBrushDepth, DWORD dwGridIndexHot);
	
	void DecLinearVertexHeightX(DWORD dwVertexIndex,  DWORD dwGridIndexHot);
	void DecLinearVertexHeightZ(DWORD dwVertexIndex,  DWORD dwGridIndexHot);
	void DecLinearVertexHeightB(DWORD dwVertexIndex,  DWORD dwGridIndexHot);
	void DecGaussVertexHeight(DWORD dwVertexIndex, DWORD dwBrushWidth, DWORD dwBrushDepth,  DWORD dwGridIndexHot);
	
	// undo
	void SetVertexHeight(DWORD dwVertexIndex, float fHeight);
	
	//////////////////////////////////////////////////////////////////////////
	// light effector
	
	void LightMe(DWORD dwVertexIndex);

	//////////////////////////////////////////////////////////////////////////
	
	// save/load
	void SaveAsLocal(std::string strFileName);
	void LoadAsLocal(std::string strFileName);

	void SaveAsClient(std::string strFileName);
	void SaveAsServer(std::string strFileName);


	int MapVertexIndexToRegion(int nVertexIndex, int nRegionIndex);
};

}
}