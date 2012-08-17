#include "stdafx.h"
#include "CSceneLoadProcess.h"
#include "ExpHelper.h"
#include "CGraphic.h"

CSceneLoadProcess::CSceneLoadProcess( IProcessCallBack* pHandler )
: m_pHandler(pHandler)
{
	Ast(m_pHandler!=NULL && "IProcessCallBack 不能为 NULL");
}

void CSceneLoadProcess::Clear(void)
{
	m_fProcess = 0;
	m_pHandler->DoProcess(50);
}

void CSceneLoadProcess::DoProcess( float process )
{
	process *= 50.0f; 
	if (process - m_fProcess < 2.0f || process<0.0f)
		return;

	m_pHandler->DoProcess( min(process + 50,99) );
	m_fProcess = process;
}

// CCameraLoadProcess::CCameraLoadProcess( IProcessCallBack* pHandler )
// : m_pHandler(pHandler)
// {
// 	Ast(m_pHandler!=NULL && "IProcessCallBack 不能为 NULL");
// }
// 
// void CCameraLoadProcess::Clear(void)
// {
// 	m_fProcess = 0;
// 	m_pHandler->DoProcess(50);
// }
// 
// void CCameraLoadProcess::DoProcess( float process )
// {
// 	process *= 25.0f; 
// 	if (process - m_fProcess < 2.0f || process<0.0f)
// 		return;
// 
// 	m_pHandler->DoProcess( process + 50 );
// 	m_fProcess = process;
// } 