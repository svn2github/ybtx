#pragma once

#include <streambuf>
#include <istream>
#include <fstream>
#include "CPkgFile.h"

namespace sqr
{

#ifdef _WIN32
#pragma warning (push)
#pragma warning (disable:4511)
#pragma warning (disable:4512)
#endif

	template <class Elem, class Tr, class Alloc>
	class basic_ipkgstream;

	template<class Elem,class Tr=std::char_traits<Elem>,class Alloc=std::allocator<Elem> >
	class basic_pkgbuf
		:public std::basic_streambuf<Elem,Tr>
	{
	private:
		friend class basic_ipkgstream<Elem,Tr,Alloc>;
		typedef Elem char_type;
		typedef Tr traits_type;
		typedef typename traits_type::int_type int_type;
		typedef typename traits_type::off_type off_type;
		typedef typename traits_type::pos_type pos_type;

		char_type buf[1];

		CPkgFile	_file;

	protected:
		basic_pkgbuf()
		{
			setg(buf,buf,buf);
		}

		virtual int_type underflow()
		{
			if( 0==_file.Read(&buf,sizeof(buf)) )
				return traits_type::eof();

			setg(buf,buf,buf+1);

			return traits_type::to_int_type(buf[0]);
		}

		virtual pos_type seekpos(pos_type _Sp,std::ios_base::openmode /*_Which*/)
		{
			int result=_file.Seek(_Sp);
			if (result)
				return traits_type::eof();
			return _file.Tell();
		}

		virtual pos_type seekoff(off_type _Off,std::ios_base::seekdir _Way,std::ios_base::openmode /*_Which*/)
		{
			int result=1;
			switch (_Way)
			{
			case std::ios_base::beg:
				result=_file.Seek(_Off,SEEK_SET);
				break;
			case std::ios_base::end:
				result=_file.Seek(_Off,SEEK_END);
				break;
			case std::ios_base::cur:
				result=_file.Seek(_Off,SEEK_CUR);
				break;
			default:
				GenErr("invalid seekdir arg!");
			}
			if (result)
				return traits_type::eof();
			return _file.Tell();
		}

	public:
		int open(const wchar_t* alias, const char *_Filename, bool _Binary, ReadOrder eReadFirst = eRO_Normal)
		{
			return (_file.Open(alias, _Filename,_Binary,eReadFirst));
		}

		void close()
		{
			_file.Close();
		}

		bool isopen()const
		{
			return _file.IsValid();
		}

		bool isinpackage()const
		{
			return _file.IsInPackage();
		}

		std::size_t size()const
		{
			return _file.Size();
		}
	};


	template <class Elem, class Tr = std::char_traits<Elem>, class Alloc = std::allocator<Elem> >
	class basic_ipkgstream
		:public std::basic_istream<Elem, Tr>
	{
	private:
		basic_pkgbuf<Elem,Tr,Alloc>	_sb;
	public:
		explicit basic_ipkgstream()
			:std::basic_istream<Elem,Tr>(&_sb)
		{
			//this->init(&_sb);
		}

		explicit basic_ipkgstream(const wchar_t* alias, const char* _Filename, bool _Binary=true, ReadOrder eReadFirst = eRO_Normal)
			:std::basic_istream<Elem,Tr>(&_sb)
		{
			//this->init(&_sb);
			if (_sb.open(alias, _Filename,_Binary, eReadFirst) != eFE_SUCCESS)
				this->setstate( std::ios::failbit );
		}

		int open(const wchar_t* alias, const char *_Filename, bool _Binary=true, ReadOrder eReadFirst = eRO_Normal)
		{
			this->clear();
			int ret = _sb.open(alias, _Filename, _Binary, eReadFirst);
			if ( eFE_SUCCESS != ret )
			{
				this->setstate( std::ios::failbit );
			}
			
			return ret;
		}

		void close()
		{
			_sb.close();
		}

		bool isopen()const
		{
			return _sb.isopen();
		}

		bool isinpackage()const
		{
			return _sb.isinpackage();
		}

		size_t size()const
		{
			return _sb.size();
		}
	};


#ifdef _WIN32
#pragma warning (pop)
#endif

	typedef basic_ipkgstream<char>	ipkgstream;
	typedef basic_ipkgstream<wchar_t> wipkgstream;

	typedef std::ofstream	opkgstream;
	typedef std::wofstream	wopkgstream;

	//template class basic_ipkgstream<char>;
	//template class basic_ipkgstream<wchar_t>;
}

