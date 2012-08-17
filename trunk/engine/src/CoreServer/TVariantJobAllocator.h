#pragma once

namespace sqr
{
	template <class T>
	class TVariantJobAllocator
	{
	public:
		typedef T					value_type;
		typedef value_type*			pointer;
		typedef const value_type*	const_pointer;
		typedef value_type&			reference;
		typedef const value_type&	const_reference;
		typedef size_t				size_type;
		typedef ptrdiff_t			difference_type;

		template <class U>
		struct rebind
		{
		public:
			typedef TVariantJobAllocator<U>	other;
		};

		template <class U>
		inline bool operator==(const TVariantJobAllocator<U>& ano)const
		{
			return true;
		}

		inline TVariantJobAllocator();
		inline pointer allocate(size_type sz);
		inline void deallocate(pointer p, size_type /*n*/);
		inline void construct(pointer p, const value_type& value);
		inline void destroy(pointer p);
		inline size_type max_size() const;
	private:
		bool	m_bWorkInCreateJobThread;
	};

}
