--[[
	这个文件定义engine/common下面的所有导出类
--]]

engine_require "common/Script/ExportClass"

local ExportTbl = 
{
	{"CConnection", "common/Connection/Connection"},
	{"CContext", "common/Context/Context"},
	{"CContextMgr", "common/Context/ContextMgr"},
	{"CSyncObject", "common/CoSync/CSyncObject"},
	{"CEvent", "common/CoSync/CEvent"},
	{"CMsgQueue", "common/CoSync/CMsgQueue"},
	{"CSemaphore", "common/CoSync/CSemaphore"},
	{"CWaitForCoInfo", "common/CoSync/CWaitForCoInfo"},
	{"CCoSync", "common/CoSync/CoSync"},
	{"CDelegate", "common/Delegate/Delegate"},
	{"CList", "common/List/List"},
	{"CDir", "common/Math/CDir"},
	{"CFPos", "common/Math/CFPos"},
	{"CFRect", "common/Math/CFRect"},
	{"CIRect", "common/Math/CIRect"},
	{"CPos", "common/Math/CPos"},
	{"CVector3f", "common/Math/CVector3f"},
	{"CQueue", "common/Queue/Queue"},
	{"CLoadPtoBase", "common/Rpc/CLoadPtoBase"},
	{"CDefRegist", "common/Rpc/CDefRegist"},
	{"CCallRegist", "common/Rpc/CCallRegist"},
	{"CSleep", "common/Tick/CSleep"},
	{"CValueTable", "common/Value/ValueTable"},
}

SetExportClass("engine", ExportTbl)