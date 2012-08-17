#include "stdafx.h"
#include "CEditLogicData.h"
#include "CEditLogicDataMgr.h"
#include "Brush/TerrainTextureData.h"

CElementNode* CEditLogicDataMgr::_CreateNode( URegionID ID )
{
	return new CEditLogicData(this, ID);
}

CEditLogicData*  CEditLogicDataMgr::GetEditLogicData(const URegionID& id)
{
	CEditLogicData* pLogicData = (CEditLogicData*)GetNode(id);
	return pLogicData;
}

CEditLogicData* CEditLogicDataMgr::GetEditLogicData( uint32 uX, uint32 uY )
{
	int16 GridX = uX/eSRegionSpan;
	int16 GridY = uY/eSRegionSpan;
	CEditLogicData* pLogicData = (CEditLogicData*)GetNode(URegionID(GridX,GridY));
	return pLogicData;
}

void CEditLogicDataMgr::SetVertexPosition(uint32 uX, uint32 uY, const CVector3f& vertex)
{
	int16 GridX = uX/eSRegionSpan;
	int16 GridY = uY/eSRegionSpan;
	CEditLogicData* pData = (CEditLogicData*)GetNode(URegionID(GridX,GridY));
	if(pData)
	{
		uint16 x = uX - GridX * eSRegionSpan;
		uint16 y = uY - GridY * eSRegionSpan;
		int index = x + y * eGridSpan;
		pData->SetVertexPosition(index, vertex);
	}
}

void CEditLogicDataMgr::SetVertexPositionHeight(uint32 uX, uint32 uY, const float fHeight)
{
	int16 GridX = uX/eSRegionSpan;
	int16 GridY = uY/eSRegionSpan;
	CEditLogicData* pData = (CEditLogicData*)GetNode(URegionID(GridX,GridY));
	if(pData)
	{
		uint16 x = uX - GridX * eSRegionSpan;
		uint16 y = uY - GridY * eSRegionSpan;
		int index = x + y * eGridSpan;
		CVector3f vertex = pData->GetVertexPosition(index);
		vertex.y = fHeight;
		pData->SetVertexPosition(index, vertex);
	}
}

CVector3f CEditLogicDataMgr::GetVertexPosition(uint32 uX, uint32 uY)
{
	int16 GridX = uX/eSRegionSpan;
	int16 GridY = uY/eSRegionSpan;
	CEditLogicData* pData = (CEditLogicData*)GetNode(URegionID(GridX,GridY));
	if(pData)
	{
		uint16 x = uX - GridX * eSRegionSpan;
		uint16 y = uY - GridY * eSRegionSpan;
		int index = x + y * eGridSpan;
		return pData->GetVertexPosition(index);
	}
	else
	{
		return CVector3f(0.0f, 0.0f, 0.0f);
	}
}

void CEditLogicDataMgr::SetVertexLogicNormal(uint32 uX, uint32 uY, const CVector3f& normal)
{
	int16 GridX = uX/eSRegionSpan;
	int16 GridY = uY/eSRegionSpan;
	CEditLogicData* pData = (CEditLogicData*)GetNode(URegionID(GridX,GridY));
	if(pData)
	{
		uint16 x = uX - GridX * eSRegionSpan;
		uint16 y = uY - GridY * eSRegionSpan;
		int index = x + y * eGridSpan;
		pData->SetLogicNormal(index, normal);
	}
}

CVector3f CEditLogicDataMgr::GetVertexLogicNormal(uint32 uX, uint32 uY) 
{
	int16 GridX = uX/eSRegionSpan;
	int16 GridY = uY/eSRegionSpan;
	CEditLogicData* pData = (CEditLogicData*)GetNode(URegionID(GridX,GridY));
	if(pData)
	{
		uint16 x = uX - GridX * eSRegionSpan;
		uint16 y = uY - GridY * eSRegionSpan;
		int index = x + y * eGridSpan;
		return pData->GetLogicNormal(index);
	}
	else
	{
		return CVector3f(0.0f, 1.0f, 0.0f);
	}
}

void CEditLogicDataMgr::SetVertexDiffuse(uint32 uX, uint32 uY, const DWORD color)
{
	int16 GridX = uX/eSRegionSpan;
	int16 GridY = uY/eSRegionSpan;
	CEditLogicData* pData = (CEditLogicData*)GetNode(URegionID(GridX,GridY));
	if(pData)
	{
		uint16 x = uX - GridX * eSRegionSpan;
		uint16 y = uY - GridY * eSRegionSpan;
		int index = x + y * eGridSpan;
		pData->SetVertexDiffuse(index, color);
	}
}

DWORD CEditLogicDataMgr::GetVertexDiffuse(uint32 uX, uint32 uY) 
{
	int16 GridX = uX/eSRegionSpan;
	int16 GridY = uY/eSRegionSpan;
	CEditLogicData* pData = (CEditLogicData*)GetNode(URegionID(GridX,GridY));
	if(pData)
	{
		uint16 x = uX - GridX * eSRegionSpan;
		uint16 y = uY - GridY * eSRegionSpan;
		int index = x + y * eGridSpan;
		return pData->GetVertexDiffuse(index);
	}
	else
	{
		return VERTEX_COLOR;
	}
}

void CEditLogicDataMgr::SetWaterHeight(uint32 uX, uint32 uY, const float fHeight)
{
	int16 GridX = uX/eSRegionSpan;
	int16 GridY = uY/eSRegionSpan;
	CEditLogicData* pData = (CEditLogicData*)GetNode(URegionID(GridX,GridY));
	if(pData)
	{
		uint16 x = uX - GridX * eSRegionSpan;
		uint16 y = uY - GridY * eSRegionSpan;
		int index = x + y * eGridSpan;
		pData->SetWaterHeight(index, fHeight);
	}
}

float CEditLogicDataMgr::GetWaterHeight(uint32 uX, uint32 uY)
{
	int16 GridX = uX/eSRegionSpan;
	int16 GridY = uY/eSRegionSpan;
	CEditLogicData* pData = (CEditLogicData*)GetNode(URegionID(GridX,GridY));
	if(pData)
	{
		uint16 x = uX - GridX * eSRegionSpan;
		uint16 y = uY - GridY * eSRegionSpan;
		int index = x + y * eGridSpan;
		return pData->GetWaterHeight(index);
	}
	else
	{
		return 0.0f;
	}
}

void CEditLogicDataMgr::SetBlockType(uint32 uX, uint32 uY, const int8 type)
{
	int16 GridX = uX/eSRegionSpan;
	int16 GridY = uY/eSRegionSpan;
	CEditLogicData* pData = (CEditLogicData*)GetNode(URegionID(GridX,GridY));
	if(pData)
	{
		uint16 x = uX - GridX * eSRegionSpan;
		uint16 y = uY - GridY * eSRegionSpan;
		int index = x + y * eGridSpan;
		pData->SetBlockType(index, type);
	}
}

int8 CEditLogicDataMgr::GetBlockType( uint32 uX, uint32 uY )
{
	int16 GridX = uX/eSRegionSpan;
	int16 GridY = uY/eSRegionSpan;
	CEditLogicData* pData = (CEditLogicData*)GetNode(URegionID(GridX,GridY));
	if(pData)
	{
		uint16 x = uX - GridX * eSRegionSpan;
		uint16 y = uY - GridY * eSRegionSpan;
		int index = x + y * eGridSpan;
		return pData->GetBlockType(index);
	}
	else
	{
		MessageBox(0, "鼠标当前所在的位置不在地图范围内", "提示", 0);
		return 0;
	}
}

void CEditLogicDataMgr::SetGridTextureName(uint32 uX, uint32 uY, int nLayer, const string& texname)
{
	int16 GridX = uX/eSRegionSpan;
	int16 GridY = uY/eSRegionSpan;
	CEditLogicData* pData = (CEditLogicData*)GetNode(URegionID(GridX,GridY));
	if(pData)
	{
		uint16 x = uX - GridX * eSRegionSpan;
		uint16 y = uY - GridY * eSRegionSpan;
		int index = x + y * eGridSpan;
		pData->SetGridTextureName(index, nLayer, texname);
	}
}

string CEditLogicDataMgr::GetGridTextureName(uint32 uX, uint32 uY, int nLayer)
{
	int16 GridX = uX/eSRegionSpan;
	int16 GridY = uY/eSRegionSpan;
	CEditLogicData* pData = (CEditLogicData*)GetNode(URegionID(GridX,GridY));
	if(pData)
	{
		uint16 x = uX - GridX * eSRegionSpan;
		uint16 y = uY - GridY * eSRegionSpan;
		int index = x + y * eGridSpan;
		return pData->GetGridTextureName(index, nLayer);
	}
	else
	{
		MessageBox(0, "鼠标当前所在的位置不在地图范围内", "提示", 0);
		return "";
	}
}

void CEditLogicDataMgr::SetTextureData(CTerrainTextureData* pTerrainTextureData)
{
	m_pTerrainTextureData = pTerrainTextureData;
}

int CEditLogicDataMgr::GetTextureIndex(const string& texname)
{
	return m_pTerrainTextureData->GetIndexByTextureName(texname);
}