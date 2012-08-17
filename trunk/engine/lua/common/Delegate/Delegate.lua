CDelegate = class()

-- 想不到这个函数该叫什么，就用operator()好了

CDelegate.__call = function(tbl, ...)
	for k,v in pairs(tbl.m_List) do
		v(...)
	end
end

function CDelegate:Ctor()
	self.m_List = { }
end

function CDelegate:DoFun(...)
	for k,v in pairs(self.m_List) do
		v(...)
	end
end

function CDelegate:Add(func)
	table.insert(self.m_List, func)
end

function CDelegate:Remove(func)
	for k,v in pairs(self.m_List) do
		if (v == func) then
			self.m_List[k] = nil
			return
		end
	end
end

function CDelegate:RemoveAll()
	for k,v in pairs(self.m_List) do
		self.m_List[k] = nil
	end
end

function CDelegate:IsEmpty()
	return #self.m_List == 0
end
