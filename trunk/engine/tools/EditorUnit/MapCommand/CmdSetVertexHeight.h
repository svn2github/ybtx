#pragma once
#include "Cmd.h"
#include "CVector2.h"
#include "CVector3.h"
#include "Brush\BaseBrushEnum.h"

namespace sqr_tools
{
	class CEditContext;
	class CTerrainOperate;
	class CCmdSetVertexHeight : public CCmd
	{
		struct SCmdVertex
		{
			CVector2I	vertexCoord;
			float		fHeight;
			CVector3f	vertexNormal;
		};
	protected:
		typedef vector<SCmdVertex>	DataVec;

		DataVec				m_vecDiffs;
		CVector2I			m_hotVertex;
		DWORD				m_dwBrushWidth;
		DWORD				m_dwBrushHeight;
		float				m_fStrength;
		CEditContext*		m_pEditContext;
		//Ëã·¨
		CTerrainOperate*	m_pTerrainOp;
		ETERRAIN_OP_TYPE	m_opType;

		virtual void _Execute();
		virtual void _UnExecute();

	public:
		CCmdSetVertexHeight(CEditContext* pContext);
		~CCmdSetVertexHeight();

		virtual void SetContext(CEditContext* pContext);
		virtual bool IsSetContext();

	public:
		virtual void SetOpType(ETERRAIN_OP_TYPE opType);
		virtual void BindOperate(CTerrainOperate* Op);
		virtual void AddVertex(const CVector2I& vertexcoord);
		virtual void SetHotVertex(const CVector2I& vertexcoord);
		virtual void SetBrushSize(const DWORD dwWidth, const DWORD dwHeight);
		virtual void SetStrength(const float f);
		virtual void ClearVertex();
	};

}