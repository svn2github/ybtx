 
--[[   字符	说明	   
i	表示有符号整型
I	表示无符号整型
h	表示有符号短整型
H	表示无符号短整型
c	表示有符号字符型型
C	表示无符号字符型
f	表示单精度浮点
d	表示双精度浮点
s	表示长度小于256的字符串
S	表示长度小于65536的字符串
b	表示布尔类型
--]]

return "Gac2Gas",
{
	{ "ByteCall","cc"},
	{ "SetLockingNPCID","I"},

-- 远程变量读取 相关RPC
	{ "Remote_MS_GetValue",		"S" },
	{ "Remote_MS_GetValues",	"S" },
	{ "Remote_MS_GetType",		"S" },
	{ "Remote_CB_GetValue",		"S" },
	{ "Remote_CB_GetType",		"S" }, 
	{ "Remote_ResumeResult",	"S" },
-- 登陆相关RPC
	{ "CheckUser",				"sssssss" },
	{"CheckTrusteeUser", "sssss"},
	{ "Get_Char_List",			"" },
	{ "Get_DeletedChar_List",			"" },
	{ "CreateAccount", "s"},
	{	"UserForceTipSigal","s"},
	{"LoginWaitingQueue",""},
	{"CancelLoginWaiting",""},
-- 创建角色 		
	{ "GetCampServerIpInfo",	"I"},
	{ "CreateRole",				"sHHHHHHHII"}, -- strName, nHair,nHairColor,nFace,nScale,nSex,nClass,campInfo,m_nInherence,xpos,ypos
	{ "DelChar",				"I" },
	{ "CompleteDeleteChar",		"I" },
	{ "GetBackRole",			"S" },
	{ "PreChangePlayerName",			"I" },
	{ "CharEnterGame",			"II" },
	{ "DestroyPlayer",			""  },
	{ "CharEnterOtherServer",	"IsS"},
	--{ "CompleteDeleteChar",		"I" },
	{"ChangePlayerName",		"Is"},
	
	--充值兑换
	{"ChangeTicketToMoney",		"I"},
	{"GetTotalTicket",		""},
	{"GetCompensateItemInfo",		"I"},
	{"GetActivationCodeCompenateItem",		""},
	{"TakeCompensateItem",		"IsI"},
	{"TakeActivationCodeCompensate",		"I"},
-- Agreement
	{ "SetUserAgreement", 		""},
	
-- npc for test
	{ "AddNpcForTest", 			"III"},
	
-- 角色状态、技能释放		
	{ "AttackMe",				"IsI"},
	{ "ChangeGridPos",  "Iii"},
	{ "UnLockObj",				""},
	{ "UnLockIntObj",				""},
	{ "ActiveBehavior",		""},
	{	"ActiveBehaviorDoSkill","Isff"},
	{ "QuitFitoutState",	""},
	{ "BeginKickBehavior",""},
	{ "EndKickBehavior","II"},
	{ "CancelKickBehavior",""},
--	{ "AdjustBulletTime",		"Cf"	},  -- BulletId, AdjustTime )
--Player 技能相关
	{ "AddTalentSkill", 			"sI"	},
	{ "UpdateFightSkill",		"sCC"},
	{ "GoBackOrStay",			"b"		},
	{ "GoLastMasterSceneReborn",""},
	{ "StationReborn",		""		},
	{ "ExitOrInMode",		"b"},
	{ "GoIntoFbOrStay",	"b"},
	{ "DoReborn",	""},
	{ "AreaFbRebornToRePoint",	""},
	{ "MatchGameJoinPointReborn",	""},
	
	{	"RequestSkillSeries", ""}, --请求技能树信息
	{	"SetSeries",					"I"}, --设置天赋系
	{	"SetSkillNode",				"I"}, -- 设置技能结点
	
-- 洗天赋
	{"ClearAllGenius",          ""      },
-- 存储快接栏
	{ "SaveShortCut", 			"CCsII" }, 		--pos, type, arg1, arg2, arg3
	{ "SaveShortCutEnd",        ""},
	
--召唤兽名字设置
	{"RequestSetServantName",	"Iss"},
	
--NPC跟随 朝向
	{"GetNpcDirection","I"},
--测试子弹魔法
	{ "StartBulletMagicTest", "" },
	{ "ShutDownBulletMagicTest", "" },	

-- 物品空间操作
	{"BreakItem",		"CC"},
	{"DelWithTimeOutItem",		"dHs"},
	{ "OnUseDelItemByPos",		 	"CCH" },   --nRoomIndex,nPos,nCount
	{ "OnLButtonUpDelItemByPos",	"CCH" },   --nRoomIndex,nPos,nCount
	{ "SplitItem",       		"CCCCH" }, 	--nARoom,nAPos,nBRoom,nBPos,nACount
	{ "MoveItem",        		"CCCC" },  	--nARoom,nAPos,nBRoom,nBPos
	{ "PlaceBag", 			 	"CCC"},   	--nRoom,nPos,nSlot
	{ "FetchBag", 			 	"CCC"},   	--nSlot,nRoom,nPos
	{ "Change2Bag", 			"CC"},   			--nASlotRoom,nASlot,nBSlotRoom,nBSlot
	{ "CleanBag",					"C"},    --整理包裹
	{ "DelBag", 				"C"},   			 	--nSlot
	{ "QuickMove",  			"CCC"}, 	 		--nARoom,nAPos,nStoreMain
	{ "SaveMoneyToDepot", 			"d"},
	{ "GetMoneyFromDepot", 			"d"},
	{ "GetDepotInfo", 			""},
	{	"UseItem", 				"CCHsI"},	 	--nRoomIndex,nPos,nBigID,Index,eEquipPart
	{	"UseItemEnd", 		"s"},	 			
	{ "FetchEquipByPart",		"CCC"}, 		--eEquipPart,nRoomIndex,nPos
	{"RClickFetchEquipByPart","C"},
	{ "SwitchTwoRing", 			""},     		--交换左右戒指
	{ "SwitchWeapon", 			""},     		--交换主手和副手武器
	{	"DelEquip"     , 		"C"},			--直接从身体上删除装备
	{ "FortifiedEquip", 		"CIdCICICI"},	--强化装备
	{"DelQuestItemByPos","sCCH"},--删除任务道具就会删除任务
-- 提示信息发送 相关RPC
	{ "TestMessage", 			"C" },
	
-- GM Command 相关RPC
	{ "GM_Execute",				"S" },
	{ "GM_RunScript",			"S" },
	{ "GM_OpenConsole",			"" },
--Gm Player 相关RPC
	{ "SetGridPos", 			"ii"},			--设置玩家的格子坐标
	
--组队系统相关
	{ "InviteMakeTeam", 		"I"		},	--根据id邀请组队
	{ "InviteMakeTeamByName", 		"s"		},	--根据名称邀请组队
	{ "RespondInvite", 			"Isb"	},	--响应邀请
	{ "RespondApp",				"Isb"	},	--响应申请
	{ "BreakTeam", 				""		},	--解散队伍
	{ "SetCaptain", 			"I"		},	--设置队长
	{ "LeaveTeam", 				""		},	--离开队伍
	{ "SetAssignMode", 			"H"		},	--设置分配方式
	{ "GetTeamMembers", 		""		},	--获得该队伍的队员
	{ "GetTeammateProp", 		"I"		},	--获取队友的属性
	{ "RemoveTeamMember",		"I"		},	--踢出队伍
	{ "SetAuctionStandard",		"I"		},	--设置拍卖分配方式的拍卖品质标准
	{ "SetAuctionBasePrice",		"I"		},	--设置拍卖分配方式的拍卖底价
	{ "OnLeftClickTeammateIcon","I"		},	--左键点击队友头像，选中队友
	{ "UpdateTeamMark",			"III"	},	--小队标记,参数：标记类型，对象类型，对象id
	
--申请组队
	{"getAppTeamList","I"}, 				--nType  排队种类	
	{"leaveList","I"},							--nType
	{"joinTeamList","Is"},          --nType,sMsg
	{"upFreshTeamList","I"},				--nType
--社群系统
	{"AddFriendClass",	"s"},					--添加好友组别
	{"SearchPlayerAccurately","sI"}, 	--精确查找好友 参数：好友name，好友ID
	{"AddFriendToClass","Is"},          --添加好友到好友组里 参数：玩家ID,组名称
	{"PrivateChatSendMsg","IS"},  --私聊 参数：要聊的对象ID，聊天内容
	{"RespondAddFriend", 	"III"}, --相应添加好友 参数 添加自己为好友的玩家ID，添加自己为好友的玩家组ID，自己的组ID
	{"DeleteMyFriend","I"},        --删除好友 参数：要删除好友的ID 
	{"UpdateFriendClassName","Is"},  --修改好友组名称 参数：好友组ID，新名称
	{"DeleteBlackListMember","I"},   --删除好友黑名单里面的好友 参数：要删除的好友ID
	{"MoveToBlackList","I"},    --添加好友到黑名单里面 参数：要添加的好友ID
	{"AddBlackListByName","s"}, --通过玩家name将玩家加入到黑名单里面 参数：要添加的玩家name
	{"ChangeToClass","II"},   --将好友移动到别的组里面 参数：好友ID，新组ID
	{"DeleteFriendClass","I"}, --删除好友组 参数：要删除的组ID
	{"CreateFriendGroup","sIsS"},  --创建好友群 参数：群名称,群类别，群关键字,群公告
	{"SearchGroupAccurately","sI"}, --精确查找群 参数:群name，群ID
	{"RequestJoinIntoFriendGroup","I"}, --申请加入群 参数：群ID
	{"ResponseRequestJoinGroup","III"}, --同意申请者加入群 参数：申请者ID，群ID
	{"PublicChatSendMsg","IS"},    --群聊 参数：群ID，聊天内容
	{"InviteToGroup","II"},    --邀请加入群 参数：被邀请者ID,群ID
	{"ResponseInviteToGroup","III"},  --响应邀请加入群 参数：群ID
	{"LeaveGroup","I"},   --退出群 参数：群ID
	{"DisbandGroup","I"}, --解散群 参数：群ID
	{"SetCtrlType","III"},  --设置/取消管理员 参数：玩家ID，群ID，要设置的类别 2-设置管理员；3-取消管理员
	{"KickOutOfGroup","II"}, --剔除玩家 参数：玩家ID，群ID
	{"RenameFriendClass","Iss"}, --修改好友组名称 参数：组ID,组的新name,原先的名字
	{"ChangeShowSentence","s"},		--修改心情寄语 参数：新寄语
	{"SaveAssociationPersonalInfo","CssCCCCs"}, --保存个人信息
	{"GetAssociationPersonalInfo",""}, -- 查看个人信息
	{"GetAssociationMemberInfo","I"}, -- 查看玩家信息 参数：玩家ID
	{"SearchPlayerCondition","IIIIs"}, --条件查找好友 参数：职业，性别，最低等级，最高等级，所在地图
	{"SearchGroupCondition","sI"}, --条件查找好友群 参数：关键字，类型
	{"GetMemberIdbyNameForPrivateChat", "sS"}, --通过对方名字开始私聊(返回ID) 参数：对方名字, 聊天内容
	{"CofcChatSendMsg","S"}, -- 商会群聊 参数：聊天内容
	{"AddFriendToClassByName","sI"}, --通过玩家名称添加玩家为自己好友 参数：玩家name，好友组Id
	{"ChangeGroupDeclare", "IS"},
	{"RefuseBeAddFriend","I"},			--拒绝好友邀请
	{"UpdateFriendInfo", "I"},			--刷新指定好友信息
	{"UpdateOnlineFriendInfo", ""},		--刷新在线好友信息
	{"RefuseGroupMsgOrNot","II"},--开启/屏蔽群消息
	{"GetTongProfferCount", ""}, --查询当前团贡献积分数目
	{"GetTongLevel", ""}, --查询团等级
	{"LearnTongTech", "s"},--学习佣兵小队科技

--按键映射
	{"SetKeyMap", 					"ssIb"},	--添加按键映射 (参数为功能名称, 按键名称, 第一还是第二位置)
	{"GetAllKeyMaps", 				""},		--获取所有按键映射 
	{"ChangeMouseCtrl", 			"IIIIIII"},	--修改鼠标设置或Tab筛选距离
	{"ChangeMouseCtrlBeforeGame",	"ICb"},		--进入游戏之前进行鼠标设置
	
--聊天相关
	--[[ 操作的定义如下：
		SUBSCRIBE = 1,
		UN_SUBSCRIBE = 0
	--]]
	{"SetChannelPanel",			"III"},		--设置聊天面板的频道 (频道id， 订阅or取消订阅，面板位置)
	{"SetChannelPanelEnd",			""},	
	{"SetDefaultChannelPanel", 	""},		--设置为默认信息
	{"RenamePanel", 			"sI"},		--重命名面板, 
	{"AddPanel",				"SI"},		--添加面板
	{"DeletePanel", 			"SI"},		--删除面板
	{"SetPanel",				"SI"},		--设置面板
	{"TalkToChannel",			"SI"},		--向某频道说话，s为消息内容，I为频道id
	{"MovePosB2PosA",		"II"},			--从位置B移到位置A，参数为（A位置，B位置）
	{"GetInfoByID",         "d"},   		--通过ID得到其他相关信息
	{"SaveNotShowMsgWnd","s"},
	
--获取服务器延时
	{"GetDelayTime",			""}, 	--获取服务器延时
	
--任务相关 
	{"GiveUpQuest",				"s"},	--放弃任务 参数（任务名称）
--	{"GetQuestState",			"s"}, --获取任务状态 参数（任务名称）
--	{"GetQuestVar",				"ss"},--获取某任务的任务变量 参数（任务名称，任务变量名称）
--	{"GetAllQuestVar",		"s"},	--获取某任务的所有任务变量 参数（任务名称）
--	{"GetAllQuest",				""},	--获取角色所有存放在数据库的任务信息
	{"AcceptQuest",			"sI"},		--接受任务，参数(任务名称，GlobalID)
	{"RequestQuestByItem", "s"},  --接受任务(物品触发任务)， 参数(任务名称)
	{"AcceptQuestJoinFb","ss"},--其它队员接取任务进副本
	{"FinishQuest",			"sII"},		--完成任务，参数(任务名称，选中的可选奖励的索引，GlobalID）
	{"ShowTakeQuestWnd", "sI"},			--显示任务领取面板，参数（任务名称，GlobalID）
	{"ShowFinishQuestWnd", "sI"},     --显示任务完成面板，参数（任务名称，GlobalID）
	{"ShareQuest","s"},
	{"ResponseShareQuest","Is"},  --共享任务的队伍ID，任务名
	{"SetQuestFailure","s"},	--任务失败发送信息
	{"SetQuestIsLimitTime","s"},--设置限时任务,不再限时
	{"SetQuestLevelVarNum","ss"},--等级加任务计数(接任务时,玩家等级够了就加计数)
--任务相关小游戏()
	{"SmallGameSucceed", "sI"},	--小游戏成功完成
	{"SmallGameFaild", "s"},		--小游戏失败
	{"SmallGameCancel","sI"},		--取消小游戏
	{"ExitSmallGame","sI"},--移动或者是被打退出
	{"AgreeBeginSmallGame","sI"},--是否同意开启小游戏

--佣兵等级指引	
	{"SetCheckInMercenaryInfo",""},
	{"CreateMercenaryLevel",""},
	{"MercenaryLevelUp",""},
	{"DoMercenaryLevelAppraise",""},
	{"GetMercenaryLevelAward","b"},
	{"SetMercenaryLevelAward","s"},
--npc

	{"ClickNpc", "I"},			--触发Npc行为
	{"ShowQuestNpcTalkWnd", "I"},	  --npc对话框
	{"ClickItem","IsCC"},
	--{"ShowTakeQuestObjTalkWnd","I"},
	{"ShowNpcDropItemWnd","I"},
	{"CancelGetDropItems","I"},
	{"SelectAllDropItems","I"},
	{"SelectSingleGridDropItem","II"},
	{"SubmitAuctionPrice","I"},
	{"SubmitAuctionStandard","I"},
	{"PlayerCancelAuction",""},
	
	{"AddFinishNpcTalk","sIb"},--npc交互对话加记录
	{"GetNpcTalkHeadInfo", "I"},
	
	-------------------------------------
	--DPS信息接口
	{"BeginSendFbDpsInfo",""},
	{"EndSendFbDpsInfo",""},
	{"GetBossDpsInfo",""},
	{"ChangeDpsShowType","s"},
	
	--副本统一接口
	{"EnterFbSceneFromWnd","s"},
	{"CancelFbAction","s"},
	{"AgreeJoinFbAction","s"},
	{"NotJoinFbAction","s"},
	{"AutoJoinActio", "IIs"},
	--大逃杀
	{"JoinDaTaoSha",""},
	{"ShowDaTaoShaWnd","I"},
	{"ExitDTSFbAction",""},
	--积分赛
	{"JoinJiFenSai","I"},
	{"ShowJiFenSaiWnd","I"},
	{"ExitJFSFbAction",""},
	{"ShowJiFenSaiInfoWnd","I"},
	--跳跳大夺宝
	{"JoinDaDuoBao","I"},
	{"ShowDaDuoBaoWnd","I"},
	{"ShowDrinkWnd","I"},
	{"HaveDrink","S"},
	{"GetTiredNess", "S"},
	{"IsDrunk", ""},
	--佣兵训练活动
	{"JoinYbEducateAction","s"},
	{"ShowYbEducateActionWnd","I"},
	{"ContinueYbXlAction","b"},
	{"BeginYbEducateAction",""},
	
	--佣兵团刷怪本
	{"ShowMercenaryMonsterFbWnd","I"},
	{"EnterGeneralMercenaryMonsterFb",""},
	{"AgreedJoinMercenaryMonsterFb","sI"},	
	--{"OpenSignQueryWnd",""},	
	{"OpenAutoSeekWnd",""},	
	{"FbStationAutoSeekInfo",""},
	{"JoinRobResFb",""},	
	{"JudgeWarTransport","s"},	
	{"GetProffer",""},	
	{"PlayerExitFb",""},
	{"CloseActionWnd",""},
	{"GoRobResRebormPlaceStay","b"},
	{"GetForageAutoInfo",""},
	{"IssueTongInfo",""},
	{"IssueFetchQueryWnd",""},
	{"RetIssueFetchWnd", ""},
	{"PlayerOutFb", ""},
	
	--抢夺资源点
	{"CancelSign", ""},
	{"ShowCancelWnd", ""},
	{"EnterRobScene", "b"},
	{"OpenSignQueryWnd", ""},
	{"ConfirmJoin",""},						
	{"GetHeaderAward",""},
	
	--Boss争夺战
	{"ShowBossBattleWnd","I"},
	{"CanJoinBossBattle",""},
	{"CancelJoinBossBattle",""},
	{"EnterBossBattle","Is"},
	{"TimeOverLeaveBossBattle",""},
	
	--喝酒射箭副本
	{"DrinkShootGetReady",""},
	{"DrinkShootNextPlayer",""},
	{"DrinkShootDoAction","s"},
	{"DrinkShootDoSkill","sII"},
	
	--脑电波小游戏
	{"HeadGameChangeAction","s"},
	{"HeadGameChangeActionBag","s"},
	
	--Npc对话面板
	{"RemitSpecScene","Is"},
	{"RenoSpecNpc","Is"},
	{"GetPicInfo","I"},
	{"IsCanTransport","IsIb"},
	{"RetTransInfo","sII"},
	----------------------------------------
	--龙穴活动
	{"ShowDragonCaveWnd", "I"},
	{"EnterDragonCave", "s"},
	{"IntoDragonCave","s"},
	{"LeaveDragonCave", ""},
	---------------区域副本
	{"ShowAreaFbSelWnd","I"},
	{"EnterGeneralAreaFb","Is"},
	{"EnterSpecialAreaFb","Is"},
	{"AgreedJoinAreaFb","sII"},
	{"DecideJoinAreaFb","sII"},
	{"AgreedChangeOut",""},
	{"ReferAreaFbQuest","s"},
	{"GetDungeonCount", ""},
	
	--挑战任务的副本
	{"AgreedJoinDareFb","ss"},
	
	--领域副本
	{"EnterScopesFb","s"},
	{"AgreedJoinScopesFb","sII"},
	
	--剧情副本
	{"ShowJuQingWnd","I"},
	{"EnterSenarioFb","sb"},
	{"AgreedJoinSenarioFb","sII"},
	{"CancelChangeCameraPos",""},
	
	--邀请进人公共区域副本
	{"PublicAreaInvite", "I"},			
	
	-------------活动信息面板
	{"GetActionCount",""},
	{"GetActionCountEx",""},
	
	-------------副本比赛游戏
	{"ShowMatchGameSelWnd", "I"},
	{"JoinMatchGame", "sb"},
	{"UseFbItem","sss"},
	{"UseFbSecItem","ssI"},
	
	
	-------------帮会战相关
	--{"ShowWarZoneSelWnd", "I"},
	{"EnterWarZone", "Is"},
	{"UpdateBuildingLevel", "I"},
	{"EnterZone", "I"},
	{"DeleteHandForage", ""},
	
	
	--矿洞副本
	{"JoinOreScene","IIs"},
	{"JoinOreNextScene",""},
	{"TeamJoinScene","IIsI"},
	{"CreateAreaScene","s"},
	{"LeaveAreaScene",""},
	{"SendInviteJoinSceneMsg","I"},
	{"InviteChangeScene","IIsIs"},
	-----------------
	
--交互对象
	{"ShowQuestObjTalkWnd","I"},--交互对象GlobalID
	{"ObjShowContentWnd","I"},--交互对象GlobalID
	{"ShowCollObjWnd","I"},--交互对象GlobalID
	{"SelectSingleGridItem","II"},--物品所放的格子号，交互对象GlobalID
	{"SelectAllGridItems","I"},----交互对象GlobalID
	{"SendSuccPerfetColl","I"},----交互对象GlobalID
	{"CancelCollItems","I"},--交互对象GlobalID
	{"RequestShowAnimation","I"},
	{"ShowAnimationEnd","I"},
	{"ShowCollItems","I"},
	{"OnSelectIntObj","I"},  --通知服务器端标记锁定的IntObj
	{"OnLClickIntObj","I"},   --左键点击交互对象的响应，交互对象GlobalID
	{"ResponseCallRequest","sb"}, --召唤者名字，是否接受召唤
	{"BreakPlayerActionLoading",""},

--邮件系统
	--普通邮件
	{"CheckMailTextAttachment", "II"},		--包裹中右键邮件文本附件，查看信件内容那个；参数：文本附件在包裹里的nRoomIndex，nPos
	{"RightButtonClickMailBox","I"},
	{"GetMail",				"I"},			--获取邮件内容，参数（邮件ID）
	{"GetMailList",			""},			--获取邮件列表
	{"SendMail",			"ssSII"},	--发送邮件（收件人名称，邮件标题，邮件内容，钱的数目）
	{"GetSendMoneyCess", "I"},
	{"SendMailGoodsEnd", ""},           --发送完毕
	{"TakeAtachment",		"I"},		--获取附件（邮件ID，物品名称)
	{"DeleteMail",			"I"},	   	--删除邮件（邮件ID）
	{"SendMailGoods",       "II" },	--发送邮件的物品部分，参数：物品ID，邮件物品栏的格子号grid
	{"TakeContextAtachment",        "I"},--提取文本附件，参数：邮件ID
	{"TakeAttackmentByIndex",   "II"},--收件人提取单个格子的物品，参数：邮件ID，格子号
	{"GetMailMoney",           "I"},--收件人提取金钱，参数：邮件ID
	{"TakeAttachment",        "I"},--收件人提取全部格子的物品，参数：邮件ID
	{"DeleteBatchMailBegin", ""},
	{"DeleteBatchMail","I"},--批出量删除邮件开始，参数：mailID
	{"DeleteBatchMailEnd",""},	--批出量删除邮件结束,参数：普通邮件（1），系统邮件（2）
	
--Npc Shop
	{"BuyItem", 		"sssII" },		--购买物品, 参数：Npc名，类型，物品名称，数量, 绑定或者不绑定的钱type
	{"BuyItemByPos", 	"sssIIII"},		--购买物品到某格子，Npc名，类型，物品名称，数量，RoomIndex，格子pos, 绑定或者不绑定的钱type
	{"BuyBackItem", 	"sI"},			--购回物品，参数：Npc名称，格子
	{"BuyBackAllItem", 	"s"},			--购回所有物品, 参数：Npc名称
	{"PlayerSellItemByPos", 	"siII"},--卖东西，参数：Npc名称，RoomIndex，Pos，数量
	{"PlayerSoldGoodsList", 	"s"},		--已售物品列表,参数：npcName
	
	--魂镶嵌宝石	
	{"SendSynthesisItem","d"} ,			--发送合成所需的打孔材料或宝石，参数：打孔材料或宝石ID
	{"SendSynthesisItemEnd",""} ,		--发送合成所需的打孔材料或宝石结束
	{"RequestOpenHole","IIId"},		--申请打孔，参数：面板号Panel,部位号Depart,孔号Slot,打孔材料ItemID
	{"RequestInlayStone","IIId"},		--申请镶嵌，参数：面板号Panel,部位号Depart,孔号Slot,宝石StoneId
	{"RequestRemovalStone","III"},		--摘除已经镶嵌的宝石，参数：面板号Panel,部位号Depart,孔号Slot
	{"SendSynthesisItemBegin",""},		--发送合成所需的打孔材料或宝石开始
	{"RequestStoneAppraiseAgain",""},	--申请白宝石再次鉴定
	{"TakeAppraisedStone",""},		--提取鉴定生成的宝石
	{"ChangeFrame","II"},				--更换面板
 --玩家交易
	{"SendTradeInvitation","I"},     --邀请者向服务器发送交易邀请，参数：对方ID
	{"DenyedTrade","I"},             --玩家拒绝了交易，参数：对方ID
	{"AgreeToTrade","I"},            --交易被邀请者接受了交易请求，对方ID
	{"ChoosedTradeItem","IiIId"},     --玩家选择了交易的物品，参数：对方ID，nRoomIndex，pos（在包裹中的位置，对应于nRoomIndex），物品数目, 物品id
	{"CancelChoosedItem","II"},      --玩家取消了交易的物品，参数：对方ID，物品在交易栏中的位置
	{"ReplaceItem","IIiIId"},         --玩家替换了交易的物品，参数：对方ID，物品在交易栏中的位置，nRoomIndex，在包裹中的位置（对应于nRoomIndex），物品数目, 物品id
	{"ChoosedTradeMoney","Id"},      --玩家输入了交易的金币数目，参数：对方ID，金币数目
	{"HaveLockedTrade","I"},         --玩家锁定了交易，参数：对方ID
	{"CanceledTrade","I"},           --玩家取消了交易，参数：对方ID  
	{"SubmitedTrade","I"},           --玩家提交了交易，参数：对方ID
	{"GetTradeCess","I"},          
	{"SenTradeInvitationByPlayerName", "s"},--邀请者向服务器发送交易邀请，参数：对方名称
	
	{"GetPlayersTradeRequestList", ""},	--请求显示玩家交易请求		
	{"DenyAllPlayersTradeRequest", ""},	--拒绝所有交易请求
	
	--装备的升级魂的灌入	
	{"PourUpgradeSoul","dI"},		 --装备的ID,注入魂的数
	--装备的强化魂的灌入
	{"PourIntensifySoul","dI"},		 --装备的ID，注入魂的数
	--装备强化
	{"IntensifyEquip",		"dI"},	--强化装备，参数：装备id
	{"EquipIntensifyUseStone", "dIII"}, --用装备强化石强化的装备，参数：
	--装备鉴定
	{"ArmorOrJewelryIdentify","IIssfIIIId"},--防具鉴定 参数:防具基础值，大类id，装备名称，倾向字段，倾向值,
	{"WeaponIdentify","IIssfIIIIdI"},--装备鉴定 参数：
	--护甲片设定
	{"ArmorPieceEnactment","IsIIIIdIIs"},
	--装备升级
	{"UpgradeEquip","dI"},            --装备的ID
	
	{"PourPearl2Equip", "II"},				--将魂珠里面魂灌注进装备 参数：装备ID，魂珠ID
	
	--寄售交易所
	{"CSMBuyOrder",    "I"},					--购买订单物品， 参数：订单ID
	{"CSMSearchBuyOrder",   "IIIsIIiII" },	--查询购买列表，参数：物品类型，物品名称，起始等级，终止等级，排序方式，页数
	{"CSMSearchSellOrder",   "sIIIIIIiIiib" },	--查询购买列表，参数：物品名称，起始等级，终止等级，是否可用，排序方式，页数，1-物品、2-套装，战神，魔法
	{"CSMGetOrderByItemType","IIsIIIIIIiIiib"},	--同上，比上边多两个参数：面板，物品类别
	{"CSMGetOrderByItemAttr","IIsssssIIIIIIiIiib"},--同上，多如右参数：面板，属性列名， parentNodeText，属性列名， 具体属性名
	{"CSMGetOrderBySomeItemType","IIsIIIIIIiIiib"},--查询购买列表，参数：面板， 某几类物品的代表数字（某一数字代表包含的几类物品）,物品名称，起始等级，终止等级，是否可用，排序方式，页数
	{"CSMGetOrderByExactItemAttr","IIIsIIIIIIiIiib"},--查询购买列表，参数：面板，物品类型，属性列列名，具体某个属性,物品名称，起始等级，终止等级，是否可用，排序方式，页数
	{"CSMSearchCharBuyOrder",   "" },		--玩家查询个人购买列表
	{"CSMSearchCharSellOrder",   "II"},		--玩家查询个人出售类表，参数：排序方式，页数
	{"CSMAddBuyOrder",   "sIdI" },						--玩家添加收购订单，参数：物品名，数目，价格，时间
	{"CSMAddSellOrder",   "iIIdIb"},						--玩家添加出售订单，参数：RoomIndex，pos，数目，价格，时间,是否激活记住价格功能（true或false）
	{"CSMCancelBuyOrder",   "I" },					--玩家取消收购订单，参数：订单ID
	{"CSMCancelSellOrder",   "I"},					--玩家取消出售订单，参数：订单ID
	{"CSMTakeAttachment",    "I"},					--玩家提取附件， 参数：订单ID
	{"CSMSellGoods2Order",       "IiI"},					--支付订单,参数：订单ID，RoomIndex，pos
	{"CSMFastSearchBuyOrderItem",       ""},					--支付订单,参数：订单ID，RoomIndex，pos
	{"GetCSMRememberPrice",	"Iss"},					--得到玩家上次出售某种物品的价格，参数:面板no，物品类型，物品名称
	{"CSMGetOrderBySeveralSortItem", "IIIsIIIIIIiIiib"},
	{"CSMGetTopPriceBuyOrderByItemName","s"},
	{"CSMGetAveragePriceByItemName","s"},
	{"SetMoneyType",       "I"},	  --设置包裹中玩家选中的货币类型
	
	--排行榜
	{"GetSortList", "ssC"},
	
--帮会系统------------------------------------------------------------------------------------------------
	{"EnterTongArea", "I"},				--进入帮会副本
	{"LeaveTongArea", "I"},		--帮会副本传送处
	{"EnterTongChallengeArea", "I"}, --进入帮会挑战副本
	--帮会基础
	{"RequestCreateTong", ""},			--创建帮会之前判断是否可以创建
	{"CreateTong", "sS"},				--创建帮会  参数：帮会名称
	{"GetTongInfo", ""},				--获得帮会信息 
	{"GetTongMemberInfo", ""},			--获得帮会成员信息 
	{"GetTongRequestInfo", ""},			--获得申请入会信息 
	{"BeJoinTong", "Ib"},				--是否同意加入帮会 参数：申请人ID，同意或拒绝
	{"ChangePos", "IHH"},				--修改职位， 参数：玩家id，原来的职位，新职位
	{"GetTongLog", "I"},				--获取日志信息， 参数：日志类型
	{"GetAllTongSomeInfo", ""},  --获得所有帮会信息
	{"SearchTong", "s"},			--查询输入的小队名称信息
	{"GetAllPlayerSomeInfo",""},  --获得所有未加入小队的玩家信息
	{"SearchPlayer", "s"},			--查询输入的小队名称信息
	{"RequestJoinTong", "I"},			--申请加入某帮会，参数：要加入的帮会id
	{"RequestJoinTongByName", "s"},--申请加入某帮会，参数：目标名字
	{"ChangeTongPurpose", "S"},			--修改帮会宗旨，参数：新的宗旨
	{"KickOutOfTong", "I"},				--开除帮会成员，参数：成员id
	{"RecommendJoinTong", "ss"},		--推荐某人加入帮会，参数：角色名称，推荐信息
	{"LeaveTong", ""},					--成员退出帮会
	{"TongResign", ""},					--辞职
	{"InviteJoinTong", "I"},			--招募
	{"InviteJoinTongByName", "s"},--招募
	{"ResponseBeInviteToTong", "Ib"},	--响应招募
	{"ResponseBeRecommendJoinTong","Ib"},
	{"PreChangeTongName",""},
	{"ChangeTongName","s"},
	{"ChangeTongType","I"},				--类型转变
	{"UpTongType","I"},						--提升类型
	{"ResetTongType",""},					--充值类型
	
	{"ContributeMoneyChangeExp","I"}, --捐钱得经验
	{"ContributeMoneyChangeSoul","I"}, --捐钱得魂值
	{"ContributeMoneyChangeResource","I"}, --捐钱得军资
	{"GetTongContributeMoneyInfo","I"},	--捐钱信息
	
	--帮会建筑
	{"CancelCreateBuildingItem", "I"},	--取消正在/等待建造的建筑
	{"GetCreatingBuildingItemInfo", ""},--获取正在建造和等待建造的建筑模型信息
	{"CreateBuildingItem", "sI"},		--建造建筑模型
	{"GetTongLevelToFilterAvailableBuildingItems",""},--查取小队等级用于筛选可建项目
	{"GetTongBuildingList", ""},		--获取建筑信息
	{"CreateBuilding", "sII"},			--建造建筑，参数：建筑卷轴的名称、在包裹中的位置
	{"RemoveBuilding", "I"},			--拆除建筑，参数：建筑id
	{"BeRepairBuilding", "II"},			--修理或停止修理，参数：建筑id，客户端当前显示状态
	{"GetTongBuildingItem", "s"},		--获得帮会建筑物品
	
	{"GetCreatingProdItemInfo", ""},	--获得正在建造的物品信息
	{"TongFetchBuilding", ""},
	--帮会科技
	{"GetTongFightScienceInfo", ""},	--获取佣兵团战斗科技信息
	{"UpdateTongScience", "sC"},		--升级佣兵团战斗科技
	{"CancelUpdateScience", "sC"},		--取消升级中的佣兵团战斗科技
	
	--帮会战争
	{"GetTongChallengeInfoList", ""},	--获得帮会挑战列表
	{"GetTongBattleListInfo", ""}, --获得帮会挑战对战列表
	{"SendChallenge", "III"},			--向某帮会宣战，参数：战区Id, 驻地Id, 竞标金
	{"TongChallengeReborn", ""},  --玩家在帮会挑战副本里死亡重生
	{"RequestRetreatStation", ""}, --申请驻地撤退
	{"ExitTongChallenge", ""},		--退出挑战
	{"GetTongRelativeLine", ""},	--佣兵团所在战线
	{"RequestShowTongChallengeBattleCountWnd", ""},
	{"GetTongChallengeMemberListInfo", ""},
	{"JudgeOpenChallenge", ""},
	{"GetTongWarMemberListInfo", ""},
	{"RequestShowTongWarBattleCountWnd", ""},
	{"GetTongMonsAttackListInfo", ""},
	

	
	--帮会军械
	{"GetArmInfo", ""},					--显示所有军械
	{"CreateArm", "IsI"},				--制造机械兽，参数：类别，车间id
	{"DestroyArm", "II"},				--销毁机械兽，参数：机械兽id，当前状态
	{"GetArm", "I"},					--领取机械兽，参数：机械兽id
	{"RenameArm", "sI"},				--修改机械兽名称
	{"GiveBackArm", ""},				--归还机械兽，参数：机械兽id
	{"ForciblyGiveBack", "II"},			--归还机械兽，参数：机械兽id，当前状态
	{"BeRepairArm", "II"},				--修理/停止修理机械兽，参数：机械兽id，当前状态
	--帮会资源
	{"ContributeMoney", "I"},			--捐献金钱，参数：数量
	{"GetMoneyCanContribute", ""},

	--帮会制造中心
	{"CreateTongProdItem", "sII"},		--制造帮会物品，参数：名称、大类、放入的仓库
	{"CancelCreateProdItem", "I"},		--取消制造，参数：物品id
	{"QuikMoveItemFromCDepotToBag", "HHHH"},		
	
	--帮会资源
	{"RequestAddTongBuyOrder", ""},
	{"AddTongBuyOrder", "II"},
	{"CancelTongBuyOrder", ""},
	{"SellTongResource", "I"},
	{"FetchResource", ""},
	{"GetTongMarketOrderInfoList", ""},
	{"GetTongSellResInfo", "Ib"},
	{"GetTongFethResInfo", ""},
	{"GetTongMyResOrderInfo", ""},
	{"ShowTongSellResWnd", "I"},
	{"RandomShowTongSellResWnd", "I"},
	{"ShowMyResOrderWnd", ""},
	{"ShowTongResTransWnd", "I"},
	
	{"RequestFightInTriangle","I"},
	{"AgreedFightInTriangle",""},
	{"CancelFightInTriangle",""},
	{"TransToTongArea", "s"},
	
------------------------------------------------------------------------------------------------------
	{"RequestCreateArmyCorps",""},			--创建佣兵团之前判断是否可以创建
	{"CreateArmyCorps","sS"},						--创建佣兵团
	{"GetArmyCorpsInfo",""},						--获取佣兵团基本信息
	{"ChangeArmyCorpsPurpose","S"},			--修改佣兵团宗旨
	{"ChangeArmyCorpsName","S"},        --修改佣兵团名
	{"InviteJoinArmyCorps","I"},				--邀请入团
	{"InviteJoinArmyCorpsByName","s"},	--邀请入团
	{"ResponseBeInviteToArmyCorps","Ib"},
	{"GetArmyCorpsTeamInfo",""},				--获取佣兵团内所有佣兵小队信息
	{"LeaveArmyCorps",""},							--退团
	{"KickOutOfArmyCorps","I"},					--踢出佣兵团
	{"GetTargetTongMemberInfo","I"},		--获取目标佣兵小队的成员信息
	{"ChangeArmyCorpsPos","II"},					--修改权限
	{"PreChangeArmyCorpsName",""},
----------------------------------------------------------------------------------------------------------

--商会系统
	--商会基础

	{"OpenCofCNpcDialog","I"},			-- 打开商会npc对话框，得到商会列表，参数：npcid
	{"RequestJoinCofC","Is"},			-- 申请加入商会，参数：商会id,自我描述
	{"IntroduceJoinCofC","ss"},			-- 引荐其他玩家加入商会，参数：被引荐玩家姓名，对被引荐人的描述
	{"RequestCreateCofc", "s"},			-- 创建商会，param 商会名称
	{"LeaveCofc", ""},					-- 退出商会
	{"GetCofcInfo", "I"},				-- 打开商会主界面得到商会信息，参数：npcid
	{"GetCofCMembersInfo", ""},			-- 打开商会成员面板，得到相关信息
	{"RequestOpenCofCStock",""},		--请求开通商会股票
	

	{"GetCofcTechnologyInfo", ""},			-- 打开商会科技面板，得到相关信息	
	{"GetCofCLogInfo", "I"},				-- 打开商会日志面板，得到相关信息,参数：(0-人员 1--经济 2--科技 3--任务，4--所有)
	{"GetCofCFinancialLogInfo", ""},	-- 商会主界面中查看历史财报
	{"ModifyCofcPropose", "s"},			-- 商会主界面修改商会宗旨，参数：商会宗旨
	
	--商会成员面板
	{"KickOutCofCMember", "I"},			--将商会成员踢出，参数:玩家信息id
	{"ApproveJoinCofC", "I"},			--允许申请入会者加入商会，参数：玩家信息id
	{"RefuseJoinCofC", "I"},			--拒绝申请入会者加入商会，参数：玩家信息id
	{"ChangeCofCMemberPos", "Is"},		--修改会员职位,参数：玩家信息id，新的职位
	
	--商会科技面板
	{"SetCurTech","I"},					--申请设置当前科研项目，参数：科技id
	{"SetCofcCurrentTechnology","I"},				

	--商会股票
	{"GetCofCStockKMap", "IiI"},		--得到k线图数据。参数：股票代码；范围分钟数（负值）；间隔分钟数（15、2*60、8*60）
	{"GetCofCStockInfo",""},			--得到所有股票信息
	{"GetCofCStockOrderList","I"},		--得到某支股票的订单列表，参数：股票的id
	{"GetCofCStockMyDealingInfo", ""},	--得到我的买卖订单消息
	{"GetCofCFinancialReport", "Ii"},	--得到某支股票的财务报告。参数：股票id；距现在的星期偏移（负值）
	{"GetCofcMyHaveInfo", "I"},			--得到我对某支股票的持有情况。参数：股票id			
	{"CofcStockBuy", "III"},			--购买股票。参数：股票id，交易数量，交易价格
	{"CofcStockSell", "III"},			--销售股票。参数：股票id，交易数量，交易价格
	{"CofcStockDeleteOrder", "I"},		--撤销订单。参数：订单id
	--目标属性面板
	{"RequestAimInfo","I"},             --目标的角色ID;true--目标属性面板、false--小队成员属性面板
	--for stress test
	{"TestRpc","iissiisssssss"},
	--集体仓库相关
	{"GetCollectiveBagOnePageItemList","II"},		--获得某仓库所有物品信息，参数：类型，页数
	{"CollectiveBagAddItemFromOther","IIIII"},		--从个人包裹拖动物品到集体包裹，参数：个人包裹类型，格子位置，集体包裹类型，页数，格子
	{"BagAddItemFromCollectiveBag","IIIII"},		--从集体包裹拖动物品到个人包裹，参数：集体包裹类型，页数，格子，个人包裹类型，格子位置
	--盒子物品相关
	{"GetBoxitemInfo",			"IIIs"},				--右键包裹中的盒子物品，得到掉落的物品信息，参数：盒子物品所在的nRoomIndex, nPos
	{"SelectBoxState", "II"},							--查询盒子的状态 参数：盒子物品所在的nRoomIndex, nPos
	{"TakeAllBoxitemDrop",		"I"},				--提取盒子物品掉落的所有物品，参数：盒子物品id
	{"TakeOneDropItemByIndex",	"III"},				--提取盒子物品掉落的众多物品中的一个，参数：当前显示的页数，掉落的物品流水号,盒子物品id
	{"GetOnePageDropItem",		"III"},				--得到某一页要显示的掉落物品信息，参数：盒子物品id，当前显示页，所有物品总页数（每4个物品为一页）
	{"CollectiveBagMoveItemSelf","IIIII"},			--集体仓库物品内部移动，参数：集体仓库类型，起始物品所在页数，物品起始格子，物品目标页数，去拼目标格子；

	--生活技能相关
	{"ProduceByTwoMaterials","sIIsIII"},			--锻造类生产技能物品，参数：胚胎、品质材料、生活技能类型
	{"ProduceByProductName","sssI"},		--制药类生产技能物品，参数：产品名称，要生产的数量
	{"ProduceByProductNameAll","sss"},		--制药类生产技能物品---全部生产；参数：产品名称
	{"GetLiveSkillHasLearned",""},					--获得所有生活技能信息
	{"LearnLiveSkill","s"},							--学习生活技能
	{"WashLiveSkill", "s"},							--洗白生活技能
	{"GetPracticedInfo","C"},						--获得熟练度信息
	{"GetPracticedInfoForNpcPanel","C"},			--获得熟练度信息
	{"LearnLiveSkillExpert","ss"},					--专精学习
	{"GetLiveSkillExpert","s"},						--获得专精信息
	{"GetProductsCanMake","s"},
	{"ForgetLiveSkillExpert", ""},					--遗忘专精
	{"GetLiveSkillProdRemainCDTime", "sss"},		--获得剩余冷却时间
	--生活技能种花
	{"CareForFlower","I"},							--种花
	{"FlowerStateClean",""},						
	{"FlowerPerishGiveItem","b"},			
	--魂珠
	{"AvargeSoulCountToEquip", "II"},				--右键魂珠,将魂值平均分配给既有装备，参数：魂珠在包裹中的roomindex，pos
	
	--表情动作相应
	{"FacialAction", "Is"},					--做表情动作服务器的相应事件，参数:Npc名字，NpcID，表情动作名称
	
--阵法系统
	{"SaveBattleArrayPos", "sIIIIIii"},    --保存阵型
	{"SaveBattleArrayMemPos","IsIIIII"},   --保存列阵信息
	{"RevertToBattleArrayBook","Is"},      --还原阵法书
	{"UseBattleArraySkill","s"},           
	{"SaveBattleArrayShortCutIndex", "sI"},--保存阵法快捷栏位置
	
	--骑骆驼
	{"CheckCamelState",			"s"},	--检验骑骆驼状态（特殊）
	{"CheckCrazyHorseState",		"s"},	--检验骑疯马状态（特殊）
	{"SendSuccStateName","ss"},  --检验状态（一般）
	--道具区域选择
	{"SelectArea",			"fffs"},
	
	--问答系统
	{"CreateAnswerTbl","I"},--创建玩家选择答案结果表 参数  待定作用
	{"AnswerQuestion","sIIsII"},--参数( 题库名称,问题的ID,答案选项)(选择答案后调用)
	{"AnswerQuestionFinish","IsIII"}, --参数 ( 题目数量, 任务名称, 答题形式, 答题正确数量要求)
	--消耗物品类技能
	{"FightSpecialSkill",		"s"},--特殊技能
	--区域系统
	--{"GetPlayerAreaInfo","s"}, ---参数（区域名称）
	{"GetPlayerSceneAreaInfo","s"},---参数（场景名称）
	{"PlayerChangeAreaPlace","ffsb"},---
	{"GetTeammatePos",""}, -- 区域面板上获取队友位置
	{"StopGetTeammatePos",""},
	{"GetTongmatePos",""}, -- 区域面板上获取帮会成员位置 
	{"StopGetTongmatePos",""},
	{"StopGetFbPlayerPos", ""},
	{"GetBornNpcOnMiddleMap", ""},
	{"GetFbBossOnMiddleMap", ""},
	{"SendAreaPkOnMiddleMap", "s"},
	{"SendRobResOnMiddleMap", "s"},

	{"GetCampInfoByNum", "I"},

	--游戏设置
	{"SysSettingBegain",	""},
	{"SysSettingEnd",		""},
	{"GameSetting",			"iiiiiiI"}, --设置属性，参数：是否可以组队、是否可以交易、是否可以加入群、是否可以交友、是否可以入团,是否允许自动组队
	{"GetGameSettingInfo",	""},		--得到游戏设置信息
	{"SaveUISetting",		"bbbbbbbbbbbbbb"},
	
	-- 个人设置
	{"SetShowModeOfArmet", "b"},	-- 头盔显示状态
	
	--下线
	{"NotifyClientLeave", "CI"},	--向服务端发送下线请求
	{"NotifyToCharSelect", "I"}, --回角色选择界面
	{"ClientLogout", "CC"},			--下线通知
--	{"Exit2CharList", "" },		--返回角色选择界面
	{"CancelReadLeftTime", ""},  --取消读秒
	
	--召唤兽相关
	{"MasterAttackCommand", "II"},	--主角向宝宝发送攻击指令
	{"MasterRetreatCommand", "II"},	--主角向宝宝发送撤退指令
	{"MasterDisbandCommand", "II"},	--主角想宝宝发送解散指令
	{"MasterSpreadCommand", "II"},--命令宝宝展开
	{"MasterOrderServantMoveCommand", "IIII"},--命令宝宝移动
	{"TestEquipIntensifyAddAttr2", ""},
	{"AgreeTakeOverNpc", "II"},
	
	--商会运输车
	{"TruckFollow", "I"},
	{"TruckStop", "I"},
	{"TruckKillSelf", "I"},
	{"GetChallengParam", ""},
	
	--装备强化后的洗白
	{"EquipIntensifyBack", "IsdI"}, 	--要求洗白强化的装备，参数：装备type，装备名称，装备id,装备部位（0表示在包裹里）
	
	--道具商城
	{"BuyItemInToolsMall", "IsII"},    --购买东西, 参数:ItemType, itemName, 支付方式
	{"GiveItemInToolsMall","HsICss"},    --赠送他人，参数：itemType，itemName,ncount,支付方式，受赠者name
	{"GetAimPlayerSoulNum","I"},          --查询玩家身上的魂值  参数：要查看的玩家Id
    {"GetHotSaleItemInfo", "I"},--查询当前商城热卖的物品,参数：物品类型
	{"TakeYongBingBi", ""}, --提取佣兵币
	{"RequestTakeYongBingBi", "I"},--发送提取佣兵币的数目,参数：要提取的佣兵币数目
	--Pk开关
	{"OpenPkSwitch", "b"},						--打开PK开关
	
--/**旧PK代码**/
	{"OpenCSMWnd", ""},
	{"OpenMailWnd", ""},

	--插旗挑战
	{"JudgeDistance", ""},
	{"CreateChallengeFlag", ""},
	{"DestroyChallengeFlag",""},
	{"SendChallengeMessage", "I"},
	{"StartChallenge", "b"},
	
	--需求分配拾取方式
	{"SelectNeedAssign", "IHII"},
	{"SavePlayerAppellation","II"}, --保存角色称谓 参数：称谓内容
	{"EquipSmeltSoul","IsIIIIdIsII"},--装备炼化魂 参数：装备大类Id，装备名称，炼魂卷轴包裹位置、炼魂卷轴包裹格子、装备包裹位置、装备格子位置、装备Id、炼魂卷轴大类Id、炼魂卷轴名称、装备是从包裹出来的还是从角色属性面板上出来的
    {"EquipAdvance", "dIII"}, 		--装备进阶equipID, equipPart, stoneRoomIndex1, stonePos1
    {"ActiveJingLingSkillPiece", "dII"}, --激活精灵激活项 物品ID、精灵激活项ID、装备的部位
    {"AdvancedEquipReborn","dI"},    	--精灵激活项重生 物品ID、装备的部位
    {"ActiveJingLingSkill","I"},		--激活精灵技能
    {"RenewEquipDuraValue","III"}, --参数：装备部位，装备id,修理装备的支付方式
    {"RenewAllEquipDuraValue", "I"}, --修复身上所有装备
    {"RenewEquipInBagDuraValue", "I"},--仅修复背包里的装备
    {"RenewEquipOnWearingDuraValue", "I"}, --仅修复角色身上的装备
    {"SelectAcutionAssign", "IIIb"},
    
  --读条相关
	{"StopLoadProgress", ""},			--ESC键打断读条
		
	{"PlayActionCyc", "Isssbss"},
	
	-- 中地图相关
	{"GetMatchGameNpcTips", "s"},
	{"GetWarZoneState", "s"},
	{"GetFbPlayerPos", ""},

	{"PlayActionCyc", "Isssbss"},	
	--临时包
	{"ClearTempBagObj",	"I"},
	{"UseTempBagObj",	"I"},
	--幻兽
	{"IncubatePetEgg","sII"}, 		--孵化幻兽蛋 参数：名称，包裹，包裹格子。
	{"PetEggPourSoul","sI"},  --幻兽蛋注魂孵化成幻兽 参数：幻兽蛋名称、消耗魂值
	{"ThrowAwayPet","I"},			--放生幻兽 参数：幻兽id
	{"ChangePetName","Is"},		--幻兽改名 参数：新名称
	{"ThrowAwayPetEgg","s"},	--放生幻兽蛋 参数：蛋的名称
	{"UsePetSkillStone","sII"}, --幻兽灵石的使用 参数：名称，包裹，包裹格子。
	{"PreparePet","I"},			--准备幻兽 参数：幻兽id
	{"InitPetWndInfo",""},
	{"SendUserAdviceInfo",	"SHH"},
	{"QueryQuestionList", ""},
	--攻略系统
	{"FinishPlayerDirect",	"s"},
	{"GetDirectAwardItem",	"Hs"},
	{"AddPlayerDirectAction",	"sss"},
	{"UpdatePlayerDirect",	""},
	{"UseEquipIntenBackItem", "IIIIIIIs"},
	
	--生活技能代工
	{"AddNeedContractManufactureOrder", "ssIIs"},    --添加代工订单;参数：liveSkillName，direction，time,money,配方名
    {"SearchCharContractManufactureOrder", "I"},    --参数：当前查询页数
    {"CancelContractManufactureOrder", "I"}, --取消代工订单，参数：订单ID
    {"SearchCommonContractManuOrder", "ssIIII"}, --查找代工订单，productName, skillName, skillLevel, productLowLevel, productHighLevel, curPage)
    {"TakeContractManufactureOrder", "I"}, --代工，参数：代工订单

	--热更新相关,客户端启动时通知服务器端发送热更新代码进行更新
	{"SendIdToGas", "i"},  --热更新记录的id
	{"SendHotUpdateMsg", "Isss"}, --热更新信息及结果
	
	--玩家灵感值相关
	{"GetAfflatus_OffLineExp_Info", ""},
	{"ExChangeAfflatus", "I"},
	{"OpenExpOrSoulBottle","II"},
	
	{"UpdateChoosedMoneyPayType", "I"},--根据打开的商店类型（流通商店or通用商店），更新选择的支付方式；参数：
	{"EquipRefine", "dIsIs"},--装备精炼；参数：装备id，装备type，装备name，精炼石type，精炼石name
	{"UpdateShopTipFlag","II"},--参数：1、商店类型（1为流通，0为通用）；2、tip提示与否（1为提示，0为不提示）
	{"SetDefaultShopTipFlag", ""},--恢复关于商店提示信息的设置
	{"IdentityAffirm","s"},
	{"LockItemBagByTime","I"},
	{"OpenItemBagLockWnd",""},
	{"ItemBagLockTimeOut",""},
	
	{"EquipSuperadd", "Isd"}, --装备追加；参数：
	{"UpdateEquipEffectToGas","I"},
	{"OpenTongNeedFireActivity",""},--开启篝火活动
	{"CheckNeedFireIsOpen","I"},
	{"AddNeedFireItem","IIss"},
	{"YYRequest","sss"},
	{"YYlogin","S"},
	
	{"GetCapableOfBuyingCouponsList", ""},
	{"BuyCouponsUsingJinBi", "Is"}, --购买点券，参数：编号，名称
	{"GetBoughtCouponsInfo", ""},
	{"UseVIPItem","sHH"},
	{"ChangeSceneByMsg","IIII"},
	
	{"WeiBoRequestStart", "sss"},
	{"WeiBoLoginCheck", "s"},
}
	 


