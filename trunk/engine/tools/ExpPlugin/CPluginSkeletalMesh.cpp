#include "stdafx.h"
#include "CPluginSkeletalMesh.h"
#include "CPluginMeshBase.h"
#include "CPluginDataSources.h"
#include "CVertexDeclaration.h"

namespace sqr
{
	CPluginSkeletalMesh::CPluginSkeletalMesh()
	{
		m_VertexType = VT_SKELETALANI;
	}

	CPluginSkeletalMesh::~CPluginSkeletalMesh()
	{
	}

	VERTEXTYPE& CPluginSkeletalMesh::GetMeshType()
	{
		return m_VertexType;
	}

	CPluginMeshLevel* CPluginSkeletalMesh::createMeshLevel()
	{
		m_MeshLevels.push_back( (CMeshLevel)CPluginMeshLevel() );
		return static_cast<CPluginMeshLevel*>(&m_MeshLevels.back());
	}

	CPluginMeshLevel* CPluginSkeletalMesh::GetPluginMeshLevel( size_t level )
	{
		return static_cast<CPluginMeshLevel*>(&m_MeshLevels[level]);
	}

	size_t CPluginSkeletalMesh::GetPluginMeshLevelNum()
	{
		return m_MeshLevels.size();
	}

	void CPluginSkeletalMesh::setBufVertexNum( int level, int vertNum )
	{
		m_VertexBuf.vertexStart			= 0;
		m_VertexBuf.vertexCount			= vertNum;
		m_VertexBuf.vertexDeclaration	= CPluginDataSources::GetInst()->NewVertexDeclaration(VertexSkeletalFormat::Format);
		CVertexHardwareBuffer* pVB = CPluginDataSources::GetInst()->NewVertexBuffer(
			sizeof(VertexSkeletalFormat), vertNum, HBU_WRITE_ONLY, 0, true, true);

		m_VertexBuf.vertexBufferBinding->SetBinding(0, pVB);

		m_OrgVertex.resize(vertNum);
	}

	void CPluginSkeletalMesh::generatePosBuffer( int level, const vector<CVector3f>& posBuf )
	{
		CVertexHardwareBuffer* pVB = m_VertexBuf.vertexBufferBinding->GetBuffer(0);
		VertexSkeletalFormat* pDest = (VertexSkeletalFormat*)pVB->lock(HBL_NORMAL);
		for ( size_t i = 0; i<posBuf.size(); ++i )
		{
			pDest[i].x = posBuf[i].x;
			pDest[i].y = posBuf[i].y;
			pDest[i].z = posBuf[i].z;
		}
		pVB->unlock();

		for ( size_t i = 0; i<m_OrgVertex.size(); ++i )
		{
			m_OrgVertex[i].x = posBuf[i].x;
			m_OrgVertex[i].y = posBuf[i].y;
			m_OrgVertex[i].z = posBuf[i].z;
		}
	}

	void CPluginSkeletalMesh::generateNormalBuffer( int level, const vector<CVector3f>& normBuf )
	{
		CVertexHardwareBuffer* pVB = m_VertexBuf.vertexBufferBinding->GetBuffer(0);
		VertexSkeletalFormat* pDest = (VertexSkeletalFormat*)pVB->lock(HBL_NORMAL);
		for ( size_t i = 0; i<normBuf.size(); ++i )
		{
			pDest[i].n	= normBuf[i].x;
			pDest[i].ny = normBuf[i].y;
			pDest[i].nz = normBuf[i].z;
		}
		pVB->unlock();

		for ( size_t i = 0; i<normBuf.size(); ++i )
		{
			m_OrgVertex[i].n  = normBuf[i].x;
			m_OrgVertex[i].ny = normBuf[i].y;
			m_OrgVertex[i].nz = normBuf[i].z;
		}
	}

	void CPluginSkeletalMesh::generateTexCoordBuffer0( int level, const vector<CVector2f>& texBuf0 )
	{
		CVertexHardwareBuffer* pVB = m_VertexBuf.vertexBufferBinding->GetBuffer(0);
		VertexSkeletalFormat* pDest = (VertexSkeletalFormat*)pVB->lock(HBL_NORMAL);
		for ( size_t i = 0; i<texBuf0.size(); ++i )
		{
			pDest[i].u	= texBuf0[i].x;
			pDest[i].v	= texBuf0[i].y;
		}
		pVB->unlock();

		for ( size_t i = 0; i<texBuf0.size(); ++i )
		{
			m_OrgVertex[i].u = texBuf0[i].x;
			m_OrgVertex[i].v = texBuf0[i].y;
		}
	}

	void CPluginSkeletalMesh::generateTexCoordBuffer1( int level, const vector<CVector2f>& texBuf1 )
	{
		CVertexHardwareBuffer* pVB = m_VertexBuf.vertexBufferBinding->GetBuffer(0);
		VertexSkeletalFormat* pDest = (VertexSkeletalFormat*)pVB->lock(HBL_NORMAL);
		for ( size_t i = 0; i<texBuf1.size(); ++i )
		{
			pDest[i].u1	= texBuf1[i].x;
			pDest[i].v1	= texBuf1[i].y;
		}
		pVB->unlock();

		for ( size_t i = 0; i<texBuf1.size(); ++i )
		{
			m_OrgVertex[i].u1	= texBuf1[i].x;
			m_OrgVertex[i].v1	= texBuf1[i].y;
		}
	}

	void CPluginSkeletalMesh::generateTexCoordBuffer2( int level, const vector<CVector2f>& texBuf2 )
	{
		CVertexHardwareBuffer* pVB = m_VertexBuf.vertexBufferBinding->GetBuffer(0);
		VertexSkeletalFormat* pDest = (VertexSkeletalFormat*)pVB->lock(HBL_NORMAL);
		for ( size_t i = 0; i<texBuf2.size(); ++i )
		{
			pDest[i].u2	= texBuf2[i].x;
			pDest[i].v2	= texBuf2[i].y;
		}
		pVB->unlock();

		for ( size_t i = 0; i<texBuf2.size(); ++i )
		{
			m_OrgVertex[i].u2	= texBuf2[i].x;
			m_OrgVertex[i].v2	= texBuf2[i].y;
		}
	}

	void CPluginSkeletalMesh::generateSkeWeightBuffer( int level, const vector<CWeight>& skeWeightBuf )
	{
		CVertexHardwareBuffer* pVB = m_VertexBuf.vertexBufferBinding->GetBuffer(0);
		VertexSkeletalFormat* pDest = (VertexSkeletalFormat*)pVB->lock(HBL_NORMAL);
		for ( size_t i = 0; i<skeWeightBuf.size(); ++i )
		{
			pDest[i].w = skeWeightBuf[i];
		}
		pVB->unlock();

		for ( size_t i = 0; i<skeWeightBuf.size(); ++i )
		{
			m_OrgVertex[i].w = skeWeightBuf[i];
		}
	}

	void CPluginSkeletalMesh::generateSkeIndexBuffer( int level, const vector<SkltIndex>& skeIndexBuf )
	{
		CVertexHardwareBuffer* pVB = m_VertexBuf.vertexBufferBinding->GetBuffer(0);
		VertexSkeletalFormat* pDest = (VertexSkeletalFormat*)pVB->lock(HBL_NORMAL);
		for ( size_t i = 0; i<skeIndexBuf.size(); ++i )
		{
			pDest[i].i = skeIndexBuf[i];
		}
		pVB->unlock();

		for ( size_t i = 0; i<skeIndexBuf.size(); ++i )
		{
			m_OrgVertex[i].i = skeIndexBuf[i];
		}
	}

	float& CPluginSkeletalMesh::GetBaize()
	{
		return m_fBaize;
	}

	CVector3f& CPluginSkeletalMesh::GetScale()
	{
		return m_Scale;
	}

	CVector3f& CPluginSkeletalMesh::GetOff()
	{
		return m_Off;
	}

	CVector3f& CPluginSkeletalMesh::GetOrgScale()
	{
		return m_OrgScale;
	}

	CVector3f& CPluginSkeletalMesh::GetOrgOff()
	{
		return m_OrgOff;
	}

	float& CPluginSkeletalMesh::GetTexScale()
	{
		return m_fTextScale;
	}

	int& CPluginSkeletalMesh::GetChannel()
	{
		return m_Channel;
	}

	bool& CPluginSkeletalMesh::GetIsCompress()
	{
		return m_IsCompress;
	}
}
