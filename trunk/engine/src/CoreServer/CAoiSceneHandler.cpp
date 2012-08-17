#include "stdafx.h"
#include "CAoiSceneHandler.h"
#include "CSyncCoreSceneServer.h"

CAoiSceneHandler::CAoiSceneHandler(CSyncCoreSceneServer *pScene)
: m_pScene(pScene)
{

}

CAoiSceneHandler::~CAoiSceneHandler()
{

}

void CAoiSceneHandler::OnHalted()
{
	m_pScene->EndDestroyScene();
}

