#include "stdafx.h"
#include "CPluginVertexAniMesh.h"
#include "CPluginDataSources.h"
#include "CVertexDeclaration.h"

namespace sqr
{
	CPluginVertexAniMesh::CPluginVertexAniMesh()
	{
		m_VertexType = VT_VERTEXANI;
	}

	CPluginVertexAniMesh::~CPluginVertexAniMesh()
	{
	}

	VERTEXTYPE& CPluginVertexAniMesh::GetMeshType()
	{
		return m_VertexType;
	}

	CPluginMeshLevel* CPluginVertexAniMesh::createMeshLevel()
	{
		m_MeshLevels.push_back( (CMeshLevel)CPluginMeshLevel() );
		return static_cast<CPluginMeshLevel*>(&m_MeshLevels.back());
	}

	CPluginMeshLevel* CPluginVertexAniMesh::GetPluginMeshLevel( size_t level )
	{
		return static_cast<CPluginMeshLevel*>(&m_MeshLevels[level]);
	}

	size_t CPluginVertexAniMesh::GetPluginMeshLevelNum()
	{
		return m_MeshLevels.size();
	}

	void CPluginVertexAniMesh::setBufVertexNum( int level, int vertNum )
	{

	}

	void CPluginVertexAniMesh::generatePosBuffer( int level, const vector<CVector3f>& posBuf )
	{

	}

	void CPluginVertexAniMesh::generateNormalBuffer( int level, const vector<CVector3f>& normBuf )
	{

	}

	void CPluginVertexAniMesh::generateTexCoordBuffer0( int level, const vector<CVector2f>& texBuf0 )
	{

	}

	void CPluginVertexAniMesh::generateTexCoordBuffer1( int level, const vector<CVector2f>& texBuf1 )
	{

	}

	void CPluginVertexAniMesh::generateTexCoordBuffer2( int level, const vector<CVector2f>& texBuf2 )
	{

	}

	void CPluginVertexAniMesh::generateSkeWeightBuffer( int level, const vector<CWeight>& skeWeightBuf )
	{

	}

	void CPluginVertexAniMesh::generateSkeIndexBuffer( int level, const vector<SkltIndex>& skeIndexBuf )
	{

	}

	float& CPluginVertexAniMesh::GetBaize()
	{
		return m_fBaize;
	}

	CVector3f& CPluginVertexAniMesh::GetScale()
	{
		return m_Scale;
	}

	CVector3f& CPluginVertexAniMesh::GetOff()
	{
		return m_Off;
	}

	CVector3f& CPluginVertexAniMesh::GetOrgScale()
	{
		return m_OrgScale;
	}

	CVector3f& CPluginVertexAniMesh::GetOrgOff()
	{
		return m_OrgOff;
	}

	float& CPluginVertexAniMesh::GetTexScale()
	{
		return m_fTextScale;
	}

	int& CPluginVertexAniMesh::GetChannel()
	{
		return m_Channel;
	}

	bool& CPluginVertexAniMesh::GetIsCompress()
	{
		return m_IsCompress;
	}
}
