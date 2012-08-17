#include "stdafx.h"
#include "CModelConsole.h"
#include "BaseHelper.h"
#include "CRenderTarget.h"
#include "CRenderPipeline.h"
#include "CSkeletalsMgr.h"
#include "CGraphic.h"
#include "TSqrAllocator.inl"
#include "CCamera.h"
#include "CColor.inl"

DYNAMIC_CREATE_EFFECTUNITBASE( eModelConsole, IDS_MODELCONSOLE, CModelConsole, CModelConsoleProp )

enum
{
	MODELCONSOLE_ORIGIN,
	MODELCONSOLE_ADD_TEXTURE_COORDEDIT,	
	MODELCONSOLE_ADD_SCALE,				// 增加了xyz缩放的功能
	MODELCONSOLE_ADD_LINK_SKELETAL,		// 增加了骨骼动态改变的需求
	MODELCONSOLE_ADD_FACETYPE,			// 增加了模型特效的朝向
	MODELCONSOLE_ADD_XYZCALE,			// 增加了模型特效在一个周期中XYZ轴上放大缩小的分量
	MODELCONSOLE_ADD_CLIP_PLANE,		// 增加裁剪面
	MODELCONSOLE_ADD_REFRACT_INDEX,		// 增加折射率
	MODELCONSOLE_CHANGE_REFRACT,		// 折射率改为map
	MODELCONSOLE_ENABLE_SHADOW,			// 增加是否有阴影的标志
	MODELCONSOLE_CHANGE_CLIP_DIST,		// 裁剪面改为可以变化的
	MODELCONSOLE_ADD_BIRTH_DELETE,		// 增加延时生成和释放
	MODELCONSOLE_ADD_SKENAME,			// 增加了模型特效的骨架名索引
};	

CModelConsoleProp::CModelConsoleProp( uint32 ID, IEffectClassDesc* pEffectClassDesc )
	: CEffectUnitProp( ID, pEffectClassDesc )
{
	m_vecRotate			= CVector3f( 0, 0, 0 );
	m_SkeName			= "";
	m_szARPName			= "";
	m_nPiece0			= 0;
	m_nRS0				= 0;
	m_nPiece1			= 0;
	m_nRS1				= 0;
	m_nPiece2			= 0;
	m_nRS2				= 0;
	m_nPiece3			= 0;
	m_nRS3				= 0;
	m_nPiece4			= 0;
	m_nRS4				= 0;
	m_nPiece5			= 0;
	m_nRS5				= 0;
	m_szARAName			= "";
	m_Animation			= 0;
	m_bSynch			= 0;

	//  初始化大小
	m_fMinXSize			= 0;
	m_fMinYSize			= 0;
	m_fMinZSize			= 0;

	m_fMaxXSize			= 10;
	m_fMaxYSize			= 10;
	m_fMaxZSize			= 10;

	m_eFaceType			= SPRF_NORMAL;
	m_fStartAngle		= 0;

	m_vecTextMoveVel	= CVector2f ( 0, 0 );
	m_fTextAngularVel	= 0;
	m_fTextMaxScale		= 1.0f;
	m_fTextMinScale		= 1.0f;

	m_pPieceGroup		= NULL;
	m_pAnimationGroup	= NULL;
	m_pSkeletalFrame	= NULL;
	m_Mark				= 0;

	m_vecOffset[0.0f] = CVector3f( 0, 0, 0 );
	m_Scale[ 0.0f] = (BYTE)( ((uint32)0x00000000)>>24 );
	m_XYZcale[0.0f] = CVector3f( 10.0f, 10.0f, 10.0f);
	m_UVRCenter[0.0f] = CVector3f( 0.5f,  0.0f,  0.5f);

	m_szParentSk		= "";
	m_szSk				= "";
	m_nSkIdx			= -1;
	m_bEnableShadow		= false;
	m_BirthTime			= 0;
	m_DeleteTime		= 1000;
	LoadFullRes();
}

CModelConsoleProp::~CModelConsoleProp(void)
{
	SafeRelease( m_pPieceGroup );
	//SafeRelease( m_pSkeletalFrame );
	SafeRelease( m_pAnimationGroup );
}

int32 CModelConsoleProp::LoadBin( IEffectRead& File )
{
	uint32 Version = 0;
	File.read( (char*)&Version,					sizeof(uint32) );
	bool bLoadFailed = false;

	switch( Version )
	{
	case MODELCONSOLE_ADD_SKENAME:
		LoadStringT( File, m_SkeName );
	case MODELCONSOLE_ADD_BIRTH_DELETE:
		File.read( (char*)&m_BirthTime,				sizeof(int) );
		File.read( (char*)&m_DeleteTime,			sizeof(int) );
	case MODELCONSOLE_CHANGE_CLIP_DIST:
		LoadMapValue(File, m_ClipDistMap);
	case MODELCONSOLE_ENABLE_SHADOW:
		File.read( (char*)&m_bEnableShadow,		sizeof(m_bEnableShadow) );
		if (m_ClipPlane.originPlane.IsValid() && m_ClipDistMap.empty())
		{
			m_ClipDistMap[0.0f] = -m_ClipPlane.originPlane.m_Dist;
			m_ClipDistMap[1.0f] = -m_ClipPlane.originPlane.m_Dist;
		}
	case MODELCONSOLE_CHANGE_REFRACT:
	case MODELCONSOLE_ADD_REFRACT_INDEX:
		LoadMapValue(File, m_mapRefract);
	case MODELCONSOLE_ADD_CLIP_PLANE:
		File.read(&m_ClipPlane.originPlane, sizeof(m_ClipPlane.originPlane));
	case MODELCONSOLE_ADD_XYZCALE			:
		LoadMapValue( File, m_XYZcale );
	case MODELCONSOLE_ADD_FACETYPE		   :
		File.read( (char*)&m_eFaceType,			sizeof(SPR_FACE) );
	case MODELCONSOLE_ADD_LINK_SKELETAL:
		LoadStringT( File, m_szParentSk );
		File.read( (char*)&m_nSkIdx,			sizeof(int32) );
	case MODELCONSOLE_ADD_SCALE:
		File.read( (char*)&m_fMinYSize,			sizeof(float) );
		File.read( (char*)&m_fMaxYSize,			sizeof(float) );
		File.read( (char*)&m_fMinZSize,			sizeof(float) );
		File.read( (char*)&m_fMaxZSize,			sizeof(float) );
	case MODELCONSOLE_ADD_TEXTURE_COORDEDIT:
		File.read( (char*)&m_vecTextMoveVel,		sizeof(CVector2f) );
		File.read( (char*)&m_fTextAngularVel,		sizeof(float) );
		File.read( (char*)&m_fTextMaxScale,		sizeof(float) );
		File.read( (char*)&m_fTextMinScale,		sizeof(float) );
	default:
		LoadStringT( File, m_szARPName );
		LoadStringT( File, m_szARAName );

		if(!m_szARPName.empty())
		{
			CPieceGroup*		pPieceGroup = NULL;
			//CAnimationsRes*	pAnimationGroup = NULL;
			CSkeletalFrame*		pSkeletalFrame = NULL;
			CAnimationGroup*		pAnimationGroup = NULL;
			m_Mark = 1;
			//CPkgFile::EnableAsyncRead( false );

			if( FAILED( CGraphic::HasInst() && CGraphic::GetInst()->CreatePieceGroup( m_szARPName.c_str(), &pPieceGroup ) ) )
			{
				m_nPiece0 = m_nPiece1 = m_nPiece2 = m_nPiece3 = m_nPiece4 = m_nPiece5 = 0;
				bLoadFailed = true;
			}

			if(m_szARAName.find(".ske")!=string::npos)
				m_szARAName = m_szARAName.substr(0,m_szARAName.size()-4)+".agp";

			if( !m_szARAName.empty())
			{
				if(FAILED(CGraphic::HasInst() && CGraphic::GetInst()->CreateAnimationGroup( m_szARAName.c_str(), &pAnimationGroup )) )
					bLoadFailed = true;

				if(pAnimationGroup && pAnimationGroup->IsValid() && pAnimationGroup->GetSkeletalFrameName()!="")
					m_SkeName = pAnimationGroup->GetSkeletalFrameName();

				EString sSkeFile = m_szARAName;
				sSkeFile = sSkeFile.substr(0,sSkeFile.size()-4)+".ske";

				if(m_SkeName!="")
					sSkeFile = m_SkeName;

				if(FAILED(CSkeletalsMgr::GetInst()->GetSkeInPool( sSkeFile.c_str(), &pSkeletalFrame )))
					bLoadFailed = true;

				m_Mark = 2;
			}
			SafeRelease( m_pPieceGroup );
			m_pPieceGroup = pPieceGroup;

			//SafeRelease( m_pSkeletalFrame );
			m_pSkeletalFrame = pSkeletalFrame;

			SafeRelease( m_pAnimationGroup );
			m_pAnimationGroup = pAnimationGroup;
			
		}

		File.read( (char*)&m_vecRotate,			sizeof(CVector3f) );
		File.read( (char*)&m_nPiece0,			sizeof(int32) );
		File.read( (char*)&m_nRS0,				sizeof(int32) );
		File.read( (char*)&m_nPiece1,			sizeof(int32) );
		File.read( (char*)&m_nRS1,				sizeof(int32) );
		File.read( (char*)&m_nPiece2,			sizeof(int32) );
		File.read( (char*)&m_nRS2,				sizeof(int32) );
		File.read( (char*)&m_nPiece3,			sizeof(int32) );
		File.read( (char*)&m_nRS3,				sizeof(int32) );
		File.read( (char*)&m_nPiece4,			sizeof(int32) );
		File.read( (char*)&m_nRS4,				sizeof(int32) );
		File.read( (char*)&m_nPiece5,			sizeof(int32) );
		File.read( (char*)&m_nRS5,				sizeof(int32) );
		File.read( (char*)&m_Animation,			sizeof(int32) );
		File.read( (char*)&m_bSynch,			sizeof(BOOL) ); //=== bool
		File.read( (char*)&m_fMinXSize,			sizeof(float) );
		File.read( (char*)&m_fMaxXSize,			sizeof(float) );

		

		if( Version < MODELCONSOLE_ADD_SCALE )
		{
            m_fMinYSize = m_fMinXSize;
			m_fMinZSize = m_fMinXSize;
			m_fMaxYSize = m_fMaxXSize;
			m_fMaxZSize = m_fMaxXSize;
		}
		if(Version >=MODELCONSOLE_ADD_SCALE)
		{
			m_XYZcale[0.0f].x = m_fMaxXSize;
			m_XYZcale[0.0f].y = m_fMaxYSize;
			m_XYZcale[0.0f].z = m_fMaxZSize;
		}
	}

	if( FAILED( CEffectUnitProp::LoadBin( File ) ) )
		return GERROR;

	if( bLoadFailed )
		return GERROR;

	return 0;
}

bool	CModelConsoleProp::	CaculateProp()
{
	bool bLoadFailed = false;
	if(!m_szARPName.empty())
	{
		CPieceGroup*		pPieceGroup = NULL;
		//CAnimationsRes*	pAnimationGroup = NULL;
		CSkeletalFrame*		pSkeletalFrame = NULL;
		CAnimationGroup*		pAnimationGroup = NULL;
		m_Mark = 1;
		//CPkgFile::EnableAsyncRead( false );

		if( FAILED( CGraphic::GetInst()->CreatePieceGroup( m_szARPName.c_str(), &pPieceGroup ) ) )
		{
			m_nPiece0 = m_nPiece1 = m_nPiece2 = m_nPiece3 = m_nPiece4 = m_nPiece5 = 0;
			bLoadFailed = true;
		}

		if(m_szARAName.find(".ske")!=string::npos)
			m_szARAName = m_szARAName.substr(0,m_szARAName.size()-4)+".agp";

		if( !m_szARAName.empty())
		{
			if(FAILED(CGraphic::GetInst()->CreateAnimationGroup( m_szARAName.c_str(), &pAnimationGroup )) )
				bLoadFailed = true;

			if(pAnimationGroup&& pAnimationGroup->IsValid() && pAnimationGroup->GetSkeletalFrameName()!="")
				m_SkeName = pAnimationGroup->GetSkeletalFrameName();

			EString sSkeFile = m_szARAName;
			sSkeFile = sSkeFile.substr(0,sSkeFile.size()-4)+".ske";

			if(m_SkeName!="")
				sSkeFile = m_SkeName;

			if(FAILED(CSkeletalsMgr::GetInst()->GetSkeInPool( sSkeFile.c_str(), &pSkeletalFrame )))
				bLoadFailed = true;

			m_Mark = 2;
		}
		SafeRelease( m_pPieceGroup );
		m_pPieceGroup = pPieceGroup;

		//SafeRelease( m_pSkeletalFrame );
		m_pSkeletalFrame = pSkeletalFrame;

		SafeRelease( m_pAnimationGroup );
		m_pAnimationGroup = pAnimationGroup;

	}
	m_XYZcale[0.0f].x = m_fMaxXSize;
	m_XYZcale[0.0f].y = m_fMaxYSize;
	m_XYZcale[0.0f].z = m_fMaxZSize;
	return bLoadFailed;
}

void CModelConsoleProp::_UpdateParams(void)
{
	if( m_pSkeletalFrame )
	{
		if( m_nSkIdx > 0 && m_nSkIdx <= m_pSkeletalFrame->GetSkeletalTotalNum() )
			m_szSk = m_pSkeletalFrame->GetSkeletalName(m_nSkIdx-1);
	}


	m_AngularVelocity = m_vecRotate.Mag();
	m_bSetTextureMatrix = m_vecTextMoveVel.x != 0.0f ||
		m_vecTextMoveVel.y != 0.0f ||
		m_fTextAngularVel != 0.0f ||
		m_fTextMaxScale != 1.0f ||
		m_fTextMinScale != 1.0f ;

	if(m_szARPName.empty())
		return;

	CPieceGroup*		pPieceGroup = NULL;
	//CAnimationsRes*	pAnimationGroup;
	CSkeletalFrame*		pSkeletalFrame = NULL;
	CAnimationGroup*	pAnimationGroup = NULL;

	if( FAILED( CGraphic::GetInst()->CreatePieceGroup( m_szARPName.c_str(), &pPieceGroup ) ) )
		m_nPiece0 = m_nPiece1 = m_nPiece2 = m_nPiece3 = m_nPiece4 = m_nPiece5 = 0;
	//CGraphic::GetInst()->CreateAnimationsRes( m_szARAName.c_str(), &pAnimationGroup );

	//DebugOut("m_szARAName:%s",m_szARAName.c_str());
	if(m_szARAName!="")
	{
		EString sAgpFile = m_szARAName;
		sAgpFile = sAgpFile.substr(0,sAgpFile.size()-4)+".agp";
		CGraphic::GetInst()->CreateAnimationGroup( sAgpFile.c_str(), &pAnimationGroup );

		if(pAnimationGroup && pAnimationGroup->IsValid() && pAnimationGroup->GetSkeletalFrameName()!="")
			m_SkeName = pAnimationGroup->GetSkeletalFrameName();

		EString sSkeFile = m_szARAName;
		sSkeFile = sSkeFile.substr(0,sSkeFile.size()-4)+".ske";

		if(m_SkeName!="")
			sSkeFile = m_SkeName;

		CSkeletalsMgr::GetInst()->GetSkeInPool( sSkeFile.c_str(), &pSkeletalFrame );
	}


	SafeRelease( m_pPieceGroup );
	m_pPieceGroup = pPieceGroup;

	//SafeRelease( m_pSkeletalFrame );
	m_pSkeletalFrame = pSkeletalFrame;

	SafeRelease( m_pAnimationGroup );
	m_pAnimationGroup = pAnimationGroup;
}

bool CModelConsoleProp::IsValid()
{
	switch(m_Mark)
	{
	case 1:
		return m_pPieceGroup==NULL||m_pPieceGroup->IsValid();
		break;
	case 2:
		return m_pPieceGroup==NULL||m_pSkeletalFrame==NULL||m_pAnimationGroup==NULL||(m_pPieceGroup->IsValid()&&m_pSkeletalFrame->IsValid()&&m_pAnimationGroup->IsValid());
		break;
	default:
		return true;
	}
	return true;
}

int32 CModelConsoleProp::SaveBin( IEffectWrite& File )
{
    uint32 Version = MODELCONSOLE_ADD_SKENAME;
	File.write( (char*)&Version,				sizeof(uint32) );

	SaveStringT( File, m_SkeName.c_str() );

	File.write( (char*)&m_BirthTime,			sizeof(int) );
	File.write( (char*)&m_DeleteTime,			sizeof(int) );

	SaveMapValue(File, m_ClipDistMap);
	File.write( (char*)&m_bEnableShadow, sizeof(m_bEnableShadow) );
	SaveMapValue(File, m_mapRefract);
	File.write(&m_ClipPlane.originPlane, sizeof(m_ClipPlane.originPlane));
	SaveMapValue( File, m_XYZcale );
	m_fMaxXSize = m_XYZcale[0.0f].x;
	m_fMaxYSize = m_XYZcale[0.0f].y;
	m_fMaxZSize = m_XYZcale[0.0f].z;
	File.write( (char*)&m_eFaceType,			sizeof(SPR_FACE) );

	SaveStringT( File, m_szParentSk.c_str() );
	File.write( (char*)&m_nSkIdx,				sizeof(int32) );

	File.write( (char*)&m_fMinYSize,			sizeof(float) );
	File.write( (char*)&m_fMaxYSize,			sizeof(float) );
	File.write( (char*)&m_fMinZSize,			sizeof(float) );
	File.write( (char*)&m_fMaxZSize,			sizeof(float) );

	File.write( (char*)&m_vecTextMoveVel,		sizeof(CVector2f) );
	File.write( (char*)&m_fTextAngularVel,		sizeof(float) );
	File.write( (char*)&m_fTextMaxScale,		sizeof(float) );
	File.write( (char*)&m_fTextMinScale,		sizeof(float) );

	if(!m_szARPName.empty())
	{
		size_t npos = m_szARPName.rfind(".arp");
		if(npos != -1)
		{
			m_szARPName = m_szARPName.substr(0, npos);
			m_szARPName += ".mod";
		}
	}
	SaveStringT( File, m_szARPName.c_str() );

	if(!m_szARAName.empty())
	{
		size_t npos = m_szARAName.rfind(".ara");
		if(npos != -1)
		{
			m_szARAName = m_szARAName.substr(0, npos);
			m_szARAName += ".ske";
		}
	}
	SaveStringT( File, m_szARAName.c_str() );

	File.write( (char*)&m_vecRotate,			sizeof(CVector3f) );
	File.write( (char*)&m_nPiece0,				sizeof(int32) );
	File.write( (char*)&m_nRS0,					sizeof(int32) );
	File.write( (char*)&m_nPiece1,				sizeof(int32) );
	File.write( (char*)&m_nRS1,					sizeof(int32) );
	File.write( (char*)&m_nPiece2,				sizeof(int32) );
	File.write( (char*)&m_nRS2,					sizeof(int32) );
	File.write( (char*)&m_nPiece3,				sizeof(int32) );
	File.write( (char*)&m_nRS3,					sizeof(int32) );
	File.write( (char*)&m_nPiece4,				sizeof(int32) );
	File.write( (char*)&m_nRS4,					sizeof(int32) );
	File.write( (char*)&m_nPiece5,				sizeof(int32) );
	File.write( (char*)&m_nRS5,					sizeof(int32) );
	File.write( (char*)&m_Animation,			sizeof(int32) );
	File.write( (char*)&m_bSynch,				sizeof(BOOL) );
	File.write( (char*)&m_fMinXSize,			sizeof(float) );
	File.write( (char*)&m_fMaxXSize,			sizeof(float) );

	


	if( FAILED( CEffectUnitProp::SaveBin( File ) ) )
		return GERROR;
	return 0;
}

//////////////////////////////////////////////////////////////////////////

CModelConsole::CModelConsole( IEffectUnitProp* pEffectUnitProp )
			  : CEffectUnit( pEffectUnitProp )
			  , m_pModel(NULL)
{
	m_CurAngle		= 0;
	m_lastPercent	= 0;
	m_mark			= 0;
	m_move			= 0;
	m_count			= 0;
	m_counter		= 0;
	m_pModel		= new CModel(m_pLogicHandler);
	m_pModel->AddRef();
	SetProp( pEffectUnitProp );
	const CModelConsoleProp* pProp = (CModelConsoleProp*)m_pEffectUnitProp;
	if( m_pModel )
	{		
		m_pModel->UpdateBox();
		CAxisAlignedBox tempBox = m_pModel->GetAABB();
		CVector3f vecMax = CVector3f( 0, 0, 0 );
		for( EMap< float, CVector3f >::iterator it = m_pEffectUnitProp->m_XYZcale.begin(); 
		it != m_pEffectUnitProp->m_XYZcale.end(); it++ )
		{
			vecMax.x = max( it->second.x, vecMax.x );
			vecMax.y = max( it->second.y, vecMax.y );
			vecMax.z = max( it->second.z, vecMax.z );
		}

		float scale = 0.0f;
		for( EMap< float, BYTE >::iterator it = m_pEffectUnitProp->m_Scale.begin(); 
			it != m_pEffectUnitProp->m_Scale.end(); it++ )
		{
			scale = max( it->second/255.0f, scale );
		}
		vecMax = vecMax * scale;
		tempBox.scaleBox(vecMax);
		tempBox.setMaximum(tempBox.getMaximum() + m_OrgAABB.getMaximum());
		tempBox.setMinimum(tempBox.getMinimum() + m_OrgAABB.getMinimum());
		m_OrgAABB = tempBox;
	}
	m_DelayRender	= (float)pProp->m_DeleteTime;
	m_BirthRender	= 0.0f;//(float)((CSpriteProp*)m_pEffectUnitProp)->m_BirthTime;
	m_pLoadUnitProp = NULL;
}

CModelConsole::~CModelConsole(void)
{
	m_pModel->SetVisible(false);
	SafeRelease( m_pModel );
}

void CModelConsole::SetStartTime( uint32 STime,uint32 DTime )
{
	CEffectUnit::SetStartTime( STime,DTime );
	m_pModel->Render(0,NULL);
	m_pModel->SetNextAnimation( m_AniName, FramePair( 0, -1 ),false, 0 );
	
	//if(m_pModel)
	//	m_pModel->ResetAnimation();
}

void CModelConsole::ResetTime( uint32 STime,uint32 DTime )
{
	CEffectUnit::ResetTime( STime,DTime );
	m_CurAngle			= 0;
	m_lastPercent		= 0;
	m_mark				= 0;
	m_move				= 0;
	m_count				= 0;
	m_counter			= 0;
}

bool CModelConsole::SetProp( IEffectUnitProp* pEffectUnitProp )	
{ 
	if( !CEffectUnit::SetProp( pEffectUnitProp ) )
		return false;
	CModelConsoleProp* pNewProp = NULL;
	pNewProp = (CModelConsoleProp*)m_pEffectUnitProp;

	if( pNewProp == NULL )
		return false;

	
	if(!pNewProp->IsValid())
	{
		m_pLoadUnitProp = pEffectUnitProp;
		return false;
	}

	if( pNewProp->m_pSkeletalFrame != m_pModel->GetSketetalFrame() )
	{
		m_pModel->SetSkeletalFrame( pNewProp->m_pSkeletalFrame);
	}
	if( pNewProp->m_pAnimationGroup != m_pModel->GetAnimateGroup() )
	{
		m_pModel->SetAnimateGroup( pNewProp->m_pAnimationGroup);	
	}
	m_pModel->ClearMeshPiece();

	if( pNewProp->m_pPieceGroup )
	{
		if( pNewProp->m_nPiece0 && 	pNewProp->m_pPieceGroup->GetPieceClassName( pNewProp->m_nPiece0 - 1 ) )

			m_pModel->AddMeshPiece( pNewProp->m_pPieceGroup, 
									pNewProp->m_pPieceGroup->GetPieceClassName( pNewProp->m_nPiece0 - 1 ), 
									pNewProp->m_nRS0 );
		if( pNewProp->m_nPiece1 && pNewProp->m_pPieceGroup->GetPieceClassName( pNewProp->m_nPiece1 - 1 ) )
			m_pModel->AddMeshPiece( pNewProp->m_pPieceGroup, 
									pNewProp->m_pPieceGroup->GetPieceClassName( pNewProp->m_nPiece1 - 1 ), 
									pNewProp->m_nRS1 );
		if( pNewProp->m_nPiece2 && pNewProp->m_pPieceGroup->GetPieceClassName( pNewProp->m_nPiece2 - 1 ) )
			m_pModel->AddMeshPiece( pNewProp->m_pPieceGroup, 
									pNewProp->m_pPieceGroup->GetPieceClassName( pNewProp->m_nPiece2 - 1 ), 
									pNewProp->m_nRS2 );
		if( pNewProp->m_nPiece3 && pNewProp->m_pPieceGroup->GetPieceClassName( pNewProp->m_nPiece3 - 1 ) )
			m_pModel->AddMeshPiece( pNewProp->m_pPieceGroup, 
									pNewProp->m_pPieceGroup->GetPieceClassName( pNewProp->m_nPiece3 - 1 ), 
									pNewProp->m_nRS3 );
		if( pNewProp->m_nPiece4 && pNewProp->m_pPieceGroup->GetPieceClassName( pNewProp->m_nPiece4 - 1 ))
			m_pModel->AddMeshPiece( pNewProp->m_pPieceGroup, 
									pNewProp->m_pPieceGroup->GetPieceClassName( pNewProp->m_nPiece4 - 1 ), 
									pNewProp->m_nRS4 );
		if( pNewProp->m_nPiece5 && pNewProp->m_pPieceGroup->GetPieceClassName( pNewProp->m_nPiece5 - 1 ) )
			m_pModel->AddMeshPiece( pNewProp->m_pPieceGroup, 
									pNewProp->m_pPieceGroup->GetPieceClassName( pNewProp->m_nPiece5 - 1 ), 
									pNewProp->m_nRS5 );
	}



	ESet<EString> PieceAniName;
	EVector<IDNAME> AniName;
	for( int32 i = 0; pNewProp->m_pPieceGroup && i < pNewProp->m_pPieceGroup->GetPieceClassAniNum(); ++i )
		PieceAniName.insert( pNewProp->m_pPieceGroup->GetPieceClassAniName( i ) );

	for( ESet<EString>::iterator it = PieceAniName.begin(); it != PieceAniName.end(); ++it )
		AniName.push_back( (*it).c_str() );
	for( int32 i = PieceAniName.size(); 
		pNewProp->m_pAnimationGroup && i < pNewProp->m_pAnimationGroup->GetAnimationNum() + (int32)PieceAniName.size();
		++i )
		AniName.push_back( pNewProp->m_pAnimationGroup->GetAnimationName(i) );
	m_AniName = AniName.size() ? AniName[pNewProp->m_Animation] : "";
	m_pModel->SetNextAnimation( m_AniName, FramePair( 0, -1 ),false, 0 );
	m_pModel->Render(0,NULL);
	pNewProp->m_ClipPlane.RenderDebug();
	m_pLoadUnitProp = NULL;

	return true;
}

void CModelConsole::OnUnlinked( void )
{
	m_pParent = NULL;

	m_pModel->SetVisible(false);
}

void CModelConsole::SetVisible( bool isVisible )
{
	CEffectUnit::SetVisible(isVisible);
	if(m_pModel)
		m_pModel->SetVisible(isVisible&&IsInRenderTime());
}

void CModelConsole::RenderChild(IEffectUnit* pEffectUnit, uint32 uCurTime, RenderObjStyle* pRORS)
{
	if(	pEffectUnit->GetBindSkeletalId() == -1 && pEffectUnit->GetBindSocketId() == 0xFF )
	{
		int nSkeletalID = -1;
		int nSocketID = -1;
		CSkeletalFrame* pSkeFrame = NULL;
		if( m_pModel )
			pSkeFrame = m_pModel->GetSketetalFrame();
		if(pSkeFrame)
		{	
			eUpdatePosType eType = pEffectUnit->GetProp()->GetUpdatePosType();
			if( eType == UP_NODEOFFSET || eType == UP_NODE )
				nSkeletalID = pSkeFrame->GetSkeletalIDByName( pEffectUnit->GetProp()->GetAttachName() );
			else if( eType == UP_SOCKETOFFSET || eType == UP_SOCKET )
				pSkeFrame->GetIDBySocketName( nSkeletalID, nSocketID,  pEffectUnit->GetProp()->GetAttachName() );
			pEffectUnit->SetBindID((int16)nSkeletalID,(uint8)nSocketID);
		}
	}

	if( NULL != m_pModel && pEffectUnit->GetBindSkeletalId() >= 0 )
	{
		CMatrix World;
		m_pModel->GetMatrix( World, pEffectUnit->GetBindSkeletalId(), pEffectUnit->GetBindSocketId() );
		if( pEffectUnit->GetProp()->GetUpdatePosType() == UP_NODEOFFSET ||
			pEffectUnit->GetProp()->GetUpdatePosType() == UP_SOCKETOFFSET )
		{
			World._11 = World._22 = World._33 = World._44 = 1.0f;
			World._12 = World._13 = World._14 = 
			World._21 = World._23 = World._24 = 
			World._31 = World._32 = World._34 = 0.0f;
		}
		pEffectUnit->Render( World, uCurTime, pRORS );	
	}
	else
		pEffectUnit->Render( m_matWorld, uCurTime, pRORS );	
}

const EFFECT_RESULT CModelConsole::RenderEx( const CMatrix& matWorld, RenderObjStyle* pRORS )
{
	CModelConsoleProp* pProp = (CModelConsoleProp*)m_pEffectUnitProp;
	EFFECT_RESULT Result( NULL, true, false );

	if(m_pLoadUnitProp)
		SetProp(m_pLoadUnitProp);

	if( !IsInRenderTime() ) 
	{
		m_pModel->SetVisible(false);
		return Result;
	}	

	float Percent = GetUnitPercent();
	m_pModel->SetVisible(true);
	m_ParentMat = matWorld;
	CCamera* pCamera = NULL;
	if ( CGraphic::GetInst()->GetActiveRenderTarget() )
		pCamera = CGraphic::GetInst()->GetActiveRenderTarget()->GetCamera();
	else	
		pCamera = CMainWindowTarget::GetInst()->GetCamera();


	CVector3f XYZcale = GetMapValue( pProp->m_XYZcale, Percent );

	float ScaleX = ( pProp->m_fMinXSize + ( XYZcale.x - pProp->m_fMinXSize )*GetMapValue( pProp->m_Scale, Percent )/255.0f );
	float ScaleY = ( pProp->m_fMinYSize + ( XYZcale.y - pProp->m_fMinYSize )*GetMapValue( pProp->m_Scale, Percent )/255.0f );
	float ScaleZ = ( pProp->m_fMinZSize + ( XYZcale.z - pProp->m_fMinZSize )*GetMapValue( pProp->m_Scale, Percent )/255.0f );

	CVector3f Offset = GetMapValue( pProp->m_vecOffset, Percent );

	bool bEnableShadow = pProp->m_bEnableShadow;

	// 添加可改变模型颜色和alpha功能

	CColor Color = 0;
	uint32 color = 0;

	if( pProp->m_Color.size() )
	{
		Color = GetMapValue( pProp->m_Color, Percent );
		

		if( uint32( Color ) && ( uint32( Color )&0xffffff ) != 0xffffff )
		{
			color = Color|0xff000000;
		}
	}

	uint8 a = Color.A;
	float alpha = a/255.0f;

	if(m_BirthRender>=pProp->m_BirthTime)
	{
		if( pProp->m_DeleteTime > 0 )
			alpha = alpha * (m_DelayRender / pProp->m_DeleteTime);
	}
	else
	{
		if( pProp->m_BirthTime > 0 )
			alpha = alpha * (m_BirthRender / pProp->m_BirthTime );
		m_BirthRender += m_DeltaTime;
	}

	if( alpha < 0.01f )
	{
		m_pModel->SetVisible(false);
		return Result;
	}
	color = Color|0xff000000;

	sMaterial Mat = 
	{ 
		CColor4(color), 
		CColor4(color), 
		CColor4((uint32)0), 
		CColor4((uint32)0xffffffff), 
		0, 
	};

	//////////////////////////////////////////////////////////////////////////

	CMatrix temp; //= matWorld;
	switch(pProp->m_eFaceType)// == SPRF_CAMERA )
	{
	case SPRF_NORMAL:
		temp = matWorld;
		break;
	case SPRF_CAMERA:
		pCamera->GetBillboardAll( temp );
		temp._41 = matWorld._41;
		temp._42 = matWorld._42;
		temp._43 = matWorld._43;
		break;
	case SPRF_CAMERANONEX:
		pCamera->GetBillboardNoneX( temp );
		temp._41 = matWorld._41;
		temp._42 = matWorld._42;
		temp._43 = matWorld._43;
		break;
	default:
		break;
	}

	m_matWorld.SetScale( ScaleX, ScaleY, ScaleZ );
	m_matWorld._41 = Offset.x;
	m_matWorld._42 = Offset.y;
	m_matWorld._43 = Offset.z;//放这里空有用
	m_matWorld = m_matWorld*temp;

	if( pProp->m_AngularVelocity > 0.00001f )
	{
		m_CurAngle += pProp->m_AngularVelocity*m_DeltaTime/1000;
		if( m_CurAngle > 6.283f )
			m_CurAngle = 0.0;
		if( m_CurAngle < 0.0f )
			m_CurAngle = 6.283f;
		CMatrix Rotate;
		pProp->m_pEffectClassDesc->SetRotate( Rotate, pProp->m_vecRotate/pProp->m_AngularVelocity, m_CurAngle );
		m_matWorld = Rotate*m_matWorld;
	}
	//////////////////////////////////////////////////////////////////////////
	
	//bool bLink = false;
	RenderObjStyle rs = {ROS_LIGHTMAT1, true, true, 0, 0};
	
	if( pRORS )
	{ 
		rs = *pRORS;
	}

	if( bEnableShadow )
		rs.m_eType = (ERenderObjStyle)((DWORD)(rs.m_eType) | (DWORD)ROS_SHADOW);
	else
		rs.m_eType = (ERenderObjStyle)((DWORD)(rs.m_eType) & ~(DWORD)ROS_SHADOW);

	rs.m_nParam2 = (uint32)&Mat;

	if( m_pParent && m_pParent->GetType() == ROT_MODEL && ((CModel*)m_pParent)->GetSketetalFrame() )
	{
		int32 nParentSkID = ((CModel*)m_pParent)->GetSketetalFrame()->GetSkeletalIDByName( pProp->m_szParentSk.c_str() );

		int32 nSkID = pProp->m_nSkIdx - 1;
		if( nParentSkID != -1 && nSkID != -1 )
		{
			m_pModel->SetFixMatrix(m_matWorld);
			m_pModel->Render( (uint32)(m_StartTime + m_FrameTime) , &rs );
		}
	}

	if( alpha < 0.99f )
	{
		rs.m_eType = (ERenderObjStyle)((DWORD)(rs.m_eType) | (DWORD)ROS_ALPHA);
		rs.m_nParam1 |= DELTARS_ALPHADIF;
		Mat.Diffuse.a = alpha;
	}

	// add end
	

	if( pProp->m_bSetTextureMatrix )//纹理旋转平移
	{
		CMatrix matTextureRotate,matTextureScale;		
		float fCurTime	= GetUnitPercent();//m_FrameTime /(float)pProp->m_TimeRange;
		if(abs(m_lastPercent-Percent)>0.2)
		{
			++m_count;
			m_mark =!m_mark;
		}
		m_lastPercent = Percent;
		if(m_mark)
			fCurTime+=1.0f;
		
		if(m_count==2)
		{
			m_counter+=2;
			m_count = 0;
		}
		if(m_counter>100000)
			m_counter = 0;
		m_move = m_counter + fCurTime;

		float fScale			= pProp->m_fTextMinScale + ( pProp->m_fTextMaxScale - pProp->m_fTextMinScale )*Percent;
		matTextureRotate.SetRotateZ( pProp->m_fTextAngularVel*m_move );
		matTextureScale.SetScale( fScale, fScale, fScale );
		matTextureRotate		= matTextureRotate*matTextureScale;
		
		matTextureRotate._31	= pProp->m_vecTextMoveVel.x*m_move;
		matTextureRotate._32	= pProp->m_vecTextMoveVel.y*m_move;

		CRenderPipeline::GetInst()->SetTextureMatrix( 0, &matTextureRotate );
	}

	CPieceRS pieceRS;
	ZeroMemory(&pieceRS, sizeof(pieceRS));
	if (!pProp->m_ClipDistMap.empty())
	{
		pProp->m_ClipPlane.originPlane.m_Dist = -GetMapValue(pProp->m_ClipDistMap, Percent);
		pieceRS.m_nMask |= PIECERS_CLIPPLANE;
		pProp->m_ClipPlane.SetTransform(matWorld);
		pieceRS.m_ClipPlane = pProp->m_ClipPlane.transformedPlane;
	}

	rs.m_eType = (ERenderObjStyle)((DWORD)(rs.m_eType) & ~(DWORD)(ROS_ZFAILD));
	if (!pProp->m_mapRefract.empty())
	{
		rs.m_eType = ROS_REFRACT;
		pieceRS.m_fRefractIndex = GetMapValue(pProp->m_mapRefract, Percent);
	}

	m_pModel->SetPieceClassRS("", pieceRS);
	m_pModel->SetFixMatrix(m_matWorld);
	m_pModel->Render( (uint32)(m_StartTime + m_FrameTime - m_DelayTime), &rs );

	if( pProp->m_bSetTextureMatrix )
		CRenderPipeline::GetInst()->SetTextureMatrix( 0, NULL );
	
	return Result;
}


void CModelConsole::Free()
{
	if( IsInRenderTime() && m_FrameTime!= 0 && m_pEffectUnitProp->m_bSlownDisappear )
	{
		((CModelConsoleProp*)m_pEffectUnitProp)->m_pEffectClassDesc->AddLeavingsEffectUnit( this );
	}
	else
		delete this;
}

const EFFECT_RESULT	CModelConsole::RenderEx( const CMatrix& matWorld,uint32 uCurTime,  RenderObjStyle* pRORS )
{
	// time rejust for the modle shake
	RejustTime(uCurTime);
	m_DeltaTime = uCurTime>m_CurTime?(uCurTime - m_CurTime):0.0f;
	m_CurTime = uCurTime;
	m_FrameTime += m_DeltaTime;

	if(!IsInRenderTime())
		m_FrameTime = float(m_pEffectUnitProp->m_TimeRange + m_DelayTime);
	RenderEx( m_ParentMat , pRORS );

	m_DelayRender -= m_DeltaTime;
	return EFFECT_RESULT( NULL, true, false , m_DelayRender <= 0.0f || m_DelayRender > (float)((CModelConsoleProp*)m_pEffectUnitProp)->m_DeleteTime );
}

const AniResult CModelConsole::Render( const CMatrix& matWorld, uint32 uCurTime,  RenderObjStyle* pRORS )
{
	RejustTime(uCurTime);
	return CEffectUnit::Render( matWorld, uCurTime,  pRORS );
}

void CModelConsole::RejustTime(uint32& uCurTime)
{
	// times to reset, the reset dis
	static int bst = -1;
	static int frame_times = 1;
	static int frame_dis = 1;

	if (m_FrameTime == 0.0f)
	{
		bst = -1;
		float dis =  33.33f - (uint32)( 33.33f);
		dis /=  33.33f;
		
		if ( dis == 0.0f)
		{
			frame_dis = 0;
			frame_times = 1;
		}
		else
		{
			float tm = 1.0f / ( dis *  33.33f);
			if (tm > 2.0f)
			{
				frame_dis = 1;
				frame_times = (int)(tm);
			}
			else
			{
				frame_dis = 2;
				frame_times = 2*(int)(tm);
			}
		}
		
	}
	m_DeltaTime = uCurTime>m_CurTime?(uCurTime - m_CurTime):0.0f;
	if (m_DeltaTime != 0.0f)
	{
		uCurTime = (uint32)((((uint32)(m_DeltaTime / 33.33f)) * 33.33f) + m_CurTime);
		if (uCurTime != m_CurTime)
		{
			++bst;
			if ( bst == frame_times / 2 )
			{
				uCurTime += (uint32)(frame_dis);
			}
			if ( bst == frame_times )
			{
				bst = -frame_times/2;
			}
		}
	}
}

//bool FunModelConsoleSetProp::Do(void)
//{
//	m_pObject->SetProp(m_pEffectUnitProp);
//	return true;
//}
