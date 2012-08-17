function g_GetTexRectByIndex(Index, TexWidth, TexHeight, GridWidth, GridHeight)
	local rect = CFRect:new()
	
	local u = math.floor(TexWidth / GridWidth)
	local v = math.floor(TexHeight / GridHeight)
	
	local y = math.floor(Index / v)
	local x = Index - y*u
	
	local height = GridHeight
	local width = GridWidth
	
	rect.top = y * GridHeight
	rect.left = x * GridWidth
	
	rect.right = rect.left + width
	rect.bottom = rect.top + height
		
	return rect
end

local function SetWndImage(ID, Wnd, Index, szResFile, TexWidth, TexHeight, GridWidth, GridHeight, WndStateBegin, WndStateEnd)
	local pos = CFPos:new()
	pos.x = 0
	pos.y = 0
	
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	local rect = g_GetTexRectByIndex(ID, TexWidth, TexHeight, GridWidth, GridHeight)
	for i = WndStateBegin , WndStateEnd do
		Flag.StateMask = i
		local Image = Wnd:GetWndBkImage(Flag)
		if Index == -2 then
			Image:Release()
		end
		Image:SetGUIAsynEnable(true)
		Image:AddImage(Wnd:GetGraphic(), Index, szResFile, rect, pos, 4294967295, 0)
	end
end

function g_DelWndImage(Wnd, Index, WndStateBegin, WndStateEnd)
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	
	for i = WndStateBegin , WndStateEnd do
		Flag.StateMask = i
		local Image = Wnd:GetWndBkImage(Flag)
		Image:DeleteImage(Index)
	end
end

function g_LoadIconFromRes(ImageID, Wnd, Index, WndStateBegin, WndStateEnd) -- ID从0开始
	-- 每张纹理为		256 x 256
	-- 每个图标为		32 x 32
	-- 每张图表数为		8 x 8  = 64
	local dir = g_ImageMgr:GetImagePath(10001)
	local path = dir .. math.floor(ImageID/64) .. ".tex"
	local ImageIndex = ImageID % 64
	
	SetWndImage(ImageIndex, Wnd, Index, path, 256, 256, 32, 32, WndStateBegin, WndStateEnd)
end

function g_LoadGrayIconFromRes(ImageID, Wnd, Index, WndStateBegin, WndStateEnd)
	-- 每张纹理为		256 x 256
	-- 每个图标为		32 x 32
	-- 每张图表数为		8 x 8  = 64
	local dir = g_ImageMgr:GetImagePath(10002)
	local path = dir .. math.floor(ImageID/64) .. ".tex"
	local ImageIndex = ImageID % 64
	
	SetWndImage(ImageIndex, Wnd, Index, path, 256, 256, 32, 32, WndStateBegin, WndStateEnd)		
end

function g_LoadBufIconFromRes(ImageID, Wnd, Index, WndStateBegin, WndStateEnd) -- ID从0开始
	-- 每张纹理为		192 x 192
	-- 每个图标为		24 x 24
	-- 每张图表数为	8 x 8  = 64
	local dir = g_ImageMgr:GetImagePath(10003)
	local path = dir .. math.floor(ImageID/64) .. ".tex"
	local ImageIndex = ImageID % 64
	
	SetWndImage(ImageIndex, Wnd, Index, path, 192, 192, 24, 24, WndStateBegin, WndStateEnd)
end

function g_RePlaceWndImage(ImageID, Wnd, descIndex, WndStateBegin, WndStateEnd)
	assert(descIndex>=0)
	-- 每张纹理为		256 x 256
	-- 每个图标为		32 x 32
	-- 每张图表数为	8 x 8  = 64
	local dir = g_ImageMgr:GetImagePath(10001)
	local path = dir .. math.floor(ImageID/64) .. ".tex"
	local ImageIndex = ImageID % 64
	g_DelWndImage(Wnd, descIndex, WndStateBegin, WndStateEnd)
	SetWndImage(ImageIndex, Wnd, descIndex, path, 256, 256, 32, 32, WndStateBegin, WndStateEnd)
end
	

function g_ClearWndImage( IconWnd, WndStateBegin, WndStateEnd )
	local IP = IMAGE_PARAM:new( SM_BS_BK, IP_ENABLE );
	for i = WndStateBegin, WndStateEnd do
		IP.StateMask = i
		local WndImage = IconWnd:GetWndBkImage( IP )
		if WndImage ~= nil then
			WndImage:Release();
		end
	end
end