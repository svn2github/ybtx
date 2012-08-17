EProtocol = class(IPipeHandler)
EProMsgRecv = class(IPipeHandler)
EProMsgRecvHandler = class(IPipeHandler)

g_AGIP_Sequence_ID = 1
size_msg_header = 20
size_msg_checksum = 4
g_tbl_AGIP_CallBack = {}
g_tbl_AGIP_Tick = {}

g_tbl_AGIP_CMD_Handler = {}

UserLoginRetCode = 
{
	LoginSucc = 1,
	LoginFail = 2,
	LocalLogin = 3,
}

ERatingConfig = {}
