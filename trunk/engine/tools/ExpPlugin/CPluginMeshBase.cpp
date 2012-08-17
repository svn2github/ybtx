#include "stdafx.h"
#include "model.h"
// 临时修改，解决编译不过问题
//#include "ParaAdapterCtrl\CModRsTexPathCfgMgr.h"
#include "CHardwareBufferMgr.h"

#include "stdmat.h"
#include "Phyexp.h"
#include "Bipexp.h"
#include "decomp.h"
#include "ISkin.h"
#include "iiksys.h"
#include "BoneFix.h"
#include "SoftClothDlg.h"
#include "PropSelectDlg.h"
#include "cfourcc.h"
#include "GraphicBase.h"
#include "CEditPieceGroup.h"
#include "CPluginMeshBase.h"
#include <Windows.h>

#define	  DoubleUV
#define   MINIMUMWEIGHT    0.0001f

extern SAVEPARAM			g_SaveParam;
extern float				g_fCurNode;
extern bool					g_bIsExistModFile;
extern BOOL					g_IsCpSke;
extern vector<BoxNode>		g_vecNode;

int  FaceIndex[2][3]={ { 0, 1, 2 },{ 0, 2, 1, } };

enum ESoftType 
{
	SoftPlane,    //平面状柔体:绳子 ＋ 一片
	SoftCross,    //交叉状柔体：绳子 ＋ 两片
};


struct WeightOfBone
{
	WeightOfBone( float f, BYTE ID ) : m_f(f), m_ID(ID){}
	float m_f;
	BYTE  m_ID;

	bool operator< ( const WeightOfBone& a )const { return m_f < a.m_f; }
};



struct ClothFace
{   
	ClothFace( int v1, int v2, int v3 )
	{
		VerIndex0 = v1;
		VerIndex1 = v2;
		VerIndex2 = v3;
	}

	int InFace( int v)
	{
		if (VerIndex0 == v) return 0;
		if (VerIndex1 == v) return 1;
		if (VerIndex2 == v) return 2;
		return -1;
	}

	void GetOtherTwo( int v0, int& v1, int& v2)
	{
		switch(v0)
		{
		case 0:
			{
				v1 = VerIndex1;
				v2 = VerIndex2;
			}
			break;
		case 1:
			{
				v1 = VerIndex0;
				v2 = VerIndex2;
			}
			break;
		case 2:
			{
				v1 = VerIndex0;
				v2 = VerIndex1;
			}
			break;
		}
	}

	int VerIndex0;
	int VerIndex1;
	int VerIndex2;
};

struct VertextFace
{
	VertextFace():nNum(0),nNewIndex(-1){}
	int nNewIndex;
	int nNum;
	int nFace[4];
};

struct NodePhyPara
{
	float      m_fAccGravity;     //重力加速度（牛顿/千克）
	float      m_fMass;           //每个质点的质量
	float      m_fCoeDamp;        //阻力系数
};

////////////////////////////////////////////////////////////////////////
// 功能：将3DMAX矩阵转换成CMatrix
// 参数：const Matrix3& original 3DMAX矩阵
// 返回：转换后的CMatrix
////////////////////////////////////////////////////////////////////////// 
const CMatrix Matrix3ToD3DXMatrix( const Matrix3& original, BOOL Scale )
{
	CMatrix result;

	if( Scale )
	{
		//将Y和Z交换
		result.Identity();
		//D3DXMatrixIdentity( &result );
		result.m[0][0] = original.GetRow(0).x;
		result.m[0][1] = original.GetRow(0).z;
		result.m[0][2] = original.GetRow(0).y;
		result.m[1][0] = original.GetRow(2).x;
		result.m[1][1] = original.GetRow(2).z;
		result.m[1][2] = original.GetRow(2).y;
		result.m[2][0] = original.GetRow(1).x;
		result.m[2][1] = original.GetRow(1).z;
		result.m[2][2] = original.GetRow(1).y;
		result.m[3][0] = original.GetRow(3).x;
		result.m[3][1] = original.GetRow(3).z;
		result.m[3][2] = original.GetRow(3).y;
	}
	else
	{
		// 分解
		AffineParts affineParts;
		decomp_affine( original, &affineParts );

		// 四元数旋转
		D3DXQUATERNION Quat( affineParts.q.x, affineParts.q.z, affineParts.q.y, affineParts.q.w );
		D3DXMatrixRotationQuaternion( (D3DXMATRIX*)&result, &Quat );

		// 平移
		result.m[3][0]= affineParts.t.x;
		result.m[3][1]= affineParts.t.z;
		result.m[3][2]= affineParts.t.y;
	}

	return result;
}

////////////////////////////////////////////////////////////////////////
// 功能：得到指定节点的全局矩阵
// 参数：INode* MaxNode 3DMAX节点,
//       int Time       时间
// 返回：指定节点的CMatrix
////////////////////////////////////////////////////////////////////////// 
const CMatrix GetNodeGlobeMatrix( INode* MaxNode, int Time, BOOL Scale )
{
	return Matrix3ToD3DXMatrix( MaxNode->GetObjTMBeforeWSM( Time ), Scale );
}

////////////////////////////////////////////////////////////////////////
// 功能：得到指定节点的局部矩阵
// 参数：INode* MaxNode 3DMAX节点,
//       int Time       时间
// 返回：指定节点的CMatrix
////////////////////////////////////////////////////////////////////////// 
const CMatrix GetNodeLocalMatrix( INode* MaxNode, int Time, BOOL Scale )
{
	CMatrix CurNodeMat = GetNodeGlobeMatrix( MaxNode, Time, Scale );
	if( INode* Parent = MaxNode->GetParentNode() )
	{
		CMatrix ParentMat = GetNodeGlobeMatrix( Parent, Time, Scale );
		ParentMat.Invert();
		//D3DXMatrixInverse( &ParentMat, NULL, &ParentMat );
		CurNodeMat = CurNodeMat*ParentMat;
	}
	return CurNodeMat;
}

BOOL Parity( const CMatrix& original )
{
	if ( D3DXMatrixIsIdentity( (D3DXMATRIX*)&original ) ) 
		return FALSE;

	CVector3f X( original.m[0] );
	CVector3f Y( original.m[1] );
	CVector3f Z( original.m[2] );
	CVector3f A;
	A = X.Cross(Y);
	if(A.Dot(Z)>0)
		return FALSE;

	return TRUE;
}
bool LoadVetex( CVector3f& v, CVector3f& n, CVector2f& t, SaveInfo& SI, ifstream& File )
{
	if( SI.sPos < 1024 && SI.sTex <= 1.02f )
	{
		CTinyVetex<10,11> Vetex;
		File.read( Vetex._Buf, sizeof( Vetex._Buf ) );
		v = Vetex.GetV();
		n = Vetex.GetN();
		t = Vetex.GetT();
	}
	else if( SI.sPos < 4096 && SI.sTex <= 2.04f )
	{
		CTinyVetex<12,12> Vetex;
		File.read( Vetex._Buf, sizeof( Vetex._Buf ) );
		v = Vetex.GetV();
		n = Vetex.GetN();
		t = Vetex.GetT();
	}

	else if( SI.sPos < 4096 && SI.sTex < 32.7f )
	{
		CTinyVetex<12,16> Vetex;
		File.read( Vetex._Buf, sizeof( Vetex._Buf ) );
		v = Vetex.GetV();
		n = Vetex.GetN();
		t = Vetex.GetT();
	}
	else if( SI.sPos < 16384 && SI.sTex < 32.7f )
	{
		CTinyVetex<14,16> Vetex;
		File.read( Vetex._Buf, sizeof( Vetex._Buf ) );
		v = Vetex.GetV();
		n = Vetex.GetN();
		t = Vetex.GetT();
	}
	else if( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		CTinyVetex<16,16> Vetex;
		File.read( Vetex._Buf, sizeof( Vetex._Buf ) );
		v = Vetex.GetV();
		n = Vetex.GetN();
		t = Vetex.GetT();
	}
	else
		return false;

	v.x += SI.xStr*0.25f;
	v.y += SI.yStr*0.25f;
	v.z += SI.zStr*0.25f;

	return true;
}

bool LoadVetex( CVector3f& v, CVector3f& n, CVector2f& t,CVector2f& t1, SaveInfo& SI, ifstream& File )
{
	if( SI.sPos < 1024 && SI.sTex <= 1.02f )
	{
		CTinyVetex<10,11,2> Vetex;
		File.read( Vetex._Buf, sizeof( Vetex._Buf ) );
		v = Vetex.GetV();
		n = Vetex.GetN();
		t = Vetex.GetT();
		t1 = Vetex.GetT1();
	}
	else if( SI.sPos < 4096 && SI.sTex <= 2.04f )
	{
		CTinyVetex<12,12,2> Vetex;
		File.read( Vetex._Buf, sizeof( Vetex._Buf ) );
		v = Vetex.GetV();   
		n = Vetex.GetN();
		t = Vetex.GetT();
		t1 = Vetex.GetT1();
	}
	else if( SI.sPos < 4096 && SI.sTex < 32.7f )
	{
		CTinyVetex<12,16,2> Vetex;
		File.read( Vetex._Buf, sizeof( Vetex._Buf ) );
		v = Vetex.GetV();
		n = Vetex.GetN();
		t = Vetex.GetT();
		t1 = Vetex.GetT1();
	}
	else if( SI.sPos < 16384 && SI.sTex < 32.7f )
	{
		CTinyVetex<14,16,2> Vetex;
		File.read( Vetex._Buf, sizeof( Vetex._Buf ) );
		v = Vetex.GetV();
		n = Vetex.GetN();
		t = Vetex.GetT();
		t1 = Vetex.GetT1();
	}
	else if( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		CTinyVetex<16,16,2> Vetex;
		File.read( Vetex._Buf, sizeof( Vetex._Buf ) );
		v = Vetex.GetV();
		n = Vetex.GetN();
		t = Vetex.GetT();
		t1 = Vetex.GetT1();
	}
	else
		return false;

	v.x += SI.xStr*0.25f;
	v.y += SI.yStr*0.25f;
	v.z += SI.zStr*0.25f;


	return true;
}

bool LoadVetex( CVector3f& v, CVector3f& n, CVector2f& t,CVector2f& t1,CVector2f& t2, SaveInfo& SI, ifstream& File )
{
	if( SI.sPos < 1024 && SI.sTex <= 1.02f )
	{
		CTinyVetex<10,11,3> Vetex;
		File.read( Vetex._Buf, sizeof( Vetex._Buf ) );
		v = Vetex.GetV();
		n = Vetex.GetN();
		t = Vetex.GetT();
		t1 = Vetex.GetT1();
		t2 = Vetex.GetT2();
	}
	else if( SI.sPos < 4096 && SI.sTex <= 2.04f )
	{
		CTinyVetex<12,12,3> Vetex;
		File.read( Vetex._Buf, sizeof( Vetex._Buf ) );
		v = Vetex.GetV();
		n = Vetex.GetN();
		t = Vetex.GetT();
		t1 = Vetex.GetT1();
		t2 = Vetex.GetT2();
	}
	else if( SI.sPos < 4096 && SI.sTex < 32.7f )
	{
		CTinyVetex<12,16,3> Vetex;
		File.read( Vetex._Buf, sizeof( Vetex._Buf ) );
		v = Vetex.GetV();
		n = Vetex.GetN();
		t = Vetex.GetT();
		t1 = Vetex.GetT1();
		t2 = Vetex.GetT2();
	}
	else if( SI.sPos < 16384 && SI.sTex < 32.7f )
	{
		CTinyVetex<14,16,3> Vetex;
		File.read( Vetex._Buf, sizeof( Vetex._Buf ) );
		v = Vetex.GetV();
		n = Vetex.GetN();
		t = Vetex.GetT();
		t1 = Vetex.GetT1();
		t2 = Vetex.GetT2();
	}
	else if( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		CTinyVetex<16,16,3> Vetex;
		File.read( Vetex._Buf, sizeof( Vetex._Buf ) );
		v = Vetex.GetV();
		n = Vetex.GetN();
		t = Vetex.GetT();
		t1 = Vetex.GetT1();
		t2 = Vetex.GetT2();
	}
	else
		return false;

	v.x += SI.xStr*0.25f;
	v.y += SI.yStr*0.25f;
	v.z += SI.zStr*0.25f;

	return true;
}

bool LoadVetex( CVector3f& v, CVector3f& n, SaveInfo& SI, ifstream& File )
{
	if( SI.sPos < 1024 && SI.sTex <= 1.02f )
	{
		CTinyVetex<10,11> Vetex;
		File.read( Vetex._Buf, sizeof( Vetex._Buf ) - 11*2/8 );
		v = Vetex.GetV();
		n = Vetex.GetN();
	}
	else if( SI.sPos < 4096 && SI.sTex <= 2.04f )
	{
		CTinyVetex<12,12> Vetex;
		File.read( Vetex._Buf, sizeof( Vetex._Buf ) - 12*2/8 );
		v = Vetex.GetV();
		n = Vetex.GetN();
	}
	else if( SI.sPos < 4096 && SI.sTex < 32.7f )
	{
		CTinyVetex<12,16> Vetex;
		File.read( Vetex._Buf, sizeof( Vetex._Buf ) - 16*2/8 );
		v = Vetex.GetV();
		n = Vetex.GetN();
	}
	else if( SI.sPos < 16384 && SI.sTex < 32.7f )
	{
		CTinyVetex<14,16> Vetex;
		File.read( Vetex._Buf, sizeof( Vetex._Buf ) - 16*2/8  );
		v = Vetex.GetV();
		n = Vetex.GetN();
	}
	else if( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		CTinyVetex<16,16> Vetex;
		File.read( Vetex._Buf, sizeof( Vetex._Buf ) - 16*2/8 );
		v = Vetex.GetV();
		n = Vetex.GetN();
	}
	else
		return false;

	v.x += SI.xStr*0.25f;
	v.y += SI.yStr*0.25f;
	v.z += SI.zStr*0.25f;

	return true;
}

bool LoadVetexFull( CVector3f& v, CVector3f& n, CVector2f& t, SaveInfo& SI, ifstream& File )
{
	if( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		File.read( (char*)&v, sizeof( v ) );
		File.read( (char*)&n, sizeof( n ) );
		File.read( (char*)&t, sizeof( t ) );
		return true;
	}
	else
		return false;
}

bool LoadVetexFull( CVector3f& v, CVector3f& n, CVector2f& t,CVector2f& t1, SaveInfo& SI, ifstream& File )
{
	if( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		File.read( (char*)&v, sizeof( v ) );
		File.read( (char*)&n, sizeof( n ) );
		File.read( (char*)&t, sizeof( t ) );
		File.read( (char*)&t1,sizeof( t1) );
		return true;
	}
	else
		return false;
}

bool LoadVetexFull( CVector3f& v, CVector3f& n, CVector2f& t,CVector2f& t1,CVector2f& t2, SaveInfo& SI, ifstream& File )
{
	if( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		File.read( (char*)&v, sizeof( v ) );
		File.read( (char*)&n, sizeof( n ) );
		File.read( (char*)&t, sizeof( t ) );
		File.read( (char*)&t1,sizeof( t1) );
		File.read( (char*)&t2,sizeof( t2) );
		return true;
	}
	else
		return false;
}
bool LoadVetexFull( CVector3f& v, CVector3f& n, SaveInfo& SI, ifstream& File )
{
	if( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		File.read( (char*)&v, sizeof( v ) );
		File.read( (char*)&n, sizeof( n ) );
		return true;
	}
	else
		return false;
}

//struct SaveInfo
//{
//	SaveInfo( CVector3f Scale, CVector3f Off, CVector2f MinUV, CVector2f MaxUV )
//	{
//		int xOff = (int32)( Off.x*4 + 0.5f );
//		int yOff = (int32)( Off.y*4 + 0.5f );
//		int zOff = (int32)( Off.z*4 + 0.5f );
//		int xScl = (int32)( Scale.x*4 + 0.5f );
//		int yScl = (int32)( Scale.y*4 + 0.5f );
//		int zScl = (int32)( Scale.z*4 + 0.5f );
//
//		sPos = 0;
//		sPos = max( sPos, abs( xScl ) );
//		sPos = max( sPos, abs( yScl ) );
//		sPos = max( sPos, abs( zScl ) );
//
//		xStr = xOff - xScl/2;
//		yStr = yOff - yScl/2;
//		zStr = zOff - zScl/2;
//
//		uStr = MinUV.x >= 0 ? (int)abs( MinUV.x ) : -(int)abs( MinUV.x );
//		if( uStr > MinUV.x )
//			uStr--;
//		vStr = MinUV.y >= 0 ? (int)abs( MinUV.y ) : -(int)abs( MinUV.y );
//		if( vStr > MinUV.y )
//			vStr--;
//
//		sTex = 0;
//		sTex = max( sTex, abs( MaxUV.x - uStr ) );
//		sTex = max( sTex, abs( MaxUV.y - vStr ) );
//	}
//	SaveInfo( CVector3f Scale, CVector3f Off, float fTextScale, VERTEXTYPE eType ) : eVetexType( eType )
//	{
//		short xOff = (short)( Off.x*4 + 0.5f );
//		short yOff = (short)( Off.y*4 + 0.5f );
//		short zOff = (short)( Off.z*4 + 0.5f );
//		short xScl = (short)( Scale.x*4 + 0.5f );
//		short yScl = (short)( Scale.y*4 + 0.5f );
//		short zScl = (short)( Scale.z*4 + 0.5f );
//
//		sPos = 0;
//		sPos = max( sPos, abs( xScl ) );
//		sPos = max( sPos, abs( yScl ) );
//		sPos = max( sPos, abs( zScl ) );
//
//		xStr = xOff - xScl/2;
//		yStr = yOff - yScl/2;
//		zStr = zOff - zScl/2;
//		sTex = fTextScale;
//	}
//
//	VERTEXTYPE eVetexType;
//
//	int xStr;
//	int yStr;
//	int zStr;
//	int sPos;
//
//	int uStr;
//	int vStr;
//	float sTex;
//};



bool PluginSaveVetex( CVector3f v, CVector3f n, CVector2f t, SaveInfo& SI, ofstream& File )
{
	int x = int( v.x*4 + 0.5f ) - SI.xStr;
	int y = int( v.y*4 + 0.5f ) - SI.yStr;
	int z = int( v.z*4 + 0.5f ) - SI.zStr;
	CVector3f _v( x*0.25f, y*0.25f, z*0.25f );

	if( SI.sPos < 1024 && SI.sTex <= 1.02f )
	{
		CTinyVetex<10,11> Vetex( _v, n, CVector2f( t.x - SI.uStr, t.y - SI.vStr ) );
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else if( SI.sPos < 4096 && SI.sTex <= 2.04f )
	{
		CTinyVetex<12,12> Vetex( _v, n, CVector2f( t.x - SI.uStr, t.y - SI.vStr ) );
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else if( SI.sPos < 4096 && SI.sTex < 32.7f )
	{
		CTinyVetex<12,16> Vetex( _v, n, CVector2f( t.x - SI.uStr, t.y - SI.vStr ) );
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else if( SI.sPos < 16384 && SI.sTex < 32.7f )
	{
		CTinyVetex<14,16> Vetex( _v, n, CVector2f( t.x - SI.uStr, t.y - SI.vStr ) );
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else if( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		CTinyVetex<16,16> Vetex( _v, n, CVector2f( t.x - SI.uStr, t.y - SI.vStr ) );
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else
		throw "模型或者贴图范围超标";

	return true;
}
bool PluginSaveVetex( CVector3f v, CVector3f n, CVector2f t, CVector2f t1, SaveInfo& SI, ofstream& File )
{
	int x = int( v.x*4 + 0.5f ) - SI.xStr;
	int y = int( v.y*4 + 0.5f ) - SI.yStr;
	int z = int( v.z*4 + 0.5f ) - SI.zStr;
	CVector3f _v( x*0.25f, y*0.25f, z*0.25f );

	if( SI.sPos < 1024 && SI.sTex <= 1.02f )
	{

		CTinyVetex<10,11,2> Vetex( _v, n, CVector2f( t.x - SI.uStr, t.y - SI.vStr ) ,CVector2f( t1.x - SI.uStr, t1.y - SI.vStr ));
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else if( SI.sPos < 4096 && SI.sTex <= 2.04f )
	{
		CTinyVetex<12,12,2> Vetex( _v, n, CVector2f( t.x - SI.uStr, t.y - SI.vStr ) ,CVector2f( t1.x - SI.uStr, t1.y - SI.vStr ));
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else if( SI.sPos < 4096 && SI.sTex < 32.7f )
	{
		CTinyVetex<12,16,2> Vetex( _v, n, CVector2f( t.x - SI.uStr, t.y - SI.vStr ) ,CVector2f( t1.x - SI.uStr, t1.y - SI.vStr ));
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else if( SI.sPos < 16384 && SI.sTex < 32.7f )
	{
		CTinyVetex<14,16,2> Vetex( _v, n, CVector2f( t.x - SI.uStr, t.y - SI.vStr ) ,CVector2f( t1.x - SI.uStr, t1.y - SI.vStr ));
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else if( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		CTinyVetex<16,16,2> Vetex( _v, n, CVector2f( t.x - SI.uStr, t.y - SI.vStr ) ,CVector2f( t1.x - SI.uStr, t1.y - SI.vStr ));
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else
	{
		throw "模型或者贴图范围超标";
	}

	return true;
}
bool PluginSaveVetex( CVector3f v, CVector3f n, CVector2f t, CVector2f t1,CVector2f t2,SaveInfo& SI, ofstream& File )
{
	int x = int( v.x*4 + 0.5f ) - SI.xStr;
	int y = int( v.y*4 + 0.5f ) - SI.yStr;
	int z = int( v.z*4 + 0.5f ) - SI.zStr;
	CVector3f _v( x*0.25f, y*0.25f, z*0.25f );

	if( SI.sPos < 1024 && SI.sTex <= 1.02f )
	{
		CTinyVetex<10,11,3> Vetex( _v, n, CVector2f( t.x - SI.uStr, t.y - SI.vStr ) ,CVector2f( t1.x - SI.uStr, t1.y - SI.vStr ),CVector2f( t2.x - SI.uStr, t2.y - SI.vStr ));
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else if( SI.sPos < 4096 && SI.sTex <= 2.04f )
	{
		CTinyVetex<12,12,3> Vetex( _v, n, CVector2f( t.x - SI.uStr, t.y - SI.vStr ) ,CVector2f( t1.x - SI.uStr, t1.y - SI.vStr ),CVector2f( t2.x - SI.uStr, t2.y - SI.vStr ));
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else if( SI.sPos < 4096 && SI.sTex < 32.7f )
	{
		CTinyVetex<12,16,3> Vetex( _v, n, CVector2f( t.x - SI.uStr, t.y - SI.vStr ) ,CVector2f( t1.x - SI.uStr, t1.y - SI.vStr ),CVector2f( t2.x - SI.uStr, t2.y - SI.vStr ));
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else if( SI.sPos < 16384 && SI.sTex < 32.7f )
	{
		CTinyVetex<14,16,3> Vetex( _v, n, CVector2f( t.x - SI.uStr, t.y - SI.vStr ) ,CVector2f( t1.x - SI.uStr, t1.y - SI.vStr ),CVector2f( t2.x - SI.uStr, t2.y - SI.vStr ));
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else if( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		CTinyVetex<16,16,3> Vetex( _v, n, CVector2f( t.x - SI.uStr, t.y - SI.vStr ) ,CVector2f( t1.x - SI.uStr, t1.y - SI.vStr ),CVector2f( t2.x - SI.uStr, t2.y - SI.vStr ));
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) );
	}
	else
		throw "模型或者贴图范围超标";

	return true;
}
bool PluginSaveVetex( CVector3f v, CVector3f n, SaveInfo& SI, ofstream& File )
{
	int x = int( v.x*4 + 0.5f ) - SI.xStr;
	int y = int( v.y*4 + 0.5f ) - SI.yStr;
	int z = int( v.z*4 + 0.5f ) - SI.zStr;
	CVector3f _v( x*0.25f, y*0.25f, z*0.25f );

	if( SI.sPos < 1024 && SI.sTex <= 1.02f )
	{
		CTinyVetex<10,11> Vetex( _v, n, CVector2f( 0, 0 ) );
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) - 11*2/8 );
	}
	else if( SI.sPos < 4096 && SI.sTex <= 2.04f )
	{
		CTinyVetex<12,12> Vetex( _v, n, CVector2f( 0, 0 ) );
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) - 12*2/8 );
	}
	else if( SI.sPos < 4096 && SI.sTex < 32.7f )
	{
		CTinyVetex<12,16> Vetex( _v, n, CVector2f( 0, 0 ) );
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) - 16*2/8 );
	}
	else if( SI.sPos < 16384 && SI.sTex < 32.7f )
	{
		CTinyVetex<14,16> Vetex( _v, n, CVector2f( 0, 0 ) );
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) - 16*2/8 );
	}
	else if( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		CTinyVetex<16,16> Vetex( _v, n, CVector2f( 0, 0 ) );
		File.write( Vetex._Buf, sizeof( Vetex._Buf ) - 16*2/8 );
	}
	else
		throw "模型或者贴图范围超标";

	return true;
}

bool PluginSaveVetexFull( CVector3f v, CVector3f n, CVector2f t, SaveInfo& SI, ofstream& File )
{
	if( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		File.write( (char*)&v, sizeof( v ) );
		File.write( (char*)&n, sizeof( n ) );
		File.write( (char*)&t, sizeof( t ) );
		return true;
	}
	else
		throw "模型或者贴图范围超标";

}
bool PluginSaveVetexFull( CVector3f v, CVector3f n, CVector2f t, CVector2f t1, SaveInfo& SI, ofstream& File )
{
	if( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		File.write( (char*)&v, sizeof( v ) );
		File.write( (char*)&n, sizeof( n ) );
		File.write( (char*)&t, sizeof( t ) );
		File.write( (char*)&t1,sizeof( t1) );
		return true;
	}
	else
		throw "模型或者贴图范围超标";
}
bool PluginSaveVetexFull( CVector3f v, CVector3f n, CVector2f t, CVector2f t1,CVector2f t2,SaveInfo& SI, ofstream& File )
{
	if( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		File.write( (char*)&v, sizeof( v ) );
		File.write( (char*)&n, sizeof( n ) );
		File.write( (char*)&t, sizeof( t ) );
		File.write( (char*)&t1,sizeof( t1) );
		File.write( (char*)&t2,sizeof( t2) );
		return true;
	}
	else
		throw "模型或者贴图范围超标";
}
bool PluginSaveVetexFull( CVector3f v, CVector3f n, SaveInfo& SI, ofstream& File )
{
	if( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		File.write( (char*)&v, sizeof( v ) );
		File.write( (char*)&n, sizeof( n ) );
		return true;
	}
	else
		throw "模型或者贴图范围超标";
}


////////////////////////////////////////////////////////////////////////
// 功能：将浮点颜色值转换成双字颜色
// 参数：Point3 color 浮点颜色值
// 返回：双字颜色
////////////////////////////////////////////////////////////////////////
inline DWORD RGBFromPoint3( Point3 color )
{
	color.x = min( 1.0f, color.x );
	color.x = max( 0.0f, color.x );
	color.y = min( 1.0f, color.y );
	color.y = max( 0.0f, color.y );
	color.z = min( 1.0f, color.z );
	color.z = max( 0.0f, color.z );

	return 0xff000000 || ( ( (DWORD)( 255.1f*color.x ) )<<24 )
		||( ( (DWORD)( 255.1f*color.y ) )<<16 )||( (DWORD)( 255.1f*color.z ) );
}



typedef bool (*SaveVetexBase1)(CVector3f v, CVector3f n, CVector2f t, SaveInfo& SI, ofstream& File);
typedef bool (*SaveVetexBase2)( CVector3f v, CVector3f n, CVector2f t, CVector2f t1, SaveInfo& SI, ofstream& File );
typedef bool (*SaveVetexBase3)( CVector3f v, CVector3f n, CVector2f t, CVector2f t1,CVector2f t2,SaveInfo& SI, ofstream& File );
typedef bool (*SaveVetexBase)( CVector3f v, CVector3f n, SaveInfo& SI, ofstream& File );



namespace sqr
{
	class VerSkinNCTComp
	{
	public:
		bool operator()(const VerSkinNCT& Left, const VerSkinNCT& Right) const
		{
			return memcmp( &Left, &Right, sizeof(VerSkinNCT) ) > 0;
		}
	};

	CPluginMesh::CPluginMesh()
	{
		m_ExpNode = NULL;
		m_MinUV.x = FLT_MAX;
		m_MaxUV.x = -FLT_MAX;
		m_MinUV.y = FLT_MAX;
		m_MaxUV.y = -FLT_MAX;
		m_oldMesh = NULL;
		
		m_tIsCompress = true;
	}

	CPluginMesh::~CPluginMesh()
	{
	}

	int CPluginMesh::BinLoad( ifstream& File ,DWORD Version)
	{
		DWORD tVersionBox = GETBOX(Version);
		DWORD tVersionCp = GETCP(Version);
		DWORD tVersionLow = GETVERSION(Version);
		
		File.read( (char*)&GetMeshType(), sizeof(GetMeshType()) );
		File.read( (char*)&GetBaize(), sizeof(GetBaize()) );
		File.read( (char*)&GetOrgScale(), sizeof(GetOrgScale()) );
		File.read( (char*)&GetOrgOff(), sizeof(GetOrgOff()) );
		
		File.read( (char*)&GetScale(), sizeof(GetScale()) );
		File.read( (char*)&GetOff(), sizeof(GetOff()) );
		File.read( (char*)&GetTexScale(), sizeof(GetTexScale()) );
		

		if(tVersionLow>=10005)
			File.read( (char*)&GetChannel(),sizeof(GetChannel()));

		if((tVersionLow>=10007)&&tVersionCp)
			File.read( (char*)&GetIsCompress(), sizeof(GetIsCompress()));

		if(tVersionLow==10007||tVersionBox)
		{
			DWORD BoxCount = 0;
			File.read( (char*)&BoxCount,sizeof(DWORD) );		
			uint16 ID;
			CVector3f MinPos;
			CVector3f MaxPos;

			for(DWORD j = 0;j<BoxCount;++j)
			{
				CBaseAABB Box;
				File.read( (char*)&ID,sizeof(uint16));
				File.read( (char*)&MinPos,sizeof(CVector3f) );
				File.read( (char*)&MaxPos,sizeof(CVector3f) );
				Box.m_vMinimum = MinPos;  Box.m_vMaximum = MaxPos;
				m_BoxMap.insert(BoxMap::value_type(ID,Box));
			}
		}

		SaveInfo SI( GetOrgScale(), GetOrgOff(), GetTexScale(), GetMeshType() );

		if( GetMeshType() == VT_SOFTCLOTHANI )
		{
		}
		else
			if( GetMeshType() == VT_SOFTBODYANI )
			{
			}
			else
			{
				int Size;
				File.read( (char*)&Size, sizeof(Size) );
				if(!GetIsCompress())
				{
					for( int i = 0; i < Size; i++ )
					{
						CPluginMeshLevel* pMeshLevel = createMeshLevel();
						float fDist = 0.0f;
						File.read( (char*)&fDist, sizeof(fDist) );
						pMeshLevel->SetDist(fDist);
						int Size;
						if( SI.eVetexType == VT_SKELETALANI )
						{
							File.read( (char*)&Size, sizeof(Size) );
							m_FramesVertex.resize(1);
							m_FramesNormal.resize(1);
							m_FramesVertex[0].resize(Size);
							m_FramesNormal[0].resize(Size);
							m_TextCoord.resize(Size);
							m_TextCoord1.resize(Size);
							m_TextCoord2.resize(Size);
							m_Weight.resize(Size);
							m_SkeletalIndex.resize(Size);
							for( int i = 0; i < Size; i++ )
							{
								switch(GetChannel())
								{
								case 0:
								case 1:
									LoadVetexFull( m_FramesVertex[0][i], m_FramesNormal[0][i], m_TextCoord[i], SI, File );
									break;
								case 2:
									LoadVetexFull( m_FramesVertex[0][i], m_FramesNormal[0][i], m_TextCoord[i], m_TextCoord1[i], SI, File );
									break;
								default:
									LoadVetexFull( m_FramesVertex[0][i], m_FramesNormal[0][i], m_TextCoord[i], m_TextCoord1[i], m_TextCoord2[i], SI, File );
									break;
								}

								File.read( (char*)&m_Weight[i], sizeof(CTinyWeight) );
								File.read( (char*)&m_SkeletalIndex[i], sizeof(SkltIndex) );
							}
						}
						else if( SI.eVetexType == VT_STATICVERTEX )
						{
							File.read( (char*)&Size, sizeof(Size) );
							m_FramesVertex.resize(1);
							m_FramesNormal.resize(1);
							m_FramesVertex[0].resize(Size);
							m_FramesNormal[0].resize(Size);
							m_TextCoord.resize(Size);
							m_TextCoord1.resize(Size);
							m_TextCoord2.resize(Size);
							switch(GetChannel())
							{
							case 0:
							case 1:
								for( int i = 0; i < Size; i++ )
									LoadVetexFull( m_FramesVertex[0][i], m_FramesNormal[0][i], m_TextCoord[i], SI, File );
								break;
							case 2:
								for( int i = 0; i < Size; i++ )
									LoadVetexFull( m_FramesVertex[0][i], m_FramesNormal[0][i], m_TextCoord[i], m_TextCoord1[i], SI, File );
								break;
							default:
								for( int i = 0; i < Size; i++ )
									LoadVetexFull( m_FramesVertex[0][i], m_FramesNormal[0][i], m_TextCoord[i], m_TextCoord1[i], m_TextCoord2[i], SI, File );
								break;
							}
						}
						else if( SI.eVetexType == VT_VERTEXANI )
						{
						}

						File.read( (char*)&Size, sizeof(Size) );
						pMeshLevel->CreateIndexBuffer(Size);
						for( int j = 0; j < pMeshLevel->GetIndexNum(); j++ )
						{
							uint16 vertIndex = pMeshLevel->GetIndex(j);
							File.read( (char*)&vertIndex, m_FramesVertex[0].size() < 256 ? sizeof(BYTE) : sizeof(WORD) );
						}
					}
				}
				else
				{
					for( int i = 0; i < Size; i++ )
					{
						CPluginMeshLevel* pMeshLevel = createMeshLevel();
						
						float fDist = 0.0f;
						File.read( (char*)&fDist, sizeof( fDist ) );
						pMeshLevel->SetDist(fDist);

						int Size;
						if( SI.eVetexType == VT_SKELETALANI )
						{
							File.read( (char*)&Size, sizeof(Size) );
							m_FramesVertex.resize(1);
							m_FramesNormal.resize(1);
							m_FramesVertex[0].resize(Size);
							m_FramesNormal[0].resize(Size);
							m_TextCoord.resize(Size);
							m_TextCoord1.resize(Size);
							m_TextCoord2.resize(Size);
							m_Weight.resize(Size);
							m_SkeletalIndex.resize(Size);
							for( int i = 0; i < Size; i++ )
							{
								switch(GetChannel())
								{
								case 0:
								case 1:
									LoadVetex( m_FramesVertex[0][i], m_FramesNormal[0][i], m_TextCoord[i], SI, File );
									break;
								case 2:
									LoadVetex( m_FramesVertex[0][i], m_FramesNormal[0][i], m_TextCoord[i], m_TextCoord1[i], SI, File );
									break;
								default:
									LoadVetex( m_FramesVertex[0][i], m_FramesNormal[0][i], m_TextCoord[i], m_TextCoord1[i], m_TextCoord2[i], SI, File );
									break;
								}
								//////////////////////////////////////////////////////////////////////////

								//LoadVetex( m_FramesVertex[0][i], m_FramesNormal[0][i], m_TextCoord[i], SI, File );
								File.read( (char*)&m_Weight[i], sizeof(CTinyWeight) );
								File.read( (char*)&m_SkeletalIndex[i], sizeof(SkltIndex) );
							}
						}
						else if( SI.eVetexType == VT_STATICVERTEX )
						{
							File.read( (char*)&Size, sizeof(Size) );
							m_FramesVertex.resize(1);
							m_FramesNormal.resize(1);
							m_FramesVertex[0].resize(Size);
							m_FramesNormal[0].resize(Size);
							m_TextCoord.resize(Size);
							m_TextCoord1.resize(Size);
							m_TextCoord2.resize(Size);
							switch(GetChannel())
							{
							case 0:
							case 1:
								for( int i = 0; i < Size; i++ )
									LoadVetex( m_FramesVertex[0][i], m_FramesNormal[0][i], m_TextCoord[i], SI, File );
								break;
							case 2:
								for( int i = 0; i < Size; i++ )
									LoadVetex( m_FramesVertex[0][i], m_FramesNormal[0][i], m_TextCoord[i], m_TextCoord1[i], SI, File );
								break;
							default:
								for( int i = 0; i < Size; i++ )
									LoadVetex( m_FramesVertex[0][i], m_FramesNormal[0][i], m_TextCoord[i], m_TextCoord1[i], m_TextCoord2[i], SI, File );
								break;
							}
						}
						else if( SI.eVetexType == VT_VERTEXANI )
						{
						}

						File.read( (char*)&Size, sizeof(Size) );
						pMeshLevel->CreateIndexBuffer(Size);
						for( int j = 0; j < pMeshLevel->GetIndexNum(); j++ )
						{
							uint16 vertIndex = pMeshLevel->GetIndex(j);
							File.read( (char*)&vertIndex, m_FramesVertex[0].size() < 256 ? sizeof(BYTE) : sizeof(WORD) );
							pMeshLevel->SetIndex(j, vertIndex);
						}
					}
				}
			}

			return 0;
	}

	int CPluginMesh::BinSave( ofstream& File, vector<CapsuleNode> & CapsuleNodes )
	{
		VERTEXTYPE vertType = GetMeshType();
		File.write( (char*)&vertType, sizeof(vertType) );
		
		float fBaize = GetBaize();
		File.write( (char*)&fBaize, sizeof( fBaize ) );

		CVector3f OrgScale	= GetOrgScale();
		CVector3f OrgOff	= GetOrgOff();
		CVector3f scale		= GetScale();
		CVector3f off		= GetOff();
		if ( g_bIsExistModFile && m_oldMesh )
		{
			OrgScale = m_oldMesh->GetOrgScale();
			OrgOff   = m_oldMesh->GetOrgOff();
			scale    = m_oldMesh->GetScale();
			off	     = m_oldMesh->GetOff();
		}

		File.write( (char*)&GetScale(), sizeof(GetScale()) );
		File.write( (char*)&GetOff(),   sizeof(GetOff()) );
		File.write( (char*)&scale,    sizeof( scale ) );
		File.write( (char*)&off,	  sizeof( off ) );

		SaveInfo SI( GetScale(), GetOff(), m_MinUV, m_MaxUV );
		File.write( (char*)&SI.sTex, sizeof( SI.sTex ) );
		File.write( (char*)&GetChannel(),		sizeof( GetChannel()));
		File.write( (char*)&GetIsCompress(),		sizeof( GetIsCompress()));

		//-------------保存OBB------------------------------------
		if(g_bIsExistModFile&&m_oldMesh)
		{
			DWORD Size = (DWORD)m_oldMesh->m_BoxMap.size();
			File.write( (char*)&Size,sizeof(DWORD));
			BoxMap::iterator it = m_oldMesh->m_BoxMap.begin();
			BoxMap::iterator eit = m_oldMesh->m_BoxMap.end();
			for(;it!=eit;++it)
			{
				uint16 ID = it->first;
				File.write( (char*)&ID,sizeof(uint16));
				File.write( (char*)&(it->second.m_vMinimum),sizeof(CVector3f) );
				File.write( (char*)&(it->second.m_vMaximum),sizeof(CVector3f) );
			}
		}
		else
		{
			DWORD Size = (DWORD)m_BoxMap.size();
			File.write( (char*)&Size,sizeof(DWORD));
			BoxMap::iterator it = m_BoxMap.begin();
			BoxMap::iterator eit = m_BoxMap.end();
			for(;it!=eit;++it)
			{
				uint16 ID = it->first;
				File.write( (char*)&ID,sizeof(uint16));
				File.write( (char*)&(it->second.m_vMinimum),sizeof(CVector3f) );
				File.write( (char*)&(it->second.m_vMaximum),sizeof(CVector3f) );
			}
		}


		//---------------------------------------------------------	

		if( GetMeshType() == VT_SOFTCLOTHANI )
		{
			SoftClothExpProp Exps;
			Exps.MaxNode = this->m_ExpNode->m_3DMaxNode;
			CSoftClothDlg SCDlg;
			SCDlg.DoModal( &Exps );

			///////////////////////////////////////////////////
			//
			// 开始导出柔体（披风）
			//
			// 1面引用Fix点 ClothVertexLT
			//
			//       [@]---[@]---[@]---[@]---[@]---[@] Fix点
			//        |    /|    /|    /|    /|    /|
			//        |   / |   / |   / |   / |   / |
			//        |  /  |  /  |  /  |  /  |  /  |
			//        | /   | /   | /   | /   | /   |
			//        |/    |/    |/    |/    |/    |
			//        @-----@-----@-----@-----@-----@
			//        |    /|    /|    /|    /|    /|
			//        |   / |   / |   / |   / |   / |
			//        |  /  |  /  |  /  |  /  |  /  |
			//        | /   | /   | /   | /   | /   |
			//        |/    |/    |/    |/    |/    |
			//        @-----@-----@-----@-----@-----@
			//        |    /|    /|    /|    /|    /|
			//        |   / |   / |   / |   / |   / |
			//        |  /  |  /  |  /  |  /  |  /  |
			//        | /   | /   | /   | /   | /   |
			//        |/    |/    |/    |/    |/    |
			//        @-----@-----@-----@-----@-----@
			//  
			///////////////////////////////////////////////////

			// <1> 定义柔体参数
			unsigned __int8 sc_Width;       // 横宽点数
			unsigned __int8 sc_Height;      // 竖高点数

			// <2> 初始化柔体参数           已加入 //暂时手动初始化，要加对话框让美术输入

			sc_Width  = Exps.Width;     //sc_Width  = 10;
			sc_Height = Exps.Height;    //sc_Height = 10;

			// <3> 统计原始数据
			vector<CVector3f>& VerBuf = m_FramesVertex[0];
			CPluginMeshLevel*    pMeshLevel = GetPluginMeshLevel(0);
			vector<VertextFace>  VerUse;
			vector<ClothFace>    ClothFaces;

			VerUse.resize( VerBuf.size() );
			for( int i = 0; i < (int)pMeshLevel->GetIndexNum(); i++ )
			{
				VerUse[pMeshLevel->GetIndex(i)].nNum++;
				if( (i+1)%3 == 0 )
				{
					ClothFaces.push_back( ClothFace(pMeshLevel->GetIndex(i-2), pMeshLevel->GetIndex(i-1),pMeshLevel->GetIndex(i)) );
				}
			}

#ifdef OUTPUT_DEBUG        
			printf( " VerBuf.size()=%d \n", VerBuf.size());       
			printf( " ClothFaces.size()=%d \n", ClothFaces.size());
			for(int i=0; i< ClothFaces.size(); ++i)
			{
				printf("[%d] %d %d %d   ", i, ClothFaces[i].VerIndex0, ClothFaces[i].VerIndex1, ClothFaces[i].VerIndex2);
				if(i%3==0) printf("\n");
			} 
#endif

			// <4> 找出顶行最左Fix点

			float   _yMax       = -1.0 * FLT_MAX;
			float   _yMin       = 1.0 * FLT_MAX;
			int     FixVertexLT = -1;

			for( int i = 0; i < VerBuf.size(); ++i)
			{
				if( VerUse[i].nNum == 1)
				{
					if( Exps.HengXiang != 0)
					{
						if (VerBuf[i].z > _yMax)
						{
							_yMax       = VerBuf[i].z;
							FixVertexLT = i;
						}
					}
					else
					{
						if (VerBuf[i].y > _yMax)
						{
							_yMax       = VerBuf[i].y;
							FixVertexLT = i;
						}
					}

				}
			}

			if( FixVertexLT == -1 )
			{
				::MessageBox( 0, "柔布不能做成这样，改改吧 :)", 0, 0 );
				return false;
			}

			// <5> 填充披风顶点
			set< int >            AlreadyPush;
			vector< vector<int> > ClothNode;

			int _FixVertexLT = FixVertexLT;

			for( int j = 0; j < sc_Height; ++j)
			{
				int _NV = -5;
				int _NextLineFixVertexLT = -5;              

				vector<int> LineNode;
				LineNode.push_back( _FixVertexLT );
				AlreadyPush.insert( _FixVertexLT );

				for( int i = 0; i < sc_Width - 1; ++i )
				{
					int FV1 = -1, FV2 = -1;
					for( int i = 0; i<ClothFaces.size(); ++i )
					{
						int hr = ClothFaces[i].InFace( _FixVertexLT );
						if( hr != -1 )
						{
							ClothFaces[i].GetOtherTwo( hr, FV1, FV2 );
							if( j < sc_Height - 1 )
							{
								if( FV1 != _NV && FV2 != _NV && AlreadyPush.count(FV1)==0 && AlreadyPush.count(FV2)==0 )
								{
									if( VerBuf[ FV1 ].y < VerBuf[ FV2 ].y ) { int temp = FV1; FV1 = FV2; FV2 = temp; }
									_FixVertexLT = FV1;
									_NV          = FV2;
									LineNode.push_back( _FixVertexLT );

									AlreadyPush.insert( _FixVertexLT );
									if( _NextLineFixVertexLT < 0) _NextLineFixVertexLT = FV2;
								}      
							} else {
								if( AlreadyPush.count(FV1) + AlreadyPush.count(FV2) == 1)
								{
									if( AlreadyPush.count(FV1)==1 ) { FV1 = FV2; }
									_FixVertexLT = FV1;
									LineNode.push_back( _FixVertexLT );
									AlreadyPush.insert( _FixVertexLT );
								}     
							}
						}
					}
				}

				ClothNode.push_back( LineNode );

#ifdef OUTPUT_DEBUG        
				for(int j=0; j<LineNode.size(); ++ j)
				{
					printf( "%d ", LineNode[j] );
				}
				printf("\n _NextLineFixVertexL = %d \n", _NextLineFixVertexLT);
#endif
				_FixVertexLT = _NextLineFixVertexLT;

			}

#ifdef OUTPUT_DEBUG        

			for(int i=0; i<ClothNode.size(); ++i)
			{
				for(int j=0; j<ClothNode[i].size(); ++ j)
				{
					printf( "%d ", ClothNode[i][j] );
				}
				printf("\n");
			}     

			std::string input;           
			while( input!="end" )
			{
				std::cout<<"Debug:";
				std::cin>>input;
				if(input=="end") break;
				if(input=="ver")
				{   
					int para;
					std::cin>>para;
					printf("VerBuf[%d] xyz = %f %f %f\n", para, VerBuf[para].x, VerBuf[para].y, VerBuf[para].z);
				}
				if(input=="cn")
				{
					for(int i=0; i<ClothNode.size(); ++i)
					{
						for(int j=0; j<ClothNode[i].size(); ++ j)
						{
							printf( "%d ", ClothNode[i][j] );
						}
						printf("\n");
					}   
				}
			}
#endif

			// <6> 保存信息

			//File.write( (char*)&sc_Width,  sizeof(uint8) );
			//File.write( (char*)&sc_Height, sizeof(uint8) );

			File.write( (char*)&Exps, sizeof(SoftClothExpPropSave) );

			// 输出第一行控制点位置
			for(int j=0; j<sc_Width; ++j)
			{

				File.write( (char*)&VerBuf[ClothNode[0][j]], sizeof( CVector3f ) );
				File.write( (char*)&CTinyWeight(m_Weight[ClothNode[0][j]]._w[0], 
					m_Weight[ClothNode[0][j]]._w[1],
					m_Weight[ClothNode[0][j]]._w[2],
					m_Weight[ClothNode[0][j]]._w[3]),        sizeof( CTinyWeight ) );
				File.write( (char*)&(m_SkeletalIndex[ClothNode[0][j]]), sizeof( SkltIndex ) );
			}

			// 输出横向弹簧
			for(int i=0; i<sc_Height; ++i)
			{
				for(int j=0; j<sc_Width-1; ++j)
				{
					CVector3f a = *(CVector3f*)&VerBuf[ClothNode[i][j]];
					CVector3f b = *(CVector3f*)&VerBuf[ClothNode[i][j+1]];
					float l = CVector3f(b - a).Mag();
					File.write( (char*)&l, sizeof( float ) );
				}
			}

			// 输出纵向弹簧
			for(int i=0; i<sc_Width; ++i)
			{
				for(int j=0; j<sc_Height-1; ++j)
				{
					CVector3f a = *(CVector3f*)&VerBuf[ClothNode[j][i]];
					CVector3f b = *(CVector3f*)&VerBuf[ClothNode[j+1][i]];
					float l = CVector3f(b - a).Mag();
					File.write( (char*)&l, sizeof( float ) );
				}
			}

			// 输出
			for(int i=0; i<sc_Height; ++i)
			{
				for(int j=0; j<sc_Width; ++j)
				{
					uint16 u,v;
					if( m_TextCoord[ClothNode[i][j]].x > 32.02f ||
						m_TextCoord[ClothNode[i][j]].y > 32.02f )
					{
						::MessageBox(0,"柔布贴图坐标太大，本次导出无效果,请修改后重新导出",0,0);
						break;
					}
					u = m_TextCoord[ClothNode[i][j]].x * 2046.0f;
					v = m_TextCoord[ClothNode[i][j]].y * 2046.0f;
					File.write( (char*)&u, sizeof( uint16 ) );	
					File.write( (char*)&v, sizeof( uint16 ) );	
				}
			}
			uint8 CollideNum = CapsuleNodes.size();
			File.write( (char*)&CollideNum, sizeof( uint8 ) );
			for( uint8 i = 0; i < CollideNum; ++i)
			{
				File.write( (char*)&CapsuleNodes[i], sizeof( CapsuleNode ) );
			}

			///////////////////// 导出完毕 ///////////////////

		} else
			if( GetMeshType() == VT_SOFTBODYANI )
			{
				vector<CVector3f>& VerBuf = m_FramesVertex[0];
				CPluginMeshLevel* pMeshLevel = GetPluginMeshLevel(0);

				//printf( "SaveSoftBody\n", nCount );

				vector<VertextFace> VerUse;
				VerUse.resize( VerBuf.size() );
				for( int i = 0; i < (int)pMeshLevel->GetIndexNum(); i++ )
				{
					if( VerUse[pMeshLevel->GetIndex(i)].nNum < 4 )
						VerUse[pMeshLevel->GetIndex(i)].nFace[VerUse[pMeshLevel->GetIndex(i)].nNum++] = i/3;
					else
						return false;
				}

				int CatchVer[2] = { -1, -1 };
				//找出面引用数为一的顶点
				for( int i = 0; i < (int)VerBuf.size(); i++ )
				{
					if( VerUse[i].nNum == 1 && ( CatchVer[0] == -1 || VerBuf[i].y > VerBuf[CatchVer[0]].y ) )
					{
						CatchVer[0] = i;
						int nFace = VerUse[i].nFace[0]*3;
						for( int n = nFace; n < nFace + 3; n++ )
						{
							if( VerUse[pMeshLevel->GetIndex(n)].nNum == 2 )
							{
								CatchVer[1] = pMeshLevel->GetIndex(n);
								break;
							}
						}
					}
				}

				if( CatchVer[0] == -1 || CatchVer[1] == -1 )
					return false;

				printf( "%d, %d, %d\n", VerBuf.size(), m_Weight.size(), m_SkeletalIndex.size() );
				CVector3f p1 = VerBuf[CatchVer[0]];
				CVector3f p2 = VerBuf[CatchVer[1]];
				CWeight   W1 = m_Weight[CatchVer[0]];
				CWeight   W2 = m_Weight[CatchVer[1]];
				SkltIndex	I1 = m_SkeletalIndex[CatchVer[0]];
				SkltIndex	I2 = m_SkeletalIndex[CatchVer[1]];

				int Index = 0;
				CVector3f vDir = VerBuf[CatchVer[1]] - VerBuf[CatchVer[0]];
				VerUse[CatchVer[0]].nNewIndex = Index++;
				VerUse[CatchVer[1]].nNewIndex = Index++;

				while(1)
				{
					set<WORD> PreFace;
					for( int i = 0; i < 2; i++ )
					{
						for( int j = 0; j < VerUse[CatchVer[i]].nNum; j++ )
							PreFace.insert( VerUse[CatchVer[i]].nFace[j] );
					}
					set<WORD> NewVer;
					for( set<WORD>::iterator it = PreFace.begin(); it != PreFace.end(); it++ )
					{
						int nFace = (*it)*3;
						for( int n = nFace; n < nFace + 3; n++ )
							if( VerUse[pMeshLevel->GetIndex(n)].nNewIndex == -1 )
								NewVer.insert(pMeshLevel->GetIndex(n)); 
					}
					if( NewVer.size() == 0 )
						break;
					if( NewVer.size() != 2 )
						return false;
					CatchVer[0] = *(NewVer.begin()); 
					CatchVer[1] = *(++(NewVer.begin()));

					CVector3f vCurDir = VerBuf[CatchVer[1]] - VerBuf[CatchVer[0]];
					if(vDir.Dot(vCurDir)<0)
						//if( D3DXVec3Dot( &vDir, &vCurDir ) < 0 )
						swap( CatchVer[0], CatchVer[1] );
					VerUse[CatchVer[0]].nNewIndex = Index++;
					VerUse[CatchVer[1]].nNewIndex = Index++;
				}

				vector<VerNorTex> SaveBuf;
				SaveBuf.resize( Index );
				for( int i = 0; i < (int)VerUse.size(); i++ )
				{
					if( VerUse[i].nNewIndex != -1 )
					{
						SaveBuf[VerUse[i].nNewIndex].p = VerBuf[i];
						SaveBuf[VerUse[i].nNewIndex].np = m_FramesNormal[0][i];
						SaveBuf[VerUse[i].nNewIndex].tu = m_TextCoord[i].x;
						SaveBuf[VerUse[i].nNewIndex].tv = m_TextCoord[i].y;
					}
				}

				//写柔体类型
				ESoftType nType = SoftPlane;
				File.write( (char*)&nType,   sizeof(nType) );

				//写每个结点的物理参数参数信息
				NodePhyPara nodePhyPara = { 9.8f, 0.09f, 0.1f };
				File.write( (char*)&nodePhyPara, sizeof(NodePhyPara) );

				//写入弹簧信息

				float fRatioPull     = 1.1f;			//最大拉长比率(与自然长度相比)

				float fRatioCompress = 0.9f;			//最小缩短比率(与自然长度相比)
				float fCoe			 = 0.098f;
				File.write( (char*)&fRatioCompress, sizeof(float) );
				File.write( (char*)&fRatioPull,     sizeof(float) );
				File.write( (char*)&fCoe,			sizeof(float) );

				File.write( (char*)&p1, sizeof( CVector3f ) );
				File.write( (char*)&W1, sizeof( CWeight ) );
				File.write( (char*)&I1, sizeof( SkltIndex ) );
				File.write( (char*)&p2, sizeof( CVector3f ) );
				File.write( (char*)&W2, sizeof( CWeight ) );
				File.write( (char*)&I2, sizeof( SkltIndex ) );

				int nCount = (int)SaveBuf.size()/2;

				printf( "SaveBuf: %d\n", nCount );
				File.write( (char*)&nCount,  sizeof(int) );
				for( int i = 0; i < nCount; i++ )
				{
					CVector3f d  = SaveBuf[i*2+0].p - SaveBuf[i*2+1].p;
					float fWidth = d.Mag();/*D3DXVec3Length( &d );*/
					float fLen = 0;
					if( i )
					{
						d = ( SaveBuf[i*2+0].p + SaveBuf[i*2+1].p )/2 - ( SaveBuf[i*2-2].p + SaveBuf[i*2-1].p )/2;
						fLen = d.Mag();/*D3DXVec3Length( &d );*/
					}
					File.write( (char*)&SaveBuf[i*2+0].tu, sizeof( float ) );	
					File.write( (char*)&SaveBuf[i*2+0].tv, sizeof( float ) );	
					File.write( (char*)&SaveBuf[i*2+1].tu, sizeof( float ) );	
					File.write( (char*)&SaveBuf[i*2+1].tv, sizeof( float ) );	
					File.write( (char*)&fLen, sizeof( float ) );		
					File.write( (char*)&fWidth, sizeof( float ) );		
				}

				DWORD nBnCount = 0;
				File.write( (char*)&nBnCount, sizeof(DWORD) );
			}
			else
			{
				int Size;
				Size = (int)GetPluginMeshLevelNum();
				File.write( (char*)&Size, sizeof(Size) );

				SaveVetexBase1 save1 = NULL;
				SaveVetexBase2 save2 = NULL;
				SaveVetexBase3 save3 = NULL;
				SaveVetexBase  saveNoTex  = NULL;
				if(!GetIsCompress())
				{
					//不压缩
					save1		= &PluginSaveVetexFull;
					save2		= &PluginSaveVetexFull;
					save3		= &PluginSaveVetexFull;
					saveNoTex	= &PluginSaveVetexFull;
				}
				else
				{
					//压缩
					save1		= &PluginSaveVetex;
					save2		= &PluginSaveVetex;
					save3		= &PluginSaveVetex;
					saveNoTex   = &PluginSaveVetex;
				}
				try
				{
					for( int i = 0; i < (int)GetPluginMeshLevelNum(); i++ )
					{
						float fDist = GetPluginMeshLevel(i)->GetDist();
						File.write( (char*)&fDist, sizeof(fDist) );
						if( GetMeshType() == VT_SKELETALANI )
						{
							Size = (int)m_FramesVertex[0].size();
							File.write( (char*)&Size, sizeof(Size) );
							for( int j = 0; j < Size; j++ )
							{
								switch(GetChannel())
								{
								case 0:
								case 1:
									(*save1)( m_FramesVertex[0][j], m_FramesNormal[0][j], m_TextCoord[j], SI, File );
									break;
								case 2:
									(*save2)( m_FramesVertex[0][j], m_FramesNormal[0][j], m_TextCoord[j],m_TextCoord1[j], SI, File );
									break;
								default:
									(*save3)( m_FramesVertex[0][j], m_FramesNormal[0][j], m_TextCoord[j], m_TextCoord1[j],m_TextCoord2[j],SI, File );
									break;
								}
								//SaveVetex( m_FramesVertex[0][j], m_FramesNormal[0][j], m_TextCoord[j], SI, File );
								CTinyWeight w( m_Weight[j]._w[0], m_Weight[j]._w[1],
									m_Weight[j]._w[2], m_Weight[j]._w[3] );
								File.write( (char*)&w, sizeof(CTinyWeight) );
								File.write( (char*)&m_SkeletalIndex[j], sizeof(SkltIndex) );
							}
						}
						else if( GetMeshType() == VT_STATICVERTEX )
						{
							Size = (int)m_FramesVertex[0].size();
							File.write( (char*)&Size, sizeof(Size) );
							/*for( int j = 0; j < Size; j++ )
							SaveVetex( m_FramesVertex[0][j], m_FramesNormal[0][j], m_TextCoord[j], SI, File );*/
							switch(GetChannel())
							{
							case 0:
							case 1:
								for( int j = 0; j < Size; j++ )
									(*save1)( m_FramesVertex[0][j], m_FramesNormal[0][j], m_TextCoord[j], SI, File );
								break;
							case 2:
								for( int j = 0; j < Size; j++ )
									(*save2)( m_FramesVertex[0][j], m_FramesNormal[0][j], m_TextCoord[j],m_TextCoord1[j], SI, File );
								break;
							default:
								for( int j = 0; j < Size; j++ )
									(*save3)( m_FramesVertex[0][j], m_FramesNormal[0][j], m_TextCoord[j], m_TextCoord1[j],m_TextCoord2[j],SI, File );
								break;
							}
						}
						else if( GetMeshType() == VT_VERTEXANI )
						{
							Size = 1;
							File.write( (char*)&Size, sizeof(Size) );
							Size = (int)m_FramesVertex[0].size();
							File.write( (char*)&Size, sizeof(Size) );
							Size = m_FramesVertex.size();

							IDNAME AniName = "Default";
							File.write( (char*)&AniName, sizeof(AniName) );
							File.write( (char*)&m_FramePerKeyFrame, sizeof( m_FramePerKeyFrame ) );
							File.write( (char*)&Size, sizeof(Size) );

							for( int j = 0; j < m_FramesVertex.size(); j++ )
							{
								for( int k = 0; k < m_FramesVertex[j].size(); k++ )
								{
									if( j )
										(*saveNoTex)( m_FramesVertex[j][k], m_FramesNormal[j][k], SI, File );
									else
									{
										//SaveVetex( m_FramesVertex[j][k], m_FramesNormal[j][k], m_TextCoord[k], SI, File );
										switch(GetChannel())
										{
										case 0:
										case 1:
											(*save1)( m_FramesVertex[j][k], m_FramesNormal[j][k], m_TextCoord[k], SI, File );
											break;
										case 2:
											(*save2)( m_FramesVertex[j][k], m_FramesNormal[j][k], m_TextCoord[k],m_TextCoord1[k], SI, File );
											break;
										default:
											(*save3)( m_FramesVertex[j][k], m_FramesNormal[j][k], m_TextCoord[k],m_TextCoord1[k],m_TextCoord2[k],SI, File );
											break;
										}
									}
								}
							}
						}
						Size = (int)GetPluginMeshLevel(i)->GetIndexNum();
						File.write( (char*)&Size, sizeof(Size) );
						for( int j = 0; j < GetPluginMeshLevel(i)->GetIndexNum(); j++ )
						{
							uint16 vertIndex = GetPluginMeshLevel(i)->GetIndex(j);
							File.write( (char*)&vertIndex, m_FramesVertex[0].size() < 256 ? sizeof(BYTE) : sizeof(WORD) );
						}
					}
				}
				catch(...)
				{
					CString err;
					err.Format("导出%s出错,请检查这个piece的纹理坐标设置是否有问题或者是否过大!",m_ObjectName.c_str());
					MessageBox(NULL,err,"提示",MB_OK);
				}
			}

			return 0;
	}

	bool CPluginMesh::GetMeshFromExpNode( CAnimateGroup& AnimateGroup )
	{	
		g_SaveParam.m_uiSP_pInterface->ProgressUpdate( int( g_fCurNode += 0.1f )*100/g_SaveParam.m_uiSP_NodeNum );
		
		m_FramePerKeyFrame = (float)g_SaveParam.m_uiSP_FramePerSample;

		CPluginMeshLevel* pMeshLevel = createMeshLevel();

		if ( false == generateBaseInfo(pMeshLevel, AnimateGroup) )
			return false;

		if ( false == generateVertexAniInfo(pMeshLevel, AnimateGroup) )
			return false;

		// 求范围和偏移
		CVector3f max = CVector3f( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		CVector3f min = CVector3f(  FLT_MAX,  FLT_MAX,  FLT_MAX );
		for ( int frame = 0; frame < m_FramesVertex.size(); ++frame )
		{
			for( int i = 0; i < m_FramesVertex[frame].size(); i++ )			
			{
				max.x = max( m_FramesVertex[frame][i].x, max.x);
				max.y = max( m_FramesVertex[frame][i].y, max.y);
				max.z = max( m_FramesVertex[frame][i].z, max.z);
				min.x = min( m_FramesVertex[frame][i].x, min.x);
				min.y = min( m_FramesVertex[frame][i].y, min.y);
				min.z = min( m_FramesVertex[frame][i].z, min.z);
			}
		}

		CVector3f scale = max - min;
		CVector3f off = ( max + min )/2.0f;
		scale.x = ( (int32)( scale.x*2 ) + 1 )*0.5f;
		scale.y = ( (int32)( scale.y*2 ) + 1 )*0.5f;
		scale.z = ( (int32)( scale.z*2 ) + 1 )*0.5f;
		GetOrgScale()	= scale;
		GetScale()		= scale;
		off.x = ( (int32)( off.x*4 ) )*0.25f;
		off.y = ( (int32)( off.y*4 ) )*0.25f;
		off.z = ( (int32)( off.z*4 ) )*0.25f;
		GetOrgOff()		= off;
		GetOff()		= off;

		if( min.x < -8000 || min.y < -8000 || min.z < -8000 || 
			max.x >  8000 || max.y >  8000 || max.z >  8000  )
		{
			MessageBox( NULL, "模型范围太大,无法导出", "错误", 0 );
			return false;
		}

		CVector2f Delta = m_MaxUV - m_MinUV;
		if( Delta.x >  31 || Delta.y >  31 )
		{
			MessageBox( NULL, "贴图范围太大,无法导出", "错误", 0 );
			return false;
		}

		optimizeUV(pMeshLevel);
		SaveInfo SI( GetScale(), GetOff(), m_MinUV, m_MaxUV );
		GetTexScale() = SI.sTex;

		// mesh实际内容生成

		setBufVertexNum(0, m_FramesVertex[0].size());
		generatePosBuffer(0, m_FramesVertex[0]);
		generateNormalBuffer(0, m_FramesNormal[0]);
		generateTexCoordBuffer0(0, m_TextCoord);
		generateTexCoordBuffer1(0, m_TextCoord1);
		generateTexCoordBuffer2(0, m_TextCoord2);
		generateSkeWeightBuffer(0, m_Weight);
		generateSkeIndexBuffer(0, m_SkeletalIndex);

		return pMeshLevel->GetIndexNum() != 0;
	}

	//////////////////////////////////////////////////////////
	// 功能：得到顶点的权重
	// 参数：CMeshFrame* Frame   要填充的顶点帧, 
	//       ExportNode* ExpNode 相应的输出节点, 
	//       Modifier* mod       修改器, 
	//       CMotion& Motion     骨架层次和动画
	// 返回：无
	//////////////////////////////////////////////////////////
	void CPluginMesh::GetPhysiqueWeight( vector<CWeight>& VerWeight, vector<SkltIndex>& SkeIndex, Modifier* mod, CAnimateGroup& AnimateGroup )
	{
		// 得到IPhysiqueExport接口
		IPhysiqueExport* phyExport = (IPhysiqueExport*)mod->GetInterface( I_PHYEXPORT );

		// get the physique export context interface (context is specific to a node, as
		// the modifier may be applied across several nodes)
		IPhyContextExport* phyContext = (IPhyContextExport*)phyExport->GetContextInterface( m_ExpNode->m_3DMaxNode );
		phyContext->ConvertToRigid( TRUE );

		// get the nodes used for the skeletal deformation
		int numVerts = phyContext->GetNumberVertices();

		IPhyVertexExport* vert;				// physique base vertex interface
		IPhyBlendedRigidVertex* blendVert;	// physique rigid blended vertex interface
		IPhyRigidVertex* rigidVert;			// physique rigid vertex interface
		IPhyFloatingVertex* floatVert;		// physique floating vertex interface
		float floatingTmpWeight;			// temporary weight for floating bones
		bool canDoFloatingVerts = false;	// true if this is not physique 2.x


		// check the physique version
		if( phyExport->Version() >= 300 )
			canDoFloatingVerts = true;

		TimeValue Start = g_SaveParam.m_uiSP_pInterface->GetAnimRange().Start();
		Object* obj = m_ExpNode->m_3DMaxNode->EvalWorldState( Start ).obj;
		TriObject* triObj = (TriObject*)obj->ConvertToType( Start, triObjectClassID );
		float* pfWeight = triObj->GetMesh().getVertexWeights(); 

		int curNode;
		// 考察每个顶点，获得相应权重
		for( int i = 0; i < numVerts; i++ )
		{
			VerWeight[i]._w[0] = VerWeight[i]._w[1] = VerWeight[i]._w[2] = VerWeight[i]._w[3] = 0.0f;
			SkeIndex[i].b[0] = SkeIndex[i].b[1] = SkeIndex[i].b[2] = SkeIndex[i].b[3] = 0;

			list<WeightOfBone> Weight;	
			int ID;
			vert = phyContext->GetVertexInterface( i );
			if( vert )
			{
				int type = vert->GetVertexType();
				switch( type )
				{
					//顶点受多个骨骼影响
				case RIGID_BLENDED_TYPE:
					blendVert = (IPhyBlendedRigidVertex*)vert;
					for( curNode = 0; curNode < blendVert->GetNumberNodes(); curNode++ )
					{
						// 忽略过小的权重
						if( blendVert->GetWeight( curNode ) < MINIMUMWEIGHT )
							continue;
						if( SUCCEEDED( ID = AnimateGroup.GetSkeletalIDByName( blendVert->GetNode( curNode )->GetName() ) ) )
							Weight.push_back( WeightOfBone( blendVert->GetWeight( curNode ), ID ) );
					}
					break;
					//顶点受一个骨骼影响
				case RIGID_TYPE:
					rigidVert = (IPhyRigidVertex*)vert;
					if( SUCCEEDED( ID = AnimateGroup.GetSkeletalIDByName( rigidVert->GetNode()->GetName() ) ) )
						Weight.push_back( WeightOfBone( 1.0f, ID ) );
					break;
				default:
					break;
				}
			}
			phyContext->ReleaseVertexInterface( vert );

			if( canDoFloatingVerts )
			{
				floatVert = (IPhyFloatingVertex*)phyContext->GetFloatingVertexInterface( i );
				if( floatVert )
				{
					for( curNode = 0; curNode < floatVert->GetNumberNodes(); curNode++ )
					{
						// 忽略过小的权重 ？？？
						if( floatVert->GetWeight( curNode, floatingTmpWeight ) < MINIMUMWEIGHT )
							continue;
						if( SUCCEEDED( ID = AnimateGroup.GetSkeletalIDByName( floatVert->GetNode( curNode )->GetName() ) ) )
							Weight.push_back( WeightOfBone( floatVert->GetWeight( curNode, floatingTmpWeight ), ID ) );
					}
					phyContext->ReleaseVertexInterface( floatVert );
				}
			}

			//下面将得到的权重排序，保留最大的4个，并且单位化，填入CMeshFrame* Frame
			Weight.sort();
			list<WeightOfBone>::reverse_iterator it = Weight.rbegin();
			int j = 0;
			float W[4] = { 0, 0, 0, 0 };
			float Total = 0;
			int nWeightPerVertex = g_SaveParam.m_uiSP_WeightPerVertex;
			map<float,int>::iterator itW = m_ExpNode->m_AddBoneWeight.find( pfWeight[i] );
			if( itW != m_ExpNode->m_AddBoneWeight.end() )
			{
				nWeightPerVertex = min( 4, nWeightPerVertex + itW->second );
				printf( "Vertex%d have %d bone weight\n", i, nWeightPerVertex );
			}

			for( ; it!=Weight.rend()&&j<(int)nWeightPerVertex; it++,j++ )
			{			
				SkeIndex[i].b[j] = (*it).m_ID;
				Total += W[j] = (*it).m_f;
			}
			if( Total > MINIMUMWEIGHT )
			{
				for( j = 0; j < (int)nWeightPerVertex; j++ )
					VerWeight[i]._w[j] = W[j]/Total;
			}
		}
	}

	//////////////////////////////////////////////////////////
	// 功能：得到顶点的权重
	// 参数：CMeshFrame* Frame   要填充的顶点帧, 
	//       ExportNode* ExpNode 相应的输出节点, 
	//       Modifier* mod       修改器, 
	//       CMotion& Motion     骨架层次和动画
	// 返回：无
	//////////////////////////////////////////////////////////
	void CPluginMesh::GetSkinWeight( vector<CWeight>& VerWeight, vector<SkltIndex>& SkeIndex, Modifier* mod, CAnimateGroup& AnimateGroup )
	{
		TimeValue Start = g_SaveParam.m_uiSP_pInterface->GetAnimRange().Start();
		Object* obj = m_ExpNode->m_3DMaxNode->EvalWorldState( Start ).obj;
		TriObject* triObj = (TriObject*)obj->ConvertToType( Start, triObjectClassID );
		float* pfWeight = triObj->GetMesh().getVertexWeights(); 

		ISkin* skinExport = (ISkin*)mod->GetInterface( I_SKIN );

		// get the skin export context interface (context is specific to a node, as
		// the modifier may be applied across several nodes)
		ISkinContextData* skinContext = (ISkinContextData*)skinExport->GetContextInterface( m_ExpNode->m_3DMaxNode );

		int numVerts = skinContext->GetNumPoints();

		int curNode;
		int curBoneIndex;
		// loop through verts adding bones to the skeletal nodes set as found
		for( int i = 0; i < numVerts; i++ )
		{
			VerWeight[i]._w[0] = VerWeight[i]._w[1] = VerWeight[i]._w[2] = VerWeight[i]._w[3] = 0.0f;
			SkeIndex[i].b[0] = SkeIndex[i].b[1] = SkeIndex[i].b[2] = SkeIndex[i].b[3] = 0;
			list<WeightOfBone> Weight;	
			int ID;
			for( curNode = 0; curNode < skinContext->GetNumAssignedBones( i ); curNode++ )
			{
				if( skinContext->GetBoneWeight( i, curNode ) < MINIMUMWEIGHT )
					continue;
				curBoneIndex = skinContext->GetAssignedBone( i, curNode );
				if( SUCCEEDED( ID = AnimateGroup.GetSkeletalIDByName( skinExport->GetBone( curBoneIndex )->GetName() ) ) )
					Weight.push_back( WeightOfBone( skinContext->GetBoneWeight( i, curNode ), ID ) );
			}
			Weight.sort();
			list<WeightOfBone>::reverse_iterator it = Weight.rbegin();
			UINT j = 0;
			float W[4] = { 0, 0, 0, 0 };
			float Total = 0;
			int nWeightPerVertex = g_SaveParam.m_uiSP_WeightPerVertex;
			map<float,int>::iterator itW = m_ExpNode->m_AddBoneWeight.find( pfWeight[i] );
			if( itW != m_ExpNode->m_AddBoneWeight.end() )
			{
				nWeightPerVertex = min( 4, nWeightPerVertex + itW->second );
				printf( "Vertex%d have %d bone weight\n", i, nWeightPerVertex );
			}

			for( ; it!=Weight.rend() && j < (UINT)nWeightPerVertex; it++,j++ )
			{			
				SkeIndex[i].b[j] = (*it).m_ID;
				Total += W[j] = (*it).m_f;
			}
			if( Total > MINIMUMWEIGHT )
			{
				for( j = 0; j < (UINT)nWeightPerVertex; j++ )
					VerWeight[i]._w[j] = W[j]/Total;
			}
		}
	}

	//////////////////////////////////////////////////////////
	// 功能：得到顶点的权重
	// 参数：CMeshFrame* Frame   要填充的顶点帧, 
	//       ExportNode* ExpNode 相应的输出节点, 
	//       Modifier* mod       修改器, 
	//       CMotion& Motion     骨架层次和动画
	// 返回：无
	//////////////////////////////////////////////////////////
	void CPluginMesh::GetBPWeight( vector<CWeight>& VerWeight, vector<SkltIndex>& SkeIndex, Modifier* mod, CAnimateGroup& AnimateGroup )
	{
		TimeValue Start = g_SaveParam.m_uiSP_pInterface->GetAnimRange().Start();
		Object* obj = m_ExpNode->m_3DMaxNode->EvalWorldState( Start ).obj;
		TriObject* triObj = (TriObject*)obj->ConvertToType( Start, triObjectClassID );
		float* pfWeight = triObj->GetMesh().getVertexWeights(); 

		//for( int i = 0; i < VerWeight.size(); i++ )
		//	printf( "Vertex%d W = %f\n", i, pfWeight[i] );

		BonesPro_WeightArray* wa = NULL;
		mod->SetProperty ( BP_PROPID_GET_WEIGHTS, &wa );

		if( wa == NULL )
		{
			char Buf[256];
			sprintf_s( Buf, "无法得到BonesPro修改器关于%s的骨骼权重", m_ExpNode->m_3DMaxNode->GetName() );
			MessageBox( NULL, Buf, "Error" , 0 );
			return;
		}

		int numVerts = wa->nv;

		// loop through verts adding bones to the skeletal nodes set as found
		for( int i = 0; i < numVerts; i++ )
		{
			VerWeight[i]._w[0] = VerWeight[i]._w[1] = VerWeight[i]._w[2] = VerWeight[i]._w[3] = 0.0f;
			SkeIndex[i].b[0] = SkeIndex[i].b[1] = SkeIndex[i].b[2] = SkeIndex[i].b[3] = 0;
			list<WeightOfBone> Weight;	
			int ID;

			for( int b = 0; b < wa->nb; b++ )
			{
				if( wa->w[ i * wa->nb + b ] < MINIMUMWEIGHT )
					continue;
				BonesPro_Bone  bone;
				bone.t     = g_SaveParam.m_uiSP_pInterface->GetAnimRange().Start();
				bone.index = b;
				mod->SetProperty ( BP_PROPID_GET_BONE, &bone );

				if( SUCCEEDED( ID = AnimateGroup.GetSkeletalIDByName( bone.node->GetName() ) ) )
					Weight.push_back( WeightOfBone( wa->w[ i * wa->nb + b ], ID ) );
			}

			Weight.sort();
			list<WeightOfBone>::reverse_iterator it = Weight.rbegin();
			UINT j = 0;
			float W[4] = { 0, 0, 0, 0 };
			float Total = 0;
			int nWeightPerVertex = g_SaveParam.m_uiSP_WeightPerVertex;
			map<float,int>::iterator itW = m_ExpNode->m_AddBoneWeight.find( pfWeight[i] );
			if( itW != m_ExpNode->m_AddBoneWeight.end() )
			{
				nWeightPerVertex = min( 4, nWeightPerVertex + itW->second );
				//printf( "Vertex%d have %d bone weight\n", i, nWeightPerVertex );
			}

			for( ; it != Weight.rend() && j < (UINT)nWeightPerVertex; it++,j++ )
			{			
				SkeIndex[i].b[j] = (*it).m_ID;
				Total += W[j] = (*it).m_f;
			}
			if( Total > MINIMUMWEIGHT )
			{
				for( j = 0; j < (UINT)nWeightPerVertex; j++ )
					VerWeight[i]._w[j] = W[j]/Total;
			}
		}
	}



	bool CPluginMesh::generateFrameVertList( VerSkinNCTList& verList, TimeValue time, CAnimateGroup& animGroup )
	{
		Object* pObj = m_ExpNode->m_3DMaxNode->EvalWorldState(time).obj;
		TriObject* pTriObj = (TriObject*)pObj->ConvertToType(time, triObjectClassID);
		Mesh& mesh = pTriObj->GetMesh();
		CMatrix matWorld = GetNodeGlobeMatrix(m_ExpNode->m_3DMaxNode, time, TRUE);

		verList.resize(mesh.getNumFaces()*3);

		int* Index = FaceIndex[1];
		if( Parity( matWorld ) )
			Index = FaceIndex[0];

		DWORD diffuse = 0xff000000;
		Mtl* pMtrl = m_ExpNode->m_3DMaxNode->GetMtl();
		TimeValue Start = g_SaveParam.m_uiSP_pInterface->GetAnimRange().Start();
		if( pMtrl && pMtrl->ClassID() == Class_ID( DMTL_CLASS_ID,0 ) )
		{
			StdMat* mat = (StdMat*)pMtrl;
			DWORD c = mat->GetAmbient( Start ).toRGB();//yhl_mod
			BYTE* bc = (BYTE*)&c;
			diffuse |= bc[2]|(((DWORD)bc[1])<<8)|(((DWORD)bc[0])<<16);
		}

		// 找出要光滑的顶点法线
		vector<BOOL> NorSmooth;
		getMeshSmoothInfo(mesh, NorSmooth);

		SklWeightList	sklWeightList;	
		SklIndexList	sklIndexList;
		if( GetMeshType() == VT_SKELETALANI || GetMeshType() == VT_SOFTBODYANI || GetMeshType() == VT_SOFTCLOTHANI )//如果是骨骼动画，则需要获取顶点权重
			generateSkeletalInfo(sklWeightList, sklIndexList, m_ExpNode->m_3DMaxNode, time, animGroup);

		map< CVertexAndGroup , CVector3f>  mapNormal;
		for( int i = 0; i < mesh.getNumFaces(); i++ )
		{
			for( int j = 0; j < 3; j++ )
			{
				int Ind = mesh.faces[i].v[j]; 
				VerSkinNCT& v = verList[ i*3 + Index[j] ];

				// 填充顶点坐标
				v.p = CVector3f( mesh.verts[Ind].x, mesh.verts[Ind].z, mesh.verts[Ind].y );			
				D3DXVec3TransformCoord( (D3DXVECTOR3*)&v.p, (D3DXVECTOR3*)&v.p, (D3DXMATRIX*)&matWorld );

				if ( GetMeshType() == VT_SKELETALANI || GetMeshType() == VT_SOFTBODYANI || GetMeshType() == VT_SOFTCLOTHANI )
				{
					v.b1 = sklWeightList[Ind]._w[0];
					v.b2 = sklWeightList[Ind]._w[1];
					v.b3 = sklWeightList[Ind]._w[2];
					memcpy( v.Index, sklIndexList[Ind].b, 4 );
				}

				// 填充顶点颜色
				if ( mesh.getNumVertCol() > 0 )
					v.diffuse = RGBFromPoint3( mesh.vertCol[ mesh.vcFace[i].t[j] ] )|0xff000000;   
				else
					v.diffuse = diffuse;

				// 填充法向量
				if( g_SaveParam.m_bSP_SmoothAll || NorSmooth[ i*3 + j ] )
				{
					Point3 MaxNor = mesh.FaceNormal( i, true );
					map< CVertexAndGroup , CVector3f>::iterator iter;
					iter = mapNormal.find(CVertexAndGroup(Ind,mesh.faces[i].smGroup));
					if(iter==mapNormal.end())
					{
						mapNormal[CVertexAndGroup(Ind,mesh.faces[i].smGroup)] = CVector3f(0.0f,0.0f,0.0f);
					}
					mapNormal[CVertexAndGroup(Ind,mesh.faces[i].smGroup)]+=CVector3f( MaxNor.x, MaxNor.z, MaxNor.y );
				}
				else
				{
					Point3 MaxNor = mesh.FaceNormal( i, true );
					v.np = CVector3f( MaxNor.x, MaxNor.z, MaxNor.y );					
				}
			}
		}

		// 向量赋值到非索引缓冲区
		for( int i = 0; i < mesh.getNumFaces(); i++ )
		{
			for( int j = 0; j < 3; j++ )
			{
				VerSkinNCT& v = verList[ i*3 + Index[j] ];
				if( g_SaveParam.m_bSP_SmoothAll || NorSmooth[ i*3 + j ] )
					v.np = mapNormal[CVertexAndGroup( mesh.faces[i].v[j] ,mesh.faces[i].smGroup)];
				D3DXVec3TransformNormal( (D3DXVECTOR3 *)&v.np, (D3DXVECTOR3 *)&v.np, (D3DXMATRIX*)&matWorld );
				v.np.Normalize();
			}
		}

		bool bRet = generateUV(mesh, verList, matWorld);

		if ( pObj != pTriObj )
		{
			if ( pTriObj )
				pTriObj->DeleteThis();
		}

		return bRet;
	}

	bool CPluginMesh::generateUV( Mesh& mesh, VerSkinNCTList& verList, const CMatrix& matWorld )
	{
		// 多个贴图通道map channel
		int* Index = FaceIndex[1];
		if( Parity( matWorld ) )
			Index = FaceIndex[0];
		vector<int> NumOfUV;
#ifdef DoubleUV
		for ( int chanloop = 1; chanloop < 10; chanloop++ )
		{
#endif
#ifdef SingleUV
			chanloop = 1;
#endif

			if (mesh.mapSupport(chanloop)) 
			{
				GetChannel()++;
				NumOfUV.push_back(mesh.getNumMapVerts(chanloop));
				if(GetChannel()!=1)
				{
					for(int j = 0;j<NumOfUV.size()-1;j++)
					{
						if(NumOfUV[j]!=NumOfUV[j+1])
						{
							char Buf[256];
							sprintf_s( Buf, "ObjectName = %s,UV点数两层不一样!", m_ObjectName.c_str() );
							MessageBox( 0, Buf, "错误", 0 );

							return false;
						}
					}
				}

				for ( int i = 0; i < mesh.getNumMapVerts(chanloop); i++ ) 
				{
					UVVert tvert =  mesh.mapVerts(chanloop)[i]; // 当前channel每个贴图点数的uv坐标
					if ( GetChannel() != 1 )
						mesh.setMapVert(1,i,tvert);
				}

				for ( int i = 0; i <  mesh.getNumFaces(); i++ ) 
				{
					TVFace tface =  mesh.mapFaces(chanloop)[i]; // 可以得到当前channel的这个面的三个点的顶点索引
					switch(chanloop)
					{
					case 0:
					case 1:
						for( int j = 0; j < 3; j++ )
						{
							VerSkinNCT& v = verList[ i*3 + Index[j] ];
							UVVert curVertUV = mesh.getTVert( tface.t[j] );
							v.tu = curVertUV.x;
							v.tv = 1.0f - curVertUV.y;
						}
						break;
					case 2:
						for( int j = 0; j < 3; j++ )
						{
							VerSkinNCT& v = verList[ i*3 + Index[j] ];
							UVVert curVertUV = mesh.getTVert( tface.t[j] );
							v.tu1 = curVertUV.x;
							v.tv1 = 1.0f - curVertUV.y;
						}
						break;
					default:
						for( int j = 0; j < 3; j++ )
						{
							VerSkinNCT& v = verList[ i*3 + Index[j] ];
							UVVert curVertUV = mesh.getTVert( tface.t[j] );
							v.tu2 = curVertUV.x;
							v.tv2 = 1.0f - curVertUV.y;
						}
						break;
					}
				}
			}
#ifdef DoubleUV
		}
#endif

		return true;
	}

	void CPluginMesh::generateSkeletalInfo( 
		SklWeightList&	sklWeightList, 
		SklIndexList&	sklIndexList, 
		INode*			pMaxNode, 
		TimeValue		time,
		CAnimateGroup&	animGroup 
		)
	{
		Object* pObject = pMaxNode->EvalWorldState(time).obj;
		TriObject* pTriObject = (TriObject*)pObject->ConvertToType(time, triObjectClassID);
		Mesh& mesh = pTriObject->GetMesh();

		sklWeightList.resize( mesh.getNumVerts() );			
		sklIndexList.resize( mesh.getNumVerts() );

		// 以下将取得权重，填充顶点缓冲区
		Object* obj = pMaxNode->GetObjectRef();
		while( obj && obj->SuperClassID() == GEN_DERIVOB_CLASS_ID )
		{
			IDerivedObject* derObj = (IDerivedObject*)obj;
			// 从3DMAX编辑堆栈中获得最后一个PHYSIQUE编辑参数，计算权重
			for( int modStackIndex = 0; modStackIndex < derObj->NumModifiers(); modStackIndex++ )
			{
				Modifier* mod = derObj->GetModifier( modStackIndex );
				if( mod->ClassID() == Class_ID( PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B ) )
				{
					//取得权重，填充顶点缓冲区
					GetPhysiqueWeight( sklWeightList, sklIndexList, mod, animGroup );
					break;
				}
				if( mod->ClassID() == SKIN_CLASSID  )
				{
					GetSkinWeight( sklWeightList, sklIndexList, mod, animGroup );
					break;
				}
				if( mod->ClassID() == BP_CLASS_ID_OSM  )
				{
					GetBPWeight( sklWeightList, sklIndexList, mod, animGroup );
					break;
				}
			}
			if( modStackIndex < derObj->NumModifiers() )
				break;
			obj = derObj->GetObjRef();
		}

		if ( pTriObject != pObject )
		{
			pTriObject->DeleteThis();
		}
	}

	void CPluginMesh::optimizeUV( CPluginMeshLevel* pMeshLevel )
	{
		uint32 TrunkID = 0;
		vector< int32 > MeshTrunk( m_TextCoord.size(), -1 );
		for( uint32 j = 0; j < pMeshLevel->GetIndexNum(); j += 3 )
		{
			for( uint32 k = j ; k < j + 3; k++ )
			{
				if( MeshTrunk[ pMeshLevel->GetIndex(k)] >= 0 )
				{
					int32 nID = MeshTrunk[ pMeshLevel->GetIndex(k) ];
					int32 nPreID[3];
					bool  bSetOther = false;
					for( uint32 n = 0; n < 3; n++ )
					{
						nPreID[n] = MeshTrunk[pMeshLevel->GetIndex(j+n)];
						MeshTrunk[pMeshLevel->GetIndex(j+n)] = nID;
						if( nPreID[n] < 0 )
							continue;
						if( nPreID[n] != nID )
							bSetOther = true;
					}

					if( bSetOther )
					{
						for( uint32 m = 0; m < j; m++ )
						{
							for( uint32 n = 0; n < 3; n++ )
							{
								if( MeshTrunk[pMeshLevel->GetIndex(m)] == nPreID[n] )
								{
									MeshTrunk[pMeshLevel->GetIndex(m)] = nID;
									break;
								}
							}
						}
					}
					break;
				}
				else if( k == j + 2 )
				{
					MeshTrunk[pMeshLevel->GetIndex(j+0)] = TrunkID;
					MeshTrunk[pMeshLevel->GetIndex(j+1)] = TrunkID;
					MeshTrunk[pMeshLevel->GetIndex(j+2)] = TrunkID;
					TrunkID++;
				}
			}
		}

		vector< CVector2f > maxUV( TrunkID, CVector2f( -FLT_MAX, -FLT_MAX ) );
		vector< CVector2f > minUV( TrunkID, CVector2f( FLT_MAX, FLT_MAX ) );
		for( uint32 j = 0; j < m_TextCoord.size(); j++ )
		{
			uint32 uTunkID = MeshTrunk[j];
			maxUV[uTunkID].x = max( m_TextCoord[j].x, maxUV[uTunkID].x );
			minUV[uTunkID].x = min( m_TextCoord[j].x, minUV[uTunkID].x );
			maxUV[uTunkID].y = max( m_TextCoord[j].y, maxUV[uTunkID].y );
			minUV[uTunkID].y = min( m_TextCoord[j].y, minUV[uTunkID].y );
		}

		vector<POINT> strUV( TrunkID );
		for( uint32 j = 0; j < TrunkID; j++ )
		{
			if( maxUV[j].x < minUV[j].x )
				continue;

			strUV[j].x = minUV[j].x >= 0 ? (int)abs( minUV[j].x ) : -(int)abs( minUV[j].x );
			if( strUV[j].x > minUV[j].x )
				strUV[j].x--;
			strUV[j].y = minUV[j].y >= 0 ? (int)abs( minUV[j].y ) : -(int)abs( minUV[j].y );
			if( strUV[j].y > minUV[j].y )
				strUV[j].y--;
		}

		for( uint32 j = 0; j < m_TextCoord.size(); j++ )
		{
			uint32 uTunkID = MeshTrunk[j];
			m_TextCoord[j] -= CVector2f( strUV[uTunkID].x, strUV[uTunkID].y );
			m_MinUV.x = min( m_MinUV.x, m_TextCoord[j].x );
			m_MaxUV.x = max( m_MaxUV.x, m_TextCoord[j].x );
			m_MinUV.y = min( m_MinUV.y, m_TextCoord[j].y );
			m_MaxUV.y = max( m_MaxUV.y, m_TextCoord[j].y );
		}
	}

	void CPluginMesh::getMeshSmoothInfo( const Mesh& mesh, vector<BOOL>& norSmoothList )
	{
		int nTriNum = mesh.getNumFaces();
		norSmoothList.resize(nTriNum, FALSE);
		if( !g_SaveParam.m_bSP_SmoothAll )
		{
			// 双重遍历所有面,找出要平滑的点
			for( int i = 0; i < nTriNum; i++ ) 
			{
				BOOL* pSmooth = &( norSmoothList[0] ) + i*3;
				for( int j = 0; j < nTriNum; j++ )
				{
					if ( i==j )
						continue;

					if( mesh.faces[i].smGroup & mesh.faces[j].smGroup )
					{
						// 遍历面的三个顶点
						for( int m = 0; m < 3; m++ )
						{
							for( int n = 0; n < 3; n++ )
							{
								if( mesh.faces[i].v[m] == mesh.faces[j].v[n] )
								{
									pSmooth[m] = TRUE;
									break;
								}
							}
						}
					}
				}
			}
		}	
	}

	bool CPluginMesh::generateBaseInfo( CPluginMeshLevel* pMeshLevel, CAnimateGroup& animGroup )
	{
		// 将颜色、贴图坐标、顶点权重、骨骼索引一些和顶点动画帧无关的数据规类

		TimeValue startTime = g_SaveParam.m_uiSP_pInterface->GetAnimRange().Start();
		Object* obj = m_ExpNode->m_3DMaxNode->EvalWorldState( startTime ).obj;
		TriObject* triObj = (TriObject*)obj->ConvertToType( startTime, triObjectClassID );//如果是新分配的,用完没删除,那么这里在第二次调用的时候会出错
		Mesh& mesh = triObj->GetMesh();	

		// 最大非索引顶点缓冲区
		VerSkinNCTList verList;
		if ( false == generateFrameVertList(verList, startTime, animGroup) )
			return false;

		pMeshLevel->CreateIndexBuffer( mesh.getNumFaces()*3 );

		// 历便每一个面的顶点，将相同的顶点的索引合并到一个
		m_FramesVertex.resize(1);
		m_FramesNormal.resize(1);
		map<VerSkinNCT, WORD, VerSkinNCTComp> VerMap;
		for( int i = 0; i < mesh.getNumFaces(); i++ )
		{
			for( int j = 0; j < 3; j++ )
			{
				VerSkinNCT& Ver = verList[ i*3 + j ];
				map<VerSkinNCT, WORD, VerSkinNCTComp>::iterator it = VerMap.find( Ver );
				if( it == VerMap.end() )
				{
					WORD nCurVerNum = (WORD)VerMap.size();
					VerMap.insert(make_pair(Ver, nCurVerNum));

					if( GetMeshType() == VT_SKELETALANI || GetMeshType() == VT_SOFTBODYANI || GetMeshType() == VT_SOFTCLOTHANI)
					{
						CWeight sw(Ver.b1, Ver.b2, Ver.b3, 1 - Ver.b1 - Ver.b2 - Ver.b3);
						m_Weight.push_back( sw );

						SkltIndex si = { Ver.Index[0], Ver.Index[1], Ver.Index[2], Ver.Index[3] };
						m_SkeletalIndex.push_back( si );
					}

					CVector2f tc( Ver.tu, Ver.tv );
					CVector2f tc1( Ver.tu1, Ver.tv1 );
					CVector2f tc2( Ver.tu2, Ver.tv2 );

					m_FramesVertex[0].push_back(verList[ i*3 + j ].p);
					m_FramesNormal[0].push_back(verList[ i*3 + j ].np);
					m_TextCoord.push_back( tc );
					m_TextCoord1.push_back(tc1);
					m_TextCoord2.push_back(tc2);
					pMeshLevel->SetIndex(i*3 + j, nCurVerNum);
				}
				else
					pMeshLevel->SetIndex(i*3 + j, it->second);
			}
		}

		if ( triObj != obj )
		{
			triObj->DeleteThis();
		}

		return true;
	}

	bool CPluginMesh::generateVertexAniInfo( CPluginMeshLevel* pMeshLevel, CAnimateGroup& animGroup )
	{
		// 如果有顶点动画，从第一帧开始处理
		TimeValue Start = g_SaveParam.m_uiSP_pInterface->GetAnimRange().Start();
		TimeValue End   = g_SaveParam.m_uiSP_pInterface->GetAnimRange().End();
		if( VT_VERTEXANI != GetMeshType() )
			End = Start + 1;

		int Frame	  = 1;
		int FrameTime = GetTicksPerFrame()*g_SaveParam.m_uiSP_FramePerSample;
		for( TimeValue curTime = Start + FrameTime; curTime < End; curTime += FrameTime, Frame++ )
		{
			Object*		obj		= m_ExpNode->m_3DMaxNode->EvalWorldState( curTime ).obj;
			TriObject*	triObj	= (TriObject*)obj->ConvertToType( curTime, triObjectClassID );
			Mesh&		mesh	= triObj->GetMesh();	

			VerSkinNCTList verList;
			if ( false == generateFrameVertList(verList, curTime, animGroup) )
				return false;

			map<VerSkinNCT, WORD, VerSkinNCTComp> VerMap;
			for( int i = 0; i < mesh.getNumFaces(); i++ )
			{
				for( int j = 0; j < 3; j++ )
				{
					VerSkinNCT& Ver = verList[ i*3 + j ];
					map<VerSkinNCT, WORD, VerSkinNCTComp>::iterator it = VerMap.find( Ver );
					if( it == VerMap.end() )
					{
						WORD nCurVerNum = (WORD)VerMap.size();
						VerMap.insert(make_pair(Ver, nCurVerNum));

						m_FramesVertex[Frame].push_back(verList[ i*3 + j ].p);
						m_FramesNormal[Frame].push_back(verList[ i*3 + j ].np);
					}
				}
			}

			if ( triObj != obj )
			{
				triObj->DeleteThis();
			}
		}

		return true;
	}

	void CPluginMeshLevel::CreateIndexBuffer( size_t nIndexNum )
	{
		m_IndexBuf.indexStart	= 0;
		m_IndexBuf.indexCount	= nIndexNum;
		m_IndexBuf.indexBuffer = CHardwareBufferMgr::GetInst()->createIndexBuffer(nIndexNum, HBU_WRITE_ONLY );
	}

	void CPluginMeshLevel::SetIndex( size_t nIndex, uint16 vertIndex )
	{
		m_IndexBuf.indexBuffer->write( nIndex*sizeof(uint16), sizeof(uint16), &vertIndex );
	}

	uint16 CPluginMeshLevel::GetIndex( size_t nIndex )
	{
		uint16 index = 0;
		m_IndexBuf.indexBuffer->read( nIndex*sizeof(uint16), sizeof(uint16), &index );
		return index;
	}

	uint32 CPluginMeshLevel::GetIndexNum()
	{
		return m_IndexBuf.indexBuffer->getNumIndexes();
	}
}