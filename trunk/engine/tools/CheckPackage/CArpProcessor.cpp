#include "stdafx.h"
#include ".\carpprocessor.h"
#include <algorithm>
#include "CCoderNameHelper.h"
#include "CDataChunk.h"

#define GETVERSION(VER)	0x0000ffff&VER
#define GETCP(VER)  (0x00010000&VER)>>16
#define GETBOX(VER)	 (0x00100000&VER)>>20
#define ENABLECP(VER)		0x00010000|VER	
#define ENABLEBOX(VER)		0x00100000|VER
#define MAPCHANNEL0		192		
#define MAPCHANNEL1		256
#define MAPCHANNEL2		320
#define MAPCHANNEL3		384

template< size_t v, size_t t ,size_t mp>
inline size_t VerSize()
{
	enum { nSize = v*3 + 12 + t*2*mp };
	return ( nSize&7 ) ? nSize/8 + 1 : nSize/8;
}

size_t CalVetexSize( SaveInfo& SI )
{
	if( SI.sPos < 1024 && SI.sTex <= 1.02f )
		return VerSize<10,11,1>();
	else if( SI.sPos < 4096 && SI.sTex <= 2.04f )
		return VerSize<12,12,1>();
	else if( SI.sPos < 4096 && SI.sTex < 32.7f )
		return VerSize<12,16,1>();
	else if( SI.sPos < 16384 && SI.sTex < 32.7f )
		return VerSize<14,16,1>();
	else
		return VerSize<16,16,1>();
}
size_t CalVetexSize1( SaveInfo& SI )
{
	if( SI.sPos < 1024 && SI.sTex <= 1.02f )
		return VerSize<10,11,2>();
	else if( SI.sPos < 4096 && SI.sTex <= 2.04f )
		return VerSize<12,12,2>();
	else if( SI.sPos < 4096 && SI.sTex < 32.7f )
		return VerSize<12,16,2>();
	else if( SI.sPos < 16384 && SI.sTex < 32.7f )
		return VerSize<14,16,2>();
	else
		return VerSize<16,16,2>();
}
size_t CalVetexSize2( SaveInfo& SI )
{
	if( SI.sPos < 1024 && SI.sTex <= 1.02f )
		return VerSize<10,11,3>();
	else if( SI.sPos < 4096 && SI.sTex <= 2.04f )
		return VerSize<12,12,3>();
	else if( SI.sPos < 4096 && SI.sTex < 32.7f )
		return VerSize<12,16,3>();
	else if( SI.sPos < 16384 && SI.sTex < 32.7f )
		return VerSize<14,16,3>();
	else
		return VerSize<16,16,3>();
}
size_t CalVetexSizeNoTex( SaveInfo& SI )
{
	if( SI.sPos < 1024 && SI.sTex <= 1.02f )
		return VerSize<10,11,1>() - 11*2/8;
	else if( SI.sPos < 4096 && SI.sTex <= 2.04f )
		return VerSize<12,12,1>() - 12*2/8;
	else if( SI.sPos < 4096 && SI.sTex < 32.7f )
		return VerSize<12,16,1>() - 16*2/8;
	else if( SI.sPos < 16384 && SI.sTex < 32.7f )
		return VerSize<14,16,1>() - 16*2/8;
	else
		return VerSize<16,16,1>() - 16*2/8;
}

void LoadVertexFull1(VerTexBuf& buf,ifstream& File)
{
	File.read( (char*)&buf, 32 );
}

void LoadVertexFull2(VerTexBuf& buf,ifstream& File)
{
	File.read( (char*)&buf, 40 );

}
void LoadVertexFull3(VerTexBuf& buf,ifstream& File)
{
	File.read( (char*)&buf, 48 );
}

void LoadVertexFull(VerTexBuf& buf,ifstream& File)
{
	File.read( (char*)&buf, 24 );
}

int CModelLevel::BinLoadFull( ifstream& File, SaveInfo& SI ,int m_Channel)
{
	int Size;
	File.read( (char*)&m_Dist, sizeof( m_Dist ) );
	if( SI.eVetexType == VT_SKELETALANI )
	{
		File.read( (char*)&Size, sizeof(Size) );
		m_VerAnimate.resize(1);
		m_VerAnimate[0].m_FramesVertex.resize(1);
		m_VerAnimate[0].m_FramesVertex[0].resize(Size);
		m_Weight.resize(Size);
		m_SkeletalIndex.resize(Size);
		for( int i = 0; i < Size; i++ )
		{
			switch(m_Channel)
			{
			case 0:
			case 1:
				LoadVertexFull1(m_VerAnimate[0].m_FramesVertex[0][i],File);
				break;
			case 2:
				LoadVertexFull2(m_VerAnimate[0].m_FramesVertex[0][i],File);
				break;
			default:
				LoadVertexFull3(m_VerAnimate[0].m_FramesVertex[0][i],File);
				break;
			}
			File.read( (char*)&m_Weight[i], sizeof(uint32) );
			File.read( (char*)&m_SkeletalIndex[i], sizeof(uint32) );
		}
	}
	else if( SI.eVetexType == VT_STATICVERTEX )
	{
		File.read( (char*)&Size, sizeof(Size) );
		m_VerAnimate.resize(1);
		m_VerAnimate[0].m_FramesVertex.resize(1);
		m_VerAnimate[0].m_FramesVertex[0].resize(Size);
		switch(m_Channel)
		{
		case 0:
		case 1:
			for( int i = 0; i < Size; i++ )
				LoadVertexFull1(m_VerAnimate[0].m_FramesVertex[0][i],File);
			break;
		case 2:
			for( int i = 0; i < Size; i++ )
				LoadVertexFull2(m_VerAnimate[0].m_FramesVertex[0][i],File);
			break;
		default:
			for( int i = 0; i < Size; i++ )
				LoadVertexFull3(m_VerAnimate[0].m_FramesVertex[0][i],File);
			break;
		}

	}
	else if( SI.eVetexType == VT_VERTEXANI )
	{
		int FrameSize, VetexSize;
		File.read( (char*)&Size, sizeof(Size) );
		File.read( (char*)&VetexSize, sizeof(VetexSize) );
		m_VerAnimate.resize(Size);
		for( int i = 0; i < m_VerAnimate.size(); i++ )
		{
			File.read( (char*)&m_VerAnimate[i].m_AnimationeName, sizeof(m_VerAnimate[i].m_AnimationeName) );
			File.read( (char*)&m_VerAnimate[i].m_FramePerKeyFrame, sizeof( m_VerAnimate[i].m_FramePerKeyFrame ) );
			File.read( (char*)&FrameSize, sizeof(FrameSize) );

			m_VerAnimate[i].m_FramesVertex.resize( FrameSize );
			for( int j = 0; j < m_VerAnimate[i].m_FramesVertex.size(); j++ )
			{
				m_VerAnimate[i].m_FramesVertex[j].resize(VetexSize);

				for( int k = 0; k < VetexSize; k++ )
				{
					if( i || j )
						LoadVertexFull(m_VerAnimate[0].m_FramesVertex[0][i],File);
					else
						LoadVertexFull1(m_VerAnimate[0].m_FramesVertex[0][i],File);
				}
			}
		}
	}

	File.read( (char*)&Size, sizeof(Size) );
	m_VertexIndex.resize( Size );	
	for( int j = 0; j < m_VertexIndex.size(); j++ )
		File.read( (char*)&m_VertexIndex[j], m_VerAnimate[0].m_FramesVertex[0].size() < 256 ? sizeof(BYTE) : sizeof(WORD) );

	return 0;
}
int CModelLevel::BinLoad( ifstream& File, SaveInfo& SI ,int m_Channel)
{
	int Size;
	File.read( (char*)&m_Dist, sizeof( m_Dist ) );
	if( SI.eVetexType == VT_SKELETALANI )
	{
		File.read( (char*)&Size, sizeof(Size) );
		m_VerAnimate.resize(1);
		m_VerAnimate[0].m_FramesVertex.resize(1);
		m_VerAnimate[0].m_FramesVertex[0].resize(Size);
		m_Weight.resize(Size);
		m_SkeletalIndex.resize(Size);
		for( int i = 0; i < Size; i++ )
		{
			switch(m_Channel)
			{
			case 0:
			case 1:
				File.read( (char*)&m_VerAnimate[0].m_FramesVertex[0][i], CalVetexSize( SI ) );
				break;
			case 2:
				File.read( (char*)&m_VerAnimate[0].m_FramesVertex[0][i], CalVetexSize1( SI ) );
				break;
			default:
				File.read( (char*)&m_VerAnimate[0].m_FramesVertex[0][i], CalVetexSize2( SI ) );
				break;
			}
			//File.read( (char*)&m_VerAnimate[0].m_FramesVertex[0][i], CalVetexSize( SI ) );
			File.read( (char*)&m_Weight[i], sizeof(uint32) );
			File.read( (char*)&m_SkeletalIndex[i], sizeof(uint32) );
		}
	}
	else if( SI.eVetexType == VT_STATICVERTEX )
	{
		File.read( (char*)&Size, sizeof(Size) );
		m_VerAnimate.resize(1);
		m_VerAnimate[0].m_FramesVertex.resize(1);
		m_VerAnimate[0].m_FramesVertex[0].resize(Size);
		switch(m_Channel)
		{
		case 0:
		case 1:
			for( int i = 0; i < Size; i++ )
				File.read( (char*)&m_VerAnimate[0].m_FramesVertex[0][i], CalVetexSize( SI ) );
			break;
		case 2:
			for( int i = 0; i < Size; i++ )
				File.read( (char*)&m_VerAnimate[0].m_FramesVertex[0][i], CalVetexSize1( SI ) );
			break;
		default:
			for( int i = 0; i < Size; i++ )
				File.read( (char*)&m_VerAnimate[0].m_FramesVertex[0][i], CalVetexSize2( SI ) );
			break;
		}

	}
	else if( SI.eVetexType == VT_VERTEXANI )
	{
		int FrameSize, VetexSize;
		File.read( (char*)&Size, sizeof(Size) );
		File.read( (char*)&VetexSize, sizeof(VetexSize) );
		m_VerAnimate.resize(Size);
		for( int i = 0; i < m_VerAnimate.size(); i++ )
		{
			File.read( (char*)&m_VerAnimate[i].m_AnimationeName, sizeof(m_VerAnimate[i].m_AnimationeName) );
			File.read( (char*)&m_VerAnimate[i].m_FramePerKeyFrame, sizeof( m_VerAnimate[i].m_FramePerKeyFrame ) );
			File.read( (char*)&FrameSize, sizeof(FrameSize) );

			m_VerAnimate[i].m_FramesVertex.resize( FrameSize );
			for( int j = 0; j < m_VerAnimate[i].m_FramesVertex.size(); j++ )
			{
				m_VerAnimate[i].m_FramesVertex[j].resize(VetexSize);

				for( int k = 0; k < VetexSize; k++ )
				{
					if( i || j )
						File.read( (char*)&m_VerAnimate[i].m_FramesVertex[j][k], CalVetexSizeNoTex( SI ) );
					else
						File.read( (char*)&m_VerAnimate[i].m_FramesVertex[j][k], CalVetexSize( SI ) );
				}
			}
		}
	}

	File.read( (char*)&Size, sizeof(Size) );
	m_VertexIndex.resize( Size );	
	for( int j = 0; j < m_VertexIndex.size(); j++ )
		File.read( (char*)&m_VertexIndex[j], m_VerAnimate[0].m_FramesVertex[0].size() < 256 ? sizeof(BYTE) : sizeof(WORD) );

	return 0;
}
//////////////////////////////////////////////////////////////////////////
//柔体结构
//////////////////////////////////////////////////////////////////////////
CSoftLevel::CSoftLevel( CProMesh* pMesh ) : CMeshLevel( pMesh )
{
	//=================node初始化===================//
	m_nodePhyPara.m_fAccGravity       = 9.8f;             //重力加速度（牛顿/千克）
	m_nodePhyPara.m_fMass             = 0.09f;//0.005;    //每个质点的质量
	m_nodePhyPara.m_fCoeDamp          = 0.1f  ;           //阻力系数

	//==============spring变量初始化================//
	m_spInfo.m_fRatioPull     = 1.1f;             //最大拉长比率（与自然长度相比）
	m_spInfo.m_fRatioCompress = 0.9f;             //最小缩短比率（与自然长度相比）
	m_spInfo.m_fCoe			  = 0.98f;

	m_arrSpNode.clear();
	m_arrVertex.clear();

	m_eSoftType = SoftPlane;
}

int CSoftLevel::BinLoad( ifstream& File, SaveInfo& SI )
{
	//读出柔体类型
	File.read( (char*)&m_eSoftType,   sizeof(m_eSoftType) );

	//读出每个结点的物理参数参数信息
	File.read( (char*)&m_nodePhyPara, sizeof(NodePhyPara) );
	//读出弹簧信息
	File.read( (char*)&m_spInfo, sizeof(SpringInfo) );

	File.read( (char*)&m_vecTop, sizeof( m_vecTop ) );

	//读出节点
	int nSpNode;
	File.read( (char*)&nSpNode,  sizeof(int) );
	m_arrVertex.clear();
	m_arrSpNode.clear();
	m_arrVertex.resize( nSpNode );
	m_arrSpNode.resize( nSpNode );

	float y = 0;
	for( int i = 0; i < nSpNode; i++ )
	{	
		File.read( (char*)&m_arrVertex[i].m_fTU1, sizeof( float ) );	
		File.read( (char*)&m_arrVertex[i].m_fTV1, sizeof( float ) );	
		File.read( (char*)&m_arrVertex[i].m_fTU2, sizeof( float ) );	
		File.read( (char*)&m_arrVertex[i].m_fTV2, sizeof( float ) );	
		File.read( (char*)&m_arrVertex[i].m_fNatualLen, sizeof( float ) );
		File.read( (char*)&m_arrVertex[i].m_fWidth, sizeof( float ) );	

		y -= m_arrVertex[i].m_fNatualLen;
		m_arrSpNode[i].Init( m_arrVertex[i], y );	
	}

	//读出要碰撞检测的骨骼ID
	DWORD nBnCount;
	File.read( (char*)&nBnCount, sizeof(DWORD) );
	m_arrCollideStatic.clear();
	m_arrCollideDynamic.clear();
	m_arrCollideStatic.resize( nBnCount );
	m_arrCollideDynamic.resize( nBnCount );
	if(!m_arrCollideStatic.empty())
		File.read( (char*)&m_arrCollideStatic[0], sizeof( CollideStatic )*nBnCount );

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

int CProMesh::BinLoad( ifstream& File ,DWORD Version)
{
	DWORD tVersionBox = GETBOX(Version);
	DWORD tVersionCp = GETCP(Version);
	DWORD tVersionLow = GETVERSION(Version);
	File.read( (char*)&m_eVertexType, sizeof(m_eVertexType) );
	File.read( (char*)&m_fBaize, sizeof( m_fBaize ) );
	File.read( (char*)&m_OrgScale, sizeof( m_OrgScale ) );
	File.read( (char*)&m_OrgOff, sizeof( m_OrgOff ) );
	File.read( (char*)&m_Scale, sizeof( m_Scale ) );
	File.read( (char*)&m_Off, sizeof( m_Off ) );
	File.read( (char*)&m_sTex, sizeof( m_sTex ) );

	if(tVersionLow>=10005)
		File.read( (char*)&m_Channel,sizeof(m_Channel));

	if(tVersionLow>=10007&&tVersionCp)
		File.read( (char*)&m_IsCompress,sizeof(m_IsCompress));

	if(tVersionLow==10007||tVersionBox)
	{
		DWORD BoxCount = 0;
		File.read( (char*)&BoxCount,sizeof(DWORD) );		
		uint16 ID;
		CVector3f MinPos;
		CVector3f MaxPos;

		for(DWORD j = 0;j<BoxCount;++j)
		{
			CBaseAABB Box;
			File.read( (char*)&ID,sizeof(uint16));
			File.read( (char*)&MinPos,sizeof(CVector3f) );
			File.read( (char*)&MaxPos,sizeof(CVector3f) );
			Box.m_vMinimum = MinPos;  Box.m_vMaximum = MaxPos;
			m_BoxMap.insert(BoxMap::value_type(ID,Box));
		}
	}

	SaveInfo SI( m_OrgScale, m_OrgOff, m_sTex, m_eVertexType );

	if( m_eVertexType == VT_SOFTCLOTHANI )
	{
		m_MeshLevels.resize( 1, NULL );
		m_MeshLevels[0] = new CSoftClothLevel( this );
		m_MeshLevels[0]->BinLoad( File, SI ,m_Channel);
	}
	else
	if( m_eVertexType == VT_SOFTBODYANI )
	{
		m_MeshLevels.resize( 1, NULL );
		m_MeshLevels[0] = new CSoftLevel( this );
		m_MeshLevels[0]->BinLoad( File, SI ,m_Channel);
	}
	else
	{
		int Size;
		File.read( (char*)&Size, sizeof(Size) );
		m_MeshLevels.resize( Size, NULL );
		if(!m_IsCompress)
		{
			for( int i = 0; i < (int)m_MeshLevels.size(); i++ )
			{
				m_MeshLevels[i] = new CModelLevel( this );
				m_MeshLevels[i]->BinLoadFull( File, SI ,m_Channel);
			}
		}
		else
		{
			for( int i = 0; i < (int)m_MeshLevels.size(); i++ )
			{
				m_MeshLevels[i] = new CModelLevel( this );
				m_MeshLevels[i]->BinLoad( File, SI ,m_Channel);
			}
		}

	}

	return 0;
}

int CPieceProcessorGroup::BinLoad( string LoadName, CBaseResProcessor* pProcessor )
{
	locale loc = locale::global(locale("")); //要打开的文件路径包含中文，设置全局locale为本地环境
	ifstream File( LoadName.data(), ios::binary|ios::in );
	locale::global(loc);
	if( File.fail() )
	{
		return GERROR;
	}

	DWORD Res;
	DWORD Version;
	File.read( (char*)&Res, sizeof(DWORD) );
	File.read( (char*)&Version, sizeof(DWORD) );

	m_dwVersion = Version;

	if(LoadName.find("face01")!=string::npos)
	{
		int i=0;
	}

	DWORD tVersion = GETVERSION(Version);
	if(tVersion<ARP_COMP_VERSION)
	{
		//string err;
		//err += LoadName;
		//err += "这个文件不是最新版本,可能会导致问题,请保存成最新版本!!";
		//printf("%s\n", err.c_str());
	}
	
	if( Res != MAKEFOURCC( 'M', 'O', 'D', 0 ) || tVersion > ARP_COMP_VERSION )
	{
		return GERROR;
	}

	int Size;
	File.read( (char*)&Size, sizeof(int) );
	m_Piece.resize( Size, NULL );
	for( int i = 0; i < (int)m_Piece.size(); i++ )
	{
		m_Piece[i] = new CPieceProcessor;
		if( FAILED( m_Piece[i]->BinLoad( File, LoadName, pProcessor ,Version) ) )
		{
			return GERROR;
		}
	}
	ZeroMemory(&m_portraitInfo, sizeof(m_portraitInfo));
	if (!File.eof())
	{
		File.read(reinterpret_cast<char*>(&m_portraitInfo.fourcc), sizeof(m_portraitInfo.fourcc));
		if (m_portraitInfo.fourcc == 'PTIF')
		{
			File.read(reinterpret_cast<char*>(&m_portraitInfo.translate),
				sizeof(m_portraitInfo) - sizeof(m_portraitInfo.fourcc));
		}
	}
	File.close();

	return 0;
}

int CPieceProcessor::BinLoad( ifstream& File, string LoadName, CBaseResProcessor* pProcessor ,DWORD Version)
{	
	int Size;

	File.read( (char*)&Size, sizeof(int) );
	m_Name.resize( Size );
	File.read( (char*)m_Name.data(), Size );

	const char* pStr = m_Name.c_str();
	int i = 0;
	for( ; i < (int)m_Name.size(); i++ )
		if( m_Name[ m_Name.size() - i - 1 ] == '_' )
			break;
	pStr += m_Name.size() - i;
	m_ClassName = pStr;
	for( i = (int)m_ClassName.size() - 1; i > 0 ; i-- )
	{
		if( m_ClassName[i] < '0' || m_ClassName[i] > '9' )
			break;
		m_ClassName.erase( i, 1 );
	}

	File.read( (char*)&Size, sizeof(int) );
	m_RenderStyle.resize( Size, NULL );
	DWORD tVersionLow = GETVERSION(Version);
	for( int i = 0; i < (int)m_RenderStyle.size(); i++ )
	{
		m_RenderStyle[i] = new CPieceRenderStyleProcessor;
		m_RenderStyle[i]->SetVersion(tVersionLow);
		m_RenderStyle[i]->BinLoad( File, LoadName, pProcessor );
	}

	return m_Mesh.BinLoad( File ,Version);
}

bool comp(char cLeft,char cRight)
{
	return cLeft==cRight && (cLeft=='\\' || cLeft=='/');
}

template< class RF >
inline void StreamLoadStringT( RF& oFile, std::string& str )
{
	int Size;
	std::vector<char> Temp;

	oFile.read( (char*)&Size, sizeof( int ) );
	Temp.resize( Size + 1 );
	memset( &Temp[0], 0, Size + 1 );
	oFile.read( &Temp[0], Size );
	str = &Temp[0];
}
void CPieceRenderStyleProcessor::SetVersion(DWORD Ver)
{
	m_Version = Ver;
}
int CPieceRenderStyleProcessor::BinLoad( ifstream& File, string LoadName, CBaseResProcessor* pProcessor )
{	
	StreamLoadStringT( File, m_RenderStyleName );
	m_RenderStyleName.erase(unique(m_RenderStyleName.begin(),m_RenderStyleName.end(),comp),m_RenderStyleName.end());

	if( m_RenderStyleName.size() )
	{
		size_t npos = m_RenderStyleName.rfind(".marl");

		if( npos != -1 )
			pProcessor->GetMgr()->ProcessWith( m_RenderStyleName.c_str(),LoadName.c_str() ) ;
	}

	if(m_Version>=10006)
	{
		StreamLoadStringT( File, m_RSIndexName );
		m_RSIndexName.erase(unique(m_RSIndexName.begin(),m_RSIndexName.end(),comp),m_RSIndexName.end());
	}
	int TextNum;
	File.read( (char*)&TextNum, sizeof(int) );
	for( int i = 0; i < TextNum; i++ )
	{
		string Name;
		StreamLoadStringT( File, Name );

		Name.erase(unique(Name.begin(),Name.end(),comp),Name.end());		
		
		if( Name.size() )
		{
			const char* szExt = Name.c_str() + Name.length() - 3;
			pProcessor->GetMgr()->ProcessWith( Name.c_str(), LoadName.c_str() ) ;
		}
		
		m_Texture.push_back( Name );
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
// 柔布(披风)结构
//////////////////////////////////////////////////////////////////////////

CSoftClothLevel::CSoftClothLevel( CProMesh* pMesh ) : CMeshLevel( pMesh )
{
	m_pMesh = pMesh;
	m_VertexBuf = NULL;
	m_VertexNum = 0;
	m_ClothWidth  = 0;
	m_ClothHeight = 0;
	m_FooCloth    = NULL;
}

CSoftClothLevel::~CSoftClothLevel()
{
	if (m_FooCloth) delete m_FooCloth;
	if (m_VertexBuf) delete[] m_VertexBuf;
	//--    delete m_Cloth;
}

int CSoftClothLevel::BinLoad( ifstream& GraphicFile, SaveInfo& SI )
{

	//GraphicFile.read( (void*)&m_ClothWidth, sizeof(uint8) );
	//GraphicFile.read( (void*)&m_ClothHeight, sizeof(uint8) );

	GraphicFile.read( (char*)&m_ClothProp, sizeof(SoftClothProp) );

	m_ClothWidth    = m_ClothProp.Width;
	m_ClothHeight   = m_ClothProp.Height;

	int VertexSize = 0;

	m_VertexBuf = new VerNorTex[m_ClothWidth * m_ClothHeight];
	m_VertexNum = m_ClothWidth * m_ClothHeight;

	//////////////
	VertexSize = 0;
	for(int j=0; j<m_ClothWidth; ++j)
	{
		CVector3f p;
		CTinyWeight ff;
		BYTE		b[4];
		GraphicFile.read( (char*)&p, sizeof(CVector3f) );
		GraphicFile.read( (char*)&ff, sizeof(CTinyWeight) );
		GraphicFile.read( (char*)b,  sizeof(BYTE)*4 );
		m_VertexBuf[VertexSize++] = VerNorTex( p.x, p.y, p.z, 0, 0, 0, 0, 0);

		float f[4];
		f[0] = ff.GetWeight(0); 
		f[1] = ff.GetWeight(1);
		f[2] = ff.GetWeight(2);
		f[3] = ff.GetWeight(3);
		m_TopFixVerBuf.push_back( VertexFomat( VerNorTex( p.x, p.y, p.z, 0, 0, 0, 0, 0), f, b) );
	}

	// 输出横向弹簧
	for(int i=0; i< m_ClothHeight * (m_ClothWidth-1); ++i)
	{
		float l;
		GraphicFile.read( (char*)&l, sizeof(float) );
		this->m_HSprings.push_back(l);
	}

	// 输出纵向弹簧
	for(int i=0; i < m_ClothWidth * (m_ClothHeight-1); ++i)
	{
		float l;
		GraphicFile.read( (char*)&l, sizeof(float) );
		this->m_VSprings.push_back(l);
	}
	//////////////

	VertexSize = 0;
	for(int i=0; i<m_ClothHeight; ++i)
	{
		for(int j=0; j<m_ClothWidth; ++j)
		{
			uint16 u, v;
			GraphicFile.read( (char*)&u, sizeof(uint16) );
			GraphicFile.read( (char*)&v, sizeof(uint16) );

			if( i > 0 )
			{
				CVector3f p;
				m_VertexBuf[VertexSize].p.x = m_VertexBuf[VertexSize-1].p.x;
				m_VertexBuf[VertexSize].p.z = m_VertexBuf[VertexSize-1].p.z;
				m_VertexBuf[VertexSize].p.y = m_VertexBuf[VertexSize-1].p.y - 50.0f;				
			}

			m_VertexBuf[VertexSize].tu = u / 2046.0f;
			m_VertexBuf[VertexSize].tv = v / 2046.0f;
			m_TextureU.push_back(u);
			m_TextureV.push_back(v);

			VertexSize++;
		}
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
			m_IndexBuf.push_back( ibase + j);
			m_IndexBuf.push_back( ibase + j + 1);
			m_IndexBuf.push_back( ibase + j + m_ClothWidth );

			m_IndexBuf.push_back( ibase + j + 1);
			m_IndexBuf.push_back( ibase + j + m_ClothWidth + 1);
			m_IndexBuf.push_back( ibase + j + m_ClothWidth );
		}
		ibase += m_ClothWidth;
	}

	uint8 CollideNum;
	GraphicFile.read( (char*)&CollideNum, sizeof(uint8));

	for( uint8 i = 0; i < CollideNum; ++i)
	{

		CapsuleNode CsNod;
		GraphicFile.read( (char*)&CsNod, sizeof(CapsuleNode) );

		float f[4];
		f[0] = CsNod.w.GetWeight(0);
		f[1] = CsNod.w.GetWeight(1);
		f[2] = CsNod.w.GetWeight(2);
		f[3] = CsNod.w.GetWeight(3);

		m_Collides.push_back( CollideCapsule( CsNod.Begin, CsNod.End, CsNod.Raidus, f, CsNod.b) );
	}

	if (m_FooCloth) delete m_FooCloth;
	CDynamicSoftCloth _FooCloth;
	InitDynData( _FooCloth );
	m_FooCloth = new  CDynamicSoftCloth( _FooCloth );

	return 0;
}

void CSoftClothLevel::InitDynData( CDynamicSoftCloth& Data )
{
	Data.m_ClothProp    = &this->m_ClothProp;
	Data.m_Width        = this->m_ClothWidth;
	Data.m_Height       = this->m_ClothHeight;
	Data.m_VertexBuf    = new VerNorTex[ this->m_VertexNum ];
	Data.m_HSprings	    = &this->m_HSprings;
	Data.m_VSprings		= &this->m_VSprings;
	memcpy( (void*)Data.m_VertexBuf, (void*)m_VertexBuf, sizeof(VerNorTex) * m_VertexNum );

	for( int i = 0 ; i < this->m_TopFixVerBuf.size(); ++i)
		Data.m_TopFixVerBuf.push_back( *((CDynamicSoftCloth::VertexFomat*)&this->m_TopFixVerBuf[i]) );

	Data.m_Cloth        = new Cloth::CCloth;
	Data.m_Cloth->SetWind( Data.m_ClothProp->WindMin, Data.m_ClothProp->WindMax, Data.m_ClothProp->TimePass );
	Data.m_VertexNum	= this->m_VertexNum;
	Data.m_Capsules.resize( this->m_Collides.size() );
}

CArpProcessor::CArpProcessor(void)
	: CCoderProcessor(CCoderNameHelper::MOD_CODER, 0)
{
	RegProcessTrunk(CCoderNameHelper::TEX_REF_CODER);
	RegProcessTrunk(CCoderNameHelper::RS_REF_CODER);
}

CArpProcessor::~CArpProcessor(void)
{
}

bool CArpProcessor::RunProcess( const char* szFilename )
{
	ChangeToSrcDir();

	if ( CheckIsCoderFormat(szFilename) == true )
	{
		return CCoderProcessor::RunProcess(szFilename);
	}
	
	CPieceProcessorGroup Group;
	if( FAILED( Group.BinLoad( szFilename, this ) ) )
	{
		return false;
	}
	
	m_pMgr->PostProcessed(szFilename);
		
	return true;
}

bool CArpProcessor::CheckIsCoderFormat( const char* szFileName )
{
	SQR_TRY
	{
		if ( string(szFileName).find("tile/史城城外/史城城门/model/sccw_sccq_cmxj.mod") != string::npos )
		{
			printf("");
		}
		

		CPkgFile PkgFile;
		if( PkgFile.Open( 0, szFileName ) != eFE_SUCCESS )
			return false;

		if ( PkgFile.Size() < sizeof(CDataChunk::ChunkHdr) )
			return false;

		char* pBuf = new char[PkgFile.Size()];
		PkgFile.read( pBuf, PkgFile.Size() );
		CBufFile* pBufFile = new CBufFile( pBuf, PkgFile.Size(), PkgFile.Offset() );
		CDataChunk dataChunk;
		pBufFile->SafeRead(&dataChunk.GetHdr(),sizeof(CDataChunk::ChunkHdr));
		if ( dataChunk.GetName() != CCoderNameHelper::MOD_CODER )
		{
			delete[] pBuf;
			return false;
		}

		delete[] pBuf;

		return true;
	}
	SQR_CATCH(exp)
	{
		return false;
	}
	SQR_TRY_END

	return true;
}
