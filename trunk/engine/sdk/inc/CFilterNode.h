#pragma once
#include "CRenderCommand.h"
#include "CRenderStyle.h"
namespace sqr
{
	typedef DWORD PARAM_ID;
	class CFilterNodePtrComp
	{
	public:
		bool operator()(const FilterNode* pLeft, const FilterNode* pRight) const;
	};
	typedef GList<FilterNode>	NodePool;

	typedef GList<FilterNode*>	NodeList;
	typedef GMap<FilterNode*,NodeList::iterator,CFilterNodePtrComp> MapList;
	class CRenderList;
	class FilterNode : public CRenderCommand
	{
		friend class CRenderList;
	public:
		FilterNode();
		FilterNode(const FilterNode& fn);
		~FilterNode();
	public:
		//äÖÈ¾Êý¾Ý
		RenderParam			m_RP;
		GVector<uint>		sortSeed;
		CPieceRenderStyle*	pRenderStyle;
		CAxisAlignedBox*	pAABox;
		void*				pUser;
		ERenderObjStyle		m_eType;
		CRenderGroup*		m_ParentGroup;

		void				Release(void);		

		FilterNode&			operator=(const FilterNode& fn);
		CRenderCommand&		operator=(const CRenderCommand& Com);
		CRenderOperation&	operator=(const CRenderOperation& op);
	protected:
		void				_Remove(void);	
		MapList::iterator	m_node;
		CRenderList*		m_ParentList;
	};
}