#include "stdafx.h"
#include "CWater.h"
#include "CSceneClassDefine.h"
#include "BaseHelper.h"
#include "CGraphic.h"
#include "IEffect.h"

//---------------------------------
#include "TSqrAllocator.inl"
//---------------------------------

CWaterWave::CWaterWave( const CWaterWaveInfo* pWaveInfo )
{
	m_pWaveInfo = pWaveInfo;
	m_pWaves = new WAVE[m_pWaveInfo->WavePhysicInfo.nWavePerGrid];
}

CWaterWave::~CWaterWave()
{
	delete[] m_pWaves;
}

void CWaterWave::Init( float xDir, float zDir, const CVector3f& vecOff )
{
	float fLen  = sqrtf( xDir*xDir + zDir*zDir );
	m_xDir		= xDir/fLen;
	m_zDir		= zDir/fLen;
	m_vecOff	= vecOff;
	for( int i = 0; i < m_pWaveInfo->WavePhysicInfo.nWavePerGrid; i++ )
		m_pWaves[i].bInUsed = FALSE;
	m_RemainTime = 0;
}

int CWaterWave::Update( uint32 nCurTime, float DeltaTime )
{
	int n = 0;
	m_RemainTime += DeltaTime;
	for( int i = 0; i < m_pWaveInfo->WavePhysicInfo.nWavePerGrid; i++ )
	{
		if( m_pWaves[i].bInUsed )
		{
			if( nCurTime - m_pWaves[i].nStartTime > (uint32)m_pWaves[i].nLifeTime )
			{
				m_pWaves[i].bInUsed = FALSE;
				continue;
			}
			float zDelta = m_pWaves[i].fSpeed *DeltaTime;
			m_pWaves[i].verPos[0].p.z += zDelta*0.9f;
			m_pWaves[i].verPos[1].p.z += zDelta;
			m_pWaves[i].verPos[2].p.z =  m_pWaves[i].verPos[0].p.z;
			m_pWaves[i].verPos[3].p.z =  m_pWaves[i].verPos[1].p.z;
			m_pWaves[i].verPos[0].p.x -= zDelta*0.5f;
			m_pWaves[i].verPos[1].p.x -= zDelta*1.5f;
			m_pWaves[i].verPos[2].p.x += zDelta*0.5f;
			m_pWaves[i].verPos[3].p.x += zDelta*1.5f;
			float fPercent = float( nCurTime - m_pWaves[i].nStartTime )/m_pWaveInfo->WavePhysicInfo.nWaveLife;
			m_pWaves[i].verPos[0].diffuse = 
			m_pWaves[i].verPos[1].diffuse = 
			m_pWaves[i].verPos[2].diffuse = 
			m_pWaves[i].verPos[3].diffuse = ( ((DWORD)min( abs( 0.5f - abs( 0.5f - fPercent ) )*511, 255 ) ) << 24 ) | 0xffffff;
			n++;
		}
		else
		{
			if( m_RemainTime > m_pWaveInfo->WavePhysicInfo.nWaveLife/m_pWaveInfo->WavePhysicInfo.nWavePerGrid )
			{			
				m_RemainTime -= m_pWaveInfo->WavePhysicInfo.nWaveLife/m_pWaveInfo->WavePhysicInfo.nWavePerGrid;
				float fDeltaSize = m_pWaveInfo->WavePhysicInfo.nWaveSize/2.0f;
				float x1 = -fDeltaSize + rand()*fDeltaSize/32768.0f;
				float x2 =  fDeltaSize - rand()*fDeltaSize/32768.0f;
				float x  =  fDeltaSize/2 - rand()*fDeltaSize/32768.0f;

				m_pWaves[i].bInUsed = TRUE;
				m_pWaves[i].verPos[0].p = CVector3f( x + x1, 2, -fDeltaSize - m_pWaveInfo->WavePhysicInfo.nWaveOff );
				m_pWaves[i].verPos[1].p = CVector3f( x + x1, 2,	 fDeltaSize - m_pWaveInfo->WavePhysicInfo.nWaveOff );
				m_pWaves[i].verPos[2].p = CVector3f( x + x2, 2, -fDeltaSize - m_pWaveInfo->WavePhysicInfo.nWaveOff );
				m_pWaves[i].verPos[3].p = CVector3f( x + x2, 2,  fDeltaSize - m_pWaveInfo->WavePhysicInfo.nWaveOff );
				m_pWaves[i].verPos[0].diffuse = 0x00ffffff;
				m_pWaves[i].verPos[1].diffuse = 0x00ffffff;
				m_pWaves[i].verPos[2].diffuse = 0x00ffffff;
				m_pWaves[i].verPos[3].diffuse = 0x00ffffff;
				m_pWaves[i].nStartTime = nCurTime;
				m_pWaves[i].fSpeed = ( rand()*0.0005f/32768.0f + 0.0005f )*m_pWaveInfo->WavePhysicInfo.fWaveSpeed;
				m_pWaves[i].xDir = m_xDir;
				m_pWaves[i].zDir = m_zDir;
				m_pWaves[i].vecOff = m_vecOff;
				m_pWaves[i].nLifeTime = (uint32)( m_pWaveInfo->WavePhysicInfo.nWaveLife*( rand()*0.5f/32768.0f + 0.75f ) );
				n++;
			}
		}
	}
	return n;
}

void CWaterWave::Render( VerNCT*& pVB, uint16*& pIB, uint16& Base )
{
	CMatrix mat;
	mat.Identity();
	for( int i = 0; i < m_pWaveInfo->WavePhysicInfo.nWavePerGrid; i++ )
	{
		if( m_pWaves[i].bInUsed )
		{
			mat._11 =  m_pWaves[i].zDir;
			mat._13 = -m_pWaves[i].xDir;
			mat._31 = -mat._13;
			mat._33 =  mat._11;
			mat._41 =  m_pWaves[i].vecOff.x;
			mat._42 =  m_pWaves[i].vecOff.y;
			mat._43 =  m_pWaves[i].vecOff.z;

			pVB[0].p = VEC_FASTMULT( m_pWaves[i].verPos[0].p, mat );
			pVB[1].p = VEC_FASTMULT( m_pWaves[i].verPos[1].p, mat );
			pVB[2].p = VEC_FASTMULT( m_pWaves[i].verPos[2].p, mat );
			pVB[3].p = VEC_FASTMULT( m_pWaves[i].verPos[3].p, mat );
			pVB[0].Set( pVB[0].p.x, pVB[0].p.y, pVB[0].p.z, 0, 1, 0, m_pWaves[i].verPos[0].diffuse, 0, 0 );
			pVB[1].Set( pVB[1].p.x, pVB[1].p.y, pVB[1].p.z, 0, 1, 0, m_pWaves[i].verPos[1].diffuse, 1, 0 );
			pVB[2].Set( pVB[2].p.x, pVB[2].p.y, pVB[2].p.z, 0, 1, 0, m_pWaves[i].verPos[2].diffuse, 0, 1 );
			pVB[3].Set( pVB[3].p.x, pVB[3].p.y, pVB[3].p.z, 0, 1, 0, m_pWaves[i].verPos[3].diffuse, 1, 1 );

			pIB[0] = Base + 0;
			pIB[1] = Base + 1;
			pIB[2] = Base + 2;
			pIB[3] = Base + 1;
			pIB[4] = Base + 3;
			pIB[5] = Base + 2;
			pVB  += 4;
			pIB  += 6;
			Base += 4;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

CDimple::CDimple( CWater* pWater, IEffect* pDimpleFx, const CMatrix& matWorld )
	: m_pWater(pWater)
	, m_matWorld( matWorld )
	, m_pEffectObj( pDimpleFx )
	, m_uStartTime( (uint32)GetProcessTime() )
{
	m_pEffectObj->AddRef();
	m_pWater->AddDimple( this );
}

void CDimple::Render( uint32 uPreTime )
{
	m_pEffectObj->UpdateTime(uPreTime);
	m_pEffectObj->Render( m_matWorld );
}

CDimple::~CDimple()
{
	SafeRelease( m_pEffectObj );
}

//////////////////////////////////////////////////////////////////////////

CWater::CWater(uint32 orgID)
{
	memset( m_pWaterWave, WATER_NULL, sizeof(m_pWaterWave) );
	m_MinWater = 255;
	m_MaxWater = 0;
	m_MinWave = 255;
	m_MaxWave = 0;
	m_OrgID		= orgID;
}

CWater::~CWater(void)
{
	for( int i = 0; i < eGridNumPerRegion; i++ )
		if( m_pWaterWave[i] > WATER_CENTER )
			delete m_pWaterWave[i];


	DimplePool::iterator eit = m_pPoolDimples.end();
	for( DimplePool::iterator it = m_pPoolDimples.begin(); it!=eit; ++it)
		delete *it;
}

void CWater::AddDimple( CDimple* pObj )
{
	//CDimple** ppNewDimples = new CDimple*[ m_nDimplesNum + 1 ];
	//memcpy( ppNewDimples, m_ppDimples, sizeof(CDimple*)*m_nDimplesNum );
	//ppNewDimples[m_nDimplesNum++] = pObj;
	//delete [] m_ppDimples;
	//m_ppDimples = ppNewDimples;
	m_pPoolDimples.push_back(pObj);
}

void CWater::SetGridValid( uint8 nGrid )
{
	m_pWaterWave[nGrid] = WATER_CENTER;
	m_MinWater = min( m_MinWater, nGrid );
	m_MaxWater = max( m_MaxWater, nGrid );
	m_WaterGrid.push_back(nGrid);
}

void CWater::SetGridDir( const CWaterWaveInfo* pWaveInfo, 
						uint8 nGrid, float xDir, float yDir, const CVector3f& vecGridOffset )
{
	m_pWaterWave[nGrid] = new CWaterWave(pWaveInfo);
	m_pWaterWave[nGrid]->Init( xDir, yDir, vecGridOffset );

	m_MinWave = min( m_MinWave, nGrid );
	m_MaxWave = max( m_MaxWave, nGrid );
	m_WaterGrid.push_back(nGrid);
}

//------------------------------------------------------------------------------
uint32 CWater::UpdateWave( uint32 nCurTime )
{
	int WaveNum = 0;
	for( uint32 i = m_MinWave; i <= (uint32)m_MaxWave; i++ )
		if( m_pWaterWave[i] > WATER_CENTER )
			WaveNum += m_pWaterWave[i]->Update( nCurTime, (float)( nCurTime - m_nPreTime ) );
	m_nPreTime = nCurTime;
	return WaveNum;
}

//------------------------------------------------------------------------------
void CWater::RenderWave( ITexture* texture, uint32 nCurTime )
{
	VerNCT* pVB;
	uint16* pIB;
	RenderState* pRS;

	if( texture != 0 && texture->IsValid() )
	{
		int nWaveNum = UpdateWave( nCurTime );
		if( nWaveNum > 0 )
		{
			CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, ROS_LIGHTMAT2, texture, NULL, PT_TRIANGLELIST, nWaveNum*4, nWaveNum*2, 
				VerNCT::Format, sizeof(VerNCT), (void**)&pVB, (void**)&pIB, (void**)&pRS )->Identity();
			pRS->m_TexOP_S0			= 7;
			pRS->m_AlpOP_S0			= 4;
			pRS->m_ZTestEnable 		= TRUE;
			pRS->m_ZWrite 			= FALSE;
			uint16 Base = 0;
			// render
			for( uint32 i = m_MinWave; i <= (uint32)m_MaxWave; i++ )
			{
				if( m_pWaterWave[i] > WATER_CENTER )
					m_pWaterWave[i]->Render( pVB, pIB, Base );
			}
		}
	}
}

//------------------------------------------------------------------------------
void CWater::RenderDimple()
{
	for( DimplePool::iterator it = m_pPoolDimples.begin(); it!=m_pPoolDimples.end(); )
	{
		(*it)->Render( m_nPreTime );

		if( int32( GetProcessTime() -(*it)->m_uStartTime ) > (*it)->m_pEffectObj->GetTimeRange() )
		{
			GfkSafeDelete(*it);
			it = m_pPoolDimples.erase(it);
		}
		else
		{
			++it;
		}
	}
}