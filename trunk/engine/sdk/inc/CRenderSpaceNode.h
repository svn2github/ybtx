#ifndef _CRender_Child_H_
#define _CRender_Child_H_
#include "CDynamicObject.h"
#include "CVector3.h"
#include "CColor.h"
#include "GraphicBase.h"
#include "CRenderSpace.h"
#include "ISpaceNodeHandler.h"

namespace sqr
{
	class CRenderSpaceNode : public virtual CDynamicObject
	{
		friend class		CRenderSpace;
		ISpaceNodeHandler*	m_pNodeHandler;
	protected:
		CRenderSpace*		m_pParentSpace;
	public:
		static const DWORD st_NodeType;

		uint32				m_uLinkID;
		eLinkType			m_eLinkType;
		string				m_AttachName;
	public:
		CRenderSpaceNode();
		virtual ~CRenderSpaceNode();

		virtual void Activate(){}
		virtual IntersectFlag	IntersectRay( const CVector3f& rayPos, const CVector3f& rayDir ) {	return IF_IMPOSSIBLE; };
		virtual uint8			GetAlphaValue() { return 0;	};			
		
		virtual void	SetAlphaValue( uint8 uAlpha ) {};
		virtual void	SetDiffuse( CColor uColor ) {}
		virtual void	SetAmbient( CColor uColor ) {};
		virtual void	Do( const char* Action, bool bLoop, uint32 DelayTime, int16 nStartFrame, int16 nEndFrame ) {};
		virtual void	Update(void) {};

		//OnPreRenderEndÒª¿¼ÂÇÉ¾³ý
		virtual void	OnPreRenderEnd(){};

		virtual void	OnPreRender( uint32 uCurTime,CCamera* pCamera ) {};
		virtual void	Render(void){};
		virtual void	RenderSel( uint32 uCurTime, RenderObjStyle* pRS ) {};
				void	SetNodeHandler(ISpaceNodeHandler* pHandler);

		virtual DWORD	GetNodeType();
		virtual bool	UpdateTime(uint32 uCurTime);
	};
	
	inline void CRenderSpaceNode::SetNodeHandler(ISpaceNodeHandler* pHandler)
	{
		m_pNodeHandler = pHandler;
	}

}

#endif //_CScene_Objcet_H_
