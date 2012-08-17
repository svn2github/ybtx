#pragma once

#include <iosfwd>
#include "CMathMallocObject.h"

namespace sqr
{
	class longint : public CMathMallocObject
	{
	public:
		longint();
		longint(const char*);
		longint(const longint&);
		~longint();
		void clear();

		longint operator <<(const unsigned)const;
		longint operator >>(const unsigned)const;

		longint shiftright(const unsigned);
		longint shiftleft(const unsigned);

		longint operator -()const;

		longint operator ++(const int);
		longint operator --(const int);
		longint operator --();
		longint operator ++();

		longint operator +(const longint&)const;
		longint operator -(const longint&)const;
		longint operator /(const longint&)const;
		longint operator *(const longint&)const;
		longint inverse(const longint&)const;
		longint power(const longint&)const;
		longint abs()const;
		longint powermod(const longint&,const longint&)const;
		longint sqrt()const;
		longint rand(unsigned);
		longint(unsigned a);
		longint operator %(const longint&)const;


		bool operator >=(const longint&)const;
		bool operator <(const longint&)const;
		bool operator <=(const longint&)const;
		bool operator !=(const longint&)const;
		bool operator >(const longint&)const;
		bool operator ==(const longint&)const;
		bool iseven()const;
		bool isodd()const;
		bool iszero()const;
		bool isone()const;
		bool ispositive()const;
		bool isprime(const unsigned&)const;

		const longint& operator =(const longint&);
		const longint& operator %=(const longint&);
		const longint& operator -=(const longint&);

		void cutzero();

		static void swap(longint*a,longint*b);

		unsigned length;
		int sign;
		unsigned *number;
	private:
		friend class monty;

		/*template<typename CharType,typename Traits>
		std::basic_ostream<CharType, Traits>&
			operator<<(std::basic_ostream<CharType, Traits>& os,const longint& i)
		{
			os.write(&sign,sizeof(sign));
			os.write(&length,sizeof(length));
			os.write(number,length*sizeof(unsigned));
			return os;
		}

		template<typename CharType,typename Traits>
		std::basic_ostream<CharType, Traits>&
			operator>>(std::basic_ostream<CharType, Traits>& is,const longint& i)
		{
			is.read(sizeof(sign),&sign);
			is.read(sizeof(length),&length);
			delete[]number;
			number=NULL;
			number=new unsigned [length];
			is.read(number,length*sizeof(unsigned));
			return is;
		}*/

		/*template<typename CharType,typename Traits>
		friend std::basic_ostream<CharType, Traits>&
		operator<<(std::basic_ostream<CharType, Traits>& os,const longint& i);

		template<typename CharType,typename Traits>
		friend std::basic_ostream<CharType, Traits>&
		operator>>(std::basic_ostream<CharType, Traits>& is,const longint& i);*/
	};
}
