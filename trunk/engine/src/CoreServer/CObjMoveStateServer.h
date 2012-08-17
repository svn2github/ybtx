#pragma once
#include "TObjMoveState.h"
#include "CSyncTraitsServer.h"

namespace sqr
{
	class CObjMoveStateServer
		:public TObjMoveState<CSyncTraitsServer>
	{
		typedef TObjMoveState<CSyncTraitsServer>	Parent_t;
	public:
		CObjMoveStateServer(CSyncCoreObjectServer* pObj,CPixelPath* pPath,float fSpeed,float fBeginDist);
		~CObjMoveStateServer(void);

		uint32 GetMoveCycDerived()const;
		void SetMoveCycDerived(uint32 uMoveCyc);
		void CreateTickingDerived(uint32 uCyc);
		void DeleteTickingDerived();

	private:
		uint32			m_uMoveCyc;
	};

}

