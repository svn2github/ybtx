gac_gas_require "framework/message/MessageFun"
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

local funlist = 
{ 
	{"ClearDb2GacMsg",""},
	
	{"OpenMessageBox","I"},

	{"RetSendUserAdviceInfo","I"},
	{"RetQuestionsList","S"},
--GM指令打开面板
  {"ReturnOpenPanel","i"},
  {"OpenMailWnd",""},
-- 
	{ "ByteCall",				"cc" },
	{ "Reload",					"S"  },
	{ "GMPrint",				"S"  },
-- 远程变量读取 相关RPC
	{ "Remote_MS_GetValue",		"S" },
	{ "Remote_MS_GetValues",	"S" },
	{ "Remote_MS_GetType",		"S" },
	{ "Remote_ResumeResult",	"S" },
	{ "Remote_ReceiveResult",	"S" },
	{ "RetOpenGMConsole",	"b" },

--角色属性相关RPC
	{ "SetExp",					"IIIII"  	},
	{ "SetClass",				"II" 	},
	{ "SetCamp" ,				"II"	},
	{"NotifyClientCreateNewCoreObj",         ""},
	{	"RetSetMercenaryLevel",				"Ib"},
	{	"RetSetMercenaryIntegral",		"I"},
	{	"ExposePlayer",		"I"},
	
-- 角色战斗属性
	{ "RetGetTotalTicket", "I"},
	{ "SendUserGameID", "i"},
	{ "RetDelTicket", "I"},
--Player 技能相关
	{ "UpdateFightSkillError",  "s"      }, --技能升级学习失败
	{ "UpdateFightSkill",      "sCC"      }, --公共技能的学习和升级，天赋技能的升级
	{ "ReturnNonFightSkill",    "s"     }, --非战斗技能学习
	{ "ReturnLearnSkill",       "sC"     }, -- 返回技能名称
	{ "RetAllFightSkill", 			"sCC"	}, --技能名称、 级别 、
	{ "AddTalentSkill",             "sC"   }, --技能名称、 级别 、
	{ "RetAddFightSkillStart",    ""},	--技能返回成功
	{ "ReCommonlySkill",        "s"      }, 
	{ "ReAssociateAttr",        "b"       },
	{ "ReWeaponAttr",          "bs"       },
	{ "RemoveTempSkill",       "I"     }, 
	{ "OnCastTempSkill",       "I"     }, 
	{ "CreateTempSkill",        "sIII"       },
	
	{	"ReturnSeries",			"I"},  --返回系信息
	{	"ReturnSkillNode",		"I"}, --返回结点信息
	{	"RetAddFightSkillEnd",""},
	
	{	"ReturnGetCompensateItemInfoBegin", "I"},
	{	"ReturnGetCompensateItemInfo", "IsIII"},
	{	"ReturnActivationCodeCompensateItem", "Iisi"},
	{	"ReturnGetCompensateItemInfoEnd", "I"},
	{	"LearnNoneFightSkill",	"Is"},
	{	"CancelLearnNoneFightSkill", "I"},
-- 洗天赋
	{"ClearAllGenius",        "" },

-- 冷却时间相关
	{	"SetCoolDownTime",		"III"	},
	
-- 召唤兽技能相关
	{ "CreateServantSkill",      "s" }, -- 添加召唤兽技能
	{ "DelServantSkill",		 "s" }, -- 删除召唤兽技能
--npc for test
	{ "RetrunNpcIdForTest", 		"I"		},
	
-- 登陆相关RPC
	{	"RetReSetLogin",		"b"},		--告诉客户有人在用他的号
	{ "ReturnCheckUser",		"bis" },
	{ "ReturnCheckUserByActivationCode",		"sss" },
	{ "ReceiveAntiIndulgenceExitGame",		"" },
	{	"ReturnStartApex", ""},
	{ "ReturnCharList",			"IsIIIfIIIsCsIIssssssIIIIIIIII" },
	{ "ReturnDelCharList",			"IsIIIfIIIIIIssssssIIIIIIIII" },              --test add
	{ "ReturnDelCharListEnd",			"" },                      --test add
	{ "ReturnCharListEnd",		"" },
	{ "ReturnRecruitInfo",		"III" },
	{ "RetReturnRecruitInfo",		"IIII" },
	
	{ "ReturnCreateRole",		"i" },
	{ "DelCharEnd",				"" },
	{ "CompleteDelCharEnd",				"" },
	{"GetBackRoleEnd",      ""},
	{"DelCharErr", "I"},
	{"RuturnGetBackRole", ""},
	{"ReturnNameDiff",    "b"},
	{ "ReturnCharListBegin", "I"},--是否抢注角色活动开启，false为没有
	{	"RetServerIpInfo",	"sIsI"},
	{	"RetAccountSucc", ""},
	
	{ "InitMoneyInfo",				"ddd" },
	{ "InitMainPlayer",				"IIsI" },
	{ "InitOtherPlayer",			"IIIIiiICiiiisHsssssssssIbIH" },
	{ "CreatePlayerEnded", 			"sI" },
	{"ReturnLoginWaitingQueue",		"I"},
	{"ReturnDelWaitingUser",""},
	{"ReturnUpdateWaitSequence",	"I"},
--	{ "InitAgileInfo", 			"IIII" },
	{"ReturnPreChangePlayerName",	"b"},
	{"ReturnChangePlayerName",		"bI"},
--获取进入角色选择界面网络延时
	{ "RetGetDelayTime",           ""},

-- 角色状态、技能释放
--	{ "AdjustBulletTime",		"ICf" },	-- ObjGlobalId, BulletId, AdjustTime
	{ "ForceDirection",			"IC" },
	{ "AddFx",					"Iii" },
	{ "MustRemoveFx",	"II" },
	{ "RemoveFx",				"II" },
	{ "OnInitQianXing",		"Ib" },
	{ "OnQianXing",         		"Ib" },
	{ "OnBlind",            		"Ib" },

	{ "OnFallOver",         		"Ib" },
	{ "MoveStop", "" },
	{ "DoNormalAttackAni", "I" },
	{ "CancelNormalAttack", "" },
	{ "CancelAutoTrack", "I" },
	{ "OnShowDeadMsgBox", "IHH" },--后一个参数，古战场会用到，死亡的次数
	{ "OnPermitReborn", "s" },
	{ "CloseRebornMsgWnd", "" },
	{ "ChangeSceneCancelSelectGround", "" },
--npc
	{ "InitNpc",				"IIiiICiisHI" },
	{ "CreateCreature",	"iiiiCiis" },	
	{ "ChangedGridPosEnd", ""},
	{"RetShowTextNpc", "IsIIH"}, --NpcShowContent
	{"RetNpcDropItem", "IsIb"},
	{"RetNpcDropItemEnd","I"},
	{"RetGetSingleGridDropItem","I"},
	{"RetSelectAllDropItems","b"},
	{"NotifyClosePickUpWnd",""},
	{"RetGetDropItemGrid","I"},
	{"NotifyAuctionDropItem", "IsII"},
	
	{"FinishAuction", "I"},
	{"NotifyClearBagPos",""},
	{"NotifyChangeAuctionPrice","II"},
	{"SetDestroyBtnEnable",""},
	{"RetCancelAuction",""},
	{"NotifyCloseAuctionWnd",""},
	{"SendNpcRpcMessage", "sIs"},
	{"SendNpcRpcMessageByMessageID", "sII"},
	{"NotifyNpcDropItem","Ib"},
	{"NpcDoSkillTalk","IsIIs"},
	{"NpcDoSkillAdvise","sIs"},
	{"NpcDoSkillAdviseRepl","sIsss"},
	{"ShowTheaterNpcTalk","IsIsh"},
	{"ShowTheaterNpcDoTalk","IsIsss"},
	{"DoAlertPhaseEffect", "II"},
	{"DoAlertEndEffect", "I"},
	{"ClearNpcAlertEffect", "I"},
	
	{"RetShowFuncNpcOrObjTalkWnd", ""},  --npc对话框
	{"ShowNpcTalkWnd", "sIIb"},	--npc交互对话
	{"RetNpcTalkHeadInfo", "Ib"},
	
	--龙穴活动
	{"RetShowDragonCaveWnd", "I"},
	{"RetEnterDragonCave", "s"},
	{"SendMsgToLocalSence", ""},
	{"SendMsgToOtherSence",""},
	{"SendMsgToEverySence",""},
	
	--区域副本
	{"RetShowAreaFbSelWnd","I"},
	{"RetAreaFbBossNum","sIIII"},
	{"RetIsJoinAreaFb","sIIII"},
	{"RetDelAllAreaFb", ""},
	{"RetIsChangeOut", "II"},
	{"RetInsertAreaFbList", "s"}, 
	{"OpenAllAreaFb", ""},
	
	{"ChangeBackgroundMusic","s"},
	{"RevertBackgroundMusic",""},
	
	--佣兵团刷怪本
	{"RetShowMercenaryMonsterFbWnd",""},
	{"RetIsJoinMercenaryMonsterFb","sI"},	
	
	--挑战任务的副本
	--{"RetIsJoinDareFb","ss"},
	{"RetAreaFbListBegin","ss"},
	{"RetAreaFbListEnd",""},
	
	--领域副本
	{"RetIsJoinScopesFb","sII"},
	
	--剧情副本
	{"SendScenarioFinishInfo","s"},
	{"RetShowJuQingWnd","I"},
	{"RetIsJoinSenarioFb","sII"},
	{"SendScopesExplorationDPS","sIIIII"},
	{"SendScopesExplorationDPSEnd",""},
	{"RetChangeCameraPos","sb"},
	
	--帮会战相关
	--{"RetShowWarZoneSelWnd", "II"},
	{"ReturnGetMoneyCanContribute", "dd"},
	{"RetEnterWarZone", "bs"},
	{"OpenTongChallengeExitBtnWnd", ""},
	{"OpenTongChallengeBattleCountWnd", ""},
	{"OpenTongMonsAttackExitBtnWnd", ""},
	{"OpenTongMonsAttackCountWnd", ""},
	{"OpenTongWarExitBtnWnd", ""},
	{"OpenTongWarBattleCountWnd", ""},
	{"TongChallengeBegin", "I"},
	{"TongChallengeBattleBegin", "I"},
	{"TongWarBattleBegin", "I"},
	{"TongMonsAttackBegin", "I"},
	{"InitTongDownTimeWnd", "II"},
	{"CloseTongChallengePanel", ""},
	{"CloseTongWarPanel", ""},
	{"InitTongBattlePersonalCountWnd", "IIIII"},
	{"UpdateTongBattlePersonalCountInfo", "III"},
	{"UpdateCountInfo", "III"},
	{"OpenTongMonsAttackCountWnd", ""},
    {"ReturnTongProfferCount", "I"}, 	--返回团贡献积分数目
    {"ReturnTongLevel", "II"}, 			--返回图案贡献等级
    {"ReturnProfferPoint","I"},			--返回团贡献积分数目
    {"ReturnTeamProfferPoint","I"},		--返回队贡
	{"ShowRobResZeroExitWnd", "s"},
	{"ShowRobResOverExit", ""},
	{"RetRobResourceWnd", "s"},
	{"RetOpenAutoSeekWnd", "sI"},
	{"RetOpenAutoSeekWndBegin", ""},
	{"RetStationSeekWnd", "IIsIIII"},	
	{"RetFbStationAutoSeek", "sII"},
	{"RetNotForageWnd", ""},
	{"ShowCountDown", "sI"},
	{"RetCharLearnedTech", "sI"},
		
	
	{"OpenConfirmWnd", "ss"},
	{"OpenSecConfirmWnd", "ssI"},
	{"CreateTbl", ""},
	{"InsertTbl", "sssI"},
	
	
	{"RetNpcHeadInfo", "I"},
	
	{"SetActionPanelWarnValue", "I"},
	
	--矿洞副本
	{"OreMapInfoShow","sII"},
	{"JoinAreaSceneMsgBox","IIsI"},
	{"TeamJoinSceneMsgBox","IIsI"},
	{"CreateAreaSceneMsgBox","s"},
	{"IsExitSceneMsgBox","b"},
	{"InviteJoinSceneMsgBox","IIssIs"},
	
	--插旗挑战
	{"ShowChallengeMessage","Is"},
	{"ShowChallengeAdvertMessage","IsIII"},
	{"PropChallengeMessage","Is"},
	{"PropChallengeAdvertMessage","IsIII"},
	{"SetChallengeFlag","b"},
	{"CancelChallengeWnd","b"},
	
	--脑电波小游戏
	{"CheckHeadFishing","sI"},
	
	--物品使用
	{"NotifyUseItemProgressLoad",  "Is"},	--通知客户端开始使用物品读条
	{"NotifyUseItemProgressStop",  ""},		--通知客户端结束使用物品读条
	
	{"NotifySandGlassLoad",  "I"},
	{"NotifySandGlassClose", ""},
	-----------------
	
--交互对象
	{"RetObjShowContentWnd","I"},-- 交互对象GlobalID
	{ "InitIntObj",  "IsI"},
	{"RetShowAnimation","I"},
	{ "RetShowQuestObjTalkWnd",  "I"},---- 交互对象GlobalID
	{"RetShowCollObjWnd","I"},-- 交互对象GlobalID
	{"NotifyCloseCollWnd",""},
	{"RetCanCollItemEnd","II"},-- 交互对象GlobalID,用于判断是否读进度条
	{"RetCanCollItem","IsI"},-- 物品大类，物品名，数量，物品所放的格子号
	{"RetCollGetGridItem","Ib"},--物品所放的格子号，bool值表示提取该格子的物品是否成功
	{"RetCollAllItemGrid","I"},
	{"RetSelectAllGridItems","b"},--bool值表示提取该所有格子的物品是否成功
	{"NotifyLoadProgressBar",  "Ibs"}, --通知客户端读取进度条（交互对象系统专用）
	{"ResourceLoadProgress",  "Is"}, --通知客户端读取进度条（装载资源）
	{"NotifyTeammateTransport","s"}, --通知被队友召唤，队友的名称
	{"IntObjChangeModel","Iss"},
	{"IntObjChangeState","IH"},
	{"NotifyStopProgressBar",""},
	{"NotifyIntObjEffect","Is"},
	{"NotifyIntObjDoAction","IsH"},

	--交互对象
	{"PlayerTransportSetGrid",""},-- 这个函数主要是针对本地图内跨trap寻路

-- 物品空间操作

	{ "ReturnBreakItemBegin",""},		
	{ "ReturnBreakProducts","CsC"},		
	{ "ReturnBreakItemEnd","bCC"},		
	{ "AddBreakItemExp","I"},	
	{ "InitBreakItemExp","I"},			
	{ "ReturnAddMoney","d"},									 --添加金钱
	{ "ReturnAddMoneySlowly","dI"},						 --添加金钱
	{ "ReturnAddMoneyError","d"},							 --添加金钱
	{ "ReturnAddTicket","d"},								 --添加佣兵币
	{ "ReturnAddTicketError","d"},						 --添加佣兵币
	{ "ReturnAddBindingMoney","d"},								 --添加绑定金钱
	{ "ReturnAddBindingMoneyError","d"},						 --添加绑定金钱出错
	{ "ReturnAddBindingTicket","d"},								 --添加绑定佣兵币
	{ "ReturnAddBindingTicketError","d"},						 --添加绑定佣兵币出错
		
	{ "RetCharBagInfo",		"dCCCsI" },
	{ "RetCharBagInfoEnd",	"" },
	{ "RetItemTypeInfo", "dCsi" },
	{ "SetItemBindingTypeByID", "di" },
	{ "RetItemLeftTime", "di" },
	{ "RetItemRoom", "C" },
	{ "RetAddItemToGrid", "d" },
	{ "RetAddItemToGridEnd", "dCIsI" },
	{ "RetAddItemToGridEndEx", "dC" },
	{ "RetRefreshBag", "" },
	{ "RetAddItem", "dCC" },
	
	{ "RetItemMakerName", "sd" },		-- 物品制造者
	{ "RetAddItemAllGMEnd", "CsC"},		--"大类，小类，数目"(负责事件和表示添加的命令全部完成)
	{ "RetAddItemGMError", "Css"},		--"大类，小类，错误说明"
	{ "RetPlayerItemSound", "Cs"},
	
	{ "RetDelItemFromGrid",			"dCCb"},					--"空间，位置，ItemID"
	{ "RetQuestDelItemEnd","CC"},				--"空间，位置"
	{ "RetDelItemError",		"c"},
	{ "RetDelItemAllGMEnd",		"CsC"},
	{ "RetDelItemGMError",	"Css"}, 		--"大类，小类，错误说明"
	
	{ "RetSplitItemEnd",	"CCCC"},			--"空间a，位置a，空间b,位置b"
	{ "RetSplitItemError",	"cCCCC"},		--"错误信息，空间a，位置a，空间b,位置b"
	
	{ "RetQuickMoveEnd",""},           --Room,Pos
	{ "RetQuickMoveError","c"},
	
	{ "RetMoveItemEnd",	"CCCC"},					--"空间a，位置a，空间b,位置b"
	{ "RetMoveItemError",	"CCCC"},
	{ "ReturnPlaceBag",    "CCCC"}, 				--空间，位置，巢的空间，使用了哪个RoomIndex(0:表示交换包)
	{ "ReturnPlaceBagError",   ""},
	{ "ReturnFetchBag",  "CCCd"},						--nSlot,nRoom,nPos
	{ "ReturnFetchBagError", ""},	
	{ "ReturnChange2Bag", "CC"},						--nASlotRoom,nASlot,nBSlotRoom,nBSlot
	{ "ReturnChange2BagError", ""},	
	{ "ReturnDelBag" ,"C"},
	{ "ReturnDelBagError", "c"},	
	{ "RetCleanBag", ""},
	{ "ReturnSetBagState",""}, 							--设置包裹的使用状态
	{ "RetMoveItemByGridID", "dCC"},
	{ "RetReplaceItemByGridIDEnd", "CCCC"},

	{ "ReturnGetDepotInfo", "d"},
	{ "ReturnOpenDepotSlotError", ""},
	{ "RetUseItem", "CCdI"},					--Room,Pos,nItemID,eEquipPart	

-- 提示信息发送 相关RPC
--	{ "Message",				"I" },
	{ "Talk",					"Is" },
	{ "AddItemMsgToConn",        "d"}, 
	
--获取主角视野内对象
	{ "ReturnObjInSight","IH" }, --对象ID, 对象类型
	
-- 快捷栏
	{ "ReturnShortcut", "CCsII"}, --Pos, Type, Arg1, Arg2, Arg3
	{"ReturnShortcutEnd","I"},
	
--装备系统，穿在人物身上的装备
	{ "UpdateModel",		"I" },
	{ "UpdateAni",			"I" },
	{ "UpdateCoreInfo",		"I"},
	{ "AoiAddEquip",				 "IIss"},	
	{"RetEquipInfo",				"CsdCI" },	--初始化静态装备
	{ "RetJewelryInfo", 			"CsdC" }, 	--初始化饰品(静,动)
	{ "RetFetchEquipByPart",       	"Cb" },  	 --取下装备,包含删除装备
	{ "RetSwitchTwoRing",          	"CsCs"    },  --交换左右戒指
	{ "RetSwitchWeapon",          	"CsCs"    },  --交换主手副手武器
	{"RetCommonWeaponInfo",   		"dsIIIfffIIIs"}, --[[	返回装备武器的信息,
															参数（ID, 显示名称，绑定类型，基础等级，
															当前等级，DPS,
															当前已有升级魂数，当前已有强化魂数，
															附加属性1，附加属性1的值，
															附加属性2，附加属性2的值,
															强化到第几阶段，套装类型（极品or普通），套装名,
															护甲片设定的属性1，护甲片设定的属性2，
															护甲片设定的属性3，护甲片设定的属性4,
															强化三阶段的天赋名对应的序号,耐久上限，当前耐久值）
														--]]
	{"RetCommonArmorInfo",			"dsIIIIIIIIIIs"}, --[[返回护甲的信息
															参数（ID, 显示名称，绑定类型，基础等级，
															当前等级，装备产出时的属性1，装备产出时的属性2，装备产出时的属性3，静态装备属性
															当前已有升级魂数，当前已有强化魂数，
															附加属性1，附加属性1的值，
															附加属性2，附加属性2的值，
															强化到第几阶段，套装类型（极品or普通），套装名，
															护甲片设定的属性1，护甲片设定的属性2,
															强化三阶段的天赋名对应的序号,耐久上限，当前耐久值）
														--]]
	{"RetCommonRingInfo",			"dsIIIIIIIIs"}, --[[返回戒指的信息
															参数（ID, 显示名称，绑定类型，基础等级，
															当前等级，DPS，静态装备属性
															当前已有升级魂数，当前已有强化魂数，
															附加属性1，附加属性1的值，
															附加属性2，附加属性2的值，
															强化到第几阶段，套装类型（极品or普通），套装名，
															护甲片设定的属性1，护甲片设定的属性2,
															强化三阶段的天赋名对应的序号,耐久上限，当前耐久值）
														--]]
	{"RetCommonShieldInfo",			"dsIIIIIIIIIIs"}, --[[返回盾牌的信息
															参数（ID, 显示名称，绑定类型，基础等级，
															当前等级，
															当前已有升级魂数，当前已有强化魂数，
															附加属性1，附加属性1的值，
															附加属性2，附加属性2的值，
															强化到第几阶段，护甲片设定的属性1，护甲片设定的属性2,产出属性1的值、产出属性2的值、产出属性3的值、产出属性4的值,
															强化三阶段的天赋名对应的序号,耐久上限，当前耐久值）
																
														--]]
	{"NoticeIntensifySuccess",		"dI"},		--返回装备强化成功，参数：物品id,装备part
	{"NoticePourIntensifyError","dI"},         -- 注入强化魂错误的信息提示， 参数：物品ID，魂数
	{"NoticePourIntensifySuccess","d"},			-- 注入强化的魂成功		
	{"NoticeAdvanceError","d"},               	-- 装备进阶失败
	{"NoticeAdvanceSuccess", "d"},				-- 装备进阶成功，参数：物品ID
	{"NoticeAbsorbSoul", "Id"},					-- 装备吸取魂，参数：物品ID，魂的数目
	{"NoticePourUpgradeError", "dI"},			--注升级魂错误，参数：物品ID，魂的数目
	{"NoticePourUpgradeSuccess","d"},     		--注升级魂成功
	{"RetEquipAddAttrInfo", "dsIsIsIsIsIsI"},    --返回装备强化（4~9阶段）的附加属性和对应的值
  	{"RetEquipIntensifyInfo", "dIsIsIIsIII"},  --返回装备强化相关信息到客户端
	{"UpdateEquipIntensifyInfo", "d"},		--装备强化失败消息返回
--组队系统相关
	{ "ReturnInviteMakeTeam",		"IsICH"	},	--邀请者邀请组队
	{ "ReturnAppJoinTeam",			"IsIC"	},	--队长得到申请组队信息
	{ "UpdateAssignMode",			"H"		},	--更新分配方式
	{ "NotifyNoTeam",				""		},	--通知 自己已经不再队伍中
	{ "ReturnGetTeamMemberBegin",	""		},	--返回获得队伍队员开始
	{ "ReturnGetTeamMember",		"IsbIICH"},	--返回获得队伍队员
	{ "ReturnGetTeamMemberEnd",		"IH"	},	--返回获得队伍结束
	{ "UpdateTeammateIcon",			""		},	--通知更新队友头像
	{ "NotifyOffline",				"I"		},	--通知其已经下线
	{ "NotifyOnline",				"I"		},	--通知其已经上线
	{ "UpdateAuctionStandard",		"I"		},	--通知拍卖的品质标准
	{ "UpdateAuctionBasePrice",		"I"		},	--通知拍卖的底价标准
	{ "ReturnUpdateTeamMark",		"III"	},	--返回小队标记信息，参数：标记类型，被标记对象类型，对象id
	{ "ReturnUpdateTeamMarkEnd",	""		},	--返回小队标记信息结束
	{ "ReturnUpdateHPMPByCharID",	"IIIII"	},	--根据CharID刷新小队成员血值魔值
	{ "SendTeamMemberLevel",		"IC"	},	--返回小队成员等级
	{ "ReceiveTeamMemberEntityId",		"II"	},

--组队排队
	{"GetTeamListBegin","I"},	--nType
	{"GetTeamList","IsIIIIsI"},	
	{"GetTeamListEnd","I"},	--nType
	{"RenewTeamList","I"},	--nType
	{"GetLineTeamList","II"},
	{"GetLineTeamListBegin",""},
	{"GetLineTeamListEnd",""},
	{"ChangeBtnState","I"},
	
	
--任务
	{"RetIsShowQuestTraceBack","b"},	--是否要显示任务追踪面板
	{ "NotifyTeammateShareQuest","Iss"}, --共享任务的队伍ID, 玩家名，任务名称
	{ "RetGetQuest", "sIIHH"},			  --任务名称,任务状态，接任务时间,任务需求是否已经满足,任务对应的物品索引ID
	{ "RetGetQuestLoop", "sI"},
	{ "RetAcceptQuest", "sHsI"},
	{ "RetFinishQuest", "ss"},		  --任务名称	 	
	{ "RetQuestVar", "ssI"},          --任务具体细节
	{"RetQuestBuffVar","ssI"},   --返回任务的Buff需求（交任务状态需求）完成数
	{ "RetAddQuestVar", "ssI"},          --任务具体细节
	{ "RetGiveUpQuest", "sb"},		  --放弃任务成功
 	{ "RetIncrementQuestVar", "ssI"}, --任务的更新(任务名称，任务变量名称，任务增量)
 	{ "ClearLoopQuest", "s"},
 	{ "InitMasterStrokeQuestLevTbl", ""},

	{ "RetAcceptTask",  ""}, --返回接受任务领取
	{ "RetShowTakeQuest", "s"},	--返回显示任务领取面板，参数(任务名称)
	{	"RetShowTakeDareQuest","ss"},--返回挑战任务领取面板
	{ "RetShowFinishQuest", "sH"},--返回显示任务完成面板，参数(任务名称,佣兵任务物品要给那一个)
	{ "NotifyQuestFailure", "s" }, --任务失败通知客户端
	{ "RetShowQihunjiZhiyinWnd", "b" },
--隐藏任务yy
	{ "SendHideQuestSign", "s"}, --触发的隐藏任务

--打开任务池窗口
	{	"RetSendMercQuestPool","sI"},
	{	"RetShowMercQuestPoolWndEnd","Is"},
	{	"RetSendMercTempQuestPool","sI"},
	{	"RetSendMercTempQuestPoolEnd","b"},
	
	--佣兵等级指引
	{"RetCheckInMercenaryInfo",""},
	{"RetUpdateMercenaryLevelTrace","IIsssIIIIII"},
	{"RetUpdateMercenaryLevelAwardItem","s"},
	{"RetShowMercenaryLevelAward",""},
	{"SetMercenaryLevelFinishAwards","s"},
	{"ClearMercenaryLevelFinishAwards",""},
	{"ShowMercenaryLevelMessage","I"},
	{"RetShowOpenYbAssessWnd",""},
	
--更新Npc头顶信息
	{ "UpdateNpcHeadSign", ""},
	{	"RetSetNpcDirection", "II"},
--聊天相关
	--[[ 操作的定义如下： 
		SUBSCRIBE = 1,
		UN_SUBSCRIBE = 0
	--]]
	{ "TalkResult", "sbsII"},							--聊天后的返回消息(角色名,是否成功，内容，角色的ID，角色的等级)
	{ "TalkContent", "ssII" },						--接收到消息 (返回名字 聊天的内容，角色的ID，角色的等级)
	{"SecretChatFail",  "s" },							--返回聊天失败消息
	{ "ChannelMessage", 			"IsISI" },   	--收消息(说话人id，说话人姓名，频道，消息内容)
	{ "RetSetDefaultChannelPanel", 	"b" },			--返回设回default
	{ "RetRenamePanel",	 		"b" },				--返回重命名面板基本信息
	{ "RetDeletePanel",				"b" },			--返回删除定制面板信息
	{ "RetGetChannelPanelInfo", 	"III" },		--获取面板配置信息（频道ID，定制操作，面板位置）
	{ "RetGetPanelInfo", 			"sII" },		--获取面板信息 （名称，位置，操作）
	{"RetInfoByID",                  "dCs"},			--返回ID，BigID，Item
	{"ReturnSaveChatInWorldShow",""},
	{"ReturnSaveChatInCHItemShow",""},
	{"ReturnSaveChatInCHMoneyShow",""},

--社群系统
	{"ReturnAddFriendClass","bIs"},			--添加分组返回结果,参数：添加分组成功与否标志,添加新组的ID,新添加组的名称
	{"ReturnSearchPlayer","IsIb"},			--返回精确查找好友的基本信息 参数：玩家ID，玩家name，玩家等级，玩家是否在线 
	{"ReturnSearchPlayerEnd",""},			--返回发送好友信息结束标志
	{"ReturnInviteToBeFriend","sII"},		--通知玩家自己被加为好友 参数：要加自己的玩家name,玩家要将自己加入的组ID,要加自己的玩家ID
	{"ReturnAddFriendToClass","IIsHCCCsCI"},		--响应添加好友 参数：好友组ID, 玩家ID, 好友name, 等级, 阵营, 职业, 小队人数, 所在场景, 是否在线
	{"ReturnGetAllFriendClassBegin",""},	--发送分组信息开始
	{"ReturnGetAllFriendClass","Is"},		--向客户端发送好友分组信息 参数：好友组ID，好友组名
	{"ReturnGetAllFriendClassEnd",""},		--发送分组信息结束
	{"ReturnGetAllFriendInfo","IIsHCCCsCI"},	-- 向客户端发送玩家好友信息 参数：好友组ID, 玩家ID, 好友name, 等级, 阵营, 职业, 小队人数, 所在场景, 是否在线
	{"ReturnGetAllFriendInfoEnd",""},		--向客户端发送玩家好友信息结束标志
	{"PrivateChatReceiveMsg","IsS"},		--向对方发送私聊内容 参数：发送内容玩家的ID,发送内容玩家name，发送的内容
	{"ReturnDeleteMyFriend","bII"},			--删除好友成功后通知玩家  参数：删除成功与否的标志, 对方的ID, 对方所在组的ID
	{"ReturnBeDeleteByFriend","II"}, --删除好友成功后通知对方 参数：删除自己的玩家ID，该玩家所在的自己的好友组ID
	{"ReturnDeleteBlackListMem","bI"}, --删除黑名单返回标志 参数：成功标志, 黑名单成员ID 
	{"ReturnMoveToBlackList","b"},  --返回添加黑名单的成功与否 参数：成功与否的标志
	{"ReturnChangeToClass","b"},   --返回移动好友标志 参数：返回成功与否
	{"ReturnDeleteFriendClass","bI"}, --删除好友组成功标志 参数：删除好友组成功标志, 好友组ID
	{"ReturnCreateFriendGroup","I"}, --创建群 参数：群ID
	{"ReturnSearchGroup","Iss"}, --返回精确查找群 参数：群ID，群name，群主
	{"ReturnSearchGroupEnd",""}, --精确查找群结束标志
	{"ReturnGetAllFriendGroupBegin",""}, --发送好友群开始
	{"ReturnGetAllFriendGroup","IsIssI"},  --向客户端发送好友群信息 参数：--好友群序号，好友群名称，好友群类型，好友群标签，好友群公告
	{"ReturnGetAllFriendGroupEnd",""}, --发送好友群结束
	{"ReturnGetAllGroupMemInfo","IsCIC"}, --群成员信息 参数：--玩家ID,玩家name,玩家职位标识(1:群主，2:管理员(最多只有三个)，3:普通成员),--玩家所在群ID,玩家是不是在线(1--在线，2--离线)
	{"ReturnGetAllGroupMemInfoEnd","I"}, --发送好友信息结束 参数：群ID
	{"ReturnAddGroupMem","IsCIC"}, --群成员信息 参数：--玩家ID,玩家name,玩家职位标识(1:群主，2:管理员(最多只有三个)，3:普通成员),--玩家所在群ID,玩家是不是在线(1--在线，2--离线)
	{"ReturnInitAssociationInfo",""}, --初始化社群面板结束
	{"RecevieRequestJoinIntoFriendGroup","IsI"}, --群主或管理员收到申请消息 参数：玩家ID，玩家name，群ID
	{"ReturnRequestJoinIntoFriendGroupEnd",""}, --加入群被批准后
	{"PublicChatReceiveMsg","IsS"},  --向群里面的别的玩家发送群消息 参数：群Id, 发消息的玩家Name,消息内容text
	{"ReceiveInviteToGroup","IsIs"},    --向玩家发送邀请加入群请求 参数：玩家ID，玩家name，群ID，群name
	{"ReturnLeaveGroup","I"},  --返回离开群 参数：群ID
	{"ReturnDisbandGroup","I"},   --解散群返回 参数：群ID
	{"ReturnSetCtrlType","III"}, --返回设置管理员 参数：玩家ID，群ID，设置/取消的类别 2-设置管理员；3-取消管理员
	{"NotifyFriendOnline","II"},    --通知好友玩家上线 参数：好友组Id，玩家ID
	{"NotifyFriendOffline","II"}, --下线通知好友 参数：好友组ID，好友ID
	{"ReceiveMemberLeaveGroup","II"}, --返回剔除人 参数：玩家ID，群ID
	{"ReceiveBeKickOutOfGroup","I"}, --收到被剔除的消息 参数：群ID
	{"ReturnRenameFriendClass","bIss"},--返回修改好友组名称 参数：成功与否的标志,组ID,新组名,旧组名
	{"NotifyFriendGroupOnline","II"}, --返回上线通知好友群成员 参数：玩家ID ,群ID
	{"NotifyFriendGroupOffline","II"},--返回下线通知好友群成员 参数：玩家ID ,群ID
	{"ReturnChangeShowSentence", "bs"},--返回修改心情寄语 参数；心情寄语
	{"ReturnSaveAssociationPersonalInfo",""}, --返回保存个人信息 参数：保存成功与否
	{"ReturnGetAssociationPersonalInfo","CssCCCCs"}, --返回查看的个人信息 参数(心情,寄语,兴趣,体型,个性,交友状态,类型,
																												--	最新动态第一个,最新动态第二个,最新动态第三个)
	{"ReturnGetAssociationMemberInfo","IIsIIssssIIIIs"}, --返回玩家信息 参数(玩家ID,心情,寄语,阵营,等级,佣兵团,商会,所处的区域(地图)
																												--	兴趣,体型,个性,交友状态,类型,最新动态第一个,最新动态第二个,最新动态第三个)
	{"ReturnGetMemberIdbyNameForPrivateChat", "IsS"}, --返回通过对方名字开始私聊 参数：对方ID, 对方名字, 聊天信息
	{"ReceiveCofcChatSendMsg","sS"}, --向商会中其他成员发送群聊消息 参数：发消息的玩家name，消息内容
	{"ReturnAddToBlackList",  "Ibs"}, --返回添加到黑名单
	{"ReturnChangeGroupDeclare", "IS"},
	{"ReturnInitFriendGroupInfoEnd",""},
	{"ReceiveSendOfflineMsg","IssS"},
	{"ReturnUpdateFriendInfo", "IHCsII"},			--刷新指定好友信息 好友组ID, 等级, 小队人数, 所在场景, 好友ID
	{"ReturnUpdateOnlineFriendInfo", "IIHCsI"},		--刷新在线好友信息 好友组ID, 好友ID, 等级, 小队人数, 所在场景
	{"ReturnUpdateOnlineFriendInfoClassEnd", "I"},	--刷新在线好友信息结束一个组别 组ID
	{"ReturnRefuseGroupMsgOrNot","II"},--开启/屏蔽群消息
--延时
	{"RetGetDelayTime",			""},		--返回服务器延时

--邮件系统
	{"RetDeleteMail", 		"I"},					--返回删除邮件成功与否信息，参数（是否成功）

	{"RetMailListBegin",    ""},               --邮件列表返回之前的gas2gac，
	{"RetMailList",		"IIsIsId"},				--获取邮件列表，参数（邮件ID，发信人ID，标题，邮件状态，最后修改时间-单位是秒，发件人姓名）
	{"RetSysMailList",  "IssId"},				--获取系统邮件列表，参数（邮件ID，发件人姓名（npc或系统），标题，状态，最后修改时间-单位是秒）
	{"RetMailListEnd",    ""},
	{"RetSendEmail",		"b"},				--返回发送邮件成功与否，参数（是否成功）
	{ "SendMailGoodsBegin", ""}, 				--返回邮件id ,根据id发送物品
	{"RetTakeAtachment",	"bI"},				--返回获取附件，参数（是否成功,mailID）
	
	{"RetGetMailGoods",        "IIIssI"},			--返回附件详细信息,参数(EmailID, 物品id,第几个格子,物品类型名,物品名 及 物品数量)
	{"RetGetMailGoodsEnd",         "I"},			--返回附件信息结束
	{"RetTakeAtachmentByIndex",     "IbI"},		--通过玩家选择提取附件的返回 参数(第几个格子,是否提取成功,mailID)
	{"RetMailTextAttachmentInfo",         "dss"},			--返回邮件文本附件的信息
	{"RetSoulPearlItemInfo",         "dI"},			--返回魂珠的信息
	{"RetOreMapItemInfo",         "ds"},			--返回矿藏图的信息
	{"RetPickOreItemInfo",         "dII"},			--返回采矿工具的信息
	{"RetGetMailInfo",		"IIIsSIIsIi"},		--[[
	
													返回获取邮件信息，参数:
														邮件ID，
														发件人id，
														收件人id，
														信件标题，
														信件内容，
														信件状态，
														附赠金钱,
														发件人发信时间，
														最后更新时间，
														发件人姓名
												--]]
	{"RetCheckMailTextAttachment", "sssSIi"},	--返回邮件文本附件的内容。参数：时间、发送者、标题、内容、类型（0为普通用户邮件，1为系统邮件）

	{"RetGetSendMoneyCess", "Ii"},	
	
--魂镶嵌宝石
	{"ReturnOpenHole","b"},                     --打孔功能的返回, 参数: 是否成功
	{"ReturnInlayStone","b"},			  --镶嵌功能的返回, 参数: 是否成功
	{"ReturnGetInlayedHole","IIIIs"},          --初始化镶嵌的宝石   参数: 面板ID ,部位ID , 孔的ID, 宝石类型,  宝石名字
	{"ReturnGetOpenedHole","III"},           --初始化打孔的孔       参数: 面板ID ,部位ID , 孔的ID
	{"NotifyAddAttr","III"},			  --通知增加附加属性    参数: 面板ID ,部位ID , 孔的ID(4 or 8)
	{"ReturnRemovalStone","b"},		  --摘除宝石的返回   参数: 是否成功
	{"ReturnSynthesisItemEnd","b"},	  --合成结束通知   参数: 是否成功
	{"ReturnSynthesisItem","dIs"},		  --合成 返回  参数:  ItemID, type  , name
	{"UpdateAllHoleInfoBegin",""},		  --重新初始化所有数据 开始通知
	{"SendAllHoleInfoEnd",""},			  --初始化结束通知
	{"RetWhiteStoneAppraise","IsII"},	  --白宝石鉴定返回  参数: type  , name  ,  ItemID,   第几次鉴定的结果
	{"RetWhiteStoneAppraiseFirstTime","Is"},	  --白宝石第一次鉴定返回12个高级宝石信息  参数: type  , name
	{"RetTakeAppraisedStone","b"},		  --提取返回  参数: 是否成功
	{"NotifyOpenPanel", "I"},			  --通知开启后续面板  参数: 面板ID
	{"UsePanel", "I"},			  	  --通知正在使用哪个面板
	
	--NPC shop
	{"RetNpcTradeSucc", ""},   								--NPC交易成功
	{"ReturnPlayerSoldGoodsListBegin",""},
	{"ReturnPlayerSoldGoodsList", "dsIIdII"},		--返回已售物品列表，参数：物品类型，物品名称，物品数目，index, 物品id
	{"ReturnPlayerSoldGoodsListEnd", ""},			--返回已售物品列表结束
    {"ConcealOrNotPlayersTradeRequest", "b"},         --拒绝所有玩家交易请求成功
  --玩家交易
	{"GotTradeInvitation","Is"},              --通知玩家有交易请求；参数：交易邀请者ID，邀请者名字
	{"ReturnChoosedTradeItem","IssId"},        --显示交易邀请者要交易的物品：参数：物品显示在交易栏的位置，物品类型，物品名称，要交易的物品数目, 物品id
	{"ReturnInviteeChoosedItem","IssId"},      --显示交易被邀请者要交易的物品，参数：在交易栏的位置，物品类型，物品名称，要交易的物品数目, 物品id
	{"ReturnChoosedTradeMoney","d"},          --显示交易邀请者交易的金币数目，参数：金币数目
	{"ReturnChoosedInviteeTradeMoney","d"},   --显示交易被邀请者交易的金币数目，参数：金币数目
	{"ReturnDenyedTrade",""},                 --通知玩家对方拒绝交易，
	{"ReturnAgreeToTrade","I"},               --通知玩家对方接受交易，  
	{"ReturnCancelChoosedItemSuc",""},        --通知玩家取消交易物品的请求成功，
	{"ReturnResetItemList","IIsId"},           --返回给玩家新的交易列表（取消某个要交易的物品后），参数：物品显示在交易栏的位置，物品类型，物品名称，要交易的物品数目, 物品id
	{"ReturnResetItemListEnd","I"},            --返回给玩家新的交易列表（取消某个要交易的物品后）结束
	{"ReturnReplaceItem","IssIdIIII"},             --替换某个交易的物品（交易栏已满的情况下），参数：物品显示在交易栏的位置，物品类型，物品名称，要交易的物品数目, 物品id
	{"ReturnLockedTrade",""},                 --通知玩家对方已经锁定了交易
	{"ReturnCanceledTrade",""},               --通知玩家对方取消了交易
	{"RetSubmitedTrade",""},                  --通知玩家对方提交了交易
 	{"RetGetTradeCess","IIi"}, 
 	{"ReturnChoosedTradeItemError", "II"},	  --要交易的物品不被允许交易，参数：viewIndex
  	{"ReturnReplaceItemError", "II"},			  --replace交易物品不成功
  	--得到多个玩家请求交易的信息
  	{"GetPlayersTradeRequestBegin", ""},	  --开始通知多个玩家请求交易
  	{"GetPlayersTradeRequest", "Is"},		  --发送多个请求交易的玩家信息，参数：玩家id， 玩家姓名
  	{"GetPlayersTradeRequestEnd", ""}, 		  --结束通知多个玩家请求交易
  	
  
  --寄售交易所；1、2、3、4分别为：出售列表、收购列表、出售物品、收购物品  
  {"RetCSMTotalNo",     "II"},                --物品列表总数目，参数：面板位置，物品总数目
  {"RetCSMOrderList",   "dIsssdIdd"},                --服务器端返回物品要显示的相关信息，参数：面板位置，订单ID，角色名称，物品类型，物品名称，价格，数目，终止时间, 物品id
  {"RetCSMOrderListEnd","I"},                --物品列表返回结束，参数：面板位置
  {"RetCSMBuyOrderFinishNum", "II"},				 --角色收购列表显示完成数目，参数：订单ID，完成数目
  {"RetCSMAddOrderSuc","Ib"},                 --添加物品订单成功,参数：面板位置，是否成功
  {"RetCSMCancelOrderSucc","Ib"},               --取消物品订单成功,参数：面板位置，是否成功
  {"RetCSMTakeAttachmentError", "b"},           --提取物品完成，参数：是否提取成功
  {"RetCSMPay2OrderError", "b"},                 -- 像收购者出售符合条件的物品
  {"RetCSMBuyOrderError", "I"},                --购买是否成功，参数：是否成功
  {"RetIfHaveItemToTake", ""},                 --收购物品订单中是否有可提取物品
  {"RetGetCSMRememberPrice", "II"},				--返回上次玩家出售或收购的物品价格，参数：面板no（出售还是收购面板），物品价格
  {"RetCloseCSMWnd", ""},
--货币流通系统
 	{"ReturnTicketToTicket", "bii"},							--佣兵币兑换绑定的佣兵币 参数：是否成功 增加的佣兵币，增加的绑定的佣兵币
 	{"ReturnTicketToMoney", "bii"},								--佣兵币币兑换金币 参数：是否成功，增加的佣兵币 增加的金币
 	{"ReturnMoneyToMoney", "bii"},								--金币币兑换绑定的金币 参数：是否成功  增加的金币，增加的绑定的金币

--按键映射
	{"ReturnAllKeyMaps",				"ssI"},		--返回所有按键映射Key表
	{"ReturnAllKeyMapsEnd",				""},		--返回结束通知
	{"ReturnSetKeyMap",					"b"},		--返回通知设置按键映射操作是否成功
	{"ReturnMouseKey",					"IIIIIII"},	--返回鼠标操作设置
	{"ReturnChangeMouseCtrlBeforeGame",	"b"},		--返回在游戏之前进行的鼠标设置
	
--排行榜
	{"RetSortListBegin",	""},		--开始返回排行版信息
	{"RetSortList",			"IisCd"},	--返回排行版信息
	{"RetSortListEnd",		"ss"},		--返回排行版信息结束
--帮会系统
	--帮会基础
	{"ReturnRequestCreateTong",	""},			--创建帮会之前的判断返回
	{"ReturnCreateTong",		""},		
	{"ReturnJoinInTong",		""},			--返回加入佣兵团
	
	--参数：团id，团名称，团主名称，团当前人数，团人数上限，团等级，团荣誉，团功勋，团资金，团当前粮草，团粮草上限，团宗旨
	{"ReturnGetTongInfo", "IssIIIIIIIISHHHIIIs"},		--帮会主界面信息
	
	{"ReturnGetTongMemberInfoStart", ""},		--返回成员信息开始
	{"ReturnGetTongMemberInfo", "IsIIIIIIIIs"},	--返回成员信息，参数：角色id，角色名称，在帮会中的职位，帮贡，等级，职业，角色名称,心情寄语，心情，是否在线,团贡,功勋
	{"ReturnGetTongMemberInfoEnd", "I"},		--返回成员信息结束,参数：帮会人员上限
	{"ReturnGetTongRequestInfoBegin", ""},	
	{"ReturnGetTongRequestInfo", "IsIIII"},	--发送申请信息  参数：申请者id，申请者名字，申请者等级，申请者职业。
	{"ReturnGetTongRequestInfoEnd", "I"},		--发送申请信息结束，参数：申请信息人数上限
	{"ReturnBeJoinTong", "b"},					--返回是否同意加入帮会 参数：同意或拒绝
	{"ReturnPosChanged", "H"},					--返回在帮会中的职位
	{"ReturnChangePos", "b"},					--返回修改职位是否成功，参数：是否成功
	{"ReturnGetTongLog", "IIS"},				--返回日志信息，参数：时间，类型，内容
	{"ReturnGetTongLogEnd", ""},				--返回日志信息结束
	{"ReturnGetAllTongSomeInfo", "IsIIsIIII"},		--返回所有帮会信息，参数：帮会id，名称，人数
	{"ShowFoundTong","s"},             --显示查询到的小队
	{"ReturnGetAllTongSomeInfoEnd", ""},		--返回所有帮会信息结束
	{"ReturnGetAllPlayerSomeInfo", "IsIIfII"},		--返回所有未加入帮会玩家的信息，参数：玩家id，名称，等级，职业，战斗力，国贡，声望
	{"ShowFoundPlayer","s"},             --显示查询到的玩家
	{"ReturnGetAllPlayerSomeInfoEnd", ""},		--返回所有未加入帮会玩家信息结束
	{"ReturnChangeTongPurpose", "b"},			--返回修改帮会宗旨是否成功
	{"ReturnKickOutOfTong", "b"},				--返回开除帮会成员是否成功
	{"ReturnBeOutOfTong", ""},					--通知离开帮会的人关闭面板
	{"ReturnTongResign", "b"},					--辞职是否成功
	{"ReseiveInviteJoinTong", "Iss"},			--招募
	{"ReseiveResponseBeInviteToTong", "sb"},	--响应招募
	{"ReseiveRecommendJoinTong", "ISS"},
	{"ReturnPreChangeTongName", ""},
	{"ReturnGetTongRelativeLine", "H"},			--返回佣兵团当前战线
	{"ShowTongMsg","I"},							--通知提升佣兵小队类型成功
	{"TongTypeChanged","I"},					--通知小队类型改变
	{"OnRequestJoinInTong",""},
	{"ReturnTongContributeMoneyInfo","III"}, --返回捐钱余额
	{"ReturnContributeMoney",""},
	{"RetChangeInfoWnd",""},   ---返回弹出面板
	
	
	-----上面的改成佣兵小队了
	-----下面是现在的佣兵团
	{"ReturnRequestCreateArmyCorps",""},		--请求创建佣兵团成功
	{"ReturnCreateArmyCorps",""},						--创建佣兵团成功
	{"OnArmyCorpsPosChanged","I"},					--通知权限
	{"ReturnArmyCorpsInfo","sIISsIII"},			--返回佣兵团基本信息
	{"ReturnPreChangeArmyCorpsName",""},
	{"OnArmyCorpsNameChanged","S"},         --团名改变
	{"OnArmyCorpsPurposeChanged","S"},			--宗旨改变
	{"OnInviteJoinArmyCorps","Iss"},				--邀请入团
	{"ReturnArmyCorpsTeamInfoBegin","II"},	--返回佣兵小队信息
	{"ReturnArmyCorpsTeamInfoEnd",""},			--返回佣兵小队信息
	{"ReturnArmyCorpsTeamInfo","IssIIIIII"},	--返回佣兵小队信息
	{"OnLeaveArmyCorps","b"},								--退团返回
	{"OnKickOutOfArmyCorps","b"},						--踢出团返回
	{"OnBeKickOutOfArmyCorps",""},					--踢出团通知被踢得所有人
	-----佣兵团end
	
	
	--帮会资源
	{"ReturnRequestAddTongBuyOrder", ""},
	{"ReturnGetTongSellResInfo", "III"},
	{"ReturnGetTongFethResInfo", "II"},
	{"ShowTongSellResWnd", "Ib"},
	{"ShowMyResOrderWnd", ""},
	{"ShowTongResTransWnd", ""},
	{"RetIssueFetchQueryWnd", "sIIII"},
	
	
	{"ReturnGetTongMarketOrderInfoListBegin", ""},
	{"ReturnGetTongMarketOrderInfoList", "SSSII"},
	{"ReturnGetTongMarketOrderInfoListEnd", ""},
	{"ReturnGetTongMyResOrderInfo", "SSSII"},
	
	--帮会建筑
	{"ReturnGetCreatingBuildingitemInfo", "sIIsIsIsIsIsIsI"},	--参数：正在建造的名称，用的时间，id，6个等待建造的名称、id
	{"ReturnGetTongBuildingList", "IsffIII"},	--返回帮会建筑信息，参数：建筑id，名称，横坐标，纵坐标，当前生命，状态, 最大等级）
	{"ReturnGetTongBuildingListEnd", ""},		--返回建筑信息结束
	{"ReturnCreateBuilding", "b"},				--建造建筑成功
	{"ReturnRemoveBuilding", "b"},				--拆除建筑成功
	{"ReturnGetCreatingProdItemInfo", "sIIsIsIsIsIsIsI"},		
	{"TongMoneyMsgToConn", "II"},		
	{"FilterAvailableBuildingItems","I"}, --返回佣兵小队等级用于筛选可建筑项目,参数：佣兵小队等级				
	--帮会科技
	{"ReturnGetTongFightScienceInfoBegin", "CC"},	--返回佣兵团科技信息开始
	{"ReturnGetTongFightScienceInfo", "sCII"},		--返回佣兵团科技信息
	{"ReturnGetTongFightScienceInfoEnd", ""},		--返回佣兵团科技信息结束
	
	--帮会战争
	{"ReturnGetTongChallengeInfoListBegin", "HH"},
	{"ReturnGetTongChallengeInfoListEnd", ""},
	{"ReturnGetTongChallengeInfoList", "ssII"},	--返回佣兵团挑战列表信息，参数：团名称，团长名称，战区，驻地
	{"ReturnTongChallengeMemberListInfoBegin", ""},
	{"ReturnTongChallengeMemberListInfo", "sIIII"},
	{"ReturnTongChallengeMemberListInfoEnd", ""},
	{"ReturnTongLevelInfo", "b"},
	
	{"ReturnTongWarMemberListInfoBegin", ""},
	{"ReturnTongWarMemberListInfo", "sIIIII"},
	{"ReturnTongWarMemberListInfoEnd", ""},
	{"ReturnTongWarAwardListInfoBegin",""},
	{"ReturnTongWarAwardListInfo","sIIII"},
	{"ReturnTongWarAwardListInfoEnd",""},
	{"ReturnTongGetExploitSum", "I"},
	{"ReturnCampGetExploitSum","III"},
	{"ShowNoneAwardWnd",""},
	{"ShowNoTongNoneAwardWnd",""},
	{"ShowNoPlayerGetAwardWnd",""},
	{"ReturnGetTongBattleListInfoBegin", ""},
	{"ReturnGetTongBattleListInfo", "ssII"},
	{"ReturnGetTongBattleListInfoEnd", ""},
	{"ReturnTongMonsAttackListInfoBegin", ""},
	{"ReturnTongMonsAttackListInfo", "sII"},
	{"ReturnTongMonsAttackListInfoEnd", ""},
	{"ReturnSendChallenge", "b"},					--返回帮会宣战信息，参数：是否成功
	{"ShowTongChallengeStatisticsWnd", "sIIIsIII"},
	--帮会军械
	{"ReturnGetTongArmList", "IssIIIsI"},		--返回军械信息，参数：军械id，类型，名称，等级，耐久，状态，领取者名字，所在用的军械库建筑id
	{"ReturnGetTongArmIdAndNum", "II"},			--返回军械建筑信息，参数：建筑id，已经被占用的数量
	{"ReturnGetArmInfoEnd", ""},				--返回军械信息完毕
	{"ReturnCreateArm", "b"},					--返回制造机械兽是否成功，参数：是否成功
	{"ReturnDestroyArm", "b"},					--返回机械兽销毁是否成功，参数：是否成功
	{"ReturnGetArm", "b"},						--返回领取机械兽是否成功
	{"ReturnRenameArm", "b"},					--返回修改机械兽名称是否成功
	{"ReturnGiveBackArm", "b"},					--返回归还机械兽是否成功
	{"ReturnForciblyGiveBack", "b"},			--返回强制归还机械兽是否成功
	{"ReturnBeRepairArm", "b"},					--返回修理/停止修理机械兽是否成功
	--帮会资源
	{"ReturnGetTongAllResInfo", "II"},		--返回帮会资源信息，参数：帮会金钱和四种资源数量
	{"ReturnGetTongAllResInfoEnd", ""},			--返回帮会资源信息结束
--	{"ReturnContributeRes", "b"},				--返回捐献资源是否成功
	{"ProvideParam", "s"},

   {"NotifyJoinFightInTriangle", ""},   --
   {"RetRequestFightInTriangle","b"},
   {"RetCancelFightInTriangle",""},
   {"RetAgreedFightInTriangle","b"},
--商会系统
	--商会基本
	{"RetOpenCofCNpcDialogBegin",""},			-- 准备开始传商会列表信息等
	{"RetOpenCofCNpcDialog","IsI"},				-- 传某npc处商会列表信息，参数：商会id，商会名称、商会成员人数
	{"RetOpenCofCNpcDialogEnd",""},				-- 传商会列表信息结束
	{"RetRequestCreateCofc", ""},				-- 创建商会成功									
  	{"RetGetCofcInfo","ssIIIIIIIIIIsIIs"},  	-- 商会主界面中除了任务以外的相关信息，--参数：自己的职位，商会名称，等级，资金，商业指数，在线商会人数，总人数，人气值，
  																						--资源产量加成，上周商会盈利，本周商会盈利，杂物回收加成，目前正在升级科技名称， 现有科技等级，总科技等级，商会宗旨
  								
  	{"RetGetCofcQuestInfo", "s"},				-- 商会主界面中的任务信息，参数：任务名称
  	{"RetGetCofcQuestInfoEnd", ""},				-- 返回任务信息结束
  	{"RetModifyCofcPropose", ""},				-- 修改商会宗旨成功	
  	
  	--商会成员面板
  	{"RetGetCofCMembersInfoBegin", "" },		--准备开始传商会成员信息
  	{"RetGetCurCofCMembersInfo","IssIssssII"},		--传商会成员信息，参数：玩家信息id, 玩家姓名, 职位, 等级, 种族, 职业，心情寄语，最新动态，心情，是否在线
  	{"RetGetCofCMembersInfoEnd", "I"},			--传商会成员信息结束，开始显示在客户端,参数：在线玩家数目
  	
  	{"RetGetCofCAppInfoBegin",""},				--准备开始传申请加入商会的玩家信息
  	{"RetGetCurCofCAppInfo","IsIssss"},			--传商会申请者信息，参数：玩家信息id, 玩家姓名,, 等级, 种族, 职业, 引荐人, 描述
  	{"RetGetCofCAppInfoEnd","I"},				--传商会申请者信息结束，开始显示在客户端,参数：在线申请入会者数目
	{"RetKickOutCofCMember",""},        --将玩家从商会中删除之后通知客户端
		
	--商会科技面板
	{"ReturnGetCofcTechnologyInfo", "III"},			--传商会科技相关信息，参数：科技Id ,科技名称, 科技等级, 已有科技等级, 总科技等级数
	{"ReturnGetCofcTechnologyInfoEnd", "I"},				--传商会科技信息结束，参数：现在正在升级科技id

	--商会日志
	{"ReturnLogBegin",""},						--返回商会日志信息开始
	{"ReturnLog","sII"},                --返回商会日志信息 参数：日志内容、日志类型(0-人员 1--经济 2--科技 3--任务)、创建时间
 	{"ReturnLogEnd",""},                --返回商会日志信息结束
 	
	--商会股票
	{"RetGetCofCStockKMapBegin", ""},			--股票交易记录开始
	{"RetGetCofCStockKMap", "III"},				--股票交易记录信息：交易时间、交易量、交易累计总额
	{"RetGetCofCStockKMapEnd", "II"},				--股票交易记录结束

	{"RetGetCofCStockInfoBegin",""},			--准备开始传商会股票信息
	{"RetGetCofCStockInfo","IsIIsIIIII"},		--开始传股票相关信息,参数：股票代号，名称，执有量，成本，涨幅，交易量，叫卖价（所有中最低的），叫买价（所有中最高的），最高价，最低价
	{"RetGetCofCStockInfoEnd",""},				--传商会股票信息结束，
	
	{"RetGetCofCStockOrderListBegin", "I"},		--得到股票的订单列表，参数：股票代号
	{"RetGetCofCStockOrderList", "IIII"},		--得到股票的订单列表，参数：类型（0为买单，1为卖单）、订单号、价格、数量
	{"RetGetCofCStockOrderListEnd", "II"},		--买单总股数，卖单总股数
	
	{"RetGetCofCStockMyDealingInfoBegin", ""},			--得到我的股票订单信息	
	{"RetGetCofCStockMyDealingInfo", "IIsIIIsIIII"},		--得到我的股票订单信息，参数：参数：订单流水号、代码、名称、挂单数量、类型、挂单价格、涨幅、叫卖、叫买、持有、成本
	{"RetGetCofCStockMyDealingInfoEnd", ""},
	
	{"RetGetCofCFinancialReport", "sIssIIIIIIIIIIsIsIsIsIsI"},	--参数：股票名称；股票编号；期间开始时间；期间结束时间；
																--	商会等级；商会金库资金；期间收入；售出股票；期间交易总量；发出红利；我持有股数；我的持有成本；
																--	商会人数；商会活跃度；依次为第一到第五大股东的角色名和持有股数
	{"RetGetCofcMyHaveInfo", "II"},				--得到我对某支股票的持有情况。参数：持有数量，成本
	{"RetDeleteOrder","b"},              --撤销订单后向客户端返回操作成功与否
	
--目标属性面板
	{"RetAimInfoEnd",        "I"},               --通知客户端显示目标属性面板;true--目标属性面板、false--小队成员属性面板
	{"RetFightingEvaluationInfoInfo","ffffIIf"},
	{"RetAimFightingEvaluationInfoInfo","IffffIIf"},
	{"RetAimEquipInfo",	"CsdCI" },				--初始化静态装备
--装备鉴定
	{"ReturnIdentifyResult","b"}, --通知客户端鉴定成功与否
--护甲片设定
	{"ReturnArmorPieceEnactment","b"}, --通知客户端设定成功与否
	
--集体仓库相关
	{"ReturnGetCollectiveBagOnePageItemListStart", "I"},				--返回仓库物品信息开始，参数：类型，每页格子最大数量
	{"ReturnGetCollectiveBagOnePageItemListEnd", "II"},				--返回仓库物品信息完毕，参数：仓库类型，页数
	{"ReturnCollectiveBagMoveItemSelf", "bII"},						--返回集体仓库内部物品移动是否成功，参数：类型，页数
	{"ReturnCollectiveBagAddItemFromOther", "bII"},					--返回从个人包裹拖动物品到集体包裹是否成功，参数：是否成功，集体包裹页数，格子
	{"ReturnBagAddItemFromCollectiveBag", "bII"},					--返回从集体包裹拖动物品到个人包裹是否成功
	
--盒子物品掉落相关
	{"RetTakeAllBoxitemDrop",			"I"},					--提取所有盒子物品掉落的物品 boxitemID
	{"RetGetBoxitemInfoBegin",			""},					--准备开始显示掉落的物品
	{"RetGetBoxitemInfo",				"IssI"},					--传盒子物品掉落的物品信息，参数：物品类型，物品名称，物品数目
	{"RetGetBoxitemInfoEnd",			"III"},					--传物品信息结束，参数：掉落物品总页数，当前页数,盒子物品id
	
--生活技能相关
	{"ReturnProduceByTwoMaterials",					"bIsd"},	--锻造类生产技能物品是否成功,产品类型,名称,ID
	{"ReturnProduceByProductName",					"b"},		--制药类生产技能物品是否成功----生产某数量的
	{"ReturnGetLiveSkillHasLearnedBegin",			""},	--开始发送生活技能信息
	{"ReturnGetLiveSkillHasLearned",				"sII"},	--发送生活技能信息
	{"ReturnGetLiveSkillHasLearnedEnd",				""},	--发送生活技能信息结束
	{"ReturnLiveSkillRefine",						"bIsI"},--精魂淬炼是否成功
	{"ReturnGetPracticedInfoBegin",					""},	--开始发送熟练度信息
	{"ReturnGetPracticedInfo",						"sI"},	--发送熟练度信息
	{"ReturnGetPracticedInfoEnd",					"sI"},	--发送熟练度信息结束
	{"ReturnGetPracticedInfoForNpcPanelBegin",		""},	--开始发送熟练度信息
	{"ReturnGetPracticedInfoForNpcPanel",			"sI"},	--发送熟练度信息
	{"ReturnGetPracticedInfoForNpcPanelEnd",		"sI"},	--发送熟练度信息结束
	{"ReturnGetLiveSkillExpert",					"sI"},	--发送专精信息
	{"ReturnGetProductsCanMakeBegin",	"sI"},	--得到可制造的药品开始
	{"ReturnLearnNewDirection",			"sss"},	--得到可制造的药品。参数：物品名称
	{"ReturnGetProductsCanMake",			"s"},	
	{"ReturnGetProductsCanMakeEnd",		""},	--得到可烹饪的食品结束
	{"ReturnLiveSkillExp",							"Is"},	--得到技能经验。参数：经验，技能名
	{"ReturnGetLiveSkillProdRemainCDTime",			"sI"},	--返回剩余冷却时间
--生活技能种花
	{"CultivateFlowerStart",					"sI"},
	{"UpdateFlowerState",							"H"},		--更新花的状态(正常,长虫,缺阳光)
	{"UpdateFlowerHealthPoint",				"i"},		--更新花健康值
	{"AddFlowerHealthPoint",				"i"},		--更新花健康值
	{"UpdateFlowerGetCount",				"i"},			--更新花卉收获次数
	{"CultivateFlowerSkillBtnEnable", "i"},		--更新花卉技能冷却时间
	{"PlayFlowerSkillEffect", "i"},		--更新花卉技能冷却时间
	{"CultivateFlowerSkillCoolDown",	"i"},
	{"FlowerGiveItemCoolDownCD",	""},
	{"FlowerEnd",	""},
	{"ShowGatherItemsWnd",	"IsIIs"},
-- 小游戏
 	{"BeginSmallGame","sI"},--通知客户端，小游戏开始
 	{"ExitSmallGame",""},--退出小游戏
 	{"RetMsgBeginSmallGame","sI"},--发送消息,告诉客户端是否开启小游戏
 	{"CloseSmallGameMsgWnd",""},--超出了小游戏的响应时间
 	{"PlayerSmallGameSuccFx","sI"},--小游戏成功同步特效

--阵法系统
	{"ReturnSaveBattleArrayPos", "Id"},				--返回保存阵法
	{"ReturnGetAllBattleArrayPosInfo","IsIIIIII"},	--返回所有的阵型信息
	{"ReturnGetAllBattleArrayPosInfoEnd",""},		--返回阵型信息结尾标志
	{"ReturnGetAllBattleArrayMemPosInfo","IIIIII"},	--返回所有列阵信息
	{"ReturnGetAllBattleArrayMemPosInfoEnd",""},	--返回列阵信息结尾标志
	{"ReturnSaveBattleArrayMemPos", "b"},           --返回保存阵法
	{"ReturnRevertToBattleArrayBook","b"},			--返回阵法技能转换成阵法书的结果
	{"ReturnDeleteBattaleShape","b"},				--返回删除阵型信息成功与否的标志
	{"RetCommonBattleArrayBookInfo","diiiii"},		--返回该玩家动态信息表里的记录
--{"RetCommonBattleArrayBookInfoEnd",""},
	{"ReturnSaveBattleArrayShortCutIndex", "b"},	--返回保存阵法快捷栏位置
	{"ReturnGetAllBattleSkill","Is"},				--返回阵法技能
	{"RetCloseBattlePanel","I"},						--关闭保存阵型的面板 参数：物品ID
	
	
	--骑骆驼
	{"RetCheckCamelState",			"sb"}, --验证状态的返回
	{"RetPhysicalStrengh",		"I"},	 --体力设置
	{"RetPhysicalState", 			"b"},		--任务完成后通知客户端关闭面板
	--使用物品
	{"CreatItemSelector","IsI"},
	{"SetItemCoolDownTime","sII"},
	
--表情动作响应
	{"OtherPlayFacial", 		"Is"},			--表情动作相应，参数（对方角色ID，播放动作名)
	{"NpcDoActionState", 		"Iss"},			--NPC做表情动作, 参数（NpcID， 玩家名， 动作名)
	{"DoFacialAction", "ssI"},	
	{"PlayFacialActionFailed", "s"},
	-- 交互动作
	--{"DoAlternatingAction",		"IIss"},			-- 非战斗的交互动作，参数(玩家ID, 对象ID，动作状态)
	{"RetHoldThingName",	"Is"},--拿起OBJ后,通知客户端使用那个技能
	{"RetHoldThingTimeOver", "I"},--拿起OBJ,超时间
	{"OpenXLProgressWnd","I"},--弹出蓄力的窗口
	{"CloseXLProgressWnd",""},--关闭蓄力的窗口
	--问答系统
	{"RetShowEssayQuestionWnd","ssI"},--显示答题板	参数(答题GAME参数包含  题库名称，取题范围 和 取题数量 任务名称)
	--{"RetInitEssayQuestion","s"},		--任务初始化题目	参数(题库名称，问题的ID,任务名称)
	{"RetAnswerResult","Ibs"},												--回传客户端答题结果 参数( 问题ID,答题结果 ) yy
	{"ShowBoxItemWnd","b"},						--当答题系统用于打开盒子时(参数.  b答题的结果)
	{"InitBoxItemWnd","b"},           
	-- 问答系统结束
	
	---------------------------------------
	{"WeekAwardMsgFromJfs",""},--每周给奖励的消息
	
	--战斗统计面板
	{"OpenFbDpsInfoWnd",""},
	{"CloseFbDpsInfoWnd",""},
	{"UpdateMemberDpsInfo", "sII"},
	{"DeleteMemberDpsInfo", "s"},
	
	--副本中的记分板
	{"RetOpenFbActionWnd","si"},
	{"RetCloseFbActionWnd","s"},
	{"RetOpenCountScoreWnd","sIs"},
	{"RetCloseCountScoreWnd","s"},
	{"RetInsertFbActionToQueue","sbI"},
	{"RetDelQueueFbAction","sb"},
	{"RetInitFbActionQueueWnd","s"},
	{"RetSetJoinBtnEnable","s"},
	{"RetInFbActionPeopleNum","sI"},
	{"RetIsJoinFbActionAffirm","sII"},
	{"RetFbActionOverTime","s"},
	
	{"RetShowRollMsg","ss"},
	
	--大逃杀
	{"RetShowDaTaoShaWnd","I"},
	{"RetSetSceneStateForClient","H"},
	
	{"DtsUpdateAllInfo","IIII"},
	{"DtsUpdateLivePlayerNum","I"},
	--积分赛
	{"RetShowJiFenSaiWnd","I"},--积分赛副本
	{"RetSetJFSPlayerCamp",""},
	{"RetShowJiFenSaiInfoWnd","IIIfIII"},
	--{"RetJFSAddMemberInfo","II"},
	{"RetJFSUpdateMemberInfo","II"},
	--{"RetJFSAllMember","III"},
	{"SetPlayerAlphaValue","I"},--设置玩家透明度
	--跳跳大夺宝
	{"RetShowDaDuoBaoWnd",""},
	{"RetShowDrinkWnd",""},
	--佣兵训练活动
	{"RetShowYbEducateActionWnd",""},
	{"RetIsContinueYbXlAction","Ib"},
	{"RetOverYbXlAction","IsI"},
	{"RetShowYbEducateActInfoWnd","sII"},
	{"RetShowSmallActInfoWnd","I"},
	{"SendResumeNum","bIII"},
	{"RetUpDateSmallInfo",""},
	{"RetCloseYbEducateActInfoWnd",""},
	{"RetStopXiuXingTaBgm",""},
	{"RetUpdateYbEduActInfo","IsfI"},
	{"SendPlayXiuXingTaBgm","I"},
	{"RetBeginYbEduActGame","I"},
	{"RetYbEducateActShowMsg","IIII"},
	{"RetYbEducateActShowLoseGameMsg","IIII" },
	{"RetRecordGameInfo","sI" },
	
	{"ActionAllReady","IIs"},
	{"WaitOtherTeammate","s"},
	{"ChangeActionWarModel",""},
	
	--Boss争夺战
	{"RetShowBossBattleWnd",""},
	{"RetShowBossBattleInfoWnd","sb"},
	{"RetUpdateBossBattleInfoWnd","I"},
	{"BossBattleKillBossMsg","ssss"},
	{"BossBattleJoinFailMsg","s"},
	{"RetEnterBossBattle", "s"},
	{"RetLeaveBossBattle", "b"},
	{"RetWaitBossBattle", ""},
	
	--喝酒射箭
	{"BeginDrinkShoot", ""},
	{"UpdateDrinkShootTurnArrow", "II"},
	
	--倒计时窗口
	{"RetShowDownTimeWnd","I"},--打开倒计时窗口
	{"RetCloseDownTimeWnd",""},--关闭倒计时窗口
	-----------------------------------------
	
	{"SpecailItemCoolDown","sIICC"},
	{"SetTickCompassHeadDir","HIII"},
	{"SetCompassHeadDir","HII"},
	{"ClearCompassHeadDir",""},	
	{"UseItemPlayerEffect","ss"},
	{"UseItemTargetEffect","ssI"},
	{"UseItemOnPosEffect","ssIII"},
--	{"SendNpcStateMsg","sss"},
--	{"SendNpcNotStateMsg","ss"},
	{"SetItemGridWndState","IIb"},			--给物品格子加锁
	{"OnRClickQuestItem","Is"},
	--区域系统
	{"RetFinishAreaQuest","s"},  ---参数（区域名称）
	{"RetFirstTimeIntoArea","sII"},  ---参数（区域名称）
	{"RetPlayerAreaInfo","sII"},
	{"RetPlayerSceneAreaInfo","s"},---参数（场景名称）
	{"SendTeammatePos","IsII"}, -- 向区域面板返回队友位置
	{"SendTongmatePos","IsIIs"}, -- 向区域面板返回队友位置
	{"CheckIsNeedSendTeammatePos",""},--组队时询问客户端是否需要发送队友位置
	{"CheckIsNeedSendTongmatePos","I"},--组队时询问客户端是否需要发送帮友位置
	{"StopSendTeammatePos",""},  -- 离开队伍服务器不再发送队友位置
	{"StopSendTongmatePos",""},  -- 离开队伍服务器不再发送帮友位置
	{"StopSendFbPlayerPos", ""},
	{"HideLeavedTongmatePos","I"},
	{"HideLeftFbPlayerPos", "I"},
	{"HideLeavedTeammatePos","I"},
	{"SendFbPlayerPos", "ISIISIb"},

	--NPC对话系统
	
	--游戏设置
	{"ReturnGetUISetting", "bbbbbbbbbbbbbb"},	--返回UI设置
	{"ReturnGetGameSettingInfo", "iiiiiiI"},	--返回游戏设置的信息
	{"RetQuestSortSettingInfo",	"I"},			--游戏中任务追踪面板的排序信息
	{"ReturnNotifyClientLeave", "C"},			--服务器允许下线
	{"ChangeToStateNow", "C"},					--告诉客户端可以真正注销客户端窗口了		
	{"ChangeSceneErrMsg", "ss"},
	{"RetGMGetSceneGridPos","ss"},
	
	--设置Npc的透明度
	{"SetNpcAlphaValue", "IC"},
	
	--打开是否接收Npc的面板
	{"NotifyWhetherTakeOverNpc", "I"},
	{"SetNpcAlphaValue", "IC"},
	--返回角色升级提示面板要显示的信息
	{"ReturnModifyPlayerSoulNum","I"},--向客户端返回角色身上剩下的魂值
	{"ReturnModifyPlayerSoulNumSlowly","III"},--向客户端返回角色身上剩下的魂值
 {"ReturnPlayerPoint","IIIIIIIIIII"},--上线通知客户端，角色身上剩下的7种积分的值
 {"UpdatePlayerPoint","Ii"},--向客户端发送玩家购买完东西后剩余的积分 参数：支付类型、剩余积分
 {"UpdateComboHitsTimes","I"}, --同步连击数
 
 --小副本相关
 {"InitMatchGameCountWnd", "sCIC"},  -- 玩家名称    当前得分(总得分)  计时 类型 剩余时间 活动状态
 {"MatchGameAddTeam","bIs"},--添加新的成员信息
 {"MatchGameAddTeamMember","bIIs"},
 {"MatchGameRemoveTeam","I"},
 {"MatchGameRemoveTeamMember","II"},
 {"MatchGameShowTeam","I"},
 {"MatchGameShowTeamMember","IIs"},
 {"MatchGameUpdatePlayerScore", "IIIIb"},
 {"MatchGameUpdateTeamScore","IIIb"},
 --{"InsertMatchGameListItem", "I"}, --进入人数
 {"CloseList", ""},   --关闭记分板
 {"RetShowMatchGameSelWnd","Ss"},
 {"BeginGame","I"},      --游戏进行时间
 {"PlayHeadEffect","ssI"},      --头顶显示(同加血加经验)
 {"OpenGameCountdownWnd","II"},
 {"CloseGameCountdownWnd",""},
 {"DelNpcFunLinkTbl","s"},
 {"DelNpcFunTbl",""},
 
 {"SetCountdownTime","I"},
 {"SetMatchGameWndTitleText", "I"},
 {"ShowMatchGameStatisticsWnd", "sSII"},
 {"MatchGameSetWaitTime", "I"},
 
 {"SetCanBeSelectOtherPlayer", "b"},
 {"SetSceneState", "b"},
 
 {"GetTiredNess", "S"},
 {"IsDrunk", "b"},
 
 {"SetMoneyTypeBtnCheck", "I"}, --npc商店切换购买物品的支付方式时，更新包裹里当前选定支付方式chkbtn的状态
 --系统信息
-- {"SystemFriendMsgToClient","I"}, --系统消息，参数：消息Id,消息内容
 {"SysMovementNotifyToClient","S"}, --系统活动通告，参数：消息内容
 {"OpenGacProfiler", "b"},	-- 开关 Lua Profiler
 {"ReturnAimPlayerSoulNum","I"}, -- 返回要查看的玩家身上的魂值 参数：魂值。
 {"SendPlayerBossMsg","Isss"}, --玩家触发刷BOSS公告

	--Pk开关
	{"UpdatePkSwitchState", ""},						--更新PK值状态
--/**旧PK代码**/
	{"InitPkSwitch","b"},
	{"TransferSwitchState","b"},
	{"CreateWarTransprot",""},
	{"CreateMatchGameWnd","s"},
	{"RetGetForageAutoInfo","sII"},
	{"RetShowIssueWnd","I"},
	{"RetOpenFetchWnd",""},
	
	--抢夺资源点
	{"RetShowCancelWnd",""},
	{"CreateRobTransprot",""},
	{"RetOpenSignQueryWnd","sIIs"},
	{"RetNoneOpenSignQueryWnd","s"},
	{"ShowJudgnJoinWnd","s"},
	{"DelItem",""},
	
	
	--多服接口
	{"RetEnterOtherServer","sIIsS"},--参数1:ip;参数2:端口;参数3:跳转场景的类型;参数4:密钥; 参数5:各服务器的lastMsgId
	{"ChangedInSameSceneSuc", ""},
	
	--压测用的rpc消息
	{"ClearActionTimesSucMsg", ""},--活动次数清理完成
	{"ExitMatchGameSuccMsg", ""},--活动次数清理完成
	
	{"ShowAcutionAssignWnd",	"IsIIII"},

	--需求分配拾取方式
	{"ShowNeedAssignWnd",	"IsIIIHI"},						--打开掷骰子面板
	{"CloseNeedAssignWnd",	"I"},							--关闭掷骰子面板
	{"ReturnPlayerAppellationInfo",""}, --返回查询到的称谓内容 参数:称谓内容
	
	{"HideFBDeadMsgWnd",	""},							--关闭复活面板
	{"HideTeamPVPFBDeadMsgWnd",	""},							--关闭复活面板
		--临时包
	{"TempBagSetGrid",	"Is"},
	{"TempBagClearGrid",	"I"},
	{"OpenTempBag",	""},
	{"CloseTempBag",	""},
	
	{"UpdateHeadInfoByEntityID", "I"},						--更新指定对象头顶信息
	{"ReturnCharAllAppellationInfo","I"}, --返回所有角色称谓
	{"ReturnAddNewAppellation","I"}, --返回添加称谓
	{"ReturnDelAppellationBegin",""}, --清空称号列表

	{"RetEquipAdvanceInfo", "dIIIIIIIsIIIIIIsssss"}, -- 装备进阶信息
	{"RetEquipEnactmentInfo","IsIsIsIsI"}, --装备护甲属性
	{"RetActiveJingLingSkill", "I"}, 		--激活精灵技能成功否，激活的精灵技能index
	{"RetActiveJingLingSkillPiece", "dII"}, --激活精灵激活项 物品ID、激活ID、装备部位
	{"AdvancedEquipReborn","d"},			--精灵重生成功 物品ID
	{"SetActionCount","sII"}, --设置活动信息面板上的参加次数
	{"RetShowActionInfoWnd", "II"},
	{"ReturnGetActionCountExBegin",""},
	{"ReturnGetActionCountEx","sII"},
	{"ReturnGetActionCountExEnd",""},
	{"ReturnGetActionAllTimes", "sI"},
	{"AddActionNum", "sI"},
	{"SetDungeonCount","sI"}, --设置活动信息面板上的参加次数
	{"RetShowDungeonInfoWnd", ""},
	{"ShowMercCard","I"},		-- 显示修行塔卡片
	{"RecordMercCard","I"},
	{"InitMercCard",""},
	
	{"LoadSpecialEffect", "ss"},
	{"LoadContinuancelEffect", "ss"},
	{"CancelSpecialEffect", "ss"},
	
	{"UpdateHeadModelByEntityID", "I"},		-- 更新头部模型
	
	{"UpdateShadowModel", "I"},		--变身镜像通知视野内玩家
	{"RetEquipDuraValue", "dIIII"},--参数：装备id，装备部位，装备的耐久状态（0装备无效、1装备变红、2装备正常）
    {"RetEquipDuraValueInBag", "dIIII"}, --参数：装备id,roomindex，pos，装备curDuraValue, maxDuraValue
    {"RetShowEquipDuraWnd", "b"},--参数，控制耐久损伤后窗口的显示与否
    {"RetRenewEquipSuc", "IIsd"}, --修理装备成功返回函数，参数：装备部位，装备type，装备名称，装备id
    {"RenewAllEquipEnd", ""},
    {"RefreshListCtrlInRole", ""},
    {"RefreshListCtrlInBag", ""}, --修理装备成功后刷新显示列表
  -- 中地图相关
  {"SetMatchGameNpcTips", "sS"},
  {"RetShowWarZoneState", "IIs"},
  {"EndSendWarZoneState", ""},
  {"UpdateWarZoneState", "IIs"},
  
  --复活LOG
  {"RebornTimeLog", ""},
  {"BornNpcOnMiddleMap", "ssff"},
  {"DeadNpcOnMiddleMap", "ss"},
  {"AreaPkOnMiddleMap", "ssbII"},
  {"RobResWinOnMiddleMap", "sssI"},
  
  --幻兽
	{"ReturnIncubatePetEgg","bss"},
	{"ReturnPetEggPourSoul","IssIsIIIIss"}, --返回幻兽孵化成功信息
	{"ReturnCharPetInfo","IssIIIIII"},
	{"ReturnCharPetEggInfo","ss"},
	{"ReturnCharPetInfoEnd",""},
	{"ReturnThrowAwayPet","Ib"},
	{"ReturnChangePetName","Is"},
	{"ReturnThrowAwayPetEgg","s"},
	{"ReturnPreparePet","I"},
	{"ReturnUsePetSkillStone","IsI"},
	{"RetInitPetWndInfoBegin",""},
	--道具商城
	{"RetHotSaleItemBegin", ""}, 
	{"RetHotSaleItem", "IIsI"},--返回商城热卖的商品，参数：热卖商品所属类型、物品type、物品名称
	{"RetHotSaleItemEnd", "I"},--参数：热卖商品所属类型
	{"RetTakeYongBingBi", "I"},--返回当前拥有佣兵币数目，参数：当前拥有佣兵币数目
	
	{"ShowNewDirect", "si"},--参数：指引名 状态
	{"ShowNewDirectBegin", ""},
	{"ShowNewDirectEnd", ""},
	
	{"UpdateDirectAwardInfo", "s"},--参数：行为名
	{"RetEquipIntenBack", "IIII"},--装备强化回退，参数：装备在包裹中的roomIndex，装备在包裹中的pos
	{"RetCancelLeaveGame", ""},--切场景时，取消返回状态
	
	{"RetPicInfo", "Ib"},
	
	{"ReturnFightingEvaluation","If"},  --PlayerID,FightingEvaluation
	{"ReturnCharInfo","IsIII"}, 		--PlayerID,PlayerName,PlayerClass,PlayerLevel,PlayerCamp
	{"RetUpdateEquipEffect","I"},
	{"PickOreStart",""},
	{"PickOreEnd",""},
	{"PickOreActionEnd","I"},
	
	{"RetCharCMOrderBegin", ""},   --  参数：订单总页数（50个订单为1页）
	{"RetCharContractManufactureOrder", "IsssdI"},--返回生活技能代工订单信息，参数：订单id，生活技能名称、方向、配方名称、截止时间、手工费
    {"RetCharCMOrderMaterialInfo", "IIIsI"},--返回生活技能代工订单所需材料信息，参数：订单id，第几种材料，材料type，材料name，材料数目
	{"RetCharCMOrderEnd", "I"},
	{"RetAddCMOrderSuc", ""},
	{"RetCancelContractManufactureOrder", ""},
	{"RetCommonCMOrderBegin",""},
	{"RetCommonCMOrderEnd",""},
	{"RetCommonContractManufactureOrder","IssssdI"},--参数：订单ID、玩家Name、技能名称、生活技能方向、配方名称、订单到期时间、手工费
	{"RetCommonCMOrderMaterialInfo","IIIsI"},--参数：订单ID、第几种材料、materialType、materialName、materialNum
    {"RetTakeContractManufactureOrder", ""},
	--服务器向客户端传递即时的热更新的文件和代码
	{"SendMsgToGac", "IsS"},
	{"NotifyGacReply", ""},
	
	--玩家灵感值相关
	{"InitPlayerAfflatusValue", "IIII"},
	{"ReturnExpOrSoulBottleInfo","dII"},
	{"ShowSoulBottleAddSoulMsg","I"},
	{"ReturnBottleState","dI"},
	{"SendResistanceValue","Isffff"},
	{"UpdatePlayerCurOffLineExp", "I"},
	
	{"ReturnUpdateMoneyPayType", ""},
	
	{"RetCreateRoleSucAtRushActivity", ""},--抢注活动中创建角色成功
	{"ReturnFinishInfo", "sI"},
	
	
	{"RetEquipRefine", "b"},    --装备精炼rpc
	{"RetBoxitemOpened", "db"},  --返回盒子物品是否打开过信息
	{"ReturnNpcShopBuyTipInfo", "II"},--返回从商店购买东西时，是否提示“使用流通金币..”信息；参数1：流通商店提示标志（1为提示，0为不提示）；1：通用商店提示标志（1为提示，0为不提示）；
	{"GetBackRoleFail", "I"}, 
	
	{"ShowTransWndInfo", "sIsIIb"},
	
	{"RetLoginConnectStepInfo", "I"}, --登陆连接过程中更新连接步骤信息，参数：msgID
	{"RetLoginCheckUserQueue", "I"}, --登录等待验证帐号队列
	{"ReturnCharAddSoulMsg","I"},
	{"ReturnIdentityAffirm","b"},
	{"ReturnLockItemBagByTime","I"},
	{"RetOpenIdentityAffirmWnd",""},
	{"ReturnInitLockItemBagByTime","I"},
	{"RetItemBagLockTimeOut",""},
	{"RetEquipSuperaddRate", "dII"}, --返回装备追加比例值;参数：装备id，装备追加比例
	{"RetEquipSuperaddSuc", ""}, --装备追加成功
	
	{"ReturnMyPurchasingInfoBegin",""}, --求购行
	{"ReturnMyPurchasingInfo","IsIII"},	--求购行
	{"ReturnMyPurchasingInfoEnd",""},		--求购行
	{"RetAddMyPurchasing","bI"},					--求购行
	{"RetCancelMyPurchasing","bI"},					--求购行
	{"ReturnPurchasingInfoBegin",""}, --求购行
	{"ReturnPurchasingInfo","IsIIIIs"},	--求购行
	{"ReturnPurchasingInfoEnd","I"},		--求购行
	{"ReturnSellGoods","b"},		--求购行
	{"ReturnFastSellItemInfoBegin",""}, --求购行
	{"ReturnFastSellItemInfo","sII"},	--求购行
	{"ReturnFastSellItemInfoEnd",""},		--求购行
	{"ReturnFastSellItemOrder","III"},
	{"ReturnFastSellItemOrderFail",""},
	{"GetAveragePriceByItemName","II"},
	
	{"TestDb2Gac", "Is"},
	{"RetOpenTongNeedFireActivity","bI"},
	{"RetCheckNeedFireIsOpen","bI"},
	{"RetAddNeedFireItem","b"},
	{"RetAddFirewoodMsg","sss"},
	{"RetNeedFireActivityEnd","I"},
	{"RetTongNeedFireActivityMsg",""},
	{"YYReturn","S"},
	{"YYLoginFail",""},
	
	{"RetCapableOfBuyingCouponsInfoBegin", ""},
    {"RetCapableOfBuyingCouponsInfo", "IsIISS"},--参数：sequenceID, name, price, smallIcon, desc, url
	{"RetCapableOfBuyingCouponsInfoEnd", ""},
	{"RetBoughtCouponsInfoBegin", ""},
	{"RetBoughtCouponsInfo", "IIssIISS"},--参数：index, ID, sequenceID, name, price, smallIcon, desc, url
	{"RetBoughtCouponsInfoEnd",""},    
	{"RetCSMAddSellOrderOverAvgPrice", "IsI"},
	{"ShowPanelByTongItem", "sIIsII"},
	
	{"OpenWebBrowser", "s"},
	{"WeiBoLoginFaild", ""},
	
}



Db2GacFunList =
{
["TestDb2Gac"] = true,
["RetInsertFbActionToQueue"] = true,
["RetInFbActionPeopleNum"] = true,
["RetDelQueueFbAction"] = true,
["WaitOtherTeammate"] = true,
["ActionAllReady"] = true,
["SetActionPanelWarnValue"] = true,
}




--定义messge消息函数
local MessageFunTbl,IDToFunTbl = GetMessageFun("Message", "Lan_Message_Common")
g_MsgIDToFunTbl = IDToFunTbl
g_MessageFunTbl = MessageFunTbl
for i , p in pairs (MessageFunTbl) do
	table.insert(funlist,p)
	Db2GacFunList[p[1]] = true
end

local FriendMsgToClientFunTbl, IDToFriendMsgFunTbl = GetMessageFun("SystemFriendMsgToClient", "Lan_SystemFriendMessage_Common")
g_MsgIDIDToFriendFunTbl = IDToFriendMsgFunTbl
g_FriendMsgToClientFunTbl = FriendMsgToClientFunTbl
for i , p in pairs (FriendMsgToClientFunTbl) do
	table.insert(funlist,p)
end


--定义Db2GacRp函数
local temp = {}  

for i, v in pairs(funlist) do
	if Db2GacFunList[v[1]] then
		table.insert(temp, {"_Db2" .. v[1], "sIb" .. v[2]})
	end
end

for i = 0, #temp do
	table.insert(funlist, temp[i])
end


return "Gas2Gac",funlist
