
#include "stdafx.h"

#include "CBTUpdater_Imp.h"
#include "CAsyncUpdater.h"

#include "IAsyncUpdateHandler.h"


CAsyncUpdater::CAsyncUpdater()
	: m_pImp(new CBTUpdater_Imp)
{
}

CAsyncUpdater::~CAsyncUpdater()
{
	delete m_pImp;
}

int CAsyncUpdater::DownloadPatch(IAsyncUpdateHandler* updater, const TCHAR* Host, const TCHAR* File, const TCHAR* szCurPath)
{
	return m_pImp->BeginDownloadPatchThread(updater, Host, File, szCurPath);
}


