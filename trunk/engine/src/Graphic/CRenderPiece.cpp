#include "StdAfx.h"
#include "CRenderPiece.h"
#include "CGraphic.h"
#include "CRenderMatrix.h"
#include "TSqrAllocator.inl"
#include "CColor.inl"
// add end
void CRenderPiece::Render( float Dist, CRenderMatrix& ModelMatrix, RenderObjStyle* pRORS )
{
	//// modify 2005-10-8
	//// …Ë÷√rsµƒ¥ÌŒÛ
	//if ( m_RS.m_nMask )
	//{
	//	memset( &m_RS, 0, sizeof( CPieceRS ) );
	//}

	//if ( !ModelMatrix.m_IsAniBlend )
	//	m_PreFrameNo = (WORD)ModelMatrix.m_CurFrame;

	if ( m_bShow )
	{
		size_t size = m_Pieces.size();
		for ( size_t iPieces = 0; iPieces < size; ++iPieces )
		{
			if (m_Pieces[iPieces].m_bShow)
				m_Pieces[iPieces].m_pPiece->Render( m_RenderStyle, Dist, ModelMatrix, pRORS, this, iPieces,m_Pieces[iPieces].m_Modify );
		}
	}
}

void CRenderPiece::SetPieceClassRS( const CPieceRS& rs )
{
	m_RS = rs;
}


bool CRenderPiece::SetNextAnimation( const IDNAME& szAniName )
{
	m_MaxVerAniNo = 0;
	bool isHasAni = false;

	size_t size = m_Pieces.size();
	for ( size_t iPieces = 0; iPieces < size; ++iPieces )
	{
		m_Pieces[iPieces].m_CurPiecesAniNo = 0;
		m_Pieces[iPieces].m_PrePiecesAniNo = m_Pieces[iPieces].m_CurPiecesAniNo;
		CMesh* Mesh = m_Pieces[iPieces].m_pPiece->GetMesh();
		if ( Mesh->GetVertexType() == VT_VERTEXANI )
		{
			for ( WORD i = 0; i < (int)Mesh->GetAniNum(); i++ )
			{
				if ( Mesh->GetAniName(i) == szAniName )
				{
					m_Pieces[iPieces].m_CurPiecesAniNo = i;
					isHasAni = true;
					break;
				}
			}
		}
		m_MaxVerAniNo = max( m_MaxVerAniNo, (WORD)Mesh->GetFrameNum( m_Pieces[iPieces].m_CurPiecesAniNo ) );
	}
	return isHasAni;
}

CRenderPiece::CRenderPiece( const GVector<CPiece*>& Pieces, uint16 rs )
{
	size_t size = Pieces.size();
	for ( size_t i = 0; i < size; ++i )
	{
		if ( Pieces[i] )
		{
			m_Pieces.push_back( PieceData() );
			PieceData& PD       = m_Pieces[ m_Pieces.size() - 1 ];
			PD.m_pPiece         = Pieces[i];
			PD.m_CurPiecesAniNo = 0;
			PD.m_PrePiecesAniNo = 0;
		}
	}

	m_MaxVerAniNo	= 0;
	m_RenderStyle	= rs;
	m_bShow			= TRUE;
	memset( &m_RS, 0, sizeof( CPieceRS ) );
}

CRenderPiece::CRenderPiece( CPiece* Pieces, uint16 rs )
{
	if(Pieces)
	{

		m_Pieces.push_back( PieceData() );
		PieceData& PD       = m_Pieces[0];
		PD.m_pPiece         = Pieces;
		PD.m_CurPiecesAniNo = 0;
		PD.m_PrePiecesAniNo = 0;
	}
	m_MaxVerAniNo	= 0;
	m_RenderStyle	= rs;
	m_bShow			= TRUE;
	memset( &m_RS, 0, sizeof( CPieceRS ) );
}

