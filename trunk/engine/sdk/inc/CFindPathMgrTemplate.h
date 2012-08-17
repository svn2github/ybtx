#pragma once


namespace sqr
{

	template< typename buf >
	class CFindPathMgrTemplate
	{
	public:
		void	SetBuf( buf*  pBuf )
		{
			m_pBuf = pBuf;
		}
		uint8	GetBlock( uint32 uX, uint32 uY )
		{
			return m_pBuf->GetBlock( uX, uY );
		}

		uint32	GetWidth()
		{
			return m_pBuf->GetWidth();
		}
		uint32	GetHeight()
		{
			return m_pBuf->GetHeight();
		}

	public:
		buf* m_pBuf;
	};

}

