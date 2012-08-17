// D3d.cpp: implementation of the CD3d class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3dd.h"
#include "BaseHelper.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CD3d::CD3d()
{
	m_lpD3d=NULL;
	m_lpD3DDc=NULL;
	m_FogColor=0xffcccc88;
	m_TexMode=D3DFMT_R5G6B5;
	m_UpdateRes=NULL;
	m_DisModeNum=0;
	m_FadeVer=NULL;
	m_FadeMode=FADE_NULL;
	m_FadeColor=0x0;
	m_DisList = NULL;
}

CD3d::~CD3d()
{
	ReleaseUtilRes();
	SafeRelease(m_lpD3DDc);
	SafeRelease(m_lpD3d);
	delete []m_DisList;
}

////////////////////////////////////////////////////////////////////////
// 功能：创建D3D渲染设备并获得设备特性
// 参数：HWND hWnd 主应用程序窗口,
//       UpdateRes* UDRes 主应用程序类指针（用于资源管理）,
//       int Width 主窗口宽度,
//       int Height 主窗口高度,
//       D3DFORMAT Format 象素格式,
//       D3DDEVTYPE type 设备类型
// 返回：>=0 成功，<0 失败
////////////////////////////////////////////////////////////////////////
int CD3d::CreateD3d(HWND hWnd,UpdateRes* UDRes,int Width,int Height,D3DFORMAT Format,D3DDEVTYPE type)
{
	m_hWnd=hWnd;
	m_UpdateRes=UDRes;
	m_DeviceType=type;

	m_lpD3d=Direct3DCreate9(D3D_SDK_VERSION);
	if(!m_lpD3d)
		return GERROR;
	
	ListDisPlayMode();
	
	if(FAILED(m_lpD3d->GetAdapterDisplayMode( D3DADAPTER_DEFAULT,&m_CurDis)))
		return GERROR;

	ZeroMemory(&m_PresentParam, sizeof(m_PresentParam));	
	if(Width)
	{
		m_CurDis.Width=Width;
		m_CurDis.Height=Height;
		m_CurDis.Format=Format;

		if(FAILED(GetDisPlayMode(m_CurDis)))
			return GERROR;

		m_PresentParam.Windowed=FALSE;
		m_PresentParam.hDeviceWindow = m_hWnd;
		m_PresentParam.BackBufferCount = 1;
		m_PresentParam.BackBufferWidth=m_CurDis.Width;
		m_PresentParam.BackBufferHeight=m_CurDis.Height;
		m_PresentParam.BackBufferFormat=m_CurDis.Format;
		m_PresentParam.SwapEffect=D3DSWAPEFFECT_FLIP;
		m_PresentParam.FullScreen_RefreshRateInHz=m_CurDis.RefreshRate;
		m_PresentParam.EnableAutoDepthStencil=TRUE;
		m_PresentParam.AutoDepthStencilFormat=D3DFMT_D16;
		m_PresentParam.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
	}
	else
	{
		RECT rect;
		GetClientRect(m_hWnd,&rect);
		m_CurDis.Width=rect.right-rect.left;
		m_CurDis.Height=rect.bottom-rect.top;

		m_PresentParam.Windowed=TRUE;
		m_PresentParam.hDeviceWindow = m_hWnd;
		m_PresentParam.BackBufferCount = 1;
		m_PresentParam.BackBufferWidth=m_CurDis.Width;
		m_PresentParam.BackBufferHeight=m_CurDis.Height;
		m_PresentParam.BackBufferFormat=m_CurDis.Format;
		m_PresentParam.SwapEffect=D3DSWAPEFFECT_COPY;
		m_PresentParam.FullScreen_RefreshRateInHz=0;
		m_PresentParam.EnableAutoDepthStencil=TRUE;
		m_PresentParam.AutoDepthStencilFormat=D3DFMT_D16;
		m_PresentParam.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
	}
	
	m_VertexProcess=D3DCREATE_MIXED_VERTEXPROCESSING;
	if(FAILED(m_lpD3d->CreateDevice(D3DADAPTER_DEFAULT,m_DeviceType,
		m_hWnd,m_VertexProcess,&m_PresentParam,&m_lpD3DDc)))
	{
		m_VertexProcess=D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		if(FAILED(m_lpD3d->CreateDevice(D3DADAPTER_DEFAULT,m_DeviceType,
			m_hWnd,m_VertexProcess,&m_PresentParam,&m_lpD3DDc)))
			return GERROR;
	}
	m_lpD3DDc->GetDeviceCaps(&m_D3dCaps);

	//检测贴图格式
	LPDIRECT3DTEXTURE9 text;
	if(m_lpD3DDc->CreateTexture(4,4,1,0,D3DFMT_DXT1,
		D3DPOOL_MANAGED,&text,NULL)==D3D_OK)
		m_TexMode=D3DFMT_DXT1;
	else if(m_lpD3DDc->CreateTexture(4,4,1,0,D3DFMT_A4R4G4B4,
		D3DPOOL_MANAGED,&text,NULL)==D3D_OK)
		m_TexMode=D3DFMT_A4R4G4B4;
	else if(m_lpD3DDc->CreateTexture(4,4,1,0,D3DFMT_R5G6B5,
		D3DPOOL_MANAGED,&text,NULL)==D3D_OK)
		m_TexMode=D3DFMT_R5G6B5;
	else
		return GERROR;
	text->Release();

	//检查雾化功能
	DWORD dwCaps=m_D3dCaps.RasterCaps;
    if((dwCaps&D3DPRASTERCAPS_FOGTABLE)&&
		((dwCaps&D3DPRASTERCAPS_ZFOG)||(dwCaps&D3DPRASTERCAPS_WFOG)))
		m_FogMode=D3DRS_FOGTABLEMODE;
	else if(dwCaps&D3DPRASTERCAPS_FOGVERTEX)
		m_FogMode=D3DRS_FOGVERTEXMODE;
	else
		m_FogMode=D3DRS_FORCE_DWORD;

	if(FAILED(SetEnvironment()))
		return GERROR;
	return CreateUtilRes();
}

////////////////////////////////////////////////////////////////////////
// 功能：设置基本渲染环境
// 参数：无
// 返回：>=0 成功，<0 失败
////////////////////////////////////////////////////////////////////////
int CD3d::SetEnvironment()
{
	D3DVIEWPORT vp;
    m_lpD3DDc->GetViewport(&vp);
    FLOAT fAspect = ((FLOAT)vp.Width)/vp.Height;

	D3DMATERIAL mtrl;
	ZeroMemory( &mtrl, sizeof(D3DMATERIAL) );
    mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
    mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
    mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
    mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
    m_lpD3DDc->SetMaterial(&mtrl);

	D3DXVECTOR3 vEyePt(0.0f,100.0f,0.0f);
    D3DXVECTOR3 vLookatPt(0.0f,0.0f,0.0f);
    D3DXVECTOR3 vUpVec(0.0f,0.0f,0.0f);
    D3DXMATRIX matWorld, matView, matProj;

    D3DXMatrixIdentity(&matWorld);
	D3DXMatrixLookAtLH(&matView,&vEyePt,&vLookatPt,&vUpVec);
	D3DXMatrixPerspectiveFovLH(&matProj,1.570796f,fAspect,1.0f,20000.0f);
    
    m_lpD3DDc->SetTransform( D3DTS_WORLD,&matWorld );
    m_lpD3DDc->SetTransform( D3DTS_VIEW,&matView );
    m_lpD3DDc->SetTransform( D3DTS_PROJECTION, &matProj );

	D3DLIGHT DirLight;
	ZeroMemory(&DirLight ,sizeof(DirLight));
	DirLight.Type =D3DLIGHT_DIRECTIONAL;	
	DirLight.Direction =D3DXVECTOR3( 0.3f, -0.1f, 0.3f );
	DirLight.Diffuse.r = 1.0f;
	DirLight.Diffuse.g = 1.0f;
	DirLight.Diffuse.b = 1.0f;
	DirLight.Diffuse.a = 1.0f;
	DirLight.Specular = DirLight.Diffuse; 
	DirLight.Attenuation0 = 1;
	DirLight.Range = 1000;
	DirLight.Falloff = 0;
	DirLight.Attenuation0 = 1;
	DirLight.Attenuation1 = 0;
	DirLight.Attenuation2 = 0;

	D3DXVec3Normalize( (D3DXVECTOR3*)&DirLight.Direction, (D3DXVECTOR3*)&DirLight.Direction );
	if(m_lpD3DDc->SetLight(0,&DirLight)!=D3D_OK)
		return GERROR;
	if(m_lpD3DDc->LightEnable(0,TRUE)!=D3D_OK)
		return GERROR;

	float Start = 2500.0f,End = 2700.0f,Density = 0.66f;

    m_lpD3DDc->SetRenderState(D3DRS_AMBIENT,0x00bfbfbf); 
	m_lpD3DDc->SetRenderState(D3DRS_NORMALIZENORMALS,TRUE);

	m_lpD3DDc->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
	m_lpD3DDc->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
	m_lpD3DDc->SetSamplerState( 2, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
	m_lpD3DDc->SetSamplerState( 3, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
	m_lpD3DDc->SetRenderState( D3DRS_SPECULARENABLE, TRUE );

	return 0;
}

////////////////////////////////////////////////////////////////////////
// 功能：释放D3D渲染设备
// 参数：无
// 返回：>=0 成功，<0 失败
////////////////////////////////////////////////////////////////////////
int CD3d::ReleaseD3d()
{
	ReleaseUtilRes();
	SafeRelease(m_lpD3DDc);
	SafeRelease(m_lpD3d);
	delete []m_DisList;

	m_lpD3d=NULL;
	m_lpD3DDc=NULL;
	m_FogColor=0xffcccc88;
	m_TexMode=D3DFMT_R5G6B5;
	m_UpdateRes=NULL;
	m_DisModeNum=0;
	m_FadeVer=NULL;
	m_FadeMode=FADE_NULL;
	m_FadeColor=0x0;
	m_DisList = NULL;

	return 0;
}

////////////////////////////////////////////////////////////////////////
// 功能：列出所有显示模式
// 参数：无
// 返回：>=0 成功，<0 失败
////////////////////////////////////////////////////////////////////////
int CD3d::ListDisPlayMode()
{
	D3DFORMAT DisFmt[3] = { D3DFMT_X8R8G8B8, D3DFMT_X1R5G5B5, D3DFMT_R5G6B5 };
	int DisFormatBit[3] = { 32, 16, 16 };

	int i, j;
	m_DisModeNum = 0;
	vector<int> NumPerFormat;
	for( i = 0; i < 3; i++ )
	{
		NumPerFormat.push_back( m_lpD3d->GetAdapterModeCount( D3DADAPTER_DEFAULT, DisFmt[i] ) );
		m_DisModeNum += NumPerFormat[i];
	}
	m_DisList=new D3DDISPLAYMODE[m_DisModeNum];

	m_DisModeNum = 0;
	for( i = 0; i < (int)NumPerFormat.size(); i++ )
	{
		for( j = 0; j < NumPerFormat[i]; j++ )
			m_lpD3d->EnumAdapterModes( D3DADAPTER_DEFAULT, DisFmt[i], j, &m_DisList[ m_DisModeNum + j ]);
		m_DisModeNum += NumPerFormat[i];
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////
// 功能：根据参数期望的显示模式返回适当的显示模式
// 参数：D3DDISPLAYMODE& mode 期望的显示模式，
// 说明：返回的显示模式宽和高大于等于期望值，但尽量接近；
//       而刷新率从健康角度考虑，尽量选择高的
// 返回：>=0 成功，<0 失败
////////////////////////////////////////////////////////////////////////
int CD3d::GetDisPlayMode(D3DDISPLAYMODE& mode)
{
	int i,j;
	D3DDISPLAYMODE Select={0,0,0,mode.Format};
	for(i=0;i<m_DisModeNum;i++)
	{
		if(m_DisList[i].Format==mode.Format)
		{
			for(j=i;m_DisList[j].Format==mode.Format&&j<m_DisModeNum;j++)
			{
				if(m_DisList[j].Width>=mode.Width&&
					m_DisList[j].Height>=mode.Height&&
					m_DisList[j].Width-mode.Width<=Select.Width-mode.Width&&
					m_DisList[j].Height-mode.Height<=Select.Height-mode.Height)
				{
					Select.Width=m_DisList[j].Width;
					Select.Height=m_DisList[j].Height;
					Select.RefreshRate=max( Select.RefreshRate,m_DisList[j].RefreshRate);
				}
			}
		}
	}

	if(Select.Width==0||Select.Height==0)
		return GERROR;
	mode=Select;
	return 0;
}

////////////////////////////////////////////////////////////////////////
// 功能：根据参数期望的显示模式重新设置显示模式
// 参数：int Width 主窗口宽度,
//       int Height 主窗口高度,
//       D3DFORMAT Format 象素格式
// 返回：>=0 成功，<0 失败
////////////////////////////////////////////////////////////////////////
int CD3d::ResetD3d(int Width,int Height,D3DFORMAT Format)
{
	if(FAILED(m_lpD3d->GetAdapterDisplayMode( D3DADAPTER_DEFAULT,&m_CurDis)))
		return GERROR;

	ZeroMemory(&m_PresentParam, sizeof(m_PresentParam));	
	if(Width)
	{
		m_CurDis.Width=Width;
		m_CurDis.Height=Height;
		m_CurDis.Format=Format;

		if(FAILED(GetDisPlayMode(m_CurDis)))
			return GERROR;

		m_PresentParam.Windowed=FALSE;
		m_PresentParam.hDeviceWindow = m_hWnd;
		m_PresentParam.BackBufferCount = 1;
		m_PresentParam.BackBufferWidth=m_CurDis.Width;
		m_PresentParam.BackBufferHeight=m_CurDis.Height;
		m_PresentParam.BackBufferFormat=m_CurDis.Format;
		m_PresentParam.SwapEffect=D3DSWAPEFFECT_FLIP;
		m_PresentParam.FullScreen_RefreshRateInHz=m_CurDis.RefreshRate;
		m_PresentParam.EnableAutoDepthStencil=TRUE;
		m_PresentParam.AutoDepthStencilFormat=D3DFMT_D16;
		m_PresentParam.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
	}
	else
	{
		RECT rect;
		GetClientRect(m_hWnd,&rect);
		m_CurDis.Width=rect.right-rect.left;
		m_CurDis.Height=rect.bottom-rect.top;

		m_PresentParam.Windowed=TRUE;
		m_PresentParam.hDeviceWindow = m_hWnd;
		m_PresentParam.BackBufferCount = 1;
		m_PresentParam.BackBufferWidth=m_CurDis.Width;
		m_PresentParam.BackBufferHeight=m_CurDis.Height;
		m_PresentParam.BackBufferFormat=m_CurDis.Format;
		m_PresentParam.SwapEffect=D3DSWAPEFFECT_COPY;
		m_PresentParam.FullScreen_RefreshRateInHz=0;
		m_PresentParam.EnableAutoDepthStencil=TRUE;
		m_PresentParam.AutoDepthStencilFormat=D3DFMT_D16;
		m_PresentParam.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
	}
	
	ReleaseUtilRes();

	if(FAILED(m_lpD3DDc->Reset(&m_PresentParam)))
	{		
		SafeRelease(m_lpD3DDc);
		SafeRelease(m_lpD3d);
		delete []m_DisList;
		
		m_lpD3d=NULL;
		m_lpD3DDc=NULL;
		m_FogColor=0xffcccc88;
		m_TexMode=D3DFMT_R5G6B5;
		m_DisModeNum=0;
		m_FadeVer=NULL;
		m_FadeMode=FADE_NULL;
		m_FadeColor=0x0;
		m_DisList = NULL;
		if(FAILED(CreateD3d(m_hWnd,m_UpdateRes,Width,Height,Format,m_DeviceType)))
			return GERROR;

		return D3DOK_RECREATE;
	}

	if(FAILED(SetEnvironment()))
		return GERROR;
	if(FAILED(CreateUtilRes()))
		return GERROR;

	return D3DOK_RESET;
}

////////////////////////////////////////////////////////////////////////
// 功能：创建应用程序所需所有资源
// 参数：无
// 返回：>=0 成功，<0 失败
////////////////////////////////////////////////////////////////////////
int CD3d::CreateUtilRes()
{
	SafeRelease(m_FadeVer);
	
	HRESULT hr = 0;
	hr|= m_lpD3DDc->CreateVertexBuffer(sizeof(VerColor2D)*4,
		D3DUSAGE_WRITEONLY,VerColor2D::Format,D3DPOOL_DEFAULT,&m_FadeVer,NULL);	
	
	VerColor2D* Ver;
	float x=(float)m_CurDis.Width;
	float y=(float)m_CurDis.Height;
	m_FadeVer->Lock(0,0,(void**)&Ver, 0);
	Ver[0]=VerColor2D(0,y,0,0);
	Ver[1]=VerColor2D(0,0,0,0);
	Ver[2]=VerColor2D(x,y,0,0);
	Ver[3]=VerColor2D(x,0,0,0);
	m_FadeVer->Unlock();
	
	if(m_UpdateRes)
		m_UpdateRes->RestoreRes();

	return 0;
}

////////////////////////////////////////////////////////////////////////
// 功能：释放应用程序所有资源
// 参数：无
// 返回：>=0 成功，<0 失败
////////////////////////////////////////////////////////////////////////
int CD3d::ReleaseUtilRes()
{
	SafeRelease(m_FadeVer);
	if( m_UpdateRes )
		m_UpdateRes->ReleaseRes();
	return 0;
}

////////////////////////////////////////////////////////////////////////
// 功能：渲染淡入淡出过程
// 参数：无
// 返回：>=0 成功，<0 失败
////////////////////////////////////////////////////////////////////////
int CD3d::RendFade()
{	
	if(m_FadeMode==FADE_NULL)
		return 0;
	DWORD st=GetTickCount();

	if(st-m_FadeTime<m_FadePeriod)
	{
		int n=0;
		if(m_FadeMode>FADE_NULL)
			n=1;
		if(m_FadeMode<FADE_NULL)
			n=-1;
		DWORD al=(m_FadeColor>>24)+(((65535/m_FadePeriod)*((int)(st-m_FadeTime)))>>8)*n;
		DWORD alpha=((((DWORD)al)<<24)&0xff000000)|(m_FadeColor&0x00ffffff);

		VerColor2D* Ver;
		m_FadeVer->Lock(0,0,(void**)&Ver, 0);
		Ver[0].diffuse=Ver[1].diffuse=Ver[2].diffuse=Ver[3].diffuse=alpha;
		m_FadeVer->Unlock();
	}
	else if(m_FadeMode==FADE_IN)
	{
		m_FadeMode=FADE_NULL;
		return 0;
	}
	else if(m_FadeMode==FADE_INEX)
	{
		m_FadeMode=FADE_IN;
		m_FadeTime=GetTickCount();
	}
	else if(m_FadeMode==FADE_OUT)
	{
		VerColor2D* Ver;
		m_FadeVer->Lock(0,0,(void**)&Ver, 0);
		Ver[0].diffuse=Ver[1].diffuse=Ver[2].diffuse=Ver[3].diffuse=0xff000000|m_FadeColor;
		m_FadeVer->Unlock();
		m_FadeMode=FADE_OUTEX;
	}

	m_lpD3DDc->SetTexture(0,NULL);
	m_lpD3DDc->SetFVF(VerColor2D::Format);
	m_lpD3DDc->SetStreamSource(0,m_FadeVer,0,sizeof(VerColor2D));
	m_lpD3DDc->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);

	return 0;
}










