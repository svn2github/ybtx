#pragma once
#include "CCoreObjectClient.h"

namespace sqr
{
	class ICoreObjectPerformerHandler;
	class CRenderObject;

	class CCoreObjectPerformer
		:public CCoreObjectClient
	{
	public:
		ICoreObjectPerformerHandler* GetHandler()const;

		CRenderObject* GetRenderObject()const;

		EClientObjectType GetType()const;

		ESetSpeedResult SetSpeed(float fSpeed);
		EStopMovingResult StopMoving();
		EMoveToResult MoveTo(const CFPos& PixelPosDes,float fSpeed,EFindPathType eFindPathType,EBarrierType eBarrierType,float fReachDist);
		ESetPosResult SetPixelPos(const CFPos& PixelPos);
		ESetPosResult SetGridPos(const CPos& GridPos);

		void SetHeight(float fHeightInPixel);
		float GetHeight()const;

		void SetCanBeSelected( bool bCan );
		bool GetCanBeSelected();

		void SetLieOnTheTerrain( bool bLieOn );
		bool IsMoving()const;

	private:
		friend class CCoreSceneClient;

		CCoreObjectPerformer(ICoreObjectPerformerHandler* pHandler,const CFPos& PixelPos);
		~CCoreObjectPerformer(void);

		void RefreshRenderObjectPos();
		void IntSetPositionDerived(const CFPos& PixelPos);
		void OnEndMoveDerived();

		CRenderObject*	m_pRenderObject;
		float			m_fHeightInPixel;
		bool			m_bCanBeSelected;
		bool			m_bLieOnTheTerrain;
	};
}