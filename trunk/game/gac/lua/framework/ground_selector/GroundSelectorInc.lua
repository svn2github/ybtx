CGroundSelector				= class()
GroundSelector				= CGroundSelector:new()

--	选择状态	
--	0 非选择状态
--	1 进入选择状态, 鼠标特效已创建
--	2 退出选择状态，特效已经取消，检查技能施放	--> 返回 状态0				

GroundSelector.m_Actived	= false
GroundSelector.m_Attack		= false
GroundSelector.m_SkillIdIncLevel	= 0
GroundSelector.m_State		= 0
GroundSelector.m_MouseFx	= nil
GroundSelector.m_Ticker		= nil
GroundSelector.m_Size		= 3
GroundSelector.m_Range = 0
GroundSelector.m_vecPos		= CVector3f:new()
