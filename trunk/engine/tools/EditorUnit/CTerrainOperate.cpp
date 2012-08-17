#include "StdAfx.h"
#include "CTerrainOperate.h"
#include "CEditLogicDataMgr.h"

CTerrainOperate::CTerrainOperate(void)
{
	m_LogicDataMgr=NULL;
}

CTerrainOperate::~CTerrainOperate(void)
{
}
bool CTerrainOperate::IsNeighborPoint(int p1,int p2,int w)
{
	if(abs(p1%w-p2%w)<=1)
		return true;
	return false;
}

CVector3f CTerrainOperate::CalculateVertexNormal(uint32 uX, uint32 uY,int widthGrid,int heightGrid)
{
	if ( uX<=0 || uY<=0 || (int)uX>=widthGrid || (int)uY>=heightGrid )
	{
		return CVector3f(0.0f,1.0f,0.0f);
	}
	// Slope法线生成算法
	const int NEIGHBOR_NUM = 4;	// 左,上,右,下
	CVector2I neighborIndex[NEIGHBOR_NUM];
	neighborIndex[0].x=uX-1;	neighborIndex[0].y=uY;
	neighborIndex[1].x=uX;		neighborIndex[1].y=uY+1;
	neighborIndex[2].x=uX+1;	neighborIndex[2].y=uY;
	neighborIndex[3].x=uX;		neighborIndex[3].y=uY-1;

	CVector3f neighborVertex[NEIGHBOR_NUM];
	for (int i = 0; i < NEIGHBOR_NUM; i++)
	{
		neighborVertex[i] =m_LogicDataMgr->GetVertexPosition(neighborIndex[i].x,neighborIndex[i].y);
	}
	CVector3f normal(neighborVertex[0].y - neighborVertex[2].y, eSGridSpan * 2.0f, neighborVertex[3].y - neighborVertex[1].y);
	normal.normalize();
	m_LogicDataMgr->SetVertexLogicNormal(uX,uY,normal);
	return normal;
}

float CTerrainOperate::SmoothVertexHeight(uint32 uX, uint32 uY,int widthGrid,int heightGrid,float fStrength)
{
	if(!m_LogicDataMgr)
		return 0.0f;
	//点数
	int num = 0;
	float fTotal = 0.0f;
	//九宫格遍历法
	for (int i = uX-1; i <= int(uX+1); i ++)
	{
		for (int j = uY-1; j <= int(uY+1); j++)
		{
			if(i<0 || i>(widthGrid+1) || j<0 && j> (heightGrid+1) )
				continue;
			if( !IsNeighborPoint(i,uX,(widthGrid+1) ) || ( !IsNeighborPoint(j,uY,(widthGrid+1)) ) )
				continue;
			CVector3f &vertex = m_LogicDataMgr->GetVertexPosition(i,j);
			fTotal += vertex.y;
			++num;
		}
	}
	if(num!=0)
	{
		CVector3f &vertex = m_LogicDataMgr->GetVertexPosition(uX,uY);
		vertex.y = vertex.y + (fTotal / num - vertex.y) * fStrength / 32;
		m_LogicDataMgr->SetVertexPosition(uX, uY, vertex);
		return vertex.y;
	}
	return 0.0f;
}
float CTerrainOperate::AlignVertexHeight(uint32 uX, uint32 uY, float fMinY)
{
	if(!m_LogicDataMgr)
		return 0.0f;
	CVector3f &vertex = m_LogicDataMgr->GetVertexPosition(uX,uY);
	vertex.y = fMinY;
	m_LogicDataMgr->SetVertexPosition(uX, uY, vertex);
	return vertex.y;
}
float CTerrainOperate::CulGaussVertexHeight(uint32 uX, uint32 uY, uint32 uHotX, uint32 uHotY, const DWORD dwBrushWidth, const DWORD dwBrushDepth, const float fStrength,bool isInc)
{
	if(!m_LogicDataMgr)
		return 0.0f;
	CVector3f &vertex			= m_LogicDataMgr->GetVertexPosition(uX, uY);
  	const CVector3f &vertexHot	= m_LogicDataMgr->GetVertexPosition(uHotX, uHotY);
  
  	float distanceZ = vertex.z - ( vertexHot.z + 0.5f );
  	float distanceX = vertex.x - ( vertexHot.x + 0.5f );
  
  	float k,r;
  	DWORD a = dwBrushWidth * eSGridSpan;
  	DWORD b = dwBrushDepth * eSGridSpan;
  
  	k = distanceX/distanceZ;
  	r = ((a*a*b*b)/(k*k*b*b+a*a))*(1+k*k);
  	r = sqrtf((distanceX*distanceX+distanceZ*distanceZ)/r);
	if(isInc)
		vertex.y += fStrength*250*expf(-r*r*3)/100;
	else
		vertex.y -= fStrength*250*expf(-r*r*3)/100;

	m_LogicDataMgr->SetVertexPosition(uX, uY, vertex);
	return vertex.y;
}

float CTerrainOperate::CulLineVertexHeightX(uint32 uX, uint32 uY, uint32 uHotX, uint32 uHotY, const DWORD dwBrushWidth, const DWORD dwBrushDepth, const float fStrength,bool isInc)
{
	if(!m_LogicDataMgr)
		return 0.0f;
	CVector3f &vertex			= m_LogicDataMgr->GetVertexPosition(uX, uY);
  	const CVector3f &vertexHot	= m_LogicDataMgr->GetVertexPosition(uHotX, uHotY);
	float distance = abs(vertex.x - vertexHot.x);
	float radio = 1.0f - distance/(eSGridSpan * dwBrushWidth);
	if(isInc)
		vertex.y += radio*fStrength;
	else
		vertex.y -= radio*fStrength;
	m_LogicDataMgr->SetVertexPosition(uX, uY, vertex);
	return vertex.y;
}
float CTerrainOperate::CulLineVertexHeightZ(uint32 uX, uint32 uY, uint32 uHotX, uint32 uHotY, const DWORD dwBrushWidth, const DWORD dwBrushDepth, const float fStrength,bool isInc)
{
	if(!m_LogicDataMgr)
		return 0.0f;
	CVector3f &vertex			= m_LogicDataMgr->GetVertexPosition(uX, uY);
  	const CVector3f &vertexHot	= m_LogicDataMgr->GetVertexPosition(uHotX, uHotY);
	float distance = abs(vertex.z - vertexHot.z);
	float radio = 1.0f - distance/(eSGridSpan * dwBrushDepth);
	if(isInc)
		vertex.y += radio*fStrength;
	else
		vertex.y -= radio*fStrength;
	m_LogicDataMgr->SetVertexPosition(uX, uY, vertex);
	return vertex.y;
}
float CTerrainOperate::CulLineVertexHeightB(uint32 uX, uint32 uY, uint32 uHotX, uint32 uHotY, const DWORD dwBrushWidth, const DWORD dwBrushDepth, const float fStrength,bool isInc)
{
	if(!m_LogicDataMgr)
		return 0.0f;
	CVector3f &vertex			= m_LogicDataMgr->GetVertexPosition(uX, uY);
  	const CVector3f &vertexHot	= m_LogicDataMgr->GetVertexPosition(uHotX, uHotY);
	float distanceZ = fabs(vertex.z - vertexHot.z );
	float distanceX = fabs(vertex.x - vertexHot.x );
	float radio, radio1, radio2;
	if ( dwBrushWidth > dwBrushDepth )
	{
		radio1 = (dwBrushWidth*eSGridSpan - distanceX) / (eSGridSpan*dwBrushDepth);
		radio2 = (dwBrushDepth*eSGridSpan - distanceZ) / (eSGridSpan*dwBrushDepth);
		radio = radio1 > radio2 ? radio2 : radio1;
	}
	else
	{
		radio1 = (dwBrushWidth*eSGridSpan - distanceX) / (eSGridSpan*dwBrushWidth);
		radio2 = (dwBrushDepth*eSGridSpan - distanceZ) / (eSGridSpan*dwBrushWidth);
		radio = radio1 > radio2 ? radio2 : radio1;
	}
	if(isInc)
		vertex.y += fStrength*radio;
	else
		vertex.y -= fStrength*radio;
	m_LogicDataMgr->SetVertexPosition(uX, uY, vertex);
	return vertex.y;
}

//

//
//float radio, radio1, radio2;
//
//if ( dwBrushWidth > dwBrushDepth )
//{
//	radio1 = (dwBrushWidth*GRID_SPACE - distanceX) / (GRID_SPACE*dwBrushDepth);
//	radio2 = (dwBrushDepth*GRID_SPACE - distanceZ) / (GRID_SPACE*dwBrushDepth);
//	radio = radio1 > radio2 ? radio2 : radio1;
//}
//else
//{
//	radio1 = (dwBrushWidth*GRID_SPACE - distanceX) / (GRID_SPACE*dwBrushWidth);
//	radio2 = (dwBrushDepth*GRID_SPACE - distanceZ) / (GRID_SPACE*dwBrushWidth);
//	radio = radio1 > radio2 ? radio2 : radio1;
//}
//
//float fHeightStrength = CToolSetTileHeight::GetInst()->GetHeightStrength();
//vertex.vPosition.y -= fHeightStrength*radio;