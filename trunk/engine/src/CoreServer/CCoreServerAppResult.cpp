#include "stdafx.h"
#include "CCoreServerResult.inl"
#include "CCoreServerAppResult.h"
#include "CAppServer.h"

CCoreServerAppQuitResult::CCoreServerAppQuitResult()
{
}

CCoreServerAppQuitResult::~CCoreServerAppQuitResult()
{
	CAppServer::Inst()->IntQuit();
}

CCoreServerAppCleanResult::CCoreServerAppCleanResult()
{
}

CCoreServerAppCleanResult::~CCoreServerAppCleanResult()
{
	CAppServer::Inst()->IntCleanUp();
}

