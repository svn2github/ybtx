CBreakItemRpc = class()

function CBreakItemRpc.ReturnBreakItemBegin(Conn)
	if g_GameMain.m_BreakItemWnd == nil then
		g_GameMain.m_BreakItemWnd = CBreakItemWnd.CreateBreakItemWnd(g_GameMain)
	end
	g_GameMain.m_BreakItemWnd.m_CurPageNo = 1
	g_GameMain.m_BreakItemWnd.m_TotalPageNo = 1
	g_GameMain.m_BreakItemWnd.m_tblBreakItemProducts	=	{}	
end

function CBreakItemRpc.ReturnBreakProducts(Conn,item_type,item_name,item_num)
	local node		=	{}
	node.ItemType	=	item_type					
	node.ItemName	=	item_name					
	node.ItemCount	=	item_num					
	table.insert(g_GameMain.m_BreakItemWnd.m_tblBreakItemProducts, node)
end

function CBreakItemRpc.ReturnBreakItemEnd(Conn,bFlag,nRoomIndex,nPos)
	CStoreRoomWndMgr.GetMgr():SetItemGridWndState(nRoomIndex,nPos,true)
	if not g_GameMain.m_BreakItemWnd then
		return 
	end
	if g_GameMain.m_BreakItemWnd.m_tblBreakItemProducts and #g_GameMain.m_BreakItemWnd.m_tblBreakItemProducts > 0 then
		if g_GameMain.m_BreakItemWnd then
			--g_GameMain.m_BreakItemWnd:ShowWnd(true)
			--g_GameMain.m_BreakItemWnd:ShowBreakProductsDetails()
		end
		g_WndMouse:ClearCursorSpecialState()
	end
end

function CBreakItemRpc.AddBreakItemExp(Conn,nAddExp)
	g_DynItemInfoMgr:AddBreakItemExp(nAddExp)
	if g_GameMain.m_WndMainBag and g_GameMain.m_WndMainBag.m_BtnBreakItem then
		g_GameMain.m_WndMainBag.m_BtnBreakItem:SetMouseOverDescAfter(ToolTipsText(297,g_DynItemInfoMgr.m_BreakItemExp or 0))	
	end
end

function CBreakItemRpc.InitBreakItemExp(Conn,nAddExp)
	g_DynItemInfoMgr:InitBreakItemExp()
	g_DynItemInfoMgr:AddBreakItemExp(nAddExp)
	if g_GameMain.m_WndMainBag and g_GameMain.m_WndMainBag.m_BtnBreakItem then
		g_GameMain.m_WndMainBag.m_BtnBreakItem:SetMouseOverDescAfter(ToolTipsText(297,g_DynItemInfoMgr.m_BreakItemExp or 0))	
	end
end