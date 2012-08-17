
DbProcessResult = {}
setmetatable(DbProcessResult, { __mode = "k" })

--用来表明某一个dbbox里面的某一个函数是local还是global的调用方式的
DbFuncTypeTbl = {}

DbFuncGlobalType = "Global"
DbFuncLocalType = "Local"

--表明这一个dbtrans是local还是global的调用方式的
DbTransEntryType = {}
setmetatable(DbTransEntryType, { __mode = "k" })

