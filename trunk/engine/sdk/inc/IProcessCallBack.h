#pragma once

namespace sqr
{
	class IProcessCallBack 
	{
	public:
		virtual void Clear(void)				= 0;
		virtual void DoProcess( float process ) = 0;
	};
}