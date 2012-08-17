#include "stdafx.h"
#include "PortraitView\CPortraitView.h"
#include "BaseHelper.h"
#include "CVertexDeclaration.h"
#include "PortraitView\PortraitViewCtrlImp.h"
#include "CRenderPipeline.h"
#define XMLPortraitView "PortraitView"

CPortraitView::CPortraitView( CWindowTarget* pTarget )
	: m_pRenderModel(NULL)
	, m_pParentTarget(pTarget)
	, m_bVisible(true)
	, m_bFullScreen(false)
	, m_bShowFrame(false)
	//, m_szConfigType("PortraitView")
	
{
	m_pParentTarget->InsertRenderer(this);
	m_pViewPort = m_pParentTarget->AddViewport();
	m_pRenderGroup = new CRenderGroup;
	m_pViewPort->SetRenderGroup(m_pRenderGroup);
	m_pRenderGroup->EnableFog(false);
	CCamera* pCamera = m_pRenderGroup->GetCamera();
	pCamera->SetPosition(CVector3f( 0.0f, 100.0f, -600.0f ));
	pCamera->lookAt( CVector3f( 0, 0, 1 ));
	pCamera->setFOVy(30.0f*CMath::pi/180.0f);
	RenderQueueFlag SceneFlag;
	SceneFlag<<ID_NPC_FILTER<<ID_ALPHA_NPC_FILTER;
	m_pRenderGroup->SetRenderQueueFlag(SceneFlag);
	m_pRenderGroup->SetFogColor(0xFF000000);

	m_pRenderGroup->GetLightSystem()->SetGlobalLightDir( CVector3f(-0.420000f,-0.495984f,-0.760000f) );
	m_pRenderGroup->GetLightSystem()->SetAmbient(0xFFC8C8C8,1);
	m_pRenderGroup->GetLightSystem()->SetGlobalLightColor(0xFF464646,1);

	m_Rp.m_Mat.Identity();
	m_Rp.m_RS.m_AlphaBlendEnable	= TRUE;
	m_Rp.m_RS.m_ZTestEnable			= FALSE;
	m_Rp.m_RS.m_AlpOP_S0			= TOP_SELECTARG1;
	m_Rp.m_RS.m_TexOP_S0			= TOP_SELECTARG1;
	m_Rp.m_RS.m_AlpOP_S1			= TOP_DISABLE;
	m_Rp.m_RS.m_TexOP_S1			= TOP_DISABLE;
	m_Rp.m_RS.m_TexArg1_S0			= TA_TEXTURE;
	m_Rp.m_RS.m_AlpArg1_S0			= TA_TEXTURE;

	m_Op.useIndexes = false;
	m_Op.vertexData = new VertexData();
	m_Op.vertexData->vertexBufferBinding = new CVertexBufferBinding();

	CVertexDeclaration* pDecl = CVertexDeclaration::GET_VERDECL(VerText2D::Format);
	CVertexHardwareBuffer* _pVertexHBuffer	= CHardwareBufferMgr::GetInst()->createVertexBuffer(
		sizeof(VerText2D), 4 ,HBU_WRITE_ONLY);

	m_Op.vertexData->vertexDeclaration = pDecl;
	m_Op.vertexData->vertexBufferBinding->SetBinding(0,_pVertexHBuffer);
	m_Op.vertexData->vertexDeclaration = CVertexDeclaration::GET_VERDECL(VerText2D::Format);
	m_Op.vertexData->vertexStart = 0;
	m_Op.vertexData->vertexCount = 4;
	m_Op.operationType = PT_TRIANGLESTRIP;

	SetRect(CIRect(0,0,150,150));
	Initialize();
}

CPortraitView::~CPortraitView()
{
	m_Op.vertexData->vertexBufferBinding->DestroyAllBuffer();
	delete m_Op.vertexData->vertexBufferBinding;
	delete m_Op.vertexData;
	SafeRelease(m_Rp.m_RS.m_Texture_S0);
	SafeRelease(m_pViewPort);
	m_pParentTarget->DeleteRenderer(this);
}

void CPortraitView::Render()
{
	if( m_pRenderModel == NULL || !m_bVisible )
		return;

	PortraitInfo* pInfo = m_pRenderModel->GetPortraitInfo();
	CCamera* pCamera = m_pRenderGroup->GetCamera();
	pCamera->SetPosition(pInfo->cameraPos - pInfo->translate);
	pCamera->lookAt(-pInfo->translate);
	pCamera->setFOVy(pInfo->fov);

	sMaterial	temp;
	temp.Diffuse.a = 1.0f;

	temp.Diffuse.r = MATAMBIENT;
	temp.Diffuse.g = MATAMBIENT;
	temp.Diffuse.b = MATAMBIENT;

	temp.Ambient.a = 1.0f;
	temp.Ambient.r = MATAMBIENT;
	temp.Ambient.g = MATAMBIENT;
	temp.Ambient.b = MATAMBIENT;

	RenderObjStyle ros =
	{
		ROS_LIGHTMAT2,
		TRUE,
		TRUE,
		DELTARS_DIFFUSE|DELTARS_AMBIENT,
		(uint32)&temp
	};

	m_pRenderModel->ClearModelRender();
	CGraphic::GetInst()->ActiveRenderTarget(m_pViewPort);
	m_pRenderModel->RenderFrame(CMatrix::UNIT_MATRIX, m_pRenderModel->GetCurFrameNum(),&ros);
	CGraphic::GetInst()->ActiveRenderTarget(NULL);	
	//if( m_bFullScreen )
	//m_pViewPort->

	m_pViewPort->Render();
	if(m_bShowFrame)
		CRenderPipeline::GetInst()->Render(&m_Op,&m_Rp);
	m_pParentTarget->UpdateViewport();
	m_pRenderModel->ClearModelRender();
}

void CPortraitView::SetRect(CIRect& Rect)
{
	m_PortraitRect = Rect;
	UpdateRect();
}

void CPortraitView::SetVisible(bool bVis)
{
	m_bVisible = bVis;
}

void CPortraitView::SetFullScreen(bool bScreen)
{
	m_bFullScreen = bScreen;
	UpdateRect();	
}

void CPortraitView::UpdateRect(void)
{
	CIRect tRect;
	if(m_bFullScreen)
		m_pParentTarget->GetRect(tRect);
	else
		tRect = m_PortraitRect;

	m_pViewPort->SetRect( tRect );
	

	CVertexHardwareBuffer* _pVertexHBuffer =m_Op.vertexData->vertexBufferBinding->GetBuffer(0);
	VerText2D *vertex = (VerText2D*)_pVertexHBuffer->lock(HBL_DISCARD);

	float x1 = (float)tRect.left;
	float y1 = (float)tRect.top;
	float x2 = (float)tRect.right;
	float y2 = (float)tRect.bottom;

	float u1 = 0.0f;
	float v1 = 0.0f;
	float u2 = 1.0f;
	float v2 = 1.0f;

	vertex[0] = VerText2D( x1, y2, MAX_DEPTH,  u1, v2 ); //对点写数据
	vertex[1] = VerText2D( x1, y1, MAX_DEPTH,  u1, v1 );
	vertex[2] = VerText2D( x2, y2, MAX_DEPTH,  u2, v2 );
	vertex[3] = VerText2D( x2, y1, MAX_DEPTH,  u2, v1 );

	_pVertexHBuffer->unlock();

}

void CPortraitView::SetModel( CEditModel* pModel )
{
	m_pRenderModel = pModel;
}

bool CPortraitView::Initialize(void)
{
	TiXmlNode* pConfigXml = CEditorConfig::GetInst()->GetEditorConfig(XMLPortraitView);
	TiXmlElement* pElement = pConfigXml->ToElement();

	int	iShow		 = 0;
	int	iShowFrame	 = 0;

	pElement->QueryIntAttribute("Show",&iShow);
	TiXmlNode* pNode = NULL;
	pNode = pConfigXml->FirstChild("PhotoFrame");
	if( NULL!=pNode )
	{
		pElement = pNode->ToElement();
		pElement->QueryIntAttribute("ShowFrame",&iShowFrame);

		pNode = pNode->FirstChild("Texture");
		ITexture* pText = NULL;
		if(NULL!=pNode)
		{
			pElement  = pNode->ToElement();
			if( NULL!=pElement->Attribute("Path") )
			{
				m_strPhotoFrame = pElement->Attribute("Path") ;
				CGraphic::GetInst()->CreateTexture( L"", m_strPhotoFrame.c_str(), &pText );
			}
		}

		m_bShowFrame = iShowFrame && pText!=NULL;
		m_bVisible	 = iShow == 1;
		if( m_bShowFrame )
		{	
			SafeRelease(m_Rp.m_RS.m_Texture_S0);			
			m_Rp.m_RS.m_Texture_S0 = pText;
		}
	}

	return true;
}

void CPortraitView::ShowSettingDlg(void)
{
	/*if( IDOK == Config.DoModal() )
	{
		TiXmlNode* pConfigXml = CEditorConfig::GetInst()->GetEditorConfig(XMLPortraitView);
		pConfigXml->Clear();
		TiXmlElement* pElement = pConfigXml->ToElement();
		pElement->SetAttribute("Show",m_bVisible?1:0);

		TiXmlElement PhotoFrameXml("PhotoFrame");
		{
			PhotoFrameXml.SetAttribute("ShowFrame",m_bShowFrame?1:0);
			TiXmlElement tTexture("Texture");
			tTexture.SetAttribute("Path",m_strPhotoFrame.c_str());
			PhotoFrameXml.InsertEndChild(tTexture);
		}
		pConfigXml->InsertEndChild(PhotoFrameXml);
		CEditorConfig::GetInst()->SaveEditorConfig();
	}*/
	Initialize();
}
