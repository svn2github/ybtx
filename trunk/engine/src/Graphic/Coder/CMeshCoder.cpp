#include "stdafx.h"
#include "CMesh.h"
#include "CMeshCoder.h"
#include "CMeshLoader.h"
#include "CGraphic.h"
#include "CRenderPipeline.h"
#include "CVertexDeclaration.h"
#include "CCoderNameHelper.h"
#include "CPieceGroup.h"
#include "CVertexAniMesh.h"
#include "CDataSources.h"

namespace
{
	class CMeshCoderPiece : CPiece
	{
		friend class CVertexAniMeshCoder;
		friend class CStaticMeshCoder;
		friend class CSkeletalMeshCoder;
		friend class CCPUSkeletalMeshCoder;
	};

	class CVertexAniMeshCoderMesh : public CVertexAniMesh
	{
		friend class CVertexAniMeshCoder;
	};

	class CStaticMeshCoderMesh : public CStaticMesh
	{
		friend class CStaticMeshCoder;
	};

	class CCPUSkeletalMeshCoderMesh : public CCPUSkeletalMesh
	{
		friend class CSkeletalMeshCoder;
	};

	class CSkeletalMeshCoderMesh : public CSkeletalMesh
	{
		friend class CSkeletalMeshCoder;
	};

	class CCommonMeshCoderMesh : public CMesh
	{
		friend class CCommonMeshCoder;
	};


	bool SaveVetex( CVector3f v, CVector3f n, CVector2f t, SaveInfo& SI, WriteDataInf& wdi )
	{
		int x = (int)( v.x*4 - SI.xStr + 0.5f );
		int y = (int)( v.y*4 - SI.yStr + 0.5f );
		int z = (int)( v.z*4 - SI.zStr + 0.5f );
		CVector3f _v( x*0.25f, y*0.25f, z*0.25f );

		if( SI.sPos < 1024 && SI.sTex <= 1.02f )
		{
			CTinyVertex<10,11> Vetex( _v, n, t );
			wdi.Write( Vetex._Buf, sizeof( Vetex._Buf ) );
		}
		else if( SI.sPos < 4096 && SI.sTex <= 2.04f )
		{
			CTinyVertex<12,12> Vetex( _v, n, t );
			wdi.Write( Vetex._Buf, sizeof( Vetex._Buf ) );
		}
		else if( SI.sPos < 4096 && SI.sTex < 32.7f )
		{
			CTinyVertex<12,16> Vetex( _v, n, t );
			wdi.Write( Vetex._Buf, sizeof( Vetex._Buf ) );
		}
		else if( SI.sPos < 16384 && SI.sTex < 32.7f )
		{
			CTinyVertex<14,16> Vetex( _v, n, t );
			wdi.Write( Vetex._Buf, sizeof( Vetex._Buf ) );
		}
		else if( SI.sPos < 65536 && SI.sTex < 32.7f )
		{
			CTinyVertex<16,16> Vetex( _v, n, t );
			wdi.Write( Vetex._Buf, sizeof( Vetex._Buf ) );
		}
		else
		{
#if 0
			TCHAR szBuf[256];
			sprintf( szBuf, "Î»ÖÃ·¶Î§£º%d,ÌùÍ¼·¶Î§£º%.02f\x0d\x0a", SI.sPos, SI.sTex );
			g_ConverLog += szBuf;
#endif
			return false;
		}

		return true;
	}
	bool SaveVetex( CVector3f v, CVector3f n, CVector2f t, CVector2f t1,SaveInfo& SI, WriteDataInf& wdi )
	{
		int x = (int)( v.x*4 - SI.xStr + 0.5f );
		int y = (int)( v.y*4 - SI.yStr + 0.5f );
		int z = (int)( v.z*4 - SI.zStr + 0.5f );
		CVector3f _v( x*0.25f, y*0.25f, z*0.25f );

		if( SI.sPos < 1024 && SI.sTex <= 1.02f )
		{
			CTinyVertex<10,11,2> Vetex( _v, n, t ,t1);
			wdi.Write( Vetex._Buf, sizeof( Vetex._Buf ) );
		}
		else if( SI.sPos < 4096 && SI.sTex <= 2.04f )
		{
			CTinyVertex<12,12,2> Vetex( _v, n, t ,t1);
			wdi.Write( Vetex._Buf, sizeof( Vetex._Buf ) );
		}
		else if( SI.sPos < 4096 && SI.sTex < 32.7f )
		{
			CTinyVertex<12,16,2> Vetex( _v, n, t ,t1);
			wdi.Write( Vetex._Buf, sizeof( Vetex._Buf ) );
		}
		else if( SI.sPos < 16384 && SI.sTex < 32.7f )
		{
			CTinyVertex<14,16,2> Vetex( _v, n, t ,t1);
			wdi.Write( Vetex._Buf, sizeof( Vetex._Buf ) );
		}
		else if( SI.sPos < 65536 && SI.sTex < 32.7f )
		{
			CTinyVertex<16,16,2> Vetex( _v, n, t ,t1);
			wdi.Write( Vetex._Buf, sizeof( Vetex._Buf ) );
		}
		else
		{
#if 0
			TCHAR szBuf[256];
			sprintf( szBuf, "Î»ÖÃ·¶Î§£º%d,ÌùÍ¼·¶Î§£º%.02f\x0d\x0a", SI.sPos, SI.sTex );
			g_ConverLog += szBuf;
#endif
			return false;
		}

		return true;
	}
	bool SaveVetex( CVector3f v, CVector3f n, CVector2f t,CVector2f t1,CVector2f t2, SaveInfo& SI, WriteDataInf& wdi )
	{
		int x = (int)( v.x*4 - SI.xStr + 0.5f );
		int y = (int)( v.y*4 - SI.yStr + 0.5f );
		int z = (int)( v.z*4 - SI.zStr + 0.5f );
		CVector3f _v( x*0.25f, y*0.25f, z*0.25f );

		if( SI.sPos < 1024 && SI.sTex <= 1.02f )
		{
			CTinyVertex<10,11,3> Vetex( _v, n, t ,t1,t2);
			wdi.Write( Vetex._Buf, sizeof( Vetex._Buf ) );
		}
		else if( SI.sPos < 4096 && SI.sTex <= 2.04f )
		{
			CTinyVertex<12,12,3> Vetex( _v, n, t ,t1,t2);
			wdi.Write( Vetex._Buf, sizeof( Vetex._Buf ) );
		}
		else if( SI.sPos < 4096 && SI.sTex < 32.7f )
		{
			CTinyVertex<12,16,3> Vetex( _v, n, t ,t1,t2);
			wdi.Write( Vetex._Buf, sizeof( Vetex._Buf ) );
		}
		else if( SI.sPos < 16384 && SI.sTex < 32.7f )
		{
			CTinyVertex<14,16,3> Vetex( _v, n, t ,t1,t2);
			wdi.Write( Vetex._Buf, sizeof( Vetex._Buf ) );
		}
		else if( SI.sPos < 65536 && SI.sTex < 32.7f )
		{
			CTinyVertex<16,16,3> Vetex( _v, n, t ,t1,t2);
			wdi.Write( Vetex._Buf, sizeof( Vetex._Buf ) );
		}
		else
		{
#if 0
			TCHAR szBuf[256];
			sprintf( szBuf, "Î»ÖÃ·¶Î§£º%d,ÌùÍ¼·¶Î§£º%.02f\x0d\x0a", SI.sPos, SI.sTex );
			g_ConverLog += szBuf;
#endif
			return false;
		}
		return true;
	}
	bool SaveVetex( CVector3f v, CVector3f n, SaveInfo& SI, WriteDataInf& wdi )
	{
		int x = int( v.x*4 + 0.5f ) - SI.xStr;
		int y = int( v.y*4 + 0.5f ) - SI.yStr;
		int z = int( v.z*4 + 0.5f ) - SI.zStr;
		CVector3f _v( x*0.25f, y*0.25f, z*0.25f );

		if( SI.sPos < 1024 && SI.sTex <= 1.02f )
		{
			CTinyVertex<10,11> Vetex( _v, n, CVector2f( 0, 0 ) );
			wdi.Write( Vetex._Buf, sizeof( Vetex._Buf ) - 11*2/8 );
		}
		else if( SI.sPos < 4096 && SI.sTex <= 2.04f )
		{
			CTinyVertex<12,12> Vetex( _v, n, CVector2f( 0, 0 ) );
			wdi.Write( Vetex._Buf, sizeof( Vetex._Buf ) - 12*2/8 );
		}
		else if( SI.sPos < 4096 && SI.sTex < 32.7f )
		{
			CTinyVertex<12,16> Vetex( _v, n, CVector2f( 0, 0 ) );
			wdi.Write( Vetex._Buf, sizeof( Vetex._Buf ) - 16*2/8 );
		}
		else if( SI.sPos < 16384 && SI.sTex < 32.7f )
		{
			CTinyVertex<14,16> Vetex( _v, n, CVector2f( 0, 0 ) );
			wdi.Write( Vetex._Buf, sizeof( Vetex._Buf ) - 16*2/8 );
		}
		else if( SI.sPos < 65536 && SI.sTex < 32.7f )
		{
			CTinyVertex<16,16> Vetex( _v, n, CVector2f( 0, 0 ) );
			wdi.Write( Vetex._Buf, sizeof( Vetex._Buf ) - 16*2/8 );
		}
		else
		{
#if 0
			TCHAR szBuf[256];
			sprintf( szBuf, "Î»ÖÃ·¶Î§£º%d,ÌùÍ¼·¶Î§£º%.02f\x0d\x0a", SI.sPos, SI.sTex );
			g_ConverLog += szBuf;
#endif
			return false;
		}
		return true;
	}

	bool SaveVetexFull( CVector3f v, CVector3f n, CVector2f t, SaveInfo& SI, WriteDataInf& wdi )
	{
		if( SI.sPos < 65536 && SI.sTex < 32.7f )
		{
			wdi.Write( (char*)&v, sizeof( v ) );
			wdi.Write( (char*)&n, sizeof( n ) );
			wdi.Write( (char*)&t, sizeof( t ) );
			return true;
		}
		else
		{
#if 0
			TCHAR szBuf[256];
			sprintf( szBuf, "Î»ÖÃ·¶Î§£º%d,ÌùÍ¼·¶Î§£º%.02f\x0d\x0a", SI.sPos, SI.sTex );
			g_ConverLog += szBuf;
#endif
			return false;
		}
	}
	bool SaveVetexFull( CVector3f v, CVector3f n, CVector2f t, CVector2f t1,SaveInfo& SI, WriteDataInf& wdi )
	{
		if( SI.sPos < 65536 && SI.sTex < 32.7f )
		{
			wdi.Write( (char*)&v, sizeof( v ) );
			wdi.Write( (char*)&n, sizeof( n ) );
			wdi.Write( (char*)&t, sizeof( t ) );
			wdi.Write( (char*)&t1, sizeof( t1 ) );
			return true;
		}
		else
		{
#if 0
			TCHAR szBuf[256];
			sprintf( szBuf, "Î»ÖÃ·¶Î§£º%d,ÌùÍ¼·¶Î§£º%.02f\x0d\x0a", SI.sPos, SI.sTex );
			g_ConverLog += szBuf;
#endif
			return false;
		}
	}

	bool SaveVetexFull( CVector3f v, CVector3f n, CVector2f t,CVector2f t1,CVector2f t2, SaveInfo& SI, WriteDataInf& wdi )
	{
		if( SI.sPos < 65536 && SI.sTex < 32.7f )
		{
			wdi.Write( (char*)&v, sizeof( v ) );
			wdi.Write( (char*)&n, sizeof( n ) );
			wdi.Write( (char*)&t, sizeof( t ) );
			wdi.Write( (char*)&t1, sizeof( t1 ) );
			wdi.Write( (char*)&t2, sizeof( t2 ) );
			return true;
		}
		else
		{
#if 0
			TCHAR szBuf[256];
			sprintf( szBuf, "Î»ÖÃ·¶Î§£º%d,ÌùÍ¼·¶Î§£º%.02f\x0d\x0a", SI.sPos, SI.sTex );
			g_ConverLog += szBuf;
#endif
			return false;
		}
	}

	bool SaveVetexFull( CVector3f v, CVector3f n, SaveInfo& SI, WriteDataInf& wdi )
	{
		if( SI.sPos < 65536 && SI.sTex < 32.7f )
		{
			wdi.Write( (char*)&v, sizeof( v ) );
			wdi.Write( (char*)&n, sizeof( n ) );
			return true;
		}
		else
		{
#if 0
			TCHAR szBuf[256];
			sprintf( szBuf, "Î»ÖÃ·¶Î§£º%d,ÌùÍ¼·¶Î§£º%.02f\x0d\x0a", SI.sPos, SI.sTex );
			g_ConverLog += szBuf;
#endif
			return false;
		}
	}
}

namespace sqr
{
	FourCC CVertexAniMeshCoder::GetChunkName(void)	
	{ 
		return CCoderNameHelper::VER_ANI_MESH_CODER; 
	}

	bool CVertexAniMeshCoder::Encode(WriteDataInf& wdi , OBJ_HANDLE handle )
	{
		CMeshCoderPiece* ps = static_cast<CMeshCoderPiece*>(handle);
		if ( ps->m_Mesh->GetVertexType() == VT_VERTEXANI )
		{
			_Encode(wdi, ps->m_Mesh);
			return true;
		}

		return true;
	}

	bool CVertexAniMeshCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		return true;
	}

	bool CVertexAniMeshCoder::DeCode_VertexAniMesh( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		CMeshCoderPiece* ps = static_cast<CMeshCoderPiece*>(handle);
		ps->m_Mesh = CDataSources::GetInst()->NewVertexAniMesh();
		
		return true;
	}

	BEGIN_VERSION_MAP(CVertexAniMeshCoder)
		ON_VERSION( 1 , DeCode_VertexAniMesh )
	END_VERSION_MAP()

	// ---- CStaticMeshCoder -------------------------------------------------------------------------

	FourCC CStaticMeshCoder::GetChunkName(void)	
	{ 
		return CCoderNameHelper::STATIC_MESH_CODER; 
	}

	CStaticMeshCoder::CStaticMeshCoder()
	{
		this->RegisterCoder(new CCommonMeshCoder);
	}

	bool CStaticMeshCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		CMeshCoderPiece* ps = static_cast<CMeshCoderPiece*>(handle);
		if ( ps->m_Mesh->GetVertexType() == VT_STATICVERTEX )
		{
			_Encode(wdi, ps->m_Mesh);
			return true;
		}

		return false;
	}

	bool CStaticMeshCoder::_EncodeImpl(WriteDataInf& wdi , PRIVATE_HANDLE handle )
	{
		CStaticMeshCoderMesh* tSaveBase = static_cast<CStaticMeshCoderMesh*>(handle);
		_AutoEncode(wdi, tSaveBase);

		bool IsSucced = true;
		{
			int		Size = 1; 
			float	fDist	 = 0.0f;
			size_t VetexSize = tSaveBase->GetVerNum();
			wdi.Write( (char*)&Size, sizeof(Size) );
			wdi.Write( (char*)&fDist, sizeof( fDist ) ); 
			wdi.Write( (char*)&VetexSize, sizeof(VetexSize) );

			if(tSaveBase->m_IsCompress)
			{
				CVertexHardwareBuffer* pBuffer = tSaveBase->m_VertexBuf.vertexBufferBinding->GetBuffer(0);
				void* pVerBuf = pBuffer->lock(HBL_READ_ONLY);
				SaveInfo SI( tSaveBase->m_OrgScale, tSaveBase->m_OrgOff, 0, 0, tSaveBase->m_fTextScale, tSaveBase->m_VertexType );
				switch(tSaveBase->m_Channel)
				{
				case 0:
				case 1:
					{
						VerNorTex* OutBuf = (VerNorTex*)pVerBuf;
						for( size_t i = 0; i < VetexSize; i++ )
						{
							CVector2f tVec2(OutBuf[i].tu,OutBuf[i].tv);
							if( !SaveVetex( OutBuf[i].p, OutBuf[i].np, tVec2, SI, wdi ) )
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
							if( !SaveVetex( OutBuf[i].p, OutBuf[i].np, tVec2,tVec2_1, SI, wdi ) )
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
							if( !SaveVetex( OutBuf[i].p, OutBuf[i].np,  tVec2,tVec2_1,tVec2_2, SI, wdi ) )
							{
								pBuffer->unlock();
								return false;
							}
						}
					}
					break;
				}
				pBuffer->unlock();
			}
			else
			{
				CVertexHardwareBuffer* pBuffer = tSaveBase->m_VertexBuf.vertexBufferBinding->GetBuffer(0);
				void* pVerBuf = pBuffer->lock(HBL_READ_ONLY);
				SaveInfo SI( tSaveBase->m_OrgScale, tSaveBase->m_OrgOff, 0, 0, tSaveBase->m_fTextScale, tSaveBase->m_VertexType );
				switch(tSaveBase->m_Channel)
				{
				case 0:
				case 1:
					{
						VerNorTex* OutBuf = (VerNorTex*)pVerBuf;
						for( size_t i = 0; i < VetexSize; i++ )
						{
							CVector2f tVec2(OutBuf[i].tu,OutBuf[i].tv);
							if( !SaveVetexFull( OutBuf[i].p, OutBuf[i].np, tVec2, SI, wdi ) )
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
							if( !SaveVetexFull( OutBuf[i].p, OutBuf[i].np, tVec2,tVec2_1, SI, wdi ) )
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
							if( !SaveVetexFull( OutBuf[i].p, OutBuf[i].np,  tVec2,tVec2_1,tVec2_2, SI, wdi ) )
							{
								pBuffer->unlock();
								return false;
							}
						}
					}
					break;
				}
				pBuffer->unlock();
			}

			IndexData*	pIndexData = tSaveBase->m_MeshLevels[0].GetIndexData();
			int IndSize = (int)pIndexData->indexCount;
			wdi.Write( (char*)&IndSize, sizeof(IndSize) );
			WORD* pIB = (WORD*)pIndexData->indexBuffer->lock(HBL_READ_ONLY);
			if( VetexSize >= 256 )
				wdi.Write( (const char*)pIB, sizeof(WORD)*IndSize );
			else
			{
				for( int i = 0; i< IndSize; ++i )
				{
					BYTE tBy = (BYTE)pIB[i];
					wdi.Write( (const char*)&tBy, sizeof(BYTE));
				}
			}
			pIndexData->indexBuffer->unlock();
		}
		return IsSucced;
	}

	bool CStaticMeshCoder::DeCode_StaticMesh( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		CMeshCoderPiece* ps = static_cast<CMeshCoderPiece*>(handle);
		ps->m_Mesh = CDataSources::GetInst()->NewStaticMesh();
		CBufFile tData( inData.GetBuffer(), inData.GetSize() );
		CDataChunk tChunk;
		tData.SafeRead(&tChunk.GetHdr(), sizeof(CDataChunk::ChunkHdr));
		ReadChunk(tData,tChunk);
		_DoDecode(tChunk, ps->m_Mesh);

		CStaticMeshCoderMesh* pMesh = static_cast<CStaticMeshCoderMesh*>(ps->m_Mesh);
		SaveInfo SI( pMesh->m_OrgScale, pMesh->m_OrgOff, 0, 0, pMesh->m_fTextScale, pMesh->m_VertexType );

		int Size;
		tData.SafeRead( (char*)&Size, sizeof(Size) );
		pMesh->m_MeshLevels.resize(Size);

		float Dist;
		tData.SafeRead( (char*)&Dist, sizeof( Dist ) ); //ÔÝÊ±»¹Ã»ÓÐÓÃ´¦
		tData.SafeRead( (char*)&Size, sizeof(Size) );
		if (Size==0)
			return 0;

		pMesh->m_VertexBuf.vertexCount = Size;
		pMesh->m_VertexBuf.vertexStart = 0;
		CVertexHardwareBuffer* pVB;

		CStaticMesh::VertexFomat VertexBuf;
		CStaticMesh::VertexFomatFull VertexBufFull;


		switch (pMesh->m_Channel)
		{
		case 0:
		case 1:
			{
				pMesh->m_VertexBuf.vertexDeclaration =  CVertexDeclaration::GET_VERDECL(VerNorTex::Format);
				if (NULL==(pVB = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VerNorTex),Size,HBU_WRITE_ONLY)))
					return 0;
				VerNorTex* OutBuf = (VerNorTex*)pVB->lock(HBL_NO_OVERWRITE);
				if (!pMesh->m_IsCompress)
				{
					for ( int i = 0; i < Size; i++,OutBuf++ )
					{
						LoadVertexFull( VertexBufFull, SI, tData );
						OutBuf->p.x = VertexBufFull.x/**0.25f*/;
						OutBuf->p.y = VertexBufFull.y/**0.25f*/;
						OutBuf->p.z = VertexBufFull.z/**0.25f*/;
						OutBuf->np.x  = VertexBufFull.n;
						OutBuf->np.y  = VertexBufFull.ny;
						OutBuf->np.z  = VertexBufFull.nz;
						OutBuf->tu = VertexBufFull.u/**0.0005f*/;
						OutBuf->tv = VertexBufFull.v/**0.0005f*/;
						CSyncSystem::TryToStop();
					}
				}
				else
				{
					for ( int i = 0; i < Size; i++,OutBuf++ )
					{
						LoadVertex( VertexBuf, SI, tData );
						OutBuf->p.x = VertexBuf.x*0.25f;
						OutBuf->p.y = VertexBuf.y*0.25f;
						OutBuf->p.z = VertexBuf.z*0.25f;
						OutBuf->np  = (CVector3f&)_CovData::_s.m_fNorInd[ VertexBuf.n ];
						OutBuf->tu = VertexBuf.u*0.0005f;
						OutBuf->tv = VertexBuf.v*0.0005f;
						CSyncSystem::TryToStop();
					}
				}

				pVB->unlock();
				break;
			}
		case 2:
			{
				pMesh->m_VertexBuf.vertexDeclaration =  CVertexDeclaration::GET_VERDECL(VerNorTex1::Format);
				if (NULL==(pVB = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VerNorTex1),Size,HBU_WRITE_ONLY)))
					return 0;
				VerNorTex1* OutBuf = (VerNorTex1*)pVB->lock(HBL_NO_OVERWRITE);
				if (!pMesh->m_IsCompress)
				{
					for ( int i = 0; i < Size; i++,OutBuf++ )
					{
						LoadVertexFull1( VertexBufFull, SI, tData );
						OutBuf->p.x = VertexBufFull.x/**0.25f*/;
						OutBuf->p.y = VertexBufFull.y/**0.25f*/;
						OutBuf->p.z = VertexBufFull.z/**0.25f*/;
						OutBuf->np.x  = VertexBufFull.n;
						OutBuf->np.y  = VertexBufFull.ny;
						OutBuf->np.z  = VertexBufFull.nz;
						OutBuf->tu = VertexBufFull.u/**0.0005f*/;
						OutBuf->tv = VertexBufFull.v/**0.0005f*/;
						OutBuf->tu1 = VertexBufFull.u1/**0.0005f*/;
						OutBuf->tv1 = VertexBufFull.v1/**0.0005f*/;
						CSyncSystem::TryToStop();
					}
				}
				else
				{
					for ( int i = 0; i < Size; i++,OutBuf++ )
					{
						LoadVertex1( VertexBuf, SI, tData );
						OutBuf->p.x = VertexBuf.x*0.25f;
						OutBuf->p.y = VertexBuf.y*0.25f;
						OutBuf->p.z = VertexBuf.z*0.25f;
						OutBuf->np  = (CVector3f&)_CovData::_s.m_fNorInd[ VertexBuf.n ];
						OutBuf->tu = VertexBuf.u*0.0005f;
						OutBuf->tv = VertexBuf.v*0.0005f;
						OutBuf->tu1 = VertexBuf.u1*0.0005f;
						OutBuf->tv1 = VertexBuf.v1*0.0005f;
						CSyncSystem::TryToStop();
					}
				}

				pVB->unlock();
				break;
			}
		default:
			{
				pMesh->m_VertexBuf.vertexDeclaration =  CVertexDeclaration::GET_VERDECL(VerNorTex2::Format);
				if (NULL==(pVB = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VerNorTex2),Size,HBU_WRITE_ONLY)))
					return 0;
				VerNorTex2* OutBuf = (VerNorTex2*)pVB->lock(HBL_NO_OVERWRITE);
				if (!pMesh->m_IsCompress)
				{
					for ( int i = 0; i < Size; i++,OutBuf++ )
					{
						LoadVertexFull2( VertexBufFull, SI, tData );
						OutBuf->p.x = VertexBufFull.x/**0.25f*/;
						OutBuf->p.y = VertexBufFull.y/**0.25f*/;
						OutBuf->p.z = VertexBufFull.z/**0.25f*/;
						OutBuf->np.x  = VertexBufFull.n;
						OutBuf->np.y  = VertexBufFull.ny;
						OutBuf->np.z  = VertexBufFull.nz;
						OutBuf->tu = VertexBufFull.u/**0.0005f*/;
						OutBuf->tv = VertexBufFull.v/**0.0005f*/;
						OutBuf->tu1 = VertexBufFull.u1/**0.0005f*/;
						OutBuf->tv1 = VertexBufFull.v1/**0.0005f*/;
						OutBuf->tu2 = VertexBufFull.u2/**0.0005f*/;
						OutBuf->tv2 = VertexBufFull.v2/**0.0005f*/;
						CSyncSystem::TryToStop();
					}
				}
				else
				{
					for ( int i = 0; i < Size; i++,OutBuf++ )
					{
						LoadVertex2( VertexBuf, SI, tData );
						OutBuf->p.x = VertexBuf.x*0.25f;
						OutBuf->p.y = VertexBuf.y*0.25f;
						OutBuf->p.z = VertexBuf.z*0.25f;
						OutBuf->np  = (CVector3f&)_CovData::_s.m_fNorInd[ VertexBuf.n ];
						OutBuf->tu = VertexBuf.u*0.0005f;
						OutBuf->tv = VertexBuf.v*0.0005f;
						OutBuf->tu1 = VertexBuf.u1*0.0005f;
						OutBuf->tv1 = VertexBuf.v1*0.0005f;
						OutBuf->tu2 = VertexBuf.u2*0.0005f;
						OutBuf->tv2 = VertexBuf.v2*0.0005f;
						CSyncSystem::TryToStop();
					}
				}

				pVB->unlock();
				break;
			}
		}
		pMesh->m_VertexBuf.vertexBufferBinding->SetBinding(0, pVB);
		size_t IBSize = Size< 256 ? sizeof(BYTE) : sizeof(WORD);
		pMesh->m_MeshLevels[0].BinLoad( tData , IBSize );

		return true;
	}

	BEGIN_VERSION_MAP(CStaticMeshCoder)
		ON_VERSION( 1 , DeCode_StaticMesh )
	END_VERSION_MAP()

	// ---- CSkeletalMeshCoder -----------------------------------------------------------------------

	FourCC CSkeletalMeshCoder::GetChunkName(void)	
	{ 
		return CCoderNameHelper::SKE_MESH_CODER; 
	}

	CSkeletalMeshCoder::CSkeletalMeshCoder()
	{
		RegisterCoder(new CCommonMeshCoder);
	}

	bool CSkeletalMeshCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		CMeshCoderPiece* ps = static_cast<CMeshCoderPiece*>(handle);
		if ( ps->m_Mesh->GetVertexType() == VT_SKELETALANI )
		{
			_Encode(wdi, ps->m_Mesh);
			return true;
		}

		return false;
	}

	bool CSkeletalMeshCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		CSkeletalMeshCoderMesh* tSaveBase = (CSkeletalMeshCoderMesh*)handle;
		_AutoEncode(wdi, handle);

		bool IsSucced = true;
		{
			int		Size = 1; 
			float	fDist	 = 0.0f;
			size_t VetexSize = tSaveBase->_GetVertex()->size();
			wdi.Write( (char*)&Size, sizeof(Size) );
			wdi.Write( (char*)&fDist, sizeof( fDist ) ); 
			wdi.Write( (char*)&VetexSize, sizeof(VetexSize) );

			if(tSaveBase->m_IsCompress)
			{
				SaveInfo SI( tSaveBase->m_OrgScale, tSaveBase->m_OrgOff, 0, 0, tSaveBase->m_fTextScale, tSaveBase->m_VertexType );
				VertexSkeletalFormat* pSkeBuf = (VertexSkeletalFormat*)&(*tSaveBase->_GetVertex())[0];
				for( size_t i = 0; i < VetexSize; i++ )
				{
					switch(tSaveBase->m_Channel)
					{
					case 0:
					case 1:
						if( !SaveVetex( *(CVector3f*)&(pSkeBuf[i].x), *(CVector3f*)&(pSkeBuf[i].n),*(CVector2f*)&(pSkeBuf[i].u), SI, wdi ) )
							return false;
						break;
					case 2:
						if( !SaveVetex( *(CVector3f*)&(pSkeBuf[i].x), *(CVector3f*)&(pSkeBuf[i].n), *(CVector2f*)&(pSkeBuf[i].u),*(CVector2f*)&(pSkeBuf[i].u1), SI, wdi ) )
							return false;
						break;
					default:
						if( !SaveVetex( *(CVector3f*)&(pSkeBuf[i].x), *(CVector3f*)&(pSkeBuf[i].n), *(CVector2f*)&(pSkeBuf[i].u),*(CVector2f*)&(pSkeBuf[i].u1),*(CVector2f*)&(pSkeBuf[i].u2), SI, wdi ) )
							return false;
						break;
					}
					CTinyWeight ttW(pSkeBuf[i].w._w[0],pSkeBuf[i].w._w[1],pSkeBuf[i].w._w[2],pSkeBuf[i].w._w[3]);
					wdi.Write( (char*)&ttW, sizeof(CTinyWeight) );
					wdi.Write( (char*)&pSkeBuf[i].i, sizeof(SkltIndex) );
				}
				IsSucced = true;
			}
			else
			{
				SaveInfo SI( tSaveBase->m_OrgScale, tSaveBase->m_OrgOff, 0, 0, tSaveBase->m_fTextScale, tSaveBase->m_VertexType );
				VertexSkeletalFormat* pSkeBuf = (VertexSkeletalFormat*)&(*tSaveBase->_GetVertex())[0];
				for( size_t i = 0; i < VetexSize; i++ )
				{
					switch(tSaveBase->m_Channel)
					{
					case 0:
					case 1:
						if( !SaveVetexFull( *(CVector3f*)&(pSkeBuf[i].x), *(CVector3f*)&(pSkeBuf[i].n), *(CVector2f*)&(pSkeBuf[i].u), SI, wdi ) )
							return false;
						break;
					case 2:
						if( !SaveVetexFull( *(CVector3f*)&(pSkeBuf[i].x), *(CVector3f*)&(pSkeBuf[i].n), *(CVector2f*)&(pSkeBuf[i].u),*(CVector2f*)&(pSkeBuf[i].u1), SI, wdi ) )
							return false;
						break;
					default:
						if( !SaveVetexFull( *(CVector3f*)&(pSkeBuf[i].x), *(CVector3f*)&(pSkeBuf[i].n), *(CVector2f*)&(pSkeBuf[i].u),*(CVector2f*)&(pSkeBuf[i].u1),*(CVector2f*)&(pSkeBuf[i].u2), SI, wdi ) )
							return false;
						break;
					}
					CTinyWeight ttW(pSkeBuf[i].w._w[0],pSkeBuf[i].w._w[1],pSkeBuf[i].w._w[2],pSkeBuf[i].w._w[3]);
					wdi.Write( (char*)&ttW, sizeof(CTinyWeight) );
					wdi.Write( (char*)&pSkeBuf[i].i, sizeof(SkltIndex) );
				}
				IsSucced = true;
			}

			IndexData*	pIndexData = tSaveBase->m_MeshLevels[0].GetIndexData();
			int IndSize = (int)pIndexData->indexCount;
			wdi.Write( (char*)&IndSize, sizeof(IndSize) );
			WORD* pIB = (WORD*)pIndexData->indexBuffer->lock(HBL_READ_ONLY);
			if( VetexSize >= 256 )
				wdi.Write( (const char*)pIB, sizeof(WORD)*IndSize );
			else
			{
				for( int i = 0; i< IndSize; ++i )
				{
					BYTE tBy = (BYTE)pIB[i];
					wdi.Write( (const char*)&tBy, sizeof(BYTE));
				}
			}
			pIndexData->indexBuffer->unlock();
		}
		return IsSucced;
		return true;
	}

	bool CSkeletalMeshCoder::DeCode_SkeletalMesh( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		bool ret;
		if ( CGpuProgramMgr::GetInst()->GetShaderVer() >= Shader_V_2 )
			ret = _DecodeGpuSkeMesh(inData, handle);
		else
			ret = _DecodeCpuSkeMesh(inData, handle);

		return ret;
	}

	bool CSkeletalMeshCoder::_DecodeCpuSkeMesh( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		CMeshCoderPiece* ps = static_cast<CMeshCoderPiece*>(handle);
		ps->m_Mesh = CDataSources::GetInst()->NewSkeletalMesh();
		CBufFile tData( inData.GetBuffer(), inData.GetSize() );
		CDataChunk tChunk;
		tData.SafeRead(&tChunk.GetHdr(),sizeof(CDataChunk::ChunkHdr));
		ReadChunk(tData,tChunk);
		_DoDecode(tChunk,ps->m_Mesh);//¶ÁÈ¡Õâ¸öµØ·½Ë³Ðò³öÏÖµÄChunk

		CCPUSkeletalMeshCoderMesh* pMesh = static_cast<CCPUSkeletalMeshCoderMesh*>(ps->m_Mesh);
		GVector<uint16>	IndexBuf;

		SaveInfo SI( pMesh->m_OrgScale, pMesh->m_OrgOff, 0, 0, pMesh->m_fTextScale, pMesh->m_VertexType );

		int Size;
		tData.SafeRead( (char*)&Size, sizeof(Size) );
		pMesh->m_MeshLevels.resize(Size);

		float Dist;
		tData.SafeRead( (char*)&Dist, sizeof( Dist ) );
		tData.SafeRead( (char*)&Size, sizeof(Size) );

		VerNorTex2* VertexBuf;

		CSyncSystem::TryToStop();
		CVertexHardwareBuffer* pBuffer;
		if (NULL==(pBuffer = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VerNorTex2),Size,HBU_DYNAMIC,0,true,false)))
			return false;
		pMesh->m_SkinBuffer.resize(Size);
		pMesh->m_VertexBuf.vertexBufferBinding->SetBinding(0, pBuffer);
		pMesh->m_VertexBuf.vertexCount =Size;
		pMesh->m_VertexBuf.vertexStart = 0;
		VertexBuf = (VerNorTex2*)pBuffer->lock(	0, pBuffer->getSize(),HBL_NO_OVERWRITE );
		pMesh->m_VertexBuf.vertexDeclaration = CVertexDeclaration::GET_VERDECL(VerNorTex2::Format);
		pMesh->m_SkinInfo.m_pVertexBuffer = pBuffer;
		uint8	weight[4];

		CSyncSystem::TryToStop();
		switch (pMesh->m_Channel)
		{
		case 0:
		case 1:
			if (!pMesh->m_IsCompress)
			{
				for ( int i = 0; i < Size; i++ )
				{
					LoadVertexFull( *(CCPUSkeletalMesh::VertexFomatFull*)&VertexBuf[i], SI, tData );

					tData.SafeRead( (char*)weight, sizeof(weight) );
					tData.SafeRead( (char*)&pMesh->m_SkinBuffer[i].i, sizeof(SkltIndex) );
					pMesh->m_SkinBuffer[i].w._w[0] = weight[0] * 0.005f;
					pMesh->m_SkinBuffer[i].w._w[1] = weight[1] * 0.005f;
					pMesh->m_SkinBuffer[i].w._w[2] = weight[2] * 0.005f;
					pMesh->m_SkinBuffer[i].w._w[3] = weight[3] * 0.005f;
					ps->m_SkeletalMask.AddSkeletal( pMesh->m_SkinBuffer[i].i.b[0] );
					ps->m_SkeletalMask.AddSkeletal( pMesh->m_SkinBuffer[i].i.b[1] );
					ps->m_SkeletalMask.AddSkeletal( pMesh->m_SkinBuffer[i].i.b[2] );
					ps->m_SkeletalMask.AddSkeletal( pMesh->m_SkinBuffer[i].i.b[3] );
					CSyncSystem::TryToStop();
				}
			}
			else
			{
				for ( int i = 0; i < Size; i++ )
				{
					LoadVertex( *(CCPUSkeletalMesh::VertexFomatFull*)&VertexBuf[i], SI, tData );
					VertexBuf[i].np = _CovData::_s.m_fNorInd[ (int32)VertexBuf[i].np.x ];
					//GPUÐèÒª
					VertexBuf[i].p *= 0.25f;

					VertexBuf[i].tu *= 0.0005f;
					VertexBuf[i].tv *= 0.0005f;
					VertexBuf[i].tu1 = 0.0f;
					VertexBuf[i].tv1 = 0.0f;
					VertexBuf[i].tu2 = 0.0f;
					VertexBuf[i].tv2 = 0.0f;

					tData.SafeRead( (char*)weight, sizeof(weight) );
					tData.SafeRead( (char*)&pMesh->m_SkinBuffer[i].i, sizeof(SkltIndex) );
					pMesh->m_SkinBuffer[i].w._w[0] = weight[0] * 0.005f;
					pMesh->m_SkinBuffer[i].w._w[1] = weight[1] * 0.005f;
					pMesh->m_SkinBuffer[i].w._w[2] = weight[2] * 0.005f;
					pMesh->m_SkinBuffer[i].w._w[3] = weight[3] * 0.005f;
					ps->m_SkeletalMask.AddSkeletal( pMesh->m_SkinBuffer[i].i.b[0] );
					ps->m_SkeletalMask.AddSkeletal( pMesh->m_SkinBuffer[i].i.b[1] );
					ps->m_SkeletalMask.AddSkeletal( pMesh->m_SkinBuffer[i].i.b[2] );
					ps->m_SkeletalMask.AddSkeletal( pMesh->m_SkinBuffer[i].i.b[3] );
					CSyncSystem::TryToStop();
				}
			}

			break;
		case 2:
			if (!pMesh->m_IsCompress)
			{
				for ( int i = 0; i < Size; i++ )
				{
					LoadVertexFull1( *(CCPUSkeletalMesh::VertexFomatFull*)&VertexBuf[i], SI, tData );

					tData.SafeRead( (char*)weight, sizeof(weight) );
					tData.SafeRead( (char*)&pMesh->m_SkinBuffer[i].i, sizeof(SkltIndex) );
					pMesh->m_SkinBuffer[i].w._w[0] = weight[0] * 0.005f;
					pMesh->m_SkinBuffer[i].w._w[1] = weight[1] * 0.005f;
					pMesh->m_SkinBuffer[i].w._w[2] = weight[2] * 0.005f;
					pMesh->m_SkinBuffer[i].w._w[3] = weight[3] * 0.005f;
					ps->m_SkeletalMask.AddSkeletal( pMesh->m_SkinBuffer[i].i.b[0] );
					ps->m_SkeletalMask.AddSkeletal( pMesh->m_SkinBuffer[i].i.b[1] );
					ps->m_SkeletalMask.AddSkeletal( pMesh->m_SkinBuffer[i].i.b[2] );
					ps->m_SkeletalMask.AddSkeletal( pMesh->m_SkinBuffer[i].i.b[3] );
					CSyncSystem::TryToStop();
				}
			}
			else
			{
				for ( int i = 0; i < Size; i++ )
				{
					LoadVertex1( *(CCPUSkeletalMesh::VertexFomatFull*)&VertexBuf[i], SI, tData );
					VertexBuf[i].np= _CovData::_s.m_fNorInd[ (int32)VertexBuf[i].np.x ];
					VertexBuf[i].p *= 0.25f;


					VertexBuf[i].tu *= 0.0005f;
					VertexBuf[i].tv *= 0.0005f;
					VertexBuf[i].tu1 *= 0.0005f;
					VertexBuf[i].tv1 *= 0.0005f;
					VertexBuf[i].tu2 = 0.0f;
					VertexBuf[i].tv2 = 0.0f;

					tData.SafeRead( (char*)weight, sizeof(weight) );
					tData.SafeRead( (char*)&pMesh->m_SkinBuffer[i].i, sizeof(SkltIndex) );
					pMesh->m_SkinBuffer[i].w._w[0] = weight[0] * 0.005f;
					pMesh->m_SkinBuffer[i].w._w[1] = weight[1] * 0.005f;
					pMesh->m_SkinBuffer[i].w._w[2] = weight[2] * 0.005f;
					pMesh->m_SkinBuffer[i].w._w[3] = weight[3] * 0.005f;
					ps->m_SkeletalMask.AddSkeletal( pMesh->m_SkinBuffer[i].i.b[0] );
					ps->m_SkeletalMask.AddSkeletal( pMesh->m_SkinBuffer[i].i.b[1] );
					ps->m_SkeletalMask.AddSkeletal( pMesh->m_SkinBuffer[i].i.b[2] );
					ps->m_SkeletalMask.AddSkeletal( pMesh->m_SkinBuffer[i].i.b[3] );
					CSyncSystem::TryToStop();
				}
			}

			break;
		default:
			if (!pMesh->m_IsCompress)
			{
				for ( int i = 0; i < Size; i++ )
				{
					LoadVertexFull2( *(CCPUSkeletalMesh::VertexFomatFull*)&VertexBuf[i], SI, tData );

					tData.SafeRead( (char*)weight, sizeof(weight) );
					tData.SafeRead( (char*)&pMesh->m_SkinBuffer[i].i, sizeof(SkltIndex) );
					pMesh->m_SkinBuffer[i].w._w[0] = weight[0] * 0.005f;
					pMesh->m_SkinBuffer[i].w._w[1] = weight[1] * 0.005f;
					pMesh->m_SkinBuffer[i].w._w[2] = weight[2] * 0.005f;
					pMesh->m_SkinBuffer[i].w._w[3] = weight[3] * 0.005f;
					ps->m_SkeletalMask.AddSkeletal( pMesh->m_SkinBuffer[i].i.b[0] );
					ps->m_SkeletalMask.AddSkeletal( pMesh->m_SkinBuffer[i].i.b[1] );
					ps->m_SkeletalMask.AddSkeletal( pMesh->m_SkinBuffer[i].i.b[2] );
					ps->m_SkeletalMask.AddSkeletal( pMesh->m_SkinBuffer[i].i.b[3] );
					CSyncSystem::TryToStop();
				}
			}
			else
			{
				for ( int i = 0; i < Size; i++ )
				{
					LoadVertex2( *(CCPUSkeletalMesh::VertexFomatFull*)&VertexBuf[i], SI, tData );
					VertexBuf[i].np = _CovData::_s.m_fNorInd[ (int32)VertexBuf[i].np.x ];
					VertexBuf[i].p *= 0.25f;

					VertexBuf[i].tu *= 0.0005f;
					VertexBuf[i].tv *= 0.0005f;
					VertexBuf[i].tu1 *= 0.0005f;
					VertexBuf[i].tv1 *= 0.0005f;
					VertexBuf[i].tu2 *= 0.0005f;
					VertexBuf[i].tv2 *= 0.0005f;

					tData.SafeRead( (char*)weight, sizeof(weight) );
					tData.SafeRead( (char*)&pMesh->m_SkinBuffer[i].i, sizeof(SkltIndex) );
					pMesh->m_SkinBuffer[i].w._w[0] = weight[0] * 0.005f;
					pMesh->m_SkinBuffer[i].w._w[1] = weight[1] * 0.005f;
					pMesh->m_SkinBuffer[i].w._w[2] = weight[2] * 0.005f;
					pMesh->m_SkinBuffer[i].w._w[3] = weight[3] * 0.005f;
					ps->m_SkeletalMask.AddSkeletal( pMesh->m_SkinBuffer[i].i.b[0] );
					ps->m_SkeletalMask.AddSkeletal( pMesh->m_SkinBuffer[i].i.b[1] );
					ps->m_SkeletalMask.AddSkeletal( pMesh->m_SkinBuffer[i].i.b[2] );
					ps->m_SkeletalMask.AddSkeletal( pMesh->m_SkinBuffer[i].i.b[3] );
					CSyncSystem::TryToStop();
				}
			}

			break;
		}

		pBuffer->unlock();
		CSyncSystem::TryToStop();

		int ISize = 0;
		tData.SafeRead( (char*)&ISize, sizeof(ISize) );
		IndexBuf.resize( ISize );

		for ( int j = 0; j < (int)IndexBuf.size(); j++ )
			tData.SafeRead( (char*)&IndexBuf[j], Size < 256 ? sizeof(BYTE) : sizeof(WORD) );

		if (ISize==0)
			return 0;

		CSyncSystem::TryToStop();
		pMesh->m_MeshLevels[0].GetIndexData()->indexCount = IndexBuf.size();
		pMesh->m_MeshLevels[0].GetIndexData()->indexStart = 0;
		pMesh->m_MeshLevels[0].GetIndexData()->indexBuffer = CHardwareBufferMgr::GetInst()->createIndexBuffer(IndexBuf.size(),HBU_WRITE_ONLY);
		pMesh->m_MeshLevels[0].GetIndexData()->indexBuffer->write(	0,pMesh->m_MeshLevels[0].GetIndexData()->indexBuffer->getSize(),&IndexBuf[0], true );

		return true;
	}

	bool CSkeletalMeshCoder::_DecodeGpuSkeMesh( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		CMeshCoderPiece* ps = static_cast<CMeshCoderPiece*>(handle);
		ps->m_Mesh = CDataSources::GetInst()->NewSkeletalMesh();

		CBufFile tData( inData.GetBuffer(), inData.GetSize() );
		CDataChunk tChunk;
		tData.SafeRead(&tChunk.GetHdr(),sizeof(CDataChunk::ChunkHdr));
		ReadChunk(tData,tChunk);
		_DoDecode(tChunk,ps->m_Mesh); // ¶ÁÈ¡Õâ¸öµØ·½Ë³Ðò³öÏÖµÄChunk

		GVector<VertexSkeletalFormat>	VertexBuf;
		GVector<uint16>					IndexBuf;

		CSkeletalMeshCoderMesh* pMesh = static_cast<CSkeletalMeshCoderMesh*>(ps->m_Mesh);
		SaveInfo SI( pMesh->m_OrgScale, pMesh->m_OrgOff, 0, 0, pMesh->m_fTextScale, pMesh->m_VertexType );

		int Size;
		tData.SafeRead( (char*)&Size, sizeof(Size) );
		pMesh->m_MeshLevels.resize(Size);

		float Dist;
		tData.SafeRead( (char*)&Dist, sizeof( Dist ) );
		tData.SafeRead( (char*)&Size, sizeof(Size) );
		VertexBuf.resize(Size);

		pMesh->m_VertexBuf.vertexDeclaration = CVertexDeclaration::GET_VERDECL(VertexSkeletalFormat::Format);
		uint8	weight[4];

		CSyncSystem::TryToStop();
		switch (pMesh->m_Channel)
		{
		case 0:
		case 1:
			if (!pMesh->m_IsCompress)
			{
				for ( int i = 0; i < Size; i++ )
				{
					LoadVertexFull( VertexBuf[i], SI, tData );
					//GPUÐèÒª

					tData.SafeRead( (char*)weight, sizeof(weight) );
					tData.SafeRead( (char*)&VertexBuf[i].i, sizeof(SkltIndex) );
					VertexBuf[i].w._w[0] = weight[0] * 0.005f;
					VertexBuf[i].w._w[1] = weight[1] * 0.005f;
					VertexBuf[i].w._w[2] = weight[2] * 0.005f;
					VertexBuf[i].w._w[3] = weight[3] * 0.005f;
					CSyncSystem::TryToStop();
				}
			}
			else
			{
				for ( int i = 0; i < Size; i++ )
				{
					LoadVertex( VertexBuf[i], SI, tData );
					CVector3f n = _CovData::_s.m_fNorInd[ (int32)VertexBuf[i].n ];
					//GPUÐèÒª
					VertexBuf[i].x *= 0.25f;
					VertexBuf[i].y *= 0.25f;
					VertexBuf[i].z *= 0.25f;

					VertexBuf[i].n  = n.x;
					VertexBuf[i].ny = n.y;
					VertexBuf[i].nz = n.z;

					VertexBuf[i].u *= 0.0005f;
					VertexBuf[i].v *= 0.0005f;
					VertexBuf[i].u1 = 0.0f;
					VertexBuf[i].v1 = 0.0f;
					VertexBuf[i].u2 = 0.0f;
					VertexBuf[i].v2 = 0.0f;

					tData.SafeRead( (char*)weight, sizeof(weight) );
					tData.SafeRead( (char*)&VertexBuf[i].i, sizeof(SkltIndex) );
					VertexBuf[i].w._w[0] = weight[0] * 0.005f;
					VertexBuf[i].w._w[1] = weight[1] * 0.005f;
					VertexBuf[i].w._w[2] = weight[2] * 0.005f;
					VertexBuf[i].w._w[3] = weight[3] * 0.005f;
					CSyncSystem::TryToStop();
				}
			}

			break;
		case 2:
			if (!pMesh->m_IsCompress)
			{
				for ( int i = 0; i < Size; i++ )
				{
					LoadVertexFull1( VertexBuf[i], SI, tData );

					tData.SafeRead( (char*)weight, sizeof(weight) );
					tData.SafeRead( (char*)&VertexBuf[i].i, sizeof(SkltIndex) );
					VertexBuf[i].w._w[0] = weight[0] * 0.005f;
					VertexBuf[i].w._w[1] = weight[1] * 0.005f;
					VertexBuf[i].w._w[2] = weight[2] * 0.005f;
					VertexBuf[i].w._w[3] = weight[3] * 0.005f;
					CSyncSystem::TryToStop();
				}
			}
			else
			{
				for ( int i = 0; i < Size; i++ )
				{
					LoadVertex1( VertexBuf[i], SI, tData );
					CVector3f n = _CovData::_s.m_fNorInd[ (int32)VertexBuf[i].n ];
					VertexBuf[i].x *= 0.25f;
					VertexBuf[i].y *= 0.25f;
					VertexBuf[i].z *= 0.25f;
					VertexBuf[i].n  = n.x;
					VertexBuf[i].ny = n.y;
					VertexBuf[i].nz = n.z;

					VertexBuf[i].u *= 0.0005f;
					VertexBuf[i].v *= 0.0005f;
					VertexBuf[i].u1 *= 0.0005f;
					VertexBuf[i].v1 *= 0.0005f;
					VertexBuf[i].u2 = 0.0f;
					VertexBuf[i].v2 = 0.0f;

					tData.SafeRead( (char*)weight, sizeof(weight) );
					tData.SafeRead( (char*)&VertexBuf[i].i, sizeof(SkltIndex) );
					VertexBuf[i].w._w[0] = weight[0] * 0.005f;
					VertexBuf[i].w._w[1] = weight[1] * 0.005f;
					VertexBuf[i].w._w[2] = weight[2] * 0.005f;
					VertexBuf[i].w._w[3] = weight[3] * 0.005f;
					CSyncSystem::TryToStop();
				}
			}

			break;
		default:
			if (!pMesh->m_IsCompress)
			{
				for ( int i = 0; i < Size; i++ )
				{
					LoadVertexFull2( VertexBuf[i], SI, tData );

					tData.SafeRead( (char*)weight, sizeof(weight) );
					tData.SafeRead( (char*)&VertexBuf[i].i, sizeof(SkltIndex) );
					VertexBuf[i].w._w[0] = weight[0] * 0.005f;
					VertexBuf[i].w._w[1] = weight[1] * 0.005f;
					VertexBuf[i].w._w[2] = weight[2] * 0.005f;
					VertexBuf[i].w._w[3] = weight[3] * 0.005f;
					CSyncSystem::TryToStop();
				}
			}
			else
			{
				for ( int i = 0; i < Size; i++ )
				{
					LoadVertex2( VertexBuf[i], SI, tData );
					CVector3f n = _CovData::_s.m_fNorInd[ (int32)VertexBuf[i].n ];
					VertexBuf[i].x *= 0.25f;
					VertexBuf[i].y *= 0.25f;
					VertexBuf[i].z *= 0.25f;
					VertexBuf[i].n  = n.x;
					VertexBuf[i].ny = n.y;
					VertexBuf[i].nz = n.z;

					VertexBuf[i].u *= 0.0005f;
					VertexBuf[i].v *= 0.0005f;
					VertexBuf[i].u1 *= 0.0005f;
					VertexBuf[i].v1 *= 0.0005f;
					VertexBuf[i].u2 *= 0.0005f;
					VertexBuf[i].v2 *= 0.0005f;

					tData.SafeRead( (char*)weight, sizeof(weight) );
					tData.SafeRead( (char*)&VertexBuf[i].i, sizeof(SkltIndex) );
					VertexBuf[i].w._w[0] = weight[0] * 0.005f;
					VertexBuf[i].w._w[1] = weight[1] * 0.005f;
					VertexBuf[i].w._w[2] = weight[2] * 0.005f;
					VertexBuf[i].w._w[3] = weight[3] * 0.005f;
					CSyncSystem::TryToStop();
				}
			}

			break;
		}
		int ISize = 0;
		tData.SafeRead( (char*)&ISize, sizeof(ISize) );
		IndexBuf.resize( ISize );

		for ( int j = 0; j < (int)IndexBuf.size(); j++ )
			tData.SafeRead( (char*)&IndexBuf[j], Size < 256 ? sizeof(BYTE) : sizeof(WORD) );

		if (ISize==0)
			return 0;

		CSyncSystem::TryToStop();
		pMesh->_RecordVertex(VertexBuf);
		pMesh->SetSkeletalMask(ps->m_SkeletalMask, VertexBuf, IndexBuf);

		CVertexHardwareBuffer* pVB;
		if (NULL==(pVB = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VertexSkeletalFormat),VertexBuf.size(),HBU_WRITE_ONLY)))
			return false;
		pMesh->m_VertexBuf.vertexBufferBinding->SetBinding(0, pVB);
		pMesh->m_VertexBuf.vertexCount =VertexBuf.size();
		pMesh->m_VertexBuf.vertexStart = 0;
		pVB->write(	0, pVB->getSize(),&(VertexBuf[0]), true );

		CSyncSystem::TryToStop();
		pMesh->m_MeshLevels[0].GetIndexData()->indexCount = IndexBuf.size();
		pMesh->m_MeshLevels[0].GetIndexData()->indexStart = 0;
		pMesh->m_MeshLevels[0].GetIndexData()->indexBuffer = CHardwareBufferMgr::GetInst()->createIndexBuffer(IndexBuf.size(),HBU_WRITE_ONLY);
		pMesh->m_MeshLevels[0].GetIndexData()->indexBuffer->write(	0,
			pMesh->m_MeshLevels[0].GetIndexData()->indexBuffer->getSize(),
			&IndexBuf[0], true );

		return true;
	}

	BEGIN_VERSION_MAP(CSkeletalMeshCoder)
		ON_VERSION( 1 , DeCode_SkeletalMesh )
	END_VERSION_MAP()

	// ---- CCommonMeshCoder -------------------------------------------------------------------------

	FourCC CCommonMeshCoder::GetChunkName(void)	
	{
		return CCoderNameHelper::COMMON_MESH_CODER;
	}

	bool CCommonMeshCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		return _Encode(wdi, (PRIVATE_HANDLE)handle);
	}

	bool CCommonMeshCoder::_EncodeImpl( WriteDataInf& wdi , PRIVATE_HANDLE handle )
	{
		CCommonMeshCoderMesh* tSaveBase = (CCommonMeshCoderMesh*)handle;

		wdi.Write( (char*)&tSaveBase->m_fBaize, sizeof(tSaveBase->m_fBaize ) );
		wdi.Write( (char*)&tSaveBase->m_OrgScale, sizeof( tSaveBase->m_OrgScale ) );
		wdi.Write( (char*)&tSaveBase->m_OrgOff, sizeof( tSaveBase->m_OrgOff ) );
#if 0
		if(g_isBatSaveBigBox)
		{
			m_Scale = m_OrgScale;
			m_Off   = m_OrgOff;
		}
#endif
		wdi.Write( (char*)&tSaveBase->m_Scale, sizeof( tSaveBase->m_Scale ) );
		wdi.Write( (char*)&tSaveBase->m_Off, sizeof( tSaveBase->m_Off ) );
		wdi.Write( (char*)&tSaveBase->m_fTextScale, sizeof( tSaveBase->m_fTextScale ) );
		wdi.Write( (char*)&tSaveBase->m_Channel,sizeof( tSaveBase->m_Channel));
		wdi.Write( (char*)&tSaveBase->m_IsCompress,sizeof( tSaveBase->m_IsCompress));
		//-------------±£´æOBB------------------------------------
		{
			DWORD Size = (DWORD)tSaveBase->m_BoxMap.size();
			wdi.Write( (char*)&Size,sizeof(DWORD));
			CMeshBase::BoxMap::const_iterator it = tSaveBase->m_BoxMap.begin();
			CMeshBase::BoxMap::const_iterator eit = tSaveBase->m_BoxMap.end();
			for(;it!=eit;++it)
			{
				uint16 ID = it->first;
				wdi.Write( (char*)&ID,sizeof(uint16));
				wdi.Write( (char*)&(it->second.m_vMinimum),sizeof(CVector3f) );
				wdi.Write( (char*)&(it->second.m_vMaximum),sizeof(CVector3f) );
			}
		}

		return true;
	}

	bool CCommonMeshCoder::DeCode_CommonMesh( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		CBufFile tData( inData.GetBuffer(), inData.GetSize() );
		CCommonMeshCoderMesh* tSaveBase = (CCommonMeshCoderMesh*)handle;

		tData.SafeRead( (char*)&tSaveBase->m_fBaize, sizeof( tSaveBase->m_fBaize ) );
		tData.SafeRead( (char*)&tSaveBase->m_OrgScale, sizeof( tSaveBase->m_OrgScale) );
		tData.SafeRead( (char*)&tSaveBase->m_OrgOff, sizeof( tSaveBase->m_OrgOff) );
		tData.SafeRead( (char*)&tSaveBase->m_Scale, sizeof( tSaveBase->m_Scale ) );
		tData.SafeRead( (char*)&tSaveBase->m_Off, sizeof( tSaveBase->m_Off ) );
		tData.SafeRead( (char*)&tSaveBase->m_fTextScale, sizeof( tSaveBase->m_fTextScale) );

		tData.SafeRead( (char*)&tSaveBase->m_Channel,sizeof(tSaveBase->m_Channel));

		tData.SafeRead( (char*)&tSaveBase->m_IsCompress,sizeof(tSaveBase->m_IsCompress));

		DWORD BoxCount = 0;
		tData.SafeRead( (char*)&BoxCount,sizeof(DWORD) );
		uint16 ID;
		CBaseAABB Box;
		for (DWORD j = 0;j<BoxCount;++j)
		{
			tData.SafeRead( (char*)&ID,sizeof(uint16));
			tData.SafeRead( (char*)&(Box.m_vMinimum),sizeof(CVector3f) );
			tData.SafeRead( (char*)&(Box.m_vMaximum),sizeof(CVector3f) );
			tSaveBase->m_BoxMap.insert(CSkeletalMesh::BoxMap::value_type(ID,Box));
		}
		return true;
	}
	BEGIN_VERSION_MAP(CCommonMeshCoder)
		ON_VERSION( 1 , DeCode_CommonMesh )
	END_VERSION_MAP()
}