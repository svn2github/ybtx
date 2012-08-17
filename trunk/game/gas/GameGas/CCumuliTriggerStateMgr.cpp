#include "stdafx.h"
#include "CCumuliTriggerStateMgr.h"
#include "CSkillInstServer.h"
#include "CTriggerableStateMgr.inl"
#include "COtherStateMgr.inl"
#include "CStateDBData.h"

template class TOtherStateMgrServer<CCumuliTriggerStateCfgServer, CCumuliTriggerStateServer>;
template class TTriggerableStateMgrServer<CCumuliTriggerStateCfgServer, CCumuliTriggerStateServer>;
