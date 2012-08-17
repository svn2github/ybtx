#pragma once

namespace sqr
{
	template <class T>
	class TSafeBuffer
	{
	public:
		TSafeBuffer(void* pBuffer, uint32 max /*BufferµÄ³¤¶È*/)
		{
			m_pBuffer = static_cast<T*>(pBuffer);
			m_uMax = max / sizeof(T);
		}

		T&	operator[](uint32 idx)
		{
			Ast( idx < m_uMax );
			return m_pBuffer[idx];
		}
	protected:
		T*		m_pBuffer;
		uint32	m_uMax;
	};
}