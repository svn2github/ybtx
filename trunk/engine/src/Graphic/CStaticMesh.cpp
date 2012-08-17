#include "stdafx.h"
#include "CStaticMesh.h"
#include "CVertexDeclaration.h"
#include "CMeshLoader.h"
#include "CGraphic.h"
#include "CRenderPipeline.h"

#define GETVERSION(VER)	0x0000ffff&VER
#define GETCP(VER)  (0x00010000&VER)>>16
#define GETBOX(VER)	 (0x00100000&VER)>>20
#define ENABLECP(VER)		0x00010000|VER
#define ENABLEBOX(VER)		0x00100000|VER

void StaticUpdate(CRenderCommand& Com,CRenderMatrix& AniInst)
{
	Com.param_ptr = (TCHAR*)CRenderPipeline::GetInst()->GetTextureMat(0);
}

void StaticRender(CRenderCommand& Com)
{
	CRenderPipeline::GetInst()->SetTextureMatrixImmediate(0,(CMatrix*)Com.param_ptr);
}

CStaticMesh::CStaticMesh()
{
	m_VertexType = VT_STATICVERTEX;	
}

CStaticMesh::~CStaticMesh()
{}

void CStaticMesh::CreateRenderCommand(CRenderCommand& Com, UINT Index)
{
	Com.vertexData = &m_VertexBuf;
	Com.indexData = m_MeshLevels[0].GetIndexData();
	Com.useIndexes = true;
	Com.param_ptr = (TCHAR*)CRenderPipeline::GetInst()->GetTextureMat(0);
	Com.param_fun  = StaticUpdate;
	Com.param_render = StaticRender;
}

FilterNode* CStaticMesh::Render( CRenderMatrix& ModelMatrix,FilterNode& FN )
{
	CreateRenderCommand(FN);

	return (FilterNode*)CGraphic::GetInst()->Insert(&FN);
}

int	CStaticMesh::BinLoad( CBufFile& QuickFile ,DWORD Version, SKELETAL_MASK& mask  )
{
	DWORD tVersionCp,tVersionLow,tVersionBox;
	tVersionBox = GETBOX(Version);
	tVersionCp = GETCP(Version);
	tVersionLow = GETVERSION(Version);

	QuickFile.read( (char*)&m_fBaize, sizeof( m_fBaize ) );
	QuickFile.read( (char*)&m_OrgScale, sizeof( m_OrgScale ) );
	QuickFile.read( (char*)&m_OrgOff, sizeof( m_OrgOff ) );
	QuickFile.read( (char*)&m_Scale, sizeof( m_Scale ) );
	QuickFile.read( (char*)&m_Off, sizeof( m_Off ) );
	QuickFile.read( (char*)&m_fTextScale, sizeof( m_fTextScale ) );
	if (tVersionLow>=10005)
		QuickFile.read( (char*)&m_Channel,sizeof(m_Channel));

	if (tVersionLow>=10007&&tVersionCp)
		QuickFile.read( (char*)&m_IsCompress,sizeof(m_IsCompress));

	if (tVersionLow==10007||tVersionBox)
	{
		DWORD BoxCount = 0;
		QuickFile.read( (char*)&BoxCount,sizeof(DWORD) );
		uint16 ID;
		CBaseAABB Box;
		for (DWORD j = 0;j<BoxCount;++j)
		{
			QuickFile.read( (char*)&ID,sizeof(uint16));
			QuickFile.read( (char*)&(Box.m_vMinimum),sizeof(CVector3f) );
			QuickFile.read( (char*)&(Box.m_vMaximum),sizeof(CVector3f) );
			m_BoxMap.insert(BoxMap::value_type(ID,Box));
		}
	}

	_RecordData(m_OrgScale,m_OrgOff,m_fTextScale);
	SaveInfo SI( m_OrgScale, m_OrgOff, 0, 0, m_fTextScale, m_VertexType );

	int Size;
	QuickFile.read( (char*)&Size, sizeof(Size) );
	Ast(Size == 1&&"Mod文件格式出现问题");
	m_MeshLevels.resize(Size);

	float Dist;
	QuickFile.read( (char*)&Dist, sizeof( Dist ) ); //暂时还没有用处
	QuickFile.read( (char*)&Size, sizeof(Size) );
	if (Size==0)
		return 0;

	m_VertexBuf.vertexCount = Size;
	m_VertexBuf.vertexStart = 0;
	CSyncSystem::TryToStop();
	CVertexHardwareBuffer* pVB;

	VertexFomat VertexBuf;
	VertexFomatFull VertexBufFull;

	
	switch (m_Channel)
	{
		case 0:
		case 1:
		{
			m_VertexBuf.vertexDeclaration =  CVertexDeclaration::GET_VERDECL(VerNorTex::Format);
			if (NULL==(pVB = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VerNorTex),Size,HBU_WRITE_ONLY)))
				return 0;
			VerNorTex* OutBuf = (VerNorTex*)pVB->lock(HBL_NO_OVERWRITE);
			if (!m_IsCompress)
			{
				for ( int i = 0; i < Size; i++,OutBuf++ )
				{
					LoadVertexFull( VertexBufFull, SI, QuickFile );
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
					LoadVertex( VertexBuf, SI, QuickFile );
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
			m_VertexBuf.vertexDeclaration =  CVertexDeclaration::GET_VERDECL(VerNorTex1::Format);
			if (NULL==(pVB = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VerNorTex1),Size,HBU_WRITE_ONLY)))
				return 0;
			VerNorTex1* OutBuf = (VerNorTex1*)pVB->lock(HBL_NO_OVERWRITE);
			if (!m_IsCompress)
			{
				for ( int i = 0; i < Size; i++,OutBuf++ )
				{
					LoadVertexFull1( VertexBufFull, SI, QuickFile );
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
					LoadVertex1( VertexBuf, SI, QuickFile );
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
			m_VertexBuf.vertexDeclaration =  CVertexDeclaration::GET_VERDECL(VerNorTex2::Format);
			if (NULL==(pVB = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VerNorTex2),Size,HBU_WRITE_ONLY)))
				return 0;
			VerNorTex2* OutBuf = (VerNorTex2*)pVB->lock(HBL_NO_OVERWRITE);
			if (!m_IsCompress)
			{
				for ( int i = 0; i < Size; i++,OutBuf++ )
				{
					LoadVertexFull2( VertexBufFull, SI, QuickFile );
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
					LoadVertex2( VertexBuf, SI, QuickFile );
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
	m_VertexBuf.vertexBufferBinding->SetBinding(0, pVB);
	size_t IBSize = Size< 256 ? sizeof(BYTE) : sizeof(WORD);
	m_MeshLevels[0].BinLoad( QuickFile , IBSize );
	return 0;
}
