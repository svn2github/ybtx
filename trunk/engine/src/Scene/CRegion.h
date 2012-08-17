#pragma once
#include "CSceneClassDefine.h"
#include "CElementManager.h"
namespace sqr
{
	class CRegion : public CSceneMallocObject
	{
		typedef SList<CElementNode*> CtrlList;
	public:
		CRegion(CBaseScene* pParent,URegionID ID);
		~CRegion(void);
		
		bool		AddCtrlNode( CElementNode* pNode, bool bSync  );
		void		DelCtrlNode( CElementManager* pEleMgr );
		void		Destroy(void);
		URegionID	GetID(void);

		void		ToRecycle(void);
		void		SetValid(void);
	protected:
		CBaseScene*			m_pParentMgr;
		CtrlList			m_NodeList;
		URegionID			m_ID;
		bool				m_isValid;
	};

	inline URegionID CRegion::GetID()
	{
		return m_ID;
	}
}