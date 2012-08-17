CAssociationFindWnd					= class(SQRDialog)

CAssociationFindPlayerWnd			= class(SQRDialog)
CAssociationRetFindPlayerWnd		= class(SQRDialog)

CAssociationFindGroupWnd			= class(SQRDialog)
CAssociationRetFindGroupWnd			= class(SQRDialog)

CAssociationRetFindListItem			= class(SQRDialog)

--[[
// 职业分类
enum EClass
{
	eCL_NoneClass = 0,	//无职业
	eCL_Warrior,		//大剑士
	eCL_MagicWarrior,	//魔剑士
	eCL_Paladin,		//骑士
	eCL_NatureElf,		//法师
	eCL_EvilElf,		//邪魔
	eCL_Priest,			//牧师
	eCL_DwarfPaladin,	//矮人骑士
	eCL_ElfHunter,		//精灵弓箭手
	eCL_OrcWarrior,		//兽人战士
	eCL_Npc,			//NPC
	eCL_Wizard,			//巫师
	eCL_Assassin,		//刺客
	eCL_Warlock,		//邪恶法师	
	eCL_WarPriest,		//战牧  
	eCL_Archer,			//弓箭手
	eCL_Soldier,		//大战士
	eCL_Summoner,		//召唤师
	eCL_Beast,			//野兽
	eCL_Summon,			//召唤生物
	eCL_Lancer,			//攻击骑士
	eCL_Knight,			//防御骑士
	eCL_NatureElement,	//自然元素
	eCL_EvilElement,	//暗黑元素
	eCL_DestructionElement,	//破坏元素
	eCL_Special,		//特殊
	eCL_AllClass,		//所有职业
	eCL_Count
};
--]]