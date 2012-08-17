#pragma once

#include "CScript.h"
#include "Cache.h"
#include "MapView.h"
#include "NewSceneEditorDlg.h"
#include "OriginalData.h"
#include "ModeChoiceDlg.h"

extern CScript*	g_pScript;
extern CCache* g_pCache;
extern CMapView* g_pMapView;
extern EEideModeType g_eModeType;
extern CNewSceneEditorDlg* g_pMainDlg;
extern COriginalData* g_pOriginalData;
CScript*	GetScript();
CCache&		GetCCache();


COriginalData& GetOriginalData();
void	Init();
int32	InterfaceExport();
void	RunLuaFile(const TCHAR* szContext);
void	ReleaseGlobalPtr(void);

using namespace sqr;