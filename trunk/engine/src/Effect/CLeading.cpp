/*/
 +------------------------------------------------------+
 | 引导技能特效											|
 | Milk													|
 | 2007													|
 +------------------------------------------------------+
*/

#include "stdafx.h"
#include "CLeading.h"
#include "TimeHelper.h"
#include "resource.h"
#include "CRenderTarget.h"
#include "CWindowTarget.h"
#include "CMath.h"
#include "CRenderPipeline.h"
#include "TSqrAllocator.inl"
#include "CGraphic.h"
#include "CCamera.h"

DYNAMIC_CREATE_EFFECTUNITBASE( eLeading, IDS_LEADING, CLeading, CLeadingProp )

// 文件版本定义
#define LEADING_ORIGIN				0
#define LEADING_DELETE_DEMO_LEN		1
#define LEADING_ADD_LINK_TYPE		2
#define LEADING_ADD_TEXOP_FLAG		3
#define LEADING_ADD_FACE_TYPE		4

// 属性
CLeadingProp::CLeadingProp( UINT ID, IEffectClassDesc* pEffectClassDesc )
: CEffectUnitProp( ID, pEffectClassDesc )
{
	m_fStartAngle = 0;
	m_fRotSpeed	  = 0;
	m_nPieceCount = 1;

	m_nDouCount		= 0;
	m_fDouPercent   = 0;
	m_fDouInterval  = 0;
	m_fUVSpeed		= 0;

	m_eFaceType					= SPRF_NORMAL;
	m_fZBias					= 0.0f;
	m_eBlendModel				= SPRB_ADD;
	m_eTextOp					= SPRO_MODULATE;
	m_eZTest					= Z_TESTONLY;
	m_bLightEnable				= FALSE;
	m_eLType					= LPLK_JUMP;
	m_eSSType					= LPSS_Player;
	m_JumpNum					= 1;
	m_JumpSpeed					= 100;
}

CLeadingProp::~CLeadingProp(void)
{
}

int CLeadingProp::LoadBin( IEffectRead& File )
{
	UINT Version;
	File.read( (char*)&Version,					sizeof(UINT) );
	switch( Version )
	{	
	case LEADING_ADD_FACE_TYPE:
		File.read( (char*)&m_eFaceType,				sizeof(SPR_FACE) );
	case LEADING_ADD_TEXOP_FLAG:
		File.read( (char*)&m_eTextOp,				sizeof(SPR_BLENDOP) );
	case LEADING_ADD_LINK_TYPE:
		File.read( (char*)&m_JumpSpeed,				sizeof(int32) );
		File.read( (char*)&m_JumpNum,				sizeof(int32) );
		File.read( (char*)&m_eLType,				sizeof(eLP_LKType) );
		File.read( (char*)&m_eSSType,				sizeof(eLP_StartScr) );
	case LEADING_DELETE_DEMO_LEN:
		File.read( (char*)&m_fStartAngle,			sizeof(float) );
		File.read( (char*)&m_fRotSpeed,				sizeof(float) );
		File.read( (char*)&m_fUVSpeed,				sizeof(float) );
		File.read( (char*)&m_nPieceCount,			sizeof(uint32) );
		File.read( (char*)&m_nDouCount,				sizeof(uint32) );
		File.read( (char*)&m_fDouPercent,			sizeof(float) );
		File.read( (char*)&m_fDouInterval,			sizeof(float) );

		File.read( (char*)&m_eBlendModel,			sizeof(SPR_BLENDMODE) );
		File.read( (char*)&m_eZTest,				sizeof(Z_TEST) ); //=== bool
		File.read( (char*)&m_bLightEnable,			sizeof(BOOL) ); //=== bool
		File.read( (char*)&m_fZBias,				sizeof(float) );
		break;

	case LEADING_ORIGIN:
	default:
		File.read( (char*)&m_fStartAngle,			sizeof(float) );
		File.read( (char*)&m_fRotSpeed,				sizeof(float) );
		File.read( (char*)&m_fUVSpeed,				sizeof(float) );
		File.read( (char*)&m_nPieceCount,			sizeof(uint32) );
		File.read( (char*)&m_nDouCount,				sizeof(uint32) );
		File.read( (char*)&m_fDouPercent,			sizeof(float) );
		File.read( (char*)&m_fDouInterval,			sizeof(float) );
		
		File.read( (char*)&m_eBlendModel,			sizeof(SPR_BLENDMODE) );
		File.read( (char*)&m_eZTest,				sizeof(Z_TEST) ); //=== bool
		File.read( (char*)&m_bLightEnable,			sizeof(BOOL) ); //=== bool
		File.read( (char*)&m_fZBias,				sizeof(float) );
		float fDemoLength;//暂时不用了
		File.read( (char*)&fDemoLength,				sizeof(float) );
		break;
	}

	if( FAILED( CEffectUnitProp::LoadBin( File ) ) )
		return GERROR;
	return 0;
}

int CLeadingProp::SaveBin( IEffectWrite& File )
{
	UINT Version = LEADING_ADD_FACE_TYPE;
	File.write( (char*)&Version,				sizeof(UINT) );

	File.write( (char*)&m_eFaceType,			sizeof(SPR_FACE) );
	File.write( (char*)&m_eTextOp,				sizeof(SPR_BLENDOP) );

	File.write( (char*)&m_JumpSpeed,			sizeof(int32) );
	File.write( (char*)&m_JumpNum,				sizeof(int32) );
	File.write( (char*)&m_eLType,				sizeof(eLP_LKType) );
	File.write( (char*)&m_eSSType,				sizeof(eLP_StartScr) );

	File.write( (char*)&m_fStartAngle,			sizeof(float) );
	File.write( (char*)&m_fRotSpeed,			sizeof(float) );
	File.write( (char*)&m_fUVSpeed,				sizeof(float) );
	File.write( (char*)&m_nPieceCount,			sizeof(uint32) );
	File.write( (char*)&m_nDouCount,			sizeof(uint32) );
	File.write( (char*)&m_fDouPercent,			sizeof(float) );
	File.write( (char*)&m_fDouInterval,			sizeof(float) );

	File.write( (char*)&m_eBlendModel,			sizeof(SPR_BLENDMODE) );
	File.write( (char*)&m_eZTest,				sizeof(Z_TEST) );
	File.write( (char*)&m_bLightEnable,			sizeof(BOOL) );
	File.write( (char*)&m_fZBias,				sizeof(float) );

	if( FAILED( CEffectUnitProp::SaveBin( File ) ) )
		return GERROR;
	return 0;
}


// 实现 ------------------------------------------------------------------
CLeading::CLeading( IEffectUnitProp* pEffectUnitProp )
: CEffectUnit( pEffectUnitProp )
{
	m_CurAngle	= 0;
	m_CurUVBias	= 0;
	
	const CLeadingProp* pProp = (CLeadingProp*)m_pEffectUnitProp;
	m_OldDouCount = pProp->m_nDouCount;
	if(  pProp->m_nDouCount > 0 )
	{
		m_CutDouBias.resize( pProp->m_nDouCount );
		m_CutDouBiasV.resize( pProp->m_nDouCount );// = new float[  pProp->m_nDouCount ];

		for( int i = 0; i <  pProp->m_nDouCount; ++i)
		{
			m_CutDouBias[i] = 0.0f;
			m_CutDouBiasV[i] = 0.0f;
		}
	} else 
	{
		m_CutDouBias  = NULL;
		m_CutDouBiasV = NULL;
	}
	m_CurDouTime = 0;
	m_Key	  = 0.0f;
	m_CurNum  = 1.0f;
}

CLeading::~CLeading(void)
{
}

void CLeading::SetStartTime( uint32 STime,uint32 DTime )
{
	m_CurNum = 1.0f;
	CEffectUnit::SetStartTime(STime,DTime);
}

void CLeading::ResetTime( uint32 STime,uint32 DTime )
{
	CEffectUnit::ResetTime(STime,DTime);
	m_CurAngle	= 0;
	m_CurUVBias	= 0;
}

void CLeading::ChangeDouCount()
{
	const CLeadingProp* pProp = (CLeadingProp*)m_pEffectUnitProp;
	m_OldDouCount = pProp->m_nDouCount;
	if(  pProp->m_nDouCount > 0 )
	{
		m_CutDouBias.resize(pProp->m_nDouCount); // = new float[  pProp->m_nDouCount ];
		m_CutDouBiasV.resize(pProp->m_nDouCount);// = new float[  pProp->m_nDouCount ];
		for( int i = 0; i <  pProp->m_nDouCount; ++i)
		{
			m_CutDouBias[i]  = 0.0f;
			m_CutDouBiasV[i] = 0.0f;
		}
	} else 
	{
		m_CutDouBias  = NULL;
		m_CutDouBiasV = NULL;
	}
}

void CLeading::SetRenderCallback( IEffectUnitHandler* pUnitHandler )
{
	SafeRelease( m_pUnitHandler );
	if (pUnitHandler && ((CLeadingProp*)m_pEffectUnitProp)->m_eSSType == CLeadingProp::LPSS_Player)
	{
		m_pUnitHandler = pUnitHandler; 
		m_pUnitHandler->AddRef();
	}
}

const EFFECT_RESULT CLeading::RenderEx( const CMatrix& matWorld, RenderObjStyle* pRORS )
{
	const CLeadingProp* pProp = (CLeadingProp*)m_pEffectUnitProp;
	EFFECT_RESULT Result( NULL, true, false );

	float Percent = GetUnitPercent();
	
 	const EMap<float,CVector3f>& DestVec =	( (m_DestVector.size()!=0 && pProp->m_eSSType == CLeadingProp::LPSS_Player) || pProp->m_eSSType == CLeadingProp::LPSS_LINE  ) ? m_DestVector:pProp->m_vecOffset;
	if( !IsInRenderTime()|| ( DestVec.size()==0 && pProp->m_eSSType != CLeadingProp::LPSS_LINE ))
		return Result;
	
	if(pProp->m_nPieceCount<=0||pProp->m_nDouCount<0)
		return Result;

	if( m_OldDouCount != pProp->m_nDouCount )
		ChangeDouCount();

	CCamera* pCamera = NULL;
	if ( CGraphic::GetInst()->GetActiveRenderTarget() )
		pCamera = CGraphic::GetInst()->GetActiveRenderTarget()->GetCamera();
	else	
		pCamera = CMainWindowTarget::GetInst()->GetCamera();
	CVector3f OrgOffset( 0, 0, 0 );
	ITexture* pCurText = NULL;
	uint32 Color;
	float Scale = 1.0f;


	pProp->GetCurAdvanceProp( Percent, OrgOffset, pCurText, Color, Scale );

	if(  pProp->m_eSSType == CLeadingProp::LPSS_LINE  )
	{
		m_DestVector.clear();

		if(m_pParent != NULL && m_pParent->GetType() == ROT_MODEL)
			OrgOffset *= ((CBaseModel*)m_pParent)->GetWorldMatrix();
		m_DestVector[0.0] = OrgOffset;
	}

	EVector<CVector3f> pos((4 + 2 * pProp->m_nDouCount) * pProp->m_nPieceCount );
	float PieceWidth	= Scale * 32;
	

	float DeltaAngle	= CMath::pi / pProp->m_nPieceCount;
	// 摄象机放向和平面法线组成的平面与平面垂直
	// 特殊处理摄象机与平面平行的情况
	if (pProp->m_eFaceType == SPRF_CAMERA)	
		m_CurAngle = 0.0f;
	else
	{
		float RotSpeed		= pProp->m_fRotSpeed / 180.0f * CMath::pi ;
		m_CurAngle		   += RotSpeed * m_DeltaTime / 1000.0f;
	}
	
	float RotAngle		= pProp->m_fStartAngle / 180.0f * CMath::pi + m_CurAngle;
	m_CurUVBias		   += pProp->m_fUVSpeed * m_DeltaTime / 255000.f;
	
	
	if( m_CurUVBias > 1.0f )
		m_CurUVBias -= 1.0f;
	int32  LinkNum = int32(m_CurNum);
	m_CurNum += (pProp->m_JumpSpeed * m_DeltaTime) / 10000.f;
	int32  DestLinkNum = int32(m_CurNum);

	if(((size_t)DestLinkNum) > DestVec.size())
	{
		if( pProp->m_eLType==CLeadingProp::LPLK_JUMPONCE)
		{
			if(((size_t)DestLinkNum) > ( DestVec.size() + pProp->m_JumpNum - 1))
				return Result;
		}
		else
		{
			DestLinkNum = DestVec.size();
			m_CurNum  = (float)DestVec.size();
		}
	}
	m_CurDouTime += m_DeltaTime;
	if( pProp->m_nDouCount > 0 && m_CurDouTime > pProp->m_fDouInterval )
	{
		m_CurDouTime = 0;
		for( int i = 0; i < pProp->m_nDouCount; ++i)
		{
			m_CutDouBias[i] = ( pProp->m_fDouPercent * (rand() - ( RAND_MAX / 2 ))/ ( RAND_MAX / 2 )) / pProp->m_fDouInterval;
		}
	}

	for( int i = 0; i < pProp->m_nDouCount; ++i)
	{
		m_CutDouBiasV[i] += m_CutDouBias[i] * m_DeltaTime;
		if( m_CutDouBiasV[i] > pProp->m_fDouPercent )
			m_CutDouBiasV[i] = pProp->m_fDouPercent;
		if( m_CutDouBiasV[i] < -pProp->m_fDouPercent )
			m_CutDouBiasV[i] = -pProp->m_fDouPercent;
	}

	VerNCT* pVB;
	WORD* pIB;
	STATIC_RS* pRS;
	size_t	sLinkSize = 0;
	int32	index;
	CVector3f SrcPos(matWorld._41, matWorld._42, matWorld._43);
	EMap<float,CVector3f>::const_iterator it = DestVec.begin(),eit = DestVec.end();
	if(pProp->m_eSSType == CLeadingProp::LPSS_LIST )
		SrcPos.Init(it->second);
	
	switch(pProp->m_eLType)
	{
	case CLeadingProp::LPLK_JUMP:
		sLinkSize	= DestLinkNum;
		index		= 0;
		m_Key		= it->first;
		break;
	case CLeadingProp::LPLK_JUMPONCE:
		index		= DestLinkNum - pProp->m_JumpNum;
		if(pProp->m_JumpNum<1)
			return Result;
		if(index>0)
		{
			it = DestVec.find(m_Key);
			SrcPos.Init(it->second);
			++it;			
			if(LinkNum!=DestLinkNum&&LinkNum>pProp->m_JumpNum)
				m_Key = it->first;

			if((size_t)DestLinkNum > DestVec.size())
				sLinkSize	= pProp->m_JumpNum - DestLinkNum + DestVec.size() + 1;
			else				
				sLinkSize	= pProp->m_JumpNum;	
		}
		else
		{
		 	sLinkSize = DestLinkNum;
			m_Key = it->first;				
		}
		break;
	case CLeadingProp::LPLK_IMM:
	default:
		sLinkSize	= DestVec.size();
		index		= 0;
		DestLinkNum	= sLinkSize;
		m_Key		= it->first;
		break;
	}

	CMatrix* pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, pRORS->m_eType, pCurText, NULL, PT_TRIANGLELIST, 
		(4 + 2 * pProp->m_nDouCount) * pProp->m_nPieceCount * sLinkSize, (2 * (pProp->m_nDouCount + 1)) * pProp->m_nPieceCount * sLinkSize, VerNCT::Format, sizeof(VerNCT),
		(void**)&pVB, (void**)&pIB, (void**)&pRS );

	switch(pProp->m_eSSType)
	{
	case CLeadingProp::LPSS_LIST:
		(*pMatrix) = matWorld;
		break;
	default:
		(*pMatrix).Identity();
	}

	pRS->m_TexOP_S0			= 2 + pProp->m_eTextOp;
	pRS->m_Cull				= CULLT_NONE;				//D3DCULL_NONE;
	pRS->m_AlpOP_S0			= TOP_MODULATE;				//D3DTOP_MODULATE;
	pRS->m_LightEnable		= pProp->m_bLightEnable;
	pRS->m_ZTestEnable		= pProp->m_eZTest != Z_NONE;
	pRS->m_ZWrite			= pProp->m_eZTest != Z_TESTONLY;
	pRS->m_AlphaBlendEnable = pProp->m_eBlendModel != SPRB_NONE;
	pRS->m_DestBlend		= pProp->m_eBlendModel == SPRB_ADD ? 2 : pRS->m_DestBlend;
	float zbias = 0.0f;
	if ( pProp->m_fZBias != 0.0f )
		zbias = CalculateDifZBias( pProp->m_fZBias, m_matWorld, pCamera->getViewMatrix(),
								   pCamera->getFarDist(), pCamera->getNearDist() );
	//pRS->m_fSlopeScaleZBias	= 0.0f;
	pRS->m_fZBias			= zbias;

	int BaseVB = 0,BaseIB = 0;
	
	CVector3f Target;
	for( ; it!=eit && index<DestLinkNum ; ++it, ++ index,
		BaseIB += pProp->m_nPieceCount,
		BaseVB += (4 + 2 * pProp->m_nDouCount) * pProp->m_nPieceCount)
	{
		CVector3f tempv3f(it->second - SrcPos);

		float xz = sqrt( tempv3f.x * tempv3f.x + tempv3f.z * tempv3f.z);
		float nx;
		xz == 0.0f ? nx = 0.0f : nx = tempv3f.x / xz;
			
		float anglex = acos( nx );
		if( tempv3f.z < 0.0f )
			anglex = CMath::two_pi - anglex;

		float xyz = sqrt( xz * xz + tempv3f.y * tempv3f.y);
		float ny;
		xyz == 0.0f ? ny = 0.0f : ny = xz / xyz;
			
		float angley = acos( ny );
		if( tempv3f.y < 0 )
			angley = CMath::two_pi - angley;

		CMatrix rot;
		rot.Identity();

		CMatrix	matTemp;
		matTemp.SetRotateZ( angley );
		rot *= matTemp;
		matTemp.SetRotateY( -anglex );
		rot *= matTemp;

		float PieceLength	= tempv3f.Mag(); 
		PieceLength = PieceLength / ( pProp->m_nDouCount + 1);

		// 计算
		if (pProp->m_eFaceType == SPRF_CAMERA)	
		{
			if (pProp->m_eSSType == CLeadingProp::LPSS_LIST)
			{
				CVector3f pots[4];
				pots[0] = SrcPos;	pots[1] = it->second;
				pots[2] = CVector3f::ZERO;pots[3] = CVector3f::UNIT_Y; 

				for (int i = 2; i < 4; ++i)
					pots[i].FastMultiply(rot);
				for (int i = 0; i < 4; ++i)
					pots[i].FastMultiply((*pMatrix));

				tempv3f = pots[1] - pots[0];
				CVector3f tempf = pots[3] - pots[2];

				CVector3f tempSur = pCamera->getDIRECTION().Cross(tempv3f);
				tempSur = tempSur.Cross(tempv3f);

				tempSur.normalize();	tempf.normalize();
				RotAngle = CMath::ACos( tempf.Dot(tempSur) );

				CVector3f tempPo = tempSur.Cross(tempf);
				if (tempPo.Dot(tempv3f) < 0.0f)
					RotAngle = CMath::two_pi - RotAngle;
			}
			else
			{
				CVector3f tempSur = pCamera->getDIRECTION().Cross(tempv3f);
				tempSur = tempSur.Cross(tempv3f);

				CVector3f tempf(CVector3f::UNIT_Y);
				tempf.FastMultiply(rot);

				tempSur.normalize();	tempf.normalize();
				RotAngle = CMath::ACos( tempf.Dot(tempSur) );

				CVector3f tempPo = tempSur.Cross(tempf);
				if (tempPo.Dot(tempv3f) < 0.0f)
					RotAngle = CMath::two_pi - RotAngle;
			}

		}


		for( int i = 0; i < pProp->m_nPieceCount; ++i)
		{
			int base = i * ( 4 + 2 * pProp->m_nDouCount );
			float dy = sin( RotAngle);
			float dz = cos( RotAngle);		
			float Bias = 0;
			pos[base + 0].Init( 0,	-dy * PieceWidth / 3.0f,  -dz * PieceWidth / 3.0f);
			pos[base + 1].Init( 0,	 dy * PieceWidth / 3.0f,   dz * PieceWidth / 3.0f);

			pos[base + 0].FastMultiply( rot );
			pos[base + 1].FastMultiply( rot );

			pos[base + 0] += SrcPos;
			pos[base + 1] += SrcPos;


			for( int j = 1; j <= pProp->m_nDouCount; ++j)
			{
				Bias = m_CutDouBiasV[j-1];
				pos[base + j * 2 + 0].Init( PieceLength * j, -dy * PieceWidth + dy * Bias,  -dz * PieceWidth + dz * Bias );
				pos[base + j * 2 + 1].Init( PieceLength * j,  dy * (PieceWidth + Bias),   dz * (PieceWidth + Bias ));
		
				pos[base + j * 2 + 0].FastMultiply( rot );
				pos[base + j * 2 + 1].FastMultiply( rot );
				pos[base + j * 2 + 0] += SrcPos;
				pos[base + j * 2 + 1] += SrcPos;
			}												 

			int k = pProp->m_nDouCount + 1;
			pos[base + k * 2 + 0].Init( PieceLength * k, -dy * PieceWidth/ 3.0f,  -dz * PieceWidth / 3.0f);
			pos[base + k * 2 + 1].Init( PieceLength * k,  dy * PieceWidth/ 3.0f,   dz * PieceWidth / 3.0f);
			pos[base + k * 2 + 0].FastMultiply( rot );
			pos[base + k * 2 + 1].FastMultiply( rot );
			pos[base + k * 2 + 0] += SrcPos;
			pos[base + k * 2 + 1] += SrcPos;

			CVector3f Normal = (pos[base + 1] - pos[base + 0]).Cross( (pos[base + 3] - pos[base + 0]) );
			Normal.Normalize();

			for( int j = 0 ; j < pProp->m_nDouCount + 2; ++j)
			{
				pVB[BaseVB + base + 2 * j + 0].Set( pos[base + 2 * j + 0].x, pos[base + 2 * j + 0].y, pos[base + 2 * j + 0].z,	Normal.x, Normal.y, Normal.z, Color, 1.0f / (pProp->m_nDouCount+1) * j - m_CurUVBias, 1 );
				pVB[BaseVB + base + 2 * j + 1].Set( pos[base + 2 * j + 1].x, pos[base + 2 * j + 1].y, pos[base + 2 * j + 1].z,	Normal.x, Normal.y, Normal.z, Color, 1.0f / (pProp->m_nDouCount+1) * j - m_CurUVBias, 0 );
			}

			for( int j = 0; j < 2 * (pProp->m_nDouCount + 1); ++j)
			{
				int t = ((pProp->m_nDouCount + 1) * (i + BaseIB) + j);
				int b = 2 * j + BaseVB;
				pIB[ 6 * t + 0 ] = (WORD)( base + b + 0 );
				pIB[ 6 * t + 1 ] = (WORD)( base + b + 1 );
				pIB[ 6 * t + 2 ] = (WORD)( base + b + 2 );
				pIB[ 6 * t + 3 ] = (WORD)( base + b + 2 );
				pIB[ 6 * t + 4 ] = (WORD)( base + b + 1 );
				pIB[ 6 * t + 5 ] = (WORD)( base + b + 3 );
			}
			RotAngle += DeltaAngle;
		}
		SrcPos = it->second;
	}
	pos.clear();

	return Result;
}























