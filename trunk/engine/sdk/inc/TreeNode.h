#pragma once 
#include "GUIBase.h"

namespace sqr
{
	struct TreeNode;

	struct NodeContain	: public SQRGuiMallocObject
	{ 
		TreeNode* pNode; 
		bool operator< ( const NodeContain& n ) const;
	};

	struct TreeNode : public SQRGuiMallocObject
	{
		uint32						m_uParam;
		uint32						m_uAscendData;
		uint32						m_uState;
		UIWString						m_NodeName;
		WND_IMAGE_LIST				m_ImageHead;
		WND_IMAGE_LIST				m_ImageTail;
		TreeNode*					m_pParent;
		UIList<NodeContain>			m_Children;
		UIList<NodeContain>::iterator m_it;

		TreeNode() : m_pParent(0), m_uAscendData(0), m_uState(0), m_uParam(0)
		{
		};

		~TreeNode()
		{
			for( UIList<NodeContain>::iterator it = m_Children.begin(); it != m_Children.end(); it++ )
				delete (*it).pNode;
			m_Children.clear();
		}

		bool	MsgWalk( SQRTreeCtrl* pTree, CFRect Btn, float xPos, float& yPos, uint32 uMsg, CFPos CursorPos );
		void	DrawNode( SQRTreeCtrl* pTree, CFRect Btn, float xPos, float& yPos );
		void	SetSelectNode( TreeNode* pNode );
		int32	GetShowNodeCount();
		bool	GetNodeShowPos( TreeNode* pNode, int32& nPos );
	};

}