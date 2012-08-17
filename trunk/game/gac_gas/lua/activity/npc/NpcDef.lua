
--剧情NPC类型
ENpcTypeTask =
{
	eNT_None 				= 0,	--不是剧情Npc
	eNT_Task				= 1,	--任务
	eNT_Play				= 2,    --玩法
	eNT_Work				= 3,    --职业
}

--功能NPC分类
ENpcTypeFun =
{
	eNT_None 				= 0,	--不是功能Npc
	eNT_Shop				= 1,	--商店
	eNT_Union				= 2,	--工会
	eNT_Buy					= 3,    --交易所
	eNT_Hostel				= 4,    --旅店
	eNT_Carry				= 5,    --传送
}

--战斗NPC分类
ENpcTypeFgt =
{
	eNT_None 				= 0,	--不是战斗Npc
	eNT_Com					= 1,	--普通
	eNT_Soul				= 2,	--精英
	eNT_Boss				= 3,    --BOSS
	eNT_Guarder				= 4,    --守卫
	eNT_NbGuarder			= 5,   	--无敌的守卫
}

--交互对象分类
CIntObjType=
{
	CIOT_CursorShowTipsObj=0,  --鼠标移动到对象后显示文本框
	CIOT_ClickShowTipsObj=1, --鼠标点击对象后显示文本框
	CIOT_TakeQuestObj=2,  --鼠标点击对象后打开任务接取面板
	CIOT_FinishQuestObj=3, --鼠标点击对象后打开任务完成面板
	CIOT_LivesSkillObj=4,--鼠标点击对象后打生活技能（织布机等）面板
	CIOT_CollObj=5,--鼠标点击对象后打开采集物品面板
	CIOT_CanSelectObj=6,--鼠标点击对象后选中该对象（具体功能待定）
	CIOT_MailBoxObj=7--鼠标点击对象后打开邮件系统主界面
}
