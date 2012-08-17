#pragma once
#include "TSingleton.h"
#include "CDynamicObject.h"

class CCharacterDictator;

class CTestShockWaveMagic : public TSingleton<CTestShockWaveMagic>//, public virtual CDynamicObject
{
	friend class TSingleton<CTestShockWaveMagic>;
public:
	static bool StartTest();
	static bool ShutDownTest();
private:
	CTestShockWaveMagic();
	~CTestShockWaveMagic(){}

	bool IsTesting()const { return m_bIsTesting; }
	bool m_bIsTesting;
	vector<CCharacterDictator*> m_vecAttacker;
};
