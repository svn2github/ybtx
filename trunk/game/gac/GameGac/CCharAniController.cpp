#include "stdafx.h"
#include "CAnimationController.h"
#include "CCharAniController.h"
#include "CRenderObject.h"
#include "CCharacterFollower.h"

#define MAX_DEGREES			45.0f //旋转的最大角度
#define MAX_LOOK_DEGREES	75.0f //最大看度
#define MAX_Y_MAX_DEGREES	20.0f //仰头的最大角度
#define MAX_Y_MIN_DEGREES	-20.0f//低头头的最大角度

string CCharacterController::st_HeadBoneName	= "Bip01 Head";
string CCharacterController::st_SternumBoneName	= "Bip01 Spine1";
string CCharacterController::st_CoxalBoneName	= "Bip01 Spine";
string CCharacterController::st_TargetBoneName	= "Bip01 Head";

const float CCharacterController::st_MaxDegree		= MAX_DEGREES*CMath::pi/180.0f;
const float CCharacterController::st_MaxLookDegree	= MAX_LOOK_DEGREES*CMath::pi/180.0f;

enum PartAniCtrlState
{
	EPACS_OUTOFCTRL,
	EPACE_INCTRL,
	EPACE_BLANDIN,
	EPACE_BLANDOUT,
};

class CCharPartCtrl  : public CAnimationController
{
public:
	CCharPartCtrl( CCharacterController* CharCtrl,int32 SkeId )
		: CAnimationController( CharCtrl->m_pCharacter->GetRenderObject() , SkeId )
		, m_eCtrlState(EPACS_OUTOFCTRL)
		, m_CurBlandTime(0)
		, m_isNeedBegin(true)
	{
	}; 

	virtual ~CCharPartCtrl(){};
	virtual void SetDirection(float Degree)
	{
		if( m_fDegree != Degree )
		{
			m_fDegree = Degree;
			m_EndQuat.FromAngleAxis(Degree,CVector3f::UNIT_Y);
		}
	}

	void ChangeDirection(float Degree)
	{
		if(m_eCtrlState==EPACS_OUTOFCTRL)
		{
			m_isNeedBegin	= true;
		}
		else
		{
			m_isNeedBegin	= false;
			m_BeginQuat = m_CurQuat;
		}
		
		m_eCtrlState = EPACE_BLANDIN;
		m_CurBlandTime  = 0;
		SetDirection(Degree);
	}
	
	void Clear(bool Imme)
	{
		if(Imme)
		{
			m_eCtrlState = EPACS_OUTOFCTRL;
		}
		else
		{
			if(m_eCtrlState==EPACS_OUTOFCTRL||m_eCtrlState==EPACE_BLANDOUT)
				return;
			if(m_eCtrlState==EPACE_BLANDIN && m_CurBlandTime == 0)
			{
				m_eCtrlState = EPACS_OUTOFCTRL;
				return;
			}
				
			m_eCtrlState   = EPACE_BLANDOUT;
			m_BeginQuat	   = m_CurQuat;
		}
		m_CurBlandTime = 0;
		m_fDegree	   = 0.0f;
		m_EndQuat.setIdentity();
	};

	virtual void _DoControl( CMatrixUnit& CurMatrix, const CMatrixUnit& ParentMatrix ,const CSkeletalFrame& SkeFrame, CAnimate* pAnimate,float fCurFrame, uint32 uCurTime )
	{
		CQuaternion CurEnd = CurMatrix.m_FrameQuat * m_EndQuat;
		switch( m_eCtrlState )
		{
		case EPACS_OUTOFCTRL:
			return;
		case EPACE_BLANDIN:
			{
				if( m_CurBlandTime==0 && m_isNeedBegin)
				{
					m_BeginQuat = m_CurQuat = CurMatrix.m_FrameQuat;
					m_isNeedBegin = false;
				}

				m_CurBlandTime += (uCurTime - m_uCurTime);
				if(m_CurBlandTime<st_BlandTime)
				{
					float w = float(m_CurBlandTime)/float(st_BlandTime);
					CQuaternion::Slerp( m_CurQuat, m_BeginQuat, CurEnd,w );
				}
				else
				{
					m_CurBlandTime	= st_BlandTime;
					m_CurQuat		= CurEnd;
					m_eCtrlState	= EPACE_INCTRL;
				}
			}
			break;
		case EPACE_INCTRL:
			{
				m_CurQuat		= CurEnd;
			}
			break;
		case EPACE_BLANDOUT:
			{
				m_CurBlandTime += (uCurTime - m_uCurTime);
				if(m_CurBlandTime<st_BlandTime)
				{
					float w = float(m_CurBlandTime)/float(st_BlandTime);
					CQuaternion::Slerp( m_CurQuat,m_BeginQuat, CurEnd,w );
				}
				else
				{
					m_CurBlandTime	= st_BlandTime;
					m_CurQuat		= CurEnd;
					m_eCtrlState	= EPACS_OUTOFCTRL;
				}
			}
		}		
		CurMatrix.m_FrameQuat = m_CurQuat;
	}

protected:
	static const uint32		st_BlandTime;
	PartAniCtrlState		m_eCtrlState;		//混合出
	uint32					m_CurBlandTime;
	bool					m_isNeedBegin;
	float					m_fDegree;
	float					m_fHeight;
	CQuaternion				m_BeginQuat;
	CQuaternion				m_EndQuat;
	CQuaternion				m_CurQuat;
};

const uint32 CCharPartCtrl::st_BlandTime		= 450;

class CHeadController : public CCharPartCtrl //头部专属控制IK
{
public:
	CHeadController(  CCharacterController* CharCtrl,int32 SkeId )
		: CCharPartCtrl( CharCtrl,SkeId )
	{
		m_pInterestPos = &(CharCtrl->m_vecInterestedPos);
	}
protected:
	static const float	st_MaxYDegree;
	static const float	st_MinYDegree;
	CVector3f*	m_pInterestPos;

	void _DoControl( CMatrixUnit& CurMatrix, const CMatrixUnit& ParentMatrix ,const CSkeletalFrame& SkeFrame, CAnimate* pAnimate,float fCurFrame, uint32 uCurTime )
	{
		CQuaternion	tempQuat = m_EndQuat;

		if( m_eCtrlState == EPACE_BLANDIN || m_eCtrlState == EPACE_INCTRL )
		{
			CQuaternion tempY;
			CVector3f LookAt =  CurMatrix.m_FrameTrans*m_pModel->GetWorldMatrix() - (*m_pInterestPos);
			LookAt.normalize();
			float YDegree = CMath::ASin(LookAt.y);
			YDegree = limit2( YDegree, st_MinYDegree ,st_MaxYDegree );
			tempY.FromAngleAxis( YDegree , CVector3f::UNIT_X );
			m_EndQuat *= tempY; 
		}

		CCharPartCtrl::_DoControl( CurMatrix,ParentMatrix,SkeFrame,pAnimate,fCurFrame,uCurTime );
		m_EndQuat = tempQuat;
	}
};

const float CHeadController::st_MaxYDegree		= MAX_Y_MAX_DEGREES*CMath::pi/180.0f;
const float CHeadController::st_MinYDegree		= MAX_Y_MIN_DEGREES*CMath::pi/180.0f;


CCharacterController::CCharacterController(CCharacterFollower* pCharacter)
: m_pCharacter(pCharacter)
, m_pHeadCtrl(NULL)
, m_pSternumCtrl(NULL)
, m_pCoxalCtrl(NULL)
, m_isCtrlAll(true)
, m_isHasInterested(false)
, m_uTargetID(0)
{
	CModel* pModel = pCharacter->GetRenderObject();
	int32 SkeId;
	SkeId = pModel->GetSkeletalIdByName( st_HeadBoneName.c_str() );
	if( SkeId >= 0 )
	{
		m_pHeadCtrl = new CHeadController( this, SkeId );	
	}

	SkeId = pModel->GetSkeletalIdByName( st_SternumBoneName.c_str() );
	if( SkeId >= 0 )
		m_pSternumCtrl = new CCharPartCtrl( this, SkeId );

	SkeId = pModel->GetSkeletalIdByName("Bip SpinePro");
	if( SkeId >= 0 )
	{
		SkeId = pModel->GetSkeletalIdByName( st_CoxalBoneName.c_str() );
		if( SkeId >= 0 )
			m_pCoxalCtrl = new CCharPartCtrl( this, SkeId );
	}
}
CCharacterController::~CCharacterController()
{
	SafeDelete( m_pHeadCtrl );					//头部控制器
	SafeDelete( m_pSternumCtrl );				//胸部控制器
	SafeDelete( m_pCoxalCtrl );					//腰部控制器
}

void CCharacterController::SetInterestedObj(CCharacterFollower* pTargetObj)	//设置兴趣对象
{
	int SkeId;

	CRenderObject* pTargerRendObj = pTargetObj->GetRenderObject();
	CRenderObject* pRendObj= m_pCharacter->GetRenderObject();
	if (!pTargerRendObj||!pRendObj)
	{
		return;
	}
	CModel* pModel= pRendObj;
	if( (SkeId = pTargerRendObj->GetSkeletalIdByName(st_TargetBoneName.c_str())) >= 0 )
	{
		CMatrix tmpMat;
		pTargerRendObj->GetMatrix( tmpMat, uint32(SkeId), 0xffffffff );
		m_vecInterestedPos = tmpMat.GetLoc();
	}
	else
	{
		pTargerRendObj->GetRootPosition( &m_vecInterestedPos );
		if((SkeId = pModel->GetSkeletalIdByName(st_HeadBoneName.c_str())) >= 0)
		{
			CMatrix tmpMat;
			pModel->GetMatrix( tmpMat, uint32(SkeId), 0xffffffff );
			m_vecInterestedPos.y = tmpMat._42;
		}
	}

	CVector3f LookVec = -CVector3f::UNIT_Z;
	const CMatrix& wMat = pModel->GetWorldMatrix();
	LookVec.Rotate(wMat);
	CVector3f LookAt = wMat.GetLoc() - m_vecInterestedPos;
	LookVec.y = LookAt.y = 0.0f;
	LookVec.normalize();
	LookAt.normalize();
	float Degree = CMath::ACos(LookVec.Dot(LookAt));
	LookVec = LookVec.Cross(LookAt);

	if( Degree<= st_MaxLookDegree  )
	{   
		if(LookVec.y<0)
			Degree = - Degree;

		Degree = limit2(Degree,-st_MaxDegree,st_MaxDegree);
		
		if( m_uTargetID != pTargetObj->GetEntityID() )
		{
			m_uTargetID	= pTargetObj->GetEntityID();
			if(m_pHeadCtrl)
				m_pHeadCtrl->ChangeDirection(Degree*0.6f);
			if(m_isCtrlAll)
			{
				if(m_pSternumCtrl)
					m_pSternumCtrl->ChangeDirection(Degree*0.3f);
				if(m_pCoxalCtrl)
					m_pCoxalCtrl->ChangeDirection(Degree*0.1f);
			}
		}
		else
		{
			if(m_pHeadCtrl)
				m_pHeadCtrl->SetDirection(Degree*0.6f);
			if(m_isCtrlAll)
			{
				if(m_pSternumCtrl)
					m_pSternumCtrl->SetDirection(Degree*0.3f);
				if(m_pCoxalCtrl)
					m_pCoxalCtrl->SetDirection(Degree*0.1f);	
			}
		}
		m_isHasInterested	= true;
	}
	else
		ClearInterested( !m_isCtrlAll );	
}

bool CCharacterController::GetInterestedPos( CVector3f& outVec )		//获得兴趣点
{
	if(m_isHasInterested)
		outVec = m_vecInterestedPos;
	return m_isHasInterested;
}

void CCharacterController::ClearInterested(bool Imme)					//清除兴趣
{
	if(m_pHeadCtrl)
		m_pHeadCtrl->Clear(Imme);	
	if(m_pSternumCtrl)
		m_pSternumCtrl->Clear(Imme);
	if(m_pCoxalCtrl)
		m_pCoxalCtrl->Clear(Imme);		
	m_uTargetID = 0;
}