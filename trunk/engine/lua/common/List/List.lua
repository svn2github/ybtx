CList = class()
CList.IterMT = {}
CList.IterMT.__index = {}

function CList:push_front( object )
	self:insert( self:head() , object )
end

function CList:pop_front()
	self:erase( self:head() )
end

function CList:push_back(object)
	self:insert( self:tail() , object )
end

function CList:pop_back()
	local Node = self:tail()
	Node:dec()
	self:erase( Node )
end

function CList:erase( iterator_where, iterator_end )
	local iter_where = self:Iterator( iterator_where )
	local iter_end
	if( iterator_end == nil ) then
		iter_end = CList:Iterator( iter_where )
		iter_end:inc()
	else
		if( iterator_end == nil ) then
			iter_end = self:tail()
		else
			iter_end = CList:Iterator( iterator_end )
		end
	end
	
	while( iter_where ~= iter_end ) do
		local Node = iter_where:node()
		if( self._count > 0 and Node ~= self._end ) then
			Node._pre._next		= Node._next
			Node._next._pre		= Node._pre
			Node				= nil
			self._count = self._count - 1		
			iter_where:inc()
		else
			break
		end		
	end
	
	return iter_where
end

function CList:insert( iter_where, object )
	local pNode = iter_where:node()
	local Node  = { _pre = pNode._pre, _next = pNode , _value = object }
	pNode._pre._next	= Node
	pNode._pre			= Node
	self._count = self._count + 1
	return iter_where
end

function CList:size()
	return self._count
end

function CList:empty()
	return 0 == self._count
end

function CList:head()
	local iter = CList:_Iterator()
	iter:assign( self._myhead._next )
	return iter
end

function CList:tail()
	local iter = CList:_Iterator()
	iter:assign( self._end )
	return iter
end

function CList:front()
	return self._myhead._next._value
end

function CList:back()
	return self._end._pre._value
end

CList.IterMT.__eq =	function ( a, b )
	return a._pNode == b._pNode
end 

CList.IterMT.__lt =	function ( a, b )
	--print( "can not use \"<\" \"<=\" \">\" \">=\" to compare iterator!" )
	return false
end 

CList.IterMT.__le =	function ( a, b )
	--print( "can not use \"<\" \"<=\" \">\" \">=\" to compare iterator!" )
	return false
end

CList.IterMT.__add = function ( a, b)
	local iter = CList:Iterator( a )
	if ( tonumber(b) == nil ) then
		return iter
	end
	for i = 1, b do
		iter:inc()
	end
	return iter
end

CList.IterMT.__index.getvalue =	function ( self )
	return self._pNode._value			
end

CList.IterMT.__index.setvalue =	function ( self , object )
	self._pNode._value = object
end


CList.IterMT.__index.inc = function ( self )
	if( self._pNode._next ) then		
		self._pNode = self._pNode._next
		return self
	else
		return false
	end
end

CList.IterMT.__index.dec = function ( self )
	if( self._pNode._pre._pre ) then		
		self._pNode = self._pNode._pre
		return self
	else
		return false
	end
end

CList.IterMT.__index.assign = function ( self, pNode )
	self._pNode = pNode
	return self
end

CList.IterMT.__index.node =	function ( self )
	return self._pNode
end

function CList:_Iterator()
	
	local Iter		= {}
	Iter._pNode		= nil;
	
	setmetatable( Iter, CList.IterMT )	
	return Iter
end

function CList:Iterator( iterator )
	
	local Iter		= {}
	if( iterator ) then
		Iter._pNode		= iterator:node()
	else
		Iter._pNode		= nil;
	end
	
	setmetatable( Iter, CList.IterMT )	
	return Iter
end

function CList:Ctor( iter_begin, iter_end )
	self._count	= 0
	self._myhead	= { _pre = nil, _next = nil, _value = nil }
	self._end		= { _pre = self._myhead, _next = nil, _value = nil }
	self._myhead._next = self._end	
	
	if( iter_begin ) then
		if( iter_end ) then
			while( iter_begin ~= iter_end ) do
				self:insert( self:tail(), iter_begin:getvalue() )
				iter_begin:inc()
			end
		else
			while( iter_begin:node()._next ~= nil ) do
				self:insert( self:tail(), iter_begin:getvalue() )
				iter_begin:inc()
			end
		end
	end
	
	return self
end

function CList:find( value, iter_begin, fun_equal )
	local p
	if ( iter_begin ~= nil) then
		p = CList:Iterator( iter_begin )
	else 
		p = CList:head()
	end
	
	local iter_end	= self:tail()
	local iter		= iter_end
	
	if( fun_equal ) then
		while( p ~= iter_end ) do
			if( fun_equal( p:getvalue(), value ) ) then
				iter:assign( p:node() )
				return iter
			end
			if( p:inc() == false ) then break end
		end
		return iter
	else
		while( p ~= iter_end ) do
			if( p:getvalue() == value ) then
				iter:assign( p:node() )
				return iter
			end
			if( p:inc() == false ) then break end
		end
		return iter
	end	
end

function CList:enum( iterator_begin, iterator_end )
	local iter_cur = CList:Iterator( iterator_begin )
	local iter_end = CList:Iterator( iterator_end )
	
	return function ()
		if( iter_cur ~= iter_end ) then
			local value, iter = iter_cur:getvalue(), CList:Iterator( iter_cur )
			if( iter_cur:inc() == false ) then 
				return nil, nil 
			end
			return value, iter
		end
	end
end

function CList:reverse( iter_begin, iter_end )
	if( not ( iter_begin ~= nil and iter_end ~= nil) ) then
		iter_begin	= self:head()
		iter_end	= self:tail()
	end	
	if( iter_begin == iter_end ) then
		return
	end	
	local bNode = iter_begin:node()
	local eNode = iter_end:node()._pre
	while( eNode._next ~= bNode and bNode ~= eNode ) do
		local temp = bNode._value
		bNode._value = eNode._value
		eNode._value = temp
		bNode = bNode._next
		eNode = eNode._pre
		if(eNode == nil or eNode._pre == nil) then return end
		if(bNode == nil or bNode._next == nil or bNode._next._next == nil ) then return end
	end
end

function CList:merge( iterator_begin, iterator_end )
	if( iterator_begin ) then
		local iter_begin = CList:Iterator( iterator_begin )
		if( iterator_end ) then
			local iter_preend = CList:Iterator( iterator_end );
			iter_preend:dec()	
			while( iter_begin:node() ~= iter_preend:node()._next ) do
				self:insert( self:tail(), iter_begin:getvalue() )
				iter_begin:inc()
			end
		else
			while( iter_begin:node()._next ~= nil ) do
				self:insert( self:tail(), iter_begin:getvalue() )
				iter_begin:inc()
			end
		end
	end
end

function CList:copy_remove( iterator_where, ListRight, iterator_begin, iterator_end )
	if( not ( iterator_where ~= nil and ListRight ~= nil) ) then return end
	local iter_where = CList:Iterator( iterator_where )
	local iter_begin
	local iter_end
	if( iterator_begin == nil ) then
		iter_begin  = ListRight:head()
		iter_end	= ListRight:tail()
	else
		iter_begin = CList:Iterator( iterator_begin )
		if( iterator_end  == nil ) then
			iter_end	= ListRight:tail()
		else
			iter_end	= CList:Iterator( iterator_end )
		end
	end
	
	if( iter_where == iter_begin ) then return end
	while( iter_begin ~= iter_end ) do
		self:insert( iter_where, iter_begin:getvalue() )
		iter_begin = ListRight:erase( iter_begin )
	end
end

function CList:copy( iterator_where, ListRight, iterator_begin, iterator_end )
	if( not ( iterator_where  ~= nil and ListRight  ~= nil ) ) then return end
	local iter_begin
	local iter_end
	if( iterator_begin == nil ) then
		iter_begin  = ListRight:head()
		iter_end	= ListRight:tail()
	else
		iter_begin = CList:Iterator( iterator_begin )
		if( iterator_end == nil ) then
			iter_end	= ListRight:tail()
		else
			iter_end	= CList:Iterator( iterator_end )
		end
	end
	
	if( iterator_where == iter_begin ) then return end
	
	while( iter_begin ~= iter_end ) do
		self:insert( iterator_where, iter_begin:getvalue() )
		iter_begin:inc()
	end
end