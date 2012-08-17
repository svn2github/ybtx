// TextureEditorView.cpp : CTextureEditorView 类的实现
//

#include "stdafx.h"
#include "TextureEditor.h"

#include "TextureEditorDoc.h"
#include "TextureEditorView.h"
#include ".\textureeditorview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTextureEditorView

IMPLEMENT_DYNCREATE(CTextureEditorView, CView)

BEGIN_MESSAGE_MAP(CTextureEditorView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CTextureEditorView 构造/析构

CTextureEditorView::CTextureEditorView()
{
	// TODO: 在此处添加构造代码
	m_IniP=CPoint(-1,-1);
}

CTextureEditorView::~CTextureEditorView()
{
}

BOOL CTextureEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或
	// 样式

	return CView::PreCreateWindow(cs);
} 

LRESULT CTextureEditorView::ReleaseRes()
{
    return S_OK;
}

LRESULT CTextureEditorView::RestoreRes()
{	
    // Create the the vertex buffer
    if( FAILED( m_d3d.GetDevice()->CreateVertexBuffer( 4 * sizeof(Vertex2D),
		D3DUSAGE_DYNAMIC , VerText2D::Format, D3DPOOL_DEFAULT, &m_pVB, NULL ) ) )
		return E_FAIL;

	Vertex2D* pVertices;
    if( FAILED( m_pVB->Lock( 0, sizeof(Vertex2D), (void**)&pVertices, 0 ) ) )
        return E_FAIL;
    pVertices[0]=Vertex2D(  0,256,0,0xffffffff,0,1);
    pVertices[1]=Vertex2D(  0,  0,0,0xffffffff,0,0);
    pVertices[2]=Vertex2D(256,256,0,0xffffffff,1,1);
    pVertices[3]=Vertex2D(256,  0,0,0xffffffff,1,0);
    m_pVB->Unlock();

    return S_OK;
}

// CTextureEditorView 消息处理程序
int CTextureEditorView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_Menu.LoadMenu(IDR_MENU1);

	RECT rect={0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN)};
	m_show.Create("EDIT","",WS_CHILD,rect,this,1000);
	m_show.ShowWindow(SW_SHOW);
	m_show.EnableWindow(0);
	m_Start=CPoint(150,150);

	if(FAILED(m_d3d.CreateD3d(m_show.m_hWnd,this,0,0,D3DFMT_R5G6B5,D3DDEVTYPE_HAL))||m_d3d.GetTextFormat()!=D3DFMT_DXT1)
		return -1;

/*	m_d3d.SetTextAlpha();
	m_d3d.SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_POINT);*/

	GetDocument()->SetD3d(&m_d3d);

	SetTimer( 0, 20, NULL );
	return 0;
}

// CTextureEditorView 绘制
void CTextureEditorView::Draw()
{
	CTextureEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	
	if(SUCCEEDED(m_d3d.RenderBegin()))
	{
		CPoint p=pDoc->SetTexture();
		Vertex2D* pVertices;
		float x=(float)m_Start.x;
		float y=(float)m_Start.y;
		float w=(float)p.x*pDoc->GetScale()/2;
		float h=(float)p.y*pDoc->GetScale()/2;
		float dx=0.25f/max(w,1); 
		float dy=0.25f/max(h,1); 
		if( SUCCEEDED( m_pVB->Lock( 0, sizeof(Vertex2D), (void**)&pVertices, 0 ) ) )
		{
			pVertices[0]=Vertex2D(x-w,y+h,0,0xffffffff,  dx,1+dy);
			pVertices[1]=Vertex2D(x-w,y-h,0,0xffffffff,  dx,  dy);
			pVertices[2]=Vertex2D(x+w,y+h,0,0xffffffff,1+dx,1+dy);
			pVertices[3]=Vertex2D(x+w,y-h,0,0xffffffff,1+dx,  dy);
			m_pVB->Unlock();
		}

		m_d3d.SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );
		m_d3d.SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		m_d3d.SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		m_d3d.SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		m_d3d.SetRenderState( D3DRS_ALPHAREF, 1 );
		m_d3d.SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER );
		m_d3d.SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
		m_d3d.SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );  
		m_d3d.SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ); 
		m_d3d.SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE ); 

		m_d3d.SetStreamSource( 0, m_pVB, 0, sizeof(Vertex2D) );
		m_d3d.SetFVF( Vertex2D::Format );
		m_d3d.DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
		m_d3d.SetTexture(0, NULL);
		
		m_d3d.RenderEnd();
	}
}

void CTextureEditorView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CPoint p;
	GetCursorPos(&p);
	m_show.ScreenToClient(&p);
	if(m_IniP.x<0)
		m_IniP=p;
	CRect rect;
	m_show.GetClientRect(&rect);
	if(nFlags&MK_LBUTTON&&rect.PtInRect(p))
		m_Start+=p-m_IniP;
	m_IniP=p;

	__super::OnMouseMove(nFlags, point);
}

BOOL CTextureEditorView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	CTextureEditorDoc* pDoc = GetDocument();
	if(zDelta>0)
		pDoc->SetScale( pDoc->GetScale()*1.1f );
	if(zDelta<0)
		pDoc->SetScale( pDoc->GetScale()*0.9f );

	return __super::OnMouseWheel(nFlags, zDelta, pt);
}

void CTextureEditorView::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	Draw( );
	__super::OnTimer(nIDEvent);
}
