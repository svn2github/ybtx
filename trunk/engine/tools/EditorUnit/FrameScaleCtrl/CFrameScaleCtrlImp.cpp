#include "StdAfx.h"
#include "CEditContext.h"
#include "FrameScaleCtrl/CFrameScaleCtrlImp.h"
#include "FrameScaleCtrl/CFrameScaleDelegate.h"
#include "CEditPieceGroup.h"
#include "CAnimationGroup.h"
#include "CEditAnimationsRes.h"
#include "CEditSkeletalsMgr.h"
#include "CAnimation.h"
#include "CEditModel.h"
#include "CModel.h"
#include "ContextMsg.h"

#define MODEL ((CModel*)m_pOwnContext->GetActiveObj())
#define EDITMODEL ((CEditModel*)m_pOwnContext->GetActiveObj())

DEF_UI_IMP(CFrameScaleCtrlImp);
CFrameScaleCtrlImp::CFrameScaleCtrlImp(void* param)
: m_pKeyFrames(NULL)
, m_iMaxFrameNum(0)
, m_iKeyFrameNum(0)
, m_pDelegate(NULL)
{
	m_nScaleLen			= 0;
	m_nFrameStart		= 0;
	m_nFrameEnd			= 0;
	m_fFrameCur			= 0.0f;
	m_iFrameCur			= 0;
	m_nFramesPerGrid	= 0;
	m_nPixPerGrid		= 0;
	m_bPlay				= false;
}

CFrameScaleCtrlImp::~CFrameScaleCtrlImp(void)
{
	if (m_pDelegate)
	{
		delete m_pDelegate;
		m_pDelegate = NULL;
	}
}

void CFrameScaleCtrlImp::Update()
{
	if ( m_iFrameCur != (int)GetUpdateFrame())
	{
		m_iFrameCur = (int)GetUpdateFrame();
		//通知keyframe控件
		MSG_SYS_SELCHANGE msg(NULL);
		m_pOwnContext->EventCollect(msg);
	}
	CCtrlBase::Update();
}


void sqr_tools::CFrameScaleCtrlImp::SetAniEndFrame( const int end )
{
	m_nFrameEnd = end;
	this->CalFramePerGrid();
}

void sqr_tools::CFrameScaleCtrlImp::CalFramePerGrid()
{
	int FrameNum = m_nFrameEnd - m_nFrameStart + 1;

	//计算合适的每帧象素
	static int FramesPerGrid[3] = { 1, 2, 5 };
	int Start = 1;
	int n = 0, nFramesPerGrid = 0;
	m_nPixPerGrid = 0;
	while( 1 )
	{
		nFramesPerGrid = ( FramesPerGrid[n%3] * Start );
		m_nPixPerGrid = m_nScaleLen * nFramesPerGrid / FrameNum;//m_nScaleLen/( FrameNum/( FramesPerGrid[n%3]*Start ) )
		if( m_nPixPerGrid > 5 )
			break;

		++n;
		if( n == 3 )
		{
			Start *= 10; 
			n = 0;
		}
	}
	m_nFramesPerGrid = nFramesPerGrid;

	if( m_nFramesPerGrid == 0 )
		m_nFramesPerGrid = 1;

	if( m_nPixPerGrid == 0 )
		m_nPixPerGrid = 1;

	Update();
}

void sqr_tools::CFrameScaleCtrlImp::CalCurrentFrame(const float x)
{
	m_fFrameCur = ( x / m_nPixPerGrid ) * m_nFramesPerGrid;
	
	if( m_fFrameCur < 0.0f )
	{
		m_fFrameCur = 0.0f;
	}

	if( m_fFrameCur > (float)m_nFrameEnd )
	{
		m_fFrameCur = (float)m_nFrameEnd;
	}

	if (MODEL)
		((CEditModel*)MODEL)->SetFrame(m_fFrameCur);
}

void sqr_tools::CFrameScaleCtrlImp::Play( void )
{
	m_bPlay = true;
	if (MODEL)
		MODEL->ResumeAni(-1);
	//通知groundSpeed
	MSG_SYS_SELCHANGE msg(NULL);
	m_pOwnContext->EventCollect(msg);
}

void sqr_tools::CFrameScaleCtrlImp::Stop( void )
{
	m_bPlay = false;
	if (MODEL)
		MODEL->StopAni(-1);
	//通知groundSpeed
	MSG_SYS_SELCHANGE msg(NULL);
	m_pOwnContext->EventCollect(msg);
}

float sqr_tools::CFrameScaleCtrlImp::GetUpdateFrame( void )
{
	if (IsPlay())
	{
		if (MODEL)
		{
			int num = MODEL->GetAnimateMaxFrameNum();
			if ( num == 0 )
				return GetCurFrame();
			else
				return ((CEditModel*)MODEL)->GetCurFrame();
		}
		else
			return 0.0f;
	} 
	else
	{
		return GetCurFrame();
	}
}

//bool sqr_tools::CFrameScaleCtrlImp::MsgProc( const CMsg& msg )
//{
//	switch( msg.msgSource )
//	{
//	case MGS_SYSMSG:
//		{
//			switch(msg.msgType.sys_type)
//			{
//			case MGT_SYS_RENDER:
//				Update();
//				break;
//			case MGT_SYS_SELCHANGE:
//				ActiveNode();
//				break;
//			}
//		}
//		break;
//	}
//	return true;
//}

void sqr_tools::CFrameScaleCtrlImp::SetContext( CEditContext* pContext )
{
	CCtrlBaseObserver::SetContext(pContext);

	if (m_pDelegate)
		return;
	m_pDelegate = new CFrameScaleDelegate(this);
	m_pOwnContext->RegisterDelegate(m_pDelegate);
	m_pOwnContext->ActiveDelegate(m_pDelegate);
}

void sqr_tools::CFrameScaleCtrlImp::Resize( void )
{

}

void sqr_tools::CFrameScaleCtrlImp::ActiveNode( CRenderNode* pNode )
{
	if (MODEL)
	{
		int keyFrameNum = 0;
		if (( map<WORD,sqr::IDNAME>* )EDITMODEL->GetCurFrameString())
			keyFrameNum = (( map<WORD,sqr::IDNAME>* )EDITMODEL->GetCurFrameString())->size();

		if ( m_pKeyFrames != ( map<WORD,sqr::IDNAME>* )EDITMODEL->GetCurFrameString() 
			|| m_iMaxFrameNum != MODEL->GetAnimateMaxFrameNum() 
			|| m_iKeyFrameNum != keyFrameNum)
		{
			m_pKeyFrames = ( map<WORD,sqr::IDNAME>* )EDITMODEL->GetCurFrameString();
			m_iMaxFrameNum = MODEL->GetAnimateMaxFrameNum();
			m_iKeyFrameNum = keyFrameNum;
			ClearKeyFrame();
			map<WORD,sqr::IDNAME>* KeyFrame = NULL;
			KeyFrame = ( map<WORD,sqr::IDNAME>* )EDITMODEL->GetCurFrameString();
			if( KeyFrame )
			{
				string keyName = "";
				for( map<WORD,sqr::IDNAME>::iterator it = KeyFrame->begin(); it != KeyFrame->end(); ++it )
				{
					WORD keyFrame = it->first;
					keyName = it->second;
					SetKeyFrame( keyFrame );
				}
			}	
			int num = MODEL->GetAnimateMaxFrameNum();
			CalCurrentFrame(0.0f);
			SetAniEndFrame(num);
		}
	}
}