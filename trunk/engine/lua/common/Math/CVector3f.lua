
function CVector3f:Ctor( x, y, z )
	if( type( x ) == "userdata" )then
		self.x = x.x
		self.y = x.y
		self.z = x.z
	else 
		self.x = x
		self.y = y
		self.z = z 
	end 
end