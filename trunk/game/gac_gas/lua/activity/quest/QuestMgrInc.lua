
g_QuestDropItemMgr = {} --g_QuestDropItemMgr[任务名][开启掉落的物品名].Object(或者.Rate)
		                         --Object的值为掉落的物品名,Rate的值为掉落物品的概率)

g_HideQuestMgr = {} -- 返回任务类型为0(隐藏任务)的任务队列 任务名为索引

g_RepeatQuestMgr = {} -- 重复任务的任务队列 任务名为索引

g_QuestNeedMgr = {}--格式为g_QuestNeedMgr[任务名][任务需要的物品名].Num(值为需要的数目)

--g_QuestAwardMgr = {}  --格式为g_QuestAwardMgr[任务名].Must(或者Select,值为{{Itemtype,ItemName,Num},{...},...}
										--Must为必选奖励物品，Select为可选奖励物品)
g_WhereGiveQuestMgr = {} --格式为g_WhereGiveQuestMgr[对象名][任务名]								

g_WhereFinishQuestMgr = {} --格式为g_WhereFinishQuestMgr[对象名][任务名]

g_QuestPropMgr = {}

g_AreaQuestMgr = {}

g_MapQuestMgr = {}

g_ItemQuestMgr = {}

g_VarQuestMgr = {}

g_KillNpcQuestMgr = {}

g_QuestToNpcMgr = {}		--每个有杀怪需求的任务,所对应的要杀的NPC名字

g_LearnSkillMgr = {}

g_BuffQuestMgr = {}

g_MercenaryQuestMgr = {}--佣兵任务的分类管理(CampType,MapName,MerceQuestType)

g_AllMercenaryQuestMgr = {}--佣兵的所有任务
g_GeneralMercenaryQuestMgr = {}--日常佣兵任务管理 1类
g_DareMercenaryQuestMgr = {}--挑战佣兵任务管理	2,3,4类
g_FewMercenaryQuestMgr = {}--稀有佣兵任务管理		5类
g_TeamMercenaryQuestMgr = {}--小队伍佣兵任务管理		6,7类
g_AreaMercenaryQuestMgr = {}--区域佣兵任务管理		8类

g_DareQuestMgr = {}--挑战任务

g_ActionQuestMgr = {}--玩法任务

g_MasterStrokeQuestMgr = {}--主线任务

g_QuestShowNpcNameMgr = {} --显示任务相关的Npc名

g_LoopQuestMgr = {}	--跑环任务

------------------------
--现在任务分为4大类
--1 主线任务
--2 佣兵任务
--3 挑战任务
--4 玩法任务
-------------------------






--为排列任务中需求的先后顺序而加的任务顺序Tbl
g_SortQuestVarMgr = {}

g_GMTimeCountLimit = 1	--GM命令控制，1代表任务的时间限次功能有效


AddCheckLeakFilterObj(g_QuestDropItemMgr)
AddCheckLeakFilterObj(g_HideQuestMgr)
AddCheckLeakFilterObj(g_RepeatQuestMgr)
AddCheckLeakFilterObj(g_QuestNeedMgr)
AddCheckLeakFilterObj(g_WhereGiveQuestMgr)
AddCheckLeakFilterObj(g_WhereFinishQuestMgr)
AddCheckLeakFilterObj(g_QuestPropMgr)
AddCheckLeakFilterObj(g_AreaQuestMgr)
AddCheckLeakFilterObj(g_MapQuestMgr)
AddCheckLeakFilterObj(g_ItemQuestMgr)
AddCheckLeakFilterObj(g_VarQuestMgr)
AddCheckLeakFilterObj(g_KillNpcQuestMgr)
AddCheckLeakFilterObj(g_QuestToNpcMgr)
AddCheckLeakFilterObj(g_LearnSkillMgr)
AddCheckLeakFilterObj(g_BuffQuestMgr)
AddCheckLeakFilterObj(g_MercenaryQuestMgr)
AddCheckLeakFilterObj(g_AllMercenaryQuestMgr)
AddCheckLeakFilterObj(g_GeneralMercenaryQuestMgr)
AddCheckLeakFilterObj(g_DareMercenaryQuestMgr)
AddCheckLeakFilterObj(g_FewMercenaryQuestMgr)
AddCheckLeakFilterObj(g_TeamMercenaryQuestMgr)
AddCheckLeakFilterObj(g_AreaMercenaryQuestMgr)
AddCheckLeakFilterObj(g_DareQuestMgr)
AddCheckLeakFilterObj(g_ActionQuestMgr)
AddCheckLeakFilterObj(g_MasterStrokeQuestMgr)
AddCheckLeakFilterObj(g_SortQuestVarMgr)
AddCheckLeakFilterObj(g_QuestShowNpcNameMgr)