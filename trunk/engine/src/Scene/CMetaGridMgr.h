#pragma once
#include "CMetaRegion.h"
#include "CCoordinateContainer.h"

namespace sqr
{
	class IGraphicModule;
	class CMetaGridMgr : public CSceneMallocObject
	{
	public:
		enum
		{
			eBlockBits = 0x3,
			eColorEnableBits = 0x4
		};
		CMetaGridMgr( uint32 uWidthInGrid, uint32 uHeightInGrid );

		CMetaGridMgr();

		~CMetaGridMgr();

		void Alloc( uint32 uWidthInGrid, uint32 uHeightInGrid );

		///得到当前像素的高度 = TerrainHeight + LogicHeight
		float GetOffsetHeightInPixel( float fPixelX, float fPixelY );

		float GetOffsetHeightInPixel( float fPixelX, float fPixelY, bool& bHasLogicHeight );

		uint32	GetWidthInRegion();
		uint32	GetHeightInRegion();


		CMetaRegion* GetRegion( uint32 uGridX, uint32 uGridY );

		CMetaRegion* GetRegion( uint32 uRegionId );

		int GetSubRegionIndex( uint32 uGridX, uint32 uGridY );



		void SetRegion( uint32 uRegionId, CMetaRegion* pRgn );

		bool IsGridValid( uint32 uX, uint32 uY );

		//-------------------  OffsetHeight = TerrainHeight + LogicHeight  -------------------
		int16	GetOffsetHeight( uint32 uX, uint32 uY );

		//-------------------		TerranHeight	-------------------
		int16	GetTerranHeight( uint32 uX, uint32 uY );

		//-------------------		TerranLogicHeight	-------------------
		int16	GetTerranLogicHeight( uint32 uX, uint32 uY );

		//-------------------		LogicHeight	-------------------
		int16	GetLogicHeight( uint32 uX, uint32 uY );

		//-------------------		TerranNomal	-------------------
		int8	GetTerranNomal( uint32 uX, uint32 uY );

		//-------------------		TerranLogicNomal	-------------------
		CVector3f GetTerranLogicNomal( uint32 uX, uint32 uY );

		//-------------------		Terrain Material Index	-------------------
		//int8	GetTerrainMaterialIndex( uint32 uX, uint32 uY )
		//{
		//	return GetRegion( uX, uY )->GetTerrainMaterialIndex( _XInRgn(uX), _YInRgn(uY) );
		//}

		//-------------------		TerranColor	-------------------
		const CColor&	GetTerranDiffuseColor( uint32 uX, uint32 uY );

		//-------------------		Terran specular Color	-------------------
		const CColor& GetTerranSpecularColor( uint32 uX, uint32 uY );

		//-------------------		Terran baking Color	-------------------
		const CColor&	GetTerranBakingColor( uint32 uX, uint32 uY );

		//-------------------		TextureIndex	-------------------
		uint32	GetTextureIndex( uint32 uX, uint32 uY );

		const CVector2f&	GetTerranLayer1UV( uint32 uX, uint32 uY );

		const CVector2f&	GetTerranLayer2UV( uint32 uX, uint32 uY );

		const CVector2f&	GetTerranLayer3UV( uint32 uX, uint32 uY );

		const CVector2f&	GetTerranLayer4UV( uint32 uX, uint32 uY );

		bool	GetTerrainGridIsHollow( uint32 uX, uint32 uY );

		//-------------------		Block	-------------------
		uint8	GetTrap( uint32 uX, uint32 uY );

		//-------------------		tile visible mark	-------------------
		void    SetRenderTileVisibleMark(const URegionID& id, bool bVisible);
		bool    GetRenderTileVisibleMark(const URegionID& id);

	protected:
		uint32	_XInRgn( uint32 uGridX )
		{
			return uGridX & 0xf;
		}
		uint32	_YInRgn( uint32 uGridY )
		{
			return uGridY & 0xf;
		}
		uint32  _WidthInRgn( uint32 uWidthInGrid )
		{
			return uWidthInGrid >> 4;
		}
		uint32  _HeightInRgn( uint32 uHeightInGrid )
		{
			return uHeightInGrid >> 4;
		}

	protected:
		CCoordinateVector<CMetaRegion*> m_vecRegion;
		SMap<URegionID, bool>			m_mapRenderTileVisible;
		uint32	m_uWidthInGrid;
		uint32  m_uHeightInGrid;
	};
}


