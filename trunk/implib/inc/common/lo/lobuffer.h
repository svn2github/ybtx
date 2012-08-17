#ifndef __lo_BUFFER_H__
#define __lo_BUFFER_H__

#include <common/lo/lodefine.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>


DEFINE_NAMESPACE(locom)

/**
* 数据值(简单的数据存取类,实现简单的string)
*
* @author loach

* @date 2009-04-26
*/
class CloBuffer
{
public:

#define lo_Buffer_tdata_char(tj,c){\
	(tj).data = (char*)c;\
	(tj).len = strlen(c);}

#define lo_Buffer_tdata_char_len(tj,c,size){\
	(tj).data = (char*)c;\
	(tj).len = size;}

#define lo_Buffer_tdata_wchar(tj,c){\
	(tj).data = (char*)c;\
	(tj).len = wcslen(c)*sizeof(wchar_t);}

#define lo_Buffer_tdata_wchar_len(tj,c,size){\
	(tj).data = (char*)c;\
	(tj).len = size*sizeof(wchar_t);}

#define lo_Buffer_tdata_buff(tj,c,size){\
	(tj).data = (char*)c;\
	(tj).len = size;}

	// 为 char 类型
#define lo_Buffer_for_char_copy(buffer_lo,buf) {\
	size_t l = strlen(buf);\
	(buffer_lo).str(buf,l);}
	
#define lo_Buffer_for_char_copy_len(buffer_lo,buf,l) {\
	(buffer_lo).str(buf,l);}
	
	// 为 wchar 类型
#define lo_Buffer_for_wchar_copy(buffer_lo,buf) {\
	size_t l = wcslen(buf);\
	(buffer_lo).str(buf,l*sizeof(wchar_t));}

#define lo_Buffer_for_wchar_copy_len(buffer_lo,buf,size) {\
	(buffer_lo).str((const char*)buf,size*sizeof(wchar_t));}

	// 为 buff , l指定长度
#define lo_Buffer_for_buff_copy(buffer_lo,buf,l) \
	(buffer_lo).str(buf,l);

	struct tdata
	{
		char*  data;
		size_t len;
	};
	typedef void* pbuffer_data_lo;

	CloBuffer()
	{
		_tdata_lo.data = 0;
		_tdata_lo.len = 0;
	}
	CloBuffer(const CloBuffer& d)
	{
		_tdata_lo.data = 0;
		_tdata_lo.len = 0;
		*this = d;
	}
	const CloBuffer& operator = (const CloBuffer& d)
	{
		str( d.data() ,d.length() );
		return *this;
	}

	// read interface
	const char* c_str(void) const{
		if( _tdata_lo.data )
		{
			return _tdata_lo.data;
		}
		return "";
	}
	const wchar_t* wc_str(void){
		if( _tdata_lo.data )
		{
			return (wchar_t*)_tdata_lo.data;
		}
		return L"";
	}

	char* data(void) const{
		return _tdata_lo.data;
	}
	wchar_t* wdata(void){
		return (wchar_t*)_tdata_lo.data;
	}

	size_t length() const{
		return _tdata_lo.len;
	}
	size_t wlength() const{
		return _tdata_lo.len/sizeof(wchar_t);
	}

	bool operator == (const CloBuffer& buff)
	{
		if( length() != buff.length() )
			return false;
		return ( memcmp( c_str(),buff.c_str(),length() ) == 0 ) ? true : false ;
	}

	void operator = ( const tdata& f )
	{
		str(f.data , f.len );
	}

	void operator += ( const tdata& f )
	{
		if( f.len <= 0 )
			return ;
		internal_realloc(f.len);
		internal_append(f.data , f.len );
	}

	void operator += ( const CloBuffer& buff)
	{
		if( buff.length() <= 0 )
			return ;
		internal_realloc( buff.length() );
		internal_append( buff.data() , buff.length() );
	}

	bool isequal(const char* buff , size_t l) const
	{
		if( length() != l || l <= 0 || 0 == buff )
			return false;

		return ( memcmp( c_str() , buff , l ) == 0 ) ? true : false ;
	}

	size_t rfind(const tdata& f)
	{ //简单查询，不适用大数据量查询
		if( _tdata_lo.data && _tdata_lo.len > 0 && f.data && f.len <= _tdata_lo.len )
		{
			const char* sz =  _tdata_lo.data + _tdata_lo.len - f.len ;
			const char* end = _tdata_lo.data;
			while( sz > end )
			{
				if( _memicmp(sz , f.data , f.len ) == 0 )
				{
					return (size_t)(sz - _tdata_lo.data );
				}
				sz --;
			}
		}
		return -1;
	}

	size_t find(const tdata& f)
	{//简单查询，不适用大数据量查询
		if( _tdata_lo.data && _tdata_lo.len > 0 && f.data && f.len <= _tdata_lo.len)
		{
			const char* sz =  _tdata_lo.data; 
			const char* end = _tdata_lo.data + _tdata_lo.len;
			while( (sz + f.len) <= end )
			{
				if( _memicmp(sz , f.data , f.len ) == 0 )
					return (size_t)(sz - _tdata_lo.data );
				sz ++;
			}
		}
		return -1;
	}

	CloBuffer substr(size_t at ,size_t len = -1 ){
		CloBuffer buff;
		if( at < _tdata_lo.len )
		{
			if( len == 0 || len == (size_t)-1 )
			{
				len = (_tdata_lo.len - at);
			}
			else if( at + len > _tdata_lo.len )
			{
				len = _tdata_lo.len - at;
			}
			buff.str( ( _tdata_lo.data + at ) , len ) ;						
		}
		return buff;
	}

	// write interface
	void str(const char* buff , size_t l)
	{
		if( (l > _tdata_lo.len) || !buff ){
			internal_reset(buff,l);
		}
		if( l && buff ){

			if( _tdata_lo.len >= (l + 64) ){
				//需要重新分配
				internal_reset(buff,l);
			}else{
				_tdata_lo.len = l;
				memcpy_s(_tdata_lo.data ,_tdata_lo.len , buff , l);			

				// 用 两个 0 字符来表示结束
				*(_tdata_lo.data + _tdata_lo.len   ) = '\0';
				*(_tdata_lo.data + _tdata_lo.len+1 ) = '\0';
			}
		}
	}
	// free the buffer
	void clear()
	{
		if( _tdata_lo.data && _tdata_lo.len)
		{
			free( _tdata_lo.data );
			_tdata_lo.data = 0;
			_tdata_lo.len = 0;
		}
	}

public:
	~CloBuffer()
	{
		clear();
	}
private:
	void internal_realloc( size_t l )
	{
		if( _tdata_lo.data && _tdata_lo.len > 0 )
		{
			_tdata_lo.data = reinterpret_cast<char*>( realloc(_tdata_lo.data  , (_tdata_lo.len + l + 2) ) );
		}
	}

	void internal_append(const char* buff , size_t l )
	{
		if( !_tdata_lo.data )
		{
			internal_reset( buff , l );
		}

		if( _tdata_lo.data && _tdata_lo.len )
		{
			memcpy_s( (_tdata_lo.data + _tdata_lo.len) , (_tdata_lo.len + l) , buff , l);
			_tdata_lo.len += l;
			*(_tdata_lo.data + _tdata_lo.len   ) = '\0';
			*(_tdata_lo.data + _tdata_lo.len+1 ) = '\0';
		}
	}

	/** 重新分配 */
	void internal_reset(const char* buff , size_t l)
	{
		clear();
		if( buff && l )
		{
			_tdata_lo.data = reinterpret_cast<char*>( malloc( l + 2 ) );
			memset(_tdata_lo.data,0,l + 2);
			_tdata_lo.len = l;
		}
	}
private:
	tdata _tdata_lo;
};

END_NAMESPACE(locom)

// 初始值
#define Zerotdata(tj) { tj.data =0; tj.len = 0;}

#endif //__lo_Buffer_H__
