#include "stdafx.h"
#include "CPieceGroup.h"
#include "CTexture.h"
#include "CGraphic.h"
#include "CBufFile.h"
#include "CRenderPiece.h"
#include "BaseHelper.h"
#include "CVertexDeclaration.h"
#include "CCpuSkeletalMesh.h"
#include "Algorithm.h"

#include "CRenderMatrix.h"

//不同的Mesh结构
#include "CSkeletalMesh.h"
#include "CStaticMesh.h"
#include "CVertexAniMesh.h"

#include "ErrLogHelper.h"
#include "CGraphicExp.h"
#include "TSqrAllocator.inl"
#include "CDataSources.h"
#include "CModCoder.h"

const FourCC PortraitInfo::PICC('PTIF');
DEFINE_FILE_FORMAT(CPieceGroup);

CPieceGroup::CPieceGroup( const string& szName , const wstring& Alias)
: CResUnitGroup( eRT_eArp, szName,Alias ), m_pPortraitInfo(NULL) 
{
	m_bOnRead = false;
}

CPieceGroup::~CPieceGroup(void)
{
	if(m_bOnRead)
	{
		CSyncSystem::SafeDelCoder((SyncCoder*)0x1); //等Coder出现就好了
		//GfkLogErr("有读取终止","CPieceGroup");
	}

	GfkSafeDelete(m_pPortraitInfo);
	for ( int i = 0; i < (int)m_Piece.size(); i++ )
	{
		for ( size_t j = 0; j < m_Piece[i].size(); j++ )
		{
			if(m_Piece[i][j]->GetPieceGroup() == this)
				GfkSafeDelete(m_Piece[i][j]);
			else
				SafeRelease(m_Piece[i][j]);
		}
	}
}

const GVector<CPiece*>& CPieceGroup::GetPieceClass( const string& PieceClassName )
{
	static GVector<CPiece*> Null;
	GMap< GString, int >::iterator it = m_PieceClassIndex.find( PieceClassName.c_str() );
	if ( it != m_PieceClassIndex.end() )
	{
		for ( int i = 0; i < (int)m_Piece[it->second].size(); i++ )
			m_Piece[it->second][i]->AddRef();
		return m_Piece[it->second];
	}
	return Null;
}

const char* CPieceGroup::GetPieceClassName( int Num )
{
	return Num < (int)m_Piece.size() ? m_Piece[Num][0]->GetPieceClassName() : NULL;
}

int CPieceGroup::GetPieceClassAniNum()
{
	return (int)m_PieceAni.size();
}

const char* CPieceGroup::GetPieceClassAniName( int Ani )
{
	return m_PieceAni[Ani];
}

void CPieceGroup::_FreeRes(void)
{
	for ( int i = 0; i < (int)m_Piece.size(); i++ )
		for ( int j = 0; j < (int)m_Piece[i].size(); j++ )
			m_Piece[i][j]->_FreeRes();
}

void CPieceGroup::_Reclaim(void)
{
	for ( int i = 0; i < (int)m_Piece.size(); i++ )
		for ( int j = 0; j < (int)m_Piece[i].size(); j++ )
			m_Piece[i][j]->_Reclaim();
}

void  CPiece::_FreeRes(void)
{
	if(m_Mesh)
		m_Mesh->_FreeRes();

	GVector<CPieceRenderStyle*>::iterator it,eit = m_RenderStyle.end();
	for(it = m_RenderStyle.begin(); it!=eit; ++it)
	{
		(*it)->_FreeRes();
	}
}

void  CPiece::_Reclaim(void)
{
	if(m_Mesh)
		m_Mesh->_Reclaim();

	GVector<CPieceRenderStyle*>::iterator it,eit = m_RenderStyle.end();
	for(it = m_RenderStyle.begin(); it!=eit; ++it)
	{
		(*it)->_Reclaim();
	}
}

void	CPieceRenderStyle::_FreeRes(void)
{
	FreeTexture();
}

void	CPieceRenderStyle::_Reclaim(void)
{
	FreeTexture();
	GVector<GString>::iterator it,eit = m_TextureName.end();
	for( it = m_TextureName.begin(); it!=eit; ++it )
	{
		ITexture* pText = NULL;
		if ( SUCCEEDED( CGraphic::GetInst()->CreateTexture( PATH_ALIAS_RES.c_str(), (*it).c_str(), &pText ) ) )
			m_Texture.push_back( pText );
	}
}

////////////////////////////////////////////////////////////////////////
// 功能：构造和析构
// 参数：略
// 返回：无
//////////////////////////////////////////////////////////////////////////
CPieceRenderStyle::CPieceRenderStyle( )
{
	m_RenderStyle = NULL;
}

CPieceRenderStyle::~CPieceRenderStyle()
{
	SafeRelease( m_RenderStyle );
	
	for ( size_t i = 0; i < m_Texture.size(); ++i )
		SafeRelease( m_Texture[i] );
	
	m_Texture.clear();
}

void CPieceRenderStyle::GetCurRS( STATIC_RS& CurRS, uint32 FrameTime ,uint32 CurTime, void* pUser)
{
	if ( m_RenderStyle )
	{
		int nListNum = m_Texture.size();
		ITexture** ppTextureList = nListNum ? &m_Texture[0] : NULL;
		m_RenderStyle->GetCurRenderStyle( CurRS, ppTextureList, nListNum, FrameTime ,CurTime, pUser);
	}
}

int CPieceRenderStyle::BinSave( WriteDataInf& wdi )
{	
	int Size = (int)m_RenderStyleName.size();
	wdi.Write( (TCHAR*)&Size, sizeof(Size) );
	wdi.Write( (const char*)m_RenderStyleName.c_str(), Size );

	Size = (int)m_RSIndexName.size();
	wdi.Write( (TCHAR*)&Size,sizeof(Size) );
	wdi.Write( (const char*)m_RSIndexName.c_str(), Size );

	Size = (int)m_Texture.size();
	wdi.Write( (TCHAR*)&Size, sizeof(int) );
	for( size_t i = 0; i < m_Texture.size(); ++i )
	{
		string Name = m_Texture[i]->GetName();
		Size = (int)Name.size();
		wdi.Write( (TCHAR*)&Size, sizeof(Size) );
		wdi.Write( Name.data(), Size );
	}

	return 0;
}


CPieceRenderStyle* CPiece::_GetRenderStyle(uint32 Num)
{
	return m_RenderStyle[ min( Num, (uint32)m_RenderStyle.size() - 1 ) ];
}

////////////////////////////////////////////////////////////////////////
// 功能：渲染Piece
// 参数：const D3DXMATRIX* matMesh	矩阵列表,
//       uint32 SkeletalNum           骨骼矩阵数组,
//       float Frame			    要计算的帧,
//       int RSNum                  渲染类型,
//       uint32 Ani                  渲染的动画标志
// 返回：已写入的缓冲区
//////////////////////////////////////////////////////////////////////////
int CPiece::Render( uint32 SelfRSNum, float Dist, CRenderMatrix& ModelMatrix,
                    RenderObjStyle* pRORS, CRenderPiece* pRenderPiece, int32 iPiece,const PieceModify& Modify )
{
	CPieceRenderStyle* CurRS = NULL;
	if ( m_RenderStyle.size() == 0 )
		return 0;
	CurRS = _GetRenderStyle(SelfRSNum);
	if ( CurRS == NULL )
		return 0;

	uint32 fCurTime = uint32(ModelMatrix.m_CurFrame* 33.33f);
	m_RenderNode.m_eType = pRORS->m_eType;
	//fn.m_pVD = m_RenderNode.m_Op.vertexData->vertexDeclaration;

	STATIC_RS&		m_RS = m_RenderNode.m_RP.m_RS;
	CMatrix&		mat = m_RenderNode.m_RP.m_Mat;
	m_RenderNode.pRenderStyle = CurRS;
	m_RenderNode.pUser = pRenderPiece;

	if ( CurRS )
		CurRS->GetCurRS( m_RS, fCurTime ,ModelMatrix.m_CurrentTime, (void*)pRenderPiece);

	if ( !pRORS )
	{
		mat.Identity();
	}
	else if (ROS_REFRACT & pRORS->m_eType)
	{
		mat  = ModelMatrix.m_MatWorld;//	D3DXMatrixIdentity(&mat);
		sMaterial* pMat = (sMaterial*)pRORS->m_nParam2;
		if ( pRORS->m_nParam1&DELTARS_DIFFUSE )
		{
			m_RS.m_Material.Diffuse.r *= pMat->Diffuse.r;
			m_RS.m_Material.Diffuse.g *= pMat->Diffuse.g;
			m_RS.m_Material.Diffuse.b *= pMat->Diffuse.b;
		}
	}
	else
	{
		sMaterial* pMat = (sMaterial*)pRORS->m_nParam2;
		if ( pRORS->m_nParam1&DELTARS_ALPHADIF || ROS_ALPHA & pRORS->m_eType)
		{
			m_RS.m_Material.Diffuse.a	= pMat->Diffuse.a;
			m_RS.m_Alpharef			= (uint32)( m_RS.m_Alpharef*m_RS.m_Material.Diffuse.a );
			m_RS.m_AlpOP_S0			= TOP_MODULATE;
			m_RS.m_AlpArg1_S0		= TA_TEXTURE;
			m_RS.m_AlpArg2_S0		= TA_DIFFUSE;
			if ( m_RS.m_AlphaBlendEnable == FALSE )
			{
				m_RS.m_AlphaBlendEnable	= TRUE;
				m_RS.m_SrcBlend 		= BLEND_SRCALPHA;
				m_RS.m_DestBlend 		= BLEND_INVSRCALPHA;
			}
		}

		if (m_RS.m_AlphaBlendEnable == TRUE)
			m_RenderNode.m_eType = (ERenderObjStyle)((m_RenderNode.m_eType | ROS_ALPHA)  & ~ROS_SHADOWRECEIVE);

		if ( pRORS->m_nParam1&DELTARS_DIFFUSE )
		{
			m_RS.m_Material.Diffuse.r *= pMat->Diffuse.r;
			m_RS.m_Material.Diffuse.g *= pMat->Diffuse.g;
			m_RS.m_Material.Diffuse.b *= pMat->Diffuse.b;
		}
		if ( pRORS->m_nParam1&DELTARS_AMBIENT )
		{
			m_RS.m_Material.Ambient.r *= pMat->Ambient.r;
			m_RS.m_Material.Ambient.g *= pMat->Ambient.g;
			m_RS.m_Material.Ambient.b *= pMat->Ambient.b;
		}
		if ( pRORS->m_nParam1&DELTARS_SPECULAR )
		{
			m_RS.m_Material.Specular.r *= pMat->Specular.r;
			m_RS.m_Material.Specular.g *= pMat->Specular.g;
			m_RS.m_Material.Specular.b *= pMat->Specular.b;
		}
		if ( pRORS->m_nParam1&DELTARS_EMISSIVE )
		{
			m_RS.m_Material.Emissive.r *= pMat->Emissive.r;
			m_RS.m_Material.Emissive.g *= pMat->Emissive.g;
			m_RS.m_Material.Emissive.b *= pMat->Emissive.b;
		}
		if ( pRORS->m_nParam1&DELTARS_SPECPOWER )
		{
			m_RS.m_Material.Power *= pMat->Power;
		}
		mat  = ModelMatrix.m_MatWorld;//	D3DXMatrixIdentity(&mat);
	}

	if ( pRenderPiece->m_RS.m_nMask&PIECERS_DIFFUSE )
	{
		m_RS.m_Material.Diffuse.a *= float(pRenderPiece->m_RS.m_Diffuse.A) / 255.0f;
		m_RS.m_Material.Diffuse.r *= float(pRenderPiece->m_RS.m_Diffuse.R) / 255.0f;
		m_RS.m_Material.Diffuse.g *= float(pRenderPiece->m_RS.m_Diffuse.G) / 255.0f;
		m_RS.m_Material.Diffuse.b *= float(pRenderPiece->m_RS.m_Diffuse.B) / 255.0f;

		if ( m_RS.m_Material.Diffuse.a < 1.0f )
		{
			m_RS.m_AlphaBlendEnable	= TRUE;
			m_RS.m_SrcBlend			= BLEND_SRCALPHA;
			m_RS.m_DestBlend 		= BLEND_INVSRCALPHA;
			m_RS.m_AlpOP_S0			= TOP_MODULATE;
		}
	}
	if ( pRenderPiece->m_RS.m_nMask&PIECERS_AMBIENT )
	{
		m_RS.m_Material.Ambient.r *= float(pRenderPiece->m_RS.m_Ambient.R) / 255.0f;
		m_RS.m_Material.Ambient.g *= float(pRenderPiece->m_RS.m_Ambient.G) / 255.0f;
		m_RS.m_Material.Ambient.b *= float(pRenderPiece->m_RS.m_Ambient.B) / 255.0f;
	}
	if ( pRenderPiece->m_RS.m_nMask&PIECERS_SPECULAR )
	{
		m_RS.m_Material.Specular.r = max( float(pRenderPiece->m_RS.m_Specular.R) / 255.0f, m_RS.m_Material.Specular.r );
		m_RS.m_Material.Specular.g = max( float(pRenderPiece->m_RS.m_Specular.G) / 255.0f, m_RS.m_Material.Specular.g );
		m_RS.m_Material.Specular.b = max( float(pRenderPiece->m_RS.m_Specular.B) / 255.0f, m_RS.m_Material.Specular.b );
		m_RS.m_Material.Power	    = pRenderPiece->m_RS.m_fPower;
		m_RS.m_SpecularEnable		= TRUE;
	}
	if ( pRenderPiece->m_RS.m_nMask&PIECERS_SECONDTEX )
	{
		m_RS.m_Texture_S1		= pRenderPiece->m_RS.m_pTexture;
		m_RS.m_Texturefactor	= pRenderPiece->m_RS.m_BlendAlpha;
		m_RS.m_TexOP_S1		= TOP_BLENDFACTORALPHA;
		m_RS.m_TexArg1_S1	= TA_TEXTURE;
		m_RS.m_TexArg2_S1	= TA_CURRENT;
		m_RS.m_AlpOP_S1		= TOP_SELECTARG1;
		m_RS.m_AlpArg1_S1	= TA_CURRENT;
	}
	if (pRenderPiece->m_RS.m_nMask &PIECERS_CLIPPLANE)
	{
		m_RS.m_ClipPlane = pRenderPiece->m_RS.m_ClipPlane;
	}
	else
	{
		ZeroMemory(&m_RS.m_ClipPlane, sizeof(m_RS.m_ClipPlane));
	}
	m_RS.m_fRefractIndex = pRenderPiece->m_RS.m_fRefractIndex;
	//m_RenderNode.m_RP.m_RS.m_ShaderType.Clear();
	FilterNode* pNode = (FilterNode*)m_Mesh->Render(ModelMatrix,m_RenderNode);
	m_RenderNode.m_RP.m_RS.m_Texture_S0 = NULL;
	m_RenderNode.m_RP.m_RS.m_Texture_S1 = NULL;
	m_RenderNode.m_RP.m_RS.m_Texture_S2 = NULL;
	m_RenderNode.m_RP.m_RS.m_Texture_S3 = NULL;

	//这个方法只能在排序队列中使用
	if (Modify.Sorce!=0 && Modify.pAddVB != NULL && pNode)
	{
		pNode->vertexData->vertexDeclaration = CVertexDeclaration::GET_VERDECL(
		            pNode->vertexData->vertexDeclaration->GetVertexFormatID() | Modify.AddFVF);
		pNode->vertexData->vertexBufferBinding->SetBinding(Modify.Sorce,Modify.pAddVB,Modify.Offset,Modify.Lenght,Modify.pAddVB->getVertexSize());
		if(pNode->m_RP.m_RS.m_ShaderType.GetVSShaderID() != 0)
			pNode->m_RP.m_RS.m_ShaderType |= MakeSysVS_ID(SSI_PRE_RENDER_LIGHT);
		pNode->m_RP.m_RS.m_LightEnable	= FALSE;
		pNode->m_RP.m_RS.m_bPreLight	= true;
	}
	return 0;
};

bool CPieceGroup::IsValid()
{ 
	for( size_t i = 0; i < m_Piece.size(); ++i )
	{
		for( size_t j = 0; j < m_Piece[i].size(); ++j)
		{
			if(m_Piece[i][j] == NULL || !m_Piece[i][j]->IsValid())
				return false;
		}
	}
	return CResUnit::IsValid(); 
}

////////////////////////////////////////////////////////////////////
// 文件操作
int CPieceGroup::BinLoad(void)
{
	return CreateResFile();
}

void CPieceGroup::OnRead( CRefBufFile& GraphicFile )
{
	// zjtemp
#if 0
	CModFormat coder;
	coder._Load(this, GraphicFile);
#else
	Ast( m_Piece.empty() );

	DWORD Res;
	DWORD Version;
	GraphicFile.read( &Res, sizeof(DWORD) );
	GraphicFile.read( &Version, sizeof(DWORD) );

	DWORD tVersion;
	tVersion = GETVERSION(Version);

	if ( Res != MAKEFOURCC( 'M', 'O', 'D', 0 ) || tVersion > ARP_COMP_VERSION )
		return;

	m_bOnRead = true;
	int PieceNum;
	GraphicFile.read( &PieceNum, sizeof(int) );
	for ( int i = 0; i < PieceNum; i++ )
	{
		CPiece* pPiece = CDataSources::GetInst()->NewPiece(this);//new CPiece(this);
		if ( FAILED( pPiece->BinLoad( GraphicFile ,Version) )  )
		{
			delete pPiece;
			continue;
		}

		if ( m_PieceClassIndex.find( pPiece->GetPieceClassName() ) == m_PieceClassIndex.end() )
		{
			int nIndex = m_PieceClassIndex.size();
			m_PieceClassIndex[ pPiece->GetPieceClassName() ] = nIndex;
			m_Piece.push_back( CPieceClass() );
		}
		m_Piece[ m_PieceClassIndex[ pPiece->GetPieceClassName() ] ].push_back( pPiece );
	}

	//for ( int nPiece = 0; nPiece < (int)m_Piece.size(); nPiece++ )
	//{
	//	for ( int i = 0; i < (int)m_Piece[nPiece].size(); i++ )
	//	{
	//		if ( m_Piece[nPiece][i]->GetMesh()->GetVertexType() == VT_VERTEXANI )
	//		{
	//			CMesh* pMesh = m_Piece[nPiece][i]->GetMesh();
	//			for ( int j = 0; j < pMesh->GetAniNum(); j ++ )
	//			{
	//				int k = 0;
	//				IDNAME AniName = pMesh->GetAniName(j);
	//				for ( ; k < (int)m_PieceAni.size(); k++ )
	//					if ( m_PieceAni[k] == AniName )
	//						break;
	//				if ( k == (int)m_PieceAni.size() )
	//					m_PieceAni.push_back( AniName );
	//			}
	//		}
	//	}
	//}
	this->ReadPortraitInfo(GraphicFile);
	m_bOnRead = false;
#endif
}

//------------------------------------------------------------------------------
void CPieceGroup::ReadPortraitInfo( CBufFile& file )
{
	if (file.GetBufSize() <= file.Tell())
	{
		return;
	}

	uint fourcc = 0;
	file.Read(&fourcc, sizeof(fourcc));
	
	if (PortraitInfo::PICC != fourcc)
	{
		return;
	}
	m_pPortraitInfo = new PortraitInfo();
	file.Read(m_pPortraitInfo, sizeof(PortraitInfo));
}

//------------------------------------------------------------------------------
PortraitInfo*
CPieceGroup::GetPortraitInfo()
{
	return m_pPortraitInfo;
}

void CPiece::_SetPieceName( const string& strPieceName )
{
	if ( strPieceName.size() != 0 )
	{
		m_ClassName = strPieceName.c_str();
		size_t nPos = m_ClassName.rfind("_");
		if ( nPos != -1 )
			m_ClassName = strPieceName.substr(nPos+1, strPieceName.length()-nPos).c_str();

		m_sPieceName = m_ClassName;

		for ( size_t i = m_ClassName.size() - 1; i > 0 ; --i )
		{
			if ( m_ClassName[i] < '0' || m_ClassName[i] > '9' )
				break;
			m_ClassName.erase( i, 1 );
		}
	}
}

int CPiece::BinLoad( CBufFile& GraphicFile ,DWORD Version)
{
	int Size = 0;
	string sPieceName;

	LoadStringT( GraphicFile, sPieceName );
	_SetPieceName(sPieceName);
	GraphicFile.read( &Size, sizeof(int) );
	m_RenderStyle.resize( Size, NULL );
	GString szRsIndex;
	DWORD tVersionLow = GETVERSION(Version);
	for ( size_t i = 0; i < m_RenderStyle.size(); ++i )
	{
		m_RenderStyle[i] = CDataSources::GetInst()->NewPieceRenderStyle();//new CPieceRenderStyle();
		m_RenderStyle[i]->SetVersion(tVersionLow);
		if ( FAILED( m_RenderStyle[i]->BinLoad( GraphicFile ) ) )
		{
			ArtErr(string(((CPieceGroup*)m_pGroup)->GetName()) + "存在子文件缺失");
		}
		szRsIndex = m_RenderStyle[i]->GetRSIndexName();
		m_RSIndxToID[szRsIndex]=i;
	}
	//新的结构
	VERTEXTYPE	VertexType;
	GraphicFile.read( (char*)&VertexType, sizeof(VertexType) );

	switch (VertexType)
	{
		case VT_VERTEXANI:
			m_Mesh = CDataSources::GetInst()->NewVertexAniMesh();//new CVertexAniMesh;
			break;
		case VT_STATICVERTEX:
			m_Mesh = CDataSources::GetInst()->NewStaticMesh();//new CStaticMesh;
			break;
		case VT_SKELETALANI:
			m_Mesh = CDataSources::GetInst()->NewSkeletalMesh();//new CSkeletalMesh;
	}

	if ( FAILED( m_Mesh->BinLoad( GraphicFile ,Version, m_SkeletalMask ) ) )
		return GERROR;
	CVector3f Scale = m_Mesh->GetScale()*0.5f;
	const CVector3f& Off = m_Mesh->GetOff();

	Scale.x = CMath::Abs(Scale.x);
	Scale.y = CMath::Abs(Scale.y);
	Scale.z = CMath::Abs(Scale.z);
	
	m_AABox.setExtents( Off - Scale , Off + Scale );
	return 0;
}

bool CPiece::IsValid()
{
	if( m_RenderStyle.empty() )
	{
		ostringstream strm;
		strm<< m_sPieceName << "没有设置rs !";
		ArtErr(strm.str());
		return false;
	}

	if(!m_RenderStyle[0]->IsValid())
		return false;

	return true;
}

bool CPieceRenderStyle::IsValid()
{
	for( size_t i = 0; i<m_Texture.size(); ++i )
	{
		if(m_Texture[i] && !m_Texture[i]->IsValid())
			return false;
	}

	if( m_RenderStyle && !m_RenderStyle->IsValid() )
		return false;
	
	return true;
}

void CPieceRenderStyle::FreeTexture()
{
	size_t i = 0;
	for(;;)
	{
		SQR_TRY
		{
			for( ; i < m_Texture.size(); )
				 SafeRelease(m_Texture[i++]);
			break;
		}
		SQR_CATCH(exp)
		{
			GfkLogExp(exp);
		}
		SQR_TRY_END;
	}
	m_Texture.clear();
}

HRESULT CPieceRenderStyle::_CreateRenderStyle( const char* szName, CRenderStyle** pRS )
{
	return CGraphic::GetInst()->CreateRenderStyle( szName, pRS );
}

//#define ModifyFile

#ifdef ModifyFile
string ModifyFileName(const string& temp)
{
	if( temp.find("res") == -1)
		return temp;
	return temp.substr(temp.find("res") + 4,temp.size() - temp.find("res") - 4);
}
#endif

int CPieceRenderStyle::BinLoad( CBufFile& GraphicFile )
{
	int Return = 0;
	int Size = 0;
	GraphicFile.SafeRead( &Size, sizeof(Size) );

	string TempBuf(Size, 0);
	GraphicFile.SafeRead( &TempBuf[0], Size );
#ifdef ModifyFile
	m_RenderStyleName = ModifyFileName(TempBuf).c_str();
#else
	m_RenderStyleName = TempBuf.c_str();
#endif


	CRenderStyle* pRenderStyle;
	if(FAILED(_CreateRenderStyle(m_RenderStyleName.c_str(), &pRenderStyle)))
		Return = GERROR;
	m_RenderStyle = pRenderStyle ? (CRenderStyle*)pRenderStyle : NULL;

	//if (m_Version >= 10006)
	{
		GraphicFile.SafeRead( &Size, sizeof(Size) );
		TempBuf.resize(Size);
		GraphicFile.SafeRead( &TempBuf[0], TempBuf.size() );
		m_RSIndexName = TempBuf.c_str();
	}

	int TextNum = 0;
	GraphicFile.SafeRead( &TextNum, sizeof(int) );
	for ( int i = 0; i < TextNum; i++ )
	{
		//char TempBuf[FILENAME_MAX + 1];
		GraphicFile.SafeRead( &Size, sizeof(Size) );
		TempBuf.resize(Size);
		GraphicFile.SafeRead( &TempBuf[0], TempBuf.size() );

		if(Size == 0)
			continue;

#ifdef ModifyFile
		string tmp = ModifyFileName(TempBuf);
		_CreateStyleTexture(tmp.c_str());	
#else
		if(FAILED(_CreateStyleTexture(TempBuf.c_str())))	
			Return = GERROR;
#endif			
	}

	return Return;
}

HRESULT	CPieceRenderStyle::_CreateStyleTexture( const TCHAR* szTexName )
{
	ITexture* Text = NULL;
	if ( FAILED( CGraphic::GetInst()->CreateTexture( PATH_ALIAS_RES.c_str(), szTexName, &Text ) ) )
	{
		ostringstream strm;
		strm<<"CreateTexture failed with file \""<<szTexName<<"\"";
		ArtErr(strm.str());
		return GERROR;
	}
	else
	{
		m_TextureName.push_back(szTexName);
		m_Texture.push_back( Text );
	}
	return 0;
}
