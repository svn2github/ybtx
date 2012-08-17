#pragma once

namespace sqr
{
	class Timer
	{
	public:
		void restart();
		uint elapsed();

		uint m_starttime;
	};
}