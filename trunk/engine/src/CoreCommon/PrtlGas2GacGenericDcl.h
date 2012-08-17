#pragma once
//$id$
//gas到gac的通用协议定义头文件

#include "PrtlGas2GacSegment.h"

namespace sqr
{
	enum EGas2GacGenericCmdId
	{
		eGas2GacGCI_Ping_Client =eGas2GacPS_Generic_Begin,

		eGas2GacGCI_Small_Shell_Message,
		eGas2GacGCI_Middle_Shell_Message,
		eGas2GacGCI_Big_Shell_Message,
		eGas2GacGCI_Tell_Server_Time,
		eCGas2GacGCI_Tell_Guids,
		
		eGas2GacGCI_Tell_Error,
		eGas2GacGCI_Tell_Current_Version,
		eGas2GacGCI_Tell_OwnerInfo,
		eGas2GacGenericCmd_Count
	};
}


