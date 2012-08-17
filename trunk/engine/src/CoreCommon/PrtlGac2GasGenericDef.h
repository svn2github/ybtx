#pragma once
#include "TBasePrtlMsg.h"
#include "PrtlGac2GasGenericDcl.h"
#include "BaseGac2GasPrtl.h"


namespace sqr
{
#pragma pack(push,1)

	template<typename ImpClass,EGac2GasGenericCmdId uId>
	class TBaseGac2GasGM
		:public TBasePrtlMsg<ImpClass,uint8,uId>
		,public BaseGac2GasPrtl
	{
	};

#define Cmd_Begin(CommandName,CommandId) \
	class CommandName\
	:public TBaseGac2GasGM<CommandName,CommandId>\
	{\
	public:\
	static const char* GetName()\
	{\
	return #CommandName;\
	}

#define Cmd_End };


	/////////////////////////////////////////////////Protocol definition begin//////////////////////////////////////////////////////////

	Cmd_Begin( CGac2GasGC_Client_Guid, eGac2GasGCI_Client_Guid);

	unsigned char guid[16];
	char szTime[20];

	Cmd_End;
	

	Cmd_Begin( CGac2GasGC_Ping_Server, eGac2GasGCI_Ping_Server );
	Cmd_End;



	Cmd_Begin( CGac2GasGC_Update_Latency_Server, eGac2GasGCI_Update_Latency_Server );
	Cmd_End;


	Cmd_Begin( CGac2GasGC_Main_Scene_Set, eGac2GasGCI_Main_Scene_Set );
	Cmd_End;

	

	Cmd_Begin( CGac2GasGC_Small_Shell_Message, eGac2GasGCI_Small_Shell_Message );

	uint16	udbSeed;
	uint8	usbCounter;
	uint8	usbModCheckSum;
	uint8	usbDataLength;

	uint32 GetPayloadSizeDerived()const
	{
		return usbDataLength;
	}

	Cmd_End;


	Cmd_Begin( CGac2GasGC_Middle_Shell_Message, eGac2GasGCI_Middle_Shell_Message );

	uint16	udbSeed;
	uint8	usbCounter;
	uint8	usbModCheckSum;
	uint16	udbDataLength;

	uint32 GetPayloadSizeDerived()const
	{
		return udbDataLength;
	}

	Cmd_End;


	Cmd_Begin( CGac2GasGC_Tell_BeTesting, eGac2GasGCI_Tell_BeTesting);
	
	Cmd_End;

	//Cmd_Begin( CGac2GasGC_Tell_OwnerInfo, eGac2GasGCI_Tell_OwnerInfo );

	//uint8	usbDataLength;

	//uint32 GetPayloadSizeDerived()const
	//{
	//	return usbDataLength;
	//} 

	//Cmd_End;

	//Cmd_Begin( CGac2GasGC_Update_Server_Known_Time, eGac2GasGCI_Update_Server_Known_Time);
	//Cmd_End;
	/////////////////////////////////////////////////Protocol definition end////////////////////////////////////////////////////////////


#undef Cmd_End
#undef Cmd_Begin

#pragma pack(pop)
}


