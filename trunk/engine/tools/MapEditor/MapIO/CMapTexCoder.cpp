#include "stdafx.h"
#include "CMapTexCoder.h"

BEGIN_VERSION_MAP(CMapTexCoder)
	ON_VERSION(1,(CCoder::PFN_DECODE)&CMapTexCoder::DeCode_Ver_1)
END_VERSION_MAP()

CMapTexCoder::CMapTexCoder(CMetaSceneClient* pScene):m_pScene(pScene){}
CMapTexCoder::~CMapTexCoder(){}

bool CMapTexCoder::DeCode_Ver_1(const CDataChunk& inData)
{	
	m_pScene->m_pTerrain->LoadTexture(inData);
	return false;
}