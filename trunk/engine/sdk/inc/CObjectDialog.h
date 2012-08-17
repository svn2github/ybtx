#pragma once

#include "CRenderSpace.h"
#include "SQRDialog.h"
#include "CGUICameraCtrller.h"

namespace sqr
{
class CRenderTarget;
class CRenderObject;
struct PortraitInfo;

class CObjectDialog
			: public CRenderSpace
			, public SQRDialog
{
	DYNAMIC_DECLARE_WND( CObjectDialog )
public:
	CObjectDialog();
	virtual ~CObjectDialog();

	CRenderObject*	CreateRenderObject();
	void					DestroyRenderObject(CRenderObject* pObj);
	static void SetDepthAndRS( float fDepth, ERenderStyle eRS, SQRWnd& Wnd );
	void  ResizeRect( const CFRect& rt,bool refreshbak = true );
	void  SetWndRect( const CFRect& rt );
protected:
	virtual void	_ResizeRect( const CFRect& rt ,bool refreshbak = true);
	virtual void	_SetWndRect( const CFRect& rt );
public:
	int32	Create( const WndCreateParam& param );
	void	AddChild( CRenderSpaceNode* pChild, eLinkType linkType, const char* szAttachName );
	void	SetViewPortrait( bool bViewP, const char* BackTexture = NULL ,const char* sFramePhoto = NULL );
	void	SwitchCamState();

	//Implementing CRenderSpace's virtual function
	void	SetUseSceneDepth( bool bUseSceneDepth );
	uint8	GetAlphaValue();
	void	SetAlphaValue( uint8 uAlpha );

	void	GetAttachMatrix( CMatrix& matLink, uint32 uChildLinkID );
	void	OnPreRender( uint32 uCurTime ) {}
	void	RenderShadow( uint32 uCurTime, const CVector3f& vecPlaneNor, bool zTest = false ) {}
	void	Render( CCamera* pCamera, uint32 uCurTime );
	//IntersectFlag	CanBeSelected( const CVector3f& rayPos, const CVector3f& rayDir );
	void	OnScreenPosChanged();
protected:
	//Implementing SQRWnd's virtual function
	void	_DestoryRenderChild(CRenderSpaceNode* pObj);
	void	OnParentWndMoved( float /*dx*/, float /*dy*/ );
	void	OnWndMoved( float /*dx*/, float /*dy*/ );
	void	DrawWndBackground();
	void	DrawWndText() {}
	void	RefreshRect();
	void	InitCameraInfo();
protected:
	static	const uint PORTRAIT_SIZE;// = 128;// 再小某些显卡会出现花屏了
private:
	CRenderTarget	*m_pRenderTarget;
	CRenderGroup	*m_pRenderGroup;
	PortraitInfo*	m_pPortraitInfo;
	CGUICameraCtrller	m_GUICamCtrl;
	CCameraController*	m_pActiveCamCtrl;
	ITexture*			m_pTexAlpha;
	bool				m_bViewPortrait;
};
}
