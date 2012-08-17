#include "stdafx.h"
#include "IAppServerHandler.h"
#include "CFileWatcherData.h"


void IAppServerHandler::OnFileChanged(CFileWatcherData* pFileDataVec)
{
	CALL_CLASS_CALLBACK_1(pFileDataVec); 
}
