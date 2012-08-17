#pragma once
#include "CFilterNode.h"

namespace sqr
{
	//以后可以考虑写成模板
	class CRenderList
	{
	public:
		CRenderList(void);
		~CRenderList(void);

		typedef MapList::iterator	Key;
		typedef NodeList::iterator	iterator;
		
		bool	insert(FilterNode* pNode);
		void	erase(const FilterNode* pNode);
		//void	erase()
		
		iterator begin(void);
		iterator end(void);

		bool	empty(void) const;
		void	clear(void);
	protected:
		NodeList	TraverseList; 
		MapList		FindList;
	};
}