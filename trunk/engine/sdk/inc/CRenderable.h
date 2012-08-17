#pragma once
#include "GraphicRes.h"
#include "ILogicalHandler.h"
#include "IGraphic.h"
namespace sqr
{
	/// 可绘制物体
	class CRenderable 
		: public GraphicRes
	{
	public:
		//virtual void ClearLink( void ) = 0;
		//virtual int  AddLinkObj( CRenderable* MObj, const char* szAttachName, bool bFrameSynch ) = 0;
		//virtual void DeleteLinkObj( const char* szAttachName) = 0;
		//virtual int  RefLinkObj( const char* szAttachName, CRenderable** ppRenderable ) = 0;
		//virtual void OnLinked( CRenderable* pParentObj, const char* szAttachName,  bool bFrameSynch ) = 0;
		//virtual void OnUnlinked( CRenderable* pParentObj, const char* szAttachName ) = 0;
		//virtual void OnPreRender( const AniResult& ParentAniRes, float FrameRatio ) = 0;	

		virtual const AniResult Render(  const CMatrix& matWorld, RenderObjStyle* pRORS = NULL ) = 0; 
		virtual bool UpdateTime(uint32 uCurTime) = 0;
	
	public:
				CRenderable(ILogicHandler* pHandler = NULL);
		virtual ~CRenderable(void);

		virtual ERenderObjType			GetType();
		virtual const CAxisAlignedBox	GetAABB();
		virtual void					SetVisible( bool isVisible );
		inline	bool					IsVisible() const;
		inline	ILogicHandler*			GetLogicHandler(void);

	protected:
		
		CAxisAlignedBox		m_OrgAABB;
		bool				m_isVisible;
		ILogicHandler*		m_pLogicHandler;
	};

	inline	ILogicHandler* CRenderable::GetLogicHandler()
	{
		return m_pLogicHandler;
	}

	inline	bool	CRenderable::IsVisible() const
	{
		return m_isVisible;
	}

	inline ERenderObjType CRenderable::GetType()
	{
		return ROT_UNKNOW;
	}

}