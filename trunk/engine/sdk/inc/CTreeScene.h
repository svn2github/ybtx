#pragma once
#include "CRenderNode.h"
#include "CElementManager.h"

namespace sqr
{	
	typedef void CtrlFunHandle(CRenderNode* pChild,void* param);

	class CSceneTreeNode 
		: public CElementNode
		, protected CRenderSpace
	{
	public:
		CSceneTreeNode( CElementManager* pParent , URegionID id )
		: CElementNode(pParent,id)
		{};
		virtual ~CSceneTreeNode(){};
		void	AddChild( CRenderNode* pChild );
		bool	DelChild( CRenderNode* pChild );

		virtual bool	Intersects( const CVector3f& vec )	= 0;	
		virtual	void	Render(uint32 CurTime)				= 0;
		virtual	void	Update(CCamera* pCam)				= 0;
		virtual void	DoCtrl( CtrlFunHandle* pFun,void* param ) = 0;
	protected:
		virtual void	InsertNode(CRenderNode* pObj)		= 0;
		virtual bool	DeleteNode(CRenderNode* pObj)		{ return false; };
	};
	
	class CTreeScene : public CElementManager
	{
	public:
		CSceneTreeNode*	GetTreeNode( URegionID ID );
			bool		Intersects( const CVector3f& vec );	
			void		Render(uint32 CurTime);
			void		Update(CCamera* pCam);
			void		DoCtrl( CtrlFunHandle* pFun,void* param );
	};
}