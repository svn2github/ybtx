#pragma once


namespace sqr
{
	class ICamAnimHandler
	{
	public:
		virtual void OnKeyChanged() = 0;
		virtual void OnAnimEnd() = 0;
	};
}