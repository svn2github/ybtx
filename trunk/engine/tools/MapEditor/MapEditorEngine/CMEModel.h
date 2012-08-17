#ifndef _CMEModel_H_
#define _CMEModel_H_
#include "CGraphicMallocObject.h"
namespace sqr
{
	class CRenderPiece;
	namespace CMEModel
	{
		int GetRenderStyle( GMap< GString, CRenderPiece > &m_MeshsPiece );
		void SetRenderStyle(GMap< GString, CRenderPiece > &m_MeshsPiece, int nRenderStyle);
		uint16 UseNextRenderStyle(GMap< GString, CRenderPiece > &m_MeshsPiece);
	};
};

#endif