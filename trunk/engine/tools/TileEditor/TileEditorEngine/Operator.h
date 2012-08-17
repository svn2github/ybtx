//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "BluelyG.h"
#include "Scene.h"
#include "Input.h"
#include "ToolSetModel.h"
#include "ToolSetBlockType.h"
#include "ToolSetBlockLogicHeight.h"
#include "UserMsg.h"
#include "TerrainMesh.h"
#include "../TileEditor/Set.h"

namespace sqr
{

	class ICoreGraphic;
	class IGraphicModule;
	class IEffectMgrModule;
	class IEffectManager;
	class IRenderObj;
}

namespace bg
{

	namespace ui
	{
		class CCheckBox;
	}

	namespace sc
	{

		class COperator : public ISceneNode
		{
		public:
			enum ESET_ALPHA_TYPE { ESAT_LINEAR = 0, ESAT_CNT};

			enum ESET_HEIGHT_TYPE { ESHT_LINEAR = 0, ESHT_GAUSS, ESHT_CNT};

			enum EAXIS_TYPE { EAT_X = 0, EAT_Z, EAT_BOTH, EAT_CNT};

			enum ESET_WAY_TYPE { ESWT_INC = 0, ESWT_DEC, ESWT_CUT, ESWT_CNT};

			enum EBRUSH_TYPE { EBT_POINT = 0, EBT_GRID, EBT_BLOCK, EBT_NONE, EBT_CNT };

			struct SProcessParams           
			{
				SProcessParams()
				{
					dwGridIndexHot = 0;
					dwVertexIndexHot = INVALID;
					dwTextureHandleHot = INVALID;
					bBrush = true;
					dwBrushSize = 0;
					dwLayerHot = INVALID;
					eSetAlphaType = ESAT_LINEAR;
					eSetHeightType = ESHT_GAUSS;
					eSetWayType = ESWT_INC;
					eSetAxisType = EAT_BOTH;
					eBrushType = EBT_NONE;
					bSelectMode = false;
				}
				DWORD dwBrushSize;
				DWORD dwTextureHandleHot;
				DWORD dwGridIndexHot;
				DWORD dwLayerHot;
				bool  bBrush;
				bool  bSelectMode;
				EBRUSH_TYPE eBrushType;
				ESET_ALPHA_TYPE eSetAlphaType;
				ESET_HEIGHT_TYPE eSetHeightType;
				EAXIS_TYPE eSetAxisType;
				ESET_WAY_TYPE eSetWayType;
				Ptr<CTerrainMesh::CObject> pObjectHot;
				DWORD dwVertexIndexHot;
			};

			//enum EEDIT_STATE { EES_SET_BLOCK_MASK = 0, EES_SET_BLOCK_TYPE, EES_SET_BLOCK_LOGIC_HEIGHT, EES_NONE, EES_CNT};

			static IGraphicModule * ms_pGraphicModule;
			static IEffectMgrModule * ms_pEffectMgrModule;

		private:

			ICoreGraphic * m_pCoreGraphic;
			IEffectManager * m_pEffectManager; 

			SProcessParams m_processParams;

			//EEDIT_STATE m_eEditState;

			HWND m_hResSetView;

			Ptr< CTerrainMesh::CObject > m_pObject;

			IEffect * m_pEffect;

			CToolSetModel m_toolSetModel; // historical alias for ToolSetBlockMask
			CToolSetBlockType m_toolSetBlockType;
			CToolSetBlockLogicHeight m_toolSetBlockLogicHeight;

			//////////////////////////////////////////////////////////////////////////
			// UI
			ui::CCheckBox * m_pCheckBoxTextureLayerOne;
			ui::CCheckBox * m_pCheckBoxTextureLayerTwo;

			//////////////////////////////////////////////////////////////////////////
			D3DXVECTOR3 m_pHitVec;

		public:
			COperator(ISceneNode * pParent);
			~COperator();
			void OnEvent(ip::CInputDevice::SEvent & event);
			void Render();
			void Create(HWND hResSetView);
			void Update(ip::CInputDevice::SEvent & event);

			HWND GetResSetView() { return m_hResSetView; }

			DWORD GetTextureHandleHot() { return m_processParams.dwTextureHandleHot; }

			void SetTextureHandleHot(DWORD dw)
			{
				m_processParams.dwTextureHandleHot = dw;

				if ( this->GetLayerHot() == 0 )
				{
					ResetCheckBoxTextureLayerOne(dw);
				}

				if ( this->GetLayerHot() == 1 )
				{
					ResetCheckBoxTextureLayerTwo(dw);
				}
			}

			void ResetCheckBoxTextureLayerOne(int nTextureHandle);

			void ResetCheckBoxTextureLayerTwo(int nTextureHandle);

			bool GetDrawBrush() 
			{ 
				return m_processParams.bBrush; 
			}
			void SetDrawBrush(bool b)
			{ 
				m_processParams.bBrush = b; 
			}

			bool GetSelectMode() { return m_processParams.bSelectMode; }
			void SetSelectMode(bool b) { m_processParams.bSelectMode = b; }

			void SetBrushType(EBRUSH_TYPE e) { m_processParams.eBrushType = e; }
			EBRUSH_TYPE GetBrushType() { return m_processParams.eBrushType;}

			void SetHeightType(ESET_HEIGHT_TYPE e) { m_processParams.eSetHeightType = e; }
			ESET_HEIGHT_TYPE GetHeightType() { return m_processParams.eSetHeightType;}
			int GetHeightTypeCount() { return ESHT_CNT;}

			void SetAxisType(EAXIS_TYPE e) { m_processParams.eSetAxisType = e; }
			EAXIS_TYPE GetAxisType() { return m_processParams.eSetAxisType;}
			int GetAxisTypeCount() { return EAT_CNT;}

			void SetWayType(ESET_WAY_TYPE e) { m_processParams.eSetWayType = e; }
			ESET_WAY_TYPE GetWayType() { return m_processParams.eSetWayType;}
			int GetWayTypeCount() { return ESWT_CNT;}

			void SetAlphaType(ESET_ALPHA_TYPE e) { m_processParams.eSetAlphaType = e; }
			ESET_ALPHA_TYPE GetAlphaType() { return m_processParams.eSetAlphaType;}
			int GetAlphaTypeCount() { return ESAT_CNT;}

			DWORD GetBrushSize() 
			{ 
				return m_processParams.dwBrushSize; 
			}
			void SetBrushSize(DWORD dw) 
			{ 
				m_processParams.dwBrushSize = dw;	
			}

			void IncBrushSize() { m_processParams.dwBrushSize++; }
			void DecBrushSize() { m_processParams.dwBrushSize--;}

			DWORD GetGridIndexHot() 
			{ 
				return m_processParams.dwGridIndexHot; 
			}
			void SetGridIndexHot(DWORD dw) 
			{ 
				m_processParams.dwGridIndexHot = dw; 
			}

			DWORD GetVertexIndexHot() { return m_processParams.dwVertexIndexHot; }
			void SetVertexIndexHot(DWORD dw) { m_processParams.dwVertexIndexHot = dw; }

			DWORD GetLayerHot() { return m_processParams.dwLayerHot; }
			void SetLayerHot(DWORD dw) 
			{ 
				m_processParams.dwLayerHot = dw;

			}

			//void SetEditState(EEDIT_STATE e)
			//{
			//	m_eEditState = e;

			//	/*if ( m_eEditState == EES_SET_BLOCK_MASK )
			//	{
			//		this->SetDrawBrush(true);
			//		if ( m_pObject )
			//		{
			//			m_pObject->GetFace()->SetHeightBoxesVisibility(false);
			//		}
			//	}*/

			//	if ( m_eEditState == EES_SET_BLOCK_TYPE )
			//	{
			//		this->SetDrawBrush(true);
			//		if ( m_pObject )
			//		{
			//			m_pObject->GetFace()->SetHeightBoxesVisibility(false);
			//		}
			//	}

			//	/*if ( m_eEditState == EES_SET_BLOCK_LOGIC_HEIGHT )
			//	{
			//		this->SetDrawBrush(true);
			//		if ( m_pObject )
			//		{
			//			m_pObject->GetFace()->SetHeightBoxesVisibility(true);
			//		}
			//	}*/
			//}

			/*EEDIT_STATE GetEditState() 
			{
				return m_eEditState;
			}

			int GetEditStateCount()
			{
				return EES_CNT;
			}*/

			void CreateActiveObject(std::vector<std::string> & veCTileEditorModelFileNames, std::vector< std::vector<CTileEditorModel::SBlockInfo> > & vvBlock, std::vector< std::vector<CTileEditorModel::SLogicHeightInfo> > & vvHeight, bool bSubUnit, CTileEditorModel::ECenter_Type eBlockType, float fLift, float fShiftX, float fShiftZ)
			{
				m_toolSetModel.CreateActiveObject(veCTileEditorModelFileNames, vvBlock, vvHeight, bSubUnit, eBlockType, fLift, fShiftX, fShiftZ);
			}

			void SetCenterType(CTileEditorModel::ECenter_Type e)
			{
				m_toolSetModel.AdjustActiveObjectCenterType(e);
			}

			CTerrainMesh::CObject * GetObjectHot() 
			{
				return m_processParams.pObjectHot;
			}

			void SetObjectHot(CTerrainMesh::CObject * p) 
			{
				m_processParams.pObjectHot = p;
			}

			const SProcessParams * GetProcessParams() { return &m_processParams; } 

			void InitProcessParams();

			void MoveTo(CTerrainMesh::CObject * p);

			void SetBlockType(int n);

			void SetActiveObject(CTerrainMesh::CObject * p)
			{
				m_pObject = p;
			}

			CTerrainMesh::CObject * GetActiveObject()
			{
				return m_pObject;
			}

			void SetTransType(CFace::ETrans_Type e);

			ICoreGraphic * GetCoreGraphic();
			IGraphicModule * GetGraphicModule();

			void GetFXNames(std::string & strFXFileName, std::map< std::string, std::vector< std::string > > & mapFX, void ** p);

			void ToolSetModel_LinkEffect(std::string str, void * p);
		};

	}
}