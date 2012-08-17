#include "stdafx.h"
#include "CCameraFilter.h"
#include "CGraphic.h"
#include "CBaseModel.h"
#include "CCamera.h"
#include "TSqrAllocator.inl"

DYNAMIC_CREATE_EFFECTUNITBASE( eCameraFilter, IDS_CAMERAFILTER, CCameraFilter, CCameraFilterProp )
enum
{
	CAMERAFILTER_ORIGIN,
	CAMERAFILTER_ADD_POSTMODEL,
	CAMERAFILTER_ADD_FADEIN,
	CAMERAFILTER_ADD_RATIO,
};	

class CModelPostFilter : public CPostFilter
{
public:
	CModelPostFilter(void):m_pModel(NULL){}

	void RenderFilter(void)
	{
		m_pModel->RenderImme();
	};
	
	CBaseModel* m_pModel;
};

CCameraFilterProp::CCameraFilterProp( uint32 ID, IEffectClassDesc* pEffectClassDesc )
	: CEffectUnitProp( ID, pEffectClassDesc  )
{
	m_bFilterParent		= false;
	m_eBlendModel		= SPRB_ADD;
	m_eZTest			= Z_TESTONLY;
	m_xScale			= 100.0f;
	m_yScale			= 100.0f;
	m_zScale			= 100.0f;
	m_eCFilterTp		= CAMERAFILTER_RATIO_NONE;
	m_BirthTime			= 0;
	m_DeleteTime		= 1000;
}

CCameraFilterProp::~CCameraFilterProp(void)
{
}

int32 CCameraFilterProp::LoadBin( IEffectRead& File )
{
	uint32 Version;
	
	File.read( (char*)&Version,					sizeof(uint32) );

	switch(Version)
	{
	case CAMERAFILTER_ADD_RATIO:
		File.read( (char*)&m_eCFilterTp,			sizeof(CAMERAFILTER_RATIO) );
		LoadMapValue( File, m_XYZcale );
	case CAMERAFILTER_ADD_FADEIN:
		File.read( (char*)&m_BirthTime,				sizeof(int) );
		File.read( (char*)&m_DeleteTime,			sizeof(int) );
	case CAMERAFILTER_ADD_POSTMODEL:
		File.read( (char*)&m_bFilterParent,			sizeof(bool) );
	default:
		File.read( (char*)&m_eBlendModel,			sizeof(SPR_BLENDMODE) );
		File.read( (char*)&m_eZTest,				sizeof(Z_TEST) );
		File.read( (char*)&m_xScale,				sizeof(float) );
		File.read( (char*)&m_yScale,				sizeof(float) );
		File.read( (char*)&m_zScale,				sizeof(float) );
	}
	if( FAILED( CEffectUnitProp::LoadBin( File ) ) )
		return GERROR;
	return 0;
}

int32 CCameraFilterProp::SaveBin( IEffectWrite& File )
{
	uint32 Version = CAMERAFILTER_ADD_RATIO;
	File.write( (char*)&Version,				sizeof(uint32) );
	File.write( (char*)&m_eCFilterTp,			sizeof(CAMERAFILTER_RATIO) );
	SaveMapValue( File, m_XYZcale );
	File.write( (char*)&m_BirthTime,			sizeof(int) );
	File.write( (char*)&m_DeleteTime,			sizeof(int) );
	File.write( (char*)&m_bFilterParent,		sizeof(bool) );
	File.write( (char*)&m_eBlendModel,			sizeof(SPR_BLENDMODE) );
	File.write( (char*)&m_eZTest,				sizeof(Z_TEST) );
	File.write( (char*)&m_xScale,				sizeof(float) );
	File.write( (char*)&m_yScale,				sizeof(float) );
	File.write( (char*)&m_zScale,				sizeof(float) );
	if( FAILED( CEffectUnitProp::SaveBin( File ) ) )
		return GERROR;
	return 0;
}

//////////////////////////////////////////////////////////////////////////

CCameraFilter::CCameraFilter( IEffectUnitProp* pEffectUnitProp )
			  : CEffectUnit( pEffectUnitProp )
{
	const CCameraFilterProp* pProp = (CCameraFilterProp*)m_pEffectUnitProp;
	//m_vecScale += CVector3f( pProp->m_xScale*2, pProp->m_yScale*2, pProp->m_zScale*2 );
	m_pPostFilter = new CModelPostFilter;
	m_DelayRender	= (float)((CCameraFilterProp*)m_pEffectUnitProp)->m_DeleteTime;
	m_BirthRender	= 0.0f;
	m_bFinishFirstPlay = false;
}

CCameraFilter::~CCameraFilter(void)
{
	GfkSafeDelete(m_pPostFilter);
}

const EFFECT_RESULT CCameraFilter::RenderEx( const CMatrix& matWorld, RenderObjStyle* pRORS )
{
	const CCameraFilterProp* pProp = (CCameraFilterProp*)m_pEffectUnitProp;

	EFFECT_RESULT Result( NULL, true, false );
	if( !IsInRenderTime() )
		return Result;
	
	float Percent = GetUnitPercent();
	CCamera* pCamera = NULL;
	if ( CGraphic::GetInst()->GetActiveRenderTarget() )
		pCamera = CGraphic::GetInst()->GetActiveRenderTarget()->GetCamera();
	else	
		pCamera = CMainWindowTarget::GetInst()->GetCamera();

	if ( !pCamera )
		return Result;

	CVector3f	Offset( 0, 0, 0 );
	CVector3f	OrgOffset( 0, 0, 0 );
	CVector3f	XYZcale(0, 0, 0);
	CFRect		rt(0.0f, 0.0f, 1.0f, 1.0f);
	bool		bRatio		= false;
	_TEXTUREADDRESS eTexAddress = TADDRESS_WRAP;

	ITexture*	pCurText	= NULL;
	uint32		Color		=0;
	float		fScale		= 1.0f;

	pProp->GetCurAdvanceProp( Percent, OrgOffset, pCurText, Color, fScale);

	if (pProp->m_eCFilterTp == CAMERAFILTER_RATIO_CLAMP)
	{
		bRatio = true;
		eTexAddress = TADDRESS_CLAMP;
		// ¼ÆËãRT
		CVector3f XYZcale = GetMapValue( pProp->m_XYZcale, Percent );
		if ( XYZcale.y != 0.0f && XYZcale.x != 0.0f)
		{
			float r = XYZcale.x / XYZcale.y;
			CIRect wrt;	float wr = 1.0f;
			CGraphic::GetInst()->GetActiveRenderTarget()->GetRect( wrt );
			wr = (float)(wrt.Width())/wrt.Height();
			if (wr > r)
			{
				rt.left		= (r - wr)/(2.0f*r); 
				rt.right	= 1 + (wr - r)/(2.0f*r);
			}
			else
			{
				wr = 1.0f/wr;	r = 1.0f/r;
				rt.top = (r - wr)/(2.0f*r);
				rt.bottom = 1 + (wr - r)/(2.0f*r);
			}
		}
	}
	
	if(m_BirthRender>=pProp->m_BirthTime)
	{
		m_bFinishFirstPlay = true;
		Color = Color & 0x00ffffff |(((uint32)(Color * (m_DelayRender / pProp->m_DeleteTime))) & 0xff000000);
	}
	else if (!m_bFinishFirstPlay)
	{
		Color = Color & 0x00ffffff | (((uint32)(Color * (m_BirthRender / pProp->m_BirthTime ))) & 0xff000000);
		m_BirthRender += m_DeltaTime;
	}
	
	if( pProp->m_vecOffset.size() )
		Offset = GetMapValue( pProp->m_vecOffset, Percent );
	Offset.FastMultiply( matWorld );
	m_matWorld = matWorld;
	m_matWorld._41 = Offset.x;
	m_matWorld._42 = Offset.y;
	m_matWorld._43 = Offset.z;

	CVector3f CameraPos, CameraDest;
	CameraPos = pCamera->GetPosition();
	if( fabs( CameraPos.x - m_matWorld._41 ) < pProp->m_xScale &&
		fabs( CameraPos.y - m_matWorld._42 ) < pProp->m_yScale &&
		fabs( CameraPos.z - m_matWorld._43 ) < pProp->m_zScale )
	{
		float Percent = GetUnitPercent();
		CGraphic::GetInst()->GetActiveRenderTarget()->AddFilter( GetMapState( pProp->m_Texture, Percent ), 
			Color, 
			pProp->m_eZTest != Z_NONE, 
			pProp->m_eBlendModel == SPRB_MULTIPLY,
			bRatio,
			eTexAddress,
			&rt );

		if(pProp->m_bFilterParent && m_pParent != NULL && m_pParent->GetType() == ROT_MODEL)
		{
			((CModelPostFilter*)m_pPostFilter)->m_pModel = (CBaseModel*)m_pParent;
			CGraphic::GetInst()->GetActiveRenderTarget()->AddPostFilter(m_pPostFilter);
		}
	}

	return Result;
}

const EFFECT_RESULT	CCameraFilter::RenderEx( const CMatrix& matWorld,uint32 uCurTime,  RenderObjStyle* pRORS)
{
	uCurTime += m_DistortTime;
	m_DeltaTime = uCurTime>m_CurTime?(uCurTime - m_CurTime):0.0f;
	m_CurTime = uCurTime;
	m_FrameTime += m_DeltaTime;

	if(!IsInRenderTime())
		m_FrameTime = float(m_pEffectUnitProp->m_TimeRange + m_DelayTime);
	RenderEx( matWorld , pRORS );

	m_DelayRender -= m_DeltaTime;
	return EFFECT_RESULT( NULL, true, false , m_DelayRender <= 0.0f || m_DelayRender > (float)((CCameraFilterProp*)m_pEffectUnitProp)->m_DeleteTime );
}

void CCameraFilter::Free()
{
	if( IsInRenderTime() && m_FrameTime!= 0 && m_pEffectUnitProp->m_bSlownDisappear )
	{
		m_pEffectUnitProp->m_pEffectClassDesc->AddLeavingsEffectUnit( this );
	}
	else
		delete this;
}
