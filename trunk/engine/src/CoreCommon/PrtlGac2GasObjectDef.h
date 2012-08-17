#pragma once
#include "TBasePrtlMsg.h"
#include "PrtlGac2GasObjectDcl.h"
#include "TPackedPos.h"
#include "BaseGac2GasPrtl.h"
#include "CoreObjectDefs.h"


namespace sqr
{
#pragma pack(push,1)

	template<typename ImpClass,EGac2GasObjectCmdId uId>
	class TBaseGac2GasOM
		:public TBasePrtlMsg<ImpClass,uint8,uId>
		,public BaseGac2GasPrtl
	{
	};

#define Cmd_Begin(CommandName,CommandId) \
	class CommandName\
	:public TBaseGac2GasOM<CommandName,CommandId>\
	{\
	public:\
	static const char* GetName()\
	{\
	return #CommandName;\
	}

#define Cmd_End };


	/////////////////////////////////////////////////Protocol definition begin//////////////////////////////////////////////////////////



	Cmd_Begin( CGac2GasOC_Move_Mediator_To,eGac2GasOCI_Move_Mediator_To );
	
	CPackedPosFloat	PixelPos;
	uint8			usbBarrierType:4;
	uint8			usbFindPathType:4;

	CPackedPosFloat	EndPixelPos;
	
	float			fSpeed;
	float			fEndDist;

	uint16			udbPathLen;
	uint32 GetPayloadSizeDerived()const{return udbPathLen;}
	
	uint16			udbSeed;
	uint8			usbModCheckSum;
	uint8			usbCounter;

	Cmd_End;

	Cmd_Begin( CGac2GasOC_Change_Mediator_Move_Target,eGac2GasOCI_Change_Mediator_Move_Target );
	float			fSpeed;
#ifdef DIRECTOR_SEND_MOVED_DIST	
	float			fMovedDist;
#endif
	CPackedPosFloat	EndPixelPos;
	uint16			udbPathLen;

	uint32			uqbTimeStamp;

	CPackedPosFloat	BeginPixelPos;
	float			fEndDist;
	uint8			usbFindPathType:4;
	uint8			usbBarrierType:4;
	uint32 GetPayloadSizeDerived()const{return udbPathLen;}

	uint16			udbSeed;
	uint8			usbModCheckSum;
	uint8			usbCounter;

	Cmd_End;


	Cmd_Begin( CGac2GasOC_Change_Mediator_Move_Speed,eGac2GasOCI_Change_Mediator_Move_Speed );

	float	fSpeed;	

	uint16	udbSeed;
	uint8	usbModCheckSum;
	uint8	usbCounter;
	
	Cmd_End;


	Cmd_Begin( CGac2GasOC_Tell_Mediator_Stepped,eGac2GasOCI_Tell_Mediator_Stepped );

#ifdef DIRECTOR_SEND_MOVED_DIST	
	float	fMovedDist;
#endif

	uint16	udbSeed;
	uint8	usbModCheckSum;
	uint8	usbCounter;
	
	Cmd_End;


	Cmd_Begin( CGac2GasOC_Tell_Mediator_Stopped,eGac2GasOCI_Tell_Mediator_Stopped );

	uint32			uqbTimeStamp;
	CPackedPosFloat	EndPixelPos;
#ifdef DIRECTOR_SEND_MOVED_DIST	
	float			fMovedDist;
#endif

	uint16	udbSeed;
	uint8	usbModCheckSum;
	uint8	usbCounter;
	
	Cmd_End;


	Cmd_Begin( CGac2GasOC_Tell_Mediator_Reached,eGac2GasOCI_Tell_Mediator_Reached );

	uint32			uqbTimeStamp;
	CPackedPosFloat	EndPixelPos;

	uint16	udbSeed;
	uint8	usbModCheckSum;
	uint8	usbCounter;
	
	Cmd_End;


	Cmd_Begin( CGac2GasOC_Director_Is_Activated,eGac2GasOCI_Notify_Mediator_Run_Mode_Switched );
	Cmd_End;


	Cmd_Begin( CGac2GasOC_Report_Invalid_Step,eGac2GasOCI_Report_Invalid_Step);
		char s[100];

		uint16	udbSeed;
		uint8	usbModCheckSum;
		uint8	usbCounter;
	
	Cmd_End;


	Cmd_Begin( CGac2GasOC_Report_Invalid_Move,eGac2GasOCI_Report_Invalid_Move);
	Cmd_End;

	Cmd_Begin( CGac2GasOC_Tell_Mediator_SetPosition, eGac2GasOCI_Tell_Mediator_SetPosition );

	CPackedPosFloat	PixelPos;	

	Cmd_End;

	Cmd_Begin( CGac2GasOC_Tell_Mediator_Max_Speed_Set,eGac2GasOCI_Tell_Mediator_Max_Speed_Set);
#ifdef DIRECTOR_SEND_MOVED_DIST	
	float	fMovedDist;
#endif
	uint32 uqbSessionID;
	uint16	udbSeed;
	uint8	usbModCheckSum;
	uint8	usbCounter;
	Cmd_End;

	Cmd_Begin( CGac2GasOC_Tell_Mediator_Sync_Target_ViewVariant,eGac2GasOC_Tell_Mediator_Sync_Target_ViewVariant);
	bool	bSyncAgileView;
	uint32	uSessionID;
	bool	bSyncStaticView;
	uint32	uTargetGlobalID;
	Cmd_End;

	Cmd_Begin( CGac2GasOC_Tell_Mediator_Sync_Target_SyncVariant,eGac2GasOC_Tell_Mediator_Sync_Target_SyncVariant);
	bool	bSyncStaticSync;
	bool	bSyncAgileSync;
	uint32	uTargetGlobalID;
	uint32	uSessionID;
	Cmd_End;

	/////////////////////////////////////////////////Protocol definition end////////////////////////////////////////////////////////////


#undef Cmd_End
#undef Cmd_Begin

#pragma pack(pop)
}


