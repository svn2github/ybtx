#include "stdafx.h"
#include "CAnimationController.h"
#include "CModel.h"

CAnimationController::CAnimationController(void)
 : m_pModel(NULL)
 , m_SkeId(-1)
 , m_uCurTime(0)
{

}

CAnimationController::CAnimationController( CModel* pModel, int32 SkeId )
{
	AstMsg( (!(SkeId<0 || pModel==NULL || SkeId > pModel->m_RenderMatrix.m_MaxUseSkeletalID)), "AddPiece并且资源加载完成后再能创建" );
	m_pModel	= pModel;
	m_SkeId		= SkeId;
	m_pModel->m_RenderMatrix.m_pAniCtrlList[m_SkeId] = this;
	m_uCurTime	= 0;
}

void CAnimationController::SetCtrlModel( CModel* pModel, int32 SkeId )
{
	Release();
	AstMsg( (!(SkeId<0 || pModel==NULL || SkeId > pModel->m_RenderMatrix.m_MaxUseSkeletalID)), "AddPiece并且资源加载完成后再能创建" );
	m_pModel	= pModel;
	m_SkeId		= SkeId;
	m_pModel->m_RenderMatrix.m_pAniCtrlList[m_SkeId] = this;
	m_uCurTime	= 0;
}

void CAnimationController::DoControl( CMatrixUnit& CurMatrix , const CMatrixUnit& ParentMatrix,const CSkeletalFrame& SkeFrame,CAnimate* pAnimate,float fCurFrame,uint32 uCurTime )
{
	if( NULL != m_pModel && -1 != m_SkeId )
	{
		if(m_uCurTime == 0)
			m_uCurTime = uCurTime;
		SQR_TRY	
		{
			
			_DoControl( CurMatrix, ParentMatrix, SkeFrame,pAnimate,fCurFrame,uCurTime );
		}
		SQR_CATCH(exp)
		{
			exp.AppendMsg("骨骼动画控制器发生异常:");
			GfkLogExp(exp);
		}
		SQR_TRY_END;
		m_uCurTime = uCurTime;
	}
}

void CAnimationController::Release()
{
	if( NULL != m_pModel && m_pModel->m_RenderMatrix.m_pAniCtrlList[m_SkeId] == this )
		m_pModel->m_RenderMatrix.m_pAniCtrlList[m_SkeId] = NULL;
	m_pModel = NULL;
	m_SkeId	 = -1;
	m_uCurTime= 0;
}

CAnimationController::~CAnimationController()
{
	Release();
}