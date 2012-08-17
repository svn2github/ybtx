#include "stdafx.h"
#include "CEditLogicData.h"
//---------------------------------
#include "TSqrAllocator.inl"
//---------------------------------
#include "TraceHelper.h"

CEditLogicData::CEditLogicData(CElementManager* pParent , URegionID id)
: CLogicData(pParent,id) 
{
	this->Init();
}

CEditLogicData::~CEditLogicData()
{
	m_aryVertexPosition.clear();
	m_aryVertexDiffuse.clear();
	m_aryVertexBakingColor.clear();
	m_aryVertexLogicHight.clear();
}

void CEditLogicData::Init()
{
	m_aryWaterHight.resize(SRegionSpanNum, 0.0f);
	m_aryLogicNormal.resize(SRegionSpanNum,CVector3f(0.0f,1.0f,0.0f));
	m_aryLogicHight.resize(SRegionSpanNum);
	m_aryBlockType.resize(SRegionSpanNum);
	m_MaterialIndex.resize(SRegionSpanNum);

	m_aryVertexPosition.resize(SRegionSpanNum);
	m_aryVertexDiffuse.resize(SRegionSpanNum);
	m_aryVertexBakingColor.resize(SRegionSpanNum);
	m_aryVertexLogicHight.resize(SRegionSpanNum, 0);
	m_aryGridHide.resize(SRegionSpanNum, false);

	for( int m = 0; m < nTextureLayerNum; ++m )
		m_aryGridTextureNames[m].resize(SRegionSpanNum);
}

void CEditLogicData::SetLogicNormal( uint32 index, const CVector3f& normal )
{
	if( index < 0 || index >= SRegionSpanNum )
	{
		MessageBox(0, "取地形数值索引越界", "提示", 0);
		return;
	}

	m_aryLogicNormal[index] = normal;
}

CVector3f CEditLogicData::GetLogicNormal( uint32 index ) const
{
	if( index < 0 || index >= SRegionSpanNum )
	{
		MessageBox(0, "取地形数值索引越界", "提示", 0);
		return CVector3f(0.0f, 1.0f, 0.0f);
	}

	return m_aryLogicNormal[index];
}

CVector3f CEditLogicData::GetLogicNormal(uint x, uint z) const
{
	if( x == eGridSpan )
		x = eGridSpan - 1;
	if( z == eGridSpan )
		z = eGridSpan - 1;

	int index = x + z * eGridSpan;
	if( index < 0 || index >= SRegionSpanNum )
	{
		MessageBox(0, "取地形数值索引越界", "提示", 0);
		return CVector3f(0.0f, 1.0f, 0.0f);
	}

	return m_aryLogicNormal[index];
}

void CEditLogicData::SetVertexPosition( uint32 index, const CVector3f& pos )
{
	if( index < 0 || index >= SRegionSpanNum )
	{
		MessageBox(0, "取地形数值索引越界", "提示", 0);
		return;
	}

	if( pos.x > 4096.0f )
		int tt = 0;

	m_aryVertexPosition[index] = pos;
	if( pos.y > 0.0f )
	{
		int x = index % 64;
		int z = index / 64;
		//DebugOut("index %d, x = %d, z = %d, %f\n", index, x, z, pos.y);
	}

	m_aryLogicHight[index] = pos.y + m_aryVertexLogicHight[index];
}

CVector3f CEditLogicData::GetVertexPosition( uint32 index ) const
{
	if( index < 0 || index >= SRegionSpanNum )
	{
		MessageBox(0, "取地形数值索引越界", "提示", 0);
		return CVector3f(0.0f, 0.0f, 0.0f);
	}
	
	return m_aryVertexPosition[index];
}

void CEditLogicData::SetVertexDiffuse( uint32 index, DWORD color )
{
	if( index < 0 || index >= SRegionSpanNum )
	{
		MessageBox(0, "取地形数值索引越界", "提示", 0);
		return;
	}

	m_aryVertexDiffuse[index] = color;
}

DWORD CEditLogicData::GetVertexDiffuse( uint32 index ) const
{
	if( index < 0 || index >= SRegionSpanNum )
	{
		MessageBox(0, "取地形数值索引越界", "提示", 0);
		return VERTEX_COLOR;
	}

	return m_aryVertexDiffuse[index];
}

void CEditLogicData::SetVertexBakingColor(uint32 index, DWORD color)
{
	if( index < 0 || index >= SRegionSpanNum )
	{
		MessageBox(0, "取地形数值索引越界", "提示", 0);
		return;
	}

	m_aryVertexBakingColor[index] = color;
}

DWORD CEditLogicData::GetVertexBakingColor(uint32 index) const
{
	if( index < 0 || index >= SRegionSpanNum )
	{
		MessageBox(0, "取地形数值索引越界", "提示", 0);
		return VERTEX_COLOR;
	}

	return m_aryVertexBakingColor[index];
}

void CEditLogicData::SetLogicHeight( uint32 index, const int nLogicHeight )
{
	if( index < 0 || index >= SRegionSpanNum )
	{
		MessageBox(0, "取地形数值索引越界", "提示", 0);
		return;
	}

	m_aryVertexLogicHight[index] = nLogicHeight;
	m_aryLogicHight[index] = m_aryVertexPosition[index].y + nLogicHeight;
}

int CEditLogicData::GetLogicHeight( uint32 index ) const
{
	if( index < 0 || index >= SRegionSpanNum )
	{
		MessageBox(0, "取地形数值索引越界", "提示", 0);
		return 0;
	}

	return m_aryVertexLogicHight[index];
}

void CEditLogicData::SetWaterHeight( uint32 index, const float fWaterHeight )
{
	if( index < 0 || index >= SRegionSpanNum )
	{
		MessageBox(0, "取地形数值索引越界", "提示", 0);
		return ;
	}

	m_aryWaterHight[index] = fWaterHeight;
}

float CEditLogicData::GetWaterHeight( uint32 index ) const
{
	if( index < 0 || index >= SRegionSpanNum )
	{
		MessageBox(0, "取地形数值索引越界", "提示", 0);
		return 0.0f;
	}

	return m_aryWaterHight[index];
}

void CEditLogicData::SetBlockType( uint32 index, int8 nBlockType )
{
	if( index < 0 || index >= SRegionSpanNum )
	{
		MessageBox(0, "取地形数值索引越界", "提示", 0);
		return;
	}

	m_aryBlockType[index] = nBlockType;
}

int8 CEditLogicData::GetBlockType( uint32 index )
{
	if( index < 0 || index >= SRegionSpanNum )
	{
		MessageBox(0, "取地形数值索引越界", "提示", 0);
		return 0;
	}

	return m_aryBlockType[index];
}

void CEditLogicData::SetMaterialIndex( uint32 index, int8 nMatIndex )
{
	if( index < 0 || index >= SRegionSpanNum )
	{
		MessageBox(0, "取地形数值索引越界", "提示", 0);
		return;
	}

	m_MaterialIndex[index] = nMatIndex;
}

int8 CEditLogicData::GetMaterialIndex( uint32 index )
{
	if( index < 0 || index >= SRegionSpanNum )
	{
		MessageBox(0, "取地形数值索引越界", "提示", 0);
		return -1;
	}

	return m_MaterialIndex[index];
}


void CEditLogicData::SetGridHideMark( uint32 index, bool bHide )
{
	if( index < 0 || index >= SRegionSpanNum )
	{
		MessageBox(0, "取地形数值索引越界", "提示", 0);
		return;
	}

	m_aryGridHide[index] = bHide;
}

bool CEditLogicData::GetGridHideMark( uint32 index )
{
	return m_aryGridHide[index];
}

void CEditLogicData::SetGridTextureName( uint32 index, int nLayer, const string& texname )
{
	if( index < 0 || index >= SRegionSpanNum )
	{
		MessageBox(0, "取地形数值索引越界", "提示", 0);
		return;
	}
	if( nLayer >= nTextureLayerNum )
	{
		MessageBox(0, "刷纹理的层数不对", "提示", 0);
		return;
	}

	m_aryGridTextureNames[nLayer][index] = texname;
}

string CEditLogicData::GetGridTextureName( uint32 index, int nLayer )
{
	if( index < 0 || index >= SRegionSpanNum )
	{
		MessageBox(0, "取地形数值索引越界", "提示", 0);
		return "";
	}
	if( nLayer >= nTextureLayerNum )
	{
		MessageBox(0, "刷纹理的层数不对", "提示", 0);
		return "";
	}

	return m_aryGridTextureNames[nLayer][index];
}