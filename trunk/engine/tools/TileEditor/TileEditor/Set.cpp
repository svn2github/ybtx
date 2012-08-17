#include "stdafx.h"
#include "set.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CResSet::CResSet()
{
}

CResSet::~CResSet()
{
}

C2DResSetUnit::C2DResSetUnit()
{
	m_eTransType = CTerrainMesh::CTextureNode::ETT_VERTEX;
}

CTerrainMesh::CTextureNode::ETRANS_TYPE C2DResSetUnit::GetTransType()
{
	return m_eTransType;
}

void C2DResSetUnit::SetTransType(CTerrainMesh::CTextureNode::ETRANS_TYPE e)
{
	m_eTransType = e;
}

void C3DResSetUnit::SetCenterType(int eCenterType)
{
	m_eCenterType = eCenterType;    
}

int C3DResSetUnit::GetCenterType()
{
	return m_eCenterType;    
}

C3DResSetUnit::C3DResSetUnit()
{
	m_eCenterType = CTileEditorModel::ECT_NONE;
	m_fLift = m_fShiftZ = m_fShiftX = 0.0f;
	m_eTransType = CFace::ETT_TRANSLUCENT;
	m_pEffectGroup = NULL;
	m_bShadow = true;
}

C3DResSetUnit::~C3DResSetUnit()
{
}

float C3DResSetUnit::GetLift()
{
	return m_fLift;
}

float C3DResSetUnit::GetShiftX()
{
	return m_fShiftX;
}

float C3DResSetUnit::GetShiftZ()
{
	return m_fShiftZ;
}

void C3DResSetUnit::SetLift(float f)
{
	m_fLift = f;
}

void C3DResSetUnit::SetShiftX(float f)
{
	m_fShiftX = f;
}

void C3DResSetUnit::SetShiftZ(float f)
{
	m_fShiftZ = f;
}

int C3DResSetUnit::GetModelBlockSize(int n)
{
	return m_vvBlockInfo[n].size();
}

int C3DResSetUnit::GetModelHeightSize(int n)
{
	return m_vvLogicHeightInfo[n].size();
}

void C3DResSetUnit::SetModelBlockSize(int nModelIndex, int nSize)
{
	m_vvBlockInfo[nModelIndex].resize(nSize,CTileEditorModel::SBlockInfo());
}

void C3DResSetUnit::SetTotalBlockSize(int nSize)
{
	m_vvBlockInfo.resize(nSize);
}

void C3DResSetUnit::SetTotalHeightSize(int nSize)
{
	m_vvLogicHeightInfo.resize(nSize);
}

void C3DResSetUnit::SetModelHeightSize(int nModelIndex, int nSize)
{
	m_vvLogicHeightInfo[nModelIndex].resize(nSize,CTileEditorModel::SLogicHeightInfo());
}

void C3DResSetUnit::FlipModelBlockMask(int nModelIndex, int nPosition)
{
	m_vvBlockInfo[nModelIndex][nPosition].nBlockMask = 1 - m_vvBlockInfo[nModelIndex][nPosition].nBlockMask;
}

void C3DResSetUnit::SetModelBlockType(int nModelIndex, int nPosition, int nBlockType)
{
	m_vvBlockInfo[nModelIndex][nPosition].nBlockType = nBlockType;
}

void C3DResSetUnit::SetModelLogicHeight(int nModelIndex, int nPosition, float fLogicHeight)
{
	m_vvLogicHeightInfo[nModelIndex][nPosition].fLogicHeight = fLogicHeight;
}

std::vector< std::vector<CTileEditorModel::SBlockInfo> > & C3DResSetUnit::GetTotalBlock()
{
	return m_vvBlockInfo;
}

std::vector< std::vector<CTileEditorModel::SLogicHeightInfo> > & C3DResSetUnit::GetTotalHeight()
{
	return m_vvLogicHeightInfo;
}

std::vector<CTileEditorModel::SBlockInfo> & C3DResSetUnit::GetModelBlock(int n)
{
	return m_vvBlockInfo[n];
}

std::vector<CTileEditorModel::SLogicHeightInfo> & C3DResSetUnit::GetModelHeight(int n)
{
	return m_vvLogicHeightInfo[n];
}

void C3DResSetUnit::SetModelBlockMask(int nModelIndex, int nPosition, int nMask)
{
	m_vvBlockInfo[nModelIndex][nPosition].nBlockMask = nMask;
}

void C3DResSetUnit::SetTransType(CFace::ETrans_Type e)
{
	m_eTransType = e;
}

CFace::ETrans_Type C3DResSetUnit::GetTransType()
{
	return m_eTransType;
}

void C3DResSetUnit::SetAnimationByAraFileName(std::string strAraFileName)
{
	m_strAraFileName = strAraFileName;
}

std::string & C3DResSetUnit::GetAraFileName()
{
	return m_strAraFileName;
}

void C3DResSetUnit::SetShadow(bool b)
{
	m_bShadow = b;
}

bool C3DResSetUnit::GetShadow()
{
	return m_bShadow;
}