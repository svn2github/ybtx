#pragma once
#include "CPieceGroup.h"

//渲染时模型用到的的Piece
namespace sqr
{
	class CRenderPiece : public CGraphicMallocObject
	{
	public:
		struct PieceData
		{
			PieceData()
			{
				m_bShow = true;
			}

			CPiece*			m_pPiece;
			WORD			m_CurPiecesAniNo;
			WORD			m_PrePiecesAniNo;
			bool			m_bShow;
			PieceModify		m_Modify;
		};

		CRenderPiece( const GVector<CPiece*>& Pieces, uint16 rs );
		CRenderPiece( CPiece* Pieces, uint16 rs );


		uint16					m_MaxVerAniNo;
		uint16					m_RenderStyle;		//使用中的Piece渲染类型
		//uint16					m_PreFrameNo;
		bool					m_bShow;
		CPieceRS				m_RS;

		bool SetNextAnimation( const IDNAME& szAniName );

		void Render( float Dist, CRenderMatrix& ModelMatrix, RenderObjStyle* pRORS );

		void SetPieceClassRS( const CPieceRS& rs );
		void SetPieceClassRS( uint32 nRSIndex )
		{
			m_RenderStyle = (uint16)nRSIndex;
		}

		GVector<PieceData>		m_Pieces;			//使用中的Piece:
		CRenderPiece():m_MaxVerAniNo(0),m_RenderStyle(0),m_bShow(TRUE) {}
	};
	typedef CRenderPiece CPieceClassInstance;

	struct CRenderPair
	{
		CRenderPair( CRenderable* pObj, bool bShow = true ):m_pRenderObj(pObj), m_bShow(bShow) {}

		CRenderable*				m_pRenderObj;
		bool						m_bShow;
	};
}
