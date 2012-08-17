#include "stdafx.h"
#include "CCPUSkeletalMesh.h"
#include "CVertexDeclaration.h"
#include "CMeshLoader.h"
#include "SyncSystem.h"
#include "CGraphic.h"
#include "CHardwareBufferMgr.h"
#include "BaseHelper.h"
#include "CRenderPipeline.h"
#include "TSqrAllocator.inl"

#define GETVERSION(VER)	0x0000ffff&VER
#define GETCP(VER)  (0x00010000&VER)>>16
#define GETBOX(VER)	 (0x00100000&VER)>>20
#define ENABLECP(VER)		0x00010000|VER
#define ENABLEBOX(VER)		0x00100000|VER

void CpuSkeUpdate(CRenderCommand& Com,CRenderMatrix& AniInst)
{		
	if( AniInst.m_pBoneMatrixlist.size() != 0 )
	{
		CVertexHardwareBuffer* 	pOrgData = ((CCPUSkeletalMesh::SkinInfo*)Com.param_ptr)->m_pVertexBuffer;
		CCPUSkeletalMesh::PointInfoBuffer& SkinBuffer = *((CCPUSkeletalMesh::SkinInfo*)Com.param_ptr)->m_pSkinBuffer;
		VerNorTex2*				SkinBuf = NULL;
		VerNorTex2*				pOrgBuf = NULL;
		size_t					Offset	= 0;
		size_t					VerNum	= pOrgData->getNumVertices();
		size_t					Len		= sizeof(VerNorTex2)*VerNum;
		
		CVertexHardwareBuffer*	pVerBuf = CGraphic::GetInst()->GetDynamicPool()->LockVertexBuffer(
			Len ,(void**)&SkinBuf, Offset);
		Com.vertexData->vertexBufferBinding->SetBinding(0, pVerBuf, Offset, Len, sizeof(VerNorTex2));
		pOrgBuf = (VerNorTex2*)pOrgData->lock(HBL_READ_ONLY);

		//CQuatri
		//CVector3f	tOrg;
		CVector3f	tVec;

		CQuaternion	dq0;
		CQuaternion	dq;
		CMatrixUnit Vec;
		float		invSqr;

		for( Offset = 0; Offset < VerNum; ++Offset )
		{
			VerNorTex2&		SkinPoint	= SkinBuf[Offset];
			VerNorTex2&		OrgPoint	= pOrgBuf[Offset];
			CCPUSkeletalMesh::PointInfoEx&	SkinInf		= SkinBuffer[Offset];
			
			dq0 =	AniInst.m_pBoneMatrixlist[SkinInf.i.b[0]].m_FrameQuat;
			Vec =	AniInst.m_pBoneMatrixlist[SkinInf.i.b[0]] * SkinInf.w.GetWeight(0);
			dq	=	AniInst.m_pBoneMatrixlist[SkinInf.i.b[1]].m_FrameQuat;
			Vec +=	AniInst.m_pBoneMatrixlist[SkinInf.i.b[1]]*((dq0.Dot(dq)>0?1.0f:-1.0f)*SkinInf.w.GetWeight(1));
			dq	=	AniInst.m_pBoneMatrixlist[SkinInf.i.b[2]].m_FrameQuat;
			Vec +=	AniInst.m_pBoneMatrixlist[SkinInf.i.b[2]]*((dq0.Dot(dq)>0?1.0f:-1.0f)*SkinInf.w.GetWeight(2));
			dq	=	AniInst.m_pBoneMatrixlist[SkinInf.i.b[3]].m_FrameQuat;
			Vec +=	AniInst.m_pBoneMatrixlist[SkinInf.i.b[3]]*((dq0.Dot(dq)>0?1.0f:-1.0f)*SkinInf.w.GetWeight(3));
			//Vec.m_FrameQuat.normalize();
			invSqr = CMath::InvSqrt(Vec.Quat[0] * Vec.Quat[0] + Vec.Quat[1] * Vec.Quat[1] + 
				Vec.Quat[2] * Vec.Quat[2] + Vec.Quat[3] * Vec.Quat[3]);
			Vec *= invSqr;
			//Vec.Tran[0] *= invSqr;
			//Vec.Tran[1] *= invSqr;
			//Vec.Tran[2] *= invSqr;
			//Vec.Tran[3] *= invSqr;
			
			tVec.Init(Vec.m_FrameQuat.x,Vec.m_FrameQuat.y,Vec.m_FrameQuat.z);
			SkinPoint.p  = OrgPoint.p  + 2.0 * tVec.Cross(tVec.Cross(OrgPoint.p) + OrgPoint.p * Vec.m_FrameQuat.w); 
			SkinPoint.p += 2.0 * (Vec.m_FrameTrans * Vec.m_FrameQuat.w - tVec * Vec.Tran[3] + tVec.Cross(Vec.m_FrameTrans));
			SkinPoint.np = OrgPoint.np +  2.0 * tVec.Cross(tVec.Cross(OrgPoint.np) + OrgPoint.np * Vec.m_FrameQuat.w);
				
			SkinPoint.tu = OrgPoint.tu;
			SkinPoint.tv = OrgPoint.tv;
			SkinPoint.tu1 = OrgPoint.tu1;
			SkinPoint.tv1 = OrgPoint.tv1;
			SkinPoint.tu2 = OrgPoint.tu2;
			SkinPoint.tv2 = OrgPoint.tv2;		
		}

		pOrgData->unlock();
		CGraphic::GetInst()->GetDynamicPool()->UnLockVertex();
	}
}

void CpuSkeRender(CRenderCommand& Com)
{
	CRenderPipeline::GetInst()->SetTextureMatrixImmediate(0,NULL);
}

void CpuSkeRelease(CRenderCommand& Com)
{

}

void CCPUSkeletalMesh::CreateRenderCommand(CRenderCommand& Com, UINT Index )
{
	Com.vertexData = &m_VertexBuf;
	Com.indexData = m_MeshLevels[0].GetIndexData();
	Com.useIndexes = true;
	Com.vertexData->vertexDeclaration = CVertexDeclaration::GET_VERDECL(VerNorTex2::Format);
	Com.param_ptr = (TCHAR*)&m_SkinInfo;
	Com.param_fun = CpuSkeUpdate;
	Com.param_render  = CpuSkeRender;
	Com.param_release = CpuSkeRelease;
}

FilterNode* 	CCPUSkeletalMesh::Render( CRenderMatrix& ModelMatrix,FilterNode& FN )
{	
	CreateRenderCommand(FN);
	FilterNode* tFN = CGraphic::GetInst()->Insert(&FN);
	if(tFN)
		tFN->param_fun(*tFN,ModelMatrix);
	return tFN;
}

CCPUSkeletalMesh::CCPUSkeletalMesh()
{	
	m_SkinInfo.m_pSkinBuffer = &m_SkinBuffer;
}

CCPUSkeletalMesh::~CCPUSkeletalMesh()
{
}

int	CCPUSkeletalMesh::BinLoad( CBufFile& QuickFile ,DWORD Version, SKELETAL_MASK& mask  )
{
	//GVector<VertexSkeletalFormat>	VertexBuf;
	GVector<uint16>					IndexBuf;
	CVector3f OrgScale,OrgOff;
	float fTextScale;
	DWORD tVersionCp,tVersionLow,tVersionBox;
	tVersionBox = GETBOX(Version);
	tVersionCp = GETCP(Version);
	tVersionLow = GETVERSION(Version);

	QuickFile.read( (TCHAR*)&m_fBaize, sizeof( m_fBaize ) );
	QuickFile.read( (TCHAR*)&OrgScale, sizeof( OrgScale ) );
	QuickFile.read( (TCHAR*)&OrgOff, sizeof( OrgOff ) );
	QuickFile.read( (TCHAR*)&m_Scale, sizeof( m_Scale ) );
	QuickFile.read( (TCHAR*)&m_Off, sizeof( m_Off ) );
	QuickFile.read( (TCHAR*)&fTextScale, sizeof( fTextScale ) );

	_RecordData(OrgScale,OrgOff,fTextScale);
	if (tVersionLow>=10005)
		QuickFile.read( (TCHAR*)&m_Channel,sizeof(m_Channel));

	if ((tVersionLow>=10007)&&tVersionCp)
		QuickFile.read( (TCHAR*)&m_IsCompress,sizeof(m_IsCompress));

	if (tVersionLow==10007||tVersionBox)
	{
		DWORD BoxCount = 0;
		QuickFile.read( (TCHAR*)&BoxCount,sizeof(DWORD) );
		uint16 ID;
		CBaseAABB Box;
		for (DWORD j = 0;j<BoxCount;++j)
		{

			QuickFile.read( (TCHAR*)&ID,sizeof(uint16));
			QuickFile.read( (TCHAR*)&(Box.m_vMinimum),sizeof(CVector3f) );
			QuickFile.read( (TCHAR*)&(Box.m_vMaximum),sizeof(CVector3f) );
			m_BoxMap.insert(BoxMap::value_type(ID,Box));
		}
	}

	SaveInfo SI( OrgScale, OrgOff, 0, 0, fTextScale, m_VertexType );

	int Size;
	QuickFile.read( (TCHAR*)&Size, sizeof(Size) );
	m_MeshLevels.resize(Size);

	float Dist;
	QuickFile.read( (TCHAR*)&Dist, sizeof( Dist ) );
	QuickFile.read( (TCHAR*)&Size, sizeof(Size) );

	VerNorTex2* VertexBuf;

	CSyncSystem::TryToStop();
	CVertexHardwareBuffer* pBuffer;
	if (NULL==(pBuffer = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VerNorTex2),Size,HBU_DYNAMIC,0,true,false)))
		return -1;
	m_SkinBuffer.resize(Size);
	m_VertexBuf.vertexBufferBinding->SetBinding(0, pBuffer);
	m_VertexBuf.vertexCount =Size;
	m_VertexBuf.vertexStart = 0;
	VertexBuf = (VerNorTex2*)pBuffer->lock(	0, pBuffer->getSize(),HBL_NO_OVERWRITE );
	m_VertexBuf.vertexDeclaration = CVertexDeclaration::GET_VERDECL(VerNorTex2::Format);
	m_SkinInfo.m_pVertexBuffer = pBuffer;
	uint8	weight[4];

	CSyncSystem::TryToStop();
	switch (m_Channel)
	{
	case 0:
	case 1:
		if (!m_IsCompress)
		{
			for ( int i = 0; i < Size; i++ )
			{
				LoadVertexFull( *(VertexFomatFull*)&VertexBuf[i], SI, QuickFile );
				//GPU需要
				//VertexBuf[i].x *= 0.25f;
				//VertexBuf[i].y *= 0.25f;
				//VertexBuf[i].z *= 0.25f;

				//VertexBuf[i].tu *= 0.0005f;
				//VertexBuf[i].tv *= 0.0005f;

				QuickFile.read( (TCHAR*)weight, sizeof(weight) );
				QuickFile.read( (TCHAR*)&m_SkinBuffer[i].i, sizeof(SkltIndex) );
				m_SkinBuffer[i].w._w[0] = weight[0] * 0.005f;
				m_SkinBuffer[i].w._w[1] = weight[1] * 0.005f;
				m_SkinBuffer[i].w._w[2] = weight[2] * 0.005f;
				m_SkinBuffer[i].w._w[3] = weight[3] * 0.005f;
				mask.AddSkeletal( m_SkinBuffer[i].i.b[0] );
				mask.AddSkeletal( m_SkinBuffer[i].i.b[1] );
				mask.AddSkeletal( m_SkinBuffer[i].i.b[2] );
				mask.AddSkeletal( m_SkinBuffer[i].i.b[3] );
				CSyncSystem::TryToStop();
			}
		}
		else
		{
			for ( int i = 0; i < Size; i++ )
			{
				LoadVertex( *(VertexFomatFull*)&VertexBuf[i], SI, QuickFile );
				VertexBuf[i].np = _CovData::_s.m_fNorInd[ (int32)VertexBuf[i].np.x ];
				//GPU需要
				VertexBuf[i].p *= 0.25f;

				VertexBuf[i].tu *= 0.0005f;
				VertexBuf[i].tv *= 0.0005f;
				VertexBuf[i].tu1 = 0.0f;
				VertexBuf[i].tv1 = 0.0f;
				VertexBuf[i].tu2 = 0.0f;
				VertexBuf[i].tv2 = 0.0f;

				QuickFile.read( (TCHAR*)weight, sizeof(weight) );
				QuickFile.read( (TCHAR*)&m_SkinBuffer[i].i, sizeof(SkltIndex) );
				m_SkinBuffer[i].w._w[0] = weight[0] * 0.005f;
				m_SkinBuffer[i].w._w[1] = weight[1] * 0.005f;
				m_SkinBuffer[i].w._w[2] = weight[2] * 0.005f;
				m_SkinBuffer[i].w._w[3] = weight[3] * 0.005f;
				mask.AddSkeletal( m_SkinBuffer[i].i.b[0] );
				mask.AddSkeletal( m_SkinBuffer[i].i.b[1] );
				mask.AddSkeletal( m_SkinBuffer[i].i.b[2] );
				mask.AddSkeletal( m_SkinBuffer[i].i.b[3] );
				CSyncSystem::TryToStop();
			}
		}

		break;
	case 2:
		if (!m_IsCompress)
		{
			for ( int i = 0; i < Size; i++ )
			{
				LoadVertexFull1( *(VertexFomatFull*)&VertexBuf[i], SI, QuickFile );
				//VertexBuf[i].x *= 0.25f;
				//VertexBuf[i].y *= 0.25f;
				//VertexBuf[i].z *= 0.25f;

				//VertexBuf[i].tu *= 0.0005f;
				//VertexBuf[i].tv *= 0.0005f;
				//VertexBuf[i].tu1 *= 0.0005f;
				//VertexBuf[i].tv1 *= 0.0005f;

				QuickFile.read( (TCHAR*)weight, sizeof(weight) );
				QuickFile.read( (TCHAR*)&m_SkinBuffer[i].i, sizeof(SkltIndex) );
				m_SkinBuffer[i].w._w[0] = weight[0] * 0.005f;
				m_SkinBuffer[i].w._w[1] = weight[1] * 0.005f;
				m_SkinBuffer[i].w._w[2] = weight[2] * 0.005f;
				m_SkinBuffer[i].w._w[3] = weight[3] * 0.005f;
				mask.AddSkeletal( m_SkinBuffer[i].i.b[0] );
				mask.AddSkeletal( m_SkinBuffer[i].i.b[1] );
				mask.AddSkeletal( m_SkinBuffer[i].i.b[2] );
				mask.AddSkeletal( m_SkinBuffer[i].i.b[3] );
				CSyncSystem::TryToStop();
			}
		}
		else
		{
			for ( int i = 0; i < Size; i++ )
			{
				LoadVertex1( *(VertexFomatFull*)&VertexBuf[i], SI, QuickFile );
				VertexBuf[i].np= _CovData::_s.m_fNorInd[ (int32)VertexBuf[i].np.x ];
				VertexBuf[i].p *= 0.25f;
			

				VertexBuf[i].tu *= 0.0005f;
				VertexBuf[i].tv *= 0.0005f;
				VertexBuf[i].tu1 *= 0.0005f;
				VertexBuf[i].tv1 *= 0.0005f;
				VertexBuf[i].tu2 = 0.0f;
				VertexBuf[i].tv2 = 0.0f;

				QuickFile.read( (TCHAR*)weight, sizeof(weight) );
				QuickFile.read( (TCHAR*)&m_SkinBuffer[i].i, sizeof(SkltIndex) );
				m_SkinBuffer[i].w._w[0] = weight[0] * 0.005f;
				m_SkinBuffer[i].w._w[1] = weight[1] * 0.005f;
				m_SkinBuffer[i].w._w[2] = weight[2] * 0.005f;
				m_SkinBuffer[i].w._w[3] = weight[3] * 0.005f;
				mask.AddSkeletal( m_SkinBuffer[i].i.b[0] );
				mask.AddSkeletal( m_SkinBuffer[i].i.b[1] );
				mask.AddSkeletal( m_SkinBuffer[i].i.b[2] );
				mask.AddSkeletal( m_SkinBuffer[i].i.b[3] );
				CSyncSystem::TryToStop();
			}
		}

		break;
	default:
		if (!m_IsCompress)
		{
			for ( int i = 0; i < Size; i++ )
			{
				LoadVertexFull2( *(VertexFomatFull*)&VertexBuf[i], SI, QuickFile );
				//CVector3f n = _CovData::_s.m_fNorInd[ (int32)VertexBuf[i].n ];
				//VertexBuf[i].x *= 0.25f;
				//VertexBuf[i].y *= 0.25f;
				//VertexBuf[i].z *= 0.25f;

				//VertexBuf[i].tu *= 0.0005f;
				//VertexBuf[i].tv *= 0.0005f;
				//VertexBuf[i].tu1 *= 0.0005f;
				//VertexBuf[i].tv1 *= 0.0005f;
				//VertexBuf[i].tu2 *= 0.0005f;
				//VertexBuf[i].tv2 *= 0.0005f;

				QuickFile.read( (TCHAR*)weight, sizeof(weight) );
				QuickFile.read( (TCHAR*)&m_SkinBuffer[i].i, sizeof(SkltIndex) );
				m_SkinBuffer[i].w._w[0] = weight[0] * 0.005f;
				m_SkinBuffer[i].w._w[1] = weight[1] * 0.005f;
				m_SkinBuffer[i].w._w[2] = weight[2] * 0.005f;
				m_SkinBuffer[i].w._w[3] = weight[3] * 0.005f;
				mask.AddSkeletal( m_SkinBuffer[i].i.b[0] );
				mask.AddSkeletal( m_SkinBuffer[i].i.b[1] );
				mask.AddSkeletal( m_SkinBuffer[i].i.b[2] );
				mask.AddSkeletal( m_SkinBuffer[i].i.b[3] );
				CSyncSystem::TryToStop();
			}
		}
		else
		{
			for ( int i = 0; i < Size; i++ )
			{
				LoadVertex2( *(VertexFomatFull*)&VertexBuf[i], SI, QuickFile );
				VertexBuf[i].np = _CovData::_s.m_fNorInd[ (int32)VertexBuf[i].np.x ];
				VertexBuf[i].p *= 0.25f;

				VertexBuf[i].tu *= 0.0005f;
				VertexBuf[i].tv *= 0.0005f;
				VertexBuf[i].tu1 *= 0.0005f;
				VertexBuf[i].tv1 *= 0.0005f;
				VertexBuf[i].tu2 *= 0.0005f;
				VertexBuf[i].tv2 *= 0.0005f;

				QuickFile.read( (TCHAR*)weight, sizeof(weight) );
				QuickFile.read( (TCHAR*)&m_SkinBuffer[i].i, sizeof(SkltIndex) );
				m_SkinBuffer[i].w._w[0] = weight[0] * 0.005f;
				m_SkinBuffer[i].w._w[1] = weight[1] * 0.005f;
				m_SkinBuffer[i].w._w[2] = weight[2] * 0.005f;
				m_SkinBuffer[i].w._w[3] = weight[3] * 0.005f;
				mask.AddSkeletal( m_SkinBuffer[i].i.b[0] );
				mask.AddSkeletal( m_SkinBuffer[i].i.b[1] );
				mask.AddSkeletal( m_SkinBuffer[i].i.b[2] );
				mask.AddSkeletal( m_SkinBuffer[i].i.b[3] );
				CSyncSystem::TryToStop();
			}
		}

		break;
	}

	pBuffer->unlock();
	CSyncSystem::TryToStop();

	int ISize = 0;
	QuickFile.read( (TCHAR*)&ISize, sizeof(ISize) );
	IndexBuf.resize( ISize );

	for ( int j = 0; j < (int)IndexBuf.size(); j++ )
		QuickFile.read( (TCHAR*)&IndexBuf[j], Size < 256 ? sizeof(BYTE) : sizeof(WORD) );

	if (ISize==0)
		return 0;

	CSyncSystem::TryToStop();
	m_MeshLevels[0].GetIndexData()->indexCount = IndexBuf.size();
	m_MeshLevels[0].GetIndexData()->indexStart = 0;
	m_MeshLevels[0].GetIndexData()->indexBuffer = CHardwareBufferMgr::GetInst()->createIndexBuffer(IndexBuf.size(),HBU_WRITE_ONLY);
	m_MeshLevels[0].GetIndexData()->indexBuffer->write(	0,
		m_MeshLevels[0].GetIndexData()->indexBuffer->getSize(),
		&IndexBuf[0], true );

	return 0;
}
