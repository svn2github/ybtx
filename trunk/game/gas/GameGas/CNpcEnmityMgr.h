#pragma once

#include "CEnmityMgr.h"

class CCharacterDictator;
class CNpcAI;

/*
* =====================================================================================
*        Class:  CNpcEnmityMgr
*  Description:  Npc³ðºÞ¹ÜÀíÆ÷
* =====================================================================================
*/
class CNpcEnmityMgr 
	: public CEnmityMgr
{
public:

	CNpcEnmityMgr(CNpcAI* pOwner);                /* constructor */
	~CNpcEnmityMgr();                             /* constructor */
	virtual bool JudgeIsEnemy(CCharacterDictator* pTarget);
	virtual void ClearAllEnmity();
}; 
