// D3d.h: interface for the CD3d class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFXSQRD3D_H__D5890401_B53C_11D6_96FF_5254AB25D0BE__INCLUDED_)
#define AFXSQRD3D_H__D5890401_B53C_11D6_96FF_5254AB25D0BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once

#include "../../src/Graphic/Graphic_dx9/Dx9Base.h"
#include "GraphicBase.h"

#endif // _MSC_VER > 1000
#define D3DOK_RESET			MAKE_D3DSTATUS(1)
#define D3DOK_RECREATE		MAKE_D3DSTATUS(2)

struct UpdateRes
{
	virtual LRESULT RestoreRes()=0;
	virtual LRESULT ReleaseRes()=0;
};

class CD3d  
{
	//D3D设备组件
	LPDIRECT3D9           m_lpD3d;              //D3D接口
	LPDIRDEV              m_lpD3DDc;            //D3D设备

	//设备环境
	D3DPRS_PARAM          m_PresentParam;       //画面显示方式
	D3DCAPS               m_D3dCaps;            //设备性能
	DWORD                 m_VertexProcess;      //顶点处理方式
	D3DDEVTYPE            m_DeviceType;         //设备类型

	RENDSTATE             m_FogMode;            //雾的模式
	D3DFORMAT             m_TexMode;            //贴图格式
	DWORD                 m_FogColor;           //雾的颜色

	int                   m_DisModeNum;         //显示模式数量
	D3DDISMD*             m_DisList;            //显示模式列表
	D3DDISMD              m_CurDis;             //当前显示模式
	HWND                  m_hWnd;               //应用程序主窗口句柄
	UpdateRes*            m_UpdateRes;          //应用程序指针

	//淡入淡出
	LPVERBUF              m_FadeVer;            //淡入淡出所用顶点
	DWORD                 m_FadeColor;          //淡入淡出所用颜色
	DWORD                 m_FadeTime;           //淡入淡出起始时间
	DWORD                 m_FadePeriod;         //淡入淡出所用时间
	FADE_MODE             m_FadeMode;           //淡入淡出所用方式

public:
	//d3dd.cpp
	int CreateD3d(HWND hWnd,UpdateRes* UDRes,int Width=0,int Height=0,
		D3DFORMAT Format=D3DFMT_R5G6B5,D3DDEVTYPE type=D3DDEVTYPE_HAL);
	int ReleaseD3d();
	int ListDisPlayMode();
	int GetDisPlayMode(D3DDISPLAYMODE& mode);
	int SetEnvironment();
	int ResetD3d(int Width=0,int Height=0,D3DFORMAT Format=D3DFMT_R5G6B5);

	int CreateUtilRes();
	int ReleaseUtilRes();
		
	int GetDisModeNum();
	D3DDISMD* GetDisMode(int num=-1);
	D3DFORMAT GetTextFormat();
	LPDIRDEV  GetDevice();
	D3DCAPS&  GetCaps();
	
	int RenderBegin();
	int RenderEnd();
	int RendFade();
	
	int GetTransform(TANSSTATE tanstate,D3DMATRIX* mat);
	int SetTransform(TANSSTATE tanstate,D3DMATRIX* mat);
	
	int SetFVF(DWORD FVF);
	int SetFade(FADE_MODE mode,DWORD color=0x00000000,DWORD period=1000);
	int SetFog(DWORD color=0,float start=0,float end=0,float density=0);
	int GetFogColor();
	int SetTextAlpha(BYTE Enable=1);
	int SetTexture(int stage,LPDIRTEX text);
	int SetStreamSource(UINT streamnum,LPVERBUF verbuf,UINT Off,UINT stride);
	int SetIndices(LPINDBUF indbuf);
	int SetRenderState( D3DRENDERSTATETYPE State, DWORD Value );
	int SetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value);
	int SetTextureStageState( DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value );

	int SetVertexShaderConstantF( UINT streamnum, float* pData, UINT Vector4fCount );
	int SetVexBlend();

	int DrawIndexedPrimitive(VERTYPE Type,INT BaseVerInd,UINT mindex,UINT vernum,UINT startind,UINT polygen);
	int DrawPrimitive(VERTYPE Type,UINT StartVer,UINT VerNum);

	CD3d();
	virtual ~CD3d();
};
	
////////////////////////////////////////////////////////////////////////////
// 功能：预备渲染
// 参数：无
// 返回：>=0 成功，<0 失败
///////////////////////////////////////////////////////////////////////////
inline int CD3d::RenderBegin()
{ 
	m_lpD3DDc->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,m_FogColor,1.0f,0);
	
	HRESULT hr;
	if( FAILED( hr = m_lpD3DDc->TestCooperativeLevel() ) )
    {
        // 当D3DDevice丢失时,要等到设备有效后方可继续
        if( D3DERR_DEVICELOST==hr )
            return hr;
		
        // Reset D3DDevice
        if( D3DERR_DEVICENOTRESET==hr )
        {
			// 在Reset前必须要释放资源(一些容易Lost的如bmp等)
			ReleaseUtilRes();

            if( FAILED( hr=m_lpD3DDc->Reset(&m_PresentParam) ) ) 
			{
				return hr;
			}

			// 在Reset后必须要重新载入资源(由ReleaseResource释放掉的)
			SetEnvironment();
			CreateUtilRes();
		}

        return hr;
    }

	return m_lpD3DDc->BeginScene(); 
}

/////////////////////////////////////////////////////////////////
// 功能：结束渲染
// 参数：无
// 返回：>=0 成功，<0 失败
//////////////////////////////////////////////////////////////////
inline int CD3d::RenderEnd()
{ 
	m_lpD3DDc->EndScene(); 
	return m_lpD3DDc->Present(NULL,NULL,NULL,NULL); 
}

/////////////////////////////////////////////////////////////////
// 功能：设置淡入淡出参数
// 参数：FADE_MODE mode 方式,DWORD color 颜色,DWORD period 时间
// 返回：>=0 成功，<0 失败
//////////////////////////////////////////////////////////////////
inline int CD3d::SetFade(FADE_MODE mode,DWORD color,DWORD period)
{
	m_FadeMode=mode;
	m_FadeTime=GetTickCount();
	m_FadeColor=((m_FadeMode<0)?0xff000000:0)|(color&0x00ffffff);
	m_FadePeriod=period;
	return 0;
}

/////////////////////////////////////////////////////////////////
// 功能：得到设备性能
// 参数：无
// 返回：设备性能
//////////////////////////////////////////////////////////////////
inline D3DCAPS& CD3d::GetCaps()
{
	return m_D3dCaps;
}

/////////////////////////////////////////////////////////////////
// 功能：得到显示模式数量
// 参数：无
// 返回：显示模式数量
//////////////////////////////////////////////////////////////////
inline int CD3d::GetDisModeNum()
{
	return m_DisModeNum;
}

/////////////////////////////////////////////////////////////////
// 功能：得到显示模式
// 参数：int num <0当前显示模式，>=0列表的显示模式
// 返回：显示模式，不成功返回NULL
//////////////////////////////////////////////////////////////////
inline D3DDISMD* CD3d::GetDisMode(int num)
{
	return (num<0)?&m_CurDis:((num<m_DisModeNum)?&m_DisList[num]:NULL);
}

/////////////////////////////////////////////////////////////////
// 功能：得到适合的贴图格式
// 参数：无
// 返回：贴图格式
//////////////////////////////////////////////////////////////////
inline D3DFORMAT CD3d::GetTextFormat()
{ 
	return m_TexMode; 
}

//////////////////////////////////////////////////////////////////////////////////////////////
//下面函数提供对类成员m_lpD3DDc的函数调用
//////////////////////////////////////////////////////////////////////////////////////////////
inline LPDIRDEV CD3d::GetDevice()
{ 
	return m_lpD3DDc; 
}

inline int CD3d::GetTransform(TANSSTATE Transtate,D3DMATRIX* Mat)
{
	return m_lpD3DDc->GetTransform(Transtate,Mat);
}

inline int CD3d::SetFVF(DWORD FVF)
{
	return m_lpD3DDc->SetFVF(FVF);
}

inline int CD3d::SetFog(DWORD color,float start,float end,float density)
{
	if(m_FogMode==D3DRS_FORCE_DWORD)
		return 1;
	if(color)
		m_FogColor=color;
	if(start==0&&end==0&&density==0)
		return m_lpD3DDc->SetRenderState(D3DRS_FOGENABLE,FALSE);
	m_lpD3DDc->SetRenderState(D3DRS_FOGENABLE,TRUE);
	m_lpD3DDc->SetRenderState(D3DRS_FOGSTART,*((DWORD*)&start));
    m_lpD3DDc->SetRenderState(D3DRS_FOGEND,*((DWORD*)&end));
    m_lpD3DDc->SetRenderState(D3DRS_FOGDENSITY,*((DWORD*)&density));
    m_lpD3DDc->SetRenderState(D3DRS_FOGCOLOR,color);
    m_lpD3DDc->SetRenderState(m_FogMode,D3DFOG_LINEAR);
    m_lpD3DDc->SetRenderState(D3DRS_RANGEFOGENABLE,0);

	return 0;
}

inline int CD3d::GetFogColor()
{
	return m_FogColor;
}

inline int CD3d::SetTexture(int stage,LPDIRTEX text)
{
	return m_lpD3DDc->SetTexture(stage,text);
}

inline int CD3d::SetTextAlpha(BYTE Enable)
{
	if(Enable)
	{
        m_lpD3DDc->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		m_lpD3DDc->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		m_lpD3DDc->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER );
	}
	else
		m_lpD3DDc->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	return 0; 
}

inline int CD3d::SetRenderState( D3DRENDERSTATETYPE State, DWORD Value )
{
	return m_lpD3DDc->SetRenderState( State, Value );
}

inline int CD3d::SetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value)
{
	return m_lpD3DDc->SetSamplerState(Sampler,Type,Value);
}

inline int CD3d::SetTextureStageState( DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value )
{
	return m_lpD3DDc->SetTextureStageState(Stage,Type,Value);
}

inline int CD3d::SetTransform(TANSSTATE Transtate,D3DMATRIX* Mat)
{
	return m_lpD3DDc->SetTransform(Transtate,Mat);
}

inline int CD3d::SetStreamSource(UINT StreamNum,LPVERBUF VerBuf,UINT Off,UINT Stride)
{
	return m_lpD3DDc->SetStreamSource(StreamNum,VerBuf,Off,Stride);
}

inline int CD3d::SetIndices(LPINDBUF IndBuf)
{
	return m_lpD3DDc->SetIndices(IndBuf);
}

inline int CD3d::DrawIndexedPrimitive(VERTYPE Type,INT BaseVerInd,UINT MinInd,UINT VerNum,UINT StartInd,UINT Polygen)
{
	return m_lpD3DDc->DrawIndexedPrimitive(Type,BaseVerInd,MinInd,VerNum,StartInd,Polygen);
}

inline int CD3d::DrawPrimitive(VERTYPE Type,UINT StartVer,UINT VerNum)
{
	return m_lpD3DDc->DrawPrimitive(Type,StartVer,VerNum);
}

//////////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFXSQRD3D_H__D5890401_B53C_11D6_96FF_5254AB25D0BE__INCLUDED_)
