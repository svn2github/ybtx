#pragma once

namespace sqr
{

	//==============================	container use by CCoordinateAdapter	=====================
	template< typename type >
	class Vector2DContainer
	{
	public:
		Vector2DContainer()
			: m_uWidth(0)
		{}

		Vector2DContainer( uint32 uWidth, uint32 uHeight )
		{
			Alloc( uWidth, uHeight );
		}

		void Alloc( uint32 uWidth, uint32 uHeight )
		{
			m_uWidth = uWidth;
			m_buf.resize( uHeight );
			for ( uint32 i=0; i<uHeight; ++i )
			{
				m_buf[i].resize( uWidth );
				m_buf[i].assign( uWidth, 0 );
			}
		}

		inline uint32 GetHeight()
		{
			return m_buf.size();
		}
		inline uint32 GetWidth()
		{
			return m_uWidth;
		}
	public:
		vector< vector< type> > m_buf;
		uint32 m_uWidth;
	};

	template< typename type, uint32 WIDTH, uint32 HEIGHT >
	class Array2DContainer
	{
	public:
		Array2DContainer()
		{
			memset( m_buf, 0, sizeof(m_buf) );
		}
		inline uint32 GetHeight()
		{
			return HEIGHT;
		}
		inline uint32 GetWidth()
		{
			return WIDTH;
		}

	public:
		type m_buf[HEIGHT][WIDTH];
	};

	template< typename type >
	class Map2DContainer
	{
	public:
		Map2DContainer()
			: m_uWidth(0), m_uHeight(0)
		{}

		Map2DContainer( uint32 uWidth, uint32 uHeight  )
			: m_uWidth(uWidth), m_uHeight(uHeight)
		{}

	protected:
		map< pair<uint32,uint32>, type>	m_buf;
		uint32 m_uWidth;
		uint32 m_uHeight;
	};

	//=================================================================================================

	//==============================	CCoordinateAdapter		=======================================
	template< typename type, typename container_type >
	class CCoordinateAdapter
	{
	public:
		inline uint32 GetWidth()
		{
			return m_container.GetWidth();
		}
		inline uint32 GetHeight()
		{
			return m_container.GetHeight();
		}

		inline bool IsXYFail( uint32 uX, uint32 uY )
		{
			return ( uX >= m_container.GetWidth() || uY >= m_container.GetHeight() ) ;
		}

		inline bool IsIdFail( uint32 uId )
		{
			return ( uId >= m_container.GetWidth() * m_container.GetHeight() );
		}

		CCoordinateAdapter( uint32 uWidth, uint32 uHeight )
			: m_container( uWidth, uHeight )
		{}

		CCoordinateAdapter()
			: m_container()
		{}

		inline type GetElem( uint32 uX, uint32 uY )
		{
			if ( IsXYFail( uX,uY ) )
				return NULL;
			return m_container.m_buf[uY][uX];
		}

		inline type GetElem( uint32 uId )
		{
			uint32 uX=uId % GetWidth();
			uint32 uY=uId / GetWidth();
			return GetElem( uX, uY );
		}

		inline void SetElem( uint32 uX, uint32 uY, type val )
		{
			//if( IsXYFail( uX, uY ) )
			//	return;

			m_container.m_buf[uY][uX]	= val;
		}

		inline void SetElem( uint32 uId, type val )
		{
			//if( IsIdFail( uId ) )
			//	return;

			SetElem( uId % GetWidth(), uId / GetWidth(), val );
		}

	protected:
		container_type	m_container;
	};
	//=================================================================================================

	//========================		特化 CCoordinateAdapter<Map2DContainer>	===========================
	template< typename type >
	class CCoordinateAdapter< type, Map2DContainer<type> >
	{
	public:
		inline uint32 GetWidth()
		{
			return m_container.GetWidth();
		}
		inline uint32 GetHeight()
		{
			return m_container.GetHeight();
		}

		inline bool IsXYFail( uint32 uX, uint32 uY )
		{
			return ( uX >= m_container.GetWidth() || uY >= m_container.GetHeight() ) ;
		}

		inline bool IsIdFail( uint32 uId )
		{
			return ( uId >= m_container.GetWidth() * m_container.GetHeight() );
		}

		CCoordinateAdapter( uint32 uWidth, uint32 uHeight )
			: m_container( uWidth, uHeight )
		{}

		CCoordinateAdapter()
			: m_container()
		{}

		inline type& GetElem( uint32 uX, uint32 uY )
		{
			typename map< pair<uint32,uint32>, type>::iterator it;
			it = m_container.m_buf.find( pair<uint32,uint32>(uX,uY) );
			if ( it == m_container.m_buf.end() )
				return (type)0;
			else
				return *it;
		}

		inline type& GetElem( uint32 uId )
		{
			return GetElem( uId % GetWidth(), uId / GetWidth() );
		}

		inline void SetElem( uint32 uX, uint32 uY, type val )
		{
			m_container.m_buf.insert( pair<uint32,uint32>(uX,uY), val );
		}

		inline void SetElem( uint32 uId, type val )
		{
			//if( IsIdFail( uId ) )
			//	return;

			SetElem( uId % GetWidth(), uId / GetWidth(), val );
		}

	protected:
		Map2DContainer<type>	m_container;
	};
	//=================================================================================================


	//========================		对外导出的 CCoordinate 容器			==============================
	template< typename type >
	class CCoordinateVector
		: public CCoordinateAdapter<  type, Vector2DContainer<type>  >
	{
	private:
		typedef CCoordinateAdapter<  type, Vector2DContainer<type>  > _BaseClassType;
	public:
		CCoordinateVector( uint32 uWidth, uint32 uHeight )
			: _BaseClassType( uWidth, uHeight )
		{}

		CCoordinateVector()
			: _BaseClassType()
		{}

		inline void Alloc( uint32 uWidth, uint32 uHeight )
		{
			_BaseClassType::m_container.Alloc( uWidth, uHeight );
		}
	};

	template< typename type, uint32 WIDTH, uint32 HEIGHT  >
	class CCoordinateArray
		: public CCoordinateAdapter<  type, Array2DContainer<type,WIDTH,HEIGHT>  >
	{};

	template< typename type >
	class CCoordinateMap
		: public CCoordinateAdapter<  type, Map2DContainer<type>  >
	{
		typedef CCoordinateAdapter<  type, Map2DContainer<type>  > _BaseClassType;
	public:
		CCoordinateMap( uint32 uWidth, uint32 uHeight )
			: _BaseClassType( uWidth, uHeight )
		{}

	};

}
