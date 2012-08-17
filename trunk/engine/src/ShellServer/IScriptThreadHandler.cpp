#include "stdafx.h"
#include "IScriptThreadHandler.h"
#include "CFileWatcherData.h"

void IScriptThreadHandler::OnFileChanged(CFileWatcherData* pFileDataVec)
{
	CALL_CLASS_CALLBACK_1(pFileDataVec);
}
