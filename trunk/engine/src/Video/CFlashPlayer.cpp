#include "stdafx.h"
#include "CFlashPlayer.h"
#include "CGraphic.h"
#include "FlashSite.h"
#include "FlashHandler.h"
#include "CodeCvs.h"
#include "CFlashTmpFile.h"

//线程处理函数
void LoadFlashFileProc(LPVOID lparam)
{
	CFlashPlayer* flashPlay=(CFlashPlayer*)lparam;
	flashPlay->Load();
}

CFlashPlayer::CFlashPlayer(int w,int h)
:m_Width(w),
 m_Height(h),	
 m_site(NULL),
 m_shwaveflash(NULL),
 m_msgDispatch(NULL),
 m_oleObject(NULL),
 m_handler(NULL),
 m_pTexture(NULL)
{
	comCount=0;
	m_isTransparent=false;
	m_autoPlay=true;
	m_isCreated=false;
	m_isLoad=false;
}

CFlashPlayer::~CFlashPlayer(void)
{
	HRESULT hr=NULL;
	
	if(m_msgDispatch)
		m_msgDispatch->Release();
	
	if(m_shwaveflash)
		m_shwaveflash->Release();

	if(m_oleInPlaceObject)
		m_oleInPlaceObject->Release();

	if(m_handler)
	{
		hr=m_handler->Shutdown();
		m_handler->Release();
	}
	
	if(m_oleObject)
	{
		hr=m_oleObject->Close(OLECLOSE_NOSAVE);  //中止嵌入对象的激活状态 IOleClientSite--
		m_oleObject->SetClientSite(NULL);
		m_oleObject->Release();
	}
	
	if(m_site)
		m_site->Release();	

	if(m_pTexture)
	{
		m_pTexture->Release();
	}
}

bool CFlashPlayer::Create()
{
	//站点对象 m_site（IOleClientSite） 服务器对象m_oleobject（IOleObject） 通过站点对象控制服务器对象

	//创建COM
	HRESULT hr=NULL;
	m_site = new FlashSite(); 
	m_site->AddRef();	
	m_site->Init(this);
	if(!m_oleObject)
	{
		hr = CoCreateInstance(ShockwaveFlashObjects::CLSID_ShockwaveFlash, 0, CLSCTX_INPROC_SERVER, IID_IOleObject, (void**)&m_oleObject); 
	}
	if(!m_oleObject)
		return false;

	DWORD dwMiscStatus;
	m_oleObject->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);

	IOleClientSite* clientSite = 0;
	hr=m_site->QueryInterface(__uuidof(IOleClientSite), (void**)&clientSite);
	m_site->Release();
	if(!clientSite)
		return false;
	hr=m_oleObject->SetClientSite(clientSite); 
	
	hr=m_oleObject->QueryInterface(__uuidof(IOleInPlaceObject), (LPVOID*)&m_oleInPlaceObject); 
	if(!m_oleInPlaceObject)
		return false;
	InvalidateTotally();
	hr=m_oleInPlaceObject->SetObjectRects(&dirtyBounds, &dirtyBounds);
	
	hr=m_oleObject->QueryInterface(__uuidof(ShockwaveFlashObjects::IShockwaveFlash), (LPVOID*)&m_shwaveflash); 
	if(!m_shwaveflash)
		return false;
	m_shwaveflash->PutWMode("opaque");  // 实体/透明属性
	
	hr=m_oleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, 0, clientSite, 0, 0, 0);  //激活嵌入对象

	hr=m_oleObject->QueryInterface(__uuidof(IOleInPlaceObjectWindowless), (LPVOID*)&m_msgDispatch); 

	m_handler = new FlashHandler();
	m_handler->AddRef();	
	m_handler->Init(this);

	m_isCreated=true;

	//创建纹理
	CGraphic::GetInst()->CreateEmptyTexture(m_Width,m_Height,1, TF_RGB24,&m_pTexture);

	Play();

	return true;
}

void CFlashPlayer::SetFlashFile(const wchar_t* filename)
{
	m_File=filename;
	m_isLoad=false;
}

void CFlashPlayer::ResizeRect(float w,float h)
{
	if(m_pTexture)
	{	
		m_pTexture->Release();
		m_pTexture=NULL;
	}  
	//重新分配纹理
	m_Width=(int)w;
	m_Height=(int)h;
	CGraphic::GetInst()->CreateEmptyTexture(m_Width,m_Height,1, TF_RGB24,&m_pTexture);
	
	RECT rc;
	rc.top=rc.left=0;
	rc.right=m_Width;
	rc.bottom=m_Height;
	if(m_oleInPlaceObject)
		m_oleInPlaceObject->SetObjectRects(&rc, &rc);
}

void CFlashPlayer::InvalidateTotally()
{
	isClean=false;
	isTotallyDirty=true;
	dirtyBounds.left = 0;
	dirtyBounds.top = 0;
	dirtyBounds.right = m_Width;
	dirtyBounds.bottom = m_Height;
}

ITexture* CFlashPlayer::GetTexture()
{
	return m_pTexture;
}

float CFlashPlayer::GetTexWidth()
{
	return (float)m_Width;
}

float CFlashPlayer::GetTexHeight()
{
	return (float)m_Height;
}

void CFlashPlayer::UpdateTexture()
{
	if(!m_isCreated)
		Create();

	if(m_shwaveflash && m_pTexture && m_isLoad)
	{	
		RECT rc;
		rc.top=rc.left=0;
		rc.right=m_pTexture->GetWidth();
		rc.bottom=m_pTexture->GetHeight();
		//HRESULT hr=m_oleInPlaceObject->SetObjectRects(&tmprc, &tmprc);  //控制Flash绘制的大小区域 在本程序中不可见
		m_shwaveflash->put_ScaleMode(2);  //有的Flash文件在内部播放的时候可以调整了缩放参数，在这里强制其缩放到控件大小
		HRESULT hr=::OleDraw(m_shwaveflash,DVASPECT_TRANSPARENT,m_pTexture->GetHDC(),&rc);
	}
}
//Control

void CFlashPlayer::Play()
{
	if(m_shwaveflash)
	{
		Load();
		//m_hThread=::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)LoadFlashFileProc,(LPVOID)this,0,&m_threadID);
		//if(m_hThread==NULL)
		//	return ;
	}
}

void CFlashPlayer::Stop()
{
	if(m_shwaveflash)
		m_shwaveflash->Stop();
}

bool CFlashPlayer::Load()
{
	if(m_isLoad)  //已经Load
		return true; 

	CFlashTmpFile tmpFlashFile(m_File);
	if(!tmpFlashFile.CreateTmpFlash())
		return false;
	wstring pname=tmpFlashFile.GetTmpFileName();
	_bstr_t bstr(pname.c_str());
	HRESULT hr=m_shwaveflash->put_Movie(bstr);
	while(true)
	{
		if(4==m_shwaveflash->GetReadyState()) 
		{
			tmpFlashFile.Clear();
			break;
		}
	}
	m_shwaveflash->put_AlignMode(0);
	m_shwaveflash->put_ScaleMode(2);  //0 完全显示 1 无边框 2强制Flash影片大小和控件大小一致 3原始大小播放
	m_shwaveflash->Play();
	m_isLoad=true;

	return true;
}

void CFlashPlayer::SetTransparent(bool isTrans)
{
	if(!m_shwaveflash)
		return ;
	m_isTransparent = isTrans;
	if(m_isTransparent)
	{
		m_shwaveflash->PutWMode("transparent");
	}
	else
	{
		m_shwaveflash->PutWMode("opaque");
	}
}

void CFlashPlayer::SetQuality(short renderQuality)
{
	if(!m_shwaveflash)
		return ;
	switch(renderQuality)
	{
	case RQ_LOW:
		m_shwaveflash->PutQuality2("low");
		break;
	case RQ_MEDIUM:
		m_shwaveflash->PutQuality2("medium");
		break;
	case RQ_HIGH:
		m_shwaveflash->PutQuality2("high");
		break;
	case RQ_BEST:
		m_shwaveflash->PutQuality2("best");
		break;
	case RQ_AUTOLOW:
		m_shwaveflash->PutQuality2("autolow");
		break;
	case RQ_AUTOHIGH:
		m_shwaveflash->PutQuality2("autohigh");
		break;
	}
}

void CFlashPlayer::SetScaleMode(short scaleMode)
{
	if(m_shwaveflash)
		m_shwaveflash->PutScaleMode(scaleMode);
}

//MSG
void CFlashPlayer::ProcMouseMsg(UINT msg,WPARAM wParam,LPARAM lParam)
{
	if(!m_msgDispatch)
		return ;
	LRESULT result;
	switch(msg)
	{
	case WM_MOUSEMOVE:
		{
			m_msgDispatch->OnWindowMessage(WM_MOUSEMOVE,0,lParam,&result);
			break;
		}
	case WM_MOUSEWHEEL:
		{
			m_msgDispatch->OnWindowMessage(WM_MOUSEWHEEL,wParam,lParam,&result);
			break;
		}
	case WM_LBUTTONDOWN:
		{
			m_msgDispatch->OnWindowMessage(WM_LBUTTONDOWN,MK_LBUTTON,lParam,&result);
			break;
		}
	case WM_LBUTTONUP:
		{
			m_msgDispatch->OnWindowMessage(WM_LBUTTONUP,MK_LBUTTON,lParam,&result);
			break;
		}
	case WM_RBUTTONDOWN:
		{
			m_msgDispatch->OnWindowMessage(WM_RBUTTONDOWN,MK_RBUTTON,lParam,&result);
			break;
		}
	case WM_RBUTTONUP:
		{
			m_msgDispatch->OnWindowMessage(WM_RBUTTONUP,MK_RBUTTON,lParam,&result);
			break;
		}
	case WM_MBUTTONDOWN:
		{
			m_msgDispatch->OnWindowMessage(WM_MBUTTONDOWN,MK_MBUTTON,lParam,&result);
			break;
		}
	case WM_MBUTTONUP:
		{
			m_msgDispatch->OnWindowMessage(WM_MBUTTONUP,MK_MBUTTON,lParam,&result);
			break;
		}
	}
}

void CFlashPlayer::ProcKeyBoardMsg(UINT msg,WPARAM wParam,LPARAM lParam)
{
	if(!m_msgDispatch)
		return ;
	LRESULT result;
	m_msgDispatch->OnWindowMessage(msg,wParam,lParam,&result);
}

void CFlashPlayer::HandleFlashCall(const std::wstring& xmlString)
{
}