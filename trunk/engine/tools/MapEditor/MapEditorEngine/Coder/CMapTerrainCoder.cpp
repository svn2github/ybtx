#include "stdafx.h"
#include "CMapTerrainCoder.h"

BEGIN_VERSION_MAP(CMapTerrainCoder)
	ON_VERSION( 0 , DeCode_Ver_0 )
END_VERSION_MAP()

CMapTerrainCoder::CMapTerrainCoder(CDataScene* pScene)
: m_pDataScene(pScene)
{
	m_eSpri = SPRI_High;
}

bool CMapTerrainCoder::_CElementDeCode( const DeCodeParams& Params )
{
	return true;
}

bool CMapTerrainCoder::DeCode_Ver_0( const CDataChunk& inData,OBJ_HANDLE handle )
{
	return true;
}
