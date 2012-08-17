#pragma once
#include "TSingleton.h"

class CCharacterDictator;

class CTestBulletMagic : public TSingleton<CTestBulletMagic>
{
	friend class TSingleton<CTestBulletMagic>;
public:
	static bool StartTest(uint32 uGlobalId);
	static bool ShutDownTest();
private:
	CTestBulletMagic();
	~CTestBulletMagic(){}

	bool IsTesting()const { return m_bIsTesting; }
	bool m_bIsTesting;
	vector<CCharacterDictator*> m_vecAttacker;
	vector<uint32>				m_vecAttackerByID;
};
