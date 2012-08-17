#include "stdafx.h"
#include "CPathTransformer.h"
#include "CPixelPath.h"
#include <deque>
#include "CPos.h"
#include "IPathFinding.h"
#include "TSqrAllocator.inl"
#include "CoreCommon.h"
#include "ExpHelper.h"

CPathTransformer::CPathTransformer(const CPixelPath* pPath)
:m_uDataLen(0)
{
	m_pPathData = Transform(pPath);
}

CPathTransformer::CPathTransformer(const vector<CFPos>& vecPos)
:m_uDataLen(0)
{
	m_pPathData = Transform(vecPos);
}

CPathTransformer::~CPathTransformer()
{
	delete m_pPathData;
	m_pPathData = NULL;
}

void CPathTransformer::GridPos32To16(const int32& x32, const int32& y32, int16& x16, int16& y16)const
{
	x16 = int16(x32);
	y16 = int16(y32);

	if(int32(x16) != x32 || int32(y16) != y32)
	{
		ostringstream strm;
		strm << x32 << "," << y32 << endl;
		GenErr("GridPos is out of boundry", strm.str());
	}
}

const int16* CPathTransformer::Transform(const CPixelPath* pPath)
{
	const PathList& deqPathList = pPath->GetGridPath()->GetPath();

	if(deqPathList.size() < 2)
		GenErr("Invalid Grid Path: the waypoint num is less than 2");

	if(deqPathList.size() == 2)	//只有起点和终点
		return NULL;

	uint16 uSize = uint16(deqPathList.size() - 2);
	int16* pData = new int16[uSize * 2];
	for(uint16 i = 0; i < uSize; ++i)
	{
		int32 x32 = deqPathList[i+1].x;
		int32 y32 = deqPathList[i+1].y;
		int16 x16;
		int16 y16;
		GridPos32To16(x32, y32, x16, y16);
		pData[2*i] = x16;
		pData[2*i+1] = y16;
		//cout << "Waypoint No. " << i << " " << pData[2*i] << " " << pData[2*i+1] << endl;
	}
	uint32 uDataLen = uint32(uSize * 2 * sizeof(int16));
	m_uDataLen = uint16(uDataLen);
	if(uint32(m_uDataLen) != uDataLen)
	{
		ostringstream strm;
		strm << uDataLen << endl;
		GenErr("PathData is too big", strm.str());
	}
	return pData;
}

const int16* CPathTransformer::Transform(const vector<CFPos>& vecPos)
{
	if(vecPos.size() < 2)
		GenErr("Invalid Grid Path: the waypoint num is less than 2");

	if(vecPos.size() == 2)	//只有起点和终点
		return NULL;

	uint16 uSize = uint16(vecPos.size() - 2);
	int16* pData = new int16[uSize * 2];
	for(uint16 i = 0; i < uSize; ++i)
	{
		int32 x32 = int32(vecPos[i+1].x / eGridSpan * 2);	//四分之一格坐标
		int32 y32 = int32(vecPos[i+1].y / eGridSpan * 2);
		int16 x16;
		int16 y16;
		GridPos32To16(x32, y32, x16, y16);
		pData[2*i] = x16;
		pData[2*i+1] = y16;
		//cout << "Waypoint No. " << i << " " << pData[2*i] << " " << pData[2*i+1] << endl;
	}
	uint32 uDataLen = uint32(uSize * 2 * sizeof(int16));
	m_uDataLen = uint16(uDataLen);

	if(uint32(m_uDataLen) != uDataLen)
	{
		ostringstream strm;
		strm << uDataLen << endl;
		GenErr("PathData is too big", strm.str());
	}
	return pData;
}

const int16* CPathTransformer::GetData()const
{
	return m_pPathData;
}

uint16 CPathTransformer::GetDataLenInByte()const
{
	return m_uDataLen;
}

void CPathTransformer::PrintPath()const
{
	uint16 uSize = m_uDataLen / (sizeof(int16) * 2);

	for(uint16 u = 0; u < uSize; ++u)
	{
		cout << m_pPathData[u*2] << "," << m_pPathData[u*2+1] << "->";
	}
	cout << endl;
}
