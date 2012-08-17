#pragma once

// 服务端读配置表用下面的宏定义
// Faction--派别, 小集团。隶属于三大阵营或独立，根据NPC种族，地域等详细的划分
// 区别于Camp，Camp为出生的三大阵营：艾米帝国联盟、西帝君联盟、恶魔岛势力
#define szFaction_Name					"派别名称"
#define	szFaction_Camp					"派别所属阵营"
#define szFaction_AmyEmpire				"艾米帝国联盟"
#define szFaction_WestEmpire			"西帝君联盟"
#define szFaction_DevilIsland			"恶魔岛势力"

//======================	普通攻击	============================

#define szNT_Name						"名称"
#define szNT_MaxAttackDis				"最大攻击距离"
#define szNT_MagicEff					"魔法效果"
#define szNT_AttackType					"攻击类型"
#define szNT_KeyFrameFileMan			"关键帧文件男"
#define szNT_KeyFrameFileWoman			"关键帧文件女"

//======================	天赋效果	============================

#define szSkill_TalentName				"天赋名称"
#define szSkill_TypeArg					"类型参数"
#define szSkill_TalentType		"天赋类型"
#define szSkill_UpdateWithLevel		"跟随等级更新"

//======================	职业技能表	============================

#define szClass_SkillName				"职业技能名称"

//======================	技能总表	============================

#define szSkill_Name					"技能名称"
#define szSkill_HorseLimit				"技能坐骑限制"
#define szSkill_IsStanceSkill			"是否为姿态技能"
#define szSkill_TargetIsCorpse			"是否对尸体施放"
#define szSkill_IsUpdateDirection		"是否更新人物朝向"
#define szSkill_StartNormalAttack		"对普攻的影响"
#define szSkill_SelectTargetType		"选中目标类型"
#define szSkill_AttackType				"攻击类型"
#define szSkill_MinCastSkillDistance	"最小施法距离"
#define szSkill_MaxCastSkillDistance	"最大施法距离"
#define szSkill_Type					"技能类型"
#define szSkill_CoolDownTime			"冷却时间"
#define szSkill_ActiveTimeAndCount	"技能拥有时间及次数"
#define szSkill_ReachUpAction			"起手动作"
#define szSkill_ReachUpEffect			"起手特效"
#define szSkill_CastAction				"出手动作"
#define szSkill_CastEffect				"出手特效"
#define szSkill_MagicEff				"魔法效果"
#define szSkill_LevelMap				"等级映射"
#define szSkill_DoSkillRule				"是否走技能规则"
#define szSkill_BarrierType				"忽略障碍类型"

//======================	施法流程	============================

#define szCasting_Name					"名称"
#define szCasting_Type					"施法类型"
#define szCasting_InterruptType			"打断类型"
#define szCastingTime					"施法时间"
#define szInterPercent					"中断几率"
#define szLinkEffect					"是否为连接特效"
#define szProcessAction					"施法动作"
#define szProcessEffect					"施法特效"
#define szMagicEff						"魔法效果"
#define szSelfCancelableMagicEff			"自身可撤销魔法效果"
#define szObjCancelableMagicEff			"目标可撤销魔法效果"
#define szFinalMagicEff					"不可撤销最终魔法效果"

//======================	魔法效果	============================

#define szMagicEff_Name					"名称"
#define szMagicEff_MOPType				"操作类型"
#define szMagicEff_MOPExecCond			"操作执行条件"
#define szMagicEff_FilterPipe			"筛选管道"
#define szMagicEff_FXName				"特效名"
#define szMagicEff_FxType				"特效类型"
#define szMagicEff_MagicOp				"魔法操作"
#define szMagicEff_MOPParam				"魔法操作参数"

#define szGlobalParam_Name1				"名称1"
#define szGlobalParam_Name2				"名称2"
#define szGlobalParam_CfgCalc			"表达式"
#define szGlobalParam_IsArrayConst		"是否数组常量"

//======================	通用魔法状态	============================

#define szGlobalState_Name				"名称"
#define szTplState_IconID				"对应小图标编号"
#define szTplState_Model				"模型"
#define szTplState_Scale				"缩放比例"
#define szTplState_FXID					"对应特效编号"
#define szTplState_Cancelable			"是否可撤销"
#define szTplState_DecreateType			"负面效果类型"
#define szTplState_Dispellable			"是否可驱散或净化"
#define szTplState_Cancelable			"是否可撤销"
#define szBaseState_CancelCond			"取消条件"

#define szState_ForceSaveOrForceNoSave	"强制存还是强制不存"
#define szState_Persistent				"死亡不清除及无法被清除"

//======================	魔法状态	============================

#define szMagicState_Name				"名称"
#define szMagicState_Decreate			"是否为负面魔法状态"
#define szMagicState_Cancelable			"是否可撤销"
#define szMagicState_ReplaceRuler		"是否可替换"
#define szMagicState_CascadeType		"群体叠加类型"
#define szMagicState_CascadeGradation	"替换和叠加是否限制等级"
#define szMagicState_CascadeMax			"叠加上限"
#define szMagicState_Time				"作用时间"
#define szMagicState_CancelableMagicEff	"可撤销魔法效果"
#define szMagicState_DotMagicEff		"不可撤销间隔魔法效果"
#define szMagicState_FinalMagicEff		"不可撤销最终魔法效果"
//#define szMagicState_FMAssociateDotM	"最终魔法效果与间隔魔法效果的比率"

//======================	触发器魔法状态	============================

#define szTriggerState_Name					"名称"
#define szTriggerState_Decreate				"是否为负面"
#define szTriggerState_Cancelable			"是否可撤销"
#define szTriggerState_CascadeType			"群体叠加类型"
#define szTriggerState_Time					"作用时间"
#define szTriggerState_InitialCount			"作用次数初始值"
#define szTriggerState_ChangeTrigger		"更换目标为"
#define szTriggerState_Probability			"作用几率"
#define szTriggerState_Passivity			"触发事件是主动还是被动"
#define szTriggerState_TriggerEvent			"触发事件"
#define szTriggerState_SkillType			"触发事件技能类型"
#define szTriggerState_AttackType			"触发事件攻击类型"
#define szTriggerState_TriggerEff			"不可撤销触发器魔法效果"
#define szTriggerState_CancelableMagicEff	"可撤销魔法效果"
#define szTriggerState_FinalMagicEff		"不可撤销最终魔法效果"

//======================	伤害变更魔法状态	============================

#define szDamageChangeState_Name			"名称"
#define szDamageChangeState_Decreate		"是否为负面"
#define szDamageChangeState_Cancelable		"是否可撤销"
#define szDamageChangeState_Time			"作用时间"
#define szDamageChangeState_InitialValue	"伤害数值初始值"
#define szDamageChangeState_ChangeTrigger	"更换目标为"
#define szDamageChangeState_Probability		"作用几率"
#define szDamageChangeState_Passivity		"触发事件是主动还是被动"
#define szDamageChangeState_TriggerEvent	"触发事件"
#define szDamageChangeState_SkillType		"触发事件技能类型"
#define szDamageChangeState_AttackType		"触发事件攻击类型"
#define szDamageChangeState_ApplyTempValue	"是否应用改变的值"
#define szDamageChangeState_ChangeToValue	"改变成的值"
#define szDamageChangeState_FinalMagicEff	"不可撤销最终魔法效果"



//======================	积累触发状态	============================

#define szCumuliTriggerState_UnitValue				"触发单位数值"
#define szCumuliTriggerState_MaxNumInSingleEvent	"单次触发最大次数"

//======================	特殊魔法状态	============================

#define szSpecialState_Name				"名称"
#define szSpecialState_Decreate			"是否为负面"
#define szSpecialState_Cancelable		"是否可撤销"
#define szSpecialState_Time				"作用时间"
#define szSpecialState_Type				"类型"

//=====================		姿态魔法     ==========================

#define szAureMagic_SaveToDB			"是否存数据库"

//======================	魔法	============================

#define szMagic_Name					"名称"
#define szMagic_Speed					"速度"
#define szMagic_Gravity					"重力加速度"
#define szMagic_Ranges					"射程"
#define szMagic_Angle					"夹角"
#define szMagic_Amount					"数量"
#define szMagic_Radius					"范围"
#define szMagic_Time					"持续时间"
#define szMagic_AmountLimit				"作用次数"
#define szMagic_MutexType				"互斥类型"
#define szMagic_MoveArg					"移动参数"
#define szMagic_MoveType				"移动方式"
#define szMagic_TrackType				"轨迹"
#define szMagic_FireSkeleta				"出手骨骼"
#define	szMagic_StanceType				"姿态类型"
#define szMagic_TransType				"传递方式"
#define	szMagic_AgileType				"易变值类型"
#define	szMagic_OperateObject			"作用对象"
#define szMagic_BattleArrayValue		"聚气值"
#define szMagic_BarrierMagicType		"魔法类型"
#define szMagic_BarrierType				"障碍类型"
#define szMagic_Coordinate				"坐标"
#define szMagic_ModelName				"模型"
#define szMagic_ActionName				"动作名"
#define szMagic_FxName					"特效名"
#define szMagic_ActionType				"动作类型"
#define szMagic_StartFxName				"起始特效"
#define szMagic_StartActionName			"起始动作"
#define szMagic_ProcessFxName			"过程特效"
#define szMagic_ProcessActionName		"过程动作"
#define szMagic_EndFxName				"结束特效"
#define szMagic_EndActionName			"结束动作"
#define szMagic_FinalFxName				"最终特效名"
#define szMagic_BattleArrayPosFx		"阵位特效"
#define szMagic_BattleArrayPosSucFx		"阵中特效"
#define szMagic_BattleArrayAddUpSucFx	"聚气成功特效"
#define szMagic_BAPosSkill_1			"位置一技能"
#define szMagic_BAPosSkill_2			"位置二技能"
#define szMagic_BAPosSkill_3			"位置三技能"
#define szMagic_BAPosSkill_4			"位置四技能"
#define szMagic_BAPosSkill_5			"位置五技能"
#define szMagic_MagicEff				"魔法效果"
#define szMagic_MainMagicEff			"主魔法效果"
#define szMagic_SecondMagicEff			"副魔法效果"
#define szMagic_TouchMagicEff			"触发魔法效果"
#define szMagic_DotMagicEff				"不可撤销间隔魔法效果"
#define szMagic_FinalMagicEff			"不可撤销最终魔法效果"
#define szMagic_CancelableMagicEff		"可撤销魔法效果"
#define szMagic_DisappearWhenEnough     "够次数后消失"

//=====================================================
#define szColumnRestraint_TableName		"表名"
#define szColumnRestraint_ColumnName	"列名"
#define szColumnRestraint_Type			"类型"
#define szColumnRestraint_Field			"取值范围"
#define szColumnRestraint_CanEmpty		"是否可为空"
#define szColumnRestraint_Default		"默认值"
#define szColumnRestraint_IsPrimaryKey	"是否为主键"

#define szRelationRestraint_TableName		"表名"
#define szRelationRestraint_ExceptionCond	"异常条件"

//======================	宠物============================
#define szPet_Name							"名称"
#define szPet_ModelName						"模型"
#define szPet_FxName						"特效名"
#define szPet_PetType						"类型"
#define szPet_Time							"持续时间"
#define szPet_Skill							"可使用技能"

//===========================  动作  ===========================
#define szActor_Name						"ActionName"
#define szActor_PlayMode					"PlayMode"
#define szActor_PriorityLevel				"PlayPriority"
#define szActor_PlayerAniName				"PlayerAniName"
#define szActor_NPCAniName					"NPCAniName"
#define szActor_NPCReplace					"NPCReplace"
#define szActor_FxFollowAni					"FxFollowAni"
#define szActor_FxPartPath					"FxPartPath"

//=======================   表情 ===========================
#define szFacial_Name				"表情动作名称"
#define szFacial_AniName			"表情动作动画"
#define szFacial_AniPath			"表情动作路径"

//======================	技能播放动画未完成前的临时常量	============================
#define uTSP_SingActId				0
#define uTSP_SpellActId				1

//======================	表情分离表	============================

#define szFacialAni_Name					"FaceActionName"
#define szFacialAniStart_Name				"StartActionName"
#define szFacialAniContinue_Name			"ContinuanceActionName"

//======================	音效		============================
#define szSound_PlayerTypeName		"PlayerTypeName"
#define szSound_Suffer				"Suffer"
#define szSound_Die					"Die"
#define szSound_Footstep			"Footstep"

#define szSound_AniFileName			"AniFileName"
#define szSound_EnterBattle			"EnterBattle"
#define szSound_WeaponType			"WeaponType"
#define szSound_ArmorType			"ArmorType"

//======================	特效		============================
#define szEffect_Name		"特效名"
#define szEffect_LinkClass				"连接部位"

//======================	爆魂		============================
#define szBurstSoul_DoubleBurst				"连击数"
#define szBurstSoul_Count					"魂数量"
#define szBurstSoul_MagicEff				"魔法效果"
#define szBurstSoul_BulletMagicName			"爆魂子弹"
#define szBurstSoul_Probability				"概率"
#define szBurstSoul_TimesLevel				"蓄力值等级"
#define szBurstSoul_Type					"魂类型"

//======================	 技能替换表    ============================
#define szSkillReplace_SkillName			"技能名称"
#define szSkillReplace_StateName			"条件状态"
#define szSkillReplace_ReplaceName			"替换技能"
