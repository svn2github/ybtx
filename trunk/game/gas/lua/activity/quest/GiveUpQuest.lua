--此脚本是为做出某些特殊任务放弃任务时配置表无法配置的行为而创建的
QuestGiveUpTbl = {}
QuestGiveUpScript = class()

function QuestGiveUpScript:Ctor()
	self.m_GiveUpDlg = CDelegate:new()
end

function QuestGiveUpScript:OnGiveUpQuest(QuestName,player)
	self.m_GiveUpDlg(QuestName,player)
end

function QuestGiveUpScript:RegistQuestGiveUpOnScript(QuestGiveUpFunc)
	self.m_GiveUpDlg:Add(QuestGiveUpFunc)
end

g_QuestGiveUpScript = QuestGiveUpScript:new()