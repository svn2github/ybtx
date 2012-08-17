#include "stdafx.h"
#include "CRenderDialog.h"
#include "SQRRootWnd.h"
#include "CCamera.h"
#include "MathAlgorithm.h"

DYNAMIC_CREATE_WND( CRenderDialog, SQRDialog )

const float CRenderDialog::SCALE_BY_Z = 50;

CRenderDialog::CRenderDialog()
: m_xPosition(0)
, m_yPosition(0)
, m_uPreTime(0)
, m_bUseSceneDepth(false)
, m_LastWnd(NULL)
, m_bShow(false)
, m_bToflush(false)
, m_bInit(false)
, m_NeedUpdate(E_UP_STATE_NONE)
, m_uLinkIDBak(0xfffffffe)
{
	m_pCamera = CMainWindowTarget::GetInst()->GetCamera();
}

CRenderDialog::~CRenderDialog()
{
}

uint8 CRenderDialog::GetAlphaValue()
{
	return (uint8)( SQRWnd::GetTransparent()*255 );
}

void CRenderDialog::SetAlphaValue( uint8 uAlpha )
{
	SQRWnd::SetTransparent( uAlpha/255.0f );
}

void CRenderDialog::DrawWndBackground()
{
	if(!m_bInit || !(GetWndHandle()->m_bShow))
		return;

	if (m_bToflush)
		FlushDraw();

	WND_IMAGE_LIST& ImageList = IsEnable() ? GetWndHandle()->m_Enable : GetWndHandle()->m_Disable;
	CFRect rtWnd( 0, 0, (float)GetWndWidth(), (float)GetWndHeight() );
	for( int32 i = 0; i < ImageList.GetImageCount(); i++ )
	{
		IMAGE& image = ImageList.GetImage(i);
		if (NULL != image.pTexture)
		{
			CFRect rtImage(0.0f, 0.0f, (float)image.pTexture->GetWidth(), (float)image.pTexture->GetHeight()); 
			DrawRect(image.pTexture, rtWnd, image.dwColor, &rtImage, false);
		}
	}
}

int32 CRenderDialog::CreateFromRes( const char* szWindowNameInResFile, SQRWnd* ParentWnd )
{
	int32 rt = SQRDialog::CreateFromResEx(szWindowNameInResFile, ParentWnd, true);
	m_bToflush = CheckChildFlush(this);
	return rt;
}

bool  CRenderDialog::CheckChildFlush(SQRWnd* CurWnd)
{
	if ( CurWnd->GetStyleIndex() != 0 )
		return true;
	else
	{
		WND_IMAGE_LIST& ImageList = CurWnd->GetWndHandle()->m_Enable;
		for (int32 i = 0; i < ImageList.GetImageCount(); i++)
		{
			IMAGE& image = ImageList.GetImage(i);
			if ( image.texName.size() != 0)
			{
				return true;
			}
		}
	}

	SQRWnd* ChildWnd = CurWnd->GetWndHandle()->m_LastChild;
	while ( ChildWnd )
	{
		if ( CheckChildFlush(ChildWnd) )
		{
			return true;
		}
		ChildWnd = ChildWnd->GetWndHandle()->m_PreBrother;
	}

	return false;
}

void CRenderDialog::DrawAfterChildren()
{
	if (m_bToflush)
		FlushDraw();
}

void CRenderDialog::SetDepthAndRS( float fDepth, ERenderStyle eRS, SQRWnd& Wnd )
{
	Wnd.SetDepth( fDepth );
	Wnd.SetRenderStyle( eRS );
	SQRWnd* pChildWnd = Wnd.FindLastChild();
	while( pChildWnd )
	{
		SetDepthAndRS( fDepth, eRS, *pChildWnd );
		pChildWnd = pChildWnd->FindPreWnd();
	}
}

void CRenderDialog::ShowWnd( bool bShow )
{
	SQRDialog::ShowWnd(bShow);
	m_bShow = GetWndHandle()->m_bShow;
	GetWndHandle()->m_bShow = false;
	m_bInit = false;

	SetAlignToPixel(m_bAlignToPixel);	
	SQRWnd* pChildWnd = FindFirstChild();
	while( pChildWnd )
	{
		pChildWnd->SetAlignToPixel(m_bAlignToPixel);
		pChildWnd = pChildWnd->FindNextWnd();
	}
}

void CRenderDialog::SetUseSceneDepth( bool bUseSceneDepth )
{
	if( !bUseSceneDepth )
		SetDepthAndRS( 0.0f, RS_GUI, *this );
	m_bUseSceneDepth = bUseSceneDepth;
}

void CRenderDialog::SetRenderWndHeight(int32 height )
{
	m_yPosition = height / 2;
}

void CRenderDialog::SetOff( int16 x, int16 y )
{
	m_xPosition = x / 2;// * (int32)SCALE_BY_Z;
	m_yPosition = y / 2;// * (int32)SCALE_BY_Z;
}

void CRenderDialog::OnPreRenderEnd()
{
	if ( m_pParentSpace->GetWillRender() )
		m_bInit = true;
	else
	{
		m_bInit = false;
		GetWndHandle()->m_bShow = false;
	}
}

void CRenderDialog::Update(void)
{
	m_NeedUpdate = EUpDateState(E_UP_STATE_COUNT - 1);
}

bool CRenderDialog::UpdateTime(uint32 uCurTime)
{
	Render();
	return true;
}

void CRenderDialog::Render(void)
{ 
	if( !IsCreated())
		return;

	if( m_bShow )
	{
		if( !m_bInit )
		{
			GetWndHandle()->m_bShow = false;
			return;
		}
		else
			GetWndHandle()->m_bShow = true;

		

		CVector3f	vecPos;
		CVector2f	vecHW;		// 根据绑定点深度缩放后的屏幕空间控件尺寸

		CFRect		rt_tmp;
		GetRootWnd()->GetWndRect( rt_tmp );
		CIRect		rt((RECT)rt_tmp);

		CMatrix		matParent;
		CMatrix		matHead;

		m_pParentSpace->GetAttachMatrix( matParent, -1 );

		if( m_NeedUpdate != E_UP_STATE_NONE)
		{
			CMatrix	Temp = matParent;
			Temp.Invert();

			m_pParentSpace->GetAttachMatrix( m_Pos, m_uLinkID );
			m_Pos *= Temp; 
		}

		matHead = m_Pos * matParent;
		CVector3f Pos( matParent._41, matHead._42, matParent._43 );
		vecPos = SceneToScreen( rt, Pos, m_pCamera->getViewProjectMatrix() );

		float fScaleRate = (1.0f - vecPos.z) * SCALE_BY_Z;
		float fMoveRate = fScaleRate;

		if ( GetWndHandle()->m_uLockFlage != LOCK_ALL || vecPos.z == 1.0f )
			fScaleRate = 1.0f;
		else
			fScaleRate = sqrt(fScaleRate);
		SetWndZoom( fScaleRate );

		vecHW.x = GetWndHandle()->m_WidthInitial * fScaleRate;
		vecHW.y = GetWndHandle()->m_HeightInitial * fScaleRate;		

		float x = rt.left + vecPos.x - vecHW.x * 0.5f	- m_xPosition * fMoveRate - m_xPosition * 0.4f; 
		float y = rt.top  + vecPos.y - vecHW.y			- m_yPosition * fMoveRate - m_yPosition * 0.4f;


		if( m_bUseSceneDepth )
			SetDepthAndRS( vecPos.z, RS_PONITSAMPLE, *this );

		CFRect rtWnd;
		GetWndRect( rtWnd );
		if( abs(rtWnd.left - x) >= 0.25f || abs(rtWnd.top - y) >= 0.25f )
		{
			rtWnd.left		= x;
			rtWnd.top		= y;
			rtWnd.right		= x + vecHW.x;
			rtWnd.bottom	= y + vecHW.y;

			SQR_TRY
			{
				if (fScaleRate == 1.0f && !m_bUseSceneDepth && m_NeedUpdate == E_UP_STATE_NONE)
					ResetWndPos(x, y);
				else
				{
					ResizeRect( rtWnd );
					if (m_NeedUpdate > E_UP_STATE_NONE)
						m_NeedUpdate = EUpDateState(m_NeedUpdate - 1);
					else
						m_NeedUpdate = E_UP_STATE_NONE;
				}
			}
			SQR_CATCH(exp)
			{
				stringstream errmsg;
				SQR_TRY
				{
					if (GetWndHandle()->m_FirstChild)
					{
						errmsg << "Head info text:" << GetWndHandle()->m_FirstChild->GetWndText() << endl;
						errmsg << "Head info GUI manager pointer:" << GetWndHandle()->m_FirstChild->GetWndHandle()->m_pGUI << endl;
					}
					errmsg << "GUI inst manager pointer:" <<  GetRootWnd()->GetWndHandle()->m_pGUI << endl;
				}
				SQR_CATCH(exp)
				{
					errmsg << "Head info dirty" << exp.ErrorMsg() << endl;
				}
				SQR_TRY_END;

				exp.AppendMsg(errmsg.str());
				GfkLogExp(exp);
			}
			SQR_TRY_END;
		}

		if(GetWndHandle()->m_ParentWnd->GetWndHandle()->m_LastChild!=this &&  GetWndHandle()->m_ParentWnd->GetWndHandle()->m_FirstChild !=this)
		{
			GetWndHandle()->m_PreBrother->GetWndHandle()->m_NextBrother = GetWndHandle()->m_NextBrother;

			GetWndHandle()->m_NextBrother->GetWndHandle()->m_PreBrother = GetWndHandle()->m_PreBrother;
			GetWndHandle()->m_ParentWnd->GetWndHandle()->m_LastChild->GetWndHandle()->m_NextBrother = this;

			GetWndHandle()->m_PreBrother=GetWndHandle()->m_ParentWnd->GetWndHandle()->m_LastChild;
			GetWndHandle()->m_NextBrother = NULL;
			GetWndHandle()->m_ParentWnd->GetWndHandle()->m_LastChild = this;
		}
		else if( GetWndHandle()->m_ParentWnd->GetWndHandle()->m_FirstChild == this)
		{
			GetWndHandle()->m_ParentWnd->GetWndHandle()->m_FirstChild = GetWndHandle()->m_NextBrother;
			GetWndHandle()->m_NextBrother->GetWndHandle()->m_PreBrother = NULL;

			GetWndHandle()->m_ParentWnd->GetWndHandle()->m_LastChild->GetWndHandle()->m_NextBrother = this;
			GetWndHandle()->m_PreBrother=GetWndHandle()->m_ParentWnd->GetWndHandle()->m_LastChild;
			GetWndHandle()->m_NextBrother = NULL;
			GetWndHandle()->m_ParentWnd->GetWndHandle()->m_LastChild = this;
		}
	}
}

void CRenderDialog::_ResizeRect( const CFRect& rt, bool refreshbak )
{
	SQRWnd::_ResizeRect( rt, refreshbak );
	if( m_NeedUpdate == E_UP_STATE_NONE )
		m_NeedUpdate = E_UP_STATE_ONEFM;
}


bool CRenderDialog::CheckDif(CFRect& rt, CFRect& dif_rt, float dif)
{
	CFRect difrt = rt - dif_rt;
	float dif_var = difrt.left	 * difrt.left 
		+ difrt.right  * difrt.right 
		+ difrt.top	 * difrt.top 
		+ difrt.bottom * difrt.bottom;
	if (dif_var >= dif)
		return true;
	else
		return false;
}

int32 CRenderDialog::Create( const WndCreateParam& param )
{
	int32 ret = SQRWnd::Create(param);
	GetWndHandle()->m_bShow = false;
	m_bShow					= false;

	return ret;
}


void CRenderDialog::GetAttachMatrix( CMatrix& matLink, uint32 uChildLinkID )
{
	matLink.Identity();
}

IntersectFlag CRenderDialog::IntersectRay( const CVector3f& rayPos, const CVector3f& rayDir )
{
	if( IsCreated() )
	{
		const CFPos& pos = GetRootWnd()->GetCursorPos();
		if( IsInWndArea( pos.x, pos.y ) )
			return IF_POSSIBLE;
	}
	return IF_IMPOSSIBLE;
}

void sqr::CRenderDialog::DrawWndText()
{
	
}
