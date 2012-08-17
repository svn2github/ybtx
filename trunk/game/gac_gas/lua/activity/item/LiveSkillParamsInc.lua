
--花卉技能常量
CultivateFlowerParams = {

	--触发事件时间 (最小时间~最大时间)之间随机
	["TriggerTimeMin"] = 15,
	["TriggerTimeMax"] = 15,
	--事件持续时间
	["EventTime"] = 10,
	--技能冷却时间
	["SkillCooldownTime"] = 2,
	--收获冷却时间
	["GetItemCooldownTime"] = 3,
	--花卉初始健康度
	["HealthPoint"] = 100,
	--健康值每增加X点,多一次可收获次数
	["HealthPointStep"] = 30,
}

--技能加健康值
--1.浇水 2.除虫 3.晒太阳
FlowerSkillAddHealthPoint = {
	[1] = 0,
	[2] = 50,
	[3] = 50,
}

--花卉有异常状态时减去的健康值
--2.虫害时 3.萎靡时
FlowerSkillMinusHealthPoint = {
	[1] = 30,
	[2] = -20,
	[3] = -20,
}

--花卉状态特效
--1.正常 2.虫害 3.萎靡
CultivateFlowerStateEfx = {
	[1] = {"",""},
	[2] = {"fx/setting/obj/shjn_chonghai.efx","chonghai/create"},
	[3] = {"fx/setting/obj/shjn_weimi.efx","weimi/create"},
}

--清除状态的特效
--0.生长特效 1.浇水 2.除虫 3.晒太阳
CultivateFlowerCleanStateEfx = {
	[0] = {"fx/setting/obj/shjn_shengzhang.efx","shengzhang/create"},
	[1] = {"fx/setting/obj/shjn_jiaoshui.efx","jiaoshui/create"},
	[2] = {"fx/setting/obj/shjn_chuchong.efx","chuchong/create"},
	[3] = {"fx/setting/obj/shjn_shaitaiyang.efx","shaitaiyang/create"},
}
