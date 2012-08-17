#pragma once
#include "CTick.h"
#include "CActorMallocObject.h"

class CCharacterFollower;

class CFacingMgr 
	: public CTick
	, public CActorMallocObject
{
public:
	CFacingMgr();
	~CFacingMgr();

	void CreateFacingPair(CCharacterFollower* pCharSrc, CCharacterFollower* pCharDst);	
	void TouchOffFacingPair(CCharacterFollower* pCharDst);
	void RemoveFacingPair();

	void SetDstEntitiyID(uint32 uDstEntityID)	{m_uDstEntityID = uDstEntityID;}
	void SetNADistance(float fNADistance)		{m_fNADistance = fNADistance;}

private:
	void SetInterestObj(CCharacterFollower* pCharSrc, CCharacterFollower* pCharDst);
	void ClearInterested();

	bool Do();
	void OnTick();

	typedef multimap<uint32, uint32>		MultiMapDstBeLookedOnSrc;		
	static MultiMapDstBeLookedOnSrc			m_multiMapDstBeLookedOnSrc;		// pSrcFol 关注的 pDstFol 未进入视野时用

	uint32 m_uSrcEntityID;
	uint32 m_uDstEntityID;	

	float m_fNADistance;
};
