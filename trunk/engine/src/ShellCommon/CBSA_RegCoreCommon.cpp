#include "stdafx.h"
#include "CBaseScriptApp.h"
#include "ScriptX.h"
#include "CoreObjectDefs.h"
#include "FindPathDefs.h"
#include "CPixelPath.h"
#include "CoreCommon.h"
#include "CAppEvent.h"
#include "CFileWatcherData.h"
#include "TSqrAllocator.inl"

int32 CBaseScriptApp::RegisterCoreCommon(CScript& Script)
{
	REGIST_ENUMTYPE( Script, EMoveToResult );
	REGIST_ENUMERATION( Script, EMoveToResult, eMTR_Success );
	REGIST_ENUMERATION( Script, EMoveToResult, eMTR_SuperPosition );
	REGIST_ENUMERATION( Script, EMoveToResult, eMTR_NotAllowed );
	REGIST_ENUMERATION( Script, EMoveToResult, eMTR_FindPathFailed );

	REGIST_ENUMTYPE( Script, EStopMovingResult );
	REGIST_ENUMERATION( Script, EStopMovingResult, eSMR_Success );
	REGIST_ENUMERATION( Script, EStopMovingResult, eSMR_IsNotMoving );
	REGIST_ENUMERATION( Script, EStopMovingResult, eSMR_NotAllowed );

	REGIST_ENUMTYPE( Script, ESetPosResult );
	REGIST_ENUMERATION( Script, ESetPosResult, eSPR_Success );
	REGIST_ENUMERATION( Script, ESetPosResult, eSPR_IsMoving );
	REGIST_ENUMERATION( Script, ESetPosResult, eSPR_NotAllowed );
	REGIST_ENUMERATION( Script, ESetPosResult, eSPR_OutOfBound );

	//寻路相关的枚举变量
	REGIST_ENUMTYPE( Script, EFindPathType );
	REGIST_ENUMERATION( Script, EFindPathType, eFPT_Line );
	REGIST_ENUMERATION( Script, EFindPathType, eFPT_AStar );
	REGIST_ENUMERATION( Script, EFindPathType, eFPT_HypoLine );
	REGIST_ENUMERATION( Script, EFindPathType, eFPT_HypoAStar );
	REGIST_ENUMERATION( Script, EFindPathType, eFPT_TerrainAStar );
	REGIST_ENUMERATION( Script, EFindPathType, eFPT_LineIgnoreEnd );
	REGIST_ENUMERATION( Script, EFindPathType, eFPT_AStarIgnoreEnd );
	REGIST_ENUMERATION( Script, EFindPathType, eFPT_HypoAStarOpti );
	REGIST_ENUMERATION( Script, EFindPathType, eFPT_AStarUseWeight );
	REGIST_ENUMERATION( Script, EFindPathType, eFPT_HypoAStarOptiUseWeight );

	REGIST_ENUMTYPE( Script, EBarrierType );
	REGIST_ENUMERATION( Script, EBarrierType, eBT_OutRange );
	REGIST_ENUMERATION( Script, EBarrierType, eBT_NoBarrier );
	REGIST_ENUMERATION( Script, EBarrierType, eBT_LowBarrier );
	REGIST_ENUMERATION( Script, EBarrierType, eBT_MidBarrier );
	REGIST_ENUMERATION( Script, EBarrierType, eBT_HighBarrier );

	REGIST_ENUMTYPE( Script, EUnits );
	REGIST_ENUMERATION( Script, EUnits, eGridSpan );
	REGIST_ENUMERATION( Script, EUnits, eGridSpanForObj );
	REGIST_ENUMERATION( Script, EUnits, eRegionSpan );
	REGIST_ENUMERATION( Script, EUnits, eGridNumPerRegion );
	REGIST_ENUMERATION( Script, EUnits, eVertexNumPerRegion );

	REGIST_ABSTRACT_B_CLASS( Script, CPixelPath );


	REGIST_CLASSFUNCTION( Script, CPixelPath, Release, "" );
	REGIST_CLASSFUNCTION( Script, CPixelPath, Clone, ":(CPixelPath)" );
	REGIST_CLASSFUNCTION( Script, CPixelPath, GetBeginPixelPos, "(CFPos)" );
	REGIST_CLASSFUNCTION( Script, CPixelPath, GetEndPixelPos, "(CFPos)" );
	REGIST_CLASSFUNCTION( Script, CPixelPath, GetPathSize, ":t" );
	REGIST_CLASSFUNCTION( Script, CPixelPath, GetWayPointPixelPosOfPath, "(CFPos)H:b" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CPixelPath, uint16 (CPixelPath::*)(CFPos&,float)const,GetPixelPosOfPath,GetPixelPosOfPath,"(CFPos)f:H" );
	REGIST_CLASSFUNCTION( Script, CPixelPath, GetPathPixelLength, ":f" );
	REGIST_CLASSFUNCTION( Script, CPixelPath, PrintPath, "" );

	REGIST_ABSTRACT_B_CLASS( Script, CFileWatcherData );
	REGIST_CLASSFUNCTION( Script, CFileWatcherData, GetFileName, "I:s");
	REGIST_CLASSFUNCTION( Script, CFileWatcherData, GetIsLuaFile, ":b");
	REGIST_CLASSFUNCTION( Script, CFileWatcherData, GetCount, ":I");

	return true;
}

