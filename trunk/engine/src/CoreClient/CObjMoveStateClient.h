#pragma once
#include "TObjMoveState.h"
#include "CTraitsClient.h"

namespace sqr
{

	class CFolObjFollowState;
	class CCoreObjectDirector;
	class CCoreObjectClient;

	class CObjMoveStateClient
		:public TObjMoveState<CTraitsClient>
	{
		typedef TObjMoveState<CTraitsClient>	Parent_t;
		friend class CCoreObjectDirector;
		friend class CCoreObjectFollower;
		friend class CCoreObjectClient;
		friend class CFolObjFollowState;
	public:
		CObjMoveStateClient(CCoreObjectClient* pObj,CPixelPath* pPath,float fSpeed,float fBeginDist);
		~CObjMoveStateClient(void);

#ifdef USE_RENDER_SLOT
		virtual void OnUpdate(uint64 uLastFrameUsedTime);
#endif

		bool NeedSkipFrameDerived()const;
		uint32 GetMoveCycDerived()const;
		void SetMoveCycDerived(uint32 uMoveCyc);

		void CreateTickingDerived(uint32 uCyc);
		void DeleteTickingDerived();

		uint32 GetInterval()const;

		void IntSetSpeedDerived(float fSpeed);
		float IntGetNewMoveDistDerived()const;
		void SetNewPathDerived(CPixelPath* pPath,float fBeginDist);

	private:
		uint32			m_uMoveCyc;
		bool			m_bMovTickIsStopped;

		uint64			m_uMoveBeganTime;
		float			m_fMovedDistOfLastSpeed;
	};

}