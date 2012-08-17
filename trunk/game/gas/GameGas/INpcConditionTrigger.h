/*
 * =====================================================================================
 *
 *       Filename:  INpcConditionTrigger.h
 *
 *    Description:  Npc条件触发器
 *
 *        Version:  1.0
 *        Created:  2008年11月28日 10时53分05秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  廖鑫炜 (Comet), liaoxinwei@linekong.com
 *        Company:  ThreeOGCMan
 *
 * =====================================================================================
 */

#pragma once
class CNpcAI;
class INpcConditionTrigger
{
	public:
		virtual ~INpcConditionTrigger() { }
		virtual void Active() = 0;
		virtual void Freeze() = 0;
		virtual void Trigger() = 0;
		virtual bool Check() = 0;
		virtual INpcConditionTrigger* Clone() const = 0;
};
