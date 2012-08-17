#pragma once

namespace sqr
{
	enum EBeginServiceResult
	{
		eBSR_Success,
		eBSR_AlreadyServing,
		eBSR_PortBinded
	};

	enum EServiceState
	{
		eServiceState_Off,
		eServiceState_On,
		eServiceState_Starting,
		eServiceState_Shutting,
	};

	struct ServiceStateMap
	{
		ServiceStateMap()
		{
			mServiceStateName[eServiceState_Off]			= "eServiceState_Off";
			mServiceStateName[eServiceState_On]				= "eServiceState_On";
			mServiceStateName[eServiceState_Starting]		= "eServiceState_Starting";
			mServiceStateName[eServiceState_Shutting]		= "eServiceState_Shutting";
		}
		typedef map<EServiceState, string> ServiceStateMap_T;
		typedef ServiceStateMap_T::const_iterator  mapServiceStateName_itr;
		ServiceStateMap_T mServiceStateName;
	};

	static ServiceStateMap sServiceStateMap; 
}
