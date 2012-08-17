#include "stdafx.h"
#include "CEditMesh.h"
#include "CVector3.h"
#include "CTinyVector.h"
#include "CVertexDeclaration.h"
#include <fstream>

bool SaveVetex( CVector3f v, CVector3f n, CVector2f t, SaveInfo& SI, ofstream& File )
{
	int x = (int)( v.x*4 - SI.xStr + 0.5f );
	int y = (int)( v.y*4 - SI.yStr + 0.5f );
	int z = (int)( v.z*4 - SI.zStr + 0.5f );
	CVector3f _v( x*0.25f, y*0.25f, z*0.25f );

	if( SI.sPos < 1024 && SI.sTex <= 1.02f )
	{
		CTinyVertex<10,11> Vetex( _v, n, t );
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else if( SI.sPos < 4096 && SI.sTex <= 2.04f )
	{
		CTinyVertex<12,12> Vetex( _v, n, t );
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else if( SI.sPos < 4096 && SI.sTex < 32.7f )
	{
		CTinyVertex<12,16> Vetex( _v, n, t );
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else if( SI.sPos < 16384 && SI.sTex < 32.7f )
	{
		CTinyVertex<14,16> Vetex( _v, n, t );
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else if( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		CTinyVertex<16,16> Vetex( _v, n, t );
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else
	{
#if 0
		TCHAR szBuf[256];
		sprintf( szBuf, "Œª÷√∑∂Œß£∫%d,Ã˘Õº∑∂Œß£∫%.02f\x0d\x0a", SI.sPos, SI.sTex );
		g_ConverLog += szBuf;
#endif
		return false;
	}

	return true;
}
bool SaveVetex( CVector3f v, CVector3f n, CVector2f t, CVector2f t1,SaveInfo& SI, ofstream& File )
{
	int x = (int)( v.x*4 - SI.xStr + 0.5f );
	int y = (int)( v.y*4 - SI.yStr + 0.5f );
	int z = (int)( v.z*4 - SI.zStr + 0.5f );
	CVector3f _v( x*0.25f, y*0.25f, z*0.25f );

	if( SI.sPos < 1024 && SI.sTex <= 1.02f )
	{
		CTinyVertex<10,11,2> Vetex( _v, n, t ,t1);
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else if( SI.sPos < 4096 && SI.sTex <= 2.04f )
	{
		CTinyVertex<12,12,2> Vetex( _v, n, t ,t1);
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else if( SI.sPos < 4096 && SI.sTex < 32.7f )
	{
		CTinyVertex<12,16,2> Vetex( _v, n, t ,t1);
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else if( SI.sPos < 16384 && SI.sTex < 32.7f )
	{
		CTinyVertex<14,16,2> Vetex( _v, n, t ,t1);
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else if( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		CTinyVertex<16,16,2> Vetex( _v, n, t ,t1);
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else
	{
#if 0
		TCHAR szBuf[256];
		sprintf( szBuf, "Œª÷√∑∂Œß£∫%d,Ã˘Õº∑∂Œß£∫%.02f\x0d\x0a", SI.sPos, SI.sTex );
		g_ConverLog += szBuf;
#endif
		return false;
	}

	return true;
}
bool SaveVetex( CVector3f v, CVector3f n, CVector2f t,CVector2f t1,CVector2f t2, SaveInfo& SI, ofstream& File )
{
	int x = (int)( v.x*4 - SI.xStr + 0.5f );
	int y = (int)( v.y*4 - SI.yStr + 0.5f );
	int z = (int)( v.z*4 - SI.zStr + 0.5f );
	CVector3f _v( x*0.25f, y*0.25f, z*0.25f );

	if( SI.sPos < 1024 && SI.sTex <= 1.02f )
	{
		CTinyVertex<10,11,3> Vetex( _v, n, t ,t1,t2);
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else if( SI.sPos < 4096 && SI.sTex <= 2.04f )
	{
		CTinyVertex<12,12,3> Vetex( _v, n, t ,t1,t2);
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else if( SI.sPos < 4096 && SI.sTex < 32.7f )
	{
		CTinyVertex<12,16,3> Vetex( _v, n, t ,t1,t2);
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else if( SI.sPos < 16384 && SI.sTex < 32.7f )
	{
		CTinyVertex<14,16,3> Vetex( _v, n, t ,t1,t2);
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else if( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		CTinyVertex<16,16,3> Vetex( _v, n, t ,t1,t2);
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else
	{
#if 0
		TCHAR szBuf[256];
		sprintf( szBuf, "Œª÷√∑∂Œß£∫%d,Ã˘Õº∑∂Œß£∫%.02f\x0d\x0a", SI.sPos, SI.sTex );
		g_ConverLog += szBuf;
#endif
		return false;
	}
	return true;
}
bool SaveVetex( CVector3f v, CVector3f n, SaveInfo& SI, ofstream& File )
{
	int x = int( v.x*4 + 0.5f ) - SI.xStr;
	int y = int( v.y*4 + 0.5f ) - SI.yStr;
	int z = int( v.z*4 + 0.5f ) - SI.zStr;
	CVector3f _v( x*0.25f, y*0.25f, z*0.25f );

	if( SI.sPos < 1024 && SI.sTex <= 1.02f )
	{
		CTinyVertex<10,11> Vetex( _v, n, CVector2f( 0, 0 ) );
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) - 11*2/8 );
	}
	else if( SI.sPos < 4096 && SI.sTex <= 2.04f )
	{
		CTinyVertex<12,12> Vetex( _v, n, CVector2f( 0, 0 ) );
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) - 12*2/8 );
	}
	else if( SI.sPos < 4096 && SI.sTex < 32.7f )
	{
		CTinyVertex<12,16> Vetex( _v, n, CVector2f( 0, 0 ) );
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) - 16*2/8 );
	}
	else if( SI.sPos < 16384 && SI.sTex < 32.7f )
	{
		CTinyVertex<14,16> Vetex( _v, n, CVector2f( 0, 0 ) );
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) - 16*2/8 );
	}
	else if( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		CTinyVertex<16,16> Vetex( _v, n, CVector2f( 0, 0 ) );
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) - 16*2/8 );
	}
	else
	{
#if 0
		TCHAR szBuf[256];
		sprintf( szBuf, "Œª÷√∑∂Œß£∫%d,Ã˘Õº∑∂Œß£∫%.02f\x0d\x0a", SI.sPos, SI.sTex );
		g_ConverLog += szBuf;
#endif
		return false;
	}
	return true;
}

bool SaveVetexFull( CVector3f v, CVector3f n, CVector2f t, SaveInfo& SI, ofstream& File )
{
	if( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		File.write( (TCHAR*)&v, sizeof( v ) );
		File.write( (TCHAR*)&n, sizeof( n ) );
		File.write( (TCHAR*)&t, sizeof( t ) );
		return true;
	}
	else
	{
#if 0
		TCHAR szBuf[256];
		sprintf( szBuf, "Œª÷√∑∂Œß£∫%d,Ã˘Õº∑∂Œß£∫%.02f\x0d\x0a", SI.sPos, SI.sTex );
		g_ConverLog += szBuf;
#endif
		return false;
	}
}
bool SaveVetexFull( CVector3f v, CVector3f n, CVector2f t, CVector2f t1,SaveInfo& SI, ofstream& File )
{
	if( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		File.write( (TCHAR*)&v, sizeof( v ) );
		File.write( (TCHAR*)&n, sizeof( n ) );
		File.write( (TCHAR*)&t, sizeof( t ) );
		File.write( (TCHAR*)&t1, sizeof( t1 ) );
		return true;
	}
	else
	{
#if 0
		TCHAR szBuf[256];
		sprintf( szBuf, "Œª÷√∑∂Œß£∫%d,Ã˘Õº∑∂Œß£∫%.02f\x0d\x0a", SI.sPos, SI.sTex );
		g_ConverLog += szBuf;
#endif
		return false;
	}
}

bool SaveVetexFull( CVector3f v, CVector3f n, CVector2f t,CVector2f t1,CVector2f t2, SaveInfo& SI, ofstream& File )
{
	if( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		File.write( (TCHAR*)&v, sizeof( v ) );
		File.write( (TCHAR*)&n, sizeof( n ) );
		File.write( (TCHAR*)&t, sizeof( t ) );
		File.write( (TCHAR*)&t1, sizeof( t1 ) );
		File.write( (TCHAR*)&t2, sizeof( t2 ) );
		return true;
	}
	else
	{
#if 0
		TCHAR szBuf[256];
		sprintf( szBuf, "Œª÷√∑∂Œß£∫%d,Ã˘Õº∑∂Œß£∫%.02f\x0d\x0a", SI.sPos, SI.sTex );
		g_ConverLog += szBuf;
#endif
		return false;
	}
}

bool SaveVetexFull( CVector3f v, CVector3f n, SaveInfo& SI, ofstream& File )
{
	if( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		File.write( (TCHAR*)&v, sizeof( v ) );
		File.write( (TCHAR*)&n, sizeof( n ) );
		return true;
	}
	else
	{
#if 0
		TCHAR szBuf[256];
		sprintf( szBuf, "Œª÷√∑∂Œß£∫%d,Ã˘Õº∑∂Œß£∫%.02f\x0d\x0a", SI.sPos, SI.sTex );
		g_ConverLog += szBuf;
#endif
		return false;
	}
}

bool CEditMeshWriter::ComSave(ofstream& File,CMeshSaveBase* tSaveBase)
{
	File.write( (TCHAR*)&tSaveBase->m_VertexType, sizeof(tSaveBase->m_VertexType) );
	File.write( (TCHAR*)&tSaveBase->m_fBaize, sizeof(tSaveBase->m_fBaize ) );
	File.write( (TCHAR*)&m_EditOrgScale, sizeof( m_EditOrgScale ) );
	File.write( (TCHAR*)&m_EditOrgOff, sizeof( m_EditOrgOff ) );
#if 0
	if(g_isBatSaveBigBox)
	{
		m_Scale = m_OrgScale;
		m_Off   = m_OrgOff;
	}
#endif
	File.write( (TCHAR*)&tSaveBase->m_Scale, sizeof( tSaveBase->m_Scale ) );
	File.write( (TCHAR*)&tSaveBase->m_Off, sizeof( tSaveBase->m_Off ) );
	File.write( (TCHAR*)&m_EditfTextScale, sizeof( m_EditfTextScale ) );
	File.write( (TCHAR*)&tSaveBase->m_Channel,sizeof( tSaveBase->m_Channel));
	File.write( (TCHAR*)&tSaveBase->m_IsCompress,sizeof( tSaveBase->m_IsCompress));
	//-------------±£¥ÊOBB------------------------------------
	{
		DWORD Size = (DWORD)tSaveBase->m_BoxMap.size();
		File.write( (TCHAR*)&Size,sizeof(DWORD));
		CMeshBase::BoxMap::const_iterator it = tSaveBase->m_BoxMap.begin();
		CMeshBase::BoxMap::const_iterator eit = tSaveBase->m_BoxMap.end();
		for(;it!=eit;++it)
		{
			uint16 ID = it->first;
			File.write( (TCHAR*)&ID,sizeof(uint16));
			File.write( (TCHAR*)&(it->second.m_vMinimum),sizeof(CVector3f) );
			File.write( (TCHAR*)&(it->second.m_vMaximum),sizeof(CVector3f) );
		}
	}

	//--------------------------------------------------------
	bool IsSucced = true;
	{
		int		Size = 1; //
		float	fDist	 = 0.0f;
		int VetexSize = GetVertexSize();//m_VerAnimate[0].m_FramesVertex[0].size();
		//Size = (int)tSaveBase->m_MeshLevels.size();
		File.write( (TCHAR*)&Size, sizeof(Size) );
		File.write( (TCHAR*)&fDist, sizeof( fDist ) ); //
		File.write( (TCHAR*)&VetexSize, sizeof(VetexSize) );


		if(tSaveBase->m_IsCompress)
			IsSucced = _SaveCompress(File);
		else
			IsSucced = _SaveFull(File);
	
		IndexData*	pIndexData = tSaveBase->m_MeshLevels[0].GetIndexData();
		int IndSize = (int)pIndexData->indexCount;
		File.write( (TCHAR*)&IndSize, sizeof(IndSize) );
		WORD* pIB = (WORD*)pIndexData->indexBuffer->lock(HBL_READ_ONLY);
		if( VetexSize >= 256 )
			File.write( (const char*)pIB, sizeof(WORD)*IndSize );
		else
		{
			for( int i = 0; i< IndSize; ++i )
			{
				BYTE tBy = (BYTE)pIB[i];
				File.write( (const char*)&tBy, sizeof(BYTE));
			}
		}
		pIndexData->indexBuffer->unlock();
	}
	return IsSucced;
}

int	 CEditSkeletalMesh::GetVertexSize(void)
{
	return (int)m_OrgVertex.size();
}

bool CEditSkeletalMesh::_SaveCompress(ofstream& File)
{
	size_t VetexSize = m_OrgVertex.size();
	SaveInfo SI( m_EditOrgScale, m_EditOrgOff, 0, 0, m_EditfTextScale, m_VertexType );
	VertexSkeletalFormat* pSkeBuf = (VertexSkeletalFormat*)&m_OrgVertex[0];
	for( size_t i = 0; i < VetexSize; i++ )
	{
		switch(m_Channel)
		{
		case 0:
		case 1:
			if( !SaveVetex( *(CVector3f*)&(pSkeBuf[i].x), *(CVector3f*)&(pSkeBuf[i].n),*(CVector2f*)&(pSkeBuf[i].u), SI, File ) )
				return false;
			break;
		case 2:
			if( !SaveVetex( *(CVector3f*)&(pSkeBuf[i].x), *(CVector3f*)&(pSkeBuf[i].n), *(CVector2f*)&(pSkeBuf[i].u),*(CVector2f*)&(pSkeBuf[i].u1), SI, File ) )
				return false;
			break;
		default:
			if( !SaveVetex( *(CVector3f*)&(pSkeBuf[i].x), *(CVector3f*)&(pSkeBuf[i].n), *(CVector2f*)&(pSkeBuf[i].u),*(CVector2f*)&(pSkeBuf[i].u1),*(CVector2f*)&(pSkeBuf[i].u2), SI, File ) )
				return false;
			break;
		}
		CTinyWeight ttW(pSkeBuf[i].w._w[0],pSkeBuf[i].w._w[1],pSkeBuf[i].w._w[2],pSkeBuf[i].w._w[3]);
		File.write( (TCHAR*)&ttW, sizeof(CTinyWeight) );
		File.write( (TCHAR*)&pSkeBuf[i].i, sizeof(SkltIndex) );
	}
	return true;
}

bool CEditSkeletalMesh::_SaveFull(ofstream& File)
{
	size_t VetexSize = m_VertexBuf.vertexCount;
	SaveInfo SI( m_EditOrgScale, m_EditOrgOff, 0, 0, m_EditfTextScale, m_VertexType );
	VertexSkeletalFormat* pSkeBuf = (VertexSkeletalFormat*)&m_OrgVertex[0];
	for( size_t i = 0; i < VetexSize; i++ )
	{
		switch(m_Channel)
		{
		case 0:
		case 1:
			if( !SaveVetexFull( *(CVector3f*)&(pSkeBuf[i].x), *(CVector3f*)&(pSkeBuf[i].n), *(CVector2f*)&(pSkeBuf[i].u), SI, File ) )
				return false;
			break;
		case 2:
			if( !SaveVetexFull( *(CVector3f*)&(pSkeBuf[i].x), *(CVector3f*)&(pSkeBuf[i].n), *(CVector2f*)&(pSkeBuf[i].u),*(CVector2f*)&(pSkeBuf[i].u1), SI, File ) )
				return false;
			break;
		default:
			if( !SaveVetexFull( *(CVector3f*)&(pSkeBuf[i].x), *(CVector3f*)&(pSkeBuf[i].n), *(CVector2f*)&(pSkeBuf[i].u),*(CVector2f*)&(pSkeBuf[i].u1),*(CVector2f*)&(pSkeBuf[i].u2), SI, File ) )
				return false;
			break;
		}
		CTinyWeight ttW(pSkeBuf[i].w._w[0],pSkeBuf[i].w._w[1],pSkeBuf[i].w._w[2],pSkeBuf[i].w._w[3]);
		File.write( (TCHAR*)&ttW, sizeof(CTinyWeight) );
		File.write( (TCHAR*)&pSkeBuf[i].i, sizeof(SkltIndex) );
	}
	return true;
}
GVector<VertexSkeletalFormat>*	 CEditSkeletalMesh::_GetVertex()
{
	return &m_OrgVertex;
}

int	 CEditStaticMesh::GetVertexSize(void)
{
	return (int)(int)m_VertexBuf.vertexCount;
}

bool CEditStaticMesh::_SaveCompress(ofstream& File)
{
	CVertexHardwareBuffer* pBuffer = m_VertexBuf.vertexBufferBinding->GetBuffer(0);
	void* pVerBuf = pBuffer->lock(HBL_READ_ONLY);
	SaveInfo SI( m_EditOrgScale, m_EditOrgOff, 0, 0, m_EditfTextScale, m_VertexType );
	size_t VetexSize = m_VertexBuf.vertexCount;
	switch(m_Channel)
	{
	case 0:
	case 1:
		{
			VerNorTex* OutBuf = (VerNorTex*)pVerBuf;
			for( size_t i = 0; i < VetexSize; i++ )
			{
				CVector2f tVec2(OutBuf[i].tu,OutBuf[i].tv);
				if( !SaveVetex( OutBuf[i].p, OutBuf[i].np, tVec2, SI, File ) )
				{
					pBuffer->unlock();
					return false;
				}
			}
		}
		break;
	case 2:
		{
			VerNorTex1* OutBuf = (VerNorTex1*)pVerBuf;
			for( size_t i = 0; i < VetexSize; i++ )
			{
				CVector2f tVec2(OutBuf[i].tu,OutBuf[i].tv);
				CVector2f tVec2_1(OutBuf[i].tu1,OutBuf[i].tv1);
				if( !SaveVetex( OutBuf[i].p, OutBuf[i].np, tVec2,tVec2_1, SI, File ) )
				{
					pBuffer->unlock();
					return false;
				}
			}
		}
		break;
	default:
		{
			VerNorTex2* OutBuf = (VerNorTex2*)pVerBuf;
			for( size_t i = 0; i < VetexSize; i++ )
			{
				CVector2f tVec2(OutBuf[i].tu,OutBuf[i].tv);
				CVector2f tVec2_1(OutBuf[i].tu1,OutBuf[i].tv1);
				CVector2f tVec2_2(OutBuf[i].tu2,OutBuf[i].tv2);
				if( !SaveVetex( OutBuf[i].p, OutBuf[i].np,  tVec2,tVec2_1,tVec2_2, SI, File ) )
				{
					pBuffer->unlock();
					return false;
				}
			}
		}
		break;
	}
	pBuffer->unlock();
	return true;
}

bool CEditStaticMesh::_SaveFull(ofstream& File)
{
	CVertexHardwareBuffer* pBuffer = m_VertexBuf.vertexBufferBinding->GetBuffer(0);
	void* pVerBuf = pBuffer->lock(HBL_READ_ONLY);
	SaveInfo SI( m_EditOrgScale, m_EditOrgOff, 0, 0, m_EditfTextScale, m_VertexType );
	size_t VetexSize = m_VertexBuf.vertexCount;
	switch(m_Channel)
	{
	case 0:
	case 1:
		{
			VerNorTex* OutBuf = (VerNorTex*)pVerBuf;
			for( size_t i = 0; i < VetexSize; i++ )
			{
				CVector2f tVec2(OutBuf[i].tu,OutBuf[i].tv);
				if( !SaveVetexFull( OutBuf[i].p, OutBuf[i].np, tVec2, SI, File ) )
				{
					pBuffer->unlock();
					return false;
				};
			}
		}
		break;
	case 2:
		{
			VerNorTex1* OutBuf = (VerNorTex1*)pVerBuf;
			for( size_t i = 0; i < VetexSize; i++ )
			{
				CVector2f tVec2(OutBuf[i].tu,OutBuf[i].tv);
				CVector2f tVec2_1(OutBuf[i].tu1,OutBuf[i].tv1);
				if( !SaveVetexFull( OutBuf[i].p, OutBuf[i].np, tVec2,tVec2_1, SI, File ) )
				{
					pBuffer->unlock();
					return false;
				}
			}
		}
		break;
	default:
		{
			VerNorTex2* OutBuf = (VerNorTex2*)pVerBuf;
			for( size_t i = 0; i < VetexSize; i++ )
			{
				CVector2f tVec2(OutBuf[i].tu,OutBuf[i].tv);
				CVector2f tVec2_1(OutBuf[i].tu1,OutBuf[i].tv1);
				CVector2f tVec2_2(OutBuf[i].tu2,OutBuf[i].tv2);
				if( !SaveVetexFull( OutBuf[i].p, OutBuf[i].np,  tVec2,tVec2_1,tVec2_2, SI, File ) )
				{
					pBuffer->unlock();
					return false;
				}
			}
		}
		break;
	}
	pBuffer->unlock();
	return true;
}

bool CEditVertexAniMesh::_SaveCompress(ofstream& File)
{
	return false;
}
bool CEditVertexAniMesh::_SaveFull(ofstream& File)
{
	return false;
}

bool CEditSkeletalMesh::BinSave(ofstream& File)
{
	CMeshSaveBase* tSaveBase = (CMeshSaveBase*)(CMeshBase*)this;
	return ComSave(File,tSaveBase);
}

bool CEditStaticMesh::BinSave(ofstream& File)
{
	CMeshSaveBase* tSaveBase = (CMeshSaveBase*)(CMeshBase*)this;
	return ComSave(File,tSaveBase);
}

bool CEditVertexAniMesh::BinSave(ofstream& File)
{
	CMeshSaveBase* tSaveBase = (CMeshSaveBase*)(CMeshBase*)this;
	return ComSave(File,tSaveBase);
}

void CEditSkeletalMesh::SetScale(const CVector3f& OrgScal)
{
	m_Scale = OrgScal;
}

void CEditSkeletalMesh::SetOff(const CVector3f& OrgOff)
{
	m_Off = OrgOff;
}

void CEditStaticMesh::SetScale(const CVector3f& OrgScal)
{
	m_Scale = OrgScal;
}

void CEditStaticMesh::SetOff(const CVector3f& OrgOff)
{
	m_Off = OrgOff;
}

void CEditVertexAniMesh::SetScale(const CVector3f& OrgScal)
{
	m_Scale = OrgScal;
}

void CEditVertexAniMesh::SetOff(const CVector3f& OrgOff)
{
	m_Off = OrgOff;
}

void CEditSkeletalMesh::_RecordData(const CVector3f& OrgScal,const CVector3f& OrgOff, float fTextScale)
{
	m_EditOrgScale		= OrgScal;
	m_EditOrgOff		= OrgOff;
	m_EditfTextScale	= fTextScale;
}

void  CEditStaticMesh::_RecordData(const CVector3f& OrgScal,const CVector3f& OrgOff, float fTextScale)
{
	m_EditOrgScale		= OrgScal;
	m_EditOrgOff		= OrgOff;
	m_EditfTextScale	= fTextScale;
}

void CEditVertexAniMesh::_RecordData(const CVector3f& OrgScal,const CVector3f& OrgOff, float fTextScale)
{
	m_EditOrgScale		= OrgScal;
	m_EditOrgOff		= OrgOff;
	m_EditfTextScale	= fTextScale;
}
