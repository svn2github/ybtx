#pragma once
#include "CStaticObject.h"

namespace sqr
{
	class CLuaIO : public CStaticObject
	{
	public:
		CLuaIO();
		~CLuaIO();

		static CLuaIO* Open( const char* szFileName, const char* szMode );
		void		Close();
		void		Flush();

		const char* ReadAll();
		double		ReadNum();
		const char* ReadLine();

		void		WriteNum( double num );
		void		WriteString( const char* str );

		int			Seek( int nOffset, const char* szWhere );

	private:
		const char* readChars( size_t n );

	private:
		enum
		{
			BUF_SIZE = 512,
		};

		FILE*	m_file;
		char	m_buf[BUF_SIZE];
		string	m_grow_buf;
	};
}
