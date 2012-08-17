#include "stdafx.h"
#include "CMesh.h"
#include "CVertexDeclaration.h"
#include "BaseHelper.h"
#include "CGraphic.h"
#include "TSqrAllocator.inl"
namespace sqr
{

CMesh::CMesh()
{
	m_VertexType = VT_UNKNOW;
	m_Channel = 1;
	m_IsCompress = true;
	m_VertexBuf.vertexBufferBinding = new CVertexBufferBinding();
}

CMesh::~CMesh()
{
	//for( uint32 i = 0; i < m_MeshLevels.size(); i++ )
	//	delete m_MeshLevels[i];
	m_MeshLevels.clear();
	if (m_VertexBuf.vertexBufferBinding->GetBuffer(0))
		CHardwareBufferMgr::GetInst()->DestroyBuffer(m_VertexBuf.vertexBufferBinding->GetBuffer(0));
	GfkSafeDelete(m_VertexBuf.vertexBufferBinding);
}

void CMesh::_FreeRes(void)
{
	for( uint32 i = 0; i < m_MeshLevels.size(); i++ )
		m_MeshLevels[i]._FreeRes();
	CVertexHardwareBuffer* pBuffer = m_VertexBuf.vertexBufferBinding->GetBuffer(0);
	if (pBuffer)
		pBuffer->freeRes();
}

void CMesh::_Reclaim(void)
{
	for( uint32 i = 0; i < m_MeshLevels.size(); i++ )
		m_MeshLevels[i]._Reclaim();
	CVertexHardwareBuffer* pBuffer = m_VertexBuf.vertexBufferBinding->GetBuffer(0);
	if (pBuffer)
		pBuffer->updateRes();
}

CVector3f CMesh::GetVertex( int Num )
{
	CVertexHardwareBuffer* pVB = m_VertexBuf.vertexBufferBinding->GetBuffer(0);
	CVector3f RetVec;
	pVB->read(pVB->getVertexSize()*Num,sizeof(CVector3f),&RetVec);
	return RetVec;
}

//·µ»ØÃæÊý
int CMesh::GetFaceNum( int Level )
{
	return (int)m_MeshLevels[Level].GetFaceNum();
};

IndexData* CMesh::GetIndexBuf(int level)
{
	return m_MeshLevels[level].GetIndexData();
}

CVector3f CMesh::GetNormal( int32 nNum )
{
	CVertexHardwareBuffer* pVB = m_VertexBuf.vertexBufferBinding->GetBuffer(0);
	CVector3f RetVec;
	pVB->read(pVB->getVertexSize()*nNum+sizeof(CVector3f),sizeof(CVector3f),&RetVec);
	return RetVec;
}

CVector2f CMesh::GetVertexUV( int Num )
{
	CVertexHardwareBuffer* pVB = m_VertexBuf.vertexBufferBinding->GetBuffer(0);
	CVector2f RetVec;
	pVB->read(pVB->getVertexSize()*Num+sizeof(CVector3f)*2,sizeof(CVector2f),&RetVec);
	return RetVec;
}

FilterNode*  CMesh::Render( CRenderMatrix& ModelMatrix,FilterNode& FN )
{
	return (FilterNode*)CGraphic::GetInst()->Insert(&FN);
}

SaveInfo::SaveInfo( CVector3f Scale, CVector3f Off, int uTexStr, int vTexStr, float fTextScale, VERTEXTYPE eType ) : eVetexType( eType )
{
	int xOff = (int32)( Off.x*4 + 0.5f );
	int yOff = (int32)( Off.y*4 + 0.5f );
	int zOff = (int32)( Off.z*4 + 0.5f );
	int xScl = (int32)( Scale.x*4 + 0.5f );
	int yScl = (int32)( Scale.y*4 + 0.5f );
	int zScl = (int32)( Scale.z*4 + 0.5f );

	sPos = 0;
	sPos = max( sPos, abs( xScl ) );
	sPos = max( sPos, abs( yScl ) );
	sPos = max( sPos, abs( zScl ) );

	xStr = xOff - xScl/2;
	yStr = yOff - yScl/2;
	zStr = zOff - zScl/2;
	uStr = uTexStr;
	vStr = vTexStr;
	sTex = fTextScale;
}

SaveInfo::SaveInfo( CVector3f Scale, CVector3f Off, float fTextScale, VERTEXTYPE eType )
{
	short xOff = (short)( Off.x*4 + 0.5f );
	short yOff = (short)( Off.y*4 + 0.5f );
	short zOff = (short)( Off.z*4 + 0.5f );
	short xScl = (short)( Scale.x*4 + 0.5f );
	short yScl = (short)( Scale.y*4 + 0.5f );
	short zScl = (short)( Scale.z*4 + 0.5f );

	sPos = 0;
	sPos = max( sPos, abs( xScl ) );
	sPos = max( sPos, abs( yScl ) );
	sPos = max( sPos, abs( zScl ) );

	xStr = xOff - xScl/2;
	yStr = yOff - yScl/2;
	zStr = zOff - zScl/2;
	sTex = fTextScale;
	eVetexType = eType;
}

SaveInfo::SaveInfo( CVector3f Scale, CVector3f Off, CVector2f MinUV, CVector2f MaxUV )
{
	int xOff = (int32)( Off.x*4 + 0.5f );
	int yOff = (int32)( Off.y*4 + 0.5f );
	int zOff = (int32)( Off.z*4 + 0.5f );
	int xScl = (int32)( Scale.x*4 + 0.5f );
	int yScl = (int32)( Scale.y*4 + 0.5f );
	int zScl = (int32)( Scale.z*4 + 0.5f );

	sPos = 0;
	sPos = max( sPos, abs( xScl ) );
	sPos = max( sPos, abs( yScl ) );
	sPos = max( sPos, abs( zScl ) );

	xStr = xOff - xScl/2;
	yStr = yOff - yScl/2;
	zStr = zOff - zScl/2;

	uStr = MinUV.x >= 0 ? (int)abs( MinUV.x ) : -(int)abs( MinUV.x );
	if( uStr > MinUV.x )
		uStr--;
	vStr = MinUV.y >= 0 ? (int)abs( MinUV.y ) : -(int)abs( MinUV.y );
	if( vStr > MinUV.y )
		vStr--;

	sTex = 0;
	sTex = max( sTex, abs( MaxUV.x - uStr ) );
	sTex = max( sTex, abs( MaxUV.y - vStr ) );
}

}

