// GLobD3d.h: interface for the GLobD3d class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBD3D_H__97BB9FA2_A00E_48EF_AFE6_6F5A5B101D8C__INCLUDED_)
#define AFX_GLOBD3D_H__97BB9FA2_A00E_48EF_AFE6_6F5A5B101D8C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3d9.h>
#include <math.h>
#include <D3dx9math.h>
#include <mmsystem.h>
#include <dsound.h>
#include <dinput.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "CTinyVector.h"
#include "CVector2.h"
#include "CVector3.h"
////////////////////////////////////////////////////////////////                 //需要更改

#define LPVERBUF         LPDIRECT3DVERTEXBUFFER9
#define LPINDBUF         LPDIRECT3DINDEXBUFFER9
#define LPDIRDEV         LPDIRECT3DDEVICE9
#define LPDIRTEX         LPDIRECT3DTEXTURE9
#define LPSURFACE        LPDIRECT3DSURFACE9
#define LPDIRSWAP        LPDIRECT3DSWAPCHAIN9
#define LPDIRDECLR       LPDIRECT3DVERTEXDECLARATION9
#define LPDIRSHADE       LPDIRECT3DVERTEXSHADER9
#define RENDSTATE        D3DRENDERSTATETYPE
#define TANSSTATE        D3DTRANSFORMSTATETYPE
#define VERTYPE          D3DPRIMITIVETYPE
#define D3DCAPS          D3DCAPS9
#define D3DVIEWPORT      D3DVIEWPORT9
#define D3DMATERIAL      D3DMATERIAL9
#define D3DLIGHT         D3DLIGHT9
#define D3DDISMD         D3DDISPLAYMODE
#define D3DVERELM        D3DVERTEXELEMENT9
#define D3DPRS_PARAM     D3DPRESENT_PARAMETERS

#define LPSOUND   LPDIRECTSOUND8
#define LPSNDBUF  LPDIRECTSOUNDBUFFER
#define LPSNDBUF8 LPDIRECTSOUNDBUFFER8
#define LPSND3D   LPDIRECTSOUND3DBUFFER
#define LPLISTEN  LPDIRECTSOUND3DLISTENER

#define LPDIRINP  LPDIRECTINPUT8
#define LPINPDEV  LPDIRECTINPUTDEVICE8

#ifdef  GERROR
#undef  GERROR
#endif
#define GERROR            -1

#define D3DOK_RESET      MAKESQRD3DSTATUS(1)
#define D3DOK_RECREATE   MAKESQRD3DSTATUS(2)

#define SAMPLE_BUFFER_SIZE	16
#define MATH_PI				3.14159265358979323846		// Pi

#define SafeRelease(p)  { if(p)p->Release(); p=NULL; }
 

#define BONEMESH    0x00000001

enum FADE_MODE
{
	FADE_IN          =-1,
	FADE_NULL        = 0,
	FADE_OUT         = 1,
	FADE_OUTEX       = 2,
	FADE_INEX        = 3
};

//using namespace std;
//////////////////////////////////////////////////////////////////
//typedef unsigned int		uint;
extern int g_XCur;            //全局鼠标X分量
extern int g_YCur;            //全局鼠标Y分量

extern int g_XScreen;         //全局屏幕X大小
extern int g_YScreen;         //全局屏幕X大小

//////////////////////////////////////////////////////////////////

#define PIXFORMATNUM  6

static D3DFORMAT DISPIXFORMAT[PIXFORMATNUM]=
{ 
	D3DFMT_X8R8G8B8, D3DFMT_A8R8G8B8, D3DFMT_A2R10G10B10, 
	D3DFMT_X1R5G5B5, D3DFMT_A1R5G5B5, D3DFMT_R5G6B5
};

static TCHAR DISPIXFORMATNAME[PIXFORMATNUM][20]=
{ 
	"D3DFMT_X8R8G8B8", "D3DFMT_A8R8G8B8", "D3DFMT_A2R10G10B10", 
	"D3DFMT_X1R5G5B5", "D3DFMT_A1R5G5B5", "D3DFMT_R5G6B5"
};

static int DISPIXFORMATBIT[PIXFORMATNUM]=
{ 
	32, 32, 32, 16, 16, 16
};

inline int GetDisPixFormatNum()
{
	return PIXFORMATNUM;
}

inline D3DFORMAT GetDisPixFormat(int num)
{
	return (num<PIXFORMATNUM)?DISPIXFORMAT[num]:D3DFMT_UNKNOWN;
}

inline TCHAR* GetDisPixFormatName(D3DFORMAT format)
{
	for(int i=0;i<PIXFORMATNUM;i++)
		if(DISPIXFORMAT[i]==format)
			return DISPIXFORMATNAME[i];
	return "";
}

inline int GetDisPixFormatBit(D3DFORMAT format)
{
	for(int i=0;i<PIXFORMATNUM;i++)
		if(DISPIXFORMAT[i]==format)
			return DISPIXFORMATBIT[i];
	return 0;
}
///////////////////////////////////////////////////////////////////
class CD3d;

struct VerPos
{
	VerPos(){};
	VerPos(float sx,float sy,float sz){ p.x=sx; p.y=sy; p.z=sz; };

	enum{ Format=D3DFVF_XYZ };

	CVector3f p;
};

struct VerColor3D
{
	VerColor3D(){};
	VerColor3D(float sx,float sy,float sz,DWORD dif){ p.x=sx; p.y=sy; p.z=sz; diffuse=dif;};

	enum{ Format=D3DFVF_XYZ|D3DFVF_DIFFUSE };

	CVector3f p;
	DWORD diffuse;
};

//struct VerNorTex
//{
//	VerNorTex(){ p.x=0; p.y=0; p.z=0; np.x=0; np.y=0; np.z=0; tu=0; tv=0; };
//	VerNorTex(float x,float y,float z,float nx,float ny,float nz,
//		float stu,float stv)
//	{
//		p.x=x; p.y=y; p.z=z; np.x=nx; np.y=ny; np.z=nz; tu=stu; tv=stv;
//	};
//
//	enum{ Format=D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1 };
//
//	CVector3f p;
//	CVector3f np;
//    float tu,tv; // The vertex colour.
//
//	BOOL operator==(VerNorTex& ver)
//	{
//		return (p==ver.p&&np==ver.np&&tu==ver.tu&&tv==ver.tv);
//	}
//};

struct VerNor
{
	VerNor(){ p.x=0; p.y=0; p.z=0; np.x=0; np.y=0; np.z=0; };
	VerNor(float sx,float sy,float sz,float snx,float sny,float snz)
	{
		p.x=sx; p.y=sy; p.z=sz; np.x=snx; np.y=sny; np.z=snz;
	};

	enum{ Format=D3DFVF_XYZ|D3DFVF_NORMAL };

	CVector3f p;
	CVector3f np;// The transformed position for the vertex.
};

struct VerNorDif
{
	VerNorDif(){ p.x=0; p.y=0; p.z=0; np.x=0; np.y=0; np.z=0; diffuse=0xffffffff; };
	VerNorDif(float sx,float sy,float sz,float snx,float sny,float snz,DWORD dif)
	{
		p.x=sx; p.y=sy; p.z=sz; np.x=snx; np.y=sny; np.z=snz; diffuse=dif;
	};

	enum{ Format=D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE };

	CVector3f p;
	CVector3f np;// The transformed position for the vertex.
	DWORD diffuse;
};

struct Vertex2D
{
	Vertex2D(){ x=0; y=0; z=0; w=1.0f; diffuse=0; tu=0; tv=0; }
	Vertex2D(float sx,float sy,float sz,DWORD dif,float stu,float stv)
	{
		x=sx; y=sy; z=sz; w=1.0f; diffuse=dif; tu=stu; tv=stv;
	}

	enum{ Format=D3DFVF_XYZRHW|D3DFVF_TEX1|D3DFVF_DIFFUSE };

	float x,y,z,w;
	DWORD diffuse;
	float tu,tv;
};   

struct VerColor2D
{
	VerColor2D(){ x=0; y=0; z=0; w=1.0f; diffuse=0; }
	VerColor2D(float sx,float sy,float sz,DWORD dif)
	{
		x=sx; y=sy; z=sz; w=1.0f; diffuse=dif;
	}

	enum{ Format=D3DFVF_XYZRHW|D3DFVF_DIFFUSE };

	float x,y,z,w;
	DWORD diffuse;
};

struct VerText2D
{
	VerText2D(){ x=0; y=0; z=0; w=1.0f; tu=0; tv=0; }
	VerText2D(float sx,float sy,float sz,float stu,float stv)
	{
		x=sx; y=sy; z=sz; w=1.0f; tu=stu; tv=stv;
	}

	enum{ Format=D3DFVF_XYZRHW|D3DFVF_TEX1 };

	float x,y,z,w;
	float tu,tv;
};

struct VerSkinC
{
	VerSkinC(){ b1=b2=b3=0;}

	enum{ Format=D3DFVF_XYZB4|D3DFVF_LASTBETA_UBYTE4|D3DFVF_DIFFUSE };

	CVector3f p;
	float b1,b2,b3;
	DWORD Index;
	DWORD diffuse;
};

struct VerSkinNCT
{
	VerSkinNCT(){ b1=b2=b3=0; memset( Index, 0, 4 ); diffuse=0xff000000; p = np = CVector3f(0,0,0); tu=tv=tu1=tv1=tu2=tv2=0.0;}

	enum{ Format=D3DFVF_XYZB4|D3DFVF_LASTBETA_UBYTE4|D3DFVF_NORMAL|D3DFVF_TEX1|D3DFVF_DIFFUSE };

	CVector3f p;
	float b1,b2,b3;
	BYTE Index[4];
	CVector3f np;
	DWORD diffuse;
	float tu,tv;
	float tu1,tv1;
	float tu2,tv2;
};

struct VerNCT
{
	VerNCT(){ diffuse=0xff000000; p = np = CVector3f(0,0,0); }
	VerNCT(float sx,float sy,float sz,float snx,float sny,float snz,DWORD dif,float stu,float stv)
	{
		p.x=sx; p.y=sy; p.z=sz; np.x=snx; np.y=sny; np.z=snz; diffuse=dif; tu=stu; tv=stv;
	};

	enum{ Format=D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1 };

	CVector3f p;
	CVector3f np;
	DWORD diffuse;
	float tu,tv;
};

struct VerTex
{
	VerTex(){ x=0; y=0; z=0; tu=0; tv=0; }
	VerTex(float sx,float sy,float sz,float stu,float stv)
	{
		x=sx; y=sy; z=sz; tu=stu; tv=stv;
	};

	enum{ Format=D3DFVF_XYZ|D3DFVF_TEX1 };

	float x,y,z;
	float tu,tv;
};

struct VerColorTex
{
	VerColorTex(){ x=0; y=0; z=0; tu=0; diffuse=0xff000000; tv=0; }
	VerColorTex(float sx,float sy,float sz,DWORD color, float stu,float stv)
	{
		x=sx; y=sy; z=sz; diffuse=color; tu=stu; tv=stv;
	};

	enum{ Format=D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1 };

	float x,y,z;
	DWORD diffuse;
	float tu,tv;
};

struct UpdateRes
{
	virtual LRESULT RestoreRes()=0;
	virtual LRESULT ReleaseRes()=0;
};

class CDXSize : public tagSIZE
{
public:
	CDXSize(int initCX, int initCY){ cx = initCX; cy = initCY; };
};

class CDXPoint : public tagPOINT
{
public:
	CDXPoint(){};
	CDXPoint(int initX, int initY){ x = initX; y = initY; };
};

class CDXRect : public tagRECT
{
public:
	CDXRect(){};
	CDXRect( int l, int t, int r, int b ){ left = l; top = t; right = r; bottom = b; };
	int Width(){ return right - left; }
	int Height(){ return bottom - top; }
};

struct KEYSTATE
{
	BYTE PreBuf[256];
	BYTE CurBuf[256];

	BOOL IsKeyDown(BYTE DIKey) const { return (CurBuf[DIKey]&0x80)&&(!(PreBuf[DIKey]&0x80)); }
	BOOL IsKeyUp(BYTE DIKey)   const { return (PreBuf[DIKey]&0x80)&&(!(CurBuf[DIKey]&0x80)); }
	BOOL KeyState(BYTE DIKey)  const { return CurBuf[DIKey]&0x80; }
};

struct MOUSESTATE
{
	DIMOUSESTATE2 PreState;
	DIMOUSESTATE2 CurState;

	BOOL LButtonState()   const { return CurState.rgbButtons[0]&0x80; }
	BOOL IsLButtonDown()  const { return (CurState.rgbButtons[0]&0x80)&&(!(PreState.rgbButtons[0]&0x80)); }
	BOOL IsLButtonUp()    const { return (PreState.rgbButtons[0]&0x80)&&(!(CurState.rgbButtons[0]&0x80)); }
	BOOL IsRButtonDown()  const { return (CurState.rgbButtons[1]&0x80)&&(!(PreState.rgbButtons[1]&0x80)); }
	BOOL IsRButtonUp()    const { return (PreState.rgbButtons[1]&0x80)&&(!(CurState.rgbButtons[1]&0x80)); }
	BOOL IsButtonChange() const { return (*((INT64*)CurState.rgbButtons))!=(*((INT64*)PreState.rgbButtons)); }
};

//========================================================================
// CTinyQuaternion
//========================================================================
//typedef DWORD uint32;
//typedef WORD uint16;
//typedef BYTE uint8;
//typedef CVector2f CVector2f;

//typedef int int32;

//struct CVector3f : public CVector3f
//{
//	CVector3f(){}
//	CVector3f( float x, float y, float z ):CVector3f( x, y, z ){}
//	CVector3f( CVector3f v ):CVector3f( v ){}
//	void Normalize()
//	{
//		D3DXVec3Normalize( this, this );
//	}
//	// 矢量的长度
//	float Mag() const
//	{
//		return sqrtf( x*x + y*y + z*z );
//	}
//};

//template<class T>
//T limit4(const T& a, const T& max)
//{
//	return a > max ? max : a;
//}

//struct _CovData
//{
//	enum ECD
//	{
//		eCD_MAX_YAW = 640,
//		eCD_MAX_BUF = 261396,
//		eCD_WGT_MSK = 0x7ff,
//		eCD_MAX_PTC = eCD_WGT_MSK + 1,
//		eCD_VEC_BIT = 14,
//		eCD_VEC_SGX = 0x2000,
//		eCD_VEC_SGY = 0x1000,
//		eCD_VEC_SGZ = 0x800,
//
//		eCD_NOR_YAW = 27,
//		eCD_NOR_BUF = 512,
//		eCD_NOR_SGX = 0x1,
//		eCD_NOR_SGY = 0x2,
//		eCD_NOR_SGZ = 0x4,
//	};
//
//	CVector3f	m_fVecInd[eCD_MAX_BUF];
//	uint32		m_uIndStr[eCD_MAX_YAW+1];
//	uint32		m_uIndNum[eCD_MAX_YAW+1];
//	float		m_fWgtCos[eCD_MAX_PTC];
//	float		m_fWgtSin[eCD_MAX_PTC];
//	CVector3f	m_fNorInd[eCD_NOR_BUF*8];
//	uint32		m_uNorStr[eCD_NOR_YAW+1];
//	uint32		m_uNorNum[eCD_NOR_YAW+1];
//
//	_CovData();
//	static _CovData _s;
//};

class CPluginTinyQuaternion
{
public:
	uint32 _q;

	CPluginTinyQuaternion(){}

	CPluginTinyQuaternion( const D3DXQUATERNION& q )
	{
		_CovData& s = _CovData::_s;

		double w = acos( q.w );
		_q = (uint32)( w*_CovData::eCD_WGT_MSK/MATH_PI );
		if( _q )
		{
			double sin_w = sin( w );
			double x = abs( q.x/sin_w );
			double y = abs( q.y/sin_w );
			double z = abs( q.z/sin_w );
			double a = acos( limit4( y, 1.0 ) );
			uint32 uYaw = (uint32)( a*_CovData::eCD_MAX_YAW/( MATH_PI*0.5 ) + 0.5 );
			uint32 uIndex = s.m_uIndStr[uYaw];
			if( uYaw )
			{
				double b = z/sin( a );
				double c = asin( limit4( b, 1.0 ) );
				uint32 p = limit4( (uint32)( c*( s.m_uIndNum[uYaw] - 1 )/( MATH_PI*0.5 ) + 0.5 ), s.m_uIndNum[uYaw] - 1 );						
				uIndex   = uIndex + p;
			}
			_q |= uIndex<<_CovData::eCD_VEC_BIT;

			if( q.x < 0 )_q |= _CovData::eCD_VEC_SGX;
			if( q.y < 0 )_q |= _CovData::eCD_VEC_SGY;
			if( q.z < 0 )_q |= _CovData::eCD_VEC_SGZ;
		}

	}

	operator D3DXQUATERNION() const
	{
		_CovData& s = _CovData::_s;

		uint32 uWghInd = _q&_CovData::eCD_WGT_MSK;
		float* pVector = s.m_fVecInd[_q>>_CovData::eCD_VEC_BIT];
		float  fWgtSin = s.m_fWgtSin[uWghInd];
		float  fWgtCos = s.m_fWgtCos[uWghInd];
		float  xVector = _CovData::eCD_VEC_SGX&_q ? -pVector[0]*fWgtSin : pVector[0]*fWgtSin;
		float  yVector = _CovData::eCD_VEC_SGY&_q ? -pVector[1]*fWgtSin : pVector[1]*fWgtSin;
		float  zVector = _CovData::eCD_VEC_SGZ&_q ? -pVector[2]*fWgtSin : pVector[2]*fWgtSin;
		return D3DXQUATERNION( xVector, yVector, zVector, fWgtCos );
	}
};

//class CTinyVector
//{
//public:
//	uint32 _v;
//
//	CTinyVector(){};
//	CTinyVector( const CVector3f& v, float f )
//	{
//		_CovData& s = _CovData::_s;
//
//		float fLenght = D3DXVec3Length( &v );
//		_v = uint32(fLenght*f);
//		if( _v )
//		{
//			double x = abs( v.x/fLenght );
//			double y = abs( v.y/fLenght );
//			double z = abs( v.z/fLenght );
//			double a = acos( limit4( y, 1.0 ) );
//			uint32 uYaw = (uint32)( a*_CovData::eCD_MAX_YAW/( MATH_PI*0.5 ) + 0.5 );
//			uint32 uIndex = s.m_uIndStr[uYaw];
//			if( uYaw )
//			{
//				double b = z/sin( a );
//				double c = asin( limit4( b, 1.0 ) );
//				uint32 p = limit4( (uint32)( c*( s.m_uIndNum[uYaw] - 1 )/( MATH_PI*0.5 ) + 0.5 ), s.m_uIndNum[uYaw] - 1 );						
//				uIndex   = uIndex + p;
//			}
//			_v |= uIndex<<_CovData::eCD_VEC_BIT;
//
//			if( v.x < 0 )_v |= _CovData::eCD_VEC_SGX;
//			if( v.y < 0 )_v |= _CovData::eCD_VEC_SGY;
//			if( v.z < 0 )_v |= _CovData::eCD_VEC_SGZ;
//		}
//	}
//
//	CVector3f Get( float f ) const
//	{
//		_CovData& s = _CovData::_s;
//		uint32 uWghInd = 0;
//		uWghInd	= _v&_CovData::eCD_WGT_MSK;
//		float* pVector = s.m_fVecInd[_v>>_CovData::eCD_VEC_BIT];
//		float  fLenght = 0.0f;
//		fLenght = fLenght*uWghInd;
//		float  xVector = _CovData::eCD_VEC_SGX&_v ? -pVector[0]*fLenght : pVector[0]*fLenght;
//		float  yVector = _CovData::eCD_VEC_SGY&_v ? -pVector[1]*fLenght : pVector[1]*fLenght;
//		float  zVector = _CovData::eCD_VEC_SGZ&_v ? -pVector[2]*fLenght : pVector[2]*fLenght;
//		return CVector3f( xVector, yVector, zVector );
//	}
//};

//class CTinyWeight
//{
//	uint8 _w[4];
//public:
//	CTinyWeight( float w0, float w1, float w2, float w3 )
//	{
//		_w[0] = uint8(w0*200);
//		_w[1] = uint8(w1*200);
//		_w[2] = uint8(w2*200);
//		_w[3] = uint8(w3*200);
//
//		int n = 0;
//		uint8 max = 0;
//		int32 sum = 0;
//		for( int i = 0; i < 4; i++ )
//		{
//			if( _w[i] > max )
//				n = i;
//			sum += _w[i];
//		}
//
//		if( sum && sum != 200 )
//		{
//			sum = _w[n] + 200 - sum;
//			if( sum > 200 || sum < 0 )
//				throw "定点权重错误";
//			_w[n] = (uint8)sum;
//		}
//	}
//
//	float GetWeight( uint32 nIndex ) { return _w[nIndex]*0.005f;}
//};

template< int pb = 10, int tb = 11 ,int mp = 1>//pb是一个顶点中一个分量的缓冲大小,tb是纹理坐标一个分量的大小
class CTinyVetex
{
	enum { nb = 12, nSize = pb*3 + nb + tb*2*mp, };//nb是法线的缓冲大小,这三个决定了buf大小

	uint16	_GetBit( int pos, int Num )
	{
		uint32 n = *( (uint32*)( _Buf + (pos>>3) ) );
		n = n >> (pos&0x07);
		n = n & ( ( 1 << Num ) - 1 );

		return (uint16)n;
	}

	void	_SetBit( int pos, int Num, uint16 v )//设置这个buf中具体到位的运算,虽然是1个字节,但是这里的pos参数是直接到位的操作
	{
		uint32  m = ( ( 1 << Num ) - 1 ) << (pos&0x07);
		uint32& n = *( (uint32*)( _Buf + (pos>>3) ) );//是buf中的第几个字节首位
		n = ( n&(~m) )|( ( ( (uint32)v ) << (pos&0x07) )&m );
	}
public:
	TCHAR	_Buf[ ( nSize&7 ) ? nSize/8 + 1 : nSize/8 ];//把一个顶点的所有东西都压入一个以1个字节为单位的buf中

	CTinyVetex(){};
	CTinyVetex( CVector3f v, CVector3f n, CVector2f t )
	{
		_SetBit( pb*0, pb,	uint16( v.x*4 + 0.5f ) );
		_SetBit( pb*1, pb,	uint16( v.y*4 + 0.5f ) );
		_SetBit( pb*2, pb,	uint16( v.z*4 + 0.5f ) );

		_CovData& s = _CovData::_s;
		CVector3f vNor = n;
		vNor.Normalize();
		double x = abs( vNor.x );
		double y = abs( vNor.y );
		double z = abs( vNor.z );
		double a = acos( limit4( y, 1.0 ) );
		uint32 uYaw = (uint32)( a*_CovData::eCD_NOR_YAW/( MATH_PI*0.5 ) + 0.5 );
		uint32 uIndex = s.m_uNorStr[uYaw];
		if( uYaw )
		{
			double b = z/sin( a );
			double c = asin( limit4( b, 1.0 ) );
			uint32 p = limit4( (uint32)( c*( s.m_uNorNum[uYaw] - 1 )/( MATH_PI*0.5 ) + 0.5 ), s.m_uNorNum[uYaw] - 1 );						
			uIndex   = uIndex + p;
		}

		uint16 _n = 0;
		if( n.x < 0 )_n |= _CovData::eCD_NOR_SGX;
		if( n.y < 0 )_n |= _CovData::eCD_NOR_SGY;
		if( n.z < 0 )_n |= _CovData::eCD_NOR_SGZ;
		_n = _n*_CovData::eCD_NOR_BUF + (uint16)uIndex;
		_SetBit( pb*3, nb, _n );
		_SetBit( pb*3 + nb, tb, uint16( t.x*2000 + 0.5f ) );
		_SetBit( pb*3 + nb + tb, tb, uint16( t.y*2000 + 0.5f ) );
	}
	CTinyVetex( CVector3f v, CVector3f n, CVector2f t ,CVector2f t1)
	{
		_SetBit( pb*0, pb,	uint16( v.x*4 + 0.5f ) );
		_SetBit( pb*1, pb,	uint16( v.y*4 + 0.5f ) );
		_SetBit( pb*2, pb,	uint16( v.z*4 + 0.5f ) );

		_CovData& s = _CovData::_s;
		CVector3f vNor = n;
		vNor.Normalize();
		double x = abs( vNor.x );
		double y = abs( vNor.y );
		double z = abs( vNor.z );
		double a = acos( limit4( y, 1.0 ) );
		uint32 uYaw = (uint32)( a*_CovData::eCD_NOR_YAW/( MATH_PI*0.5 ) + 0.5 );
		uint32 uIndex = s.m_uNorStr[uYaw];
		if( uYaw )
		{
			double b = z/sin( a );
			double c = asin( limit4( b, 1.0 ) );
			uint32 p = limit4( (uint32)( c*( s.m_uNorNum[uYaw] - 1 )/( MATH_PI*0.5 ) + 0.5 ), s.m_uNorNum[uYaw] - 1 );						
			uIndex   = uIndex + p;
		}

		uint16 _n = 0;
		if( n.x < 0 )_n |= _CovData::eCD_NOR_SGX;
		if( n.y < 0 )_n |= _CovData::eCD_NOR_SGY;
		if( n.z < 0 )_n |= _CovData::eCD_NOR_SGZ;
		_n = _n*_CovData::eCD_NOR_BUF + (uint16)uIndex;
		_SetBit( pb*3, nb, _n );

		_SetBit( pb*3 + nb, tb, uint16( t.x*2000 + 0.5f ) );
		_SetBit( pb*3 + nb + tb, tb, uint16( t.y*2000 + 0.5f ) );

		_SetBit( pb*3 + nb +tb*2,tb,uint16(t1.x*2000+0.5f));
		_SetBit( pb*3 + nb +tb*3,tb,uint16(t1.y*2000+0.5f));
	}
	CTinyVetex( CVector3f v, CVector3f n, CVector2f t ,CVector2f t1,CVector2f t2)
	{
		_SetBit( pb*0, pb,	uint16( v.x*4 + 0.5f ) );
		_SetBit( pb*1, pb,	uint16( v.y*4 + 0.5f ) );
		_SetBit( pb*2, pb,	uint16( v.z*4 + 0.5f ) );

		_CovData& s = _CovData::_s;
		CVector3f vNor = n;
		vNor.Normalize();
		double x = abs( vNor.x );
		double y = abs( vNor.y );
		double z = abs( vNor.z );
		double a = acos( limit4( y, 1.0 ) );
		uint32 uYaw = (uint32)( a*_CovData::eCD_NOR_YAW/( MATH_PI*0.5 ) + 0.5 );
		uint32 uIndex = s.m_uNorStr[uYaw];
		if( uYaw )
		{
			double b = z/sin( a );
			double c = asin( limit4( b, 1.0 ) );
			uint32 p = limit4( (uint32)( c*( s.m_uNorNum[uYaw] - 1 )/( MATH_PI*0.5 ) + 0.5 ), s.m_uNorNum[uYaw] - 1 );						
			uIndex   = uIndex + p;
		}

		uint16 _n = 0;
		if( n.x < 0 )_n |= _CovData::eCD_NOR_SGX;
		if( n.y < 0 )_n |= _CovData::eCD_NOR_SGY;
		if( n.z < 0 )_n |= _CovData::eCD_NOR_SGZ;
		_n = _n*_CovData::eCD_NOR_BUF + (uint16)uIndex;
		_SetBit( pb*3, nb, _n );
		_SetBit( pb*3 + nb, tb, uint16( t.x*2000 + 0.5f ) );
		_SetBit( pb*3 + nb + tb, tb, uint16( t.y*2000 + 0.5f ) );

		_SetBit( pb*3 + nb +tb*2,tb,uint16(t1.x*2000+0.5f));
		_SetBit( pb*3 + nb +tb*3,tb,uint16(t1.y*2000+0.5f));

		_SetBit( pb*3 + nb +tb*4,tb,uint16(t2.x*2000+0.5f));
		_SetBit( pb*3 + nb +tb*5,tb,uint16(t2.y*2000+0.5f));
	}

	uint32    GetS(){ return ( pb*3 + nb + tb*2 )/8; }
	CVector3f GetV(){ return CVector3f( _GetBit(  0, pb )*0.25f, _GetBit( pb, pb )*0.25f, _GetBit( pb*2, pb )*0.25f ); }
	CVector3f GetN(){ return _CovData::_s.m_fNorInd[ _GetBit( pb*3, nb ) ]; }
	CVector2f GetT(){ return CVector2f( _GetBit( pb*3 + nb, tb )*0.0005f, _GetBit( pb*3 + nb + tb, tb )*0.0005f ); }
	CVector2f GetT1(){ return CVector2f( _GetBit( pb*3 + nb + tb*2 ,tb )*0.0005f,_GetBit( pb*3 + nb + tb*3,tb)*0.0005f ); }
	CVector2f GetT2(){ return CVector2f( _GetBit( pb*3 + nb + tb*4 ,tb )*0.0005f,_GetBit( pb*3 + nb + tb*5,tb)*0.0005f ); }
};

#endif // !defined(AFX_GLOBD3D_H__97BB9FA2_A00E_48EF_AFE6_6F5A5B101D8C__INCLUDED_)
