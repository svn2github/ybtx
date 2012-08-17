#pragma once

bool CheckSkillCfg();
void EndCheckSkillCfg();

bool IsIntObjSkillNameValid(const TCHAR* szName);
bool IsPlayerSkillNameValid(const TCHAR* szName);
bool IsNPCSkillNameValid(const TCHAR* szName);
void InsertTalentName(const TCHAR* sName);
bool IsStateNameValid(const TCHAR* sName);
