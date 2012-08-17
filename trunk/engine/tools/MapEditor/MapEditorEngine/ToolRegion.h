#pragma once
#include "CEventOwner.h"
#include "TSingleton.h"

namespace sqr
{
	class CLogicBrush;
	class ITexture;
	struct RenderState;

	class CToolRegion: public Singleton<CToolRegion>
	{
	public:
		struct sRegionBakingPro
		{
			bool bTerrain;
			bool bObject;
			bool bLogic;
			int  nDistrubSize;

			sRegionBakingPro();
		};

	private:
		HWND				m_hRegionView;

		//画刷
		CLogicBrush*		m_pLogicBrush;
		ITexture *			m_pSubRegionLockTexture;
		RenderState *		m_pRS;
		int					m_nRegionHot;

		set<int>			m_CurSelectRegions;
		string				m_strCurRegionName;
		sRegionBakingPro*	m_pRegionBakingPro;
	public:

		CToolRegion();
		~CToolRegion();
		void OnEvent(const CMsg& msg);
		void Render();
		void Initialize(HWND hRegionView);

		void IncBrushSizeInX();
		void DecBrushSizeInX();
		void IncBrushSizeInZ();
		void DecBrushSizeInZ();

		void SetHideGrid(const bool bHideGrid);
		void RevertAllHideGrid(const bool bHideGrid);

		void SetTerrainGridIsHide( vector<int>& gridindexs, bool b );

		void SetCurrentSelectRegions( const vector<int>& regions );
		void SelectSceneRegion();
		void UpdateCurSelectRegion();
		bool CanleSecneRegion(const vector<int>& vecBrushRegions);
		void SharpTerrainRegion(set<DWORD>& bufferindexs, const DWORD color);
		// 隐藏设置的区域的红色标志
		void	HideRegionMark();

		inline set<int> GetCurrentSelectRegions() const
		{
			return m_CurSelectRegions;
		}

		inline void ClearCurrentSelectRegions()
		{
			m_CurSelectRegions.clear();
		}

		inline HWND GetRegionView() const
		{
			return m_hRegionView;
		}
		
		inline void SetCurRegionName(const string& name )
		{
			m_strCurRegionName = name;
		}

		///region ambient effect
		void SwitchShowAllRgnAmbientEffect(const bool b);
		int  GetCurrentHotSunRegionIndex();
		void GetSelectSceneRegionIndexs(set<int>& bufferindexs );

		//////////////////////////////////////////////////////////////////////////
		//区域烘培属性
		CToolRegion::sRegionBakingPro* GetRegionBakingPro() const;
		void SetIsTerrain(const bool b);
		void SetIsObject(const bool b);
		void SetIsLogic(const bool b);
		void SetRegionDistrubSize(const int distrubSize);
		void SaveRegionBakingProConfig();
		void LoadRegionBakingProConfig();
	};
}
