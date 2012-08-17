#include "stdafx.h"
#include "CMeshLevel.h"
#include "CHardwareBufferMgr.h"
#include "SyncSystem.h"
namespace sqr
{
CMeshLevel::~CMeshLevel()
{
	if (m_IndexBuf.indexBuffer)
		CHardwareBufferMgr::GetInst()->DestroyBuffer(m_IndexBuf.indexBuffer);
}

int CMeshLevel::BinLoad(  CBufFile& QuickFile, size_t IBSize ) //, SaveInfo& SI ,int m_Channel )
{
	int Size;
	QuickFile.read( (char*)&Size, sizeof(Size) );
	m_IndexBuf.indexCount = Size;
	m_IndexBuf.indexStart = 0;

	m_IndexBuf.indexBuffer = CHardwareBufferMgr::GetInst()->createIndexBuffer(Size,HBU_WRITE_ONLY);
	AstMsg(m_IndexBuf.indexBuffer, "IB 读取失败");
	WORD* pIB = (WORD*)m_IndexBuf.indexBuffer->lock(HBL_NO_OVERWRITE);

	CSyncSystem::TryToStop();
	for ( int j = 0; j < Size; j++ )
	{
		*(pIB + j) = 0;
		QuickFile.read( (char*)(pIB + j), IBSize );
		CSyncSystem::TryToStop();
	}
	CSyncSystem::TryToStop();
	m_IndexBuf.indexBuffer->unlock();

	return 0;
}

void  CMeshLevel::_FreeRes(void)
{
	if(m_IndexBuf.indexBuffer)
		m_IndexBuf.indexBuffer->freeRes();
}

void  CMeshLevel::_Reclaim(void)
{
	if(m_IndexBuf.indexBuffer)
		m_IndexBuf.indexBuffer->updateRes();
}

/*千万不要删，虽然我们永远用不到，但需要的时候下面的代码是最直接的指导
bool CMeshLevel::GetTexture0()
{
	return NULL == m_RenderNode.m_RP.m_RS.m_Texture_S0 ? false : true;
}

byte CMeshLevel::GetTexturealpha( float& u, float& v, const int32& index )
{
	byte alpha = 0x01;
	D3DLOCKED_RECT stLockedRect;
	D3DSURFACE_DESC desc;
	((LPDIRTEX)(m_RenderNode.m_RP.m_RS.m_Texture_S0->GetBuffer()))->GetLevelDesc( 0, &desc );
	((LPDIRTEX)(m_RenderNode.m_RP.m_RS.m_Texture_S0->GetBuffer()))->LockRect( 0, &stLockedRect, 0, 0 );

	int32 U  = int32( u * desc.Width );
	int32 V  = int32( v * desc.Height );
	int32 in = V * desc.Width + U;
	if( D3DFMT_A8R8G8B8 == desc.Format )
	{
		int32 *pch=(int32*)stLockedRect.pBits;
		int32 temp = pch[in];
		alpha = byte( temp >> 24 );
	}
	else if( D3DFMT_A4R4G4B4 == desc.Format )
	{
		int16 *pch=(int16*)stLockedRect.pBits;
		int16 temp = pch[in];
		alpha = byte( temp >> 12 );
	}
	if( 0x00 == alpha )
		alpha = 0x02;
	else
		alpha = 0x01;
	((LPDIRTEX)(m_RenderNode.m_RP.m_RS.m_Texture_S0->GetBuffer()))->UnlockRect(0);
	return alpha;
}

int	CMeshLevel::GetVerNum()
{
	return m_RenderNode.m_Op.vertexData->vertexCount;
}

D3DXVECTOR3 CMeshLevel::GetVertex( int nNum )
{
	VertexHardwareBuffer* pVB = m_RenderNode.m_Op.vertexData->vertexBufferBinding->getBuffer(0);
	D3DXVECTOR3 RetVec;
	pVB->read(pVB->getVertexSize()*nNum,sizeof(D3DXVECTOR3),&RetVec);
	return RetVec;
}

D3DXVECTOR2 CMeshLevel::GetVertexUV( int32 nNum )
{
	VertexHardwareBuffer* pVB = m_RenderNode.m_Op.vertexData->vertexBufferBinding->getBuffer(0);
	D3DXVECTOR2 RetVec;
	pVB->read(pVB->getVertexSize()*nNum+sizeof(CVector3f)*2,sizeof(D3DXVECTOR2),&RetVec);
	return RetVec;
}

D3DXVECTOR3 CMeshLevel::GetNormal( int32 nNum )
{
	VertexHardwareBuffer* pVB = m_RenderNode.m_Op.vertexData->vertexBufferBinding->getBuffer(0);
	D3DXVECTOR3 RetVec;
	pVB->read(pVB->getVertexSize()*nNum+sizeof(CVector3f),sizeof(D3DXVECTOR3),&RetVec);
	return RetVec;
}

CStaticLevel::CStaticLevel()
{
	m_RenderNode.m_Op.Create();
}
CStaticLevel::~CStaticLevel()
{
	//if(NULL!=m_RenderNode.m_Op.vertexData)
	//	m_RenderNode.m_Op.vertexData->vertexBufferBinding.DestroyAllBuffer();
	m_RenderNode.m_Op.Destroy();
}

CVertexAniLevel::~CVertexAniLevel()
{
	GVector<LevelAni>::iterator ita = m_MeshFrame.begin();
	GVector<LevelAni>::iterator itae = m_MeshFrame.end();
	GVector<VertexHardwareBuffer*>::iterator itb,itbe;
	for(;ita!=itae;++ita)
	{
		itb = (*ita).m_FrameBuffer.begin();
		itbe = (*ita).m_FrameBuffer.end();
		for(;itb!=itbe;++itb)
			CHardwareBufferMgr::GetInst()->DestroyBuffer(*itb);
	}

	m_RenderNode.m_Op.vertexData->vertexBufferBinding->unsetAllBindings();
}

void CStaticLevel::ReadIndex( CBufFile& GraphicFile )
{
	int Size;
	GraphicFile.read( (char*)&Size, sizeof(Size) );
	m_IndexBuf.resize( Size );
	for( int j = 0; j < (int)m_IndexBuf.size(); j++ )
		GraphicFile.read( (char*)&m_IndexBuf[j], GetVerNum() < 256 ? sizeof(BYTE) : sizeof(WORD) );
}

void CSkeletalLevel::SetSkeletalMask( SKELETAL_MASK& Mask )
{

}

BOOL CSkeletalLevel::IsAddface( INT32& index, GMap<int32, int32>& loadbonelist,
	GMap<int32, int32>& bonelist,
	GVector<VertexSkeletalFormat>& SrcVertexBuf,
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
	mVerbuffer.resize( m_VertexBuf.size() );
	mVerbuffer = m_VertexBuf;
	ZeroMemory( &mindexinfo[0], sizeof(int32)*(usermaxboneNum+1) );

	{
		//Inituseinddex( bonelist );
		GMap<int32, int32>::iterator begin = bonelist.begin();
		GMap<int32, int32>::iterator end   = bonelist.end();
		for( ; begin != end; ++begin )
		{
			if( begin->second >= 0 )
			{
				mindexinfo[begin->second] = 1;
			}
		}
	}

	for( int32 i = 0; i < 3; ++i )
	{
		const int32 in = m_IndexBuf[index+i];
		//没有处理过的顶点
		iter    = mvector.find( in );
		alliter = mallvector.find( in );
		mOldvec[i] = mVerbuffer[in];
		if( iter == mvector.end() )
		{
			if( alliter != mallvector.end() )
			{
				m_IndexBuf[index+i] = mVerbuffer.size();
				VertexSkeletalFormat ver;
				ver = SrcVertexBuf[in];
				mSrcId[m_IndexBuf[index+i]] = in;
				mVerbuffer.push_back( ver );
				m_VertexBuf.push_back( ver );
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
				for( int32 j = 0; j < 4; ++j )
				{
					if( bonevalue.size() + bonelist.size() > usermaxboneNum )
					{
						//还需要做一些离开后的处理,这个时候已经切了一部分了
						while( i >= 0 )
						{
							mVerbuffer[in] = mOldvec[i];
							//m_IndexBuf[index+i] = m_ScrIndexBuf[index+i];
							--i;
						}
						mtestvector.clear();
						return FALSE;
					}
					boneid = mVerbuffer[in].i.b[j];
					if( !IsLoadBone( bonelist, boneid ) && !( IsLoadBone( bonevalue, boneid ) ) )
					{
						if( !IsLoadBone( loadbonelist, boneid ) && !IsLoadBone( bonevalueload, boneid ) )
						{
							//如果这块骨骼在总的骨骼中也没有找到那直接加入就可以了
							int num = -1;//GetUnuseindex();

							{
								for( int i = 1; i < usermaxboneNum+1; ++i )
								{
									if( 1 != mindexinfo[i] )
									{
										mindexinfo[i] = 1;
										num = i;
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
							if( -1 == FindBone( bonevalue, num ) &&
								-1 == FindBone( bonelist,  num ) )
							{
								//如果这块骨骼在总的骨骼索引,但是在我当前的之中没有索引
								//Setuseindex( num );
								if( 0 == mindexinfo[num] )
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
									for( int i = 1; i < usermaxboneNum+1; ++i )
									{
										if( 1 != mindexinfo[i] )
										{
											mindexinfo[i] = 1;
											id = i;
										}
									}
								}
								if( -1 != id )
								{
									mVerbuffer[in].i.b[j] = id;
									bonevalue[boneid] = id;
									mtestvector[in] = -1;
								}
								else
								{
									while( i >= 0 )
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
			if( -1 != iter->second )
			{
				//如果不等于，那表示已经生成一个新的顶点数据了
				m_IndexBuf[index+1] = iter->second;
			}
		}
	}
	//数据复制出来
	GMap<int32, int32>::iterator begin = bonevalueload.begin();
	GMap<int32, int32>::iterator end   = bonevalueload.end();
	for( ; begin != end; ++begin )
	{
		loadbonelist[begin->first] = begin->second;
	}
	begin = bonevalue.begin();
	end	  = bonevalue.end();
	for( ; begin != end; ++begin )
	{
		bonelist[begin->first] = begin->second;
	}

	begin = mtestvector.begin();
	end   = mtestvector.end();
	for( ; begin != end; ++begin )
	{
		mvector[begin->first] = begin->second;
	}

	//需要把数据复制过来
	m_VertexBuf = mVerbuffer;
	return TRUE;
}

BOOL CSkeletalLevel::IsLoadBone( GMap<int32, int32>& loadbonelist, INT32& boneid )
{
	GMap<int32, int32>::iterator begin = loadbonelist.begin();
	GMap<int32, int32>::iterator end   = loadbonelist.end();
	for( ; begin != end; ++begin )
	{
		if( boneid == begin->first )
			return TRUE;
	}
	return FALSE;
}

int32 CSkeletalLevel::FindBoneIndex( GMap<int32, int32>& loadbonelist, INT32& boneid )
{
	GMap<int32, int32>::iterator begin = loadbonelist.begin();
	GMap<int32, int32>::iterator end   = loadbonelist.end();
	for( ; begin != end; ++begin )
	{
		if( boneid == begin->first )
			return begin->second;
	}
	return -1;
}

int32 CSkeletalLevel::FindBone( GMap<int32, int32>& loadbonelist, INT32& boneid )
{
	GMap<int32, int32>::iterator begin = loadbonelist.begin();
	GMap<int32, int32>::iterator end   = loadbonelist.end();
	for( ; begin != end; ++begin )
	{
		if( boneid == begin->second )
			return begin->first;
	}
	return -1;
}

// void CSkeletalLevel::Inituseinddex( GMap<int32, int32>& bonelist )
// {
// 	GMap<int32, int32>::iterator begin = bonelist.begin();
// 	GMap<int32, int32>::iterator end   = bonelist.end();
// 	for( ; begin != end; ++begin )
// 	{
// 		if( begin->second >= 0 )
// 		{
// 			mindexinfo[begin->second] = 1;
// 		}
// 	}
// }

// int32 CSkeletalLevel::GetUnuseindex()
// {
// 	for( int i = 1; i < usermaxboneNum+1; ++i )
// 	{
// 		if( 1 != mindexinfo[i] )
// 		{
// 			mindexinfo[i] = 1;
// 			return i;
// 		}
// 	}
// 	return -1;
// }

// void CSkeletalLevel::Setuseindex( const int32 index )
// {
// 	if( 0 == mindexinfo[index] )
// 		mindexinfo[index] = 1;
// }

bool CVertexStaticLevel::Render(DX9Graphic* pCraphic, CPieceRenderStyle* CurRS, uint8 SkeletalNum,
	CRenderMatrix& ModelMatrix, RenderObjStyle* pRORS, void* pUser)
{
	float fCurFrame = ModelMatrix.m_CurFrame;
	//CRenderGroup& GroupMgr = CRenderGroup::getSingleton();

	m_RenderNode.m_eType = pRORS->m_eType;
	m_RenderNode.m_Op = m_RenderNode.m_Op;

	STATIC_RS&		m_RS = m_RenderNode.m_RP.m_RS;
	D3DXMATRIX&		mat = m_RenderNode.m_RP.m_Mat;

	if( !pRORS )
	{

		//if( CurRS->IsAlphaBlend( fCurFrame ) != AlphaBlend )
		//	return 0;

		m_RS.m_Material.Diffuse.a	= pMat->Diffuse.a;
		m_RS.m_Alpharef			= (uint32)( m_RS.m_Alpharef*m_RS.m_Material.Diffuse.a );
		m_RS.m_AlpOP_S0			= D3DTOP_MODULATE;
		m_RS.m_AlpArg1_S0			= D3DTA_TEXTURE;
		m_RS.m_AlpArg2_S0			= D3DTA_DIFFUSE;
		m_RenderNode.m_eType = (ERenderObjStyle)(m_RenderNode.m_eType | ROS_ALPHA);
		if( m_RS.m_AlphaBlendEnable == FALSE )
		{
			m_RS.m_AlphaBlendEnable	= TRUE;
			m_RS.m_SrcBlend 		= D3DBLEND_SRCALPHA;
			m_RS.m_DestBlend 		= D3DBLEND_INVSRCALPHA;
		}
	}



		CurRS->GetCurRS( m_RS, fCurFrame, pUser );
		mat = ModelMatrix.m_MatWorld;//.m_pMatrixUnit[0]->m_CurCMatrixUnit;
	}
	else if( ROS_SHADOW == pRORS->m_eType )
	{

		//if( !AlphaBlend )
		//	return 0;

		m_RS.m_Material.Ambient.r *= pMat->Ambient.r;
		m_RS.m_Material.Ambient.g *= pMat->Ambient.g;
		m_RS.m_Material.Ambient.b *= pMat->Ambient.b;
	}
	if( pRORS->m_nParam1&DELTARS_SPECULAR )
	{
		m_RS.m_Material.Specular.r *= pMat->Specular.r;
		m_RS.m_Material.Specular.g *= pMat->Specular.g;
		m_RS.m_Material.Specular.b *= pMat->Specular.b;
	}
	if( pRORS->m_nParam1&DELTARS_EMISSIVE )
	{
		m_RS.m_Material.Emissive.r *= pMat->Emissive.r;
		m_RS.m_Material.Emissive.g *= pMat->Emissive.g;
		m_RS.m_Material.Emissive.b *= pMat->Emissive.b;
	}
	if( pRORS->m_nParam1&DELTARS_SPECPOWER )
	{
		m_RS.m_Material.Power *= pMat->Power;
	}
	mat  = ModelMatrix.m_MatWorld;//	D3DXMatrixIdentity(&mat);
}
else if (ROS_REFRACT & pRORS->m_eType)
{
	if( CurRS )
		CurRS->GetCurRS( m_RS, fCurFrame, (void*)pUser );
	m_RenderNode.pRenderStyle = CurRS;
	m_RenderNode.pUser = pUser;
	Material* pMat = (Material*)pRORS->m_nParam2;
	if( pRORS->m_nParam1&DELTARS_DIFFUSE )
	{
		m_RS.m_Material.Diffuse.r *= pMat->Diffuse.r;
		m_RS.m_Material.Diffuse.g *= pMat->Diffuse.g;
		m_RS.m_Material.Diffuse.b *= pMat->Diffuse.b;
	}
	mat  = ModelMatrix.m_MatWorld;//	D3DXMatrixIdentity(&mat);
}
else
	return 0;


		DELTASH* pDSH = (DELTASH*)pRORS->m_nParam2;


		if( CurRS )
			CurRS->GetCurRS( m_RS, fCurFrame, (void*)pUser );

		uint32 uCull       = m_RS.m_Cull;
		ITexture* pTexture = m_RS.m_Texture_S0;
		m_RS					= pCraphic->GetStaticRS( RS_SHADOW );
		m_RS.m_Texture_S0		= pTexture;
		m_RS.m_Texturefactor	= pDSH->dwColor;
		m_RS.m_Alpharef		= uint32( ( pDSH->dwColor >> 24 )*0.5f );
		m_RS.m_ZTestFun		= pRORS->m_nParam1&DELTASH_ZTEST ? D3DCMP_LESSEQUAL : D3DCMP_ALWAYS;
		m_RS.m_ZWrite				= pRORS->m_nParam1&DELTASH_ZWRITE ? TRUE : FALSE;
		m_RS.m_Cull            = uCull;

		mat = ModelMatrix.m_MatWorld*(*((D3DXMATRIX*)&pDSH->matShadow));
	}
	else if( ROS_LIGHTMAT1 & pRORS->m_eType || ROS_LIGHTMAT2 & pRORS->m_eType  )
	{
		//if( ( ( pRORS->m_nParam1&DELTARS_ALPHADIF ) || CurRS->IsAlphaBlend( fCurFrame ) ) != AlphaBlend )
		//return 0;

		CurRS->GetCurRS( m_RS, fCurFrame, (void*)pUser );
		m_RenderNode.pRenderStyle = CurRS;
		m_RenderNode.pUser = pUser;
		//m_RS.m_ZTestEnable	= TRUE;			//=== 自己加
		//m_RS.m_ZWrite			= TRUE;			//===
		//m_RS.m_ZTestFun		= D3DCMP_LESS;		//===

		Material* pMat = (Material*)pRORS->m_nParam2;
		if( pRORS->m_nParam1&DELTARS_ALPHADIF )
		{
			m_RS.m_Material.Diffuse.a	= pMat->Diffuse.a;
			m_RS.m_Alpharef			= (uint32)( m_RS.m_Alpharef*m_RS.m_Material.Diffuse.a );
			m_RS.m_AlpOP_S0			= D3DTOP_MODULATE;
			m_RS.m_AlpArg1_S0			= D3DTA_TEXTURE;
			m_RS.m_AlpArg2_S0			= D3DTA_DIFFUSE;
			m_RenderNode.m_eType = (ERenderObjStyle)(m_RenderNode.m_eType | ROS_ALPHA);
			if( m_RS.m_AlphaBlendEnable == FALSE )
			{
				m_RS.m_AlphaBlendEnable	= TRUE;
				m_RS.m_SrcBlend 			= D3DBLEND_SRCALPHA;
				m_RS.m_DestBlend 			= D3DBLEND_INVSRCALPHA;
			}

		}
		if( pRORS->m_nParam1&DELTARS_DIFFUSE )
		{
			m_RS.m_Material.Diffuse.r *= pMat->Diffuse.r;
			m_RS.m_Material.Diffuse.g *= pMat->Diffuse.g;
			m_RS.m_Material.Diffuse.b *= pMat->Diffuse.b;
		}
		if( pRORS->m_nParam1&DELTARS_AMBIENT )
		{
			m_RS.m_Material.Ambient.r *= pMat->Ambient.r;
			m_RS.m_Material.Ambient.g *= pMat->Ambient.g;
			m_RS.m_Material.Ambient.b *= pMat->Ambient.b;
		}
		if( pRORS->m_nParam1&DELTARS_SPECULAR )
		{
			m_RS.m_Material.Specular.r *= pMat->Specular.r;
			m_RS.m_Material.Specular.g *= pMat->Specular.g;
			m_RS.m_Material.Specular.b *= pMat->Specular.b;
		}
		if( pRORS->m_nParam1&DELTARS_EMISSIVE )
		{
			m_RS.m_Material.Emissive.r *= pMat->Emissive.r;
			m_RS.m_Material.Emissive.g *= pMat->Emissive.g;
			m_RS.m_Material.Emissive.b *= pMat->Emissive.b;
		}
		if( pRORS->m_nParam1&DELTARS_SPECPOWER )
		{
			m_RS.m_Material.Power *= pMat->Power;
		}

		mat = ModelMatrix.m_MatWorld;
	}
	else
		return 0;

	CRenderPiece* pRenderPiece = (CRenderPiece*)pUser;

	if( pRenderPiece->m_RS.m_nMask&PIECERS_DIFFUSE )
	{
		m_RS.m_Material.Diffuse.a *= float(pRenderPiece->m_RS.m_Diffuse.A) / 255.0f;
		m_RS.m_Material.Diffuse.r *= float(pRenderPiece->m_RS.m_Diffuse.R) / 255.0f;
		m_RS.m_Material.Diffuse.g *= float(pRenderPiece->m_RS.m_Diffuse.G) / 255.0f;
		m_RS.m_Material.Diffuse.b *= float(pRenderPiece->m_RS.m_Diffuse.B) / 255.0f;

		if( m_RS.m_Material.Diffuse.a < 1.0f )
		{
			m_RS.m_AlphaBlendEnable	= TRUE;
			m_RS.m_SrcBlend			= D3DBLEND_SRCALPHA;
			m_RS.m_DestBlend 			= D3DBLEND_INVSRCALPHA;
			m_RS.m_AlpOP_S0			= D3DTOP_MODULATE;
		}
	}
	if( pRenderPiece->m_RS.m_nMask&PIECERS_AMBIENT )
	{
		m_RS.m_Material.Ambient.r *=  float(pRenderPiece->m_RS.m_Ambient.R) / 255.0f;
		m_RS.m_Material.Ambient.g *=  float(pRenderPiece->m_RS.m_Ambient.G) / 255.0f;
		m_RS.m_Material.Ambient.b *=  float(pRenderPiece->m_RS.m_Ambient.B) / 255.0f;
	}
	if( pRenderPiece->m_RS.m_nMask&PIECERS_SPECULAR )
	{
		m_RS.m_Material.Specular.r = max(  float(pRenderPiece->m_RS.m_Specular.R) / 255.0f, m_RS.m_Material.Specular.r );
		m_RS.m_Material.Specular.g = max(  float(pRenderPiece->m_RS.m_Specular.G) / 255.0f, m_RS.m_Material.Specular.g );
		m_RS.m_Material.Specular.b = max(  float(pRenderPiece->m_RS.m_Specular.B) / 255.0f, m_RS.m_Material.Specular.b );
		m_RS.m_Material.Power	    = pRenderPiece->m_RS.m_fPower;
		m_RS.m_SpecularEnable		= TRUE;
	}
	if( pRenderPiece->m_RS.m_nMask&PIECERS_SECONDTEX )
	{
		m_RS.m_Texture_S1		= pRenderPiece->m_RS.m_pTexture;//( ( CDx9Texture* )( (ITexture*)pRenderPiece->m_RS.m_pTexture ) )->GetD3DTexture();
		m_RS.m_Texturefactor	= pRenderPiece->m_RS.m_BlendAlpha;
		m_RS.m_TexOP_S1		= D3DTOP_BLENDFACTORALPHA;
		m_RS.m_TexArg1_S1	= D3DTA_TEXTURE;
		m_RS.m_TexArg2_S1	= D3DTA_CURRENT;
		m_RS.m_AlpOP_S1		= D3DTOP_SELECTARG1;
		m_RS.m_AlpArg1_S1	= D3DTA_CURRENT;
	}
	if (pRenderPiece->m_RS.m_nMask &PIECERS_CLIPPLANE)
	{
		m_RS.m_ClipPlane = pRenderPiece->m_RS.m_ClipPlane;
	}
	else
	{
		ZeroMemory(&m_RS.m_ClipPlane, sizeof(m_RS.m_ClipPlane));
	}


	mat = ModelMatrix.m_MatWorld;
}
else if (ROS_REFRACT & pRORS->m_eType)
{
	if( CurRS )
		CurRS->GetCurRS( m_RS, fCurFrame, (void*)pUser );
	m_RenderNode.pRenderStyle = CurRS;
	m_RenderNode.pUser = pUser;
	Material* pMat = (Material*)pRORS->m_nParam2;
	if( pRORS->m_nParam1&DELTARS_DIFFUSE )
	{
		m_RS.m_Material.Diffuse.r *= pMat->Diffuse.r;
		m_RS.m_Material.Diffuse.g *= pMat->Diffuse.g;
		m_RS.m_Material.Diffuse.b *= pMat->Diffuse.b;
	}
	mat  = ModelMatrix.m_MatWorld;//	D3DXMatrixIdentity(&mat);
}
else
	return 0;

m_RS.m_fRefractIndex = pRenderPiece->m_RS.m_fRefractIndex;

	return true;
	//return CRenderPipeline::GetInst()->Render(&m_RenderNode.m_Op,&rp);
}

bool CVertexAniLevel::Render(DX9Graphic* pCraphic, CPieceRenderStyle* CurRS, uint8 SkeletalNum,
	CRenderMatrix& ModelMatrix, RenderObjStyle* pRORS, void* pUser)
{
	float fCurFrame = ModelMatrix.m_CurFrame;
	m_RenderNode.m_eType = pRORS->m_eType;
	//fn.m_pVD = m_RenderNode.m_Op.vertexData->vertexDeclaration;

	STATIC_RS&		m_RS = m_RenderNode.m_RP.m_RS;
	D3DXMATRIX&		mat = m_RenderNode.m_RP.m_Mat;

	if( !pRORS )
	{
		//if( CurRS->IsAlphaBlend( fCurFrame ) != AlphaBlend )
		//return 0;


		CurRS->GetCurRS( m_RS, fCurFrame, pUser );
		D3DXMatrixIdentity( &mat );
	}
	else if( ROS_SHADOW == pRORS->m_eType )
	{
		//if( !AlphaBlend )
		//return 0;

		DELTASH* pDSH = (DELTASH*)pRORS->m_nParam2;


		if( CurRS )
			CurRS->GetCurRS( m_RS, fCurFrame, (void*)pUser );

		uint32 uCull       = m_RS.m_Cull;
		ITexture* pTexture = m_RS.m_Texture_S0;
		m_RS					= pCraphic->GetStaticRS( RS_SHADOW );
		m_RS.m_Texture_S0		= pTexture;
		m_RS.m_Texturefactor	= pDSH->dwColor;
		m_RS.m_Alpharef		= uint32( ( pDSH->dwColor >> 24 )*0.5f );
		m_RS.m_ZTestFun		= pRORS->m_nParam1&DELTASH_ZTEST ? D3DCMP_LESSEQUAL : D3DCMP_ALWAYS;
		m_RS.m_ZWrite				= pRORS->m_nParam1&DELTASH_ZWRITE ? TRUE : FALSE;
		m_RS.m_Cull            = uCull;

		mat = *((D3DXMATRIX*)&pDSH->matShadow);
	}
	else if( ROS_LIGHTMAT1 & pRORS->m_eType)
	{
		//if( ( ( pRORS->m_nParam1&DELTARS_ALPHADIF ) || CurRS->IsAlphaBlend( fCurFrame ) ) != AlphaBlend )
		//return 0;

		CurRS->GetCurRS( m_RS, fCurFrame, (void*)pUser );
		m_RenderNode.pRenderStyle = CurRS;
		m_RenderNode.pUser = pUser;
		//m_RS.m_ZTestEnable	= TRUE;			//=== 自己加
		//m_RS.m_ZWrite			= TRUE;			//===
		//m_RS.m_ZTestFun		= D3DCMP_LESS;		//===

		Material* pMat = (Material*)pRORS->m_nParam2;
		if( pRORS->m_nParam1&DELTARS_ALPHADIF || ROS_ALPHA & pRORS->m_eType)
		{
			//if(0.9f<pMat->Diffuse.a)
			//{
			//	 pRORS->m_eType = (ERenderObjStyle)(pRORS->m_eType^ROS_ALPHA);
			//	 m_RS.m_Material.Diffuse.a = 1.0f;
			//	 m_RS.m_AlphaBlendEnable = FALSE;
			//}
			//else
			//{
			m_RS.m_Material.Diffuse.a	= pMat->Diffuse.a;
			m_RS.m_Alpharef			= (uint32)( m_RS.m_Alpharef*m_RS.m_Material.Diffuse.a );
			m_RS.m_AlpOP_S0			= D3DTOP_MODULATE;
			m_RS.m_AlpArg1_S0			= D3DTA_TEXTURE;
			m_RS.m_AlpArg2_S0			= D3DTA_DIFFUSE;
			m_RenderNode.m_eType = (ERenderObjStyle)(m_RenderNode.m_eType | ROS_ALPHA);
			if( m_RS.m_AlphaBlendEnable == FALSE )
			{
				m_RS.m_AlphaBlendEnable	= TRUE;
				m_RS.m_SrcBlend 		= D3DBLEND_SRCALPHA;
				m_RS.m_DestBlend 		= D3DBLEND_INVSRCALPHA;
			}
			//}

		}
		if( pRORS->m_nParam1&DELTARS_DIFFUSE )
		{
			m_RS.m_Material.Diffuse.r *= pMat->Diffuse.r;
			m_RS.m_Material.Diffuse.g *= pMat->Diffuse.g;
			m_RS.m_Material.Diffuse.b *= pMat->Diffuse.b;
		}
		if( pRORS->m_nParam1&DELTARS_AMBIENT )
		{
			m_RS.m_Material.Ambient.r *= pMat->Ambient.r;
			m_RS.m_Material.Ambient.g *= pMat->Ambient.g;
			m_RS.m_Material.Ambient.b *= pMat->Ambient.b;
		}
		if( pRORS->m_nParam1&DELTARS_SPECULAR )
		{
			m_RS.m_Material.Specular.r *= pMat->Specular.r;
			m_RS.m_Material.Specular.g *= pMat->Specular.g;
			m_RS.m_Material.Specular.b *= pMat->Specular.b;
		}
		if( pRORS->m_nParam1&DELTARS_EMISSIVE )
		{
			m_RS.m_Material.Emissive.r *= pMat->Emissive.r;
			m_RS.m_Material.Emissive.g *= pMat->Emissive.g;
			m_RS.m_Material.Emissive.b *= pMat->Emissive.b;
		}
		if( pRORS->m_nParam1&DELTARS_SPECPOWER )
		{
			m_RS.m_Material.Power *= pMat->Power;
		}
		mat  = ModelMatrix.m_MatWorld;

	}
	else
		return 0;

	CRenderPiece* pRenderPiece = (CRenderPiece*)pUser;

	if( pRenderPiece->m_RS.m_nMask&PIECERS_DIFFUSE )
	{
		m_RS.m_Material.Diffuse.a *= float(pRenderPiece->m_RS.m_Diffuse.A) / 255.0f;
		m_RS.m_Material.Diffuse.r *= float(pRenderPiece->m_RS.m_Diffuse.R) / 255.0f;
		m_RS.m_Material.Diffuse.g *= float(pRenderPiece->m_RS.m_Diffuse.G) / 255.0f;
		m_RS.m_Material.Diffuse.b *= float(pRenderPiece->m_RS.m_Diffuse.B) / 255.0f;

		if( m_RS.m_Material.Diffuse.a < 1.0f )
		{
			m_RS.m_AlphaBlendEnable	= TRUE;
			m_RS.m_SrcBlend			= D3DBLEND_SRCALPHA;
			m_RS.m_DestBlend 			= D3DBLEND_INVSRCALPHA;
			m_RS.m_AlpOP_S0			= D3DTOP_MODULATE;
		}
	}
	if( pRenderPiece->m_RS.m_nMask&PIECERS_AMBIENT )
	{
		m_RS.m_Material.Ambient.r *= float(pRenderPiece->m_RS.m_Ambient.R) / 255.0f;
		m_RS.m_Material.Ambient.g *= float(pRenderPiece->m_RS.m_Ambient.G) / 255.0f;
		m_RS.m_Material.Ambient.b *= float(pRenderPiece->m_RS.m_Ambient.B) / 255.0f;
	}
	if( pRenderPiece->m_RS.m_nMask&PIECERS_SPECULAR )
	{
		m_RS.m_Material.Specular.r = max( float(pRenderPiece->m_RS.m_Specular.R) / 255.0f, m_RS.m_Material.Specular.r );
		m_RS.m_Material.Specular.g = max( float(pRenderPiece->m_RS.m_Specular.G) / 255.0f, m_RS.m_Material.Specular.g );
		m_RS.m_Material.Specular.b = max( float(pRenderPiece->m_RS.m_Specular.B) / 255.0f, m_RS.m_Material.Specular.b );
		m_RS.m_Material.Power	    = pRenderPiece->m_RS.m_fPower;
		m_RS.m_SpecularEnable		= TRUE;
	}
	if( pRenderPiece->m_RS.m_nMask&PIECERS_SECONDTEX )
	{
		m_RS.m_Texture_S1		= pRenderPiece->m_RS.m_pTexture;//( ( CDx9Texture* )( (ITexture*)pRenderPiece->m_RS.m_pTexture ) )->GetD3DTexture();
		m_RS.m_Texturefactor	= pRenderPiece->m_RS.m_BlendAlpha;
		m_RS.m_TexOP_S1		= D3DTOP_BLENDFACTORALPHA;
		m_RS.m_TexArg1_S1	= D3DTA_TEXTURE;
		m_RS.m_TexArg2_S1	= D3DTA_CURRENT;
		m_RS.m_AlpOP_S1		= D3DTOP_SELECTARG1;
		m_RS.m_AlpArg1_S1	= D3DTA_CURRENT;
	}
	if (pRenderPiece->m_RS.m_nMask &PIECERS_CLIPPLANE)
	{
		m_RS.m_ClipPlane = pRenderPiece->m_RS.m_ClipPlane;
	}
	else
	{
		ZeroMemory(&m_RS.m_ClipPlane, sizeof(m_RS.m_ClipPlane));
	}

	VertexAniParams* Param = (VertexAniParams*)CParamsBufferMgr::GetInst()->setBuffer(sizeof(VertexAniParams),m_RenderNode.param_size);
	m_RenderNode.param_ptr = (char*)Param;
	m_RenderNode.param_type = VERTEX_ANI;
	Param->pAniVector = &m_MeshFrame;
	Param->pRenderPiece = pRenderPiece;
	Param->iPiece = SkeletalNum;
	WORD AniNo = pRenderPiece->m_Pieces[SkeletalNum].m_CurPiecesAniNo;
	uint StartFrame,EndFrame;
	float PreFrameKeyNum = float( ModelMatrix.m_CurFrame )/m_MeshFrame[AniNo].m_FramePerKeyFrame;
	StartFrame = min( (uint)PreFrameKeyNum, (uint)m_MeshFrame[AniNo].m_FrameVertex.size() - 2 );
	EndFrame = StartFrame  + 1;
	Param->Weight = ( EndFrame*m_MeshFrame[AniNo].m_FramePerKeyFrame - ModelMatrix.m_CurFrame ) /m_MeshFrame[AniNo].m_FramePerKeyFrame;
	Param->InvWeight = 1.0f - Param->Weight;
	m_RenderNode.m_RP.m_RS.m_ShaderType |= veraniID;
	m_RenderNode.m_Op.vertexData->vertexBufferBinding->setBinding(0,m_MeshFrame[AniNo].m_FrameBuffer[StartFrame]);
	m_RenderNode.m_Op.vertexData->vertexBufferBinding->setBinding(1,m_MeshFrame[AniNo].m_FrameBuffer[EndFrame]);
	CGraphic::GetInst()->Insert(&m_RenderNode);
	return true;
}

else if (ROS_REFRACT & pRORS->m_eType)
{
	if( CurRS )
		CurRS->GetCurRS( m_RS, fCurFrame, (void*)pUser );
	m_RenderNode.pRenderStyle = CurRS;
	m_RenderNode.pUser = pUser;
	Material* pMat = (Material*)pRORS->m_nParam2;
	if( pRORS->m_nParam1&DELTARS_DIFFUSE )
	{
		m_RS.m_Material.Diffuse.r *= pMat->Diffuse.r;
		m_RS.m_Material.Diffuse.g *= pMat->Diffuse.g;
		m_RS.m_Material.Diffuse.b *= pMat->Diffuse.b;
	}
	mat  = ModelMatrix.m_MatWorld;//	D3DXMatrixIdentity(&mat);
}
else


////////////////////////////////////////////////////////////////////////
// 功能：根据格式返回相应的当前计算过的顶点
// 参数：void* DestBuf				要写入的缓冲区,
//		 uint32 Format				要返回的格式,
//       uint32 Level                 LOD层,
//		 uint32 stride				格式的跨度,
//       const D3DXMATRIX* matMesh	矩阵列表,
//		 float Frame				要计算的帧
// 返回：已写入的缓冲区
//////////////////////////////////////////////////////////////////////////
//const void* CVertexAniLevel::CalMeshVertex( CGraphic* pGraphic, void* DestBuf, uint32 Format, uint32 stride,
//										   CRenderMatrix& ModelMatrix, CRenderPiece* pRenderPiece, int32 iPiece ) const
//{
//	WORD AniNo = pRenderPiece->m_Pieces[iPiece].m_CurPiecesAniNo;
//	WORD PreNo = AniNo;
//	int StartFrame,EndFrame;
//	float Weight;
//
//	if( ModelMatrix.m_IsAniBlend && m_MeshFrame.size() > 1 )
//	{
//		PreNo = pRenderPiece->m_Pieces[iPiece].m_PrePiecesAniNo;
//		float PreFrameKeyNum = float( pRenderPiece->m_PreFrameNo )/m_MeshFrame[PreNo].m_FramePerKeyFrame;
//		StartFrame = min( (int)PreFrameKeyNum, (int)m_MeshFrame[PreNo].m_FrameVertex.size() - 1 );
//		float CurFrameKeyNum = float( ModelMatrix.m_AniStartFrame )/m_MeshFrame[PreNo].m_FramePerKeyFrame;
//		EndFrame = min( (int)CurFrameKeyNum, (int)m_MeshFrame[AniNo].m_FrameVertex.size() - 1 );
//		Weight = ModelMatrix.m_IsRended ? ( ModelMatrix.m_BlendFrame - ModelMatrix.m_CurFrame )/ModelMatrix.m_BlendFrame : 0.0f;
//		if( ModelMatrix.m_CurFrame >= ModelMatrix.m_BlendFrame )
//			Weight = 0.0f;
//	}
//	else
//	{
//		float FrameKeyNum = ModelMatrix.m_CurFrame/m_MeshFrame[PreNo].m_FramePerKeyFrame;
//		StartFrame = min( (int)FrameKeyNum, (int)m_MeshFrame[AniNo].m_FrameVertex.size() - 1 );
//		EndFrame = min( StartFrame + 1, (int)m_MeshFrame[AniNo].m_FrameVertex.size() - 1 );
//		Weight = 1 + StartFrame - FrameKeyNum;
//		if( StartFrame < 0 )
//			StartFrame = 0;
//		if( EndFrame < 0 )
//			EndFrame = 0;
//
//		if( StartFrame == EndFrame )
//			Weight = 1.0f;
//	}
//
//	Weight *= 0.25f;
//	float InvWeight = 0.25f - Weight;
//	if( VerNorTex::Format == Format  )
//	{
//		VerNorTex* OutBuf = (VerNorTex*)DestBuf;
//		GVector<VertexFomat>::const_iterator itPre = m_MeshFrame[PreNo].m_FrameVertex[StartFrame].begin();
//		GVector<VertexFomat>::const_iterator itNxt = m_MeshFrame[AniNo].m_FrameVertex[EndFrame].begin();
//		GVector<VertexFomat>::const_iterator itEnd = m_MeshFrame[PreNo].m_FrameVertex[StartFrame].end();
//
//		for( ; itPre != itEnd; itPre++, itNxt++, OutBuf++)
//		{
//			OutBuf->p.x = (*itPre).x*Weight + (*itNxt).x*InvWeight;
//			OutBuf->p.y = (*itPre).y*Weight + (*itNxt).y*InvWeight;
//			OutBuf->p.z = (*itPre).z*Weight + (*itNxt).z*InvWeight;
//			OutBuf->np  = ((CVector3f&)_CovData::_s.m_fNorInd[ (*itPre).n ])*Weight + ((CVector3f&)_CovData::_s.m_fNorInd[ (*itNxt).n ])*InvWeight;
//			//OutBuf->tu = (*itTex).u*0.0005f;
//			//OutBuf->tv = (*itTex).v*0.0005f;
//		}
//	}
//	else if(VerNorTex1::Format == Format)
//	{
//		VerNorTex1* OutBuf = (VerNorTex1*)DestBuf;
//		GVector<VertexFomat>::const_iterator itPre = m_MeshFrame[PreNo].m_FrameVertex[StartFrame].begin();
//		GVector<VertexFomat>::const_iterator itNxt = m_MeshFrame[AniNo].m_FrameVertex[EndFrame].begin();
//		GVector<VertexFomat>::const_iterator itEnd = m_MeshFrame[PreNo].m_FrameVertex[StartFrame].end();
//
//
//		for( ; itPre != itEnd; itPre++, itNxt++, OutBuf++)
//		{
//			OutBuf->p.x = (*itPre).x*Weight + (*itNxt).x*InvWeight;
//			OutBuf->p.y = (*itPre).y*Weight + (*itNxt).y*InvWeight;
//			OutBuf->p.z = (*itPre).z*Weight + (*itNxt).z*InvWeight;
//			OutBuf->np  = ((CVector3f&)_CovData::_s.m_fNorInd[ (*itPre).n ])*Weight + ((CVector3f&)_CovData::_s.m_fNorInd[ (*itNxt).n ])*InvWeight;
//		///	OutBuf->tu = (*itTex).u*0.0005f;
//		///	OutBuf->tv = (*itTex).v*0.0005f;
//			//OutBuf->tu1 = (*itTex1).u*0.0005f;
//			//OutBuf->tv1 = (*itTex1).v*0.0005f;
//		}
//	}
//	else if(VerNorTex2::Format == Format)
//	{
//		VerNorTex2* OutBuf = (VerNorTex2*)DestBuf;
//		GVector<VertexFomat>::const_iterator itPre = m_MeshFrame[PreNo].m_FrameVertex[StartFrame].begin();
//		GVector<VertexFomat>::const_iterator itNxt = m_MeshFrame[AniNo].m_FrameVertex[EndFrame].begin();
//		GVector<VertexFomat>::const_iterator itEnd = m_MeshFrame[PreNo].m_FrameVertex[StartFrame].end();
//
//
//		for( ; itPre != itEnd; itPre++, itNxt++, OutBuf++)
//		{
//			OutBuf->p.x = (*itPre).x*Weight + (*itNxt).x*InvWeight;
//			OutBuf->p.y = (*itPre).y*Weight + (*itNxt).y*InvWeight;
//			OutBuf->p.z = (*itPre).z*Weight + (*itNxt).z*InvWeight;
//			OutBuf->np  = ((CVector3f&)_CovData::_s.m_fNorInd[ (*itPre).n ])*Weight + ((CVector3f&)_CovData::_s.m_fNorInd[ (*itNxt).n ])*InvWeight;
//			//OutBuf->tu = (*itTex).u*0.0005f;
//			//OutBuf->tv = (*itTex).v*0.0005f;
//			//OutBuf->tu1 = (*itTex1).u*0.0005f;
//			//OutBuf->tv1 = (*itTex1).v*0.0005f;
//			//OutBuf->tu2 = (*itTex2).u*0.0005f;
//			//OutBuf->tv2 = (*itTex2).v*0.0005f;
//		}
//	}
//	else
//	{
//		VerTex* OutBuf = (VerTex*)DestBuf;
//		GVector<VertexFomat>::const_iterator itPre = m_MeshFrame[PreNo].m_FrameVertex[StartFrame].begin();
//		GVector<VertexFomat>::const_iterator itNxt = m_MeshFrame[AniNo].m_FrameVertex[EndFrame].begin();
//		GVector<VertexFomat>::const_iterator itEnd = m_MeshFrame[PreNo].m_FrameVertex[StartFrame].end();
//		//GVector<TextCoord>::const_iterator   itTex = m_TextCoord.begin();
//
//		for( ; itPre != itEnd; itPre++, itNxt++, OutBuf++ )
//		{
//			OutBuf->p.x = (*itPre).x*Weight + (*itNxt).x*InvWeight;
//			OutBuf->p.y = (*itPre).y*Weight + (*itNxt).y*InvWeight;
//			OutBuf->p.z = (*itPre).z*Weight + (*itNxt).z*InvWeight;
//			//OutBuf->tu = (*itTex).u*0.0005f;
//			//OutBuf->tv = (*itTex).v*0.0005f;
//		}
//	}
//
//	return DestBuf;
//}
m_RS.m_fRefractIndex = pRenderPiece->m_RS.m_fRefractIndex;

//const void* CVertexAniLevel::CalMeshVertex( CGraphic* pGraphic, void* DestBuf, uint32 Format, uint32 stride,
//										   const D3DXMATRIX* ModelMatrix, uint8 uSkeletalNum ) const
//{
//	if( VerNorTex::Format == Format  )
//	{
//		VerNorTex* OutBuf = (VerNorTex*)DestBuf;
//		GVector<VertexFomat>::const_iterator itPre = m_MeshFrame[0].m_FrameVertex[0].begin();
//		GVector<VertexFomat>::const_iterator itEnd = m_MeshFrame[0].m_FrameVertex[0].end();
//		GVector<TextCoord>::const_iterator   itTex = m_TextCoord.begin();
//
//		for( ; itPre != itEnd; itPre++, OutBuf++, itTex++ )
//		{
//			OutBuf->p.x = (*itPre).x*0.25f;
//			OutBuf->p.y = (*itPre).y*0.25f;
//			OutBuf->p.z = (*itPre).z*0.25f;
//			OutBuf->np  = (CVector3f&)_CovData::_s.m_fNorInd[ (*itPre).n ];
//			OutBuf->tu = (*itTex).u*0.0005f;
//			OutBuf->tv = (*itTex).v*0.0005f;
//		}
//	}
//	else if(VerNorTex1::Format == Format)
//	{
//		VerNorTex1* OutBuf = (VerNorTex1*)DestBuf;
//		GVector<VertexFomat>::const_iterator itPre = m_MeshFrame[0].m_FrameVertex[0].begin();
//		GVector<VertexFomat>::const_iterator itEnd = m_MeshFrame[0].m_FrameVertex[0].end();
//		GVector<TextCoord>::const_iterator   itTex = m_TextCoord.begin();
//		GVector<TextCoord1>::const_iterator   itTex1 = m_TextCoord1.begin();
//
//		for( ; itPre != itEnd; itPre++, OutBuf++, itTex++ )
//		{
//			OutBuf->p.x = (*itPre).x*0.25f;
//			OutBuf->p.y = (*itPre).y*0.25f;
//			OutBuf->p.z = (*itPre).z*0.25f;
//			OutBuf->np  = (CVector3f&)_CovData::_s.m_fNorInd[ (*itPre).n ];
//			OutBuf->tu = (*itTex).u*0.0005f;
//			OutBuf->tv = (*itTex).v*0.0005f;
//			OutBuf->tu1 = (*itTex1).u*0.0005f;
//			OutBuf->tv1 = (*itTex1).v*0.0005f;
//		}
//	}
//	else if(VerNorTex2::Format == Format)
//	{
//		VerNorTex2* OutBuf = (VerNorTex2*)DestBuf;
//		GVector<VertexFomat>::const_iterator itPre = m_MeshFrame[0].m_FrameVertex[0].begin();
//		GVector<VertexFomat>::const_iterator itEnd = m_MeshFrame[0].m_FrameVertex[0].end();
//		GVector<TextCoord>::const_iterator   itTex = m_TextCoord.begin();
//		GVector<TextCoord1>::const_iterator   itTex1 = m_TextCoord1.begin();
//		GVector<TextCoord2>::const_iterator   itTex2 = m_TextCoord2.begin();
//		for( ; itPre != itEnd; itPre++, OutBuf++, itTex++ )
//		{
//			OutBuf->p.x = (*itPre).x*0.25f;
//			OutBuf->p.y = (*itPre).y*0.25f;
//			OutBuf->p.z = (*itPre).z*0.25f;
//			OutBuf->np  = (CVector3f&)_CovData::_s.m_fNorInd[ (*itPre).n ];
//			OutBuf->tu = (*itTex).u*0.0005f;
//			OutBuf->tv = (*itTex).v*0.0005f;
//			OutBuf->tu1 = (*itTex1).u*0.0005f;
//			OutBuf->tv1 = (*itTex1).v*0.0005f;
//			OutBuf->tu2 = (*itTex2).u*0.0005f;
//			OutBuf->tv2 = (*itTex2).v*0.0005f;
//		}
//	}
//	else
//	{
//		VerTex* OutBuf = (VerTex*)DestBuf;
//		GVector<VertexFomat>::const_iterator itPre = m_MeshFrame[0].m_FrameVertex[0].begin();
//		GVector<VertexFomat>::const_iterator itEnd = m_MeshFrame[0].m_FrameVertex[0].end();
//		GVector<TextCoord>::const_iterator   itTex = m_TextCoord.begin();
//
//		for( ; itPre != itEnd; itPre++, OutBuf++, itTex++ )
//		{
//			OutBuf->p.x = (*itPre).x*0.25f;
//			OutBuf->p.y = (*itPre).y*0.25f;
//			OutBuf->p.z = (*itPre).z*0.25f;
//			OutBuf->tu = (*itTex).u*0.0005f;
//			OutBuf->tv = (*itTex).v*0.0005f;
//		}
//	}
//
//	return DestBuf;
//}

CSkeletalLevel::CSkeletalLevel()
	:m_boneNum(0)
	,m_boneSize(0)
	,m_GpuBoneSize(0)
{

}

CSkeletalLevel::~CSkeletalLevel()
{
	//if( mIdlist.size() > 1)
	//	SafeDelAry( mBonelist )
	//else
	//SafeDelAry( mBonelist )
}


int CSkeletalLevel::BinLoad( CBufFile& GraphicFile, SaveInfo& SI ,int m_Channel )
{
	int Size;
	GraphicFile.read( (char*)&m_Dist, sizeof( m_Dist ) );
	GraphicFile.read( (char*)&Size, sizeof(Size) );
	m_VertexBuf.resize(Size);
	m_RenderNode.m_Op.Create();
	//m_RenderNode.m_Op.vertexData->vertexCount = Size;
	//m_RenderNode.m_Op.vertexData->vertexStart = 0;
	m_RenderNode.m_Op.vertexData->vertexDeclaration = CVertexDeclaration::GET_VERDECL(VertexSkeletalFormat::Format);
	uint8	weight[4];
	switch(m_Channel)
	{
	case 0:
	case 1:
		for( int i = 0; i < Size; i++ )
		{
			LoadVertex( m_VertexBuf[i], SI, GraphicFile );
			CVector3f n = _CovData::_s.m_fNorInd[ (int32)m_VertexBuf[i].n ];
			//GPU需要
			m_VertexBuf[i].x *= 0.25f;
			m_VertexBuf[i].y *= 0.25f;
			m_VertexBuf[i].z *= 0.25f;

			m_VertexBuf[i].n  = n.x;
			m_VertexBuf[i].ny = n.y;
			m_VertexBuf[i].nz = n.z;

			m_VertexBuf[i].u *= 0.0005f;
			m_VertexBuf[i].v *= 0.0005f;

			GraphicFile.read( (char*)weight, sizeof(weight) );
			GraphicFile.read( (char*)&m_VertexBuf[i].i, sizeof(SkltIndex) );
			m_VertexBuf[i].w._w[0] = weight[0] * 0.005f;
			m_VertexBuf[i].w._w[1] = weight[1] * 0.005f;
			m_VertexBuf[i].w._w[2] = weight[2] * 0.005f;
			m_VertexBuf[i].w._w[3] = weight[3] * 0.005f;
		}
		break;
	case 2:
		for( int i = 0; i < Size; i++ )
		{
			LoadVertex1( m_VertexBuf[i], SI, GraphicFile );
			CVector3f n = _CovData::_s.m_fNorInd[ (int32)m_VertexBuf[i].n ];
			m_VertexBuf[i].x *= 0.25f;
			m_VertexBuf[i].y *= 0.25f;
			m_VertexBuf[i].z *= 0.25f;
			m_VertexBuf[i].n  = n.x;
			m_VertexBuf[i].ny = n.y;
			m_VertexBuf[i].nz = n.z;

			m_VertexBuf[i].u *= 0.0005f;
			m_VertexBuf[i].v *= 0.0005f;
			m_VertexBuf[i].u1 *= 0.0005f;
			m_VertexBuf[i].v1 *= 0.0005f;

			GraphicFile.read( (char*)weight, sizeof(weight) );
			GraphicFile.read( (char*)&m_VertexBuf[i].i, sizeof(SkltIndex) );
			m_VertexBuf[i].w._w[0] = weight[0] * 0.005f;
			m_VertexBuf[i].w._w[1] = weight[1] * 0.005f;
			m_VertexBuf[i].w._w[2] = weight[2] * 0.005f;
			m_VertexBuf[i].w._w[3] = weight[3] * 0.005f;
		}
		break;
	default:
		for( int i = 0; i < Size; i++ )
		{
			LoadVertex2( m_VertexBuf[i], SI, GraphicFile );
			CVector3f n = _CovData::_s.m_fNorInd[ (int32)m_VertexBuf[i].n ];
			m_VertexBuf[i].x *= 0.25f;
			m_VertexBuf[i].y *= 0.25f;
			m_VertexBuf[i].z *= 0.25f;
			m_VertexBuf[i].n  = n.x;
			m_VertexBuf[i].ny = n.y;
			m_VertexBuf[i].nz = n.z;

			m_VertexBuf[i].u *= 0.0005f;
			m_VertexBuf[i].v *= 0.0005f;
			m_VertexBuf[i].u1 *= 0.0005f;
			m_VertexBuf[i].v1 *= 0.0005f;
			m_VertexBuf[i].u2 *= 0.0005f;
			m_VertexBuf[i].v2 *= 0.0005f;

			GraphicFile.read( (char*)weight, sizeof(weight) );
			GraphicFile.read( (char*)&m_VertexBuf[i].i, sizeof(SkltIndex) );
			m_VertexBuf[i].w._w[0] = weight[0] * 0.005f;
			m_VertexBuf[i].w._w[1] = weight[1] * 0.005f;
			m_VertexBuf[i].w._w[2] = weight[2] * 0.005f;
			m_VertexBuf[i].w._w[3] = weight[3] * 0.005f;
		}
		break;
	}
	GraphicFile.read( (char*)&Size, sizeof(Size) );

	m_RenderNode.m_Op.indexData->indexCount = Size;
	m_RenderNode.m_Op.indexData->indexStart = 0;

	m_IndexBuf.resize( Size );
	for( int j = 0; j < (int)m_IndexBuf.size(); j++ )
		GraphicFile.read( (char*)&m_IndexBuf[j], GetVerNum() < 256 ? sizeof(BYTE) : sizeof(WORD) );

	if(Size==0)
		return 0;

	if(NULL==(m_RenderNode.m_Op.indexData->indexBuffer =
		CHardwareBufferMgr::GetInst()->createIndexBuffer(Size,HBU_STATIC_WRITE_ONLY)))
		return 0;



int CVertexStaticLevel::BinLoad( CBufFile& GraphicFile, SaveInfo& SI ,int m_Channel )
{
int Size;
GraphicFile.read( (char*)&m_Dist, sizeof( m_Dist ) );
GraphicFile.read( (char*)&Size, sizeof(Size) );
if(Size==0)
	return 0;
}

int CVertexStaticLevel::BinLoad( CBufFile& GraphicFile, SaveInfo& SI ,int m_Channel )
{

case 0:
case 1:
	{
 		m_RenderNode.m_Op.vertexData->vertexDeclaration =  CVertexDeclaration::GET_VERDECL(VerNorTex::Format);
 		if(NULL==(pVB = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VerNorTex),Size,HBU_STATIC_WRITE_ONLY)))
 			return 0;
 		VerNorTex* OutBuf = (VerNorTex*)pVB->lock(HBL_NO_OVERWRITE);
 		for( int i = 0; i < Size; i++,OutBuf++ )
 		{

 			LoadVertex( VertexBuf, SI, GraphicFile );
 			OutBuf->p.x = VertexBuf.x*0.25f;
 			OutBuf->p.y = VertexBuf.y*0.25f;
 			OutBuf->p.z = VertexBuf.z*0.25f;
 			OutBuf->np  = (CVector3f&)_CovData::_s.m_fNorInd[ VertexBuf.n ];
 			OutBuf->tu = VertexBuf.u*0.0005f;
 			OutBuf->tv = VertexBuf.v*0.0005f;
 		}
 		pVB->unlock();
		//m_RenderNode.m_Op.vertexData->vertexDeclaration =  CVertexDeclaration::GET_VERDECL(VerNorDiffuseTex::Format);
		//if(NULL==(pVB = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VerNorDiffuseTex),Size,HBU_STATIC_WRITE_ONLY)))
		//	return 0;
		//VerNorDiffuseTex* OutBuf = (VerNorDiffuseTex*)pVB->lock(HBL_NO_OVERWRITE);
		//for( int i = 0; i < Size; i++,OutBuf++ )
		//{

		//	LoadVertex( VertexBuf, SI, GraphicFile );
		//	OutBuf->p.x = VertexBuf.x*0.25f;
		//	OutBuf->p.y = VertexBuf.y*0.25f;
		//	OutBuf->p.z = VertexBuf.z*0.25f;
		//	OutBuf->np  = (CVector3f&)_CovData::_s.m_fNorInd[ VertexBuf.n ];
		//	OutBuf->tu = VertexBuf.u*0.0005f;
		//	OutBuf->tv = VertexBuf.v*0.0005f;
		//	OutBuf->diffuse = 0xffffffff;
		//}
		//pVB->unlock();

		break;
	}
case 2:
	{
		m_RenderNode.m_Op.vertexData->vertexDeclaration =  CVertexDeclaration::GET_VERDECL(VerNorTex1::Format);
		if(NULL==(pVB = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VerNorTex1),Size,HBU_STATIC_WRITE_ONLY)))
			return 0;
		VerNorTex1* OutBuf = (VerNorTex1*)pVB->lock(HBL_NO_OVERWRITE);
		for( int i = 0; i < Size; i++,OutBuf++ )
		{
			LoadVertex1( VertexBuf, SI, GraphicFile );
			OutBuf->p.x = VertexBuf.x*0.25f;
			OutBuf->p.y = VertexBuf.y*0.25f;
			OutBuf->p.z = VertexBuf.z*0.25f;
			OutBuf->np  = (CVector3f&)_CovData::_s.m_fNorInd[ VertexBuf.n ];
			OutBuf->tu = VertexBuf.u*0.0005f;
			OutBuf->tv = VertexBuf.v*0.0005f;
			OutBuf->tu1 = VertexBuf.u1*0.0005f;
			OutBuf->tv1 = VertexBuf.v1*0.0005f;
		}
		pVB->unlock();
		break;
	}
default:
	{
		m_RenderNode.m_Op.vertexData->vertexDeclaration =  CVertexDeclaration::GET_VERDECL(VerNorTex2::Format);
		if(NULL==(pVB = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VerNorTex2),Size,HBU_STATIC_WRITE_ONLY)))
			return 0;
		VerNorTex2* OutBuf = (VerNorTex2*)pVB->lock(HBL_NO_OVERWRITE);
		for( int i = 0; i < Size; i++,OutBuf++ )
		{
			LoadVertex2( VertexBuf, SI, GraphicFile );
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
		}
		pVB->unlock();
		break;
	}

}
int CVertexAniLevel::BinLoad(  CBufFile& GraphicFile, SaveInfo& SI ,int m_Channel )
{
	int AniSize;
	int VertexSize;
	GraphicFile.read( (char*)&m_Dist, sizeof( m_Dist ) );
	GraphicFile.read( (char*)&AniSize, sizeof(AniSize) );
	GraphicFile.read( (char*)&VertexSize, sizeof(VertexSize) );
	m_MeshFrame.resize(AniSize);

	m_TextBuffer  = NULL;
	m_TextBuffer1 = NULL;
	m_TextBuffer2 = NULL;

	m_RenderNode.m_Op.vertexData->vertexCount = VertexSize;
	m_RenderNode.m_Op.vertexData->vertexStart = 0;
	VertexAniFormat::Tex1* OutTex = NULL;
	VertexAniFormat::Tex1* OutTex1 =NULL;
	VertexAniFormat::Tex1* OutTex2 = NULL;


	for( int i = 0; i < (int)m_MeshFrame.size(); i++ )
	{
		int FrameSize;
		GraphicFile.read( (char*)&m_MeshFrame[i].m_AnimationeName, sizeof(m_MeshFrame[i].m_AnimationeName) );
		GraphicFile.read( (char*)&m_MeshFrame[i].m_FramePerKeyFrame, sizeof( m_MeshFrame[i].m_FramePerKeyFrame ) );
		GraphicFile.read( (char*)&FrameSize, sizeof(FrameSize) );

		m_MeshFrame[i].m_FrameVertex.resize( FrameSize );
		m_MeshFrame[i].m_FrameBuffer.resize( FrameSize );

		for( int j = 0; j < (int)m_MeshFrame[i].m_FrameVertex.size(); j++ )
		{
			m_MeshFrame[i].m_FrameVertex[j].resize(VertexSize);
			m_MeshFrame[i].m_FrameBuffer[j] = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VertexAniFormat::Vertex),VertexSize,HBU_STATIC_WRITE_ONLY);
			VertexAniFormat::Vertex* OutBuf = (VertexAniFormat::Vertex*)m_MeshFrame[i].m_FrameBuffer[j]->lock(HBL_NO_OVERWRITE);
			if( i || j )
			{
				for( int k = 0; k < VertexSize; k++ )
				{
					VertexFomat& Ver = m_MeshFrame[i].m_FrameVertex[j][k];
					LoadVertex( Ver.x, Ver.y, Ver.z, Ver.n, SI, GraphicFile );
					OutBuf->p.x = Ver.x*0.25f;
					OutBuf->p.y = Ver.y*0.25f;
					OutBuf->p.z = Ver.z*0.25f;
					OutBuf->n  = (CVector3f&)_CovData::_s.m_fNorInd[ Ver.n ];
					++OutBuf;
				}
			}
			else
			{
				VertexFomatEx Ver;
				switch(m_Channel)
				{
				case 0:
				case 1:
					m_TextBuffer = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VertexAniFormat::Tex1),VertexSize,HBU_STATIC_WRITE_ONLY);
					OutTex = (VertexAniFormat::Tex1*)m_TextBuffer->lock(HBL_NO_OVERWRITE);
					for( int k = 0; k < VertexSize; k++ )
					{
						LoadVertex( Ver, SI, GraphicFile );
						m_MeshFrame[i].m_FrameVertex[j][k] = Ver;
						OutBuf->p.x = Ver.x*0.25f;
						OutBuf->p.y = Ver.y*0.25f;
						OutBuf->p.z = Ver.z*0.25f;
						OutBuf->n  = (CVector3f&)_CovData::_s.m_fNorInd[ Ver.n ];

						OutTex->u = Ver.u*0.0005f;
						OutTex->v = Ver.v*0.0005f;
						++OutTex;
						++OutBuf;
					}
					m_TextBuffer = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VertexAniFormat::Tex1),VertexSize,HBU_STATIC_WRITE_ONLY);
					m_RenderNode.m_Op.vertexData->vertexDeclaration =  CVertexDeclaration::GET_VERDECL(VertexAniFormat::Tex1::Format);
					m_TextBuffer->unlock();
					m_RenderNode.m_Op.vertexData->vertexBufferBinding->setBinding(2,m_TextBuffer);
					break;
				case 2:
					m_TextBuffer = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VertexAniFormat::Tex1),VertexSize,HBU_STATIC_WRITE_ONLY);
					m_TextBuffer1 = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VertexAniFormat::Tex1),VertexSize,HBU_STATIC_WRITE_ONLY);
					OutTex = (VertexAniFormat::Tex1*)m_TextBuffer->lock(HBL_NO_OVERWRITE);
					OutTex1 = (VertexAniFormat::Tex1*)m_TextBuffer1->lock(HBL_NO_OVERWRITE);
					for( int k = 0; k < VertexSize; k++ )
					{
						LoadVertex1( Ver, SI, GraphicFile );
						m_MeshFrame[i].m_FrameVertex[j][k] = Ver;

						OutBuf->p.x = Ver.x*0.25f;
						OutBuf->p.y = Ver.y*0.25f;
						OutBuf->p.z = Ver.z*0.25f;
						OutBuf->n  = (CVector3f&)_CovData::_s.m_fNorInd[ Ver.n ];

						OutTex->u = Ver.u*0.0005f;
						OutTex->v = Ver.v*0.0005f;
						OutTex1->u = Ver.u1*0.0005f;
						OutTex1->v = Ver.v1*0.0005f;

						++OutTex;
						++OutTex1;
						++OutBuf;
					}
					m_RenderNode.m_Op.vertexData->vertexDeclaration =  CVertexDeclaration::GET_VERDECL(VertexAniFormat::Tex2::Format);
					m_TextBuffer->unlock();
					m_TextBuffer1->unlock();
					m_RenderNode.m_Op.vertexData->vertexBufferBinding->setBinding(2,m_TextBuffer);
					m_RenderNode.m_Op.vertexData->vertexBufferBinding->setBinding(3,m_TextBuffer);
					break;
				default:
					m_TextBuffer = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VertexAniFormat::Tex1),VertexSize,HBU_STATIC_WRITE_ONLY);
					m_TextBuffer1 = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VertexAniFormat::Tex1),VertexSize,HBU_STATIC_WRITE_ONLY);
					m_TextBuffer2 = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VertexAniFormat::Tex1),VertexSize,HBU_STATIC_WRITE_ONLY);
					OutTex = (VertexAniFormat::Tex1*)m_TextBuffer->lock(HBL_NO_OVERWRITE);
					OutTex1 =(VertexAniFormat::Tex1*)m_TextBuffer1->lock(HBL_NO_OVERWRITE);
					OutTex2 = (VertexAniFormat::Tex1*)m_TextBuffer2->lock(HBL_NO_OVERWRITE);
					for( int k = 0; k < VertexSize; k++ )
					{
						LoadVertex2( Ver, SI, GraphicFile );
						m_MeshFrame[i].m_FrameVertex[j][k] = Ver;
						OutBuf->p.x = Ver.x*0.25f;
						OutBuf->p.y = Ver.y*0.25f;
						OutBuf->p.z = Ver.z*0.25f;
						OutBuf->n  = (CVector3f&)_CovData::_s.m_fNorInd[ Ver.n ];

						OutTex->u = Ver.u*0.0005f;
						OutTex->v = Ver.v*0.0005f;
						OutTex1->u = Ver.u1*0.0005f;
						OutTex1->v = Ver.v1*0.0005f;
						OutTex2->u = Ver.u2*0.0005f;
						OutTex2->v = Ver.v2*0.0005f;

						++OutTex;
						++OutTex1;
						++OutTex2;
						++OutBuf;
					}
					m_RenderNode.m_Op.vertexData->vertexDeclaration =  CVertexDeclaration::GET_VERDECL(VertexAniFormat::Tex3::Format);
					m_TextBuffer->unlock();
					m_TextBuffer1->unlock();
					m_TextBuffer2->unlock();
					m_RenderNode.m_Op.vertexData->vertexBufferBinding->setBinding(2,m_TextBuffer);
					m_RenderNode.m_Op.vertexData->vertexBufferBinding->setBinding(3,m_TextBuffer1);
					m_RenderNode.m_Op.vertexData->vertexBufferBinding->setBinding(4,m_TextBuffer2);
					break;
				}
			}
			m_MeshFrame[i].m_FrameBuffer[j]->unlock();

		}
	}

	GraphicFile.read( (char*)&VertexSize, sizeof(VertexSize) );

	m_RenderNode.m_Op.indexData->indexCount = VertexSize;
	m_RenderNode.m_Op.indexData->indexStart = 0;
	m_RenderNode.m_Op.useIndexes = true;

	m_IndexBuf.resize( VertexSize );
	for( int j = 0; j < (int)m_IndexBuf.size(); j++ )
		GraphicFile.read( (char*)&m_IndexBuf[j], GetVerNum() < 256 ? sizeof(BYTE) : sizeof(WORD) );

	if(VertexSize==0)
		return 0;

	if(NULL==(m_RenderNode.m_Op.indexData->indexBuffer =
		CHardwareBufferMgr::GetInst()->createIndexBuffer(VertexSize,HBU_STATIC_WRITE_ONLY)))
		return 0;

	m_RenderNode.m_Op.indexData->indexBuffer->write(
		0,
		m_RenderNode.m_Op.indexData->indexBuffer->getSize(),
		&m_IndexBuf[0],
		true
		);
	//CStaticLevel::ReadIndex( GraphicFile );
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//柔体结构
//////////////////////////////////////////////////////////////////////////

//总是算结点2对结点1所施加的力
inline void CalForce( CBallNode& Ball1, CBallNode& Ball2, const CVerNode& Ball2Ver, const SpringInfo& SpInfo )
{
	CVector3f vDir = Ball2.m_vPos - Ball1.m_vPos;//力的方向
	float fRealLen = vDir.Mag();
	if( fRealLen > 0.00001f )
	{
		vDir.Normalize();

		CVector3f vForce = SpInfo.m_fCoe * (fRealLen - Ball2Ver.m_fNatualLen) * vDir;  //F = k*dL
		Ball1.m_vForce += vForce;
		Ball2.m_vForce -= vForce;
	}
	//else
	//	Log( WL_VERBOSE, "CalForce.Normalize() Err" );


	//m_pBall1->AddForce(m_vForce);
	//m_pBall2->AddForce(-m_vForce);
}

//////////////////////////////////////////////////////////////////////////

CSoftLevel::CSoftLevel()
{
	//=================node初始化===================//
	m_nodePhyPara.m_fAccGravity       = 9.8f;             //重力加速度（牛顿/千克）
	m_nodePhyPara.m_fMass             = 0.09f;//0.005;    //每个质点的质量
	m_nodePhyPara.m_fCoeDamp          = 0.1f  ;           //阻力系数

	//==============spring变量初始化================//
	m_spInfo.m_fRatioPull     = 1.1f;             //最大拉长比率（与自然长度相比）
	m_spInfo.m_fRatioCompress = 0.9f;             //最小缩短比率（与自然长度相比）
	m_spInfo.m_fCoe			  = 0.98f;

	m_eSoftType = SoftPlane;
}

void CSoftLevel::SetSkeletalMask( SKELETAL_MASK& Mask )
{
	for( int i = 0; i < 2; i++ )
		for( int j = 0; j < 4; j++ )
			Mask.AddSkeletal( m_vecTop[i].b[j] );
	for( int i = 0; i < (int)m_arrCollideStatic.size(); i++ )
	{
		Mask.AddSkeletal( m_arrCollideStatic[i].m_nBnID );
		if( m_arrCollideStatic[i].m_nBnID < 255 )
			Mask.AddSkeletal( m_arrCollideStatic[i].m_nBnID + 1);
	}
}

////////////////////////////////////////////////////////////////////////
// 功能：根据格式返回相应的当前计算过的顶点
// 参数：void* DestBuf				要写入的缓冲区,
//		 uint32 Format				要返回的格式,
//       uint32 Level                 LOD层,
//		 uint32 stride				格式的跨度,
//       const D3DXMATRIX* matMesh	矩阵列表,
//		 float Frame				要计算的帧
// 返回：已写入的缓冲区
//////////////////////////////////////////////////////////////////////////
const void* CSoftLevel::CalMeshVertex( CGraphic* pGraphic, void* DestBuf, uint32 Format, uint32 stride,
	CRenderMatrix& ModelMatrix, CRenderPiece* pRenderPiece, int32 iPiece ) const
{
	CDynamicSoft& DS = pRenderPiece->m_Pieces[iPiece].m_SoftData;

	CVector3f P1,P2;
	UpdateFrame( ModelMatrix, P1, P2, DS );

	int i;
	int CurPos = 0;
	int WghPos,IndPos,NorPos,DifPos,TexPos;

	//根据格式计算偏移量
	if( D3DFVF_XYZ&Format || D3DFVF_XYZB4&Format )
		CurPos += sizeof(D3DXVECTOR3);
	WghPos = CurPos;
	if( D3DFVF_LASTBETA_UBYTE4&Format )
		CurPos += sizeof(float)*3;
	IndPos = CurPos;
	if( D3DFVF_LASTBETA_UBYTE4&Format )
		CurPos += sizeof(uint32);
	NorPos = CurPos;
	if( D3DFVF_NORMAL&Format )
		CurPos += sizeof(D3DXVECTOR3);
	DifPos = CurPos;
	if( D3DFVF_DIFFUSE&Format )
		CurPos += sizeof(uint32);
	TexPos = CurPos;

	CVector3f vDir = P2 - P1;

	if( D3DFVF_XYZ&Format || D3DFVF_XYZB4&Format )
	{
		char* OutBuf = (char*)DestBuf;
		*(CVector3f*)( OutBuf )			    = P1;
		*(CVector3f*)( OutBuf + stride )	= P2;
		for( i = 1; i < (int)DS.m_arrSpNode.size(); i++ )
		{
			CVector3f vDiri;
			CVector3f v = DS.m_arrSpNode[i-1].m_vPos - DS.m_arrSpNode[i].m_vPos;
			float fLen = v.Mag();
			if( fLen > 0.00001f )
			{
				v /= fLen;
				vDiri = vDir - v*v.Dot( vDir );
			}
			else
			{
				vDiri = vDir;
				//Log( WL_VERBOSE, "D3DFVF_XYZ&Format || D3DFVF_XYZB4&Format vDir Err" );
			}

			fLen = vDiri.Mag();
			if( fLen > 0.00001f )
				vDiri /= fLen;
			//else
			//	Log( WL_VERBOSE, "D3DFVF_XYZ&Format || D3DFVF_XYZB4&Format vDiri Err" );

			vDiri *= m_arrVertex[i].m_fWidth/2;

			*(CVector3f*)( OutBuf + stride*i*2 )			= DS.m_arrSpNode[i].m_vPos - vDiri;
			*(CVector3f*)( OutBuf + stride*i*2 + stride )	= DS.m_arrSpNode[i].m_vPos + vDiri;
		}
	}
	if( D3DFVF_NORMAL&Format )
	{
		char* OutBuf = (char*)DestBuf + NorPos;
		for( i = 0; i<(int)DS.m_arrSpNode.size(); i++ )
		{
			*(CVector3f*)( OutBuf + stride*i*2 )			= DS.m_arrSpNode[i].m_vNormal;
			*(CVector3f*)( OutBuf + stride*i*2 + stride )	= DS.m_arrSpNode[i].m_vNormal;
		}
	}

	//顶点贴图坐标
	if( D3DFVF_TEX1&Format )
	{
		char* OutBuf = (char*)DestBuf + TexPos;
		for( i = 0; i<(int)DS.m_arrSpNode.size(); i++ )
		{
			*(CVector2f*)( OutBuf + stride*i*2 )			= *(CVector2f*)( &m_arrVertex[i].m_fTU1 );
			*(CVector2f*)( OutBuf + stride*i*2 + stride )	= *(CVector2f*)( &m_arrVertex[i].m_fTU2 );
		}
	}

	return DestBuf;
}

void CSoftLevel::InitDynData( CDynamicSoft& Data )
{
	Data.m_nPreTime = 0;
	Data.m_arrSpNode.resize( GetVerNum()/2 );
	Data.m_arrCollideDynamic.resize( GetCollisionCount() );
}

void CSoftLevel::ResetData( CDynamicSoft& DS, uint32 nDelTime ) const
{
	CSoftLevel::UpdateWind( DS, nDelTime );

	for( int i = 0; i < (int)DS.m_arrSpNode.size(); i++ )
	{
		DS.m_arrSpNode[i].m_vForce.Init(CVector3f(0, -m_nodePhyPara.m_fMass*m_nodePhyPara.m_fAccGravity, 0));
		DS.m_arrSpNode[i].m_vAccel.Init();
		DS.m_arrSpNode[i].m_vForce += DS.m_vecWind*0.02f;
	}
}

void CSoftLevel::CalCollideData( CRenderMatrix& ModelMatrix, CDynamicSoft& DS ) const
{
	if( !ModelMatrix.m_pAnimateGroup )
		return;

	CMatrix mat;

	for(int i = 0; i < (int)(m_arrCollideStatic.size()); i++)
	{
		if( m_arrCollideStatic[i].m_nBnID > ModelMatrix.m_MaxUseSkeletalID ||
			ModelMatrix.m_pMatrixUnit[ m_arrCollideStatic[i].m_nBnID ] == NULL )
		{
			char Buf[256];
			sprintf( Buf, "柔体碰撞骨骼不正确: 需要%d, 最大%d", m_arrCollideStatic[i].m_nBnID, ModelMatrix.m_MaxUseSkeletalID );
			//Log( WL_VERBOSE, Buf );
		}

		D3DXMATRIX mat =ModelMatrix.m_pMatrixUnit[ m_arrCollideStatic[i].m_nBnID ]->GetD3DXMATRIX();//m_CurCMatrixUnit;
		DS.m_arrCollideDynamic[i].m_vBegin = CVector3f(mat._41, mat._42, mat._43);

		CSkeletal* pSkeletal = ModelMatrix.m_pMatrixUnit[m_arrCollideStatic[i].m_nBnID]->m_SkeletalUnit->m_pSkeletal;

		//如果有孩子
		CMatrixUnit* pUnit = NULL;
		if( pSkeletal->GetChildNum() > 0 )
			pUnit = ModelMatrix.m_pMatrixUnit[pSkeletal->GetChild(0)->GetSkeletalID()];
		if( pUnit )
		{
			D3DXMATRIX& matCld = pUnit->GetD3DXMATRIX();//m_CurCMatrixUnit;
			DS.m_arrCollideDynamic[i].m_vEnd = CVector3f(matCld._41, matCld._42, matCld._43);
		}
		//如果没孩子，则任意给个长度
		else
		{
			DS.m_arrCollideDynamic[i].m_vEnd = CVector3f(5.0f, 0.0f, 0.0f).FastMultiply( mat );
		}
	}
}

void CSoftLevel::UpdateFrame( CRenderMatrix& ModelMatrix, CVector3f& P1, CVector3f& P2, CDynamicSoft& DS ) const
{
	//bool bFirst = !DS.m_nPreTime;
	//float dt = ( GetProcessTime() - DS.m_nPreTime )/20.0f;
	//if( dt > 1.0f )
	//	dt = 1.0f;

	////重力，风力
	//ResetData( DS, uint32(GetProcessTime()) - DS.m_nPreTime );
	//DS.m_nPreTime = (uint32)GetProcessTime();

	//CVector3f Temp;
	//CVector3f P[2] = { CVector3f( 0, 0, 0 ), CVector3f( 0, 0, 0 ) };
	//for( int i = 0; i < 2; i++ )
	//{
	//	for( int j = 0; j < 4; j++ )
	//	{
	//		if( !m_vecTop[i].b[j] )
	//		{
	//			if( j == 0 )
	//				P[i] = VEC_FASTMULT( m_vecTop[i].p, ModelMatrix.m_pMatrixUnit[0]->m_CurSkeletalGlobMatrix );
	//			break;
	//		}
	//		int n = min( ModelMatrix.m_MaxUseSkeletalID, m_vecTop[i].b[j] );
	//		Temp = VEC_FASTMULT( m_vecTop[i].p, ModelMatrix.m_pMatrixUnit[n]->m_CurSkeletalGlobMatrix );
	//		P[i] += Temp*m_vecTop[i].f[j];
	//	}
	//}

	//CTinyMatrix& matAttach = ModelMatrix.m_pMatrixUnit[ min( ModelMatrix.m_MaxUseSkeletalID, m_bySkeletalID ) ]->m_CurSkeletalGlobMatrix;
	//P1 = P[0];
	//P2 = P[1];

	//CVector3f p = ( P1 + P2 )*0.5f;
	//DS.m_arrSpNode[0].m_vPos = CVector3f( p.x, p.y, p.z );
	//DS.m_arrSpNode[0].m_vNormal = CVector3f( -matAttach.m[2][0], -matAttach.m[2][1], -matAttach.m[2][2] );

	////更新碰撞检测数据
	//CalCollideData( ModelMatrix, DS );

	////计算
	//for( int i = 1; i < (int)DS.m_arrSpNode.size(); i++ )
	//{
	//	//计算弹簧弹力
	//	if( bFirst )
	//	{
	//		DS.m_arrSpNode[i].m_vAccel     = CVector3f( 0, 0, 0 );
	//		DS.m_arrSpNode[i].m_vSpeed     = CVector3f( 0, 0, 0 );
	//		DS.m_arrSpNode[i].m_vPos       = DS.m_arrSpNode[i-1].m_vPos;
	//		DS.m_arrSpNode[i].m_vPos.y    -= (float)m_arrVertex[i].m_fNatualLen;
	//	}
	//	else
	//	{
	//		CalForce( DS.m_arrSpNode[i-1], DS.m_arrSpNode[i], m_arrVertex[i], m_spInfo );
	//		DS.m_arrSpNode[i].m_vAccel     = DS.m_arrSpNode[i].m_vForce / m_nodePhyPara.m_fMass ;// a = f / m;
	//		DS.m_arrSpNode[i].m_vSpeed    += ( DS.m_arrSpNode[i].m_vAccel * dt ); // v(n) = (v(n-1) + a*dt) * damp
	//		DS.m_arrSpNode[i].m_vPos      += ( DS.m_arrSpNode[i].m_vSpeed * dt );
	//	}

	//	//限制长度
	//	CVector3f vPP = DS.m_arrSpNode[i].m_vPos - DS.m_arrSpNode[i-1].m_vPos;
	//	float fRealLen = vPP.Mag();
	//	if( *( (uint32*)&fRealLen ) == 0xffc00000 )
	//	{
	//		//Log( WL_VERBOSE, "长度异常" );
	//	}
	//	else if( fRealLen > m_arrVertex[i].m_fNatualLen * m_spInfo.m_fRatioPull )
	//	{
	//		vPP /= fRealLen;
	//		DS.m_arrSpNode[i].m_vPos = DS.m_arrSpNode[i-1].m_vPos + m_arrVertex[i].m_fNatualLen * m_spInfo.m_fRatioPull*vPP;
	//		DS.m_arrSpNode[i].m_vSpeed.Init( 0.0f, 0.0f, 0.0f );
	//	}
	//	else if(fRealLen < m_arrVertex[i].m_fNatualLen * m_spInfo.m_fRatioCompress)
	//	{
	//		if( fRealLen > 0.0001f )
	//			vPP /= fRealLen;
	//		else
	//		{
	//			vPP.y = -1;
	//			//Log( WL_VERBOSE, "fRealLen < m_arrVertex[i].m_fNatualLen * m_spInfo.m_fRatioCompress" );
	//		}

	//		DS.m_arrSpNode[i].m_vPos = DS.m_arrSpNode[i-1].m_vPos + m_arrVertex[i].m_fNatualLen * m_spInfo.m_fRatioCompress*vPP;
	//		DS.m_arrSpNode[i].m_vSpeed.Init( 0.0f, 0.0f, 0.0f );
	//	}

	//	//------------------碰撞检测-----------------//

	//	for( int j = 0; j < (int)m_arrCollideStatic.size(); j++ )
	//	{
	//		CVector3f vV0ToPos  = DS.m_arrSpNode[i].m_vPos - DS.m_arrCollideDynamic[j].m_vBegin; //骨骼起点到测试点的法线
	//		CVector3f vV0ToV1   = DS.m_arrCollideDynamic[j].m_vEnd - DS.m_arrCollideDynamic[j].m_vBegin;	 //骨骼起点到骨骼终点的法线
	//		float fLenV0ToV1    = vV0ToV1.Mag();										 //骨骼长度
	//		vV0ToV1		       /= fLenV0ToV1;											 //单位长度骨骼
	//		float cos2Vec       = vV0ToPos.Dot(vV0ToV1);								 //骨骼起点到测试点的法线在骨骼上的投影

	//		//如果投影比骨骼长或者投影和骨骼法向相反，则不会和骨骼碰撞
	//		if( cos2Vec < 0 || cos2Vec > fLenV0ToV1 )
	//			continue;

	//		vV0ToV1			   *= cos2Vec;
	//		CVector3f vLineToPo = vV0ToPos - vV0ToV1;
	//		float fLenvLineToPo = vLineToPo.Mag();

	//		//如果点到骨骼的长度大于碰撞半径，也不会和骨骼碰撞
	//		if( fLenvLineToPo >= m_arrCollideStatic[j].m_fRadius )
	//			continue;

	//		if( fLenvLineToPo < 0.0001f )
	//		{
	//			CVector3f vTemp( 0, 0, 0 );
	//			if( abs( vV0ToV1.x ) < abs( vV0ToV1.y ) && abs( vV0ToV1.x ) < abs( vV0ToV1.z ) )
	//				vTemp.x = 1;
	//			else if( abs( vV0ToV1.z ) < abs( vV0ToV1.y ) )
	//				vTemp.z = 1;
	//			else
	//				vTemp.y = 1;

	//			CVector3f v = vV0ToV1.Cross( vTemp );
	//			v.Normalize();
	//			DS.m_arrSpNode[i].m_vPos += m_arrCollideStatic[j].m_fRadius*v;
	//		}
	//		else
	//			DS.m_arrSpNode[i].m_vPos += vLineToPo*( ( m_arrCollideStatic[j].m_fRadius - fLenvLineToPo )/fLenvLineToPo );
	//	}
	//}

	//UpdateNormal();
}

void CSoftLevel::UpdateNormal( CDynamicSoft& DS ) const
{
	for(int i = 0; i < (int)DS.m_arrSpNode.size(); i++)
	{
		CVector3f vNor = DS.m_arrSpNode[i].m_vNormal;
		//TRACE("\nvNor x = %f, y = %f, z = %f", vNor.x, vNor.y, vNor.z);
		CVector3f vDamp = -DS.m_arrSpNode[i].m_vSpeed*0.08f;
		CVector3f vCross = vNor.Cross(vDamp);
		float cosTheat = vNor.Dot(vDamp);
		if(fabs(cosTheat) > 0.000001f)
		{
			cosTheat /= (vNor.Mag()*vDamp.Mag());
			CMatrix mat(vCross, acos(cosTheat)*0.05f);
			DS.m_arrSpNode[i].m_vNormal = VEC_ROTATE( vNor, mat );
		}
	}
}

CVector3f CSoftLevel::s_vecMin( -1, -1, -1 );
CVector3f CSoftLevel::s_vecMax( 5, 1, 10 );

void CSoftLevel::SetWind( CVector3f& vecMax, CVector3f& vecMin )
{
	s_vecMin = vecMin;
	s_vecMax = vecMax;
}

void CSoftLevel::UpdateWind(  CDynamicSoft& DS, uint32 nDelTime )
{
	uint32 nCurTime = DS.m_nPreTime + nDelTime;
	if( nCurTime >= DS.m_nNextTime )
	{
		DS.m_NexWind.x = CQuickRand::Rand2( s_vecMax.x, s_vecMin.x );
		DS.m_NexWind.y = CQuickRand::Rand2( s_vecMax.y, s_vecMin.y );
		DS.m_NexWind.z = CQuickRand::Rand2( s_vecMax.z, s_vecMin.z );
		DS.m_nNextTime = CQuickRand::Rand2( 500, 1000 ) + nCurTime;
	}

	float w = ( (int32)( DS.m_nNextTime - nCurTime ) )/( (float)( (int32)( DS.m_nNextTime - DS.m_nPreTime ) ) + 0.1f );
	DS.m_vecWind = DS.m_vecWind*w + DS.m_NexWind*( 1.0f - w );
}

//////////////////////////////////////////////////////////////////////////

int CSoftLevel::BinLoad( CBufFile& GraphicFile, SaveInfo& SI ,int m_Channel )
{
	//读出柔体类型
	GraphicFile.read( (char*)&m_eSoftType,   sizeof(m_eSoftType) );

	//读出每个结点的物理参数参数信息
	GraphicFile.read( (char*)&m_nodePhyPara, sizeof(NodePhyPara) );
	//读出弹簧信息
	GraphicFile.read( (char*)&m_spInfo, sizeof(SpringInfo) );

	GraphicFile.read( (char*)&m_vecTop, sizeof( m_vecTop ) );

	//读出节点
	int nSpNode;
	GraphicFile.read( (char*)&nSpNode,  sizeof(int) );
	m_arrVertex.clear();
	m_arrVertex.resize( nSpNode );

	float y = 0;
	for( int i = 0; i < nSpNode; i++ )
	{
		GraphicFile.read( (char*)&m_arrVertex[i].m_fTU1, sizeof( float ) );
		GraphicFile.read( (char*)&m_arrVertex[i].m_fTV1, sizeof( float ) );
		GraphicFile.read( (char*)&m_arrVertex[i].m_fTU2, sizeof( float ) );
		GraphicFile.read( (char*)&m_arrVertex[i].m_fTV2, sizeof( float ) );
		GraphicFile.read( (char*)&m_arrVertex[i].m_fNatualLen, sizeof( float ) );
		GraphicFile.read( (char*)&m_arrVertex[i].m_fWidth, sizeof( float ) );

		y -= m_arrVertex[i].m_fNatualLen;
	}

	//读出要碰撞检测的骨骼ID
	uint32 nBnCount;
	GraphicFile.read( (char*)&nBnCount, sizeof(uint32) );
	m_arrCollideStatic.clear();
	m_arrCollideStatic.resize( nBnCount );

	if( !m_arrCollideStatic.empty() )
		GraphicFile.read( (char*)&m_arrCollideStatic[0], sizeof( CollideStatic )*nBnCount );

	float fW = -1.0f;
	for( int i = 0; i < 2; i++ )
	{
		for( int j = 0; j < 4; j++ )
		{
			if( m_vecTop[i].f[j] > fW )
			{
				fW = m_vecTop[i].f[j];
				m_bySkeletalID = m_vecTop[i].b[j];
			}
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
// 柔布(披风)结构
//////////////////////////////////////////////////////////////////////////

CSoftClothLevel::CSoftClothLevel()
{
	m_VertexNum = 0;
	m_ClothWidth  = 0;
	m_ClothHeight = 0;
}

CSoftClothLevel::~CSoftClothLevel()
{
}

////////////////////////////////////////////////////////////////////////
// 功能：根据格式返回相应的当前计算过的顶点
// 参数：void* DestBuf				要写入的缓冲区,
//		 uint32 Format				要返回的格式,
//       uint32 Level                 LOD层,
//		 uint32 stride				格式的跨度,
//       const D3DXMATRIX* matMesh	矩阵列表,
//		 float Frame				要计算的帧
// 返回：已写入的缓冲区
//////////////////////////////////////////////////////////////////////////
const void* CSoftClothLevel::CalMeshVertex( CGraphic* pGraphic, void* DestBuf, uint32 Format, uint32 stride,
	CRenderMatrix& ModelMatrix, CRenderPiece* pRenderPiece, int32 iPiece ) const
{
	Update( ModelMatrix, pRenderPiece, iPiece );

	if( VerNorTex::Format == Format )
	{
		CDynamicSoftCloth& DS = pRenderPiece->m_Pieces[iPiece].m_SoftClothData;
		UpdateClothNormal( DS, DestBuf );

		for( int i = 0; i < DS.m_VertexNum; ++i)
		{
			*(CVector3f*)((VerNorTex*)DestBuf + i) = DS.m_VertexBuf[i];
			(*((VerNorTex*)DestBuf + i)).tu = this->m_TextureU[i] / 2046.0f;
			(*((VerNorTex*)DestBuf + i)).tv = this->m_TextureV[i] / 2046.0f;
		}
	}
	if( VerTex::Format == Format )
	{
		CDynamicSoftCloth& DS = pRenderPiece->m_Pieces[iPiece].m_SoftClothData;
		for( int i = 0; i < DS.m_VertexNum; ++i)
		{
			*(CVector3f*)((VerTex*)DestBuf + i) = DS.m_VertexBuf[i];
			(*(VerTex*)((VerTex*)DestBuf + i)).tu = this->m_TextureU[i] / 2046.0f;
			(*(VerTex*)((VerTex*)DestBuf + i)).tv = this->m_TextureV[i] / 2046.0f;
		}
	}

	return DestBuf;
}
void CSoftClothLevel::UpdateClothNormal( CDynamicSoftCloth& DS , void* DestBuf) const
{
	CVector3f* NorCache = (CVector3f*)alloca( DS.m_VertexNum * sizeof( CVector3f ));
	memset( NorCache ,0, DS.m_VertexNum * sizeof( CVector3f ));

	// CVector3f NormalAvg;
	// NormalAvg.Init( 0, 0, 0);

	for( size_t i = 0; i < m_IndexBuf.size(); i+=3)
	{
		CVector3f t(( DS.m_VertexBuf[ m_IndexBuf[i+1] ] - DS.m_VertexBuf[ m_IndexBuf[i] ] ) ^ ( DS.m_VertexBuf[ m_IndexBuf[i+2] ] - DS.m_VertexBuf[ m_IndexBuf[i] ] ));
		t.Normalize();
		// NormalAvg += t;
		NorCache[ m_IndexBuf[i] ]   += t;
		NorCache[ m_IndexBuf[i+1] ] += t;
		NorCache[ m_IndexBuf[i+2] ] += t;
	}
	for( int i=0; i< DS.m_VertexNum; ++i)
	{
		((VerNorTex*)DestBuf)[i].np = NorCache[i];
	}
	// NormalAvg.Normalize();
	// DS.m_NormalAVG = NormalAvg;
}

int CSoftClothLevel::BinLoad( CBufFile& GraphicFile, SaveInfo& SI ,int m_Channel )
{
	GraphicFile.read( (void*)&m_ClothProp, sizeof(SoftClothProp) );

	m_ClothWidth    = m_ClothProp.uWidth;
	m_ClothHeight   = m_ClothProp.uHeight;
	m_VertexNum		= m_ClothWidth * m_ClothHeight;

	for(int j=0; j<m_ClothWidth; ++j)
	{
		D3DXVECTOR3 p;
		CTinyWeight f;
		BYTE		b[4];
		GraphicFile.read( (void*)&p, sizeof(D3DXVECTOR3) );
		GraphicFile.read( (void*)&f, sizeof(CTinyWeight) );
		GraphicFile.read( (void*)b,  sizeof(BYTE)*4 );
		m_TopFixVerBuf.push_back( VertexFomat( CVector3f( p.x, p.y, p.z), f, b) );
	}

	// 输出横向弹簧
	for(int i=0; i< m_ClothHeight * (m_ClothWidth-1); ++i)
	{
		float l;
		GraphicFile.read( (void*)&l, sizeof(float) );
		this->m_HSprings.push_back(l);
	}

	// 输出纵向弹簧
	for(int i=0; i < m_ClothWidth * (m_ClothHeight-1); ++i)
	{
		float l;
		GraphicFile.read( (void*)&l, sizeof(float) );
		this->m_VSprings.push_back(l);
	}

	for(int i=0; i<m_ClothHeight*m_ClothWidth; ++i)
	{
		uint16 u, v;
		GraphicFile.read( (void*)&u, sizeof(uint16) );
		GraphicFile.read( (void*)&v, sizeof(uint16) );

		this->m_TextureU.push_back(u);
		this->m_TextureV.push_back(v);
	}

	//  0     1     2     3
	//  @-----@-----@-----@
	//  |    /|    /|    /|
	//  |   / |   / |   / |
	//  |  /  |  /  |  /  |
	//  | /   | /   | /   |
	//  |/    |/    |/    |
	//  @-----@-----@-----@
	//  4     5     6     7
	//
	// Index顺序为 0->1->4  1->5->4  1->2->5  2->6->5  2->3->6  3->7->6

	int ibase = 0;
	for( int i=0; i<m_ClothHeight-1; ++i)
	{
		for( int j=0; j<m_ClothWidth-1; ++j)
		{
			m_IndexBuf.push_back( static_cast<uint16>(ibase + j));
			m_IndexBuf.push_back( static_cast<uint16>(ibase + j + 1));
			m_IndexBuf.push_back( static_cast<uint16>(ibase + j + m_ClothWidth ));

			m_IndexBuf.push_back( static_cast<uint16>(ibase + j + 1));
			m_IndexBuf.push_back( static_cast<uint16>(ibase + j + m_ClothWidth + 1));
			m_IndexBuf.push_back( static_cast<uint16>(ibase + j + m_ClothWidth) );
		}
		ibase += m_ClothWidth;
	}

	uint8 CollideNum;
	GraphicFile.read( (void*)&CollideNum, sizeof(uint8));

	for( uint8 i = 0; i < CollideNum; ++i)
	{

		CapsuleNode CsNod;
		GraphicFile.read( (void*)&CsNod, sizeof(CapsuleNode) );
		m_Collides.push_back( CollideCapsule( CsNod.Begin, CsNod.End, CsNod.Raidus, CsNod.w, CsNod.b) );
	}
	return 0;
}

void  CSoftClothLevel::Update( CRenderMatrix & ModelMatrix, CRenderPiece* pRenderPiece, int32 iPiece ) const
{
	//int nMaxSkeletalID = ModelMatrix.m_pAnimateGroup ? ModelMatrix.m_pAnimateGroup->GetSkeletalNum() - 1 : 0;
	//nMaxSkeletalID = min( ModelMatrix.m_MaxUseSkeletalID, nMaxSkeletalID );

	//CVector3f vecTempVer;

	//CDynamicSoftCloth & DCloth = pRenderPiece->m_Pieces[iPiece].m_SoftClothData;

	//// 变换Fix行
	//for( size_t i = 0; i < DCloth.m_TopFixVerBuf.size(); ++i)
	//{
	//	CVector3f v( DCloth.m_TopFixVerBuf[i].VertexT.x, DCloth.m_TopFixVerBuf[i].VertexT.y, DCloth.m_TopFixVerBuf[i].VertexT.z );
	//	CVector3f vp(0,0,0);
	//	for( uint8 j = 0; j < 4; j++ )
	//	{
	//		uint8 b = m_TopFixVerBuf[i].b[j];
	//		if( !b )
	//		{
	//			if( j == 0 )
	//			{
	//				vp = VEC_FASTMULT( v, ModelMatrix.m_pMatrixUnit[0]->m_CurSkeletalGlobMatrix );
	//			}
	//			break;
	//		}
	//		if( b > nMaxSkeletalID )
	//			b = 0;
	//		vecTempVer = VEC_FASTMULT( v, ModelMatrix.m_pMatrixUnit[b]->m_CurSkeletalGlobMatrix );

	//		float w = m_TopFixVerBuf[i].w.GetWeight(j);
	//		vp.x += vecTempVer.x * w;
	//		vp.y += vecTempVer.y * w;
	//		vp.z += vecTempVer.z * w;
	//	}

	//	DCloth.m_VertexBuf[i] = *(CVector3f*)&vp;
	//}

	//// 变换碰撞胶囊 以及 计算X-Z包围墙

	//for( size_t i=0; i < this->m_Collides.size(); ++i)
	//{
	//	const CVector3f& vb = this->m_Collides[i].m_Begin;
	//	const CVector3f& ve = this->m_Collides[i].m_End;
	//	CVector3f& vpb = DCloth.m_Capsules[i].m_Begin;
	//	CVector3f& vpe = DCloth.m_Capsules[i].m_End;

	//	for( uint8 j = 0; j < 4; j++ )
	//	{
	//		uint8 b = this->m_Collides[i].BoneIndex[j];

	//		if( !b )
	//		{
	//			if( j == 0 )
	//			{
	//				vpb = VEC_FASTMULT( vb, ModelMatrix.m_pMatrixUnit[0]->m_CurSkeletalGlobMatrix );
	//				vpe = VEC_FASTMULT( ve, ModelMatrix.m_pMatrixUnit[0]->m_CurSkeletalGlobMatrix );
	//			}
	//			break;
	//		}
	//		if( b > nMaxSkeletalID )
	//			b = 0;

	//		float w = this->m_Collides[i].BoneWeight.GetWeight(j);

	//		vecTempVer = VEC_FASTMULT( vb, ModelMatrix.m_pMatrixUnit[b]->m_CurSkeletalGlobMatrix );

	//		vpb.x += vecTempVer.x * w;
	//		vpb.y += vecTempVer.y * w;
	//		vpb.z += vecTempVer.z * w;

	//		vecTempVer = VEC_FASTMULT( ve, ModelMatrix.m_pMatrixUnit[b]->m_CurSkeletalGlobMatrix );

	//		vpe.x += vecTempVer.x * w;
	//		vpe.y += vecTempVer.y * w;
	//		vpe.z += vecTempVer.z * w;
	//	}
	//}

	//CTinyMatrix& matRoot = ModelMatrix.m_pMatrixUnit[0]->m_CurCMatrixUnit;
	////CVector3f w( -matRoot._31, -matRoot._32, -matRoot._33 );
	////w += DCloth.m_NormalAVG * 0.2f;
	//// w.Normalize();
	//DCloth.m_Cloth->Calculate( CVector3f ( -matRoot._31, -matRoot._32, -matRoot._33 ) );

	//CVector3f * Nodes = DCloth.m_VertexBuf;

	//for( size_t j=0; j < this->m_Collides.size(); ++j)
	//{
	//	float CapsuleWallXMax = -FLT_MAX, CapsuleWallXMin = FLT_MAX;
	//	float CapsuleWallZMax = -FLT_MAX, CapsuleWallZMin = FLT_MAX;

	//	float _Xb = DCloth.m_Capsules[j].m_Begin.x;
	//	float _Xe = DCloth.m_Capsules[j].m_End.x;
	//	float _Zb = DCloth.m_Capsules[j].m_Begin.z;
	//	float _Ze = DCloth.m_Capsules[j].m_End.z;

	//	_Xb > _Xe ? ( CapsuleWallXMax = _Xb, CapsuleWallXMin = _Xe):( CapsuleWallXMax = _Xe, CapsuleWallXMin = _Xb);
	//	_Zb > _Ze ? ( CapsuleWallZMax = _Zb, CapsuleWallZMin = _Ze):( CapsuleWallZMax = _Ze, CapsuleWallZMin = _Zb);

	//	float r = this->m_Collides[j].m_Radius;
	//	CapsuleWallXMax += r;
	//	CapsuleWallXMin -= r;
	//	CapsuleWallZMax += r;
	//	CapsuleWallZMin -= r;

	//	for( int i=0; i < DCloth.m_VertexNum; ++i)
	//	{
	//		if( Nodes[i].x > CapsuleWallXMin && Nodes[i].z > CapsuleWallZMin && Nodes[i].z < CapsuleWallZMax && Nodes[i].x < CapsuleWallXMax)
	//		{
	//			CVector3f SubV = Nodes[i].ClosestPointOnLine( DCloth.m_Capsules[j].m_Begin, DCloth.m_Capsules[j].m_End );
	//			CVector3f SV( Nodes[i] - SubV );
	//			if( SV.Mag() < r)
	//			{
	//				Nodes[i] = SubV + SV.Normalize() * r;
	//			}
	//		}
	//	}
	//}
}

void CSoftClothLevel::SetSkeletalMask( SKELETAL_MASK& Mask )
{
	for( int i = 0; i < (int)m_TopFixVerBuf.size(); i++ )
		for( int j = 0; j < 4; j++ )
			Mask.AddSkeletal( m_TopFixVerBuf[i].b[j] );
}

void CSoftClothLevel::InitDynData( CDynamicSoftCloth& Data )
{
	Data.m_upLevel		= this;
	Data.m_Width        = this->m_ClothWidth;
	Data.m_Height       = this->m_ClothHeight;
	Data.m_VertexBuf    = new CVector3f[ this->m_VertexNum ];

	for( size_t i = 0 ; i < this->m_TopFixVerBuf.size(); ++i)
		Data.m_TopFixVerBuf.push_back( *((CDynamicSoftCloth::VertexFomat*)&this->m_TopFixVerBuf[i]) );

	Data.m_Cloth        = new Cloth::CCloth;
	Data.m_Cloth->SetWind( Data.m_upLevel->m_ClothProp.uWindMin, Data.m_upLevel->m_ClothProp.uWindMax, Data.m_upLevel->m_ClothProp.uWindMaxH, Data.m_upLevel->m_ClothProp.fTimePass );

	Data.m_VertexNum = static_cast<int16>(this->m_VertexNum);
	Data.m_Capsules.resize( this->m_Collides.size() );

}

CDynamicSoftCloth::CDynamicSoftCloth( const CDynamicSoftCloth& b )
{
	this->m_Width        = b.m_Width;
	this->m_Height       = b.m_Height;
	this->m_VertexNum    = b.m_VertexNum;
	this->m_upLevel		 = b.m_upLevel;

	if ( this->m_VertexNum > 0 )
	{
		this->m_TopFixVerBuf = b.m_TopFixVerBuf;
		this->m_Capsules     = b.m_Capsules;

		this->m_VertexBuf    = new CVector3f[ this->m_VertexNum ];
		this->m_Cloth        = new Cloth::CCloth();

		this->m_Cloth->m_WindPowerMin = b.m_Cloth->m_WindPowerMin;
		this->m_Cloth->m_WindPowerMax = b.m_Cloth->m_WindPowerMax;
		this->m_Cloth->m_WindPowerMaxH = b.m_Cloth->m_WindPowerMaxH;
		this->m_Cloth->m_TimePass     = b.m_Cloth->m_TimePass;

		for( int i=0; i< this->m_VertexNum; ++i)
		{
			float x = (float)abs ( (int)(i % (int)m_upLevel->m_ClothWidth - ((int)m_upLevel->m_ClothWidth -1) / 2 )) / (((int)m_upLevel->m_ClothWidth-1) / 2 ); x *= x;
			float y = (float)(i / (int)m_upLevel->m_ClothWidth) / ((int)m_upLevel->m_ClothHeight -1); y *= y;

			this->m_Cloth->AddNode( Cloth::CSoftVerNode( this->m_VertexBuf + i,
				m_upLevel->m_ClothProp.uMassA/100.0f + ( (int32)m_upLevel->m_ClothProp.uMassB - (int32)m_upLevel->m_ClothProp.uMassA)/100.0f * y + ((int32)m_upLevel->m_ClothProp.uMassC - (int32)m_upLevel->m_ClothProp.uMassB ) / 100.0f * x,
				m_upLevel->m_ClothProp.fGravity, m_upLevel->m_ClothProp.fGassForce) );
			if ( i < this->m_Width )
				this->m_Cloth->SetFixNode(static_cast<uint16>(i) );
		}

		for(int i = 0; i<this->m_VertexNum; ++i)
		{
			if(i<this->m_Width)
			{
				m_VertexBuf[i] = m_TopFixVerBuf[i].VertexT;
			}
			else
			{
				m_VertexBuf[i].x = m_VertexBuf[i-1].x;
				m_VertexBuf[i].z = m_VertexBuf[i-1].z;
				m_VertexBuf[i].y = m_VertexBuf[i-1].y - 50.0f;
			}
		}

		int basei = 0;
		for(int i=0 ; i< this->m_Width; ++i)
		{
			for(int j=0; j<this->m_Height-1; ++j)
			{
				this->m_Cloth->AddSpring( static_cast<uint16>(this->m_Width * j + i), static_cast<uint16>(this->m_Width * (j + 1) + i),
					this->m_upLevel->m_ClothProp.fCoefficientK,
					m_upLevel->m_VSprings[basei++] , this->m_upLevel->m_ClothProp.fLongRatio, this->m_upLevel->m_ClothProp.fShortRatio );
			}
		}

		uint8 hw = (this->m_Width-1)/2;
		for( int i=1; i< this->m_Height; ++i)
		{
			for(int j=0; j<hw; ++j)
			{
				this->m_Cloth->AddSpring( static_cast<uint16>(this->m_Width * i + (hw - j)), static_cast<uint16>(this->m_Width * i + (hw - j - 1)),
					this->m_upLevel->m_ClothProp.fCoefficientK,
					m_upLevel->m_HSprings[ (this->m_Width-1) * i + hw - 1 - j], this->m_upLevel->m_ClothProp.fLongRatio, this->m_upLevel->m_ClothProp.fShortRatio );
				this->m_Cloth->AddSpring( static_cast<uint16>(this->m_Width * i + (hw + j)), static_cast<uint16>(this->m_Width * i + (hw + j + 1)),
					this->m_upLevel->m_ClothProp.fCoefficientK,
					m_upLevel->m_HSprings[ (this->m_Width-1) * i + hw + j], this->m_upLevel->m_ClothProp.fLongRatio, this->m_upLevel->m_ClothProp.fShortRatio );
			}
		}

		for( int j=0; j< this->m_Height-1; ++j)
		{
			for(int i=0 ; i< this->m_Width -1; ++i)
			{
				float l1 = m_upLevel->m_HSprings[ (this->m_Width-1) * j + i];
				float l2 = m_upLevel->m_VSprings[ (this->m_Height-1) * i + j];
				float l3 = sqrt(l1 * l1 + l2 * l2);
				this->m_Cloth->AddSpring( static_cast<uint16>(this->m_Width * j + i),  static_cast<uint16>(this->m_Width * (j+1) + i + 1),
					this->m_upLevel->m_ClothProp.fCoefficientK * 0.6f,
					l3, this->m_upLevel->m_ClothProp.fLongRatio , this->m_upLevel->m_ClothProp.fShortRatio * 0.35f );
				l1 = m_upLevel->m_HSprings[ (this->m_Width-1) * (j+1) + i];
				l2 = m_upLevel->m_VSprings[ (this->m_Height-1) * (i+1) + j];
				l3 = sqrt(l1 * l1 + l2 * l2);
				this->m_Cloth->AddSpring( static_cast<uint16>(this->m_Width * j + i + 1), static_cast<uint16>(this->m_Width * (j+1) + i),
					this->m_upLevel->m_ClothProp.fCoefficientK * 0.6f,
					l3, this->m_upLevel->m_ClothProp.fLongRatio , this->m_upLevel->m_ClothProp.fShortRatio * 0.35f );
			}
		}
	}
	else
	{
		this->m_TopFixVerBuf.clear();
		this->m_Capsules.clear();
		this->m_VertexBuf = NULL;
		this->m_Cloth     = NULL;
	}
}
*/
}





