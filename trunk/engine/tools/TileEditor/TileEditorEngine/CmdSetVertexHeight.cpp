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
#include "CmdSetVertexHeight.h"

#if defined(_DEBUG) && defined(_WIN32)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

namespace bg
{
namespace sc
{

void CCmdSetVertexHeight::Execute()
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
	
	for ( std::vector<SVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
	{	
		if ( m_processParams.eSetWayType == COperator::ESWT_CUT )
		{
			pTerrainMesh->CutVertexHeight(iter->dwVertexIndex);
			continue;
		}

		switch ( m_processParams.eSetAxisType ) 
		{
		case COperator::EAT_X:
			{
				switch ( m_processParams.eSetHeightType ) 
				{
				case COperator::ESHT_LINEAR:
					{
						switch ( m_processParams.eSetWayType )
						{
						case COperator::ESWT_INC:
							{
								pTerrainMesh->IncLinearVertexHeightX(iter->dwVertexIndex, m_processParams.dwGridIndexHot);
							}
						break;
		
						case COperator::ESWT_DEC:
							{
								pTerrainMesh->DecLinearVertexHeightX(iter->dwVertexIndex, m_processParams.dwGridIndexHot);
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
				switch ( m_processParams.eSetHeightType ) 
				{
				case COperator::ESHT_LINEAR:
					{
						switch ( m_processParams.eSetWayType )
						{
						case COperator::ESWT_INC:
							{
								pTerrainMesh->IncLinearVertexHeightZ(iter->dwVertexIndex, m_processParams.dwGridIndexHot);
							}
						break;

						case COperator::ESWT_DEC:
							{
								pTerrainMesh->DecLinearVertexHeightZ(iter->dwVertexIndex, m_processParams.dwGridIndexHot);
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
				switch ( m_processParams.eSetHeightType ) 
				{
				case COperator::ESHT_LINEAR:
					{
						switch ( m_processParams.eSetWayType )
						{
						case COperator::ESWT_INC:
							{
								pTerrainMesh->IncLinearVertexHeightB(iter->dwVertexIndex, m_processParams.dwGridIndexHot);
							}
						break;

						case COperator::ESWT_DEC:
							{
								pTerrainMesh->DecLinearVertexHeightB(iter->dwVertexIndex, m_processParams.dwGridIndexHot);
							}
						break;
						}
					}
				break;

				case COperator::ESHT_GAUSS:
					{
						switch ( m_processParams.eSetWayType )
						{
						case COperator::ESWT_INC:
							{
								pTerrainMesh->IncGaussVertexHeight(iter->dwVertexIndex, m_processParams.dwBrushSize, m_processParams.dwBrushSize, m_processParams.dwGridIndexHot);
							}
						break;

						case COperator::ESWT_DEC:
							{
								pTerrainMesh->DecGaussVertexHeight(iter->dwVertexIndex, m_processParams.dwBrushSize, m_processParams.dwBrushSize, m_processParams.dwGridIndexHot);
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

	for ( std::vector<SVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
	{
		pTerrainMesh->LightMe(iter->dwVertexIndex);
	}

	for ( std::vector<SVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
	{
		pTerrainMesh->ReCalculateGridsHeight(iter->dwVertexIndex);
	}
}

void CCmdSetVertexHeight::UnExecute()
{	
	CCmd::UnExecute();
	
	CTerrainMesh * pTerrainMesh = CXFile::GetInstance()->GetTerrainMesh();
	
	for ( std::vector<SVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
	{
		pTerrainMesh->SetVertexHeight(iter->dwVertexIndex, iter->fHeight);
	}
	
	for ( std::vector<SVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
	{
		pTerrainMesh->LightMe(iter->dwVertexIndex);
	}

	for ( std::vector<SVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
	{
		pTerrainMesh->ReCalculateGridsHeight(iter->dwVertexIndex);
	}
}

void CCmdSetVertexHeight::AddVertex(DWORD dwVertexIndex)
{
	SVertex s;
	s.dwVertexIndex = dwVertexIndex;
	if ( find_if(m_vecDiffs.begin(),m_vecDiffs.end(),IsEqual(s)) != m_vecDiffs.end() )
	{
		return;
	}
	CTerrainMesh::SVertex & vertex = CXFile::GetInstance()->GetTerrainMesh()->GetVertex(dwVertexIndex);
	s.fHeight = vertex.vPosition.y;
	m_vecDiffs.push_back(s);
	m_dwVertexCnt = m_vecDiffs.size();
}

}
}