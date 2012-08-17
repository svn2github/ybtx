#pragma once

namespace sqr
{
	class CFrameRateAdviser
	{
	private:
		static CFrameRateAdviser* m_pFrameRateAdviser;
	public:
		static CFrameRateAdviser* Inst();
	public:
		CFrameRateAdviser(uint32 uFrameNum);
		~CFrameRateAdviser(void);

		uint32 GetFrameCyc()const;

		//uTime∫¡√Î
		void TellFrameTime(uint32 uTime);
	private:
		deque<uint32>		m_deqTime;
		static const uint16	MaxRecordNum;
		uint32				m_uTotalTime;
		uint32				m_uFrameNum;
	};

}