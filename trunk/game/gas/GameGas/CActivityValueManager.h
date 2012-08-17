#pragma once

class CActivityValueManager 
{
public:
	CActivityValueManager();
	~CActivityValueManager();
	void ChangeTargetHP(int32 uHP);
	uint32 GetActivityValue();
	void ClearActivityValue();
private:

	uint32 m_uHPCount;
};

