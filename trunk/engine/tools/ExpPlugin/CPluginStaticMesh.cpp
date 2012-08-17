#include "stdafx.h"
#include "CPluginStaticMesh.h"
#include "CPluginDataSources.h"
#include "CVertexDeclaration.h"

namespace sqr
{
	CPluginStaticMesh::CPluginStaticMesh()
	{
		m_VertexType = VT_STATICVERTEX;
	}

	CPluginStaticMesh::~CPluginStaticMesh()
	{
	}

	VERTEXTYPE& CPluginStaticMesh::GetMeshType()
	{
		return m_VertexType;
	}

	CPluginMeshLevel* CPluginStaticMesh::createMeshLevel()
	{
		m_MeshLevels.push_back( (CMeshLevel)CPluginMeshLevel() );
		return static_cast<CPluginMeshLevel*>(&m_MeshLevels.back());
	}

	CPluginMeshLevel* CPluginStaticMesh::GetPluginMeshLevel( size_t level )
	{
		return static_cast<CPluginMeshLevel*>(&m_MeshLevels[level]);
	}

	size_t CPluginStaticMesh::GetPluginMeshLevelNum()
	{
		return m_MeshLevels.size();
	}

	void CPluginStaticMesh::setBufVertexNum( int level, int vertNum )
	{
		m_VertexBuf.vertexStart			= 0;
		m_VertexBuf.vertexCount			= vertNum;

		DWORD fmt = 0;
		size_t nVertSize = 0;
		switch ( m_Channel )
		{
		case 0:
		case 1:
			fmt = VerNorTex::Format;
			nVertSize = sizeof(VerNorTex);
			break;

		case 2:
			fmt = VerNorTex1::Format;
			nVertSize = sizeof(VerNorTex1);
			break;

		default:
			fmt = VerNorTex2::Format;
			nVertSize = sizeof(VerNorTex2);
			break;
		}

		m_VertexBuf.vertexDeclaration	= CPluginDataSources::GetInst()->NewVertexDeclaration(fmt);
		CVertexHardwareBuffer* pVB = CPluginDataSources::GetInst()->NewVertexBuffer(
			nVertSize, vertNum, HBU_WRITE_ONLY, 0, true, true);

		m_VertexBuf.vertexBufferBinding->SetBinding(0, pVB);
	}

	void CPluginStaticMesh::generatePosBuffer( int level, const vector<CVector3f>& posBuf )
	{
		CVertexHardwareBuffer* pVB = m_VertexBuf.vertexBufferBinding->GetBuffer(0);
		switch ( m_Channel )
		{
		case 0:
		case 1:
			{
				VerNorTex* pDest = (VerNorTex*)pVB->lock(HBL_NORMAL);
				for ( size_t i = 0; i<posBuf.size(); ++i )
				{
					pDest[i].p.x = posBuf[i].x;
					pDest[i].p.y = posBuf[i].y;
					pDest[i].p.z = posBuf[i].z;
				}
				pVB->unlock();
			}
			break;

		case 2:
			{
				VerNorTex1* pDest = (VerNorTex1*)pVB->lock(HBL_NORMAL);
				for ( size_t i = 0; i<posBuf.size(); ++i )
				{
					pDest[i].p.x = posBuf[i].x;
					pDest[i].p.y = posBuf[i].y;
					pDest[i].p.z = posBuf[i].z;
				}
				pVB->unlock();
			}
			break;

		default:
			{
				VerNorTex2* pDest = (VerNorTex2*)pVB->lock(HBL_NORMAL);
				for ( size_t i = 0; i<posBuf.size(); ++i )
				{
					pDest[i].p.x = posBuf[i].x;
					pDest[i].p.y = posBuf[i].y;
					pDest[i].p.z = posBuf[i].z;
				}
				pVB->unlock();
			}
			break;
		}
	}

	void CPluginStaticMesh::generateNormalBuffer( int level, const vector<CVector3f>& normBuf )
	{
		CVertexHardwareBuffer* pVB = m_VertexBuf.vertexBufferBinding->GetBuffer(0);
		switch ( m_Channel )
		{
		case 0:
		case 1:
			{
				VerNorTex* pDest = (VerNorTex*)pVB->lock(HBL_NORMAL);
				for ( size_t i = 0; i<normBuf.size(); ++i )
				{
					pDest[i].np.x = normBuf[i].x;
					pDest[i].np.y = normBuf[i].y;
					pDest[i].np.z = normBuf[i].z;
				}
				pVB->unlock();
			}
			break;

		case 2:
			{
				VerNorTex1* pDest = (VerNorTex1*)pVB->lock(HBL_NORMAL);
				for ( size_t i = 0; i<normBuf.size(); ++i )
				{
					pDest[i].np.x = normBuf[i].x;
					pDest[i].np.y = normBuf[i].y;
					pDest[i].np.z = normBuf[i].z;
				}
				pVB->unlock();
			}
			break;

		default:
			{
				VerNorTex2* pDest = (VerNorTex2*)pVB->lock(HBL_NORMAL);
				for ( size_t i = 0; i<normBuf.size(); ++i )
				{
					pDest[i].np.x = normBuf[i].x;
					pDest[i].np.y = normBuf[i].y;
					pDest[i].np.z = normBuf[i].z;
				}
				pVB->unlock();
			}
			break;
		}
	}

	void CPluginStaticMesh::generateTexCoordBuffer0( int level, const vector<CVector2f>& texBuf0 )
	{
		CVertexHardwareBuffer* pVB = m_VertexBuf.vertexBufferBinding->GetBuffer(0);
		if ( m_Channel == 0 || m_Channel == 1 )
		{
			VerNorTex* pDest = (VerNorTex*)pVB->lock(HBL_NORMAL);
			for ( size_t i = 0; i<texBuf0.size(); ++i )
			{
				pDest[i].tu = texBuf0[i].x;
				pDest[i].tv = texBuf0[i].y;
			}
			pVB->unlock();
		}
		else if ( m_Channel == 2 )
		{
			VerNorTex1* pDest = (VerNorTex1*)pVB->lock(HBL_NORMAL);
			for ( size_t i = 0; i<texBuf0.size(); ++i )
			{
				pDest[i].tu = texBuf0[i].x;
				pDest[i].tv = texBuf0[i].y;
			}
			pVB->unlock();
		}
		else if ( m_Channel >= 3 )
		{
			VerNorTex2* pDest = (VerNorTex2*)pVB->lock(HBL_NORMAL);
			for ( size_t i = 0; i<texBuf0.size(); ++i )
			{
				pDest[i].tu = texBuf0[i].x;
				pDest[i].tv = texBuf0[i].y;
			}
			pVB->unlock();
		}
	}

	void CPluginStaticMesh::generateTexCoordBuffer1( int level, const vector<CVector2f>& texBuf1 )
	{
		CVertexHardwareBuffer* pVB = m_VertexBuf.vertexBufferBinding->GetBuffer(0);
		if ( m_Channel == 2 )
		{
			VerNorTex1* pDest = (VerNorTex1*)pVB->lock(HBL_NORMAL);
			for ( size_t i = 0; i<texBuf1.size(); ++i )
			{
				pDest[i].tu1 = texBuf1[i].x;
				pDest[i].tv1 = texBuf1[i].y;
			}
			pVB->unlock();
		}
		else if ( m_Channel >= 3 )
		{
			VerNorTex2* pDest = (VerNorTex2*)pVB->lock(HBL_NORMAL);
			for ( size_t i = 0; i<texBuf1.size(); ++i )
			{
				pDest[i].tu1 = texBuf1[i].x;
				pDest[i].tv1 = texBuf1[i].y;
			}
			pVB->unlock();
		}
	}

	void CPluginStaticMesh::generateTexCoordBuffer2( int level, const vector<CVector2f>& texBuf2 )
	{
		CVertexHardwareBuffer* pVB = m_VertexBuf.vertexBufferBinding->GetBuffer(0);
		if ( m_Channel >= 3 )
		{
			VerNorTex2* pDest = (VerNorTex2*)pVB->lock(HBL_NORMAL);
			for ( size_t i = 0; i<texBuf2.size(); ++i )
			{
				pDest[i].tu2 = texBuf2[i].x;
				pDest[i].tv2 = texBuf2[i].y;
			}
			pVB->unlock();
		}
	}

	void CPluginStaticMesh::generateSkeWeightBuffer( int level, const vector<CWeight>& skeWeightBuf )
	{
	}

	void CPluginStaticMesh::generateSkeIndexBuffer( int level, const vector<SkltIndex>& skeIndexBuf )
	{
	}

	float& CPluginStaticMesh::GetBaize()
	{
		return m_fBaize;
	}

	CVector3f& CPluginStaticMesh::GetScale()
	{
		return m_Scale;
	}

	CVector3f& CPluginStaticMesh::GetOff()
	{
		return m_Off;
	}

	CVector3f& CPluginStaticMesh::GetOrgScale()
	{
		return m_OrgScale;
	}

	CVector3f& CPluginStaticMesh::GetOrgOff()
	{
		return m_OrgOff;
	}

	float& CPluginStaticMesh::GetTexScale()
	{
		return m_fTextScale;
	}

	int& CPluginStaticMesh::GetChannel()
	{
		return m_Channel;
	}

	bool& CPluginStaticMesh::GetIsCompress()
	{
		return m_IsCompress;
	}
}