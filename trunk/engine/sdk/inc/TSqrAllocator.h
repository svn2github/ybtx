#pragma once

namespace sqr
{

	template<typename T, typename AllocObject> 
	class TSqrAllocator
	{ 
	public:
		template <typename U, typename AllocObjectU> 
		friend class TSqrAllocator;

		typedef size_t    size_type; 
		typedef ptrdiff_t difference_type; 
		typedef T*        pointer; 
		typedef const T*  const_pointer; 
		typedef T&        reference; 
		typedef const T&  const_reference; 
		typedef T         value_type; 

		TSqrAllocator() throw();
		
		TSqrAllocator(const TSqrAllocator&) throw(); 
		
		template<class U> 
		TSqrAllocator(const TSqrAllocator<U, AllocObject>&) throw();
		
		~TSqrAllocator() throw(); 

		template<class U> 
		struct rebind 
		{ 
		public:
			typedef TSqrAllocator<U, AllocObject> other; 
		};

		bool operator==(const TSqrAllocator<T, AllocObject>& )const throw();
		
		pointer allocate(size_type size);
		
		void deallocate(pointer p, size_type );
		
		size_type max_size() const;

		void construct(pointer p, const value_type& value);
		
		void destroy(pointer p);
	};


#define DeclarePoolAllocator(ClassName, AllocObject) \
	template<typename T>\
	class ClassName\
	:public sqr::TSqrAllocator< T, AllocObject>\
	{\
	typedef sqr::TSqrAllocator< T, AllocObject> Parent_t;\
	public:\
	ClassName(){}\
	ClassName(const Parent_t&){}\
	};
}
