engine_require "server/thread/StmtObjInc"

function CStmtObj:Release()
	self._m_Mgr:_ReleaseStatement(self._m_uIndex)
end

function CStmtObj:ExecStat(...)
	local _, result = self._m_Mgr:StatementExecute(self._m_uIndex, ...)
	
	return result	
end


local len = string.len

function CStmtObj:BatchExec( uExecTime , ...)
	local arg = {...}
	if nil == arg[1] then
		return
	end

	local report,result = self._m_Mgr:BatchStatementExec( self._m_uIndex, uExecTime, ...)	
	if len(report)~= 0 then
		error(report)
	end
end


--需要被淘汰的函数，这个函数传入参数类型在新的ExecStat里面已经是不需要的了
function CStmtObj:ExecSql(sType, ...)
	return self:ExecStat(...)
end

