#include "stdafx.h"
#include "CLuaIO.h"
#include "CodeCvs.h"

#define NUMBER_SCAN		"%lf"
#define NUMBER_FMT		"%.14g"

namespace sqr
{
	CLuaIO::CLuaIO()
		: m_file(NULL)
	{
		memset(m_buf, 0, BUF_SIZE);
	}

	CLuaIO::~CLuaIO()
	{
	}

	CLuaIO* CLuaIO::Open( const char* szFileName, const char* szMode )
	{
#ifdef _WIN32
		FILE* file = _wfopen(utf8_to_utf16(szFileName).c_str(), utf8_to_utf16(szMode).c_str());
#else
		FILE* file = fopen(szFileName, szMode);
#endif
		if ( !file )
			return NULL;

		CLuaIO* pIOObj = new CLuaIO();
		if ( !pIOObj )
			return NULL;

		pIOObj->m_file = file;
		return pIOObj;
	}

	void CLuaIO::Close()
	{
		if ( 0 != m_file )
		{
			fclose(m_file);
			m_file = NULL;
		}
		delete this;
	}

	const char* CLuaIO::ReadAll()
	{
		return readChars(~((size_t)0)); // read MAX_SIZE_T chars
	}

	double CLuaIO::ReadNum()
	{
		if ( !m_file )
			return 0.0;

		double d;
		if (fscanf(m_file, NUMBER_SCAN, &d) == 1) 
		{
			return d;
		}
		else return 0.0;  /* read fails */
	}

	const char* CLuaIO::ReadLine()
	{
		for (;;) 
		{
			size_t l;
			memset(m_buf, 0, sizeof(m_buf));
			if (fgets(m_buf, BUF_SIZE, m_file) == NULL) 
			{  
				size_t len = strlen(m_buf);
				// check whether read something 
				if ( len > 0 )
					return m_buf;
				else
					return 0;  
			}

			l = strlen(m_buf);
			if ( l != 0 )
			{
				if ( m_buf[l-1] == '\n' )
					m_buf[l-1] = 0;
				
				return m_buf;
			}
		}
	}

	const char* CLuaIO::readChars( size_t n )
	{
		if ( !m_file )
			return 0;

		size_t rlen;		// how much to read
		size_t nr;			// number of chars actually read 
		rlen = BUF_SIZE;	// try to read that much each time
		char buf[BUF_SIZE + 1];
		m_grow_buf = "";
		do 
		{
			if (rlen > n) 
			{
				rlen = n;  // cannot read more than asked
			}
			
			memset(buf, 0, sizeof(buf));
			nr = fread(buf, sizeof(char), rlen, m_file);
			if ( nr > 0 )
			{
				m_grow_buf += buf;
			}
			
			n -= nr;		// still have to read `n' chars
		} 
		while ( n > 0 && nr == rlen );  // until end of count or eof
		
		return m_grow_buf.c_str();
	}

	void CLuaIO::Flush()
	{
		if ( !m_file )
			return;

		fflush(m_file);
	}

	void CLuaIO::WriteNum( double num )
	{
		if ( !m_file )
			return;

		fprintf(m_file, NUMBER_FMT, num);
	}

	void CLuaIO::WriteString( const char* str )
	{
		if ( !str || !m_file )
			return;

		size_t l = strlen(str);
		fwrite(str, sizeof(char), l, m_file);
	}

	int CLuaIO::Seek( int nOffset, const char* szWhere )
	{
		if ( !m_file || !szWhere )
			return -1;

		static const int mode[] = {SEEK_SET, SEEK_CUR, SEEK_END};
		
		int op = 0;
		string strWhere = szWhere;
		if ( strWhere == "set" )
		{
			op = 0;
		}
		else if ( strWhere == "cur" )
		{
			op = 1;
		}
		else if ( strWhere == "end" )
		{
			op = 2;
		}
		else
		{
			return -1;
		}
		
		op = fseek(m_file, nOffset, mode[op]);
		if ( 0 != op)
			return -1; // failed

		return 0;
	}
}
