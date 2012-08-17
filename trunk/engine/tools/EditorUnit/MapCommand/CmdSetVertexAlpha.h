#pragma once
#include "Cmd.h"
#include "CVector2.h"
#include "Brush/BaseBrushEnum.h"

namespace sqr_tools
{
	class CEditContext;
	class CTerrainTextureBrush;

	class CCmdSetVertexAlpha : public CCmd
	{
		struct SVertexAlpha
		{
			CVector2I	vertexCoord;
			DWORD		oColor; ///original color 顶点原始颜色（包括刷的顶点色）
		};

		vector<SVertexAlpha>	m_vecDiffs;
		EWAY_TYPE				m_eWayType;
		CEditContext*			m_pEditContext;
		CTerrainTextureBrush*	m_pTerrainTextureBrush;

	public:
		CCmdSetVertexAlpha(CEditContext* pContext);
		~CCmdSetVertexAlpha() { }

		virtual void _Execute();
		virtual void _UnExecute();

		void AddVertex(const CVector2I& vertexcoord);
		void SetTerrainBrush(CTerrainTextureBrush* pBrush);
		void SetEWayType(const EWAY_TYPE type)
		{
			m_eWayType = type;
		}
	};

}