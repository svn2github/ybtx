#include "stdafx.h"
#include "CEffectUnit.h"
#include "BaseHelper.h"
#include "CVector4.inl"
#include "TSqrAllocator.inl"
#include "CEffectGroup.h"
#include "CGraphic.h"
enum
{
	CEFFECTUNITPROP_ORIGIN		= 0,
	CEFFECTUNITPROP_ADD_LOD		= 1,
	CEFFECTUNITPROP_ADD_VEL		= 2,
	CEFFECTUNITPROP_ADD_SLOWNDISAPPEAR		= 3,
	CEFFECTUNITPROP_FORCE_DWORD = 0xFFFFFFFF,
};
#define MaxLod 0.99999999f
#define MinLod 0.00000001f

CEffectUnitProp::CEffectUnitProp( uint32 ID, IEffectClassDesc* pEffectClassDesc ) 
				: m_ID(ID)
				, m_pEffectClassDesc( (CEffectClassDesc*)pEffectClassDesc )
				, m_bTextValid(false)
				, m_EffectLod(MinLod)
				, m_isUseVel(true)
				, m_bSlownDisappear(true)
{
	m_eUpdatePos			= UP_FIXED;
	m_szAttachName			= "";
	m_szAniName.			insert( "all" );
	m_nTextureCircle		= 1;

	m_vecOffset[0.0f]		= CVector3f( 0, 0, 0 );
	m_XYZcale[0.0f]			= CVector3f( 10.0f, 10.0f, 10.0f );
	m_UVRCenter[0.0f]		= CVector3f( 0.5f,  0.5f,  0.5f );
	m_Color[0.0f]			= 0xffffffff;
	m_Color[1.0f]			= 0xffffffff;
	m_Scale[0.0f]			= 0xff;
	m_Scale[1.0f]			= 0xff;
	m_TextName[0.0f]		= "None";
	m_pEffectProp			= NULL;
	AddRef();
}

CEffectUnitProp::~CEffectUnitProp(void)
{
	for( EMap<float,ITexture*>::iterator it = m_Texture.begin(); it != m_Texture.end(); ++it )
		SafeRelease( it->second );
	m_pEffectProp = NULL;
}

void CEffectUnitProp::SetEffectLod(float Lod)				
{
	m_EffectLod = (Lod>MaxLod||Lod<MinLod)?MinLod:Lod;
}

float CEffectUnitProp::GetEffectLod(void)					
{ return m_EffectLod; }

//#define ModifyEfxFile
#ifdef ModifyEfxFile
EString ModifyEfxFileName(EString& temp)
{
	replace(temp.begin(), temp.end(), '\\', '/');
	if( temp.find("res/") == -1)
		return temp;
	return temp.substr(temp.find("res/") + 4,temp.size() - temp.find("res/") - 4);
}
#endif
int32 CEffectUnitProp::LoadBin( IEffectRead& File )
{
	EString	VerStr = "";
	uint32 Version = CEFFECTUNITPROP_ORIGIN;
	
	LoadStringT( File, VerStr );
	if(VerStr == "BEUP")
	{
		File.read( (char*)&Version,	sizeof(uint32) );
		LoadStringT( File, VerStr );
	}

	m_szAttachName = VerStr;
	LoadStringT( File, m_CustomName );
	LoadStringT( File, m_EffectClassName );

	int32 Size = 0;
	m_szAniName.clear();
	File.read( (char*)&Size,					sizeof(int32) );
	for( int32 i = 0; i < Size; ++i )
	{
		EString szStr = "";
		LoadStringT( File, szStr );
		m_szAniName.insert( szStr );
	}

	switch(Version)
	{
	case CEFFECTUNITPROP_ADD_SLOWNDISAPPEAR:
		File.read( (TCHAR*)&m_bSlownDisappear,	sizeof(bool) );
	case CEFFECTUNITPROP_ADD_VEL:
		File.read( (char*)&m_isUseVel,			sizeof(bool) );
	case CEFFECTUNITPROP_ADD_LOD:
		File.read( (char*)&m_EffectLod,			sizeof(float) );
	default:
		File.read( (char*)&m_TimeRange,			sizeof(int32) );
		File.read( (char*)&m_eUpdatePos,		sizeof(eUpdatePosType) );
		File.read( (char*)&m_TimeRange,			sizeof(int32) );
		File.read( (char*)&m_nTextureCircle,	sizeof(int32) );

		LoadMapValue( File, m_vecOffset );
		LoadMapValue( File, m_Color );
		LoadMapValue( File, m_Scale );


		File.read( (char*)&Size, sizeof( int32 ) );
		for( int32 i = 0; i < Size; ++i )
		{
			float Pos = 0.0f;
			EString szStr = "";
			File.read( (char*)&Pos, sizeof(float) );
			LoadStringT( File, szStr );
#ifdef ModifyEfxFile
			szStr = ModifyEfxFileName(szStr).c_str();
#endif
			m_TextName[Pos] = szStr;
		}
	}

	_CalculateAABB();
	return 0;
}

int32 CEffectUnitProp::SaveBin( IEffectWrite& File )
{
	int32 Size;
	//uint32 Version = CEFFECTUNITPROP_ADD_VEL;
	uint32 Version = CEFFECTUNITPROP_ADD_SLOWNDISAPPEAR;
	SaveStringT( File, "BEUP" ); //Base EffectUnitProp 基础数据开始标志
	File.write( (char*)&Version,			sizeof(int32) );

	SaveStringT( File, m_szAttachName.c_str() );
	SaveStringT( File, m_CustomName.c_str() );
	SaveStringT( File, m_EffectClassName.c_str() );

	Size = m_szAniName.size();
	File.write( (char*)&Size,					sizeof(int32) );
	for( ESet<EString>::iterator it = m_szAniName.begin(); it != m_szAniName.end(); it++ )
		SaveStringT( File, (*it).c_str() );

	File.write( (char*)&m_bSlownDisappear,	sizeof(bool)  );
	File.write( (char*)&m_isUseVel,				sizeof(bool)  );
	File.write( (char*)&m_EffectLod,			sizeof(float) );
	File.write( (char*)&m_TimeRange,			sizeof(int32) );
	File.write( (char*)&m_eUpdatePos,			sizeof(eUpdatePosType) );
	File.write( (char*)&m_TimeRange,			sizeof(int32) );
	File.write( (char*)&m_nTextureCircle,		sizeof(int32) );


	SaveMapValue( File, m_vecOffset );
	SaveMapValue( File, m_Color );
	SaveMapValue( File, m_Scale );

	Size = (int32)m_TextName.size();
	File.write( (char*)&Size, sizeof( int32 ) );
	for( EMap< float, EString >::const_iterator it = m_TextName.begin(); it != m_TextName.end(); it++ )
	{
		File.write( (char*)&it->first, sizeof( float ) );
		SaveStringT( File, it->second.c_str() );
	}

	return 0;
}

void	CEffectUnitProp::_FreeRes(void)
{
	for( EMap<float,ITexture*>::iterator it = m_Texture.begin(); it != m_Texture.end(); ++it )
		SafeRelease( it->second );
	m_Texture.clear();
}

void	CEffectUnitProp::_Reclaim(void)
{
	_FreeRes();
	for( EMap<float,EString>::const_iterator it = m_TextName.begin(); it != m_TextName.end(); ++it )
		CGraphic::GetInst()->CreateTexture( PATH_ALIAS_RES.c_str(),it->second.c_str(), &m_Texture[it->first] );
}

void	CEffectUnitProp::_CalculateAABB()
{
	if (!m_vecOffset.empty())
	{
		CVector3f vecMax;
		CVector3f vecMin;
		for( EMap< float, CVector3f >::iterator it = m_vecOffset.begin(); 
			it != m_vecOffset.end(); it++ )
		{
			vecMax.x = max( it->second.x, vecMax.x );
			vecMax.y = max( it->second.y, vecMax.y );
			vecMax.z = max( it->second.z, vecMax.z );
			vecMin.x = min( it->second.x, vecMin.x );
			vecMin.y = min( it->second.y, vecMin.y );
			vecMin.z = min( it->second.z, vecMin.z );
		}

		// 先算出基本的
		m_MaxAABB.setExtents(vecMin, vecMax);
	}
	else
		m_MaxAABB.setNull();
	
}

int32 CEffectUnitProp::LoadFullRes()
{
	if(CGraphic::HasInst())
	{
		EMap<float,ITexture*> Temp = m_Texture;
		m_Texture.clear();

		for( EMap<float,EString>::const_iterator it = m_TextName.begin(); it != m_TextName.end(); ++it )
			CGraphic::GetInst()->CreateTexture( PATH_ALIAS_RES.c_str(),it->second.c_str(), &m_Texture[it->first] );
		for( EMap<float,ITexture*>::iterator it = Temp.begin(); it != Temp.end(); ++it )
			SafeRelease( it->second );
	}
	_UpdateParams();
	return 0;
}

//------------------------------------------------------------------------------
void
CEffectUnitProp::GetCurAdvanceProp( float CurPos, CVector3f& Offset, ITexture*& pTexture, uint32& Color, float& Scale ) const
{
	if( m_vecOffset.size() )
		Offset = GetMapValue( m_vecOffset, CurPos );
	if( m_Texture.size() )
		pTexture = GetMapState( m_Texture, CurPos*m_nTextureCircle - (int32)( CurPos*m_nTextureCircle ) );
	if( m_Color.size() )
		Color = GetMapValue( m_Color, CurPos );
	if( m_Scale.size() )
		Scale = GetMapValue( m_Scale, CurPos )/255.0f;
}

//------------------------------------------------------------------------------
void
CEffectUnitProp::GetCurAdvanceProp( float CurPos, CVector3f& Offset, ITexture*& pTexture, uint32& Color, float& Scale, CVector3f& XYZcale ) const
{
	GetCurAdvanceProp(CurPos, Offset, pTexture, Color, Scale);
	if( m_XYZcale.size() )
		XYZcale = GetMapValue( m_XYZcale, CurPos );
}

//------------------------------------------------------------------------------
void
CEffectUnitProp::GetCurAdvanceProp( float CurPos, float& EmissionMinRadius, float& EmissionMaxRadius ) const
{
	if( m_EmissionMinRadius.size() )
		EmissionMinRadius = GetMapValue( m_EmissionMinRadius, CurPos );
	if( m_EmissionMaxRadius.size() )
		EmissionMaxRadius = GetMapValue( m_EmissionMaxRadius, CurPos );
}

//------------------------------------------------------------------------------
void
CEffectUnitProp::GetCurAdvanceProp( float CurPos, CVector3f& Offset, ITexture*& pTexture, uint32& Color, float& Scale, CVector3f& XYZcale, CVector3f& UVRCenter ) const
{
	GetCurAdvanceProp(CurPos, Offset, pTexture, Color, Scale, XYZcale);
	if( m_UVRCenter.size() )
		UVRCenter = GetMapValue( m_UVRCenter, CurPos );
}

int32 CEffectUnitProp::AddRef()
{
	return CRefObject::AddRef();
}

//////////////////////////////////////////////////////////////////////////

CEffectUnit::CEffectUnit( IEffectUnitProp* pEffectUnitProp ) 
	: m_pEffectUnitProp( (CEffectUnitProp*)pEffectUnitProp)
	, m_pUnitHandler(NULL)
	, m_pParent(NULL)
	, m_FrameTime(0.0f)
	, m_DeltaTime(0.0f)
	, m_bVisible(true)
	, m_nSkeletalID(-1)
	, m_nSocketID(0xFF)
	, m_bRefreshMat(true)
	, m_DistortTime(0)
{
	
	m_CurTime				= 0;
	m_StartTime				= 0;
	m_pEffectUnitProp->AddRef();

	m_OrgAABB = m_pEffectUnitProp->m_MaxAABB;
	if(m_pEffectUnitProp->m_pEffectProp)
		m_pEffectUnitProp->m_pEffectProp->AddRef();
	AddRef();
}

CEffectUnit::~CEffectUnit(void)
{
	if(m_pEffectUnitProp)
	{
		if(m_pEffectUnitProp->m_pEffectProp)
			m_pEffectUnitProp->m_pEffectProp->Release();
		m_pEffectUnitProp->Release();
		m_pEffectUnitProp = NULL;
	}

	SafeRelease( m_pUnitHandler );
}

bool CEffectUnit::SetProp( IEffectUnitProp* pProp )	
{ 
	if( pProp->GetGUID() != m_pEffectUnitProp->GetGUID() || pProp->GetID() != m_pEffectUnitProp->GetID() )
		return false;

	if( m_pEffectUnitProp == pProp )
		return true;

	SafeRelease(m_pEffectUnitProp);
	m_pEffectUnitProp = (CEffectUnitProp*)pProp;
	m_pEffectUnitProp->AddRef();	
	return true;
}

//void CEffectUnit::OnPreRender( const AniResult& ParentAniRes, float FrameRatio )
//{
//	m_ParenResult = ParentAniRes;
//	m_FrameRatio  = FrameRatio;
//	m_InvFrameRatio = 1.0f / m_FrameRatio;
//}

int16 CEffectUnit::GetBindSkeletalId(void)
{
	return m_nSkeletalID;
}

uint8 CEffectUnit::GetBindSocketId(void)
{
	return m_nSocketID;
}

void CEffectUnit::SetBindID( int16 SkeletalID, uint8 SocketID )
{
	m_nSkeletalID = SkeletalID;
	m_nSocketID   = SocketID;
}

void CEffectUnit::OnLinked( CRenderable* pParentObj )
{
	m_pParent = pParentObj;
	m_pLogicHandler = pParentObj->GetLogicHandler();
	m_nSkeletalID	= -1;
	m_nSocketID		= 0xFF;
}

void CEffectUnit::OnUnlinked( void )
{
	m_pParent		= NULL;
}

void CEffectUnit::SetRenderCallback( IEffectUnitHandler* pUnitHandler )
{ 
	SafeRelease( m_pUnitHandler );
	if (pUnitHandler)
	{
		m_pUnitHandler = pUnitHandler; 
		m_pUnitHandler->AddRef();
	}
};

//const CVector3f& CEffectUnit::GetBoxScale()
//{
//	return m_vecScale;
//}
//
//const CVector3f& CEffectUnit::GetBoxOff()
//{
//	return m_vecOff;
//}

void CEffectUnit::SetStartTime( uint32 STime,uint32 DTime  )		
{
	m_StartTime = STime;	
	m_CurTime	= STime; 
	m_DelayTime	= DTime;
	m_FrameTime = 0.0f; 
}

void CEffectUnit::ResetTime( uint32 STime,uint32 DTime  )
{
	SetStartTime(STime,DTime);
	//m_bRefreshMat = true;
}

void  CEffectUnit::SetDistortTime(uint32 dTime)
{
	m_DistortTime = int32(int64(m_CurTime) - int64(dTime));
}

const EFFECT_RESULT CEffectUnit::RenderEx( const CMatrix& matWorld,uint32 uCurTime,  RenderObjStyle* pRORS )
{
	uCurTime += m_DistortTime;
	m_DeltaTime = uCurTime>m_CurTime?(uCurTime - m_CurTime) :0.0f;
	m_CurTime = uCurTime;
	m_FrameTime += m_DeltaTime;
	return RenderEx( matWorld , pRORS );
}

const AniResult	CEffectUnit::Render(  const CMatrix& matWorld, RenderObjStyle* pRORS )
{
	AniResult ani;
	return ani;
}

void CEffectUnit::RenderChild(IEffectUnit* pEffectUnit, uint32 uCurTime, RenderObjStyle* pRORS)
{
	pEffectUnit->Render( m_matWorld, uCurTime, pRORS );
}

 bool CEffectUnit::UpdateTime(uint32 uCurTime)
 {
	 return false;
 }

const AniResult CEffectUnit::Render( const CMatrix& matWorld, uint32 uCurTime,  RenderObjStyle* pRORS )
{
 	/*if( (uCurTime == m_CurTime) && (m_StartTime != uCurTime) )
		return AniResult();*/

	if( !m_pEffectUnitProp->m_bTextValid )
	{
		for( EMap<float,ITexture*>::const_iterator it = m_pEffectUnitProp->m_Texture.begin(); 
			it != m_pEffectUnitProp->m_Texture.end(); it++ )
			if( it->second && !it->second->IsValid() )
				return AniResult();
		m_pEffectUnitProp->m_bTextValid = true;
	}

	m_DeltaTime = uCurTime>m_CurTime?(uCurTime - m_CurTime) :0.0f;
	m_CurTime = uCurTime;
	m_FrameTime += m_DeltaTime;

	CMatrix curMat = matWorld;
	if( m_pUnitHandler )
	{
		SQR_TRY
		{
			if( !m_pUnitHandler->RenderCallback( this, curMat, uCurTime, pRORS ) )
			{
				if( m_bRefreshMat /*&& IsInRenderTime() */)
				{
					m_bRefreshMat = false;
					m_matWorld = curMat;
				}
				return AniResult();
			}
		}
		SQR_CATCH(exp)
		{
			LogExp(exp);
		}
		SQR_TRY_END;
	}
		
	if( m_bRefreshMat /*&& IsInRenderTime() */)
	{
		m_bRefreshMat = false;
		m_matWorld = curMat;
	}

	return RenderEx( curMat , pRORS );
}

float CEffectUnit::GetUnitPercent(void)
{
	//return IsInRenderTime()?(m_FrameTime - m_DelayTime)/m_pEffectUnitProp->m_TimeRange:0.0f;
	return (m_FrameTime - m_DelayTime)/m_pEffectUnitProp->m_TimeRange;
}

bool CEffectUnit::IsInRenderTime(void)
{
	return m_FrameTime <= (m_pEffectUnitProp->m_TimeRange + m_DelayTime) && m_FrameTime >= m_DelayTime && m_bVisible;
}

bool CEffectUnit::IsEndTime()
{
	return m_FrameTime > (m_pEffectUnitProp->m_TimeRange + m_DelayTime);
}

void CEffectUnit::SetRenderVelocity( float Ratio , int32 id  )
{
	/*if(m_pEffectUnitProp->m_isUseVel)
		m_Velocity = Ratio;
	else
		m_Velocity = 1.0f;*/
}

const float sqr::CEffectUnit::GetAABBHeight()
{
	return m_OrgAABB.getMaximum().y;
}

float CEffectUnit::CalculateDifZBias( float zDis, const CMatrix& matWorld, const CMatrix& matCam
									, const float f, const float n )
{
	CVector4f pos;
	pos = pos * matWorld * matCam;
	if (pos.z == 0.0f)
		return 0;
	float pos2	= pos.z + (pos.z * pos.z)/pos.Dot(pos)*zDis;
	if (pos2 == 0.0f)
		return 0;
	float a		= -(f+n)/(f-n);
	float b		= -2*f*n/(f-n);
	return (((a*pos2 + b)/pos2 - 1.0f)-((a*pos.z + b)/pos.z - 1.0f))/2.0f;
}

const char* sqr::CEffectUnitProp::GetEffectFileName()
{
	if(m_pEffectProp)
		return m_pEffectProp->GetEffectFileName();
	return NULL;//CEffectClassEx::GetEffectName( (EBaseEffectUnit)m_ID );
}