#pragma once

#include "IBarrierInfo.h"

class CBarrierData :
	public IBarrierInfo
{
public:
	CBarrierData(void);
	~CBarrierData(void);

	bool LoadBarrierData(const string& strPathName);
	void LoadOneRegion(FILE * fp, uint32 nChunk, uint32 x, uint32 y);
	void LoadOneChunk(FILE * fp, uint32 x, uint32 y, uint32 uMagicID);

	virtual	inline uint32 GetExactWidth()
	{
		return m_nWidth << 1;
	}
	virtual inline uint32 GetExactHeight()
	{
		return m_nHeight << 1;
	}
	virtual inline int GetBarrierType(int x, int y)
	{
		return  m_pBarrierArray[ (y >> 1) * m_nWidth + (x >> 1) ];
	}

private:
	uint8* m_pBarrierArray;
	uint32 m_nWidth;
	uint32 m_nHeight;
};
