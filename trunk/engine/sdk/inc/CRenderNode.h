#pragma once
#include "CRenderable.h"
#include "CRenderSpaceNode.h"
#include "CCtrlNode.h"

namespace sqr
{
	class CRenderNode 
		: public CRenderable
		, public CCtrlNode
	{
	private:
				const AniResult	Render(  const CMatrix& matWorld, RenderObjStyle* pRORS = NULL );
	protected:
		virtual	void			SetFixMatrix(const CMatrix& parent, const CMatrix& fix = CMatrix::UNIT_MATRIX){};
		virtual	const AniResult Render( uint32 uCurTime, RenderObjStyle* pRORS = NULL ) = 0;	//äÖÈ¾Ä£ÐÍ	
		virtual void			SetParentMatrix( const CMatrix& mat4 );
	public:
		CRenderNode(ILogicHandler* pHandler = NULL):CRenderable(pHandler){};
		virtual ~CRenderNode(){};
		virtual void			SetVisible( bool isVisible );
				void			SetVisible( CCamera* pCamera );
				bool			IsVisible() const { return m_isVisible; }	
	
		virtual const			CAxisAlignedBox	 GetAABB();
		virtual IntersectFlag	IntersectRay(const CVector3f& rayPos, const CVector3f& rayDir);

		virtual void			UpdateBox(void);
				void			_UpdateEx(void) const;
		//#ifdef _DEBUG
		//bool ShowBox;
		//virtual void			RenderDebugLine(void);
				void			Render( void ) {};
		//#endif
	public:

		bool IsValid(void)	const	{ return true; };
		//void OnPreRender( const AniResult& ParentAniRes, float FrameRatio ){};
		void OnPreRender( uint32 uCurTime,CCamera* pCamera ) {};
		void SetRenderVelocity( float Velocity , int32 id = -1 ) {};
		bool IsValid(void) { return true; }
	protected:
		CMatrix						m_matParent;
		CMatrix						m_CenterMatrix;
	};
}