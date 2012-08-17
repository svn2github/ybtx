#pragma once
#include "HeadBlood.h"
#include "GameDef.h"
#include "CBloodRenderMallocObject.h"
#include "TBloodRenderAllocator.h"
#include "CStaticObject.h"

class CBloodRenderDialogPool
	: public virtual CStaticObject
	, public CBloodRenderMallocObject
{
public:
	CBloodRenderDialogPool();
	~CBloodRenderDialogPool();
	typedef vector<HeadBlood*, TBloodRenderAllocator<HeadBlood*> > BloodVec;
	typedef map<EBloodDialogType, BloodVec, less<EBloodDialogType>, TBloodRenderAllocator<pair<EBloodDialogType, BloodVec> > > MapHeadBloodDialog;

	void CreatePool(SQRWnd* ParentWnd);
	HeadBlood* GetHeadBloodDialog(SQRWnd* ParentWnd, EClass eClass);
	void ReleaseHeadBloodDialog(HeadBlood* pWnd, EClass eClass);
	void Release();
	
private:
	//vector<HeadBlood*>		m_HeadBloodDialogVec;
	MapHeadBloodDialog			m_mapHeadBloodDialog;
	uint32 m_PoolSize;
};
