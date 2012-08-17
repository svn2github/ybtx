#pragma once
//#include "CTick.h"
#include "TConnectionMgr.h"
#include "CTraitsStress.h"
//#include "IPipeHandler.h"
#include "CAddress.h"

namespace sqr
{
	class IPipeModule;
	class IPipeReactor;
	class IPipe;

	class IConnMgrClientHandler;
	class CConnStress;

	template <typename Traits>
	class TConnection;

	class CConnMgrStress
		:public TConnectionMgr<CTraitsStress>
	{
		friend class CConnStress;
	public:
		CConnMgrStress();
		~CConnMgrStress(void);

		CConnStress* Connect(const CAddress& addr);

	private:
		typedef TConnectionMgr<CTraitsStress>	Parent_t;
		friend class TConnectionMgr<CTraitsStress>;
	};
}
