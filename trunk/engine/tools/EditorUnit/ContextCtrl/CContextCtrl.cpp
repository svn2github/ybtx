#include "stdafx.h"
#include "CEditContext.h"
#include "ContextCtrl/CContextCtrl.h"
#include "CSceneBlockDebug.h"
#include "CSceneGridDeubg.h"
#include "PathManager/CPathManager.h"
#include "CDataScene.h"

DEF_UI_IMP(CContextCtrlImp);
CContextCtrlImp::CContextCtrlImp(void* param)
{
	m_pOwnContext = new CEditContext;
	m_pGridNode = NULL;
	m_pBlockNode = NULL;
}

CContextCtrlImp::~CContextCtrlImp(void)
{
	if(m_pOwnContext)
	{
		if(m_pGridNode)
			m_pOwnContext->RegistDebugNode(m_pGridNode);
		if(m_pBlockNode)
			m_pOwnContext->DelelteDebugNode(m_pBlockNode);
	}
	SafeDelete(m_pOwnContext);
}

CEditContext* CContextCtrlImp::GetContext( void )
{
	return m_pOwnContext;
}

bool	CContextCtrlImp::LoadScene(const char* path)
{
	if(m_pOwnContext && path)
	{
		string workDir;
		string ScenePath = path;
		if( CPathSetImp::GetInst()->GetResFileName(workDir,ScenePath) )
			m_pOwnContext->LoadScene(ScenePath);
	}
	return true;
}

const char*	CContextCtrlImp::GetSceneName(void)
{
	if(m_pOwnContext)
		return m_pOwnContext->GetDataScene()->GetSceneName();
	return NULL;
}

void CContextCtrlImp::ShowGrid( bool isShow )
{
	if(isShow)
	{
		if(m_pGridNode)
			return;
		m_pGridNode = new CSceneGridDebugNode(ROS_LIGHTMAT1,m_pOwnContext);
		m_pOwnContext->RegistDebugNode(m_pGridNode);
	}
	else
	{
		if(m_pGridNode)
		{
			m_pOwnContext->DelelteDebugNode(m_pGridNode);
			m_pGridNode = NULL;
		}
	}
}

void CContextCtrlImp::ShowBlock( bool isShow )
{
	if(isShow)
	{
		if(m_pBlockNode)
			return;
		m_pBlockNode = new CSceneBlockDebugNode(ROS_LIGHTMAT1,m_pOwnContext);
		m_pOwnContext->RegistDebugNode(m_pBlockNode);
	}
	else
	{
		if(m_pBlockNode)
		{
			m_pOwnContext->DelelteDebugNode(m_pBlockNode);
			m_pBlockNode = NULL;
		}
	}
}

unsigned __int32	CContextCtrlImp::GetAmbientColor(void)
{
	uint32 DefaultColor = 0xff808080;
	if(m_pOwnContext)
	{
		DefaultColor = m_pOwnContext->GetAmbientColor().ToARGB();
	}
	return DefaultColor;
}

unsigned __int32	CContextCtrlImp::GetSunLightColor(void)
{
	uint32 DefaultColor = 0xff808080;
	if(m_pOwnContext)
	{
		DefaultColor = m_pOwnContext->GetSunLightColor().ToARGB();
	}
	return DefaultColor;
}

uint32			CContextCtrlImp::GetSkyLightColor(void)
{
	return 0xff000000;
}

void			CContextCtrlImp::GetSunLightDir(float& X, float& Y, float& Z)
{
	CVector3f DefaultDir = CVector3f::UNIT_Y;
	if(m_pOwnContext)
	{
		DefaultDir = m_pOwnContext->GetSunLightDir();
	}
}

void			CContextCtrlImp::SetAmbientColor( uint32 color )
{
	if(m_pOwnContext)
	{
		m_pOwnContext->SetAmbientColor(color);
	}
}

void			CContextCtrlImp::SetSunLightColor( uint32 color )
{
	if(m_pOwnContext)
	{
		m_pOwnContext->SetSunLightColor(color);
	}
}

void	CContextCtrlImp::SetSkyLightColor( uint32 color )
{

}

void	CContextCtrlImp::SetSunLightDir( float X, float Y, float Z )
{
	if(m_pOwnContext)
	{
		m_pOwnContext->SetSunLightDir(CVector3f(X,Y,Z));
	}
}

bool CContextCtrlImp::IsShowGrid(void)
{
	return m_pGridNode != NULL;
}

bool CContextCtrlImp::IsShowBlock(void)
{
	return m_pBlockNode != NULL;
}

void CContextCtrlImp::SetContext( CEditContext* pContext )
{
	Ast(m_pOwnContext == pContext && "CContextCtrlImp 无法设置其他Context");
}