#pragma once
#include "CLogicData.h"

namespace sqr_tools
{
	const int SRegionSpanNum   = eSRegionSpan*eSRegionSpan;
	const int nTextureLayerNum = 2;
	const DWORD VERTEX_COLOR   = 0x00808080;

	class CEditLogicData : public CLogicData
	{
	public:
		CEditLogicData(CElementManager* pParent , URegionID id);
		virtual ~CEditLogicData();
		
		void		Init();

		void		SetLogicNormal(uint32 index, const CVector3f& normal);
		CVector3f	GetLogicNormal(uint32 index) const;
		CVector3f	GetLogicNormal(uint x, uint z) const;

		void		SetVertexPosition(uint32 index, const CVector3f& pos);
		CVector3f	GetVertexPosition(uint32 index) const;

		void		SetVertexDiffuse(uint32 index, DWORD color);
		DWORD		GetVertexDiffuse(uint32 index) const;

		void		SetVertexBakingColor(uint32 index, DWORD color);
		DWORD		GetVertexBakingColor(uint32 index) const;

		void		SetLogicHeight(uint32 index, const int nLogicHeight);
		int			GetLogicHeight(uint32 index) const;

		void		SetWaterHeight(uint32 index, const float fWaterHeight);
		float		GetWaterHeight(uint32 index) const;
		
		void		SetBlockType(uint32 index, int8 nBlockType);
		int8		GetBlockType(uint32 index);

		void		SetMaterialIndex(uint32 index, int8 nMatIndex);
		int8		GetMaterialIndex(uint32 index);

		void		SetGridHideMark(uint32 index, bool bHide);
		bool		GetGridHideMark(uint32 index);

		void		SetGridTextureName(uint32 index, int nLayer, const string& texname);
		string		GetGridTextureName(uint32 index, int nLayer);

	protected:
		typedef SVector<int>			IntPool;	
		typedef SVector<DWORD>			ColorPool;
		typedef SVector<bool>			BoolPool;
		typedef SVector<string>			StringPool;
		typedef SMap<int, StringPool>	TexNamePool;

		VectorPool						m_aryVertexPosition;
		ColorPool						m_aryVertexDiffuse;
		ColorPool						m_aryVertexBakingColor;
		IntPool							m_aryVertexLogicHight;
		Int8Pool						m_aryBlockType;
		BoolPool						m_aryGridHide;

		//////////////////////////////////////////////////////////////////////////
		//根据地表2层纹理混合比例，计算最后该格子地表的材质，比如石头等
		//////////////////////////////////////////////////////////////////////////
		TexNamePool						m_aryGridTextureNames;
	};
}