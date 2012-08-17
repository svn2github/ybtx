#pragma once
#include "CMeshLevel.h"
#include "IDName.h"
#include "CAxisAlignedBox.h"
#include "CFilterNode.h"
#include "CVertexHardwareBuffer.h"
#include "CIndexHardwareBuffer.h"

namespace sqr
{
enum VERTEXTYPE
{
	VT_VERTEXANI	= 0,
	VT_STATICVERTEX	= 1,
	VT_SKELETALANI	= 2,
	VT_SOFTBODYANI	= 3,	//暂时还不支持
	VT_SOFTCLOTHANI = 4,	//暂时还不支持
	VT_UNKNOW=0xffffffff,
};

struct SaveInfo  : public CGraphicMallocObject
{
	SaveInfo() {}
	SaveInfo( CVector3f Scale, CVector3f Off, int uTexStr, int vTexStr, float fTextScale, VERTEXTYPE eType );
	SaveInfo( CVector3f Scale, CVector3f Off, float fTextScale, VERTEXTYPE eType );
	SaveInfo( CVector3f Scale, CVector3f Off, CVector2f MinUV, CVector2f MaxUV );
	VERTEXTYPE eVetexType;

	int xStr;
	int yStr;
	int zStr;
	int sPos;

	int uStr;
	int vStr;
	float sTex;
};

class CMeshBase  : public CGraphicMallocObject
{
public:
	typedef					GMap<uint16,CBaseAABB>		BoxMap;
	BoxMap					m_BoxMap;	
	CVector3f				m_OrgScale;					
	CVector3f				m_OrgOff;
	float					m_fTextScale;

protected:
	VERTEXTYPE				m_VertexType;
	float					m_fBaize;					//z偏移

	CVector3f				m_Scale;					//模型在自身坐标空间的大小
	CVector3f				m_Off;						//模型中心对自身坐标空间原点的偏移量
	GVector<CMeshLevel>		m_MeshLevels;				//多层LOD

	int						m_Channel;
	bool					m_IsCompress;
	VertexData				m_VertexBuf;
};

class CMesh : public CMeshBase
{				
public:
	CMesh();
	virtual ~CMesh();

	virtual int				GetFrameNum( int AniNo );					//返回顶点动画帧数
	size_t					GetVerNum();										//返回顶点数
	int						GetFaceNum( int Level )	;								//返回面数
	CVector3f				GetVertex( int Num );
	CVector2f				GetVertexUV( int Num );
	CVector3f				GetNormal( int32 nNum );

	VertexData*				GetVertexData(void);
	IndexData*				GetIndexData(float Dist);
	IndexData*				GetIndexBuf(int level);
	GVector<CMeshLevel>&		GetMeshLevel(void) { return m_MeshLevels; };
		
	virtual void			SetSkeletalMask( SKELETAL_MASK& Mask ) {}
	virtual int				GetAniNum();
	virtual int				GetAniFrameNum( int Ani );
	virtual IDNAME			GetAniName( int Ani );

	virtual VERTEXTYPE		GetType(void);

	virtual void			AddDeCompressBuf( CGraphic* pGraphic ) {};
	virtual void			DelDeCompressBuf( CGraphic* pGraphic ) {};

	VERTEXTYPE				GetVertexType();
	CVector3f&				GetScale();
	CVector3f&				GetOff();
	float					GetZBias();
	int						GetMapChannelNum();


	void					_FreeRes(void);
	void					_Reclaim(void);
	virtual	int			BinLoad( CBufFile& QuickFile ,DWORD Version, SKELETAL_MASK& mask ) =  0;
	virtual FilterNode*	Render( CRenderMatrix& ModelMatrix,FilterNode& FN );

	virtual UINT		GetRenderCommandNum(void) { return 0; };
	virtual void		CreateRenderCommand(CRenderCommand& Com, UINT Index = 0) { return; };
protected:
	virtual void _RecordData(const CVector3f& OrgScal,const CVector3f& OrgOff, float fTextScale){};
	virtual GVector<VertexSkeletalFormat>* _GetVertex(){return NULL;};
};

//返回每帧顶点数s
inline size_t CMesh::GetVerNum()
{
	return m_VertexBuf.vertexCount;
};

//------------------------------------------------------------------------------
inline VertexData*
CMesh::GetVertexData( void )
{
	return &m_VertexBuf;
}

//------------------------------------------------------------------------------
inline IndexData*
CMesh::GetIndexData( float Dist )
{
	return m_MeshLevels[0].GetIndexData();
}

//------------------------------------------------------------------------------
inline int
CMesh::GetAniNum()
{
	return 0;
}

//------------------------------------------------------------------------------
inline int
CMesh::GetAniFrameNum( int Ani )
{
	return 0;
}

//------------------------------------------------------------------------------
inline IDNAME
CMesh::GetAniName( int Ani )
{
	return IDNAME();
}

//------------------------------------------------------------------------------
inline VERTEXTYPE
CMesh::GetType( void )
{
	return VT_UNKNOW;
}

//------------------------------------------------------------------------------
inline VERTEXTYPE
CMesh::GetVertexType()
{
	return m_VertexType;    //顶点类型
}

//------------------------------------------------------------------------------
inline CVector3f&
CMesh::GetScale()
{
	return m_Scale;
}

//------------------------------------------------------------------------------
inline CVector3f&
CMesh::GetOff()
{
	return m_Off;
}

//------------------------------------------------------------------------------
inline float
CMesh::GetZBias()
{
	return m_fBaize;
}

//------------------------------------------------------------------------------
inline int
CMesh::GetMapChannelNum()
{
	return m_Channel;
}

//------------------------------------------------------------------------------
inline int
CMesh::GetFrameNum( int AniNo )
{
	return 0;
}
}
