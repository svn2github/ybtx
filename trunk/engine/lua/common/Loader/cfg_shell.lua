--一个简单配置表外壳,  目前仅仅允许修改配置表现有的属性, 
--按需求在慢慢扩展功能

CCfgShell = class()

function CCfgShell:Ctor(cfg)
	self.Cfg = cfg
	self.ExcessData = {}
end


function CCfgShell:GetKeys(...)
	return self.Cfg:GetKeys(...)
end

function CCfgShell.__call(tbl, index, ...)
	local v = tbl.Cfg(index)
	local t = type(v)
	if t == "nil" or t == "number" or t == "string" then
		return tbl.ExcessData[index] or v
	end
	local innerShell = CCfgShell:new(v)
	innerShell.ExcessData = tbl.ExcessData[index] or {}
	if select("#", ...) > 0 then
		return innerShell(...)
	else
		return innerShell
	end
end

function CCfgShell:UpdateAttribute(index, attribute, value)
	
	local row = self.ExcessData[index]
	if not row then
		self.ExcessData[index] = {}
		row = self.ExcessData[index]
	end
	row[attribute] = value
end

function CreateCfgShell(shellName, cfg)
	assert(_G[shellName] == nil)
	_G[shellName] = CCfgShell:new(cfg)
end