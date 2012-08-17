#pragma once
#include "CTreeScene.h"
#include "CAxisAlignedBox.h"
#include "CCamera.h"

namespace sqr
{
class CQuadTreeNode;
class CQuadTreeSpace;
class CQuadNode;
class CCamera;
class CRenderNode;

class CQuadTreeScene : public CTreeScene
{
protected:
	 CElementNode*	_CreateNode( URegionID ID );
};

//------------------------------------------------------------------------------
class CQuadNode :  public CSceneTreeNode
{
	friend class CQuadTreeSpace;
	friend class CQuadTreeNode;

public:
	CQuadNode(CElementManager* pParent , URegionID id,CQuadTreeSpace* pQuadParent
		, float MinX, float MinY ,float MaxX,float MaxY );
	virtual ~CQuadNode() {};
public:
	void	Update(CCamera* pCam);
	void	Render(uint32 CurTime);
	const	CAxisAlignedBox& GetAABB(void)	{ return m_AABB; };
protected:
	virtual void	_UpdateClipStatus(ClipStatus::Type status, CCamera* pCam)	= 0;
	virtual void	_Render(uint32 CurTime)						= 0;
	virtual void    _unRender(void)								= 0;


	bool				m_IsRendering;
	CQuadTreeSpace*		m_pQuadParent;
	CAxisAlignedBox		m_AABB;
	ClipStatus::Type	m_eClipStatus;
	float				miX,miY;
	float				MX,MY;
	float				mx,my;
};

//------------------------------------------------------------------------------
class CQuadTreeSpace : public CQuadNode
{
	friend class CQuadTreeNode;

public:
	CQuadTreeSpace(CElementManager* pParent , URegionID id, CQuadTreeSpace* pQuadParent
		, float MinX, float MinY ,float MaxX,float MaxY );
	~CQuadTreeSpace();
public:
	virtual void	InsertNode(CRenderNode* pObj);
	virtual bool	DeleteNode(CRenderNode* pObj);
			bool	Intersects( const CVector3f& vec );
protected:
	void			DoCtrl( CtrlFunHandle* pFun,void* param );
	void			_UpdateClipStatus(ClipStatus::Type status,CCamera* pCam);
	void			_Render(uint32 CurTime);
	void			_unRender(void);
	CQuadNode*		m_childTree[2][2];	
};

//------------------------------------------------------------------------------
class CQuadTreeNode : public CQuadNode
{
public:
	CQuadTreeNode(CElementManager* pParent , URegionID id, CQuadTreeSpace* pQuadParent
		, float MinX, float MinY, float MaxX,float MaxY );
public:
	void		InsertNode(CRenderNode* pObj);
	bool		DeleteNode(CRenderNode* pObj);
	bool		Intersects( const CVector3f& vec );
protected:
	void		DoCtrl( CtrlFunHandle* pFun,void* param );
	void		_UpdateClipStatus(ClipStatus::Type status,CCamera* pCam);
	void		_Render(uint32 CurTime);
	void		_unRender(void);
	typedef		SList<CRenderNode*> NodeList;
	NodeList	m_List;
	bool		m_IsRendering;
};

}
