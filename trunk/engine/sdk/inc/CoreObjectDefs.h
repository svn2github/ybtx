#pragma once


namespace sqr
{
	//SetSpeed函数的返回值
	enum ESetSpeedResult {eSSR_Success,eSSR_IsNotMoving,eSSR_InvalidSpeed,eSSR_NotAllowed};
	//MoveTo函数的返回值
	enum EMoveToResult {eMTR_Success,eMTR_FindPathFailed,eMTR_SuperPosition,eMTR_NotAllowed};

	enum ETraceResult {eTR_Success,eTR_MoveFailed,eTR_NotAllowed};
	enum ETraceEndedType { eTET_Traced, eTET_Stopped, eTET_Failed };

	//watch功能的返回值
	enum EWatchResult {eWR_InScope, eWR_NotInScope};

	//StopMoving函数的返回值
	enum EStopMovingResult
	{
		eSMR_Success		=	eSSR_Success,
		eSMR_IsNotMoving	=	eSSR_IsNotMoving,
		eSMR_NotAllowed		=	eSSR_NotAllowed
	};

	//SetPos以及SetGridPos的返回值
	enum ESetPosResult {eSPR_Success,eSPR_IsMoving,eSPR_NotAllowed,eSPR_OutOfBound};

	//OnMoveMoveEnded的类型，到达目标点为Success，由StopMoving中止为Stopped
	enum EMoveEndedType {eMET_Reached,eMET_Stopped};

	enum EStillObjFlag{ eSOF_Follower,eSOF_Active_Director,eSOF_Passive_Director };

#define DIRECTOR_SEND_MOVED_DIST
}
