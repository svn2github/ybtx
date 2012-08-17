#ifndef _CRender_Wnd_H_
#define _CRender_Wnd_H_

#include "CRenderSpaceNode.h"
#include "SQRDialog.h"

namespace sqr
{
class CCamera;
enum EUpDateState
{
	E_UP_STATE_NONE = 0,
	E_UP_STATE_ONEFM = 1,
	E_UP_STATE_TWOFM = 2,
	/*E_UP_STATE_THREEFM = 3,
	E_UP_STATE_FOURFM = 4,
	E_UP_STATE_FIVEFM = 5,
	E_UP_STATE_SIXFM = 6,
	E_UP_STATE_SEVENFM = 7,*/
	E_UP_STATE_COUNT = 3
};

class CRenderDialog
			: public CRenderSpaceNode
			, public SQRDialog
{
	DYNAMIC_DECLARE_WND( CRenderDialog )

protected:
	CCamera			*m_pCamera;
	uint32			m_uPreTime;
	int32			m_xPosition;
	int32			m_yPosition;
	bool			m_bInit;
	bool			m_bShow;
	bool			m_bToflush;
	bool			m_bUseSceneDepth;
	SQRWnd*			m_LastWnd;
	SQRDialog		m_Dialog;
	CFRect			m_rtWnd;
	CMatrix			m_Pos;
	uint32			m_uLinkIDBak;
	EUpDateState	m_NeedUpdate;

	static void SetDepthAndRS( float fDepth, ERenderStyle eRS, SQRWnd& Wnd );

	static const float SCALE_BY_Z;
	static const float CHECK_DIF_Min;
	static const float CHECK_DIF_Max;

public:
	CRenderDialog();
	virtual ~CRenderDialog();

	//Implementing CRenderSpace's virtual function
	void	SetOff( int16 x, int16 y );
	void	SetUseSceneDepth( bool bUseSceneDepth );
	/*void	DrawWnd( uint32 uCurTime, SQRWnd& Wnd );*/
	uint8	GetAlphaValue();
	void	SetRenderWndHeight(int32 height);
	void	SetAlphaValue( uint8 uAlpha );
	void	ShowWnd( bool bShow );
	virtual int32 CreateFromRes( const char* szWindowNameInResFile, SQRWnd* ParentWnd );
	bool	CheckChildFlush(SQRWnd* CurWnd);

	void	GetAttachMatrix( CMatrix& matLink, uint32 uChildLinkID );
	void	OnPreRender( uint32 uCurTime,CCamera* pCamera  ) { m_uPreTime = uCurTime; m_pCamera = pCamera?pCamera:m_pCamera; }
	void	OnPreRenderEnd();
	void	RenderShadow( uint32 uCurTime, const CVector3f& vecPlaneNor, bool zTest = false ) {}

	IntersectFlag	IntersectRay( const CVector3f& rayPos, const CVector3f& rayDir );

	bool	UpdateTime(uint32 uCurTime);
	void	Render(void);

	bool	CheckDif(CFRect& rt, CFRect& dif_rt, float dif);

	int32	Create( const WndCreateParam& param );
	void	DrawWndBackground();
	void	DrawWndText();
	void	DrawAfterChildren();
	virtual void _ResizeRect( const CFRect& rt ,bool refreshbak = true);

	void	Update(void);
};
}
#endif //_CRender_Wnd_H_
