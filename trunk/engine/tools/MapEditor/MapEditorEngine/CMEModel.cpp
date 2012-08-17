#include "stdafx.h"
#include "CMEModel.h"
#include "CRenderPiece.h"

namespace sqr
{
	int CMEModel::GetRenderStyle( GMap< GString, CRenderPiece > &m_MeshsPiece )
	{
		if ( m_MeshsPiece.empty() ) return 0;

		int nMaxRS = 0;

		if ( !m_MeshsPiece.empty() )
		{
			for ( GMap< GString, CRenderPiece >::iterator it = m_MeshsPiece.begin(); it != m_MeshsPiece.end(); it++ )
			{
				::CRenderPiece & RenderPiece = it->second;

				nMaxRS = max(nMaxRS, RenderPiece.m_RenderStyle);
			}
		}

		return nMaxRS;
		//return 0;
	}

	void CMEModel::SetRenderStyle(GMap< GString, CRenderPiece > &m_MeshsPiece, int nRenderStyle)
	{
		for ( GMap< GString, CRenderPiece >::iterator it = m_MeshsPiece.begin(); it != m_MeshsPiece.end(); it++ )
		{
			CRenderPiece & RenderPiece = it->second;

			RenderPiece.m_RenderStyle = nRenderStyle;
		}

	}
	uint16 CMEModel::UseNextRenderStyle(GMap< GString, CRenderPiece > &m_MeshsPiece)
	{
		for ( GMap< GString, CRenderPiece >::iterator it = m_MeshsPiece.begin(); it != m_MeshsPiece.end(); it++ )
		{
			uint32 uMaxRSNum = 0;

			CRenderPiece & RenderPiece = it->second;
			GString str = it->first;

			for ( size_t i = 0; i < RenderPiece.m_Pieces.size(); ++i )
			{
				::CRenderPiece::PieceData * pPieceData = &RenderPiece.m_Pieces[i];
				CPiece * pPiece = pPieceData->m_pPiece;
				uMaxRSNum = max(uMaxRSNum, pPiece->GetRSNum());
			}

			RenderPiece.m_RenderStyle = (RenderPiece.m_RenderStyle+1)%uMaxRSNum;

			return RenderPiece.m_RenderStyle;
		}

		return 0;
	}
}