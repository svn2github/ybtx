#include "stdafx.h"
#include "CBigMap.h"
#include "SQRRootWnd.h"
#include "StringHelper.h"
#include "pkgstream.h"
#include "CXmlConfig.inl"

DYNAMIC_CREATE_WND( CBigMap, SQRWnd )

CBigMap::CBigMap()
:m_pAlphaTexture(NULL)
,m_MapTextureOpen(NULL)
,m_MapTextureClose(NULL)
,m_pWnd(NULL)
{
	m_AlphaTexNameList.clear();
	m_MapRegionAlphaTex.clear();
	m_bOpen = 0;
	m_bClose = 0;
	SafeRelease(m_pAlphaTexture);
}

CBigMap::~CBigMap(void)
{
	ClearUp();
	SafeRelease(m_pAlphaTexture);
}

int32 CBigMap::DestroyWnd()
{
	ClearUp();
	AdvDelRef(m_pWnd);
	return SQRWnd::DestroyWnd();
}

void CBigMap::ClearUp()
{
	m_AlphaTexNameList.clear();	
	UIMap<UIString,ITexture*>::iterator iter;
	if(!m_MapRegionAlphaTex.empty())
	{
		iter = m_MapRegionAlphaTex.begin();
		while(iter!=m_MapRegionAlphaTex.end())
		{
			SafeRelease(iter->second);
			iter++;
		}
	}
	m_MapRegionAlphaTex.clear();
	m_GameRegionToFileName.clear();
	SafeRelease(m_MapTextureOpen);
	SafeRelease(m_MapTextureClose);
	SafeRelease(m_MapTextureOpen);
	m_bOpen = 0;
	m_bClose = 0;
}


void CBigMap::DrawWndBackground()
{
	if(!m_bOpen)
		return;

	DrawBigMap();
}

void CBigMap::DrawBigMap()
{
	CFRect rtWnd = CFRect( 0, 0, (float)GetWndWidth(), (float)GetWndHeight() );

	CColor uColor = CColor::White;
	CVector2f vStart( (float)GetWndHandle()->m_xScreen, (float)GetWndHandle()->m_yScreen );
	CVector2f rtScreen[4] = { vStart
		, vStart + CVector2f((float)GetWndHandle()->m_Width,0.0f)
		, vStart + CVector2f(0.0f,(float)GetWndHandle()->m_Height)
		, vStart + CVector2f((float)GetWndHandle()->m_Width,(float)GetWndHandle()->m_Height)};

	float Zoom = GetRootWnd()->GetZoomSize();

	IMAGE& image = GetWndHandle()->m_Enable.GetImage(0);
	
	CFRect rtImage((float)image.posWnd.x, (float)image.posWnd.y, image.posWnd.x + abs(image.rtTexture.Width()), image.posWnd.y + abs(image.rtTexture.Height()));
	rtImage.top		*= Zoom;
	rtImage.left	*= Zoom;
	rtImage.bottom	*= Zoom;
	rtImage.right	*= Zoom;
	CFRect rtDraw = rtWnd.Intersection(rtImage).Offset(0.0f,0.0f);
	CFRect rtText(
		image.rtTexture.left,
		image.rtTexture.top,
		( rtImage.right  - rtImage.left )*image.rtTexture.Width() /rtImage.Width()  + image.rtTexture.left,
		( rtImage.bottom -  rtImage.top )*image.rtTexture.Height()/rtImage.Height() + image.rtTexture.top);

	if(m_bOpen&&!m_bClose)
		DrawRect(m_MapTextureOpen, rtDraw, uColor, &rtText,false);
	else
		DrawRect( rtScreen, uColor,m_pAlphaTexture,m_MapTextureOpen,m_MapTextureClose/*,&rtText,m_BackTexUV*/, RS_GUI_BIGMAP, GetWndHandle()->m_fZ );	
}

bool CBigMap::AddAreaFrog(const TCHAR* AreaName)
{
	if(m_GameRegionToFileName.find(AreaName)!=m_GameRegionToFileName.end())
	{
		UIString name = m_GameRegionToFileName[AreaName];
		UIList<UIString>::iterator iter;
		iter = find(m_AlphaTexNameList.begin(),m_AlphaTexNameList.end(),name);
		if(iter==m_AlphaTexNameList.end())
			m_AlphaTexNameList.push_back(name);
		return true;
	}
	else
		return false;
}

bool CBigMap::RemoveAreaFrog(const TCHAR* AreaName)
{
	if(m_GameRegionToFileName.find(AreaName)!=m_GameRegionToFileName.end())
	{
		UIString name = m_GameRegionToFileName[AreaName];
		UIList<UIString>::iterator iter;
		for(iter = m_AlphaTexNameList.begin();iter!=m_AlphaTexNameList.end();)
		{
			if(*iter==name)
				iter = m_AlphaTexNameList.erase(iter);
			else
				iter++;
		}
		return true;
	}
	else
		return false;
}

void CBigMap::AlphaTexsRenderToTexture()
{
	IGraphic* pGraphic = GetGraphic();
	VerText2D* pVB;
	RenderState* pRS;
	UIMap<UIString,ITexture*>::iterator iter;
	UIList<UIString>::iterator texNameIter;
	texNameIter = m_AlphaTexNameList.begin();

	float MinUV = 0.5f / m_pAlphaTexture->GetWidth();
	float MaxUV = 1.0f - MinUV;
	string FileName="";

	

	while(texNameIter!=m_AlphaTexNameList.end())
	{
		UIString strAlphaTex = *texNameIter;
		
		//动态创建一张alpha纹理
		format(FileName,"%s/BigMap/%s.dds",m_SceneName.c_str(),strAlphaTex.c_str());
		pGraphic->CreateTexture( PATH_ALIAS_RES.c_str(), FileName.c_str(), &(m_MapRegionAlphaTex[strAlphaTex]) );

		iter = m_MapRegionAlphaTex.find(strAlphaTex);
		if(iter!=m_MapRegionAlphaTex.end())
		{
			pGraphic->GetRenderStack( RS_GUI , m_MapRegionAlphaTex[strAlphaTex] , NULL , PT_TRIANGLESTRIP 
				, 4 , 2 , VerText2D::Format , sizeof(VerText2D) , (void**)&pVB , NULL ,(void**)&pRS);

			pRS->m_AlpOP_S0 = TOP_SELECTARG1;
			pRS->m_AlphaBlendEnable = TRUE;
			pRS->m_SrcBlend = BLEND_ONE;
			pRS->m_DestBlend = BLEND_ONE;
			pRS->m_AlphaTestEnable = FALSE;

			//算大小
			pVB[0] = VerText2D( 0.0f		, 0.0f			, 1.0f, MinUV,MinUV );
			pVB[1] = VerText2D( m_fRTWidth	, 0.0f			, 1.0f, MaxUV,MinUV );
			pVB[2] = VerText2D( 0.0f		, m_fRTHeight	, 1.0f, MinUV,MaxUV );
			pVB[3] = VerText2D( m_fRTWidth	, m_fRTHeight	, 1.0f, MaxUV,MaxUV );
		}
		texNameIter++;
		SafeRelease(m_MapRegionAlphaTex[strAlphaTex]);//画完后就把这个alpha纹理销毁了
	}
}

bool CBigMap::LoadNameByXml(TiXmlAttribute *XmlAtt)
{
	string strRegionFileName = XmlAtt->ValueStr();
	XmlAtt=XmlAtt->Next();
	string strRegionGameName = XmlAtt->ValueStr();
	m_GameRegionToFileName[strRegionGameName.c_str()] = strRegionFileName.c_str();
	return true;
}

bool CBigMap::LoadMapConfigByXml(TiXmlAttribute *XmlAtt)
{
	m_bOpen = DWORD(atoi(XmlAtt->Value()));
	XmlAtt=XmlAtt->Next();
	m_bClose = DWORD(atoi(XmlAtt->Value()));
	return true;
}

bool CBigMap::LoadConfig(string& SceneName)
{
	CXmlConfig *pBigMapXmlCfg = NULL;
	ipkgstream in;
	string configName;
	format(configName,"%s/BigMap/bigmapconfig.xml",SceneName.c_str());
	if (!in.open(L"res", configName.c_str()))
		pBigMapXmlCfg = new CXmlConfig( "config" , in );
	else
		return false;

	bool AllComplate = true;

	TiXmlNode *pPathNode = pBigMapXmlCfg->Get<TiXmlElement*>("path_config");
	for (TiXmlNode *pi=pPathNode->FirstChild(); pi ; pi=pi->NextSibling())
	{
		if(!pi->ToElement())
			continue;
		AllComplate &= LoadNameByXml( pi->ToElement()->FirstAttribute() );
	}

	TiXmlNode *pMapNode = pBigMapXmlCfg->Get<TiXmlElement*>("map_config");
	for (TiXmlNode *pi=pMapNode->FirstChild(); pi ; pi=pi->NextSibling())
	{
		if(!pi->ToElement())
			continue;
		
		AllComplate &= LoadMapConfigByXml( pi->ToElement()->FirstAttribute() );
	}

	in.close();
	SafeDelete(pBigMapXmlCfg);

	return AllComplate;
}

void CBigMap::UpdateVisibleRegion()
{
	if(m_pAlphaTexture==NULL)
		return;
	m_pAlphaTexture->Begin(true);
	{
		m_pAlphaTexture->SetClearColor( CColor(0,0,0,0) );
		m_pAlphaTexture->Clear();
		GetGraphic()->SwapBegin();
		AlphaTexsRenderToTexture();
		GetGraphic()->SwapEnd();
	}
	m_pAlphaTexture->End();

}

void CBigMap::InitMapData( const TCHAR* szMapFile)
{
	ClearUp();

	m_SceneName = szMapFile;
	
	//读取区域名字和文件名对应关系
	if(!LoadConfig(m_SceneName))
		return;

	//创建m_MapTextureClose
	//创建m_MapTextureOpen
	CPkgFile::EnableAsyncRead(false);
	string FileName = "";
	format(FileName,"%s/BigMap/open.dds",m_SceneName.c_str());
	if(m_bOpen)
		GetGraphic()->CreateTexture(PATH_ALIAS_RES.c_str(), FileName.c_str(),&m_MapTextureOpen);
	else
	{
		m_MapTextureOpen = NULL;
		return;
	}
	FileName.clear();
	format(FileName,"%s/BigMap/close.dds",m_SceneName.c_str());
	if(m_bClose)
		GetGraphic()->CreateTexture(PATH_ALIAS_RES.c_str(), FileName.c_str(),&m_MapTextureClose);
	else
	{
		m_MapTextureClose = NULL;
		return;
	}

	SafeRelease(m_pAlphaTexture);
	m_pAlphaTexture = GetGraphic()->CreateRenderTarget();
	m_pAlphaTexture->SetWidth(uint32(GetWndWidth()));
	m_pAlphaTexture->SetHeight(uint32(GetWndHeight()));
	m_pAlphaTexture->SetUseDepthBuffer(false);
	m_pAlphaTexture->SetFormat(TFMT_A8R8G8B8);
	m_pAlphaTexture->Create();

	m_BackTexUV[2] = CVector2f(	0.0f , float(m_pAlphaTexture->GetHeight()));
	m_BackTexUV[0] = CVector2f( 0.0f , 0.0f );
	m_BackTexUV[3] = CVector2f( float(m_pAlphaTexture->GetWidth()) , float(m_pAlphaTexture->GetHeight()) );
	m_BackTexUV[1] = CVector2f( float(m_pAlphaTexture->GetWidth())   , 0.0f);
	m_fRTWidth = float(m_pAlphaTexture->GetWidth());
	m_fRTHeight = float(m_pAlphaTexture->GetHeight());

	if(m_pAlphaTexture==NULL)
		return;
	//string testdir = szMapFile;
	//testdir+= "yhl.dds";
	//m_pAlphaTexture->SaveToFile(testdir);
}



void CBigMap::SetBigMapWnd(SQRWnd * pWnd)
{
	AdvDelRef(m_pWnd);
	m_pWnd = pWnd;
	AdvAddRef(m_pWnd);
}
