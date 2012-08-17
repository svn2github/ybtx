#pragma once

namespace sqr
{
	class SGrid;
	class CMapEditObject;

	class CModelProcesser
	{
	public:
		CModelProcesser();
		virtual ~CModelProcesser();

		void	ProcessTile( uint32 x, uint32 z );

	private:
		virtual void
				ProcessModel( uint32 iModelGroup, CMapEditObject& model ) = 0;

	private:
		CModelProcesser( const CModelProcesser& );
		CModelProcesser& operator = ( const CModelProcesser& );

		void	processGrid( const SGrid& grid );
	};

}