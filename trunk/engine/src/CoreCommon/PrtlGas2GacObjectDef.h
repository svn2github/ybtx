#pragma once
#include "TBasePrtlMsg.h"
#include "PrtlGas2GacObjectDcl.h"
#include "TPackedPos.h"
#include "CLogCoreObject.h"


namespace sqr
{
#pragma pack(push,1)

	template<typename ImpClass,EGas2GacObjectCmdId uId>
	class TBaseGas2GacOM
		:public TBasePrtlMsg<ImpClass,uint8,uId>
	{
	};

#define Cmd_Begin(CommandName,CommandId) \
	class CommandName\
	:public TBaseGas2GacOM<CommandName,CommandId>\
	{\
	public:\
	static const char* GetName()\
	{\
	return #CommandName;\
	}

#define Cmd_End };


	/////////////////////////////////////////////////Protocol definition begin//////////////////////////////////////////////////////////

	Cmd_Begin( CGas2GacOC_Create_Still_Object, eGas2GacOCI_Create_Still_Object );

	uint32			uqbGlobalID;
	CPackedPosFloat	PixelPos;
	uint32			uqbDelay;
	uint8			usbTypeFlag:2;		//0:follower,1:active director,2:passive director
	uint8			usbVarDefCode;
	float			fBarrierSize;
	uint8			usbSelfBarrierType;
	uint64			uobDistortedFrameTime;

	Cmd_End;



	Cmd_Begin( CGas2GacOC_Create_Moving_Object, eGas2GacOCI_Create_Moving_Object );

	uint32			uqbGlobalID;
	CPackedPosFloat	BeginPixelPos;
	CPackedPosFloat	EndPixelPos;
	uint32			uqbDelay;
	float			fSpeed;
	float			fMovedDist;
	float			fEndDist;
	uint8			usbVarDefCode;
	float			fBarrierSize;
	uint8			usbSelfBarrierType;
	uint64			uobDistortedFrameTime;

	uint16			udbPathDataLen;
	uint32 GetPayloadSizeDerived()const{return udbPathDataLen;}
	Cmd_End;

	Cmd_Begin( CGas2GacOC_Check_Object_ViewVariant, eGas2GacOC_Check_Object_ViewVariant );
	uint32 uqbGlobalID;
	uint32 uBindParentID;
	uint32 uSessionID;
	uint32 uStaticVariantEncodeTimes;
	uint32 uAgileVariantEncodeTimes;
	Cmd_End;

	Cmd_Begin( CGas2GacOC_Sync_View_Variant_End, eGas2GacOCI_Sync_View_Variant_End );

	uint32	uqbGlobalID;
	uint32	uBindParentID;
	uint32	uStaticVariantEncodeTimes;
	uint32	uAgileVariantEncodeTimes;

	Cmd_End;
	
	Cmd_Begin( CGas2GacOC_Init_View_Variant, eGas2GacOC_Init_View_Variant );
	uint32	uqbGlobalID;
	uint32	uStaticEncodeTimes;
	uint32	uAgileEncodeTimes;
	Cmd_End;

	Cmd_Begin( CGas2GacOC_Init_Sync_Variant, eGas2GacOC_Init_Sync_Variant );
	uint32	uqbGlobalID;
	uint32	uStaticEncodeTimes;
	uint32	uAgileEncodeTimes;
	Cmd_End;

	Cmd_Begin( CGas2GacOCI_Director_Create_End, eGas2GacOCI_Director_Create_End );

	uint32	uqbGlobalID;
	float	fDirectorMaxSpeed;

	Cmd_End;

	Cmd_Begin( CGas2GacOC_Check_Object_SyncVariant, eGas2GacOC_Check_Object_SyncVariant );
	uint32 uqbGlobalID;
	uint32 uSessionID;
	uint32 uStaticVariantEncodeTimes;
	uint32 uAgileVariantEncodeTimes;
	Cmd_End;

	Cmd_Begin( CGas2GacOC_Entered_Sync_Aoi, eGas2GacOCI_Enter_Sync_Aoi );

	uint32	uqbGlobalID;
	uint32 uStaticVariantEncodeTimes;
	uint32 uAgileVariantEncodeTimes;
	Cmd_End;


	Cmd_Begin( CGas2GacOC_Left_Sync_Aoi, eGas2GacOCI_Leave_Sync_Aoi );

	uint32	uqbGlobalID;

	Cmd_End;


	Cmd_Begin( CGas2GacOC_Destroy_Object, eGas2GacOCI_Destroy_Object );

	uint32	uqbGlobalID;

	Cmd_End;

	Cmd_Begin( CGas2GacOC_Set_Time_Distorted_Ratio, eGas2GacOC_Set_Time_Distorted_Ratio );

	float	fRatio;

	Cmd_End;


	Cmd_Begin( CGas2GacOC_Set_Object_Pos, eGas2GacOCI_Set_Object_Pos);

	uint32			uqbGlobalID;
	CPackedPosFloat	PixelPos;

	Cmd_End;


	Cmd_Begin( CGas2GacOC_Sync_Object_Pos, eGas2GacOCI_Sync_Object_Pos);

	uint32	uqbGlobalID;

	Cmd_End;


	Cmd_Begin( CGas2GacOC_Set_Main_Scene, eGas2GacOCI_Set_Main_Scene );

	uint32	uqbSceneVer;
	uint8	usbSceneNameLen;
	float	fTimeDistortedRatio;

	uint32 GetPayloadSizeDerived()const
	{
		return usbSceneNameLen;
	}

	Cmd_End;



	Cmd_Begin( CGas2GacOC_Move_Follower_To, eGas2GacOCI_Move_Follower_To );

	uint32			uqbGlobalID;
	float			fSpeed;
	float			fEndDist;
	CPackedPosFloat	EndPixelPos;
#ifdef LOG_COREOBJ_MOVE
	CPackedPosFloat	BeginPixelPos;
#endif

	uint16			udbPathDataLen;
	uint32 GetPayloadSizeDerived()const{return udbPathDataLen;}
	Cmd_End;



	Cmd_Begin( CGas2GacOC_Change_Follower_Move_Target, eGas2GacOCI_Change_Follower_Move_Target );

	uint32			uqbGlobalID;
	float			fSpeed;
	float			fEndDist;
	float			fMovedDist;
	CPackedPosFloat	EndPixelPos;
#ifdef LOG_COREOBJ_MOVE
	CPackedPosFloat	BeginPixelPos;
	uint32			uqbSessionID;
#endif

	uint16			udbPathDataLen;
	uint32 GetPayloadSizeDerived()const{return udbPathDataLen;}
	Cmd_End;



	Cmd_Begin( CGas2GacOC_Change_Follower_Move_Speed, eGas2GacOCI_Change_Follower_Move_Speed );

	uint32	uqbGlobalID;
	float	fSpeed;

	Cmd_End;


	Cmd_Begin( CGas2GacOC_Tell_Follower_Stepped_SmallDist, eGas2GacOCI_Tell_Follower_Stepped_SmallDist );

	uint32	uqbGlobalID;
	uint8	uMovedDist;

	Cmd_End;

	Cmd_Begin( CGas2GacOC_Tell_Follower_Stepped_MiddleDist, eGas2GacOCI_Tell_Follower_Stepped_MiddleDist );

	uint32	uqbGlobalID;
	uint16	uMovedDist;

	Cmd_End;

	Cmd_Begin( CGas2GacOC_Tell_Follower_Stepped_BigDist, eGas2GacOCI_Tell_Follower_Stepped_BigDist );

	uint32	uqbGlobalID;
	float	fMovedDist;

	Cmd_End;


	Cmd_Begin( CGas2GacOC_Validate_Follower_Pos, eGas2GacOCI_Validate_Follower_Pos );

	uint32			uqbGlobalID;
	CPackedPosFloat	PixelPos;

	Cmd_End;


	Cmd_Begin( CGas2GacOC_Tell_Follower_Stopped, eGas2GacOCI_Tell_Follower_Stopped );

	uint32	uqbGlobalID;
	float	fMovedDist;

	Cmd_End;

	//Cmd_Begin( CGas2GacOC_Suspend_Follower_Moving, eGas2GacOCI_Suspend_Follower_Moving );

	//uint32	uqbGlobalID;
	//float	fMovedDist;

	//Cmd_End;

	//Cmd_Begin( CGas2GacOC_Resume_Follower_Moving, eGas2GacOCI_Resume_Follower_Moving );

	//uint32	uqbGlobalID;
	//uint32	uqbSuspendedTime;

	//Cmd_End;

	Cmd_Begin( CGas2GacOC_Tell_Follower_Reached, eGas2GacOCI_Tell_Follower_Reached );

	uint32			uqbGlobalID;
#ifdef LOG_COREOBJ_MOVE
	float			EndDist;
	CPackedPosFloat	ReachedPixelPos;
#endif

	Cmd_End;


	Cmd_Begin( CGas2GacOC_Enable_Follower_Delay, eGas2GacOCI_Enable_Follower_Delay );

	uint32	uqbGlobalID;
	uint32	uqbDelay;

	Cmd_End;

	Cmd_Begin( CGas2GacOC_Set_Dir_Max_Speed_And_Move_To, eGas2GacOCI_Set_Director_Active_And_Move_To );

	uint32	uqbGlobalID;
	uint32	uqbSessionID;
	float	fDirectorMaxSpeed;
	CPackedPosFloat	PixelPos;
	uint8	usbFindPathType:4;
	uint8	usbBarrierType:4;

	Cmd_End;

	Cmd_Begin( CGas2GacOC_Set_Director_Active, eGas2GacOCI_Set_Director_Active );

	uint32	uqbGlobalID;
	uint32	uqbSessionID;
	float	fDirectorMaxSpeed;

	Cmd_End;

	Cmd_Begin( CGas2GacOC_Set_Dir_Max_Speed, eGas2GacOCI_Set_Director_Max_Speed );

	uint32	uqbGlobalID;
	uint32	uqbSessionID;
	float	fDirectorMaxSpeed;

	Cmd_End;



	Cmd_Begin( CGas2GacOC_Set_Dir_Max_Speed_Zero, eGas2GacOCI_Set_Director_Passive );

	uint32			uqbGlobalID;
	CPackedPosFloat	PixelPos;

	Cmd_End;




	Cmd_Begin( CGas2GacOC_Static_Sync_Variant_Small_Data, eGas2GacOCI_Static_Sync_Variant_Small_Data );

	uint32	uqbGlobalID;
	uint8	usbDataLength;
	uint8	uEncodeTimes;

	uint32 GetPayloadSizeDerived()const{return usbDataLength;}
	uint8 GetEncodeTimes()const{return uEncodeTimes;}
	void SetDataLength(uint32 uDataLength) { usbDataLength = static_cast<uint8>(uDataLength); }

	Cmd_End;




	Cmd_Begin( CGas2GacOC_Static_Sync_Variant_Middle_Data, eGas2GacOCI_Static_Sync_Variant_Middle_Data );

	uint32	uqbGlobalID;
	uint16	udbDataLength;
	uint8	uEncodeTimes;

	uint32 GetPayloadSizeDerived()const{return udbDataLength;}
	uint8 GetEncodeTimes()const{return uEncodeTimes;}
	void SetDataLength(uint32 uDataLength) { udbDataLength = static_cast<uint16>(uDataLength); }

	Cmd_End;




	Cmd_Begin( CGas2GacOC_Static_Sync_Variant_Big_Data, eGas2GacOCI_Static_Sync_Variant_Big_Data );

	uint32	uqbGlobalID;
	uint32	uqbDataLength;
	uint8	uEncodeTimes;

	uint32 GetPayloadSizeDerived()const{return uqbDataLength;}
	uint8 GetEncodeTimes()const{return uEncodeTimes;}
	void SetDataLength(uint32 uDataLength) { uqbDataLength = uDataLength; }

	Cmd_End;


	Cmd_Begin( CGas2GacOC_Agile_Sync_Variant_Small_Data, eGas2GacOCI_Agile_Sync_Variant_Small_Data );

	uint32	uqbGlobalID;
	uint8	usbDataLength;
	uint8	uEncodeTimes;

	uint32 GetPayloadSizeDerived()const{return usbDataLength;}
	uint8 GetEncodeTimes()const{return uEncodeTimes;}
	void SetDataLength(uint32 uDataLength) { usbDataLength = static_cast<uint8>(uDataLength); }

	Cmd_End;




	Cmd_Begin( CGas2GacOC_Agile_Sync_Variant_Middle_Data, eGas2GacOCI_Agile_Sync_Variant_Middle_Data );

	uint32	uqbGlobalID;
	uint16	udbDataLength;
	uint8	uEncodeTimes;

	uint32 GetPayloadSizeDerived()const{return udbDataLength;}
	uint8 GetEncodeTimes()const{return uEncodeTimes;}
	void SetDataLength(uint32 uDataLength) { udbDataLength = static_cast<uint16>(uDataLength); }

	Cmd_End;




	Cmd_Begin( CGas2GacOC_Agile_Sync_Variant_Big_Data, eGas2GacOCI_Agile_Sync_Variant_Big_Data );

	uint32	uqbGlobalID;
	uint32	uqbDataLength;
	uint8	uEncodeTimes;

	uint32 GetPayloadSizeDerived()const{return uqbDataLength;}
	uint8 GetEncodeTimes()const{return uEncodeTimes;}
	void SetDataLength(uint32 uDataLength) { uqbDataLength = uDataLength; }

	Cmd_End;



	Cmd_Begin( CGas2GacOC_Static_View_Variant_Small_Data, eGas2GacOCI_Static_View_Variant_Small_Data );

	uint32	uqbGlobalID;
	uint8	usbDataLength;
	uint8	uEncodeTimes;

	uint32 GetPayloadSizeDerived()const{return usbDataLength;}
	uint8 GetEncodeTimes()const{return uEncodeTimes;}
	void SetDataLength(uint32 uDataLength) { usbDataLength = static_cast<uint8>(uDataLength); }

	Cmd_End;




	Cmd_Begin( CGas2GacOC_Static_View_Variant_Middle_Data, eGas2GacOCI_Static_View_Variant_Middle_Data );

	uint32	uqbGlobalID;
	uint16	udbDataLength;
	uint8	uEncodeTimes;

	uint32 GetPayloadSizeDerived()const{return udbDataLength;}
	uint8 GetEncodeTimes()const{return uEncodeTimes;}
	void SetDataLength(uint32 uDataLength) { udbDataLength = static_cast<uint16>(uDataLength); }

	Cmd_End;




	Cmd_Begin( CGas2GacOC_Static_View_Variant_Big_Data, eGas2GacOCI_Static_View_Variant_Big_Data );

	uint32	uqbGlobalID;
	uint32	uqbDataLength;
	uint8	uEncodeTimes;

	uint32 GetPayloadSizeDerived()const{return uqbDataLength;}
	uint8 GetEncodeTimes()const{return uEncodeTimes;}
	void SetDataLength(uint32 uDataLength) { uqbDataLength = uDataLength; }

	Cmd_End;


	Cmd_Begin( CGas2GacOC_Agile_View_Variant_Small_Data, eGas2GacOCI_Agile_View_Variant_Small_Data );

	uint32	uqbGlobalID;
	uint8	usbDataLength;
	uint8	uEncodeTimes;

	uint32 GetPayloadSizeDerived()const{return usbDataLength;}
	uint8 GetEncodeTimes()const{return uEncodeTimes;}
	void SetDataLength(uint32 uDataLength) { usbDataLength = static_cast<uint8>(uDataLength); }

	Cmd_End;




	Cmd_Begin( CGas2GacOC_Agile_View_Variant_Middle_Data, eGas2GacOCI_Agile_View_Variant_Middle_Data );

	uint32	uqbGlobalID;
	uint16	udbDataLength;
	uint8	uEncodeTimes;

	uint32 GetPayloadSizeDerived()const{return udbDataLength;}
	uint8 GetEncodeTimes()const{return uEncodeTimes;}
	void SetDataLength(uint32 uDataLength) { udbDataLength = static_cast<uint16>(uDataLength); }

	Cmd_End;




	Cmd_Begin( CGas2GacOC_Agile_View_Variant_Big_Data, eGas2GacOCI_Agile_View_Variant_Big_Data );

	uint32	uqbGlobalID;
	uint32	uqbDataLength;
	uint8	uEncodeTimes;

	uint32 GetPayloadSizeDerived()const{return uqbDataLength;}
	uint8 GetEncodeTimes()const{return uEncodeTimes;}
	void SetDataLength(uint32 uDataLength) { uqbDataLength = uDataLength; }

	Cmd_End;



	Cmd_Begin(CGas2GacOC_Sync_Dynamic_Barrier, eGas2GacOCI_Sync_Dynamic_Barrier);
	uint32	uqbGlobalID;
	float	fBarrierSize;
	uint8	usbBarrierType;
	//uint16 udbDataLen;
	//uint32 GetPayloadSizeDerived()const{return udbDataLen;}
	Cmd_End;

	Cmd_Begin(CGas2GacOC_Bind_To, eGas2GacOCI_Bind_To);

	uint32	uqbGlobalID;
	uint32	uParentID;
	bool	bRet;

	Cmd_End;

	Cmd_Begin(CGas2GacOC_Disbind_From_Parent, eGas2GacOCI_Disbind_From_Parent);
	uint32	uqbGlobalID;
	Cmd_End;
	
	Cmd_Begin(CGas2GacOC_Disbind_All_Children, eGas2GacOCI_Disbind_All_Children);
	uint32	uqbGlobalID;
	Cmd_End;




	/////////////////////////////////////////////////Protocol definition end////////////////////////////////////////////////////////////


#undef Cmd_End
#undef Cmd_Begin

#pragma pack(pop)
}


