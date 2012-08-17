#ifndef _ATOMIC_HPP
#define _ATOMIC_HPP

#pragma once

#include <windows.h>
#include "types.hpp"

namespace sqr
{
	template <typename T>
	class atomic
	{
	public:
		explicit atomic(T const & rhs);
		atomic(atomic const & rhs);

		T value() const;
		void value(T const & rhs);

		atomic& operator=(T const & rhs);
		atomic& operator=(atomic const & rhs);

		bool operator<(T const & rhs) const;
		bool operator<(atomic const & rhs) const;
		bool operator<=(int32_t const & rhs) const;
		bool operator<=(T const & rhs) const;
		bool operator>(T const & rhs) const;
		bool operator>(atomic const & rhs) const;
		bool operator>=(T const & rhs) const;
		bool operator>=(atomic const & rhs) const;

		bool operator==(T const & rhs) const;
		bool operator==(atomic const & rhs) const;

		bool operator!=(T const & rhs) const;
		bool operator!=(atomic const & rhs) const;

		T const & operator++();
		T const & operator--();
		T operator++(int);
		T operator--(int);

		atomic& operator+=(T const & rhs);
		atomic& operator+=(atomic const & rhs);
		atomic& operator-=(T const & rhs);
		atomic& operator-=(atomic const & rhs);
		atomic& operator&=(T const & rhs);
		atomic& operator&=(atomic const & rhs);
		atomic& operator|=(T const & rhs);
		atomic& operator|=(atomic const & rhs);
		atomic& operator^=(T const & rhs);
		atomic& operator^=(atomic const & rhs);

		bool cas(T const & old_val, T const & new_val);
	};

	template <>
	class atomic<int32_t>
	{
	public:
		atomic()
		{
		}

		explicit atomic(int32_t rhs)
		{
			this->value(rhs);
		}

		int32_t value() const
		{
			return value_;
		}

		void value(int32_t const & rhs)
		{
			InterlockedExchange(reinterpret_cast<long*>(&value_), rhs);
		}

		atomic& operator=(int32_t const & rhs)
		{
			this->value(rhs);
			return *this;
		}
		atomic& operator=(atomic const & rhs)
		{
			this->value(rhs.value_);
			return *this;
		}

		bool cas(int32_t const & old_val, int32_t const & new_val)
		{
			return old_val == InterlockedCompareExchange(reinterpret_cast<long*>(&value_), new_val, old_val);
		}

		bool operator<(int32_t const & rhs) const
		{
			return this->value() < rhs;
		}
		bool operator<(atomic const & rhs) const
		{
			return this->value() < rhs.value();
		}
		bool operator<=(int32_t const & rhs) const
		{
			return this->value() <= rhs;
		}
		bool operator<=(atomic const & rhs) const
		{
			return this->value() <= rhs.value();
		}
		bool operator>(int32_t const & rhs) const
		{
			return this->value() > rhs;
		}
		bool operator>(atomic const & rhs) const
		{
			return this->value() > rhs.value();
		}
		bool operator>=(int32_t const & rhs) const
		{
			return this->value() >= rhs;
		}
		bool operator>=(atomic const & rhs) const
		{
			return this->value() >= rhs.value();
		}

		bool operator==(int32_t const & rhs) const
		{
			return this->value() == rhs;
		}
		bool operator==(atomic const & rhs) const
		{
			return this->value() == rhs.value();
		}

		bool operator!=(int32_t const & rhs) const
		{
			return this->value() != rhs;
		}
		bool operator!=(atomic const & rhs) const
		{
			return this->value() != rhs.value();
		}

		atomic& operator+=(int32_t const & rhs)
		{
			InterlockedExchangeAdd(reinterpret_cast<long*>(&value_), rhs);
			return *this;
		}
		atomic& operator+=(atomic const & rhs)
		{
			InterlockedExchangeAdd(reinterpret_cast<long*>(&value_), rhs.value_);
			return *this;
		}

		atomic& operator-=(int32_t const & rhs)
		{
			InterlockedExchangeAdd(reinterpret_cast<long*>(&value_), -rhs);
			return *this;
		}
		atomic& operator-=(atomic const & rhs)
		{
			InterlockedExchangeAdd(reinterpret_cast<long*>(&value_), -rhs.value_);
			return *this;
		}

		atomic& operator*=(int32_t const & rhs)
		{
			int32_t comperand;
			int32_t exchange;
			do
			{
				comperand = value_;
				exchange = comperand * rhs;
			} while (!this->cas(comperand, exchange));
			return *this;
		}

		atomic& operator/=(int32_t const & rhs)
		{
			int32_t comperand;
			int32_t exchange;
			do
			{
				comperand = value_;
				exchange = comperand / rhs;
			} while (!this->cas(comperand, exchange));
			return *this;
		}

		atomic& operator%=(int32_t const & rhs)
		{
			int32_t comperand;
			int32_t exchange;
			do
			{
				comperand = value_;
				exchange = comperand % rhs;
			} while (!this->cas(comperand, exchange));
			return *this;
		}

		atomic& operator&=(int32_t const & rhs)
		{
			int32_t comperand;
			int32_t exchange;
			do
			{
				comperand = value_;
				exchange = comperand & rhs;
			} while (!this->cas(comperand, exchange));
			return *this;
		}

		atomic& operator|=(int32_t const & rhs)
		{
			int32_t comperand;
			int32_t exchange;
			do
			{
				comperand = value_;
				exchange = comperand | rhs;
			} while (!this->cas(comperand, exchange));
			return *this;
		}

		atomic& operator^=(int32_t const & rhs)
		{
			int32_t comperand;
			int32_t exchange;
			do
			{
				comperand = value_;
				exchange = comperand ^ rhs;
			} while (!this->cas(comperand, exchange));
			return *this;
		}

		int32_t const & operator++()
		{
			InterlockedIncrement(reinterpret_cast<long*>(&value_));
			return value_;
		}

		int32_t const & operator--()
		{
			InterlockedDecrement(reinterpret_cast<long*>(&value_));
			return value_;
		}

		int32_t operator++(int)
		{
			long old_val;
			long new_val;
			do
			{
				old_val = value_;
				new_val = old_val + 1;		
			} while (!this->cas(old_val, new_val));
			return old_val;
		}

		int32_t operator--(int)
		{
			long old_val;
			long new_val;
			do
			{
				old_val = value_;
				new_val = old_val - 1;		
			} while (!this->cas(old_val, new_val));
			return old_val;
		}

	private:
		mutable int32_t value_;
	};

}

#endif
