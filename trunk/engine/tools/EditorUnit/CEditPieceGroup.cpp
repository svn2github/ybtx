#include "stdafx.h"
#include "CEditPieceGroup.h"
#include "CEditRenderStyle.h"
#include "BaseHelper.h"
#include "CEditMesh.h"
#include <fstream>
#include "GraphicCommon.h"
#include "CGraphic.h"
#include "CEditNullTexture.h"
#include "TSqrAllocator.inl"
#include "CEditTexture.h"
#include "CModCoder.h"

#define ENABLECP(VER)		0x00010000|VER	
#define ENABLEBOX(VER)		0x00100000|VER

CEditModFormat CEditPieceGroup::Format;

void	CEditPieceRenderStyle::ReLoad(const string& filename)
{
	m_RenderStyleName = filename.c_str();
	if(m_RenderStyle)
		((CEditRenderStyle*)m_RenderStyle)->ReLoad( filename );
	else
		 _CreateRenderStyle( filename.c_str(), &m_RenderStyle );
}	

void	CEditPieceRenderStyle::PushTexture( ITexture* pText )
{
	pText->AddRef();
	m_Texture.push_back( pText );
	m_TextureName.push_back(pText->GetName());
}

CEditPieceRenderStyle::CEditPieceRenderStyle( CEditPiece* pPiece, CEditRenderStyle* pRenderStyle )
{
	pPiece->AddRS(this);
	m_RenderStyle = pRenderStyle;
	m_RenderStyle->AddRef();
	m_RenderStyleName = m_RenderStyle->GetName();
}

CEditRenderStyle*	CEditPieceRenderStyle::GetEditRenderStyle(void)
{
	return (CEditRenderStyle*)m_RenderStyle;
}

void CEditPieceRenderStyle::SetParentPiece( CEditPiece* pPiece )
{
	m_pPiece = pPiece;
}

CEditPiece* CEditPieceRenderStyle::GetParentPiece()
{
	return m_pPiece;
}

HRESULT CEditPieceRenderStyle::_CreateRenderStyle( const TCHAR* szName, CRenderStyle** pRS )
{
	*pRS = (CRenderStyle*)CResMgr::GetInst()->FindResInSystemMem( szName, eRT_eRs );
	if ( *pRS )
	{
		( (CRenderStyle*)(*pRS) )->Reclaim();
		return 0;
	}

	CRenderStyle* Temp = new CEditRenderStyle( szName, L"res" );
	if ( SUCCEEDED( Temp->BinLoad() ) )
	{
		*pRS = Temp;
		Temp->SetContainerAndRecycle(CResMgr::GetInst()->SystemMemContainer(), CResMgr::GetInst()->SystemMemRecycle());
		return 0;
	}

	Temp->Release();
	return GERROR;
}

HRESULT	CEditPieceRenderStyle::_CreateStyleTexture( const TCHAR* szTexName )
{
	CEditTexture* Text;
	string texName;
	texName = szTexName;
	if(m_Path != "" && texName.find("\\")==string::npos)
		texName = m_Path + texName;//这里注意,批量处理后,如果没有创建成功的纹理,那么之前有的文件名也会没有的

	ITexture* pText;
	if ( FAILED( CGraphic::GetInst()->CreateTexture(  L"res", texName.c_str(), &pText ) ) )
	{
		ostringstream strm;
		strm<<"CreateTexture failed with file \""<<szTexName<<"\"";
		//MessageBox(NULL,strm.str().c_str(),"纹理找不到",0);		
		Text = new CEditNullTexture(szTexName);
		Text->AddRef();
	}
	else
	{
		Text = (CEditTexture* )pText;
	}	

	//CEditTexture::Format.Load(szTexName, Text);
	m_TextureName.push_back(texName.c_str());
	m_Texture.push_back( Text );
	return 0;
}

void	CEditPieceRenderStyle::SetRenderStyle( CEditRenderStyle* pRenderStyle )
{
	if( pRenderStyle != m_RenderStyle )
	{
		m_RenderStyleName = pRenderStyle->GetName();
		SafeRelease( m_RenderStyle );
		m_RenderStyle = pRenderStyle;
		m_RenderStyle->AddRef();
	}
}

int CEditPieceRenderStyle::BinSave( ofstream& File )
{	
	int Size = (int)m_RenderStyleName.size();
	File.write( (TCHAR*)&Size, sizeof(Size) );
	File.write( (const char*)m_RenderStyleName.c_str(), Size );

	Size = (int)m_RSIndexName.size();
	File.write( (TCHAR*)&Size,sizeof(Size) );
	File.write( (const char*)m_RSIndexName.c_str(), Size );
	//m_RenderStyle.BinSave( _strlwr( (TCHAR*)m_RenderStyleName.c_str() ) );

	Size = (int)m_Texture.size();
	File.write( (TCHAR*)&Size, sizeof(int) );
	for( size_t i = 0; i < m_Texture.size(); ++i )
	{
		string Name = m_Texture[i]->GetName();
		if(m_Path != "")
			Name = m_Path + Name;
		Size = (int)Name.size();
		File.write( (TCHAR*)&Size, sizeof(Size) );
		File.write( Name.data(), Size );
	}

	return 0;
}

bool CEditPiece::DecideIsSet(void)
{
	for( size_t i = 0; i < m_RenderStyle.size(); ++i )
	{
		if( m_RenderStyle[i]->GetRSName() == "" )
			return false;
		string rsname = m_RenderStyle[i]->GetRSName();
		if(rsname.find(":")!=string::npos||rsname.find("res")!=string::npos)
		{
			MessageBox(NULL,rsname.c_str(),"有rs为绝对路径,不能保存",MB_OK);
			return false;
		}
		int texnum = m_RenderStyle[i]->GetTextureNum();
		for(int j=0;j<texnum;j++)
		{
			string texname = m_RenderStyle[i]->GetTexture(j)->GetName();
			if(texname.find(":")!=string::npos||texname.find("res")!=string::npos)
			{
				MessageBox(NULL,texname.c_str(),"有tex为绝对路径,不能保存",MB_OK);
				return false;
			}
		}
	}
	return true;
}

void CEditPiece::SetForceRenderStyle(CEditRenderStyle* pStyle)
{
	if(pStyle==NULL)
	{
		GfkSafeDelete(m_pForceStyle);
		return;
	}

	if(m_pForceStyle==NULL)
		m_pForceStyle = new CEditPieceRenderStyle;

	if( m_pForceStyle->GetRenderStyle()!=pStyle )
		m_pForceStyle->SetRenderStyle(pStyle);
}

CPieceRenderStyle* CEditPiece::_GetRenderStyle(uint32 Num)
{
	if(m_pForceStyle)
	{
		CPieceRenderStyle* pStyle = CPiece::_GetRenderStyle(Num);
		m_pForceStyle->FreeTexture();

		int Num = pStyle->GetTextureNum();
		for(int i = 0; i<Num; ++i)
			m_pForceStyle->PushTexture(pStyle->GetTexture(i));

		return m_pForceStyle;
	}
	return CPiece::_GetRenderStyle(Num);
}

CPieceRenderStyle*	CEditPiece::_NewRenderStyle(void)
{
	CEditPieceRenderStyle* p = new CEditPieceRenderStyle(this);
	p->SetPath(m_Path);
	return p;
}

void CEditPiece::ChangeRsByName(const string& rsName,CEditPieceRenderStyle* pRs)
{
	int size = m_RenderStyle.size();
	for(int i=0;i<size;i++)
	{
		if(rsName == m_RenderStyle[i]->GetRSName())
		{
			pRs->SetVersion(m_RenderStyle[i]->GetVersion());
			pRs->SetRSIndexName(m_RenderStyle[i]->GetRSIndexName());
			m_RenderStyle[i] = (CPieceRenderStyle*)pRs;
		}
	}
}

void CEditPiece::AddRS( CEditPieceRenderStyle* rs )
{
	rs->SetParentPiece(this);
	m_RenderStyle.push_back( rs );
}

void CEditPiece::SetAABoxExtends( const CVector3f& min, const CVector3f& max )
{
	m_AABox.setExtents(min, max);
}

void CEditPiece::_SetPieceName( const string& strPieceName )
{
	if ( strPieceName.size() != 0 )
	{
		m_ClassName = strPieceName.c_str();
		size_t nPos = m_ClassName.rfind("_");
		if ( nPos != -1 )
			m_ClassName = strPieceName.substr(nPos+1, strPieceName.length()-nPos).c_str();

		m_sPieceName = strPieceName.c_str();

		for ( size_t i = m_ClassName.size() - 1; i > 0 ; --i )
		{
			if ( m_ClassName[i] < '0' || m_ClassName[i] > '9' )
				break;
			m_ClassName.erase( i, 1 );
		}
	}
}

void CEditPiece::InsertRenderStyle(int S,int D)
{
	GVector<CPieceRenderStyle*>::iterator iterS,iterD;

	if(S==D)
		return;
	iterD =iterS = m_RenderStyle.begin();
	iterS += S;
	iterD += D;

	CPieceRenderStyle *tmp = *iterS;

	if(S<D)
	{
		if((iterD+1)==m_RenderStyle.end())
		{
			m_RenderStyle.insert(m_RenderStyle.end(),tmp);
		}	
		else
			m_RenderStyle.insert(iterD+1,tmp);
		m_RenderStyle.erase(m_RenderStyle.begin() + S);
	}
	if(S>D)
	{
		m_RenderStyle.erase(iterS);
		m_RenderStyle.insert(iterD+1,tmp);
	}
}

void CEditPiece::SetPieceName( const string& Name )
{
	m_sPieceName = Name.c_str();
	const TCHAR* pStr = m_sPieceName.c_str();
	int i;
	for( i = 0; i < (int)m_sPieceName.size(); i++ )
		if( m_sPieceName[ m_sPieceName.size() - i - 1 ] == '_' )
			break;
	pStr += m_sPieceName.size() - i;
	m_ClassName = pStr;
	for( i = (int)m_ClassName.size() - 1; i > 0 ; i-- )
	{
		if( m_ClassName[i] < '0' || m_ClassName[i] > '9' )
			break;
		m_ClassName.erase( i, 1 );
	}

}

void CEditPiece::DeleteRS( int i )
{
	delete m_RenderStyle[i];
	m_RenderStyle.erase( m_RenderStyle.begin() + i ); 
}

void CEditPiece::SetPath(const string& Name)
{
	m_Path  = Name;
}

void CEditPiece::BinSave( ofstream& File )
{
	int Size = (int)m_sPieceName.size();
	File.write( (TCHAR*)&Size, sizeof(int) );
	File.write( m_sPieceName.c_str(), Size );

	Size = (int)m_RenderStyle.size();
	File.write( (TCHAR*)&Size, sizeof(int) );

	if( m_RenderStyle.empty() )
	{
		string log = m_sPieceName.c_str();
		log += "这个piece没有设置rs，请检查!";
		MessageBox(NULL, log.c_str(), "提示", MB_OK);	
	}

	for( size_t i = 0; i < m_RenderStyle.size(); ++i )
	{
		((CEditPieceRenderStyle*)m_RenderStyle[i])->SetPath(m_Path);
		((CEditPieceRenderStyle*)m_RenderStyle[i])->BinSave( File );
	}
	/*
	VT_VERTEXANI	= 0,
	VT_STATICVERTEX	= 1,
	VT_SKELETALANI	= 2,
	VT_SOFTBODYANI	= 3,	//暂时还不支持
	VT_SOFTCLOTHANI = 4,	//暂时还不支持
	VT_UNKNOW=0xffffffff,
	*/
	switch( m_Mesh->GetVertexType() )
	{
	case VT_VERTEXANI:
		((CEditVertexAniMesh*)m_Mesh)->BinSave( File );
		break;
	case VT_STATICVERTEX:
		((CEditStaticMesh*)m_Mesh)->BinSave( File );
		break;
	case VT_SKELETALANI:
		((CEditSkeletalMesh*)m_Mesh)->BinSave( File );
		break;
	case VT_SOFTBODYANI:
	case VT_SOFTCLOTHANI:
	case VT_UNKNOW:
	default:
		break;
	};
	return;
}

CEditMeshWriter* CEditPiece::GetMeshWriter()
{
	switch( m_Mesh->GetVertexType() )
	{
	case VT_VERTEXANI:
		return (CEditVertexAniMesh*)m_Mesh;
	case VT_STATICVERTEX:
		return (CEditStaticMesh*)m_Mesh;
	case VT_SKELETALANI:
		return (CEditSkeletalMesh*)m_Mesh;
	case VT_SOFTBODYANI:
	case VT_SOFTCLOTHANI:
	case VT_UNKNOW:
	default:
		break;
	};
	return NULL;
}

//CModFormat CEditPieceGroup::Format;

CEditPieceGroup::CEditPieceGroup( const string& szName, const wstring& Alias)
	: CPieceGroup( szName,Alias )
	//, CEditInfo(&Format)
	, m_dwVersion(0)
{
	m_TexPath = "";
}


int	CEditPieceGroup::ReLoad( const string& filename )
{
	m_szName = filename.c_str();

	// zjtemp
#if 0
	CModFormat modFormat;
	return modFormat.Load(m_szName.c_str(), this);
#else
	return CPieceGroup::BinLoad();
#endif
}

int	CEditPieceGroup::BinSave( const string& filename )
{
	size_t npos = filename.find(".mod");
	string tName = filename;
	if( npos == -1 )
		tName += ".mod";

	SetFileAttributes( tName.data(), FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_ARCHIVE );
	locale loc = locale::global(locale("")); //要打开的文件路径包含中文，设置全局locale为本地环境
	ofstream File( tName.data(), ios::binary|ios::out );
	locale::global(loc);

	DWORD Res = MAKEFOURCC( 'M', 'O', 'D', 0 );
	DWORD Version = ENABLECP(ARP_COMP_VERSION);
	Version = ENABLEBOX(Version);
	File.write( (TCHAR*)&Res, sizeof(DWORD) );
	File.write( (TCHAR*)&Version, sizeof(DWORD) );
	
	int Size = 0;//(int)m_Piece.size();

	for( size_t i = 0; i < m_Piece.size(); ++i )
		Size += (int)m_Piece[i].size();
	File.write( (TCHAR*)&Size, sizeof(int) );
	
	for( size_t i = 0; i < m_Piece.size(); ++i )
	{
		for( size_t j = 0; j < m_Piece[i].size(); ++j)
		{
			((CEditPiece*)m_Piece[i][j])->SetPath(m_TexPath);
			((CEditPiece*)m_Piece[i][j])->BinSave( File );
		}
	}
	
	// 头像信息
	if( m_pPortraitInfo )
	{
		File.write( (const char*)(&PortraitInfo::PICC),sizeof(FourCC) );
		File.write( (char*)(m_pPortraitInfo), sizeof(PortraitInfo));
	}

	File.close();

	return 0;
}

CPieceClass*	CEditPieceGroup::GetPieceClass( int index )
{
	return &(m_Piece[index]);
}

void CEditPieceGroup::DelPieceClass( int index )
{
	m_Piece.erase(m_Piece.begin() + index);
}

DWORD CEditPieceGroup::GetVersion(void)
{
	return m_dwVersion;
}

CPiece*	 CEditPieceGroup::_NewPiece(void)
{
	CEditPiece* p =  new CEditPiece(this);
	p->SetPath(m_TexPath);
	return p;
}

void CEditPieceGroup::SetPath(const string& Name)
{
	m_TexPath = Name;
}

bool CEditPieceGroup::Check()
{
	return true;
}

GVector< CPieceClass >& sqr_tools::CEditPieceGroup::GetPieceClass()
{
	return m_Piece;
}

void sqr_tools::CEditPieceGroup::Clear()
{
	m_Piece.clear();
	m_pPortraitInfo = 0;
}

void CEditPieceGroup::SetPortraitInfo( PortraitInfo* pi )
{
	m_pPortraitInfo = pi;
}

void sqr_tools::CEditPieceRenderStyle::FreeTextureNames()
{
	m_TextureName.clear();
}