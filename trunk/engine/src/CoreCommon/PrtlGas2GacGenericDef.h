#pragma once
#include "TBasePrtlMsg.h"
#include "PrtlGas2GacGenericDcl.h"
#include "GuidTypes.h"


namespace sqr
{
#pragma pack(push,1)

	template<typename ImpClass,EGas2GacGenericCmdId uId>
	class TBaseGas2GacGM
		:public TBasePrtlMsg<ImpClass,uint8,uId>
	{
	};

#define Cmd_Begin(CommandName,CommandId) \
	class CommandName\
	:public TBaseGas2GacGM<CommandName,CommandId>\
	{\
	public:\
	static const char* GetName()\
	{\
	return #CommandName;\
	}

#define Cmd_End };


	/////////////////////////////////////////////////Protocol definition begin//////////////////////////////////////////////////////////



	Cmd_Begin( CGas2GacGC_Ping_Client, eGas2GacGCI_Ping_Client );
	Cmd_End;



	
	Cmd_Begin( CGas2GacGC_Tell_Server_Time, eGas2GacGCI_Tell_Server_Time );

	uint64	uobGlobalTime;

	Cmd_End;

	Cmd_Begin (CGas2GacGC_Tell_Guids, eCGas2GacGCI_Tell_Guids);
	
	guid_t guidServer;
	guid_t guidConnection;

	Cmd_End;

	Cmd_Begin( CGas2GacGC_Small_Shell_Message, eGas2GacGCI_Small_Shell_Message );

	uint8	usbDataLength;

	uint32 GetPayloadSizeDerived()const
	{
		return usbDataLength;
	}

	Cmd_End;



	Cmd_Begin( CGas2GacGC_Middle_Shell_Message, eGas2GacGCI_Middle_Shell_Message );

	uint16	udbDataLength;

	uint32 GetPayloadSizeDerived()const
	{
		return udbDataLength;
	}

	Cmd_End;
	



	Cmd_Begin( CGas2GacGC_Big_Shell_Message, eGas2GacGCI_Big_Shell_Message );

	uint32	uqbDataLength;

	uint32 GetPayloadSizeDerived()const
	{
		return uqbDataLength;
	}

	Cmd_End;

	Cmd_Begin( CGas2GacGC_Tell_Error, eGas2GacGCI_Tell_Error );
	
	uint16	udbDataLength;

	uint16 GetPayloadSizeDerived()const
	{
		return udbDataLength;
	}

	Cmd_End;

		
	Cmd_Begin( CGas2GacGC_Tell_Current_Version, eGas2GacGCI_Tell_Current_Version );
	
	uint32 uCurrentRevision;
	char uCurrentVersion[20];

	Cmd_End;

	Cmd_Begin( CGas2GacGC_Tell_OwnerInfo, eGas2GacGCI_Tell_OwnerInfo );

	uint8	usbDataLength;

	uint32 GetPayloadSizeDerived()const
	{
		return usbDataLength;
	} 

	Cmd_End;

	/////////////////////////////////////////////////Protocol definition end////////////////////////////////////////////////////////////


#undef Cmd_End
#undef Cmd_Begin

#pragma pack(pop)
}


