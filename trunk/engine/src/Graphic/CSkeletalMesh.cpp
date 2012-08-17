#include "stdafx.h"
#include "CSkeletalMesh.h"
#include "CMeshLoader.h"
#include "CVertexDeclaration.h"
#include "CGraphic.h"
#include "CShaderConstParams.h"
#include "CRenderMatrix.h"
#include "SyncSystem.h"
#include "CRenderPipeline.h"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"

#define GETVERSION(VER)	0x0000ffff&VER
#define GETCP(VER)  (0x00010000&VER)>>16
#define GETBOX(VER)	 (0x00100000&VER)>>20
#define ENABLECP(VER)		0x00010000|VER
#define ENABLEBOX(VER)		0x00100000|VER
namespace sqr
{

void GpuSkeUpdate(CRenderCommand& Com,CRenderMatrix& AniInst)
{
	CSkeletalMesh::SkeletalParams* pParam = (CSkeletalMesh::SkeletalParams*)(Com.param_ptr);

	
	if (pParam->pBoneMap!=NULL&&!AniInst.m_pBoneMatrixlist.empty())
	{

		CMatrixUnit* boneList = (CMatrixUnit*)pParam->Bone;

#ifdef _USEVEC_
		BoneMap& rBoneMap = *pParam->pBoneMap;
		size_t Count = rBoneMap.size();

		for(size_t i = 0; i < Count; ++i)
			if( (size_t)rBoneMap[i] < AniInst.m_pBoneMatrixlist.size() )
				boneList[i] = AniInst.m_pBoneMatrixlist[rBoneMap[i]];
#else
		BoneMap::iterator iter,itere;
		iter = pParam->pBoneMap->begin();
		itere = pParam->pBoneMap->end();

		for ( ; iter != itere; ++iter )
			if( (size_t)iter->first < AniInst.m_pBoneMatrixlist.size() )
				boneList[iter->second] = AniInst.m_pBoneMatrixlist[iter->first];
#endif


	}
}

void GpuSkeRender(CRenderCommand& Com)
{
	CRenderPipeline::GetInst()->SetVertexShaderF(ACT_CUSTOM_PARAMS_LIST,(CSkeletalMesh::SkeletalParams*)(Com.param_ptr),((CSkeletalMesh::SkeletalParams*)(Com.param_ptr))->BoneSize);
	CRenderPipeline::GetInst()->SetTextureMatrixImmediate(0, NULL);//fix a d3d error
}

void GpuSkeRelease(CRenderCommand& Com)
{
	GfkSafeDelete(Com.param_ptr);
	Com.param_release = NullRelease;
}

CSkeletalMesh::CSkeletalMesh()
		: m_boneNum(0)
		, m_boneSize(0)
		, m_GpuBoneSize(0)
{
	m_VertexType = VT_SKELETALANI;
}

CSkeletalMesh::~CSkeletalMesh()
{
}

UINT CSkeletalMesh::GetRenderCommandNum(void)
{
	return mRenderPiece.size();
}

void CSkeletalMesh::CreateRenderCommand(CRenderCommand& Com, UINT Index)
{
	SkeSubMesh& rp = mRenderPiece[Index];
	bool NeedParams = Com.useIndexes && !rp.BonePool.empty();
	Com.vertexData = &m_VertexBuf;
	Com.indexData = m_MeshLevels[0].GetIndexData();
	Com.useIndexes = true;
	
	Com.indexData->indexStart = rp.Start;
	Com.indexData->indexCount = rp.Count;
	if( NeedParams )
	{
		SkeletalParams* pParam;
		Com.param_ptr = (TCHAR*)CParamsBufferMgr::GetInst()->setBuffer(sizeof(SkeletalParams),Com.param_size);
		Com.param_fun = GpuSkeUpdate;
		Com.param_render = GpuSkeRender;
		Com.param_release = GpuSkeRelease;
		AstMsg(Com.param_ptr, "骨骼参数不能为空");
		pParam = (SkeletalParams*)Com.param_ptr;
		pParam->pBoneMap =  &rp.BonePool;
		pParam->BoneSize = m_GpuBoneSize*2;
	}
	else
	{
		Com.param_fun = NullUpdate;
		Com.param_render = NullRender;
		Com.param_release = NullRelease;
		Com.param_ptr = NULL;
	}
}

FilterNode* CSkeletalMesh::Render( CRenderMatrix& ModelMatrix,FilterNode& FN )
{
	if(m_MeshLevels.size()==0)
		return NULL;

	STATIC_RS& m_RS = FN.m_RP.m_RS;
	FilterNode* pNode=NULL;
	
	for ( UINT I = 0; I < mRenderPiece.size(); ++I )
	{
		FN.useIndexes = !ModelMatrix.m_pBoneMatrixlist.empty();
		CreateRenderCommand(FN,I);

		if ( NULL!=FN.param_ptr )
			m_RS.m_ShaderType |= (MakeSysVS_ID(SSI_SKELETAL_ANI) + ((m_RS.m_UVSrc_S0 + m_RS.m_UVSrc_S1*2 + m_RS.m_UVSrc_S2*4 + m_RS.m_UVSrc_S3*8)>>16));

		FN.param_fun(FN,ModelMatrix);
		pNode = (FilterNode*)CGraphic::GetInst()->Insert(&FN);
		FN.param_fun = NullUpdate;
		FN.param_render = NullRender;
		FN.param_release = NullRelease;
	}
	return pNode;
}

void CSkeletalMesh::SetSkeletalMask(SKELETAL_MASK& Mask,
                                    GVector<VertexSkeletalFormat>& VertexBuf,
                                    GVector<uint16>&	IndexBuf)
{
	if (m_boneNum!=0)
		return;

	int testvalue = IndexBuf.size();
	INT32 id     = -1;
	INT32 index  = -1;
	INT32 boneid = 0;

	for ( int i = 0; i < (int)VertexBuf.size(); i++ )
	{	
		Mask.AddSkeletal( VertexBuf[i].i.b[0] );
		Mask.AddSkeletal( VertexBuf[i].i.b[1] );
		Mask.AddSkeletal( VertexBuf[i].i.b[2] );
		Mask.AddSkeletal( VertexBuf[i].i.b[3] );
	}

	m_boneNum  = Mask.GetSkeletalCount();
	m_boneSize = GpuMaxBoneNum*sizeof(CMatrixUnit);

	//已经处理过的骨骼索引
	GMap<int32 , int32 > loadbonelist;
	GMap<int32 , int32 > tIndexMap;
	loadbonelist[0] = 0;
	//先把原始数据拷贝出来
	GVector<VertexSkeletalFormat> SrcVertexBuf;
	SrcVertexBuf = VertexBuf;
	GVector<int32>				mindexinfo;
	GMap<int32, int32>			mvector;
	GMap<int32, int32>			mallvector;
	GMap<int32, int32>			mSrcId;

	SkeSubMesh value;
	value.Start		  = 0;
	int32 proindex   = 0;

	if ( m_boneNum > GpuMaxBoneNum )
	{
		m_GpuBoneSize = GpuMaxBoneNum;
		mindexinfo.resize( GpuMaxBoneNum + 1, 0);
		mindexinfo[0] = 1;
		int32 il = 0;
		while ( il < (int32)IndexBuf.size() )
		{
			if ( !IsAddface( il, loadbonelist, tIndexMap,SrcVertexBuf,VertexBuf, IndexBuf,mindexinfo,mvector,mallvector,mSrcId ) )
			{                                        
				GMap<int32, INT32>::iterator begin = mvector.begin();
				GMap<int32, INT32>::iterator end   = mvector.end();
				for ( ; begin != end; ++begin )
				{
					mallvector[begin->first] = begin->second;
				}
				mvector.clear();
				value.Count = il - proindex;
#ifdef _USEVEC_
				value.BonePool.resize(tIndexMap.size(),0);
				GMap<int32 , int32 >::iterator it,eit = tIndexMap.end();

				for( it = tIndexMap.begin(); it!=eit; ++it )
					value.BonePool[it->second] = it->first;
#else
				value.BonePool = tIndexMap;
#endif
				mRenderPiece.push_back( value );
				value.BonePool.clear();
				value.BonePool[0] = 0;
				value.Start		 = il;
				proindex         = il;
			}
			else
			{
				il += 3;
			}
			CSyncSystem::TryToStop();
		}
		mvector.clear();
		value.Count = IndexBuf.size() - proindex;
	}
	else
	{
		m_GpuBoneSize = m_boneNum;
		for ( int32 i = 0; i < (int32)VertexBuf.size(); ++i )
		{
			for ( int32 j = 0; j < 4; ++j )
			{
				if ( boneid != VertexBuf[i].i.b[j] )
				{
					int32 size = (int32)tIndexMap.size();
					boneid     = VertexBuf[i].i.b[j];
					if ( tIndexMap.end() == tIndexMap.find( boneid ) )
						tIndexMap[boneid] = size;
					id = tIndexMap[boneid];
				}
				VertexBuf[i].i.b[j]  = (BYTE)id;
			}
			CSyncSystem::TryToStop();
		}
		value.Start   = 0;
		value.Count = IndexBuf.size();
	
	}

#ifdef _USEVEC_
	value.BonePool.resize(tIndexMap.size(),0);
	GMap<int32 , int32 >::iterator it,eit = tIndexMap.end();

	for( it = tIndexMap.begin(); it!=eit; ++it )
		value.BonePool[it->second] = it->first;
#else
	value.BonePool = tIndexMap;
#endif
	mRenderPiece.push_back( value );
}

int CSkeletalMesh::BinLoad( CBufFile& QuickFile ,DWORD Version, SKELETAL_MASK& mask )
{
	GVector<VertexSkeletalFormat>	VertexBuf;
	GVector<uint16>					IndexBuf;
	CVector3f OrgScale,OrgOff;
	float fTextScale;
	DWORD tVersionCp,tVersionLow,tVersionBox;
	tVersionBox = GETBOX(Version);
	tVersionCp = GETCP(Version);
	tVersionLow = GETVERSION(Version);

	QuickFile.read( (char*)&m_fBaize, sizeof( m_fBaize ) );
	QuickFile.read( (char*)&OrgScale, sizeof( OrgScale ) );
	QuickFile.read( (char*)&OrgOff, sizeof( OrgOff ) );
	QuickFile.read( (char*)&m_Scale, sizeof( m_Scale ) );
	QuickFile.read( (char*)&m_Off, sizeof( m_Off ) );
	QuickFile.read( (char*)&fTextScale, sizeof( fTextScale ) );

	m_OrgScale = OrgScale;
	m_OrgOff = OrgOff;
	m_fTextScale = fTextScale;
	_RecordData(OrgScale,OrgOff,fTextScale);
	if (tVersionLow>=10005)
		QuickFile.read( (char*)&m_Channel,sizeof(m_Channel));

	if ((tVersionLow>=10007)&&tVersionCp)
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

	SaveInfo SI( OrgScale, OrgOff, 0, 0, fTextScale, m_VertexType );

	int Size;
	QuickFile.read( (char*)&Size, sizeof(Size) );
	m_MeshLevels.resize(Size);

	float Dist;
	QuickFile.read( (char*)&Dist, sizeof( Dist ) );
	QuickFile.read( (char*)&Size, sizeof(Size) );
	VertexBuf.resize(Size);

	m_VertexBuf.vertexDeclaration = CVertexDeclaration::GET_VERDECL(VertexSkeletalFormat::Format);
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
					LoadVertexFull( VertexBuf[i], SI, QuickFile );
					//GPU需要
					//VertexBuf[i].x *= 0.25f;
					//VertexBuf[i].y *= 0.25f;
					//VertexBuf[i].z *= 0.25f;

					//VertexBuf[i].u *= 0.0005f;
					//VertexBuf[i].v *= 0.0005f;

					QuickFile.read( (char*)weight, sizeof(weight) );
					QuickFile.read( (char*)&VertexBuf[i].i, sizeof(SkltIndex) );
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
					LoadVertex( VertexBuf[i], SI, QuickFile );
					CVector3f n = _CovData::_s.m_fNorInd[ (int32)VertexBuf[i].n ];
					//GPU需要
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

					QuickFile.read( (char*)weight, sizeof(weight) );
					QuickFile.read( (char*)&VertexBuf[i].i, sizeof(SkltIndex) );
					VertexBuf[i].w._w[0] = weight[0] * 0.005f;
					VertexBuf[i].w._w[1] = weight[1] * 0.005f;
					VertexBuf[i].w._w[2] = weight[2] * 0.005f;
					VertexBuf[i].w._w[3] = weight[3] * 0.005f;
					CSyncSystem::TryToStop();
				}
			}

			break;
		case 2:
			if (!m_IsCompress)
			{
				for ( int i = 0; i < Size; i++ )
				{
					LoadVertexFull1( VertexBuf[i], SI, QuickFile );
					//VertexBuf[i].x *= 0.25f;
					//VertexBuf[i].y *= 0.25f;
					//VertexBuf[i].z *= 0.25f;

					//VertexBuf[i].u *= 0.0005f;
					//VertexBuf[i].v *= 0.0005f;
					//VertexBuf[i].u1 *= 0.0005f;
					//VertexBuf[i].v1 *= 0.0005f;

					QuickFile.read( (char*)weight, sizeof(weight) );
					QuickFile.read( (char*)&VertexBuf[i].i, sizeof(SkltIndex) );
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
					LoadVertex1( VertexBuf[i], SI, QuickFile );
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

					QuickFile.read( (char*)weight, sizeof(weight) );
					QuickFile.read( (char*)&VertexBuf[i].i, sizeof(SkltIndex) );
					VertexBuf[i].w._w[0] = weight[0] * 0.005f;
					VertexBuf[i].w._w[1] = weight[1] * 0.005f;
					VertexBuf[i].w._w[2] = weight[2] * 0.005f;
					VertexBuf[i].w._w[3] = weight[3] * 0.005f;
					CSyncSystem::TryToStop();
				}
			}

			break;
		default:
			if (!m_IsCompress)
			{
				for ( int i = 0; i < Size; i++ )
				{
					LoadVertexFull2( VertexBuf[i], SI, QuickFile );
					//CVector3f n = _CovData::_s.m_fNorInd[ (int32)VertexBuf[i].n ];
					//VertexBuf[i].x *= 0.25f;
					//VertexBuf[i].y *= 0.25f;
					//VertexBuf[i].z *= 0.25f;

					//VertexBuf[i].u *= 0.0005f;
					//VertexBuf[i].v *= 0.0005f;
					//VertexBuf[i].u1 *= 0.0005f;
					//VertexBuf[i].v1 *= 0.0005f;
					//VertexBuf[i].u2 *= 0.0005f;
					//VertexBuf[i].v2 *= 0.0005f;

					QuickFile.read( (char*)weight, sizeof(weight) );
					QuickFile.read( (char*)&VertexBuf[i].i, sizeof(SkltIndex) );
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
					LoadVertex2( VertexBuf[i], SI, QuickFile );
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

					QuickFile.read( (char*)weight, sizeof(weight) );
					QuickFile.read( (char*)&VertexBuf[i].i, sizeof(SkltIndex) );
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
	QuickFile.read( (char*)&ISize, sizeof(ISize) );
	IndexBuf.resize( ISize );

	for ( int j = 0; j < (int)IndexBuf.size(); j++ )
		QuickFile.read( (char*)&IndexBuf[j], Size < 256 ? sizeof(BYTE) : sizeof(WORD) );

	if (ISize==0)
		return 0;

	CSyncSystem::TryToStop();
	_RecordVertex(VertexBuf);
	SetSkeletalMask(mask,VertexBuf,IndexBuf);

	CVertexHardwareBuffer* pVB;
	if (NULL==(pVB = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VertexSkeletalFormat),VertexBuf.size(),HBU_WRITE_ONLY)))
		return -1;
	m_VertexBuf.vertexBufferBinding->SetBinding(0, pVB);
	m_VertexBuf.vertexCount =VertexBuf.size();
	m_VertexBuf.vertexStart = 0;
	pVB->write(	0, pVB->getSize(),&(VertexBuf[0]), true );
	
	CSyncSystem::TryToStop();
	m_MeshLevels[0].GetIndexData()->indexCount = IndexBuf.size();
	m_MeshLevels[0].GetIndexData()->indexStart = 0;
	m_MeshLevels[0].GetIndexData()->indexBuffer = CHardwareBufferMgr::GetInst()->createIndexBuffer(IndexBuf.size(),HBU_WRITE_ONLY);
	m_MeshLevels[0].GetIndexData()->indexBuffer->write(	0,
	        m_MeshLevels[0].GetIndexData()->indexBuffer->getSize(),
	        &IndexBuf[0], true );

	return 0;
}


BOOL CSkeletalMesh::IsAddface( INT32& index, GMap<int32, int32>& loadbonelist,
                               GMap<int32, int32>& bonelist,
                               GVector<VertexSkeletalFormat>& SrcVertexBuf,
                               GVector<VertexSkeletalFormat>& VertexBuf,
                               GVector<uint16>&	IndexBuf,
                               GVector<int32>& mindexinfo,
                               GMap<int32, int32>& mvector,
                               GMap<int32, int32>& mallvector,
                               GMap<int32, int32>& mSrcId )
{
	GMap<int32, int32> bonevalueload;
	GMap<int32, int32> bonevalue;
	GMap<int32, int32> mtestvector;

	GMap<int32, int32>::iterator iter;
	GMap<int32, int32>::iterator alliter;

	bool  adddt  = false;

	int32 boneid = 0;
	GVector<VertexSkeletalFormat> mVerbuffer;
	GVector<VertexSkeletalFormat> mOldvec;

	mOldvec.resize( 3 );
	mVerbuffer.resize( VertexBuf.size() );
	mVerbuffer = VertexBuf;
	ZeroMemory( &mindexinfo[0], sizeof(int32)*(GpuMaxBoneNum+1) );

	{
		//Inituseinddex( bonelist );
		GMap<int32, int32>::iterator begin = bonelist.begin();
		GMap<int32, int32>::iterator end   = bonelist.end();
		for ( ; begin != end; ++begin )
		{
			if ( begin->second >= 0 )
			{
				mindexinfo[begin->second] = 1;
			}
		}
	}

	for ( int32 i = 0; i < 3; ++i )
	{
		const int32 in = IndexBuf[index+i];
		//没有处理过的顶点
		iter    = mvector.find( in );
		alliter = mallvector.find( in );
		mOldvec[i] = mVerbuffer[in];
		if ( iter == mvector.end() )
		{
			if ( alliter != mallvector.end() )
			{
				IndexBuf[index+i] = mVerbuffer.size();
				VertexSkeletalFormat ver;
				ver = SrcVertexBuf[in];
				mSrcId[IndexBuf[index+i]] = in;
				mVerbuffer.push_back( ver );
				VertexBuf.push_back( ver );
				--i;
				//GMap<int32, int32> testbonevalue;
				//BOOL bPro = FALSE;
				////如果在总的处理顶点中有被处理过
				//for( int32 j = 0; j < 4; ++j )
				//{
				//	boneid = mVerbuffer[in].i.b[j];
				//	if( 0 != boneid )
				//	{
				//		int   b0 = FindBone( bonevalue, boneid ) ;
				//		int   b1 = FindBone( bonelist,  boneid );
				//		int   b2 = FindBone( testbonevalue, boneid );
				//
				//		int32 bone = FindBone( loadbonelist, boneid );
				//		int32 num  = ( -1 == bone ? FindBone( bonevalueload, boneid ) : bone );
				//
				//		FindBoneIndex()
				//		if( num != b0 && num != b1 && num != b2 )
				//		{
				//			if( num != b0 && num != b1 && num != b2 )
				//			{
				//				//这里需要重新建立一个新的顶点出来,并且处理这个数据
				//				m_IndexBuf[index+i] = mVerbuffer.size();
				//				VertexSkeletalFormat ver;
				//				ver = m_SrcVertexBuf[in];
				//				mVerbuffer.push_back( ver );
				//				--i;
				//				bPro = TRUE;
				//				break;
				//			}
				//		}
				//		else
				//		{
				//			testbonevalue[num] = boneid;
				//		}
				//	}
				//}
				//if( !bPro )
				//{
				//	GMap<int32, int32>::iterator begin = testbonevalue.begin();
				//	GMap<int32, int32>::iterator end   = testbonevalue.end();
				//	for( ; begin != end; ++begin )
				//	{
				//		bonevalue[begin->first] = begin->second;
				//	}
				//	mtestvector[in] = -1;
				//}
			}
			else
			{
				VertexSkeletalFormat oldver = mVerbuffer[in];
				for ( int32 j = 0; j < 4; ++j )
				{
					if ( bonevalue.size() + bonelist.size() > GpuMaxBoneNum )
					{
						//还需要做一些离开后的处理,这个时候已经切了一部分了
						while ( i >= 0 )
						{
							mVerbuffer[in] = mOldvec[i];
							//m_IndexBuf[index+i] = m_ScrIndexBuf[index+i];
							--i;
						}
						mtestvector.clear();
						return FALSE;
					}
					boneid = mVerbuffer[in].i.b[j];
					if ( !IsLoadBone( bonelist, boneid ) && !( IsLoadBone( bonevalue, boneid ) ) )
					{
						if ( !IsLoadBone( loadbonelist, boneid ) && !IsLoadBone( bonevalueload, boneid ) )
						{
							//如果这块骨骼在总的骨骼中也没有找到那直接加入就可以了
							int num = -1;//GetUnuseindex();

							{
								for ( int i = 1; i < GpuMaxBoneNum+1; ++i )
								{
									if ( 1 != mindexinfo[i] )
									{
										mindexinfo[i] = 1;
										num = i;
										break;
									}
								}
							}
							mVerbuffer[in].i.b[j] = num;
							bonevalue[boneid] = num;
							bonevalueload[boneid] = num;
							mtestvector[in] = -1;

						}
						else
						{
							int32 index1 = FindBoneIndex( loadbonelist, boneid );
							int32 index2 = FindBoneIndex( bonevalueload, boneid );
							int num = -1 != index1 ? index1 : index2;
							if ( -1 == FindBone( bonevalue, num ) &&
							        -1 == FindBone( bonelist,  num ) )
							{
								//如果这块骨骼在总的骨骼索引,但是在我当前的之中没有索引
								//Setuseindex( num );
								if ( 0 == mindexinfo[num] )
									mindexinfo[num] = 1;
								mVerbuffer[in].i.b[j] = num;
								bonevalue[boneid] = num;
								mtestvector[in] = -1;
							}
							else
							{
								//还需要做一些离开后的处理,这个时候已经切了一部分了
								int id = -1;//GetUnuseindex();
								{
									for ( int i = 1; i < GpuMaxBoneNum+1; ++i )
									{
										if ( 1 != mindexinfo[i] )
										{
											mindexinfo[i] = 1;
											id = i;
											break;
										}
									}
								}
								if ( -1 != id )
								{
									mVerbuffer[in].i.b[j] = id;
									bonevalue[boneid] = id;
									mtestvector[in] = -1;
								}
								else
								{
									while ( i >= 0 )
									{
										//因为有可能索引已经改变了
										mVerbuffer[in] = mOldvec[i];
										//									m_IndexBuf[index+i] = m_ScrIndexBuf[index+i];
										--i;
									}
									return FALSE;
								}
								////恢复这个点已经处理了的数据
								//mVerbuffer[in] = oldver;

								////这里需要重新建立一个新的顶点出来
								//m_IndexBuf[index+i] = mVerbuffer.size();
								//int id = -1;
								//VertexSkeletalFormat ver;
								//if( in < m_SrcVertexBuf.size() )
								//{
								//	id  = in;
								//	ver = m_SrcVertexBuf[in];
								//}
								//else
								//{
								//	id  = mSrcId[in];
								//	ver = m_SrcVertexBuf[id];
								//}
								//mSrcId[m_IndexBuf[index+i]] = id;
								//mVerbuffer.push_back( ver );
								//--i;
								//break;
							}
						}
					}
					else
					{
						//已经了有的骨骼
						int bi = FindBoneIndex( bonevalue, boneid );
						mVerbuffer[in].i.b[j] = -1 == bi ? FindBoneIndex( bonelist, boneid ) : bi;
						mtestvector[in] = -1;
					}
				}
			}
		}
		else
		{
			if ( -1 != iter->second )
			{
				//如果不等于，那表示已经生成一个新的顶点数据了
				IndexBuf[index+1] = iter->second;
			}
		}
	}
	//数据复制出来
	GMap<int32, int32>::iterator begin = bonevalueload.begin();
	GMap<int32, int32>::iterator end   = bonevalueload.end();
	for ( ; begin != end; ++begin )
	{
		loadbonelist[begin->first] = begin->second;
	}
	begin = bonevalue.begin();
	end	  = bonevalue.end();
	for ( ; begin != end; ++begin )
	{
		bonelist[begin->first] = begin->second;
	}

	begin = mtestvector.begin();
	end   = mtestvector.end();
	for ( ; begin != end; ++begin )
	{
		mvector[begin->first] = begin->second;
	}

	//需要把数据复制过来
	VertexBuf = mVerbuffer;
	return TRUE;
}
//---static-------
BOOL  CSkeletalMesh::IsLoadBone( GMap<int32, int32>& loadbonelist, INT32& boneid )
{
	GMap<int32, int32>::iterator begin = loadbonelist.begin();
	GMap<int32, int32>::iterator end   = loadbonelist.end();
	for ( ; begin != end; ++begin )
	{
		if ( boneid == begin->first )
			return TRUE;
	}
	return FALSE;
}

int32 CSkeletalMesh::FindBone( GMap<int32, int32>& loadbonelist, INT32& boneid )
{
	GMap<int32, int32>::iterator begin = loadbonelist.begin();
	GMap<int32, int32>::iterator end   = loadbonelist.end();
	for ( ; begin != end; ++begin )
	{
		if ( boneid == begin->second )
			return begin->first;
	}
	return -1;
}

int32 CSkeletalMesh::FindBoneIndex( GMap<int32, int32>& loadbonelist, INT32& boneid )
{
	GMap<int32, int32>::iterator begin = loadbonelist.begin();
	GMap<int32, int32>::iterator end   = loadbonelist.end();
	for ( ; begin != end; ++begin )
	{
		if ( boneid == begin->first )
			return begin->second;
	}
	return -1;
}
}
