
Event =
{
	-- 测试用例使用的Event
	Test =
	{
		LoginBegan			= CQueue:new(),
		LoginEnded			= CQueue:new(),
		RCPBegan			= CQueue:new(),
		RCPRecv				= CQueue:new(),
		RCPEnded			= CQueue:new(),
		QuestReceived         = CQueue:new(),
		CharListReceived	= CQueue:new(),
		DelCharListReceived  = CQueue:new(),
		CreateCharEnded		= CQueue:new(),
		DeleteCharEnded		= CQueue:new(),
		GasCreatePlayerEnded= CQueue:new(),
		
		EnterGame			= CQueue:new(),
		Game2LoginEnded 	= CQueue:new(),
		
		ShopOpened 			= CQueue:new(),
		ShopClosed 			= CQueue:new(),
		
		BankOpened 			= CQueue:new(),
		BankClosed 			= CQueue:new(),
		SetLevel				= CQueue:new(),
		
		--GM related
		GMRunScriptDone	=	CQueue:new(),
		
		--物品操作
		AddItem 			= CQueue:new(),
		DelItem  			= CQueue:new(),
		AddSoul				= CQueue:new(),
		ReplaceItem 		= CQueue:new(), 
		MoveItem  			= CQueue:new(),
		SplitItem  			= CQueue:new(),
		PlaceBag  			= CQueue:new(),
		Change2Bag  		= CQueue:new(),
		FetchBag	 		= CQueue:new(),
		QuickMove  			= CQueue:new(),
		DelBag  			= CQueue:new(),
		UseItem 			= CQueue:new(),
		SetAddCount =  CQueue:new(),
		--装备操作
		FetchEquip 		= CQueue:new(),
		ReplaceRing   = CQueue:new(),
		Switch2Ring   = CQueue:new(),
		--金钱操作
		AddMoneyGM  		= CQueue:new(),
		AddTicketGM  		= CQueue:new(),
		AddBindingMoneyGM = CQueue:new(),
		AddBindingTicketGM = CQueue:new(),
		
		--货币流通
		OpenChangeMoneyWnd = CQueue:new(),
		
		-- Message
		MsgToClient  		= CQueue:new(),
		
		-- Object
		ObjectCreated		= CQueue:new(),
		ObjectDestroied = CQueue:new(),
		TrapCreated			=	CQueue:new(),
		CollObjCreated	= CQueue:new(),
		
		-- Player
		PlayerCreated 		= CQueue:new(),	
		PlayerDestroied		=	 CQueue:new(),
		PlayerMoveEnded  	= CQueue:new(),
		PlayerSkillEnded  	= CQueue:new(),
		NpcEnterAoi			= CQueue:new(),
		PlayerInfo			= CQueue:new(),
		PlayerSetLevel      = CQueue:new(),
		
		-- Scene
		SceneCreated		=	CQueue:new(),
		SceneDestroied	=	CQueue:new(),
		
		-- Npc
		NpcSkillEnded = CQueue:new(),
		NpcDead				= CQueue:new(),
		NpcPosChanged = CQueue:new(),
		
		--NPC商店
		OpenNpcSellWnd = CQueue:new(),
		ChangedMoneyCount = CQueue:new(),
		RetPlayerSoldGoodsEnd  = CQueue:new(),
		
		--Bullet
		BulletHit			= CQueue:new(),
		
		--邮件箱系统
		OpenEmailWnd =  CQueue:new(),
		SendEmailEnded = CQueue:new(),
		GetEmailEnded = CQueue:new(), 
		GetEmailListEnded = CQueue:new(),
		DeleteMailEnded = CQueue:new(),
--  TakeAttchmentEnded = CQueue:new(),
		--魂镶嵌宝石
		OpenHole = CQueue:new(),
		InlayStone = CQueue:new(),
		RemovalStone = CQueue:new(),
		SendAllHoleInfoEnd = CQueue:new(),
		ReturnSynthesisItemEnd = CQueue:new(),
		RetStoneAppraise = CQueue:new(),
		RetTakeAppraisedStone = CQueue:new(),
		
		--技能天赋面板
		AddTalent= CQueue:new(),
		--魂打入装备 
		SoulInEquip= CQueue:new(),
		--装备提炼魂
		--AddSoul = CQueue:new(),

		--按键映射
		SetKeyMapEnd = CQueue:new(),
		SendAllKeyMapsEnd = CQueue:new(),
		
		--物品拾取
		SetPickupItem = CQueue:new(),
		ShowPickupItem = CQueue:new(),
		
		--登陆
		FinishedLoading = CQueue:new(),
		AfterMoveCameraTick = CQueue:new(),--创建角色时，选中某个职业模型后，移动camera
		BackToLoginFinished = CQueue:new(),
		SelectRoleState = CQueue:new(),
	},

}
