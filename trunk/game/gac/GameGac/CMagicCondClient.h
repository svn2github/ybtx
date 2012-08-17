#pragma once

class CFighterDirector;
class CCfgCalc;

// Ä§·¨Ìõ¼þ
class CMagicCondClient
{
public:
	virtual uint32 Test(uint32 SkillLevel, const CCfgCalc& Arg,const CFighterDirector* pFighter) const = 0;
};
