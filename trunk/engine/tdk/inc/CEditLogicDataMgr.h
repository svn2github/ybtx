#pragma once
#include "CLogicDataMgr.h"

namespace sqr_tools
{
	class CEditLogicData;
	class CTerrainTextureData;

	class CEditLogicDataMgr : public CLogicDataMgr
	{
		friend class CTerrainOperate;
	public:
		CEditLogicDataMgr(){};
		virtual ~CEditLogicDataMgr(){};

		CEditLogicData* GetEditLogicData(const URegionID& id);
		CEditLogicData* GetEditLogicData(uint32 uX, uint32 uY);

		void		SetVertexPosition(uint32 uX, uint32 uY, const CVector3f& vertex);
		void		SetVertexPositionHeight(uint32 uX, uint32 uY, const float fHeight);
		CVector3f	GetVertexPosition(uint32 uX, uint32 uY);

		void		SetVertexLogicNormal(uint32 uX, uint32 uY, const CVector3f& normal);
		CVector3f	GetVertexLogicNormal(uint32 uX, uint32 uY) ;
		
		void		SetVertexDiffuse(uint32 uX, uint32 uY, const DWORD color);
		DWORD		GetVertexDiffuse(uint32 uX, uint32 uY);

		void		SetWaterHeight(uint32 uX, uint32 uY, const float fHeight);
		float		GetWaterHeight(uint32 uX, uint32 uY);

		void		SetBlockType(uint32 uX, uint32 uY, const int8 type);
		int8		GetBlockType(uint32 uX, uint32 uY);

		void		SetGridTextureName(uint32 uX, uint32 uY, int nLayer, const string& texname);
		string		GetGridTextureName(uint32 uX, uint32 uY, int nLayer);

		void		SetTextureData(CTerrainTextureData* pTerrainTextureData);
		int			GetTextureIndex(const string& texname);

	protected:
		CElementNode*	_CreateNode( URegionID ID );

	private:
		CTerrainTextureData*	m_pTerrainTextureData;
	};
}