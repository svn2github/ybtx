//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Face.h"
#include "../TileEditorEngine/Console.h"
#include "CTileEditorModel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

namespace bg
{
namespace sc
{

CFace::CFace() 
{
	m_eTransType = ETT_TRANSLUCENT;
}

CFace::~CFace()
{
	for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),mem_fun(&CTileEditorModel::Remove));
}

void CFace::AddContainer(std::vector<CTileEditorModel *> & vec, bool bSubUnit, CTileEditorModel::ECenter_Type eBlockType, int nXDelta, int nZDelta)
{
	m_vecSelfPresentation = vec;

	m_bSubUnit = bSubUnit;
	
	D3DXVECTOR3 vecOffsetOverall(0.0f,0.0f,0.0f);
	
	if ( vec.size() > 1 )
	{
		
		float fXMin = FLT_MAX;
		float fXMax = -FLT_MAX;
		float fZMin = FLT_MAX;
		float fZMax = -FLT_MAX;
		
		for ( std::vector<CTileEditorModel*>::iterator iter = m_vecSelfPresentation.begin(); iter != m_vecSelfPresentation.end(); ++iter )
		{
			CTileEditorModel * p = (*iter);

			float fXMinLocal = p->GetOffset().x - p->GetScale().x/2.0f; 
			float fXMaxLocal = p->GetOffset().x + p->GetScale().x/2.0f; 
			float fZMinLocal = p->GetOffset().z - p->GetScale().z/2.0f; 
			float fZMaxLocal = p->GetOffset().z + p->GetScale().z/2.0f;

			if ( fXMinLocal < fXMin )
			{
				fXMin = fXMinLocal;
			}

			if ( fXMaxLocal > fXMax )
			{
				fXMax = fXMaxLocal;
			}

			if ( fZMinLocal < fZMin )
			{
				fZMin = fZMinLocal;
			}

			if ( fZMaxLocal > fZMax )
			{
				fZMax = fZMaxLocal;
			}
		}
		
		float fCenterX = ( fXMin + fXMax ) / 2.0f; 
		float fCenterZ = ( fZMin + fZMax ) / 2.0f; 
		
		vecOffsetOverall.x = fCenterX;
		vecOffsetOverall.z = fCenterZ;
	}
	
	vecOffsetOverall.x += nXDelta*GRID_SPACE;
	vecOffsetOverall.z += nZDelta*GRID_SPACE;
	
	for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
		bind2nd(mem_fun(&CTileEditorModel::SetOffsetOverall),vecOffsetOverall) ); 
	
	for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
		bind2nd(mem_fun(&CTileEditorModel::SetCenterTypeOrigin),eBlockType) ); 
}

CTileEditorModel * CFace::GetSelfPresentation(int n) 
{
	return m_vecSelfPresentation[n];
}

int CFace::GetSelfPresentationCount() 
{
	return m_vecSelfPresentation.size();
}

void CFace::SetModelFileNames(std::vector<std::string> & vec)
{
	m_veCTileEditorModelFileNames = vec;
}

std::vector<std::string> & CFace::GetModelFileNames()
{
	return m_veCTileEditorModelFileNames;
}

void CFace::SetEditState(CTileEditorModel::EEdit_State e)
{
	for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
		bind2nd(mem_fun(&CTileEditorModel::SetEditState),e) ); 
} 

void CFace::SetPlacedEditState(CTileEditorModel::EEdit_State_Placed e)
{
	for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
		bind2nd(mem_fun(&CTileEditorModel::SetPlacedEditState),e) ); 
} 

void CFace::SetLocked(bool b)
{
	for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
		bind2nd(mem_fun(&CTileEditorModel::SetLocked),b) ); 
} 

void CFace::SetCenterGridIndex(int n)
{
	for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
		bind2nd(mem_fun(&CTileEditorModel::SetCenterGridIndex),n) ); 
}

void CFace::CleanBlockGridIndices()
{
	for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),mem_fun(&CTileEditorModel::CleanBlockGridIndices));
}

void CFace::InitPlaced()
{
	for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),mem_fun(&CTileEditorModel::InitPlaced));
}

void CFace::UpdateLift(int n)
{
	for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
		bind2nd(mem_fun(&CTileEditorModel::UpdateLift),n) );
}

void CFace::UpdateShiftX(int n)
{
	for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
		bind2nd(mem_fun(&CTileEditorModel::UpdateShiftX),n) );
}

void CFace::UpdateShiftZ(int n)
{
	for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
		bind2nd(mem_fun(&CTileEditorModel::UpdateShiftZ),n) );
}

void CFace::UpdateRotation()
{
	for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),mem_fun(&CTileEditorModel::UpdateRotation) );
}

void CFace::Render()
{
	for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),mem_fun(&CTileEditorModel::Render) );
}

void CFace::SetSelected(bool b)
{
	for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
		bind2nd(mem_fun(&CTileEditorModel::SetSelected),b) ); 
}

bool CFace::Intersected(D3DXVECTOR3 vDir,D3DXVECTOR3 vPos)
{
	bool b = false;
	
	for ( std::vector<CTileEditorModel*>::iterator iter = m_vecSelfPresentation.begin(); iter != m_vecSelfPresentation.end(); ++iter )
	{ 
		b = b||(*iter)->Intersected(vDir,vPos);
	}
	
	return b;
}

bool CFace::GetIsSubUnit()
{
	return m_bSubUnit;
}

CTileEditorModel::ECenter_Type CFace::GetBlockType()
{
	return m_eCenterType;
}

void CFace::SetCenterType(CTileEditorModel::ECenter_Type e)
{
	for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
		bind2nd(mem_fun(&CTileEditorModel::SetCenterTypeOrigin),e) ); 
}

void CFace::SetHeightBoxesVisibility(bool bIsVisible)
{
	for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
		bind2nd(mem_fun(&CTileEditorModel::SetHeightBoxesVisibility),bIsVisible) );
}

void CFace::SetTransType(ETrans_Type e)
{
	m_eTransType = e;
}

void CFace::SetAnimationByAraFileName(std::string strAraFileName)
{
	for_each(m_vecSelfPresentation.begin(),m_vecSelfPresentation.end(),
		bind2nd(mem_fun(&CTileEditorModel::SetAnimationByAraFileName),strAraFileName) );
}

void CFace::LinkEffect(IEffect * pEffect, IEffectGroup * pGroup)
{
	m_vecSelfPresentation[0]->LinkEffect(pEffect,pGroup);
}

void CFace::UnlinkEffect()
{
	m_vecSelfPresentation[0]->UnlinkEffect();
}

}
}