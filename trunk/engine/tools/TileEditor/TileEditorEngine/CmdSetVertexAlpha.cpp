//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Cmd.h"
#include "TerrainMesh.h"
#include "Operator.h"
#include "XFile.h"
#include "CmdSetVertexAlpha.h"

#if defined(_DEBUG) && defined(_WIN32)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

namespace bg
{
namespace sc
{

void CCmdSetVertexAlpha::Execute()
{
	CCmd::Execute();
	
	COperator * pOperator = CXFile::GetInstance()->GetOperator();
	CTerrainMesh * pTerrainMesh = CXFile::GetInstance()->GetTerrainMesh();
	
	// for the first time
	if ( 0 == m_processParams.dwBrushSize )
	{
		memcpy(&m_processParams,pOperator->GetProcessParams(),sizeof(COperator::SProcessParams));
		m_processParams.pObjectHot.grab();
	}

	CTerrainMesh::SGrid & grid = pTerrainMesh->GetGrid(m_processParams.dwGridIndexHot);
	int nVertexIndexHot = grid.dwVertexIndex[0];

	for ( std::vector<SVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
	{
		if ( m_processParams.eSetWayType == COperator::ESWT_CUT )
		{
			pTerrainMesh->CutVertexAlpha(iter->dwVertexIndex);
			continue;
		}

		switch ( m_processParams.eSetAxisType ) 
		{
	   case COperator::EAT_X:
		   {
			   switch ( m_processParams.eSetAlphaType ) 
			   {
			   case COperator::ESAT_LINEAR:
				   {
					   switch ( m_processParams.eSetWayType )
					   {
					   case COperator::ESWT_INC:
						   {
							   pTerrainMesh->IncLinearVertexAlphaX(iter->dwVertexIndex);
						   }
						break;

					   case COperator::ESWT_DEC:
						   {
							   pTerrainMesh->DecLinearVertexAlphaX(iter->dwVertexIndex);
						   }
					   break;
					   }

				   }
			   	break;
			   }
		   }
		break;
	
	   case COperator::EAT_Z:
		   {
			   switch ( m_processParams.eSetAlphaType ) 
			   {
			   case COperator::ESAT_LINEAR:
				   {
					   switch ( m_processParams.eSetWayType )
					   {
					   case COperator::ESWT_INC:
						   {
							   pTerrainMesh->IncLinearVertexAlphaZ(iter->dwVertexIndex);
						   }
						break;

					   case COperator::ESWT_DEC:
						   {
							   pTerrainMesh->DecLinearVertexAlphaZ(iter->dwVertexIndex);
						   }
						break;
					   }

				   }
				break;
			   }
		   }
		break;
	   
	   case COperator::EAT_BOTH:
		   {
			   switch ( m_processParams.eSetAlphaType ) 
			   {
			   case COperator::ESAT_LINEAR:
				   {
					   switch ( m_processParams.eSetWayType )
					   {
					   case COperator::ESWT_INC:
						   {
							   pTerrainMesh->IncLinearVertexAlphaB(iter->dwVertexIndex, nVertexIndexHot);
						   }
						break;

					   case COperator::ESWT_DEC:
						   {
							   pTerrainMesh->DecLinearVertexAlphaB(iter->dwVertexIndex);
						   }
					   break;
					   }
				   }
			   	break;
			   }
		   }
		break;
	   }
	}
}

void CCmdSetVertexAlpha::UnExecute()
{	
	CCmd::UnExecute();
	
	CTerrainMesh * pTerrainMesh = CXFile::GetInstance()->GetTerrainMesh();
	
	for ( std::vector<SVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
	{
		pTerrainMesh->SetVertexAlpha(iter->dwVertexIndex, iter->cColor);
	}
}

void CCmdSetVertexAlpha::AddVertex(DWORD dwVertexIndex)
{
	SVertex s;
	s.dwVertexIndex = dwVertexIndex;
	if ( find_if(m_vecDiffs.begin(),m_vecDiffs.end(),IsEqual(s)) != m_vecDiffs.end() )
	{
		return;
	}
	CTerrainMesh::SVertex & vertex = CXFile::GetInstance()->GetTerrainMesh()->GetVertex(dwVertexIndex);
	s.cColor = vertex.cColor;
	m_vecDiffs.push_back(s);
	m_dwVertexCnt = m_vecDiffs.size();
}

}
}