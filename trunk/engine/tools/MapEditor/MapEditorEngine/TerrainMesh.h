#pragma once
#include "TerrainMeshVertex.h"
#include "TerrainMeshRegion.h"
#include "TerrainMeshGrid.h"
#include "IBarrierInfo.h"

#pragma warning(disable : 4482)
namespace sqr_tools
{
	class   CEditTerrainMgr;
}

namespace sqr
{
	//Graphic 内部类
	class	CWireBox;
	class	CRiverWave;
	class	CD3DTexture;
	class	CLightSystem;
	class	IProgressCallback;
	class	ITexture;
	SmartPointer(DepathModelInfo);
	typedef vector<DepathModelInfoPtr> DepathModelInfoArray;
	typedef map<int , DepathModelInfoArray> DepathModelInfoMap;

	enum
	{
		VERTEX_COLOR = 0x00808080,
		WATER_COLOR	 = 0xff808080,
		VERTEX_SPECULAR_COLOR = 0,
	};

	class CTerrainMesh :public IBarrierInfo				//: public ISceneNode
	{
	public:
		static const uint Tile_PreNum;
		static const uint Tile_PerSqr;

#pragma region StructDefinition

		struct SdwTextureNames
		{
			SdwTextureNames()
			{
				strTextureNames[0] = strTextureNames[1] = strTextureNames[2] = strTextureNames[3] = "";
			}

			string strTextureNames[4];
		};

		struct PX
		{
			PX();
			PX(float dis, int i, int j, bool dyn, bool spotlight);

			float		distance;
			int			indexi;
			int			indexj;
			bool        Dynamic;
			bool		bSpotLight;
		};
#pragma endregion StructDefinition

#pragma region MemberVariable
	private:
		DWORD		m_dwTerrainVersion;

		///标识是否是水
		bool m_bWater;
		CTerrainMesh* m_pWater;

		vector<CRiver *> m_vecRivers;
		set<int>	m_setGridIndicesVisibleWater;

		float		m_fHeightDeltaUp; 
		float		m_fHeightDeltaDown; 

		static HWND m_hClassView;

		SVertex*    m_pVertices;
		DWORD		m_dwVertexCnt;

		SGrid*		m_pGrids;
		DWORD		m_dwGridCnt;

		DWORD		m_dwWidth;
		DWORD		m_dwDepth;

		DWORD		m_colVertexCnt;
		DWORD		m_rowVertexCnt;

		DWORD		m_dwWidthInRegion;
		DWORD		m_dwDepthInRegion;

		DWORD		m_dwWidthInDivideReg;
		DWORD		m_dwDepthInDivideReg;

		DWORD		m_dwWidthInTileRegion;
		DWORD		m_dwDepthInTileRegion;

		DWORD		m_dwVerterxCntInTileRegion;///4*4区域的顶点数
		DWORD		m_dwGridCntInTileRegion;   ///4*4区域的格子数

		string		m_strTextureNameDefault;

		SRegion*	m_pRegions;
		DWORD		m_dwRegionCnt;
		DWORD		m_dwTileRegionCnt;

		float		m_fMaxGridHeight;

		bool		m_bCliffLayerOne;
		bool		m_bCliffLayerTwo;
		bool		m_bXZPlaneProject;

		CLightSystem *pLightSystem;

		DWORD		m_nOldWidth;				// 保存地表粘贴前的宽度，以备恢复
		DWORD		m_nOldDepth;				// 保存地表粘贴前的深度，以备恢复

		static vector<PX> m_sortLight;
		static bool cmplight( PX &a, PX &b );	// 用于记录导出的客户端，服务端文件的版本ID号

		DWORD		m_dwGameClientID;
		DWORD		m_dwGameServerID;

		bool		m_bLoadSmallMap;
		map<int, SDivideRegion*> m_pDivideRegions;
		uint8		m_uReSizeMapType;
		string		m_strTerrainDefaultTexName;
	public:
		CEditTerrainMgr*	m_pTerrain;
		ITexture*			m_pTerrainTexture;

		bool		m_bHideAdhereGridsRender;
		bool		m_bDiffMapMark;				// 是否比较保存纹理名
		DWORD		m_dwVisibleRegionCnt;		// 每屏有效区域
		string		m_strMapName;				// 地图名
		DepathModelInfoMap m_DepathModelMap;
		map<string, int> m_MainRoadWeight;
		map<int, int> m_SubGridIndexs;// 临时记录细分格子的索引 // first subgridindex, second gridindex
		map<int, int> m_OldMapGridIndexMap;///扩展后旧地图格子索引在新地图中的新索引映射 first old grid index second new grid index
		map<int, int> m_OldMapRegionIndexMap;///扩展后旧地图细化区域索引在新地图中的新索引映射 first old region index second new region index
		map<DWORD, vector<DWORD>> _RegionAjenctList;
		vector<SdwTextureNames> m_vecTextureNames;
		vector<SGrid> m_vecOldGrids;			// 保存地表粘贴前的格子，以备恢复
		set<int>	groupIndex;
		set<string> m_TerrainTextureLists;
		set<int> m_DynamicLightGrids;			// 用于记录所有创建的动态点光源所在格子的索引集合
		set<int> m_VisibleDynamicLightGrids;	// 在可见区域内动态点光源所在格子的索引集合
		set<string> m_SceneModelList;// 记录场景中所有的物体模型名
		set<DWORD> m_TerrainBufferIndexs;
		vector<DWORD> VisibleGridIndices;//每帧可视格子组
		vector<DWORD> calRangeVisibleGridIndices;//每帧可视格子组
		vector<STileRegion*> m_vecTileRegionList;
		void CalRangeVisibleGrid();

#pragma endregion MemberVariable
	public:

		CTerrainMesh();
		~CTerrainMesh();

		// 创建地形
		bool	Create(DWORD dwWidth, DWORD dwDepth, string strTextureNameDefault, bool bWater, bool bNewTerrain);

		void	SetGameGacMapVersion(const DWORD version);
		void	SetGameGasMapVersion(const DWORD version);
		DWORD   GetGameGacMapVersion() const;
		DWORD   GetGameGasMapVersion() const;
	
		void	SetTerrainDefaultTextureName(const string& name);
		string  GetTerrainDefaultTextureName() const;
		// ----------------------------------------------------------------------------
		// 水
#pragma region Water
		void	SetWater(CTerrainMesh * p);
		CTerrainMesh* CTerrainMesh::GetWater();

		void	 AdjustWaterMeshVisibility(int nGridIndex, bool bLoadTime = false);

		void	 SetGridRiver(int nGridIndex, CRiver * pRiver);
		void	 AddRiver(CRiver * river);

		void	 EraseGridIndexVisibleWater(int n);
		void	 InsertGridIndexVisibleWater(int n);

		size_t   GetRiverCount() const;
		CRiver*  GetRiver(size_t index);
		set<int> GetGridIndicesVisibleWater() const;
#pragma endregion Water

		// ----------------------------------------------------------------------------
		// 关于地形数据的接口
#pragma region TerrainData
		DWORD		GetTerrainVersion() const;
		void		SetTerrainVersion(const DWORD version);
		int			GetDrawnGridCnt();
		int			GetWidth();
		int			GetDepth();
		int			GetWidthInRegion();
		int			GetDepthInRegion();
		int			GetWidthInTileRegion();
		int			GetDepthInTileRegion();
		int			GetVertexCountInTileRegion() const;
		int			GetGridCountInTileRegion() const;

		int			GetRegionIndexByGridIndex(int nGridIndex);
		int			GetDivideRgnIndexByGridIndex(int nGridIndex);
		int			GetRegionCount();
		int			GetRegionHot();
		SRegion&	GetRegion(int nRegionIndex);

		DWORD 		 GetTileRegionCount() const;
		STileRegion* GetTileRegion(DWORD nTileRegionIndex);

		SDivideRegion* GetDivideRegion(int nDivideRegionIndex);

		int			GetGridCount();
		SGrid&		GetGrid(DWORD dwGridIndex);
		SGrid*		GetGrids();
		SGrid&		GetGridHot();
		SVertex&	GetVertex(DWORD dwVertexIndex);
		SVertex*	GetVertices();
		DWORD		GetVertexCount();
		SVertex&	GetVertexHot();
		DWORD		GetVertexIndexHot();

		// 得到当前场景中用到的纹理个数
		int			GetGroundTextureCnt();

		DWORD 		ToResizeGridIndex(DWORD dwWidthNew, DWORD dwDepthNew, DWORD dwGridIndex, const uint8 uResizeType = 0);

		// 判断地形中点、格子索引、区域索引是否有效
		bool IsValid(CVector3f v);
		bool IsValid(int nGridIndex);
		bool IsRegionValid(int nRgnIndex);
		bool IsSubRegionValid(int nSubRgnIndex);
		bool IsValidVertexIndex(int nVertexIndex);
		bool IsValidVec(CVector3f& v);

		bool IsOverlappingByVertexIndex(DWORD dwWidthNew, DWORD dwDepthNew, DWORD dwVertexIndexNew, DWORD & dwVertexIndexOld, const uint8 uResizeType = 0);
		bool IsOverlappingByGridIndex(DWORD dwWidthNew, DWORD dwDepthNew, DWORD dwGridIndexNew, DWORD & dwGridIndexOld, const uint8 uResizeType = 0);

		void CopyGrids(DWORD dwGridIndexCenterDest, DWORD dwWidth, DWORD dwDepth, vector<int> & vGridIndicesSrc );
		void RotateGrids(DWORD dwGridIndexCenterDest, DWORD dwWidth, DWORD dwDepth, vector<int> & vGridIndicesSrc, int nRot, int nAnchorPoint, byte nMode = 0 );//%%%%% nMode: 0-正常粘贴地表,1-增高地表,2-降低地表
#pragma endregion TerrainData

		// ----------------------------------------------------------------------------
		// 文件操作
#pragma region IO
		void SaveAsColor(IProgressCallback* pProgress );
		void LoadAsColor(IProgressCallback* pProgress);

#pragma endregion IO

		// ----------------------------------------------------------------------------
		// 实时更新
		void UpdateFPSLimited();
		void UpdateRiverSideTexture();

		// ----------------------------------------------------------------------------
		// 渲染
#pragma region Render
		void RenderRivers();
		void UpdateSceneLight();	   // 实时状态下更新场景有效光源
		void RenderTerrainBlocks(); // 渲染阻挡
		void RenderLogicHeight();	   // 渲染逻辑高
		void SetSceneLight();		   // 开启场景中的点光源
		void SetVisibleSceneGrids();
		bool IsVisibleGrid(const DWORD& gridindex);
		bool IsVisibleTileRegion(const DWORD& bufferIndex);
#pragma endregion Render

		// ----------------------------------------------------------------------------
		// 逻辑
#pragma region Logical
		// 设置格子纹理
		void SetGridTextureHandle(DWORD dwLayer, DWORD dwGridIndex, string strTextureName, const bool bCliff );

		void ResetHeightDeltaUp();
		void ResetHeightDeltaDown();
		void InitHeightDeltaAll();

		int  InWhichRegion(int nGridIndex);

		// 是否相邻点
		bool IsPhysicallyNearBasedOnVertex(int n1, int n2);
		bool  IsWater();

		string GetCursorHotTextureName(int nGridIndex, int nLayer);

		//根据点光源的照射范围判断该点光源是否需要开启
		bool EnableDynamicLight(const int& gridindex);
		void DecideVisibleDynamicLights();
		///以当前热点格子区域为中点的九宫格区域，得到这9个区域的所有点光源或聚光灯信息，进行排序
		void DecideCurRegDynamicLights();

		void ChangeGridsTextureName(const string &oldname, const string &newname);
		int  ChangeSceneModelName(const string &oldname, string &newname);
		float GetGridTotalHeight(DWORD dwGridIndex);

		void CreateTerrainTexture( string& texname );
		bool CreateTerrainTexture( vector<string> &texnames );

		// 为了删除纹理和替换纹理，重新拼接图；主要是为了已经超过255张地图而用
		bool CreateTerrainTextureAgain( vector<string> &deletetexnames, vector<string> &texnames );
		void ReLoadLightMap(const bool bLoadLightMap);
		void LoadTerrainTexture();
		void InitTerrain(CEditTerrainMgr* pTerrain);
		void SetTerrainNodeVisbileMark(const int nodeIndex, const int nodeSubIndex, const bool bMark);
		void GetTerrainNodeVisbileMark(const int nodeIndex, vector<bool>& visibles);
		// bufferIndexs 要重Lock的buffer索引
		// 第几号数据流重新Lock
		void ReWriteTerrainBuffer( set<DWORD>& bufferIndexs, int streamIndex, const DWORD color = 0 );
		void ReWriteTerrainBuffer( const set<DWORD>& bufferIndexs, const set<int>& gridIndexs, const DWORD color0, const DWORD color1 );
		void TurnOnCliff(bool tf);

		void DepathGroupModel(index_t nodeIndex = 0);
		void DepathMapGroupModel();

		// 计算地表权重
		void CalTerrainRoadWeight();

		// 如果某一层贴图完全被另外一层贴图盖住了，就省掉这个被遮盖的纹理信息
		void WipeOffUnUseGridTextureInfo();

		void SetResizeMapType( uint8 type );
		uint8 GetResizeMapType() const;

		uint32 GetTileCoordByGrid(uint32 nGridIdx);

		bool m_bUseVertexBufferRenderBlock;

#pragma endregion Logical
		
#pragma region IBarrierInfoMember

		virtual	inline uint32 GetExactWidth();
		virtual inline uint32 GetExactHeight() ;
		virtual inline int GetBarrierType(int x, int y);

#pragma endregion IBarrierInfoMember
	};

#pragma region InlineMethod

	// -----------------------------------------------------------------------------
	inline void	CTerrainMesh::SetGameGacMapVersion(const DWORD version)
	{
		m_dwGameClientID = version;
	}

	// -----------------------------------------------------------------------------
	inline void	CTerrainMesh::SetGameGasMapVersion(const DWORD version)
	{
		m_dwGameServerID = version;
	}

	// -----------------------------------------------------------------------------
	inline DWORD CTerrainMesh::GetGameGacMapVersion() const
	{
		return m_dwGameClientID;
	}

	// -----------------------------------------------------------------------------
	inline DWORD CTerrainMesh::GetGameGasMapVersion() const
	{
		return m_dwGameServerID;
	}

	// -----------------------------------------------------------------------------
	inline void	CTerrainMesh::SetTerrainDefaultTextureName(const string& name)
	{
		m_strTerrainDefaultTexName = name;
	}

	// -----------------------------------------------------------------------------
	inline string  CTerrainMesh::GetTerrainDefaultTextureName() const
	{
		return m_strTerrainDefaultTexName;
	}

	// -----------------------------------------------------------------------------
	inline void CTerrainMesh::ResetHeightDeltaUp()
	{
		m_fHeightDeltaUp = -m_fHeightDeltaDown;
	}

	// -----------------------------------------------------------------------------
	inline void CTerrainMesh::ResetHeightDeltaDown()
	{
		m_fHeightDeltaDown = -m_fHeightDeltaUp;
	}

	// -----------------------------------------------------------------------------
	inline void CTerrainMesh::InitHeightDeltaAll()
	{
		m_fHeightDeltaUp = 0.0f;
		m_fHeightDeltaDown = 0.0f;
	}

	// -----------------------------------------------------------------------------
	inline int CTerrainMesh::InWhichRegion( int nGridIndex )
	{
		int nX = ( nGridIndex % this->GetWidth() ) / REGION_SPACE;
		int nY = ( nGridIndex / this->GetWidth() ) / REGION_SPACE;
		return nY * this->GetWidthInRegion() + nX;
	}

	// -----------------------------------------------------------------------------
	inline int CTerrainMesh::GetWidth()
	{
		return m_dwWidth;
	}

	// -----------------------------------------------------------------------------
	inline int CTerrainMesh::GetDepth()
	{
		return m_dwDepth;
	}

	// -----------------------------------------------------------------------------
	inline int CTerrainMesh::GetWidthInRegion()
	{
		return m_dwWidthInRegion;
	}

	// -----------------------------------------------------------------------------
	inline int CTerrainMesh::GetDepthInRegion()
	{
		return m_dwDepthInRegion;
	}


	// -----------------------------------------------------------------------------
	inline int CTerrainMesh::GetWidthInTileRegion()
	{
		return m_dwWidthInTileRegion;
	}

	// -----------------------------------------------------------------------------
	inline int CTerrainMesh::GetDepthInTileRegion()
	{
		return m_dwDepthInTileRegion;
	}

	// -----------------------------------------------------------------------------
	inline int CTerrainMesh::GetRegionCount()
	{
		return m_dwRegionCnt;
	}

	// -----------------------------------------------------------------------------
	inline int CTerrainMesh::GetGridCount()
	{
		return m_dwWidth*m_dwDepth;
	}

	// -----------------------------------------------------------------------------
	inline int CTerrainMesh::GetVertexCountInTileRegion() const
	{
		return m_dwVerterxCntInTileRegion;
	}

	// -----------------------------------------------------------------------------
	inline int CTerrainMesh::GetGridCountInTileRegion() const
	{
		return m_dwGridCntInTileRegion;
	}

	// -----------------------------------------------------------------------------
	inline SGrid * CTerrainMesh::GetGrids()
	{
		return m_pGrids;
	}

	// -----------------------------------------------------------------------------
	inline SVertex * CTerrainMesh::GetVertices()
	{
		return m_pVertices;
	}

	// -----------------------------------------------------------------------------
	inline DWORD CTerrainMesh::GetVertexCount()
	{
		return m_dwVertexCnt;
	}

	inline int CTerrainMesh::GetDrawnGridCnt()
	{
		return (int)(VisibleGridIndices.size());
	}

	// -----------------------------------------------------------------------------
	inline void CTerrainMesh::AddRiver(CRiver * river)
	{
		m_vecRivers.push_back(river);
	}

	// -----------------------------------------------------------------------------
	inline void CTerrainMesh::InsertGridIndexVisibleWater(int n)
	{
		m_setGridIndicesVisibleWater.insert(n);
	}

	// -----------------------------------------------------------------------------
	inline void CTerrainMesh::EraseGridIndexVisibleWater(int n)
	{
		m_setGridIndicesVisibleWater.erase(n);
	}

	// -----------------------------------------------------------------------------
	inline size_t CTerrainMesh::GetRiverCount() const
	{
		return m_vecRivers.size();
	}
	
	// -----------------------------------------------------------------------------
	inline CRiver* CTerrainMesh::GetRiver(size_t index)
	{
		return m_vecRivers[index];
	}

	// -----------------------------------------------------------------------------
	inline set<int> CTerrainMesh::GetGridIndicesVisibleWater() const
	{
		return m_setGridIndicesVisibleWater;
	}

	// -----------------------------------------------------------------------------
	inline void CTerrainMesh::SetWater(CTerrainMesh * p)
	{
		m_pWater = p;
	}

	// -----------------------------------------------------------------------------
	inline CTerrainMesh * CTerrainMesh::GetWater()
	{
		return m_pWater;
	}

	// -----------------------------------------------------------------------------
	inline bool CTerrainMesh::IsWater()
	{
		return m_bWater;
	}

	// -----------------------------------------------------------------------------
	inline bool CTerrainMesh::IsPhysicallyNearBasedOnVertex(int n1, int n2)
	{
		int deltaRow = abs((n1 % (this->GetWidth()+1)) - (n2 % (this->GetWidth()+1)));
		int deltaCol = abs((n1 / (this->GetWidth()+1)) - (n2 / (this->GetWidth()+1)));
		if ( max(deltaRow, deltaCol ) == 1 )
			return true;
		return false;
	}

	// -----------------------------------------------------------------------------
	inline bool CTerrainMesh::IsValid(CVector3f v)
	{
		return ( 0.0 <= v.x && v.x < this->GetWidth() &&
				 0.0 <= v.z && v.z < this->GetDepth() );
	}

	// -----------------------------------------------------------------------------
	inline bool CTerrainMesh::IsValidVec(CVector3f & v)
	{
		return ( 0.0 <= v.x && v.x < this->GetWidth()*GRID_SPACE &&
				 0.0 <= v.z && v.z < this->GetDepth()*GRID_SPACE );
	}

	// -----------------------------------------------------------------------------
	inline bool CTerrainMesh::IsValid(int nGridIndex)
	{
		return ( nGridIndex != INVALID && nGridIndex >= 0 && nGridIndex < (int)m_dwGridCnt );
	}

	// -----------------------------------------------------------------------------
	inline bool CTerrainMesh::IsRegionValid(int nRgnIndex)
	{
		return ( nRgnIndex != INVALID && nRgnIndex >= 0 && nRgnIndex < (int)m_dwRegionCnt );
	}

	// -----------------------------------------------------------------------------
	inline bool CTerrainMesh::IsValidVertexIndex(int nVertexIndex)
	{
		return ( nVertexIndex != INVALID && nVertexIndex >= 0 && nVertexIndex < (this->GetDepth()+1) * (this->GetWidth()+1) );
	}

	// -----------------------------------------------------------------------------
	inline SGrid & CTerrainMesh::GetGrid(DWORD dwGridIndex)
	{
		if ( !this->IsValid(dwGridIndex) )
		{
			TCHAR szMessage[256];

			sprintf(szMessage, "%d格子越界" , dwGridIndex);

			MessageBox(NULL, szMessage, "提示", MB_OK | MB_ICONERROR);

			return m_pGrids[0];
		}

		return m_pGrids[dwGridIndex];
	}

	// -----------------------------------------------------------------------------
	inline SRegion & CTerrainMesh::GetRegion(int nRegionIndex)
	{
		if( !this->IsRegionValid(nRegionIndex) )
		{
			TCHAR szMessage[256];

			sprintf(szMessage, "%d区域越界" , nRegionIndex);

			MessageBox(NULL, szMessage, "提示", MB_OK | MB_ICONERROR);

			return m_pRegions[0];
		}


		if ( nRegionIndex != INVALID )
			return m_pRegions[nRegionIndex];
		else
			return m_pRegions[0];
	}

	inline DWORD CTerrainMesh::GetTileRegionCount() const
	{
		return m_dwTileRegionCnt;
	}
	// -----------------------------------------------------------------------------

	// -----------------------------------------------------------------------------
	inline int CTerrainMesh::GetRegionIndexByGridIndex(int nGridIndex)
	{
		int nX = ( nGridIndex % this->GetWidth() ) / REGION_SPACE;
		int nY = ( nGridIndex / this->GetWidth() ) / REGION_SPACE;
		return nX + nY*this->GetWidthInRegion();
	}

	// -----------------------------------------------------------------------------
	inline int CTerrainMesh::GetDivideRgnIndexByGridIndex(int nGridIndex)
	{
		int col = nGridIndex % this->GetWidth();
		int row = nGridIndex / this->GetWidth();

		int x = col / REGION_DIVIDESPACE;
		int y = row / REGION_DIVIDESPACE;

		return x + y * this->GetWidthInRegion() * REGION_STEP_DIVIDE;
	}

	// -----------------------------------------------------------------------------
	inline string CTerrainMesh::GetCursorHotTextureName(int nGridIndex, int nLayer)
	{
		if ( nGridIndex == INVALID || nLayer == INVALID )
			return "";
		else
		{
			SGrid & grid = this->GetGrid(nGridIndex);
			return  grid.nodeInfo.strTextureNames[nLayer];
		}
	}

	// -----------------------------------------------------------------------------
	inline DWORD CTerrainMesh::GetTerrainVersion() const
	{
		return m_dwTerrainVersion;
	}

	// -----------------------------------------------------------------------------
	inline void CTerrainMesh::SetTerrainVersion( const DWORD version )
	{
		m_dwTerrainVersion = version;
	}

	// -----------------------------------------------------------------------------
	inline bool
	CTerrainMesh::cmplight( PX &a, PX &b )
	{
		return a.distance < b.distance;
	}

	// -----------------------------------------------------------------------------
	inline void 
	CTerrainMesh::SetResizeMapType( uint8 type )
	{
		m_uReSizeMapType = type;
	}

	// -----------------------------------------------------------------------------
	inline uint8 
	CTerrainMesh::GetResizeMapType() const
	{
		return m_uReSizeMapType;
	}
	
	inline uint32 CTerrainMesh::GetExactWidth()
	{
		return m_dwWidth << 1;
	}

	inline uint32 CTerrainMesh::GetExactHeight() 
	{
		return m_dwDepth << 1;
	}

	inline int CTerrainMesh::GetBarrierType(int x, int y)
	{
		SGrid * pGrid = m_pGrids + (y >> 1) * m_dwWidth + (x >> 1);
		return pGrid->nBlockType;
	}
#pragma endregion  
}
